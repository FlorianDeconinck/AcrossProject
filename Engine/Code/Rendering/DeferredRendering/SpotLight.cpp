//-----------------------------------------------------------------------------
//Engine
#include "SpotLight.h"
#include "../../Renderer_Interface.h"
#include "../Shader.h"
//Tools
#include <MathMacros.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	SPOT_LIGHT::SPOT_LIGHT() :m_Radius(0){
		
	}
	//-------------------------------------------------------------------------
	void SPOT_LIGHT::BuildLight(RENDERER_ABC& Renderer, GLfloat	Diffuse[3], GLfloat Specular[3], AT::VEC3Df Position, AT::I32F Radius){
		//--
		memcpy(m_Diffuse, Diffuse, 3*sizeof(Diffuse));
		memcpy(m_Specular, Specular, 3*sizeof(Specular));
		//--
		SolidSphere(Radius, 10, 10);
		m_Mesh.Build(3, (AT::I32F*)m_SphereVectices.data(), (AT::I32)m_SphereVectices.size(), (GLuint*)m_SphereIndices.data(), (AT::I32)m_SphereVectices.size(), GL_STATIC_DRAW);
		m_PositionOrDirection = Position;
		//--
		m_Mesh.m_GLDisplayMode = GL_TRIANGLE_FAN;
		Renderer.InitRObject(m_Mesh, SHADER_ABC::DEFERRED_LIGHT_SPOT_SHADER);
	}
	//-------------------------------------------------------------------------
	void SPOT_LIGHT::SolidSphere(AT::I32F radius, AT::U32 rings, AT::U32 sectors){
		AT::I32F R = 1.f / (AT::I32F)(rings - 1);
		AT::I32F S = 1.f / (AT::I32F)(sectors - 1);
		AT::U32 r, s;

		m_SphereVectices.resize(rings * sectors * 3);
		std::vector<GLfloat>::iterator v = m_SphereVectices.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			AT::I32F y = sin(-AT_PI_2 + AT_PI * r * R);
			AT::I32F x = cos(2 * AT_PI * s * S) * sin(AT_PI * r * R);
			AT::I32F z = sin(2 * AT_PI * s * S) * sin(AT_PI * r * R);

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;
		}

		m_SphereIndices.resize(rings * sectors * 4);
		std::vector<GLuint>::iterator i = m_SphereIndices.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------