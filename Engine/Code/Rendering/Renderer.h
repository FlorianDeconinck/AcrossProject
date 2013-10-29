#pragma once
//---------------------------------------------------------------------------
//Windows
#include <wtypes.h>
#include <wingdi.h>
//Engine
#include "../Camera.h"
#include "Shaders.h"
#include "../Scene.h"
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
	class CONTROLLER;
	class WORLD;
	class SHADER_ABC;
	class GUI;
	class RENDERER{
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
		HWND				m_hMainWnd;
	private:
		static const AT::I32 MAX_SHADERS_ATTACHED = 1024;
		HGLRC				m_hGLRC;
		HDC					m_hDC;
		GLuint				m_frameBuffer;
		GLuint				m_rboDepthStencil;
		CONTROLLER*			m_pController;
		WORLD*				m_pWorld;
		AT::I8				m_bVSync;
	public:
#ifdef _DEBUG
		AT::I8				m_bDrawDebug;
#endif
		STATUS				m_Status;
		GLuint				m_texScreenShaderColorBuffer;
		SHADER_ABC*			m_ShadersAttached[MAX_SHADERS_ATTACHED];
		AT::I32				m_ShaderAttachedCount;
		static const		AT::I32 WIDTH = 1280;
		static const		AT::I32 HEIGHT = 720;
		SCENE				m_Scene;
		BASE_CAMERA*		m_pCurrentCamera;
		CLASSIC_CAMERA		m_DefaultCamera;
		AT::I32				m_iPostProcess; //0 : nothin - 1 : blur - 2 :fxaa
		//--
		COLOR_SHADER			m_ColorShader;
		TEXTURE_SHADER		m_TextureShader;
		THICK_LINES_COLOR	m_ThickLinesColorShader;
		BLUR_SHADER				m_BlurShader;
		FXAA_SHADER				m_FXAAShader;
		//----
					RENDERER();
					~RENDERER();
		void		SetDependancies(CONTROLLER* _pC, WORLD* _pW){m_pController=_pC; m_pWorld=_pW;}
		AT::I8		CreateGLContext();
		AT::I8		IsDCReady(){ return m_hDC != NULL;};
		void		SetMainWindowSettings(HDC _hDC, HWND _hWnd){m_hDC = _hDC; m_hMainWnd = _hWnd;}
		AT::I8		Build();
#ifdef _DEBUG
		void		DefferedUpdate(GUI& Gui, CONTROLLER& C, WORLD& W);
#endif
		void		Update(GUI& Gui, CONTROLLER& C, WORLD& W);
		void		Init();
		void		ToggleVSync();
		void		Clean();
		void		InitRObject(R_OBJECT& R, SHADER_ABC::SHADERS_ID);
		//--
		inline void		SetCamera(BASE_CAMERA* pCamera){	
															m_pCurrentCamera = pCamera;
															m_pCurrentCamera->BuildProjMatrix( 45.0f, (float)RENDERER::WIDTH / (float)RENDERER::HEIGHT, 0.1f, 100.0f );
															m_pCurrentCamera->LookAt(m_pCurrentCamera->m_Eye, m_pCurrentCamera->m_Target, m_pCurrentCamera->m_Up);
														}
		//----------------------------
	};
	//---------------------------------------------------------------------------
}