//---------------------------------------------------------------------------
//Project
#include "Engine.h"
#include "./Rendering/OpenGL_Renderer.h"
//DevIL
#include <IL/il.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	TIMER ENGINE::m_Timer = TIMER();
	//---------------------------------------------------------------------------
	ENGINE::ENGINE(PLT_HINSTANCE hInstance){
		//---------------
		std::uniform_int_distribution<AT::I32>::param_type ParamInt(1, 100);
		m_DistributionInterger.param(ParamInt);
		std::uniform_real_distribution<AT::I64F>::param_type ParamReal(0.0, 1.0);
		m_DistributionReal.param(ParamReal);
		//---------------
		//TMP
		//Module choice system should remove that
		pRenderer = new OPENGL_RENDERER();
		//---------------
		MainWindow.SetNames("AcrossEngineMainWindow", "Across Engine");	
		C.SetDependancies(pRenderer);
		pRenderer->SetDependancies(&C, &W);
		MainWindow.AttachEngines(pRenderer, C);
		if(!MainWindow.Init(hInstance)){
			Break();
		}
		m_FPS = 0;
		m_FPSIndex = 0;
		memset(m_FPSs, 0, s_FPSIndexMax*sizeof(AT::I64F));
#ifdef _DEBUG
		g_pRenderer = pRenderer;
		g_pWorld = &W;
		g_pController = &C;
#endif
	}
	//---------------------------------------------------------------------------
	void ENGINE::Loop(GameCallback_t& GameInitCallback, GameCallback_t& GameUpdateCallback){
		//--
		C.m_pMainWindow = &MainWindow;
		//---
		ilInit();
		//--
		pRenderer->Init();
		//---
		W.Init(pRenderer);
		//---
		G.Init();
		//---
		GameInitCallback(*this, W, C);
		//---
		AT::I64F tStartFrame, tEndFrame;
		AT::I8 title[64];
		AT::I8 rendererTitle[64];
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
			GAME_MSG gameMsg = GameUpdateCallback(*this, W, C);
			//Go through msg
			//--
#ifdef _DEBUG
			if(g_bUpdateWorld)
#endif
				W.Update(elapsedTime_ms>FPS_hertz_ms?FPS_hertz_ms:elapsedTime_ms, C);
			//--
			pRenderer->Update(G, C, W);
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
				pRenderer->WindowTextTitle(rendererTitle);
				sprintf_s(title, "Across Engine -- FPS : %4.2lf -- %s", rendererTitle);
				SetWindowText(pRenderer->m_hMainWnd, (LPCSTR)title);
			}
			//--
			GL_TOOL::CheckGLError();
		}
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------