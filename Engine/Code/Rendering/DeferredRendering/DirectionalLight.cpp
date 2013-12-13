//-----------------------------------------------------------------------------
//Engine
#include "DirectionalLight.h"
#include "../../Renderer_Interface.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	const GLfloat DIRECTIONAL_LIGHT::m_QuadVectices[12] =	{
																-1.f, -1.f, 0.f,
																-1.f,  1.f, 0.f,
																 1.f,  1.f, 0.f,
																 1.f, -1.f, 0.f
															};
	const GLuint  DIRECTIONAL_LIGHT::m_QuadIndices[6] =		{ 0, 1, 2, 2, 3, 0 };
	//-----------------------------------------------------------------------------
	void DIRECTIONAL_LIGHT::BuildLight(RENDERER_ABC& Renderer, GLfloat Diffuse[3], GLfloat Specular[3], AT::VEC3Df Position){
		//--
		memcpy(m_Diffuse, Diffuse, 3*sizeof(Diffuse));
		memcpy(m_Specular, Specular, 3*sizeof(Specular));
		//--
		m_Mesh.Build(3, (AT::I32F*)m_QuadVectices, 12, (GLuint*)m_QuadIndices, 6, GL_STATIC_DRAW);
		m_Position = Position;
		//--
		m_Mesh.m_GLDisplayMode = GL_TRIANGLES;
		Renderer.InitRObject(m_Mesh, SHADER_ABC::DEFERRED_LIGHT_DIRECTIONAL_SHADER);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
