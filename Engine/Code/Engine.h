//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "Renderer.h"
#include "Controller.h"
#include "World.h"
#include "GUI/GUI.h"
#include "Window.h"
#include "GlobalDebug.h"
//TOOLS
#include "Tools/Timer.h"
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
		typedef GAME_MSG (GameCallback_t)(ENGINE& E, WORLD& W);
		//-----------------------------------------------------------------------
		ENGINE(PLT_HINSTANCE hInstance){
			//---------------
			std::uniform_int_distribution<AT::I32>::param_type ParamInt(1, 100);
			m_DistributionInterger.param(ParamInt);
			std::uniform_real_distribution<AT::I64F>::param_type ParamReal(0.0, 1.0);
			m_DistributionReal.param(ParamReal);
			//---------------
			MainWindow.SetNames("AcrossEngineMainWindow", "Across Engine");	
			C.SetDependancies(&R);
			R.SetDependancies(&C, &W);
			MainWindow.AttachEngines(R, C);
			if(!MainWindow.Init(hInstance)){
				Break();
			}
			m_FPS = 0;
			m_FPSIndex = 0;
			memset(m_FPSs, 0, s_FPSIndexMax*sizeof(AT::I64F));
#ifdef _DEBUG
			g_pRenderer = &R;
			g_pWorld = &W;
			g_pController = &C;
#endif
		}
		//-----------------------------------------------------------------------
		void Loop(GameCallback_t& GameInitCallback, GameCallback_t& GameUpdateCallback){
			//--
			C.m_pMainWindow = &MainWindow;
			//--
			R.Init();
			//---
			W.Init(R);
			//---
			G.Init();
			//---
			GameInitCallback(*this, W);
			//---
			AT::I64F tStartFrame, tEndFrame;
			AT::I8 title[64];
			AT::I64F elapsedTime_ms = 0;
			AT::I64F Time = 0;
			AT::I64F FPS_hertz_ms = 1000.0/60.0;
			//--
			while(!C.m_bQuit){
				//--
				tStartFrame = m_Timer.GetTime();
				elapsedTime_ms = tStartFrame - Time;
				Time = tStartFrame;
				//--
				C.Update();
				//--
				GAME_MSG gameMsg = GameUpdateCallback(*this, W);
				//Go through msg
				//--
				W.Update(elapsedTime_ms>FPS_hertz_ms?FPS_hertz_ms:elapsedTime_ms, C);
				//--
				R.Update(G, C, W);
				//--
				tEndFrame = m_Timer.GetTime();
				//--
				m_FPSs[m_FPSIndex]  = 1000.0/(tEndFrame - tStartFrame);
				m_FPSIndex = (m_FPSIndex+1)%s_FPSIndexMax;
				m_FPS = 0;
				for(AT::I32 i = 0 ; i < s_FPSIndexMax ; ++i)
					m_FPS += m_FPSs[i];
				m_FPS /= s_FPSIndexMax;
				if(!C.m_bQuit){
					if(R.m_iPostProcess==1)
						sprintf_s(title, "Across Engine -- FPS : %4.2lf -- PostProcess : Blur", m_FPS);
					else if(R.m_iPostProcess == 2)
						sprintf_s(title, "Across Engine -- FPS : %4.2lf -- PostProcess : FXAA", m_FPS);
					else
						sprintf_s(title, "Across Engine -- FPS : %4.2lf -- PostProcess : None", m_FPS);
					SetWindowText(R.m_hMainWnd, (LPCSTR)title);
				}
				//--
				GL_TOOL::CheckGLError();
			}
		}
		//-----------------------------------------------------------------------
		AT::I32 RollInt100Dice(){
			return m_DistributionInterger(m_defaultGenerator);
		}
		//-----------------------------------------------------------------------
		AT::I64F RollRealDice(){
			return m_DistributionReal(m_defaultGenerator);
		}
		//-----------------------------------------------------------------------
	private:
		RENDERER		R;
		CONTROLLER		C;
		WORLD			W;
		WINDOW			MainWindow;
		GUI				G;
		static const AT::I32 s_FPSIndexMax=1000;
		AT::I64F		m_FPS;
		AT::I64F		m_FPSs[s_FPSIndexMax];
		AT::I32			m_FPSIndex;
		TIMER			m_Timer;
		//Randoms
		std::default_random_engine				m_defaultGenerator;
		std::uniform_int_distribution<AT::I32>	m_DistributionInterger; 
		std::uniform_real_distribution<AT::I64F>m_DistributionReal; 
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
