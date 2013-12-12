//-----------------------------------------------------------------------------
//Engine
#include "Light.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	LIGHT::LIGHT() :m_Position(0, 0, 0){
		m_Diffuse[0] = 1.f;
		m_Diffuse[1] = 0.f;
		m_Diffuse[2] = 0.f;
		//--
		m_Specular[0] = 0.f;
		m_Specular[1] = 1.f;
		m_Specular[2] = 0.f;
		//--
		m_Mesh.m_pLight = this;
	}
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------