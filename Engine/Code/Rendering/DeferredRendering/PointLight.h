//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include"Light.h"
//STD
#include <vector>
//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	class RENDERER_ABC;
	class SHADER_ABC;
	class POINT_LIGHT:public LIGHT{
	public:
		POINT_LIGHT();
		void BuildLight(RENDERER_ABC& Renderer, GLfloat	Diffuse[3], GLfloat Specular[3], AT::VEC3Df Position, AT::I32F Radius);
		inline AT::I32F GetRadius(){ return m_Radius; }
	protected:
		void SolidSphere(AT::I32F radius, AT::U32 rings, AT::U32 sectors);
		//--
		AT::I32F				m_Radius;
		std::vector<AT::VEC3Df>	m_SphereVertices;
		std::vector<GLuint> 	m_SphereIndices;
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
