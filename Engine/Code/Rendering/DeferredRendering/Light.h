//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Tools
#include <AcrossTypes.h>
#include <Vec3D.h>
//Engine
#include "../RObject.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	class R_OBJECT;
	//-------------------------------------------------------------------------
	class LIGHT{
	public:
		LIGHT();
		inline void SetColor(GLfloat Diffuse[3], GLfloat Specular[3]) { 
			m_Diffuse[0] = Diffuse[0];
			m_Diffuse[1] = Diffuse[1];
			m_Diffuse[2] = Diffuse[2];
			//--
			m_Specular[0] = Specular[0];
			m_Specular[1] = Specular[1];
			m_Specular[2] = Specular[2];
		}
		GLfloat		m_Diffuse[3];
		GLfloat		m_DiffuseIntensity;
		GLfloat		m_Specular[3];
		GLfloat		m_SpecularIntensity;
		AT::VEC3Df	m_PositionOrDirection;
		R_OBJECT	m_Mesh;
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
