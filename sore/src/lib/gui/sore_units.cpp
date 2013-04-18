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


#include <sore_units.h>

namespace SORE_GUI
{
	SUnit::SUnit(double rel, int abs) : relative(rel), absolute(abs)
	{
	}

    SUnit::SUnit(double rel) : relative(rel), absolute(0)
	{
	}

    SUnit::SUnit(int abs) : relative(0.0), absolute(abs)
	{
	}

	SUnit::SUnit() : relative(0.0), absolute(0)
	{
    }

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

    SVec::SVec()
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
