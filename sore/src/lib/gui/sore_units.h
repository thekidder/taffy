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


#ifndef SORE_UNITS_H
#define SORE_UNITS_H

#include <sore_dll.h>

//inspired by the CEGUI system of "unified size and positioning"

namespace SORE_GUI
{
	class SORE_EXPORT SUnit
	{
	public:
		SUnit(double rel, int abs);
        SUnit(double rel);
        SUnit(int abs);
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
        SVec();

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
