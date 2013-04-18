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

#ifndef SORE_RESOURCEPROXY_H
#define SORE_RESOURCEPROXY_H

#include <sore_noncopyable.h>

#include <boost/shared_ptr.hpp>

#include <stdexcept>

namespace SORE_Resource
{
    template<typename T> 
    class ResourceProxy : public SORE_Utility::Noncopyable
    {
    public:
        ResourceProxy(T* resource_, boost::shared_ptr<T> proxy_ = boost::shared_ptr<T>());
        ~ResourceProxy();

        // Accessors
        T* operator->();
        const T* operator->() const;

        T* get();
        const T* get() const;

        bool Loaded() const;

        void Load(T* resource_);
        void Unload();
    private:
        T* resource;
        boost::shared_ptr<T> proxy;
    };

    // wraps a resource proxy so we can access things in it with one arrow and
    // copy it; interally uses a smart pointer
    template<typename T>
    class ResourceProxyWrapper
    {
    public:
        ResourceProxyWrapper();
        ResourceProxyWrapper(T* ptr_);
        ResourceProxyWrapper(ResourceProxy<T>* ptr_);

        T* operator->();
        const T* operator->() const;

        T& operator*();
        const T& operator*() const;

        T* get();
        const T* get() const;

        operator bool() const;

        bool Loaded() const;

        void Load(T* resource_);
        void Unload();
    private:
        boost::shared_ptr<ResourceProxy<T > > ptr;
    };

    template<typename T>
    bool operator==(const ResourceProxyWrapper<T>& one, const ResourceProxyWrapper<T>& two)
    {
        if(!one.get() && two.get())
            return false;
        if(one.get() && !two.get())
            return false;
        if(!one.get() && !two.get())
            return true;
        return *one.get() == *two.get();
    }

    template<typename T>
    bool operator!=(const ResourceProxyWrapper<T>& one, const ResourceProxyWrapper<T>& two)
    {
        return !(one == two);
    }

    template<typename T>
    bool operator<(const ResourceProxyWrapper<T>& one, const ResourceProxyWrapper<T>& two)
    {
        if(!one.get() && two.get())
            return true;
        if(one.get() && !two.get())
            return false;
        if(!one.get() && !two.get())
            return false;
        return *one.get() < *two.get();
    }

    template<typename T>
    bool operator>(const ResourceProxyWrapper<T>& one, const ResourceProxyWrapper<T>& two)
    {
        return two < one;
    }

    template<typename T>
    ResourceProxy<T>::ResourceProxy(T* resource_, boost::shared_ptr<T> proxy_)
        : resource(resource_), proxy(proxy_)
    {

    }

    template<typename T>
    ResourceProxy<T>::~ResourceProxy()
    {
        delete resource;
    }

    template<typename T>
    T* ResourceProxy<T>::operator->()
    {
        return get();
    }

    template<typename T>
    const T* ResourceProxy<T>::operator->() const
    {
        return get();
    }

    template<typename T>
    T* ResourceProxy<T>::get()
    {
        if(resource && resource->Loaded())
            return resource;
        if(proxy && proxy->Loaded())
            return proxy.get();

        return 0;
    }

    template<typename T>
    const T* ResourceProxy<T>::get() const
    {
        if(resource && resource->Loaded())
            return resource;
        if(proxy && proxy->Loaded())
            return proxy;

        return 0;
    }

    template<typename T>
    bool ResourceProxy<T>::Loaded() const
    {
        if(resource && resource->Loaded())
            return true;
        return false;
    }

    template<typename T>
    void ResourceProxy<T>::Load(T* resource_)
    {
        resource = resource_;
    }

    template<typename T>
    void ResourceProxy<T>::Unload()
    {
        delete resource;
        resource = 0;
    }

    // WRAPPER

    template<typename T>
    ResourceProxyWrapper<T>::ResourceProxyWrapper()
        : ptr(new ResourceProxy<T>(0))
    {
    }

    template<typename T>
    ResourceProxyWrapper<T>::ResourceProxyWrapper(T* ptr_)
        : ptr(new ResourceProxy<T>(ptr_))
    {
    }

    template<typename T>
    ResourceProxyWrapper<T>::ResourceProxyWrapper(ResourceProxy<T>* ptr_)
        : ptr(ptr_)
    {
    }

    template<typename T>
    T* ResourceProxyWrapper<T>::operator->()
    {
#ifdef DEBUG
        if(!*this)
            throw std::runtime_error("Called -> on null resource");
#endif
        return get();
    }

    template<typename T>
    const T* ResourceProxyWrapper<T>::operator->() const
    {
#ifdef DEBUG
        if(!*this)
            throw std::runtime_error("Called -> on null resource");
#endif
        return get();
    }

    template<typename T>
    T& ResourceProxyWrapper<T>::operator*()
    {
#ifdef DEBUG
        if(!*this)
            throw std::runtime_error("Called * on null resource");
#endif
        return *(ptr.get()->get());
    }

    template<typename T>
    const T& ResourceProxyWrapper<T>::operator*() const
    {
#ifdef DEBUG
        if(!*this)
            throw std::runtime_error("Called * on null resource");
#endif
        return *(ptr.get()->get());
    }
    
    template<typename T>
    T* ResourceProxyWrapper<T>::get()
    {
        return ptr.get()->get();
    }

    template<typename T>
    const T* ResourceProxyWrapper<T>::get() const
    {
        return ptr.get()->get();
    }

    template<typename T>
    ResourceProxyWrapper<T>::operator bool() const
    {
        return get() != 0;
    }

    template<typename T>
    bool ResourceProxyWrapper<T>::Loaded() const
    {
        return ptr->Loaded();
    }

    template<typename T>
    void ResourceProxyWrapper<T>::Load(T* resource_)
    {
        ptr->Load(resource_);
    }

    template<typename T>
    void ResourceProxyWrapper<T>::Unload()
    {
        ptr->Unload();
    }
}

#endif