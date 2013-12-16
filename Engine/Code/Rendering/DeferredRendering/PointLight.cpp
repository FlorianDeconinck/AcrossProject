//-----------------------------------------------------------------------------
//Engine
#include "PointLight.h"
#include "../../Renderer_Interface.h"
#include "../Shader.h"
//Tools
#include <MathMacros.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	POINT_LIGHT::POINT_LIGHT() :m_Radius(0){
		
	}
	//-------------------------------------------------------------------------
	void POINT_LIGHT::BuildLight(RENDERER_ABC& Renderer, GLfloat	Diffuse[4], GLfloat Specular[4], AT::VEC3Df Position, AT::I32F Radius){
		//--
		memcpy(m_Diffuse, Diffuse, 3*sizeof(Diffuse));
		m_DiffuseIntensity = Diffuse[3];
		memcpy(m_Specular, Specular, 3*sizeof(Specular));
		m_SpecularIntensity = Specular[3];
		//--
		SolidSphere(Radius, 20, 20);
		m_Mesh.Build(3, (AT::I32F*)m_SphereVertices.data(), (AT::I32)m_SphereVertices.size(), (GLuint*)m_SphereIndices.data(), (AT::I32)m_SphereIndices.size(), GL_STATIC_DRAW);
		m_PositionOrDirection = Position;
		m_Radius = Radius;
		//--
		m_Mesh.m_GLDisplayMode = GL_TRIANGLES;
		Renderer.InitRObject(m_Mesh, SHADER_ABC::DEFERRED_LIGHT_SPOT_SHADER);
	}
	//-------------------------------------------------------------------------
	void POINT_LIGHT::SolidSphere(AT::I32F radius, AT::U32 stackCount, AT::U32 sliceCount){
		m_SphereVertices.clear();
		m_SphereIndices.clear();

		float phiStep = AT_PI / stackCount;
		float thetaStep = 2.0f*AT_PI / sliceCount;

		//VERTICES

		m_SphereVertices.push_back(AT::VEC3Df(0, radius, 0)); //top vertex
		// Compute vertices for each stack ring (do not count the poles as rings).
		for (AT::U32 i = 1; i <= stackCount - 1; ++i){
			float phi = i*phiStep;
			// Vertices of ring.
			for (AT::U32 j = 0; j <= sliceCount; ++j){
				float theta = j*thetaStep;

				AT::VEC3Df v;

				// spherical to cartesian
				v.x = radius*sinf(phi)*cosf(theta);
				v.y = radius*cosf(phi);
				v.z = radius*sinf(phi)*sinf(theta);

				m_SphereVertices.push_back(v);
			}
		}
		m_SphereVertices.push_back(AT::VEC3Df(0, -radius, 0));

		//INDICES

		//
		// Compute indices for top stack.  The top stack was written first to the vertex buffer
		// and connects the top pole to the first ring.
		//

		for (AT::U32 i = 1; i <= sliceCount; ++i){
			m_SphereIndices.push_back(0);
			m_SphereIndices.push_back(i + 1);
			m_SphereIndices.push_back(i);
		}

		//
		// Compute indices for inner stacks (not connected to poles).
		//

		// Offset the indices to the index of the first vertex in the first ring.
		// This is just skipping the top pole vertex.
		AT::U32 baseIndex = 1;
		AT::U32 ringVertexCount = sliceCount + 1;
		for (AT::U32 i = 0; i < stackCount - 2; ++i)
		{
			for (AT::U32 j = 0; j < sliceCount; ++j)
			{
				m_SphereIndices.push_back(baseIndex + i*ringVertexCount + j);
				m_SphereIndices.push_back(baseIndex + i*ringVertexCount + j + 1);
				m_SphereIndices.push_back(baseIndex + (i + 1)*ringVertexCount + j);

				m_SphereIndices.push_back(baseIndex + (i + 1)*ringVertexCount + j);
				m_SphereIndices.push_back(baseIndex + i*ringVertexCount + j + 1);
				m_SphereIndices.push_back(baseIndex + (i + 1)*ringVertexCount + j + 1);
			}
		}

		//
		// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
		// and connects the bottom pole to the bottom ring.
		//
		// South pole vertex was added last.
		AT::U32 southPoleIndex = (AT::U32)m_SphereVertices.size() - 1;

		// Offset the indices to the index of the first vertex in the last ring.
		baseIndex = southPoleIndex - ringVertexCount;

		for (AT::U32 i = 0; i < sliceCount; ++i){
			m_SphereIndices.push_back(southPoleIndex);
			m_SphereIndices.push_back(baseIndex + i);
			m_SphereIndices.push_back(baseIndex + i + 1);
		}
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------