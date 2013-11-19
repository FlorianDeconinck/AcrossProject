//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "./Rendering/Renderer_Interface.h"
#include "./ResourceManager/Manager_Interface.h"
#include "./Controller/Controller.h"
#include "./World/World.h"
#include "./GUI/GUI.h"
#include "Window.h"
#include "GlobalDebug.h"
//TOOLS
#include <Timer.h>
#include <Singleton.h>
//WIN32
#include <Windows.h>
//STD
#include <random>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class ENGINE;
	//---------------------------------------------------------------------------
	struct ENGINE_API_ENTRYPOINTS{
		//-----------------------------------------------------------------------
		enum API_MSG{
			NO_MSG=0,
			QUIT,
			//--
			API_MSG_COUNT
		};
		//-----------------------------------------------------------------------
		virtual API_MSG InitCallback(ENGINE& E, WORLD& W, CONTROLLER& C)=0;
		virtual API_MSG UpdateCallback(ENGINE& E, WORLD& W, CONTROLLER& C)=0;
		virtual API_MSG RenderCallback(AE::ENGINE& E, AE::WORLD& World, AE::CONTROLLER& C)=0;
		//-----------------------------------------------------------------------
	};
	//---------------------------------------------------------------------------
	class ENGINE{
	public:
		//-----------------------------------------------------------------------
		ENGINE(PLT_HINSTANCE hInstance);
		//-----------------------------------------------------------------------
		void Loop(ENGINE_API_ENTRYPOINTS* pEntryPoints, const AT::I8* sWorldDBFilename);
		//-----------------------------------------------------------------------
		inline AT::I32 RollInt100Dice()									{ return m_DistributionInterger(m_defaultGenerator); }
		inline AT::I64F RollRealDice()									{ return m_DistributionReal(m_defaultGenerator); }
		//-----------------------------------------------------------------------
		inline void SetCamera(BASE_CAMERA* pCamera)						{ m_pRenderer->SetCamera(pCamera); }
		inline void SetCameraToDefault()								{ m_pRenderer->SetCameraToDefault(); }
		//-----------------------------------------------------------------------
		inline void SetGUIDraw(AT::I8 bDraw)							{ m_Gui.m_bDraw = bDraw; }
		//-----------------------------------------------------------------------
		inline HWND GetRendererHWND()							const	{ return m_pRenderer->m_hMainWnd; }
		//-----------------------------------------------------------------------
		inline void AddResourceToDB(const AT::I8* sResourceName)		{ m_pResourceManager->AddResrouceToDB(sResourceName); }
	public :
		static TIMER	m_Timer;
	private:
		RESOURCE_MANAGER_ABC*	m_pResourceManager;
		RENDERER_ABC*			m_pRenderer;
		CONTROLLER				m_Controller;
		WORLD					m_World;
		WINDOW					m_MainWindow;
		GUI						m_Gui;
		static const			AT::I32 s_FPSIndexMax=1000;
		AT::I64F				m_FPS;
		AT::I64F				m_FPSs[s_FPSIndexMax];
		AT::I32					m_FPSIndex;
		//Randoms
		std::default_random_engine					m_defaultGenerator;
		std::uniform_int_distribution<AT::I32>		m_DistributionInterger; 
		std::uniform_real_distribution<AT::I64F>	m_DistributionReal; 
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
