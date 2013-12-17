//-----------------------------------------------------------------------------
//Engine
#include "DeferredRendering.h"
#include "../RObject.h"
#include "../../Renderer_Interface.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "Light.h"
#include "../../GlobalDebug.h"
//Tool
#include <CodeTools.h>
//STD
#include <iostream>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	// DEFERRED_TEXTURE_SHADER
	//-----------------------------------------------------------------------------
	void DEFERRED_TEXTURE_SHADER::Init(RENDERER_ABC& Renderer){
		m_ID = DEFERRED_TEXTURE_3D_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "in_position");
		GL_TOOL::CheckGLError();
		m_texAttrib = glGetAttribLocation(m_Program, "in_texels");
		GL_TOOL::CheckGLError();
		m_norAttrib = glGetAttribLocation(m_Program, "in_normal");
		GL_TOOL::CheckGLError();
		m_viewUniform = glGetUniformLocation(m_Program, "in_view");
		GL_TOOL::CheckGLError();
		m_projUniform = glGetUniformLocation(m_Program, "in_proj");
		GL_TOOL::CheckGLError();
		m_uvOffsetUniform = glGetUniformLocation(m_Program, "in_uvOffset");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_TEXTURE_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		glBindVertexArray(Object.m_vao);
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(AT::I32F), (void*)(0 * sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_texAttrib);
		glVertexAttribPointer(m_texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(AT::I32F), (void*)(3 * sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_norAttrib);
		glVertexAttribPointer(m_norAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(AT::I32F), (void*)(5 * sizeof(AT::I32F)));
		Object.m_uniformModel = glGetUniformLocation(m_Program, "in_model");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_TEXTURE_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_View.ToGL());
		glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_Proj.ToGL());
		glUniformMatrix4fv(RObject.m_uniformModel, 1, GL_FALSE, (GLfloat*)RObject.m_trfModel.ToGL());
		glUniform2fv(m_uvOffsetUniform, 1, (GLfloat*)&RObject.m_UVOffset);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_TEXTURE_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RObject->m_TextureId);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	// DEFERRED_COLOR_SHADER
	//-----------------------------------------------------------------------------
	void DEFERRED_COLOR_SHADER::Init(RENDERER_ABC& Renderer){
		m_ID = DEFERRED_COLOR_3D_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "in_position");
		GL_TOOL::CheckGLError();
		m_colAttrib = glGetAttribLocation(m_Program, "in_color");
		GL_TOOL::CheckGLError();
		m_norAttrib = glGetAttribLocation(m_Program, "in_normal");
		GL_TOOL::CheckGLError();
		m_viewUniform = glGetUniformLocation(m_Program, "in_view");
		GL_TOOL::CheckGLError();
		m_projUniform = glGetUniformLocation(m_Program, "in_proj");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_COLOR_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		glBindVertexArray(Object.m_vao);
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(AT::I32F), (void*)(0 * sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_colAttrib);
		glVertexAttribPointer(m_colAttrib, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(AT::I32F), (void*)(3 * sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_norAttrib);
		glVertexAttribPointer(m_norAttrib, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(AT::I32F), (void*)(7 * sizeof(AT::I32F)));
		Object.m_uniformModel = glGetUniformLocation(m_Program, "in_model");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_COLOR_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_View.ToGL());
		glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_Proj.ToGL());
		glUniformMatrix4fv(RObject.m_uniformModel, 1, GL_FALSE, (GLfloat*)RObject.m_trfModel.ToGL());
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_COLOR_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	// DEFERRED_LIGHT_SPOT_SHADER
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_SPOT_SHADER::Init(RENDERER_ABC& Renderer){
		m_ID = SHADER_ABC::SHADERS_ID::DEFERRED_LIGHT_SPOT_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "in_position");
		GL_TOOL::CheckGLError();
		//--
		m_viewUniform = glGetUniformLocation(m_Program, "in_view");
		GL_TOOL::CheckGLError();
		m_projUniform = glGetUniformLocation(m_Program, "in_proj");
		GL_TOOL::CheckGLError();
		//--
		m_ScreenSizeUniform = glGetUniformLocation(m_Program, "uScreenSize");
		GL_TOOL::CheckGLError();
		//--
		m_positionMapUniform = glGetUniformLocation(m_Program, "uPositionMap");
		GL_TOOL::CheckGLError();
		m_colorMapUniform = glGetUniformLocation(m_Program, "uColorMap");
		GL_TOOL::CheckGLError();
		m_NormalMapUniform = glGetUniformLocation(m_Program, "uNormalMap");
		GL_TOOL::CheckGLError();
		//--
		m_LightSpecular = glGetUniformLocation(m_Program, "light_specular");
		GL_TOOL::CheckGLError();
		m_LightSpecularIntensity = glGetUniformLocation(m_Program, "light_specular_intensity");
		GL_TOOL::CheckGLError();
		m_LightDiffuse = glGetUniformLocation(m_Program, "light_diffuse");
		GL_TOOL::CheckGLError();
		m_LightDiffuseIntensity = glGetUniformLocation(m_Program, "light_diffuse_intensity");
		GL_TOOL::CheckGLError();
		m_LightPosition = glGetUniformLocation(m_Program, "light_position");
		GL_TOOL::CheckGLError();
		m_LightRadius = glGetUniformLocation(m_Program, "light_radius");
		GL_TOOL::CheckGLError();
		//--
		m_EyeWorldPosition = glGetUniformLocation(m_Program, "eye_world_position");
		GL_TOOL::CheckGLError();
		//--
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_SPOT_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		glBindVertexArray(Object.m_vao);
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(AT::I32F), (void*)(0 * sizeof(AT::I32F)));
		Object.m_uniformModel = glGetUniformLocation(m_Program, "in_model");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_SPOT_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		//--
		glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_View.ToGL());
		glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_Proj.ToGL());
		glUniformMatrix4fv(RObject.m_uniformModel, 1, GL_FALSE, (GLfloat*)RObject.m_trfModel.ToGL());
		//--
		const GLfloat ScreenSize[2] = { RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT };
		glUniform2fv(m_ScreenSizeUniform, 1, ScreenSize);
		//--
		POINT_LIGHT* pPointLight = (POINT_LIGHT*)RObject.m_pLight;
		//--
		glUniform3fv(m_LightDiffuse, 1, RObject.m_pLight->m_Diffuse);
		glUniform1f(m_LightDiffuseIntensity, RObject.m_pLight->m_DiffuseIntensity);
		glUniform3fv(m_LightSpecular, 1, RObject.m_pLight->m_Specular);
		glUniform1f(m_LightSpecularIntensity, RObject.m_pLight->m_SpecularIntensity);
		glUniform3fv(m_LightPosition, 1, (GLfloat*)&RObject.m_pLight->m_PositionOrDirection);
		glUniform1f(m_LightRadius, pPointLight->GetRadius());
		//--
		glUniform3fv(m_EyeWorldPosition, 1, (GLfloat*)&Renderer.m_pCurrentCamera->m_Eye);
		//--
		glUniform1i(m_colorMapUniform, GBUFFER::GBUFFER_TEXTURES_DIFFUSE);
		glUniform1i(m_positionMapUniform, GBUFFER::GBUFFER_TEXTURES_POSITION);
		glUniform1i(m_NormalMapUniform, GBUFFER::GBUFFER_TEXTURES_NORMAL);
		//--
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_SPOT_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	// DEFERRED_LIGHT_DIRECTIONAL_SHADER
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_DIRECTIONAL_SHADER::Init(RENDERER_ABC& Renderer){
		m_ID = SHADER_ABC::SHADERS_ID::DEFERRED_LIGHT_DIRECTIONAL_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "in_position");
		GL_TOOL::CheckGLError();
		//--
		m_positionMapUniform = glGetUniformLocation(m_Program, "uPositionMap");
		GL_TOOL::CheckGLError();
		m_colorMapUniform = glGetUniformLocation(m_Program, "uColorMap");
		GL_TOOL::CheckGLError();
		m_NormalMapUniform = glGetUniformLocation(m_Program, "uNormalMap");
		GL_TOOL::CheckGLError();
		//--
		m_ScreenSizeUniform = glGetUniformLocation(m_Program, "uScreenSize");
		GL_TOOL::CheckGLError();
		//--
		m_LightSpecular = glGetUniformLocation(m_Program, "light_specular");
		GL_TOOL::CheckGLError();
		m_LightSpecularIntensity = glGetUniformLocation(m_Program, "light_specular_intensity");
		GL_TOOL::CheckGLError();
		m_LightDiffuse = glGetUniformLocation(m_Program, "light_diffuse");
		GL_TOOL::CheckGLError();
		m_LightDiffuseIntensity = glGetUniformLocation(m_Program, "light_diffuse_intensity");
		GL_TOOL::CheckGLError();
		m_LightDirection = glGetUniformLocation(m_Program, "light_direction");
		GL_TOOL::CheckGLError();
		//--
		m_EyeWorldPositionUniform = glGetUniformLocation(m_Program, "eye_world_position");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_DIRECTIONAL_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		glBindVertexArray(Object.m_vao);
		GL_TOOL::CheckGLError();
		glEnableVertexAttribArray(m_posAttrib);
		GL_TOOL::CheckGLError();
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(AT::I32F), (void*)(0 * sizeof(AT::I32F)));
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_DIRECTIONAL_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		//--
		const GLfloat ScreenSize[2] = { RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT };
		glUniform2fv(m_ScreenSizeUniform, 1, ScreenSize);
		//--
		glUniform1i(m_positionMapUniform, 0);
		glUniform1i(m_colorMapUniform, 1);
		glUniform1i(m_NormalMapUniform, 2);
		//--
		glUniform3fv(m_LightDiffuse, 1, RObject.m_pLight->m_Diffuse);
		glUniform1f(m_LightDiffuseIntensity, RObject.m_pLight->m_DiffuseIntensity);
		glUniform3fv(m_LightSpecular, 1, RObject.m_pLight->m_Specular);
		glUniform1f(m_LightSpecularIntensity, RObject.m_pLight->m_SpecularIntensity);
		glUniform3fv(m_LightDirection, 1, (GLfloat*)&RObject.m_pLight->m_PositionOrDirection);
		//--
		glUniform3fv(m_EyeWorldPositionUniform, 1, (GLfloat*)&Renderer.m_pCurrentCamera->m_Eye);
		//--
		glUniform1i(m_colorMapUniform, GBUFFER::GBUFFER_TEXTURES_DIFFUSE);
		glUniform1i(m_positionMapUniform, GBUFFER::GBUFFER_TEXTURES_POSITION);
		glUniform1i(m_NormalMapUniform, GBUFFER::GBUFFER_TEXTURES_NORMAL);
		//--
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_LIGHT_DIRECTIONAL_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	// DEFERRED_RENDERER
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::LightingPass(RENDERER_ABC& Renderer, GLuint postProcessFBO){
#if CHECK_GEOMETRY_PASS()
		//Bind default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_GBuffer.BindToRead();

		GLsizei HalfWidth = (GLsizei)(RENDERER_ABC::WIDTH/ 2.0f);
		GLsizei HalfHeight = (GLsizei)(RENDERER_ABC::HEIGHT / 2.0f);

		m_GBuffer.Read(GBUFFER::GBUFFER_TEXTURES_POSITION);
		glBlitFramebuffer(0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, 0, 0, HalfWidth, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		m_GBuffer.Read(GBUFFER::GBUFFER_TEXTURES_DIFFUSE);
		glBlitFramebuffer(0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, 0, HalfHeight, HalfWidth, RENDERER_ABC::HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		m_GBuffer.Read(GBUFFER::GBUFFER_TEXTURES_NORMAL);
		glBlitFramebuffer(0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, HalfWidth, HalfHeight, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

// 		m_GBuffer.Read(GBUFFER::GBUFFER_TEXTURES_TEXCOORD);
// 		glBlitFramebuffer(0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, HalfWidth, 0, RENDERER_ABC::WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
#else
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		m_GBuffer.BindToRead(postProcessFBO);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glClear(GL_COLOR_BUFFER_BIT);
		GL_TOOL::CheckGLError();
		//--
		//Point light pass
		AT::I32 count = m_PointLights.size();
		for (int iLight = 0; iLight < count; ++iLight){
			AT::I8 bCull = (m_PointLights[iLight]->m_PositionOrDirection - Renderer.m_pCurrentCamera->m_Eye).LenghtSquared() < m_PointLights[iLight]->GetRadiusSquared();
			if (!bCull)
				glCullFace(GL_FRONT);
			m_PointLights[iLight]->m_Mesh.Draw(Renderer);
			if (!bCull)
				glCullFace(GL_FRONT);

		}
		glDisable(GL_CULL_FACE);
		//--
		//Directional light pass
		count = m_DirectionalLights.size();
		for (int iLight = 0; iLight < count; ++iLight){
			m_DirectionalLights[iLight]->m_Mesh.Draw(Renderer);
		}
#endif
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::GeometryPass(RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects){
		//Bind fbo (MRT)
		m_GBuffer.BindToWrite();
		//Render
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		AT::I32 count = Objects.size();
		for (int iObj = 0; iObj < count; ++iObj){
			Objects[iObj]->Draw(Renderer);
		}
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::Init(){
		m_GBuffer.Init(RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT);
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::Render(AT::I64F elapsedTime_ms, RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects, GLuint postProcessFBO){
		GeometryPass(Renderer, Objects);
		UpdateLight(elapsedTime_ms);
		LightingPass(Renderer, postProcessFBO);
	}
	//----------------------------------------------------------------------------
	void DEFERRED_RENDERER::AddLight(RENDERER_ABC& Renderer, DEFERRED_RENDERER_LIGHT_TYPE Type, GLfloat Diffuse[4], GLfloat Specular[4], AT::VEC3Df PositionOrDirection, AT::I32F Radius){
		switch (Type){
			case DEFERRED_RENDERER_LIGHT_POINT:{
				POINT_LIGHT* SL = new POINT_LIGHT();
				SL->BuildLight(Renderer, Diffuse, Specular, PositionOrDirection, Radius);
				m_PointLights.push_back(SL);
				break;
			}
			case DEFERED_RENDERER_LIGHT_DIRECTIONAL:{
				DIRECTIONAL_LIGHT* DL = new DIRECTIONAL_LIGHT();
				DL->BuildLight(Renderer, Diffuse, Specular, PositionOrDirection);
				m_DirectionalLights.push_back(DL);
				break;
			}
			case DEFERRED_RENDERER_LIGHT_TYPE_COUNT:
			default:
				return;
		}
	}
	//----------------------------------------------------------------------------
	void DEFERRED_RENDERER::UpdateLight(AT::I64F elapsedTime_ms){
		//--
		//Point lights update
		AT::I32 count = m_PointLights.size();
		for (int iLight = 0; iLight < count; ++iLight){
			POINT_LIGHT* pLight = m_PointLights[iLight];
			//--
			AT::I32F x = pLight->m_PositionOrDirection.x;
			AT::I32F z = pLight->m_PositionOrDirection.z;
			static const AT::I32F angularspeed = 1.f;
			m_movAngle = ((AT::I32F)elapsedTime_ms / 1000.f)*angularspeed;
			//--
			AT::I32F c = cos(m_movAngle);
			AT::I32F s = sin(m_movAngle);
			pLight->m_PositionOrDirection.x = x*c -z*s;
			pLight->m_PositionOrDirection.z = x*s +z*c;
			//--
			pLight->m_Mesh.m_trfModel.SetT(pLight->m_PositionOrDirection);
		}
		//--
		//Directional lights update
		count = m_DirectionalLights.size();
		for (int iLight = 0; iLight < count; ++iLight){
			DIRECTIONAL_LIGHT* pLight = m_DirectionalLights[iLight];
			//--
			//pLight->m_PositionOrDirection.x += (AT::I32F)elapsedTime_ms / 1000.f*1.f;
			//--
			pLight->m_Mesh.m_trfModel.SetT(pLight->m_PositionOrDirection);
		}
	}
	//----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
