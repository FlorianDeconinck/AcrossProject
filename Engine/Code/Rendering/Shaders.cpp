//-----------------------------------------------------------------------------
//Tool
#include <CodeTools.h>
//Project
#include "Shaders.h"
#include "OpenGL_Renderer.h"
#include "RObject.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	// COLOR SHADER
	//-----------------------------------------------------------------------------
	void COLOR_SHADER::Init(RENDERER_ABC& Renderer){
		m_ID = COLOR_3D_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "in_position");
		GL_TOOL::CheckGLError();
		m_colAttrib = glGetAttribLocation(m_Program, "vertex_color");
		GL_TOOL::CheckGLError();
		m_norAttrib = glGetAttribLocation(m_Program, "in_normal");
		GL_TOOL::CheckGLError();
		m_viewUniform = glGetUniformLocation(m_Program, "in_view");
		GL_TOOL::CheckGLError();
		m_projUniform = glGetUniformLocation(m_Program, "in_proj");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void COLOR_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		glBindVertexArray(Object.m_vao);
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 10*sizeof(AT::I32F), (void*)(0*sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_colAttrib);
		glVertexAttribPointer(m_colAttrib, 4, GL_FLOAT, GL_FALSE, 10*sizeof(AT::I32F), (void*)(3*sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_norAttrib, 3, GL_FLOAT, GL_FALSE, 10*sizeof(AT::I32F), (void*)(7*sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_norAttrib);
		Object.m_uniformModel = glGetUniformLocation(m_Program, "in_model");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void COLOR_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_View.ToGL());
		glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_Proj.ToGL());
		glUniformMatrix4fv(RObject.m_uniformModel, 1, GL_FALSE, (GLfloat*)RObject.m_trfModel.ToGL());
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	//TEXTURE SHADER
	//-----------------------------------------------------------------------------
	void TEXTURE_SHADER::Init(RENDERER_ABC& Renderer){
		m_ID = TEXTURE_3D_SHADER;
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
	void TEXTURE_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		glBindVertexArray(Object.m_vao);
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(AT::I32F), (void*)(0*sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_texAttrib);
		glVertexAttribPointer(m_texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(AT::I32F), (void*)(3*sizeof(AT::I32F)));
		glEnableVertexAttribArray(m_norAttrib);
		glVertexAttribPointer(m_norAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(AT::I32F), (void*)(5*sizeof(AT::I32F)));
		Object.m_uniformModel = glGetUniformLocation(m_Program, "in_model");
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void TEXTURE_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		glUniformMatrix4fv(m_viewUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_View.ToGL());
		glUniformMatrix4fv(m_projUniform, 1, GL_FALSE, (GLfloat*)Renderer.m_pCurrentCamera->m_Proj.ToGL());
		glUniformMatrix4fv(RObject.m_uniformModel, 1, GL_FALSE, (GLfloat*)RObject.m_trfModel.ToGL());
		glUniform2fv(m_uvOffsetUniform, 1, (GLfloat*)&RObject.m_UVOffset);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void TEXTURE_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RObject->m_TextureId);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	// THICK LINES COLOR SHADER
	//-----------------------------------------------------------------------------
	void THICK_LINES_COLOR::Init(RENDERER_ABC& R){
		COLOR_SHADER::Init(R);
		//--
		m_ID = THICK_LINES_3D_SHADER;
		m_viewportSizeUniform	= glGetUniformLocation(m_Program, "viewportSize");
		m_lineWidthUniform		= glGetUniformLocation(m_Program, "lineWidth");
		m_maxDepthUniform		= glGetUniformLocation(m_Program, "in_MaxDepth");
		m_CameraEyePosUniform	= glGetUniformLocation(m_Program, "in_CameraEyePos");
		m_ViewportSize[0]		= OPENGL_RENDERER::WIDTH;
		m_ViewportSize[1]		= OPENGL_RENDERER::HEIGHT;
		m_LineWidth				= 4.0f;
		m_MaxDepth              = 8.0f;
		GL_TOOL::CheckGLError();
		//--
	}
	//-----------------------------------------------------------------------------
	void THICK_LINES_COLOR::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
		Use();
		COLOR_SHADER::InitObject(Scene, Object);
		glUniform2fv(m_viewportSizeUniform, 1, (GLfloat*)m_ViewportSize);
		glUniform1f(m_lineWidthUniform, m_LineWidth);
		glUniform1f(m_maxDepthUniform, m_MaxDepth);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void THICK_LINES_COLOR::BindDynamicVertexAttrib(RENDERER_ABC& R, R_OBJECT& Object){
		COLOR_SHADER::BindDynamicVertexAttrib(R, Object);
		//--
		glUniform3fv(m_CameraEyePosUniform, 1, (GLfloat*)&R.m_pCurrentCamera->m_Eye);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	// BLUR SHADER
	//-----------------------------------------------------------------------------
	BLUR_SHADER::BLUR_SHADER(){ 
		//--
		m_Wdw = 4;
		m_Radius = 100;
	}
	//-----------------------------------------------------------------------------
	void BLUR_SHADER::DeferedInit(RENDERER_ABC& Renderer){
		m_ID = BLUR_SCREEN_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "position");
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 4*sizeof(AT::I32F), 0);
		GL_TOOL::CheckGLError();
		m_texAttrib = glGetAttribLocation(m_Program, "texcoord");
		glEnableVertexAttribArray(m_texAttrib);
		glVertexAttribPointer(m_texAttrib, 3, GL_FLOAT, GL_FALSE, 4*sizeof(AT::I32F), (void*)(2*sizeof(AT::I32F)));
		GL_TOOL::CheckGLError();
		m_blurWdwUniform = glGetUniformLocation(m_Program, "blurWdw");
		m_blurRadiusUniform = glGetUniformLocation(m_Program, "blurRadius");
	}
	//-----------------------------------------------------------------------------
	void BLUR_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){
	}
	//-----------------------------------------------------------------------------
	void BLUR_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){
		//Do nothin'
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void BLUR_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		glUniform1i(m_blurWdwUniform, m_Wdw);
		glUniform1i(m_blurRadiusUniform, m_Radius);
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	void FXAA_SHADER::DeferedInit(RENDERER_ABC& Renderer){
		m_ID = FXAA_SCREEN_SHADER;
		//--
		m_posAttrib = glGetAttribLocation(m_Program, "position");
		glEnableVertexAttribArray(m_posAttrib);
		glVertexAttribPointer(m_posAttrib, 3, GL_FLOAT, GL_FALSE, 4*sizeof(AT::I32F), 0);
		GL_TOOL::CheckGLError();
		m_texAttrib = glGetAttribLocation(m_Program, "texcoord");
		glEnableVertexAttribArray(m_texAttrib);
		glVertexAttribPointer(m_texAttrib, 3, GL_FLOAT, GL_FALSE, 4*sizeof(AT::I32F), (void*)(2*sizeof(AT::I32F)));
		GL_TOOL::CheckGLError();
		m_texCoordOffsetUniform = glGetUniformLocation(m_Program, "texcoordOffset");
		//--
		m_FXAA_SPAN_MAX_Uniform = glGetUniformLocation(m_Program, "FXAA_SPAN_MAX");
		m_FXAA_REDUCE_MUL_Uniform = glGetUniformLocation(m_Program, "FXAA_REDUCE_MUL");
		m_FXAA_REDUCE_MIN_Uniform = glGetUniformLocation(m_Program, "FXAA_REDUCE_MIN");
		m_FXAA_SPAN_MAX  = 4.f;
		m_FXAA_REDUCE_MUL = 1.f/8.f;
		m_FXAA_REDUCE_MIN = 1.0/32.f; 
		GL_TOOL::CheckGLError();
	}
	//-----------------------------------------------------------------------------
	void FXAA_SHADER::InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object){

	}
	//-----------------------------------------------------------------------------
	void FXAA_SHADER::BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject){

	}
	//-----------------------------------------------------------------------------
	void FXAA_SHADER::BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject/*=NULL*/){
		textCoordOffsetArray[0] = 1.f/OPENGL_RENDERER::WIDTH;
		textCoordOffsetArray[1] = 1.f/OPENGL_RENDERER::HEIGHT;
		float factor = 1.0f;
		glUniform2fv(m_texCoordOffsetUniform, 1, textCoordOffsetArray);
		glUniform1f(m_FXAA_SPAN_MAX_Uniform, m_FXAA_SPAN_MAX*factor);
		glUniform1f(m_FXAA_REDUCE_MUL_Uniform, m_FXAA_REDUCE_MUL*factor);
		glUniform1f(m_FXAA_REDUCE_MIN_Uniform, m_FXAA_REDUCE_MIN*factor);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
