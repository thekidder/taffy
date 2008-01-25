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

#ifndef  __SORE_SETTINGS_H__
#define  __SORE_SETTINGS_H__

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include "sore_fileio.h"

namespace SORE_Utility
{
	class Datum
	{
		public:
			Datum(std::string _datum = "");
			//Datum(Datum& _datum);
			operator int();
			operator std::string();
			operator double();
			operator float();
			operator char();
			operator bool();
			bool operator==(Datum& other);
			Datum operator=(Datum& _datum);
			bool changed;
		protected:
			std::string datum;
	};
	
	class SettingsManager;
		
	class ISettingsBackend
	{
		public:
			ISettingsBackend();
			virtual Datum Retrieve(std::string name)=0;
			virtual void       Store   (std::string name, Datum datum)=0;
			void               NotifyOnChange(SettingsManager* _sm);
		protected:
			SettingsManager* sm;
	};
	
	class IniSettingsBackend : public ISettingsBackend
	{
		public:
			IniSettingsBackend(std::string fileName);
			Datum Retrieve(std::string name);
			void    Store(std::string name, Datum datum);
			void    OnChange(std::string name);
		protected:
			void ParseFile();
			std::string file;
			std::map<std::string, Datum> data;
	};
	
	typedef boost::function<void (Datum)> DatumCallback;
	typedef std::multimap<std::string, DatumCallback >::iterator datum_watch_id;
	
	class SettingsManager
	{
		public:
			SettingsManager(ISettingsBackend* _sb);
			
			Datum GetVariable(std::string name);
			void       SetVariable(std::string name, Datum var);
			
			Datum WatchVariable(std::string name, DatumCallback func, datum_watch_id& id);
			Datum WatchVariable(std::string name, DatumCallback func);
			void  RemoveWatch(datum_watch_id id);
			
			void Changed(std::string name); //notify all registered callbacks of name of a change
			
		protected:
			std::multimap<std::string, DatumCallback > callbacks;
			ISettingsBackend* sb;
	};
	
	class WatchedDatum : public Datum
	{
		public:
			WatchedDatum(std::string _name, Datum& _datum, SettingsManager* _sm);
			WatchedDatum(std::string _name, std::string _datum, SettingsManager* _sm);
			WatchedDatum(std::string _name, SettingsManager* _sm);
			~WatchedDatum();
		protected:
			void InitWatch();
			void WatchFunction(Datum _datum);
			SettingsManager* sm;
			std::string name;
			datum_watch_id watch; 
	};
};

#endif /*__SORE_SETTINGS_H__*/
