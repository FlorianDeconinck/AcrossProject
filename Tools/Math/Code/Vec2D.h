//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tools
#include <CommonTypes.h>
//---------------------------------------------------------------------------
namespace AT{
	//-----------------------------------------------------------------------
	#define AT_EPSILON 1e-5
	template <typename unit> struct VEC2D{
		unit x, y;
		//--
						VEC2D()								{ Zero(); }
						VEC2D(unit _x, unit _y):x(_x),y(_y)	{}
						VEC2D(const VEC2D<unit>& V)			{ x = V.x; y = V.y; }
		void			Set(unit _x, unit _y)				{ x = _x; y = _y;}
		//--
		void			Zero()									{ x=y=0; }
		I8				IsZero()						const	{ return abs(x)<AT_EPSILON && abs(y)<AT_EPSILON; }
		void			Normalize()								{ unit n = sqrt(x*x + y*y); x/=n ; y/=n; }
		unit			Cross(const VEC2D<unit>& V)		const	{ return this->x*V.y - this->y*V.x; }
		VEC2D<unit>		operator-(const VEC2D<unit>& V)	const	{ VEC2D<unit> Vr; Vr.x = V.x - this->x; Vr.y = V.y - this->y; return Vr; }
		VEC2D<unit>		operator+(const VEC2D<unit>& V)	const 	{ VEC2D<unit> Vr; Vr.x = V.x + this->x; Vr.y = V.y + this->y; return Vr; }
		VEC2D<unit>		operator*(const unit& s)		const	{ VEC2D<unit> Vr; Vr.x = s*this->x; Vr.y = s*this->y; return Vr; }
		VEC2D<unit>		operator+=(const VEC2D<unit>& V)		{ x += V.x; y += V.y; return *this; }
		VEC2D<unit>		operator/(const unit& s)		const	{ VEC2D<unit> Vr; Vr.x = this->x/s ; Vr.y = this->y/s; return Vr; }
		I8						operator==(const VEC2D<unit>& V)const	{ return x == V.x && y == V.y; }
		I8						operator!=(const VEC2D<unit>& V)const	{ return x != V.x || y != V.y; }
		VEC2D<unit>		operator=(const VEC2D<unit>& V)			{ x=V.x ; y=V.y ; return *this; }
	};
	//-----------------------------------------------------------------------
	typedef VEC2D<AT::I32>	VEC2Di;
	typedef VEC2D<AT::I32F>	VEC2Df;
	typedef VEC2D<AT::I64F>	VEC2Dd;
	//-----------------------------------------------------------------------
}//namespace AT
//---------------------------------------------------------------------------