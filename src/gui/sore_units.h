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


#ifndef SORE_UNITS_H
#define SORE_UNITS_H

#include "../sore_dll.h"

//inspired by the CEGUI system of "unified size and positioning"

namespace SORE_GUI
{
	class SORE_EXPORT SUnit
	{
	public:
		SUnit(double rel, int abs);
		SUnit();
		double GetRelative() const;
		int GetAbsolute() const;

		SUnit& operator+=(const SUnit& s);
		SUnit& operator-=(const SUnit& s);
	protected:
		double relative;
		int absolute;
	};

	SUnit operator+(const SUnit& lhs, const SUnit& rhs);
	SUnit operator-(const SUnit& lhs, const SUnit& rhs);

	class SORE_EXPORT SVec //two-dimensional GUIUnit
	{
	public:
		SVec(SUnit h, SUnit v);

		SUnit GetHorizontal() const;
		SUnit GetVertical() const;

		SVec& operator+=(const SVec& s);
		SVec& operator-=(const SVec& s);
	protected:
		SUnit horz;
		SUnit vert;
	};

	SVec operator+(const SVec& lhs, const SVec& rhs);
	SVec operator-(const SVec& lhs, const SVec& rhs);
}

#endif
