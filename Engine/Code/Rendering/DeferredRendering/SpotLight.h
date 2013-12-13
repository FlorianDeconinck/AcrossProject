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
	class SPOT_LIGHT:public LIGHT{
	public:
		SPOT_LIGHT();
		void BuildLight(RENDERER_ABC& Renderer, AT::VEC3Df Position, AT::I32F Radius);
	protected:
		void SolidSphere(AT::I32F radius, AT::U32 rings, AT::U32 sectors);
		//--
		AT::I32F				m_Radius;
		std::vector<GLfloat> 	m_SphereVectices;
		std::vector<GLuint> 	m_SphereIndices;
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
