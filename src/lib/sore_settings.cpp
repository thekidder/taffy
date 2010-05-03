/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <cassert>
#include <algorithm>
#include <functional>
#include <sore_settings.h>
#include <sore_logger.h>
#include <sore_util.h>

namespace SORE_Utility
{
    using boost::lexical_cast;
    using boost::bad_lexical_cast;

    Datum::Datum(std::string _datum) : changed(false), datum(_datum)
    {
    }

    Datum Datum::operator=(Datum& _datum)
    {
        datum = _datum.datum;
        return *this;
    }

    bool Datum::operator==(Datum& other)
    {
        return datum==other.datum;
    }

    Datum::operator short()
    {
        try
        {
            return lexical_cast<short>(datum);
        }
        catch(bad_lexical_cast e)
        {
            return 0;
        }
    }

    Datum::operator unsigned short()
    {
        try
        {
            return lexical_cast<unsigned short>(datum);
        }
        catch(bad_lexical_cast e)
        {
            return 0;
        }
    }

    Datum::operator int()
    {
        try
        {
            return lexical_cast<int>(datum);
        }
        catch(bad_lexical_cast e)
        {
            return 0;
        }
    }

    Datum::operator unsigned int()
    {
        try
        {
            return lexical_cast<unsigned int>(datum);
        }
        catch(bad_lexical_cast e)
        {
            return 0;
        }
    }

    Datum::operator std::string()
    {
        return datum;
    }

    Datum::operator double()
    {
        try
        {
            return boost::lexical_cast<double>(datum);
        }
        catch(boost::bad_lexical_cast e)
        {
            return 0.0;
        }
    }

    Datum::operator bool()
    {
        std::transform(datum.begin(),datum.end(),datum.begin(),::tolower);
        if(datum=="true" || datum=="t") return true;
        if(datum=="false" || datum=="f") return false;
        try
        {
            return boost::lexical_cast<bool>(datum);
        }
        catch(boost::bad_lexical_cast e)
        {
            return false;
        }
    }

    Datum::operator float()
    {
        try
        {
            return boost::lexical_cast<float>(datum);
        }
        catch(boost::bad_lexical_cast e)
        {
            return 0.0f;
        }
    }

    Datum::operator unsigned char()
    {
        try
        {
            return boost::lexical_cast<unsigned char>(datum);
        }
        catch(boost::bad_lexical_cast e)
        {
            return '0';
        }
    }

    Datum::operator char()
    {
        try
        {
            return boost::lexical_cast<char>(datum);
        }
        catch(boost::bad_lexical_cast e)
        {
            return '0';
        }
    }

    ISettingsBackend::ISettingsBackend()
    {
        sm = NULL;
    }

    IniSettingsBackend::~IniSettingsBackend()
    {
    }

    void ISettingsBackend::NotifyOnChange(SettingsManager* _sm)
    {
        sm = _sm;
    }

    IniSettingsBackend::IniSettingsBackend(std::string fileName)
    {
        file = fileName;
        ParseFile();
        //SORE_FileIO::Notify(file, std::bind1st(std::mem_fun(&IniSettingsBackend::OnChange), this));
    }

    ISettingsBackend::~ISettingsBackend()
    {
    }

    void IniSettingsBackend::ParseFile()
    {
        SORE_FileIO::InFile ifile(file.c_str());
        std::map<std::string, std::map<std::string, std::string> > list =
            SORE_Utility::ParseIniFile(ifile);
        std::map<std::string, settingsList>::iterator it;
        std::map<std::string, Datum>::iterator it2;

        std::map<std::string, std::map<std::string, std::string> >::iterator i;
        std::map<std::string, std::string>::iterator i2;

        std::string oldValue;

        for(i=list.begin();i!=list.end();i++)
        {
            std::string section = i->first;
            for(i2=i->second.begin();i2!=i->second.end();i2++)
            {
                std::string name = i2->first;
                std::string value = i2->second;
                oldValue = "";
                if(data.find(section)!=data.end())
                    if(data[section].find(name)!=data[section].end())
                        oldValue = std::string(data[section][name]);
                Store(section, name, Datum(value));
                if(value!=oldValue) (data[section][name]).changed = true;
            }
        }

        if(sm)
        {
            for(it=data.begin();it!=data.end();it++)
            {
                for(it2=it->second.begin();it2!=it->second.end();it2++)
                {
                    if(it2->second.changed)
                    {
                        it2->second.changed = false;
                        sm->Changed(it->first, it2->first);
                    }
                }
            }
        }
    }

