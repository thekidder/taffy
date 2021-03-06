/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#ifndef SORE_RESOURCECACHE_H
#define SORE_RESOURCECACHE_H

#include <sore_logger.h>
#include <sore_resourceproxy.h>

#include <boost/unordered_map.hpp>

namespace SORE_Resource
{
    template<typename Key, typename Asset, typename ResourceLoader>
    class ResourceCache
    {
    public:
        typedef ResourceProxyWrapper<Asset> Resource_t;

        ResourceCache(const ResourceLoader& loader_);

        // retrieve the asset pointed to by key
        // if the asset does not exist in cache, calls the loader to retrieve it
        Resource_t Get(const Key& key);

        // This is to support asset types that rely on non-unique instances
        // For example, Materials are resources and can be loaded and cached, but should be
        // copied for most renderables, as they have a lot of unique state (uniforms, etc)
        // This way we get the benefits of the resourcecache class (customizable loaders,
        // only read the file once) while allowing non-unique instances

        // Returns a copy of the cached resource given by key. Equivalent to calling Get() and
        // then invoking the copy constructor. TODO: Unload/Reload work on Cloned resources
        Resource_t Clone(const Key& key);

        // Unloads the given resource. Does not actually remove it from cache; only
        // unloads it from the proxy.
        void Unload(const Key& key);
        // Ensure the given resource is loaded
        void Load(const Key& key);
        void Reload(const Key& key);

        // Query the container:
        // Return if the container contains a resource with the given key in the cache.
        // This does not mean the resource exists - only that a proxy with that name is in the cache
        bool Contains(const Key& key) const;
        // Check if the container contains a resource with the given key, and if that resource is loaded
        bool Loaded(const Key& key) const;
    private:
        // We want to gracefully handle resource loading errors - here we
        // simply catch any exceptions thrown during the loading process
        // and use the proxy, if loaded. If there is no proxy, we are still
        // in trouble
        Asset* LoadHelper(const Key& key);

        ResourceLoader loader;

        typedef boost::unordered_map<Key, Resource_t> Asset_container_t;
        Asset_container_t map;

        boost::shared_ptr<Asset> proxyObject;
    };

    template<typename Key, typename Asset, typename ResourceLoader>
    ResourceCache<Key,Asset,ResourceLoader>::ResourceCache(const ResourceLoader& loader_)
        : loader(loader_)
    {
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    void ResourceCache<Key,Asset,ResourceLoader>::Unload(const Key& key)
    {
        typename Asset_container_t::iterator it = map.find(key);
        if(it != map.end())
        {
            it->second.Unload();
        }
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    typename ResourceCache<Key,Asset,ResourceLoader>::Resource_t
    ResourceCache<Key,Asset,ResourceLoader>::Get(const Key& key)
    {
        // make sure proxy is loaded
        if(!proxyObject)
            proxyObject = boost::shared_ptr<Asset>(loader.LoadProxy());

        typename Asset_container_t::iterator it = map.find(key);
        if(it == map.end())
        {
            // load the resource
            Asset* rawResource = LoadHelper(key);
            Resource_t resource(new ResourceProxy<Asset>(rawResource, proxyObject));
            map.insert(std::make_pair(key, resource));
            return resource;
        }
        else
        {
            return it->second;
        }
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    typename ResourceCache<Key,Asset,ResourceLoader>::Resource_t
    ResourceCache<Key,Asset,ResourceLoader>::Clone(const Key& key)
    {
        Resource_t original = Get(key);

        Asset* asset;
        if(original.Loaded())
            // copy the resource if it exists
            asset = new Asset(*original.get());
        else
            asset = 0;

        // create a new wrapper and proxy
        Resource_t clone(new ResourceProxy<Asset>(asset, proxyObject));

        return clone;
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    void ResourceCache<Key,Asset,ResourceLoader>::Load(const Key& key)
    {
        typename Asset_container_t::iterator it = map.find(key);
        if(it != map.end())
        {
            // in the cache, but maybe not loaded
            if(!it->second.Loaded())
            {
                it->second.Load(LoadHelper(key));
            }
        }
        else
        {
            // not in our cache, calling Get() will ensure it's loaded and put in the cache
            Get(key);
        }
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    void ResourceCache<Key,Asset,ResourceLoader>::Reload(const Key& key)
    {
        Unload(key);
        Load(key);
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    bool ResourceCache<Key,Asset,ResourceLoader>::Contains(const Key& key) const
    {
        return map.find(key) != map.end();
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    bool ResourceCache<Key,Asset,ResourceLoader>::Loaded(const Key& key) const
    {
        typename Asset_container_t::const_iterator it = map.find(key);
        return it != map.end() && it->second.Loaded();
    }

    template<typename Key, typename Asset, typename ResourceLoader>
    Asset* ResourceCache<Key,Asset,ResourceLoader>::LoadHelper(const Key& key)
    {
        try
        {
            return loader.Load(key);
        }
        catch(...)
        {
            // assumes key is convertible to string - works for now; all keys
            // we are using are paths
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not load " + key + "; falling back to proxy if it exists");
            return 0;
        }
    }
}

#endif
