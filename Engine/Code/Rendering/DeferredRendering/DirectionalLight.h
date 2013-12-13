//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include"Light.h"
//Tools
#include <Vec3D.h>
//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	class RENDERER_ABC;
	class SHADER_ABC;
	class DIRECTIONAL_LIGHT :public LIGHT{
	public:
		DIRECTIONAL_LIGHT(){}
		void BuildLight(RENDERER_ABC& Renderer, GLfloat Diffuse[3], GLfloat Specular[3], AT::VEC3Df Position);
	protected:
		//--
		static const GLfloat m_QuadVectices[12];
		static const GLuint	 m_QuadIndices[6];
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
