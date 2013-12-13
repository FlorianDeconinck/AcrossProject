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
	const GLuint  DIRECTIONAL_LIGHT::m_QuadIndices[6] =		{ 0, 3, 2, 2, 1, 0 };
	//-----------------------------------------------------------------------------
	void DIRECTIONAL_LIGHT::BuildLight(RENDERER_ABC& Renderer, GLfloat Diffuse[4], GLfloat Specular[4], AT::VEC3Df Direction){
		//--
		memcpy(m_Diffuse, Diffuse, 3*sizeof(Diffuse));
		m_DiffuseIntensity = Diffuse[3];
		memcpy(m_Specular, Specular, 3*sizeof(Specular));
		m_SpecularIntensity = Specular[3];
		//--
		m_Mesh.Build(3, (AT::I32F*)m_QuadVectices, 12, (GLuint*)m_QuadIndices, 6, GL_STATIC_DRAW);
		m_PositionOrDirection = Direction;
		m_PositionOrDirection.Normalize();
		//--
		m_Mesh.m_GLDisplayMode = GL_TRIANGLES;
		Renderer.InitRObject(m_Mesh, SHADER_ABC::DEFERRED_LIGHT_DIRECTIONAL_SHADER);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
