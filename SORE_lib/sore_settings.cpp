/***************************************************************************
 *   Copyright (C) 2007 by Adam Kidder   *
 *   thekidder@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$ 

#include <cassert>
#include <functional>
#include "sore_settings.h"
#include "sore_logger.h"

namespace SORE_Utility
{
	using boost::lexical_cast;
	using boost::bad_lexical_cast;

	Datum::Datum(std::string _datum) : datum(_datum), changed(false)
	{
	}
	
	//Datum::Datum(Datum& _datum) :datum(_datum)
	//{
	//}
	
	Datum Datum::operator=(Datum& _datum)
	{
		datum = _datum.datum;
		return *this;
	}
	
	bool Datum::operator==(Datum& other)
	{
		return datum==other.datum;
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
		if(datum=="true") return true;
		if(datum=="false") return false;
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
	
	void ISettingsBackend::NotifyOnChange(SettingsManager* _sm)
	{
		sm = _sm;
	}
	
	std::string Trim(std::string toTrim)
	{
		std::string trimmed = toTrim;
		size_t pos;
		while((pos=trimmed.find(' '))!=trimmed.npos)
		{
			trimmed.erase(pos,1);
		}
		while((pos=trimmed.find('\r'))!=trimmed.npos) //windows files
		{
			trimmed.erase(pos,1);
		}
		return trimmed;
	}

	IniSettingsBackend::IniSettingsBackend(std::string fileName)
	{
		file = fileName;
		ParseFile();
		SORE_FileIO::Notify(file, std::bind1st(std::mem_fun(&IniSettingsBackend::OnChange), this));
	}
	
	void IniSettingsBackend::ParseFile()
	{
		SORE_FileIO::file_ref settingsFile = SORE_FileIO::Open(file.c_str());
		int len;
		if(settingsFile == 0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Could not load settings file %s") % file.c_str());
		}
		else
		{
			char dataStr[64];
			unsigned int size = SORE_FileIO::Size(settingsFile);
			len = SORE_FileIO::Read(dataStr, 63, "\n", settingsFile);
			
			while(len>0)
			{
				std::string name, value, oldValue;
				std::string setting = dataStr;
				int eqPos=setting.find('=');
				if(eqPos!=-1)
				{
					name=setting.substr(0,eqPos);
					value=setting.substr(eqPos+1);
					name = Trim(name);
					value = Trim(value);
					oldValue = (std::string)Retrieve(name);
					ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Parsed setting: '%s:%s'") % name % value);
					Store(name, Datum(value));
					if(value!=oldValue) data[name].changed = true;
				}
				len = SORE_FileIO::Read(dataStr, 63, "\n", settingsFile);
			}
			std::map<std::string, Datum>::iterator it;
			for(it=data.begin();it!=data.end();it++)
			{
				if(sm && it->second.changed)
				{
					it->second.changed = false;
					sm->Changed(it->first);
				}
			}
			SORE_FileIO::Close(settingsFile);
		}
	}
	
	void IniSettingsBackend::OnChange(std::string name)
	{
		ParseFile();
	}
	
	Datum IniSettingsBackend::Retrieve(std::string name)
	{
		if(data.find(name)==data.end())
			return Datum("");
		return data[name];
	}
	
	void IniSettingsBackend::Store(std::string name, Datum datum)
	{
		data[name] = datum;
	}
	
	SettingsManager::SettingsManager(ISettingsBackend* _sb) : sb(_sb)
	{
	}
	
	Datum SettingsManager::GetVariable(std::string name)
	{
		assert(sb!=NULL && "Settings backend is null");
		return sb->Retrieve(name);
	}
	
	void SettingsManager::SetVariable(std::string name, Datum datum)
	{
		assert(sb!=NULL && "Settings backend is null");
		sb->Store(name, datum);
	}
	
	Datum SettingsManager::WatchVariable(std::string name, DatumCallback func, datum_watch_id& id)
	{
		id = callbacks.insert(std::pair<std::string, DatumCallback>(name, func));
		return GetVariable(name);
	}
	
	Datum SettingsManager::WatchVariable(std::string name, DatumCallback func)
	{
		callbacks.insert(std::pair<std::string, DatumCallback>(name, func));
		return GetVariable(name);
	}
	
	void SettingsManager::RemoveWatch(datum_watch_id id)
	{
		callbacks.erase(id);
	}
			
	void SettingsManager::Changed(std::string name) //notify all registered callbacks of name of a change
	{
		std::multimap<std::string, DatumCallback>::iterator it;
		it = callbacks.find(name);
		if(it == callbacks.end()) return;
		Datum var = GetVariable(name);
		//if(var==NULL) return;
		while(it!=callbacks.end() && it->first == name)
		{
			it->second(var);
			it++;
		}
	}
	
	WatchedDatum::WatchedDatum(std::string _name, Datum& _datum, SettingsManager* _sm) : sm(_sm), Datum(_datum), name(_name)
	{
		InitWatch();
	}
	
	WatchedDatum::WatchedDatum(std::string _name, std::string _datum, SettingsManager* _sm) : sm(_sm), Datum(_datum), name(_name)
	{
		InitWatch();
	}
	
	WatchedDatum::WatchedDatum(std::string _name, SettingsManager* _sm) : sm(_sm), name(_name)
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
			std::string temp = sm->WatchVariable(name, std::bind1st(boost::mem_fn(&WatchedDatum::WatchFunction), this), watch);
			datum = temp;
		}
	}
	
	void WatchedDatum::WatchFunction(Datum _datum)
	{
		datum = (std::string)_datum;
	}
}
