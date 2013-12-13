#pragma once
//-----------------------------------------------------------------------------
//Project
#include "Vec3D.h"
#include "Quaternion.h"
//STD
#include <memory.h>
//---------------------------------------------------------------------------
namespace AT{
	//-----------------------------------------------------------------------------
	template <typename unit> struct TRF{
	public :
							TRF();
		void				BuildGLMat();
		unit*				ToGL();
		void				Identity();
		void				SetT(unit x, unit y, unit z);
		void				SetT(VEC3Df p);
		VEC3D<unit>			GetT() const;
		VEC3D<unit>			operator=(const VEC3D<unit>& T);
		//---
		unit				m_GLMat[4][4];
		bool				m_bDirty;
	private :
		VEC3D<unit>			m_T;
		QUAT<unit>			m_Q;
	};
	typedef TRF<float> TRFf;
	typedef TRF<double> TRFd;
	//-----------------------------------------------------------------------------
	template <typename unit> TRF<unit>::TRF(){
		memset(m_GLMat, 0, 16*sizeof(unit));
		m_bDirty=true;
	}
	//-----------------------------------------------------------------------------
	template <typename unit> void TRF<unit>::BuildGLMat(){
		m_Q.Normalize();
		//--
		//--
		//Col 1
		m_GLMat[0][0] = 1.0f - 2.0f*m_Q.j*m_Q.j - 2.0f*m_Q.k*m_Q.k; 
		m_GLMat[0][1] = 2.0f*m_Q.i*m_Q.j - 2.0f*m_Q.k*m_Q.w;
		m_GLMat[0][2] = 2.0f*m_Q.i*m_Q.k + 2.0f*m_Q.j*m_Q.w; 
		m_GLMat[0][3] = 0.f;
		//--
		//Col 2
		m_GLMat[1][0] = 2.0f*m_Q.i*m_Q.j + 2.0f*m_Q.k*m_Q.w;
		m_GLMat[1][1] = 1.0f - 2.0f*m_Q.i*m_Q.i - 2.0f*m_Q.k*m_Q.k; 
		m_GLMat[1][2] = 2.0f*m_Q.j*m_Q.k - 2.0f*m_Q.i*m_Q.w; 
		m_GLMat[1][3] = 0.f;
		//--
		//Col 3
		m_GLMat[2][0] = 2.0f*m_Q.i*m_Q.k - 2.0f*m_Q.j*m_Q.w;
		m_GLMat[2][1] = 2.0f*m_Q.j*m_Q.k + 2.0f*m_Q.i*m_Q.w;
		m_GLMat[2][2] = 1.0f - 2.0f*m_Q.i*m_Q.i - 2.0f*m_Q.j*m_Q.j;
		m_GLMat[2][3] = 0.f;
		//---
		//Col 4
		m_GLMat[3][0] =	m_T.x;
		m_GLMat[3][1] =	m_T.y;
		m_GLMat[3][2] =	m_T.z;
		m_GLMat[3][3] =	1.0f;
		//---
		m_bDirty = false;
	}
	//---------------------------------------------------------------------------
	template <typename unit> unit* TRF<unit>::ToGL(){
		if(m_bDirty)
			BuildGLMat();
		return (unit*)m_GLMat;
	}
	//---------------------------------------------------------------------------
	template <typename unit> void TRF<unit>::Identity(){
		m_Q.Zero();
		m_T.Zero();
		BuildGLMat();
	}
	//---------------------------------------------------------------------------
	template <typename unit> void TRF<unit>::SetT(unit x, unit y, unit z){
		m_T.x = x;
		m_T.y = y;
		m_T.z = z;
		m_bDirty=true;
	}
	//---------------------------------------------------------------------------
	template <typename unit> void TRF<unit>::SetT(VEC3Df p){
		m_T = p;
		m_bDirty = true;
	}
	//---------------------------------------------------------------------------
	template <typename unit> VEC3D<unit> TRF<unit>::GetT() const{
		return m_T;
	}
	//---------------------------------------------------------------------------
	template <typename unit> VEC3D<unit> TRF<unit>::operator=(const VEC3D<unit>& T) {
		m_T.x = T.x;
		m_T.y = T.y;
		m_T.z = T.z;
		m_bDirty=true;
		return m_T;
	}
	//---------------------------------------------------------------------------
}//namespace AT
//---------------------------------------------------------------------------