    void IniSettingsBackend::OnChange(std::string name)
    {
        ParseFile();
    }

    Datum IniSettingsBackend::Retrieve(std::string section, std::string name)
    {
        if(data.find(section)==data.end())
            return Datum("");
        if(data[section].find(name)==data[section].end())
            return Datum("");
        return data[section][name];
    }

    void IniSettingsBackend::Store(std::string section, std::string name, Datum datum)
    {
        if(data.find("section")==data.end())
            data.insert(std::pair<std::string, settingsList >(section, settingsList()));
        (data[section])[name] = datum;
    }

    SettingsManager::SettingsManager(ISettingsBackend* _sb) : sb(_sb)
    {
    }

    Datum SettingsManager::GetVariable(std::string section, std::string name)
    {
        assert(sb!=NULL && "Settings backend is null");
        return sb->Retrieve(section, name);
    }

    void SettingsManager::SetVariable(std::string section, std::string name, Datum datum)
    {
        assert(sb!=NULL && "Settings backend is null");
        sb->Store(section, name, datum);
    }

    Datum SettingsManager::WatchVariable(std::string section, std::string name, DatumCallback func, datum_watch_id& id)
    {
        id = callbacks.insert(std::pair<setting_identifier, DatumCallback>(setting_identifier(section,name), func));
        return GetVariable(section, name);
    }

    Datum SettingsManager::WatchVariable(std::string section, std::string name, DatumCallback func)
    {
        callbacks.insert(std::pair<setting_identifier, DatumCallback>(setting_identifier(section,name), func));
        return GetVariable(section, name);
    }

    void SettingsManager::RemoveWatch(datum_watch_id id)
    {
        callbacks.erase(id);
    }

    void SettingsManager::Changed(std::string section, std::string name) //notify all registered callbacks of name of a change
    {
        std::multimap<setting_identifier, DatumCallback>::iterator it;
        it = callbacks.find(setting_identifier(section,name));
        if(it == callbacks.end()) return;
        Datum var = GetVariable(section, name);
        //if(var==NULL) return;
        while(it!=callbacks.end() && it->first == setting_identifier(section,name))
        {
            it->second(var);
            it++;
        }
    }

    WatchedDatum::WatchedDatum(std::string _section, std::string _name, Datum& _datum, SettingsManager* _sm) : Datum(_datum), sm(_sm), name(_name), section(_section)
    {
        InitWatch();
    }

    WatchedDatum::WatchedDatum(std::string _section, std::string _name, std::string _datum, SettingsManager* _sm) : Datum(_datum), sm(_sm),name(_name),  section(_section)
    {
        InitWatch();
    }

    WatchedDatum::WatchedDatum(std::string _section, std::string _name, SettingsManager* _sm) : sm(_sm), name(_name), section(_section)
    {
        InitWatch();
    }

    WatchedDatum::~WatchedDatum()
    {
        if(sm)
            sm->RemoveWatch(watch);
    }

    void WatchedDatum::InitWatch()
    {
        if(sm)
        {
            std::string temp = sm->WatchVariable(section, name, std::bind1st(boost::mem_fn(&WatchedDatum::WatchFunction), this), watch);
            datum = temp;
        }
    }

    void WatchedDatum::WatchFunction(Datum _datum)
    {
        datum = (std::string)_datum;
    }

    bool operator<(setting_identifier one, setting_identifier two)
    {
        if(one.section<two.section) return true;
        if(one.section>two.section) return false;
        if(one.name<two.name) return true;
        return false;
    }

    bool operator==( setting_identifier one, setting_identifier two )
    {
        if(one.section==two.section && one.name==two.name) return true;
        return false;
    }
}
