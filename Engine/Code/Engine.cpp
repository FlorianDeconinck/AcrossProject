//---------------------------------------------------------------------------
//Project
#include "Engine.h"
#include "./ResourceManager/XMLStaticResourceManager.h"
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
		m_pRenderer = new OPENGL_RENDERER();
		m_pResourceManager = (RESOURCE_MANAGER_ABC*)new XML_STATIC_RESOURCE_MANAGER();
		//---------------
		m_MainWindow.SetNames("AcrossEngineMainWindow", "Across Engine");	
		m_Controller.SetDependancies(m_pRenderer);
		m_pRenderer->SetDependancies(&m_Controller, &m_World);
		m_MainWindow.AttachEngines(m_pRenderer, m_Controller);
		if(!m_MainWindow.Init(hInstance)){
			Break();
		}
		m_FPS = 0;
		m_FPSIndex = 0;
		memset(m_FPSs, 0, s_FPSIndexMax*sizeof(AT::I64F));
#ifdef _DEBUG
		g_pRenderer = m_pRenderer;
		g_pWorld = &m_World;
		g_pController = &m_Controller;
		//--
		g_Profiler.AddSubTimer("Controller");
		g_Profiler.AddSubTimer("Game callback");
		g_Profiler.AddSubTimer("World");
		g_Profiler.AddSubTimer("Renderer");
		const char* RenderingGUI[128]={"GUI","Renderer"};
		g_Profiler.AddSubTimer(RenderingGUI, 2);
#endif
	}
	//---------------------------------------------------------------------------
	void ENGINE::Loop(ENGINE_API_ENTRYPOINTS* pEntryPoints, const AT::I8* sWorldDBFilename){
		//--
		m_Controller.m_pMainWindow = &m_MainWindow;
		//---
		ilInit();
		//--
		m_pRenderer->Init();
		//---
		m_World.Init(sWorldDBFilename, m_pRenderer, m_pResourceManager);
		//---
		m_Gui.Init();
		//---
		pEntryPoints->InitCallback(*this, m_World, m_Controller);
		//---
		AT::I64F tStartFrame, tEndFrame;
		AT::I8 title[64];
		AT::I8 rendererTitle[64];
		AT::I64F elapsedTime_ms = 0;
		AT::I64F Time = 0;
		AT::I64F FPS_hertz_ms = 1000.0/60.0;
		//--
		while(!m_Controller.m_bQuit){
			//--
			tStartFrame = m_Timer.GetTime();
			elapsedTime_ms = tStartFrame - Time;
			Time = tStartFrame;
			//--
#ifdef _DEBUG			
			g_Profiler.StartFrameTimer();
			g_Profiler.StartSubTimer("Controller");
#endif
			m_Controller.Update();
#ifdef _DEBUG			
			g_Profiler.StopSubTimer("Controller");
#endif
			//--
#ifdef _DEBUG			
			g_Profiler.StartSubTimer("Game callback");
#endif
			ENGINE_API_ENTRYPOINTS::API_MSG gameMsg = pEntryPoints->UpdateCallback(*this, m_World, m_Controller);
#ifdef _DEBUG			
			g_Profiler.StopSubTimer("Game callback");
#endif
			//Go through msg
			//--
#ifdef _DEBUG
			if(g_bUpdateWorld){
				g_Profiler.StartSubTimer("World");
#endif
				m_World.Update(elapsedTime_ms>FPS_hertz_ms?FPS_hertz_ms:elapsedTime_ms, m_Controller);
#ifdef _DEBUG
				g_Profiler.StopSubTimer("World");
			}
#endif
			//--
#ifdef _DEBUG			
			g_Profiler.StartSubTimer("Renderer");
#endif
			m_pRenderer->Update(m_Gui, m_Controller, m_World);
			pEntryPoints->RenderCallback(*this, m_World, m_Controller);
			m_pRenderer->SwapDrawBuffers();
			//--
			tEndFrame = m_Timer.GetTime();
			//--
			m_FPSs[m_FPSIndex]  = 1000.0/(tEndFrame - tStartFrame);
			m_FPSIndex = (m_FPSIndex+1)%s_FPSIndexMax;
			m_FPS = 0;
			for(AT::I32 i = 0 ; i < s_FPSIndexMax ; ++i)
				m_FPS += m_FPSs[i];
			m_FPS /= s_FPSIndexMax;
			if(!m_Controller.m_bQuit){
				m_pRenderer->WindowTextTitle(rendererTitle);
				sprintf_s(title, "Across Engine -- FPS : %4.2lf -- %s", m_FPS, rendererTitle);
				SetWindowText(m_pRenderer->m_hMainWnd, (LPCSTR)title);
			}
			//--
			GL_TOOL::CheckGLError();
		}
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------