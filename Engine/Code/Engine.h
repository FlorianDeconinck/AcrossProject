//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "./Rendering/Renderer_Interface.h"
#include "./Controller/Controller.h"
#include "World.h"
#include "./GUI/GUI.h"
#include "Window.h"
#include "GlobalDebug.h"
//TOOLS
#include "./Tools/Timer.h"
//WIN32
#include <Windows.h>
//STD
#include <random>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class RESOURCE_MANAGER_ABC;
	class RENDERER_ABC;
	class ENGINE{
	public:
		//-----------------------------------------------------------------------
		enum GAME_MSG{
			NO_MSG=0,
			QUIT,
			//--
			GAME_MSG_COUNT
		};
		//-----------------------------------------------------------------------
		typedef GAME_MSG (GameCallback_t)(ENGINE& E, WORLD& W, CONTROLLER& C);
		//-----------------------------------------------------------------------
		ENGINE(PLT_HINSTANCE hInstance);
		//-----------------------------------------------------------------------
		void Loop(GameCallback_t& GameInitCallback, GameCallback_t& GameUpdateCallback, const AT::I8* sWorldDBFilename);
		//-----------------------------------------------------------------------
		inline AT::I32 RollInt100Dice()				{ return m_DistributionInterger(m_defaultGenerator); }
		inline AT::I64F RollRealDice()				{ return m_DistributionReal(m_defaultGenerator); }
		//-----------------------------------------------------------------------
		inline void SetCamera(BASE_CAMERA* pCamera)	{ m_pRenderer->SetCamera(pCamera); }
		inline void SetCameraToDefault()			{ m_pRenderer->SetCameraToDefault(); }
		//-----------------------------------------------------------------------
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
