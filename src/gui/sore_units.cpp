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


#include "sore_units.h"

namespace SORE_GUI
{
	SUnit::SUnit(double rel, int abs) : relative(rel), absolute(abs)
	{
	}

	SUnit::SUnit() : relative(0.0), absolute(0)
	{}

	double SUnit::GetRelative() const
	{
		return relative;
	}

	int SUnit::GetAbsolute() const
	{
		return absolute;
	}

	SUnit& SUnit::operator+=(const SUnit& s)
	{
		relative += s.relative;
		absolute += s.absolute;

		return *this;
	}

	SUnit& SUnit::operator-=(const SUnit& s)
	{
		relative -= s.relative;
		absolute -= s.absolute;

		return *this;
	}

	SUnit operator+(const SUnit& lhs, const SUnit& rhs)
	{
		SUnit temp = lhs;
		return temp += rhs;
	}

	SUnit operator-(const SUnit& lhs, const SUnit& rhs)
	{
		SUnit temp = lhs;
		return temp -= rhs;
	}

	SVec::SVec(SUnit h, SUnit v) : horz(h), vert(v)
	{
	}

	SUnit SVec::GetHorizontal() const
	{
		return horz;
	}

	SUnit SVec::GetVertical() const
	{
		return vert;
	}

	SVec& SVec::operator+=(const SVec& s)
	{
		horz += s.horz;
		vert += s.vert;

		return *this;
	}

	SVec& SVec::operator-=(const SVec& s)
	{
		horz -= s.horz;
		vert -= s.vert;

		return *this;
	}

	SVec operator+(const SVec& lhs, const SVec& rhs)
	{
		SVec temp = lhs;
		return temp += rhs;
	}

	SVec operator-(const SVec& lhs, const SVec& rhs)
	{
		SVec temp = lhs;
		return temp -= rhs;
	}
}
