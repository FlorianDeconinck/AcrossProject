//-----------------------------------------------------------------------------
//Engine
#include "Light.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	LIGHT::LIGHT() :m_PositionOrDirection(0, 0, 0){
		m_Diffuse[0] = 1.f;
		m_Diffuse[1] = 1.f;
		m_Diffuse[2] = 1.f;
		m_DiffuseIntensity = 0.01f;
		//--
		m_Specular[0] = 1.f;
		m_Specular[1] = 1.f;
		m_Specular[2] = 1.f;
		m_SpecularIntensity = 0.01f;
		//--
		m_Mesh.m_pLight = this;
	}
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------