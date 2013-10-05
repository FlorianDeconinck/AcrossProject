//STD
#include <vector>
#include <iostream>
//AT
#include <CodeTools.h>
//Engine
#include "Renderer.h"
#include "Controller.h"
#include "RObject.h"
#include "World.h"
#include "Shader.h"
#include "GlobalDebug.h"
#include "GUI/GUI.h"
//GL
#include <GL\wglew.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	RENDERER::RENDERER():m_iPostProcess(2),m_hGLRC(NULL),m_hDC(NULL),m_hMainWnd(NULL),m_bVSync(false),m_pController(NULL),m_pWorld(NULL),m_Status(UNIT),m_ShaderAttachedCount(0){
#ifdef _DEBUG
		m_bDrawDebug = true;
#endif
	}
	//---------------------------------------------------------------------------
	RENDERER::~RENDERER(){	
		Clean();
		GL_TOOL::CheckGLError();
	}
	//---------------------------------------------------------------------------
#ifdef _DEBUG
	void RENDERER::DefferedUpdate(GUI& Gui, CONTROLLER& C, WORLD& W){
		Update(Gui, C, W);
		SetWindowText(m_hMainWnd, "Pathfing DBG");
		UpdateWindow(m_hMainWnd);
		Sleep(1000);
	}
#endif
	//---------------------------------------------------------------------------
	void RENDERER::Update(GUI& Gui, CONTROLLER& C, WORLD& W){
		//-----------------------------
		//-----------------------------
		if(m_iPostProcess){
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glEnable( GL_BLEND );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//--
#ifdef _DEBUG
			m_pWorld->DebugDraw(*this);
#endif
			//--
			//Render scene
			m_Scene.PreRender();
			AT::I32 Count = m_Scene.ObjectCount();
			for (int iObj = 0 ; iObj < Count ; ++iObj)
				m_Scene.RenderAtom(*this);
			//--
			//Render NPC
			Count = m_pWorld->GetNPCCount();
			for (AT::I32 iObj = 0 ; iObj < Count ; ++iObj)
				m_pWorld->RenderNPC(*this, iObj);
			m_pWorld->RenderPlayer(*this);
			//--
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_BLEND);
			if(m_iPostProcess==1)
				m_BlurShader.ApplyPostProcess(*this);
			else
				m_FXAAShader.ApplyPostProcess(*this);
		}else{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glEnable( GL_BLEND );
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			//--
#ifdef _DEBUG
			m_pWorld->DebugDraw(*this);
#endif
			//--
			//Render scene (decor)
			m_Scene.PreRender();
			int Count = m_Scene.ObjectCount();
			for (AT::I32 iObj = 0 ; iObj < Count ; ++iObj)
				m_Scene.RenderAtom(*this);
			//--
			//Render NPC
			Count = m_pWorld->GetNPCCount();
			for (AT::I32 iObj = 0 ; iObj < Count ; ++iObj)
				m_pWorld->RenderNPC(*this, iObj);
			m_pWorld->RenderPlayer(*this);
			//--
		}
		//-----------------------------
		Gui.Update(C, *this, W);
		//-----------------------------
		SwapBuffers(m_hDC);
		//-----------------------------
		GL_TOOL::CheckGLError();
	}
	//---------------------------------------------------------------------------
	AT::I8 RENDERER::CreateGLContext(){
		PIXELFORMATDESCRIPTOR pfd;
		memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
		pfd.nSize	   = sizeof(PIXELFORMATDESCRIPTOR);
		pfd.nVersion   = 1;
		pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.iPixelType = PFD_TYPE_RGBA;
		pfd.cColorBits = 32;
		pfd.cDepthBits = 32;
 
		AT::I32 nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);
 
		if (nPixelFormat == 0) return false;
 
		BOOL bResult = SetPixelFormat(m_hDC, nPixelFormat, &pfd);
 
		if (!bResult) return false; 
 
		HGLRC tempContext = wglCreateContext(m_hDC);
		wglMakeCurrent(m_hDC, tempContext);
 
		GLenum err = glewInit();
		if (GLEW_OK != err){
			printf("GLEW is not initialized!");
		}
 
		int attribs[] ={
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
			WGL_CONTEXT_MINOR_VERSION_ARB, 1,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};
 
		if(wglewIsSupported("WGL_ARB_create_context") == 1){
			//hGLRC = wglCreateContext(hDC);
			m_hGLRC = wglCreateContextAttribsARB(m_hDC, 0, attribs);
			wglMakeCurrent(NULL,NULL);
			wglDeleteContext(tempContext);
			wglMakeCurrent(m_hDC, m_hGLRC);
		}else{       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
			m_hGLRC = tempContext;
		}
 
		//VSYNC 
		wglSwapIntervalEXT(m_bVSync);

		//Or better yet, use the GL3 way to get the version number
		AT::I32 OpenGLVersion[2];
		glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
		glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
 
		if(!m_hGLRC)
			return false;
		glViewport(0, 0, WIDTH, HEIGHT);
		//---
		return true;
	}
	//---------------------------------------------------------------------------
	AT::I8 RENDERER::Build(){
		//Read INI
		FILE* SettingsFile = fopen("../../../Asset/BasicSettings.ini", "r");
		AT::VEC3Df Eye;
		fscanf(SettingsFile, "EyeX=%f\nEyeY=%f\nEyeZ=%f\n", &Eye.x, &Eye.y, &Eye.z);
		AT::VEC3Df Target;
		fscanf(SettingsFile, "TargetX=%f\nTargetY=%f\nTargetZ=%f\n",&Target.x, &Target.y, &Target.z);
		AT::VEC3Df Up;
		fscanf(SettingsFile, "UpX=%f\nUpY=%f\nUpZ=%f\n",&Up.x, &Up.y, &Up.z);
		fclose(SettingsFile);
		//!!! ORDER MATTERS !!!
		//TO MOVE TO A BUILD_ENGINE (this is more BUILD SCENE)
		//Shaders
		m_ColorShader.Load(*this, "color.vs", "color.fs");
		m_ThickLinesColorShader.Load(*this, "thickLines.vs", "thickLines.fs", "thickLines.gs");
		m_BlurShader.Load(*this, "blur.vs", "blur.fs");
		m_FXAAShader.Load(*this, "blur.vs", "FXAA.fs");
		//Enable test
		glEnable( GL_DEPTH_TEST );
		//Build obj
		m_ShadersAttached[m_ShaderAttachedCount] = &m_ColorShader; m_ShaderAttachedCount++;
		m_ShadersAttached[m_ShaderAttachedCount] = &m_BlurShader; m_ShaderAttachedCount++;
		m_ShadersAttached[m_ShaderAttachedCount] = &m_FXAAShader; m_ShaderAttachedCount++;
		m_ShadersAttached[m_ShaderAttachedCount] = &m_ThickLinesColorShader; m_ShaderAttachedCount++;
		m_Scene.Load();
		//Camera
		m_MainCamera.BuildProjMatrix( 45.0f, (float)RENDERER::WIDTH / (float)RENDERER::HEIGHT, 0.1f, 100.0f );
		m_MainCamera.LookAt(Eye, Target, Up);
		//FrameBuffer
		glGenTextures(1, &m_texScreenShaderColorBuffer);
		glBindTexture(GL_TEXTURE_2D, m_texScreenShaderColorBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RENDERER::WIDTH, RENDERER::HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenFramebuffers(1, &m_frameBuffer);
		glBindFramebuffer( GL_FRAMEBUFFER, m_frameBuffer );
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texScreenShaderColorBuffer, 0);
		GL_TOOL::CheckGLError();
		//Render buffer object
		glGenRenderbuffers(1, &m_rboDepthStencil);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, RENDERER::WIDTH, RENDERER::HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rboDepthStencil);
		GL_TOOL::CheckGLError();
		//---
		return true;
	}
	//---------------------------------------------------------------------------
	void RENDERER::Init(){
		while(!IsDCReady()){
			Sleep(100); //Wait for window init to wear off
		}
		//--
		if(CreateGLContext())
			m_Status = RENDERER::GL_CONTEXT_READY;
		else
			m_Status = RENDERER::GL_CONTEXT_ERROR;
		//--
		if(Build())
			m_Status = RENDERER::READY;
		else
			m_Status = RENDERER::BUILD_ERROR;
	}
	//---------------------------------------------------------------------------
	void RENDERER::ToggleVSync(){
		m_bVSync = !m_bVSync;
		wglSwapIntervalEXT(m_bVSync); 
	}
	//---------------------------------------------------------------------------
	void RENDERER::Clean(){
		//Clean
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		GL_TOOL::CheckGLError();
		glDeleteFramebuffers(1, &m_frameBuffer);
		GL_TOOL::CheckGLError();
		glDeleteRenderbuffers(1, &m_rboDepthStencil);
		GL_TOOL::CheckGLError();
// 		wglMakeCurrent(m_hDC,NULL);
// 		GL_TOOL::CheckGLError();
// 		wglDeleteContext(m_hGLRC);		
// 		GL_TOOL::CheckGLError();
	}
	//--------------------------------------------------------------------------
	void RENDERER::InitRObject(R_OBJECT& R, SHADER_ABC::SHADERS_ID ID){
		for(AT::I32 iShader = 0 ; iShader < m_ShaderAttachedCount ; ++iShader){
			SHADER_ABC* pShader =m_ShadersAttached[iShader];
			if(pShader->m_ID == ID){
				pShader->InitObject(m_Scene, R);
				R.m_pShader = pShader;
			}
		}
	}
	//--------------------------------------------------------------------------
}//namespace AE
//--------------------------------------------------------------------------
