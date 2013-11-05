#pragma once
//---------------------------------------------------------------------------
//Engine
#include "Shaders.h"
#include "Renderer_Interface.h"
//AT
#include <CommonTypes.h>
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	typedef void (*GENBUFFERS) ( GLsizei, GLuint* );
	//---------------------------------------------------------------------------
	class OPENGL_RENDERER:public RENDERER_ABC{
	public : 
		//----------------------------
		enum STATUS{
			UNIT,
			GL_CONTEXT_ERROR,
			GL_CONTEXT_READY,
			READY,
			BUILD_ERROR,
			//--
			STATUS_COUNT,
		};
		//----------------------------
	private:
		HGLRC				m_hGLRC;
		GLuint				m_frameBuffer;
		GLuint				m_rboDepthStencil;
		AT::I8				m_bVSync;
	public:
		STATUS				m_Status;
		GLuint				m_texScreenShaderColorBuffer;
		AT::I32				m_iPostProcess; //0 : nothin - 1 : blur - 2 :fxaa
		//--
		COLOR_SHADER		m_ColorShader;
		TEXTURE_SHADER		m_TextureShader;
		THICK_LINES_COLOR	m_ThickLinesColorShader;
		BLUR_SHADER			m_BlurShader;
		FXAA_SHADER			m_FXAAShader;
		//----
					OPENGL_RENDERER();
					~OPENGL_RENDERER();
		//----
		/*virtual*/ void		Update(GUI& Gui, CONTROLLER& C, WORLD& W);
		/*virtual*/ void		Init();
		/*virtual*/ void		InitRObject(R_OBJECT& R, SHADER_ABC::SHADERS_ID);
		//----
		/*virtual*/ void		KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown);
		/*virtual*/ void		WindowTextTitle(AT::I8* Title);
		//----
		AT::I8		CreateGLContext();
		AT::I8		Build();
		void		ToggleVSync();
		void		Clean();
		//----
		//----------------------------
	};
	//---------------------------------------------------------------------------
}