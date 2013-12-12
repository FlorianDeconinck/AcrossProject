//-----------------------------------------------------------------------------
//Engine
#include "DeferredRendering.h"
#include "../RObject.h"
#include "../../Renderer_Interface.h"
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
	// DEFERRED_RENDERER
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::LightingPass(){
		//Bind defautl frame buffer
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

		m_GBuffer.Read(GBUFFER::GBUFFER_TEXTURES_TEXCOORD);
		glBlitFramebuffer(0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, HalfWidth, 0, RENDERER_ABC::WIDTH, HalfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::GeometryPass(RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects){
		//Bind fbo (MRT)
		m_GBuffer.BindToWrite();
		//Render
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		AT::I32 count = Objects.size();
		for (int iObj = 0; iObj < count; ++iObj){
			Objects[iObj]->Draw(Renderer);
		}
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::Init(){
		m_GBuffer.Init(RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT);
	}
	//-----------------------------------------------------------------------------
	void DEFERRED_RENDERER::Render(RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects){
		GeometryPass(Renderer, Objects);
		LightingPass();
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------