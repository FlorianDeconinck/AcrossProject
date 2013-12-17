#pragma once
//---------------------------------------------------------------------------
//Engine
#include "Shaders.h"
#include "../Renderer_Interface.h"
#include "./DeferredRendering/DeferredRendering.h"
//AT
#include <AcrossTypes.h>
#include <Singleton.h>
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	typedef void (*GENBUFFERS) ( GLsizei, GLuint* );
	//---------------------------------------------------------------------------
	class OPENGL_RENDERER:public RENDERER_ABC, public AT::SINGLETON<OPENGL_RENDERER>{
		friend class AT::SINGLETON<OPENGL_RENDERER>;
	protected :
		OPENGL_RENDERER();
		~OPENGL_RENDERER();
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
		enum MODE{
			FORWARD=0,
			DEFERRED,
			MODE_COUNT
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
		MODE				m_Mode;
		DEFERRED_RENDERER	m_DeferredRenderer;
		//--
		COLOR_SHADER		m_ColorShader;
		TEXTURE_SHADER		m_TextureShader;
		THICK_LINES_COLOR	m_ThickLinesColorShader;
		BLUR_SHADER			m_BlurShader;
		FXAA_SHADER			m_FXAAShader;
		//----
		/*virtual*/ AT::I8		Init();
		/*virtual*/ void		Update(AT::I64F elapsedTime_ms, GUI& Gui, CONTROLLER& C, WORLD_ABC& W);
		/*virtual*/ void		SwapDrawBuffers();
		/*virtual*/ void		AddLight(LIGHT_TYPE Type, GLfloat Diffuse[4], GLfloat Specular[4], AT::VEC3Df PositionOrDirection, AT::I32F Radius = 0.f);
		/*virtual*/ R_OBJECT*	CreateRObject(RESOURCE_MANAGER_ABC& ResourceManager, const char* sResourceName, AT::VEC3Df& BBoxMin, AT::VEC3Df& BBoxMax);
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