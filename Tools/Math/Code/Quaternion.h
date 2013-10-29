//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//STD
#include <math.h>
//---------------------------------------------------------------------------
namespace AT{
	//---------------------------------------------------------------------------
	template <typename unit> struct QUAT{
		//----
		unit i, j, k, w;
		//----
								QUAT()													{	Zero(); }
								QUAT(unit _i, unit _j, unit _k, unit _w)				{	i=_i, j=_j, k=_k, w=_w; }
		inline void				Zero()													{	i=j=k=0; w=1; }
		inline void				Normalize()												{	unit n = 1.0f/sqrt(i*i+j*j+k*k+w*w); i*=n; j*=n; k*=n; w*=n; }
		inline void				FromAngle(unit angle, unit x, unit y, unit z)			{
																							w = cos(angle/(unit)2);
																							unit s = sin(angle/(unit)2);
																							i = x*s;
																							j = y*s;
																							k = z*s;
																						}
		inline QUAT<unit>		Conjugate()										const	{	return QUAT<unit>(-i, -j, -k, w); }
		inline QUAT<unit>		HamiltonProduct(const QUAT<unit>& Q)			const	{
																							QUAT<unit> Qr;
																							Qr.w = i*Q.i - j*Q.j - k*Q.k - w*Q.w;
																							Qr.i = i*Q.j + j*Q.i + k*Q.w - w*Q.k;
																							Qr.j = i*Q.k - j*Q.w + k*Q.i + w*Q.j;
																							Qr.k = i*Q.w + j*Q.k - k*Q.j + w*Q.i;
																							return Qr;
																						}
		inline AT::VEC3D<unit>	RotateVector(const AT::VEC3D<unit>& V)			const 	{
																							//QUAT<unit> Qr = (Conjugate().HamiltonProduct(QUAT<unit>(V.x, V.y, V.z, 0))).HamiltonProduct(*this);
																							QUAT<unit> Qr = HamiltonProduct(QUAT<unit>(V.x, V.y, V.z, 0)).HamiltonProduct(Conjugate()); //QVQ* = Hamilton(Hamilton(Q, V), Q*)
																							return AT::VEC3D<unit>(Qr.i, Qr.j, Qr.k);
																						}
		//---------------------------------------------------------------------------
		static AT::VEC3D<unit> RotateAngleAxisHamilton(const VEC3D<unit>& V, unit angle_in_rad, unit axisX, unit axisY, unit axisZ){
			unit halfAngle = angle_in_rad/2;
			unit sinHalfAngle = sin(halfAngle);
			QUAT<unit> Q(sinHalfAngle*axisX, sinHalfAngle*axisY, sinHalfAngle*axisZ, cos(halfAngle));
			return Q.RotateVector(V);
		}
		//---------------------------------------------------------------------------
		//Derivation found on http://mollyrocket.com/forums/viewtopic.php?t=833&sid=3a84e00a70ccb046cfc87ac39881a3d0 
		static AT::VEC3D<unit> RotateAngleAxis(const VEC3D<unit>& V, unit angle_in_rad, unit axisX, unit axisY, unit axisZ){
			unit halfAngle = angle_in_rad/2;
			unit sinHalfAngle = sin(halfAngle);
			QUAT<unit> Q(sinHalfAngle*axisX, sinHalfAngle*axisY, sinHalfAngle*axisZ, cos(halfAngle));
			VEC3D<unit> Qv(Q.i,Q.j,Q.k);
			AT::VEC3D<unit> T = Qv.Cross(V)*2;
			return V + T*Q.w + Qv.Cross(T);
		}
	};
	//---------------------------------------------------------------------------
}//namespace AT
//---------------------------------------------------------------------------