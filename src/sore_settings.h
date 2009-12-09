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

#ifndef  SORE_SETTINGS_H
#define  SORE_SETTINGS_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

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
    class SORE_EXPORT Datum
    {
    public:
        Datum(std::string _datum = "");
        //Datum(Datum& _datum);
        operator int();
        operator short();
        operator unsigned int();
        operator unsigned short();
        operator std::string();
        operator double();
        operator float();
        operator char();
        operator unsigned char();
        operator bool();
        bool operator==(Datum& other);
        Datum operator=(Datum& _datum);
        bool changed;
    protected:
        std::string datum;
    };

    //typedefs and structs to make for less typing
    struct setting_identifier
    {
        setting_identifier(std::string s, std::string n) {section = s; name = n; }
        std::string section;
        std::string name;
    };

    bool operator< ( setting_identifier one, setting_identifier two );
    bool operator==( setting_identifier one, setting_identifier two );

    typedef boost::function<void (Datum)> DatumCallback;
    typedef std::map<std::string, Datum> settingsList;
    typedef std::multimap<setting_identifier, DatumCallback >::iterator datum_watch_id;

    class SORE_EXPORT SettingsManager;

    class SORE_EXPORT ISettingsBackend
    {
    public:
        ISettingsBackend();
        virtual ~ISettingsBackend() = 0;
        virtual Datum Retrieve(std::string section, std::string name) = 0;
        virtual void Store(std::string section, std::string name, Datum datum) = 0;
        void NotifyOnChange(SettingsManager* _sm);
    protected:
        SettingsManager* sm;
    };

    class SORE_EXPORT IniSettingsBackend : public ISettingsBackend
    {
    public:
        IniSettingsBackend(std::string fileName);
        virtual ~IniSettingsBackend();
        Datum Retrieve(std::string section, std::string name);
        void    Store(std::string section, std::string name, Datum datum);
        void    OnChange(std::string name);
    protected:
        void ParseFile();
        std::string file;
        std::map<std::string, settingsList > data;
    };

    class SORE_EXPORT SettingsManager
    {
    public:
        SettingsManager(ISettingsBackend* _sb);

        Datum GetVariable(std::string section, std::string name);
        void  SetVariable(std::string section, std::string name, Datum var);

        Datum WatchVariable(std::string section, std::string name,
                            DatumCallback func, datum_watch_id& id);
        Datum WatchVariable(std::string section, std::string name, DatumCallback func);
        void  RemoveWatch(datum_watch_id id);

        //notify all registered callbacks of name of a change
        void Changed(std::string section, std::string name);
    private:
        std::multimap<setting_identifier, DatumCallback> callbacks;
        ISettingsBackend* sb;
    };

    class SORE_EXPORT WatchedDatum : public Datum
    {
    public:
        WatchedDatum(std::string _section, std::string _name, Datum& _datum,
                     SettingsManager* _sm);
        WatchedDatum(std::string _section, std::string _name, std::string _datum,
                     SettingsManager* _sm);
        WatchedDatum(std::string _section, std::string _name, SettingsManager* _sm);
        ~WatchedDatum();
    protected:
        void InitWatch();
        void WatchFunction(Datum _datum);
        SettingsManager* sm;
        std::string name, section;
        datum_watch_id watch;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
