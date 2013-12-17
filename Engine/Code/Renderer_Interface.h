#pragma once
//Project
#include "Camera.h"
#include "World/StaticRenderObject.h"
#include "Rendering/Shader.h"
//Windows
#include <wtypes.h>
#include <wingdi.h>
//Tools
#include <StackAllocator.h>
//STD
#include <vector>
//-----------------------------------------------------------------------------
namespace AE{
	class CONTROLLER;
	class GUI;
	class WORLD_ABC;
	class RESOURCE_MANAGER_ABC;
	class RENDERER_ABC{
		friend class R_OBJECT;
	public :
		RENDERER_ABC():m_hDC(NULL),m_hMainWnd(NULL),m_pController(NULL),m_ShaderAttachedCount(0),m_DynamicAllocator(102400000){
#ifdef _DEBUG
			m_bDrawDebug = true;
#endif
			SetCamera(&m_DefaultCamera);
		}
		~RENDERER_ABC(){}
		//---
		enum LIGHT_TYPE{
			RENDERER_LIGHT_POINT = 0,
			RENDERER_LIGHT_DIRECTIONAL = 1,
			//--
			RENDERER_LIGHT_TYPE_COUNT,
		};
		//---
		virtual AT::I8		Init()=0;
		virtual void		Update(AT::I64F elapsedTime_ms, GUI& Gui, CONTROLLER& C, WORLD_ABC& W) = 0;
		virtual void		SwapDrawBuffers()=0;
		//---
		virtual R_OBJECT*	CreateRObject(RESOURCE_MANAGER_ABC& ResourceManager, const char* sResourceName, AT::VEC3Df& BBoxMin, AT::VEC3Df& BBoxMax)=0;
		virtual void		InitRObject(R_OBJECT& R, SHADER_ABC::SHADERS_ID)=0;
		//---
		virtual void		KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown){}
		virtual void		WindowTextTitle(AT::I8* Title){}
		//---
		virtual void		AddLight(LIGHT_TYPE Type, GLfloat Diffuse[4], GLfloat Specular[4], AT::VEC3Df PositionOrDirection, AT::I32F Radius=0.f)=0;
		//---
		inline void			SetDependancies(CONTROLLER* _pC){m_pController=_pC;}
		inline void			SetMainWindowSettings(HDC _hDC, HWND _hWnd){m_hDC = _hDC; m_hMainWnd = _hWnd;}
		inline AT::I8		IsDCReady(){ return m_hDC != NULL;};
		inline void			SetCamera(BASE_CAMERA* pCamera){	
															m_pCurrentCamera = pCamera;
															m_pCurrentCamera->BuildProjMatrix( 45.0f, (float)RENDERER_ABC::WIDTH / (float)RENDERER_ABC::HEIGHT, 0.01f, 100.0f );
															m_pCurrentCamera->LookAt(m_pCurrentCamera->m_Eye, m_pCurrentCamera->m_Target, m_pCurrentCamera->m_Up);
														}
		inline void			SetCameraToDefault(){ SetCamera(&m_DefaultCamera); }
		inline STATIC_RENDER_OBJECT&		GetScene()			{ return m_Scene; }//UGLY TMP
		//---
		static const	AT::I32 WIDTH = 1280;
		static const	AT::I32 HEIGHT = 720;
		BASE_CAMERA*		m_pCurrentCamera;
		HWND				m_hMainWnd;
#ifdef _DEBUG
		AT::I8				m_bDrawDebug;
#endif
	protected :
		STATIC_RENDER_OBJECT						m_Scene;
		HDC							m_hDC;
		static const AT::I32 MAX_SHADERS_ATTACHED = 1024;
		//--
		CONTROLLER*					m_pController;
		ARCBALL_CAMERA				m_DefaultCamera;
		SHADER_ABC*					m_ShadersAttached[MAX_SHADERS_ATTACHED];
		AT::I32						m_ShaderAttachedCount;
		//--
		std::vector<R_OBJECT*>		m_Objects;
		//--
		AT::STACK_ALLOCATOR_UNSAFE	m_DynamicAllocator; //(~100Mo)
		//--
	};
}//namespace AE
//-----------------------------------------------------------------------------