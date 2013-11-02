//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "./Rendering/Renderer.h"
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
		void Loop(GameCallback_t& GameInitCallback, GameCallback_t& GameUpdateCallback);
		//-----------------------------------------------------------------------
		inline AT::I32 RollInt100Dice()				{ return m_DistributionInterger(m_defaultGenerator); }
		inline AT::I64F RollRealDice()				{ return m_DistributionReal(m_defaultGenerator); }
		//-----------------------------------------------------------------------
		inline void SetCamera(BASE_CAMERA* pCamera)	{ R.SetCamera(pCamera); }
		inline void SetCameraToDefault()			{ R.SetCameraToDefault(); }
		//-----------------------------------------------------------------------
	public :
		static TIMER	m_Timer;
	private:
		RENDERER		R;
		CONTROLLER		C;
		WORLD			W;
		WINDOW			MainWindow;
		GUI				G;
		static const	AT::I32 s_FPSIndexMax=1000;
		AT::I64F		m_FPS;
		AT::I64F		m_FPSs[s_FPSIndexMax];
		AT::I32			m_FPSIndex;
		//Randoms
		std::default_random_engine				m_defaultGenerator;
		std::uniform_int_distribution<AT::I32>	m_DistributionInterger; 
		std::uniform_real_distribution<AT::I64F>m_DistributionReal; 
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
