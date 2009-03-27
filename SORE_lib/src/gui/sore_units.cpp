//$Id$

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
