//$Id$

#ifndef SORE_UNITS_H
#define SORE_UNITS_H

//inspired by the CEGUI system of "unified size and positioning"

namespace SORE_GUI
{
	class SUnit
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

	class SVec //two-dimensional GUIUnit
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
