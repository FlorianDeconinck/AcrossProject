#pragma once
//---------------------------------------------------------------------------
namespace AT{
	//---------------------------------------------------------------------------
	template <typename unit> struct VEC3D{
		unit x, y, z;
		//--
									VEC3D()																				{ Zero(); }
									VEC3D(unit _x, unit _y, unit _z):x(_x),y(_y),z(_z){}
		//--
		void					Zero()							{ x=y=z=0; }
		void					Normalize()					{ unit n = sqrt(x*x + y*y + z*z); x/=n ; y/=n; z/=n; }
		VEC3D<unit>				Cross(VEC3D<unit> V){ VEC3D<unit> Vr; Vr.x = this->y*V.z - this->z*V.y;  Vr.y = this->z*V.x - this->x*V.z; Vr.z = this->x*V.y - this->y*V.x; return Vr; }
		VEC3D<unit>				operator-(VEC3D& V)	{ VEC3D<unit> Vr; Vr.x = V.x - this->x; Vr.y = V.y - this->y; Vr.z = V.z - this->z; return Vr; }
		VEC3D<unit>				operator+(VEC3D& V)	{ VEC3D<unit> Vr; Vr.x = V.x + this->x; Vr.y = V.y + this->y; Vr.z = V.z + this->z; return Vr; }
		VEC3D<unit>				operator*(unit& s)	{ VEC3D<unit> Vr; Vr.x = s*this->x; Vr.y = s*this->y; Vr.z = s*this->z; return Vr; }
		VEC3D<unit>				operator*(unit s)	{ VEC3D<unit> Vr; Vr.x = s*this->x; Vr.y = s*this->y; Vr.z = s*this->z; return Vr; }
	};
	//---------------------------------------------------------------------------
	typedef VEC3D<float> VEC3Df;
	typedef VEC3D<double> VEC3Dd;
	//-------------------------------------------------------------------------
}//namespace AT
//-------------------------------------------------------------------------
