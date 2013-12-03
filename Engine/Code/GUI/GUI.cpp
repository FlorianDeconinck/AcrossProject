//-----------------------------------------------------------------------------
//Project
#include "GUI.h"
#include "../Renderer_Interface.h"
#include "../Controller/Controller.h"
//Tool
#include <imgui.h>
#include <imguiRenderGL3.h>
#include <CodeTools.h>
//-----------------------------------------------------------------------------
namespace AE{
	GUI::GUI_PERF_LOG	GUI::m_PerfLogContent[GUI::GUI_PERF_LOG::GUI_PERF_LOG_INDEX_COUNT];
	//-----------------------------------------------------------------------------
	GUI::GUI():m_bDraw(true){

	}
	//-----------------------------------------------------------------------------
	void GUI::Init(){
		if (!imguiRenderGLInit("DroidSans.ttf"))
			Break();
	}
	//-----------------------------------------------------------------------------
	GUI::~GUI(){
		imguiRenderGLDestroy();
	}
	//-----------------------------------------------------------------------------
	void GUI::Update(CONTROLLER& C, OPENGL_RENDERER& R, WORLD_ABC& W){
		if(!m_bDraw)
			return;
		//Convert mouse button into ImGUI
		AT::U8 MouseButton;
		switch(C.m_MouseButton){
			case CONTROLLER::LEFT:
				MouseButton = imguiMouseButton::IMGUI_MBUT_LEFT;
				break;
			case CONTROLLER::RIGHT:
				MouseButton = imguiMouseButton::IMGUI_MBUT_RIGHT;
				break;
			default:
				MouseButton = 0 ;
		}
		//--
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		//--
		imguiBeginFrame(C.m_MouseX, C.m_MouseY, MouseButton, C.m_Scroll);
		//--
		AT::I32 PerfLogHeight = 150;
		imguiBeginScrollArea("--- Perf Log ---", 10, 10, 250, PerfLogHeight, &m_PerfLogScrollArea);
		for(AT::I32 iDebugLog = 0 ; iDebugLog < GUI_PERF_LOG::GUI_PERF_LOG_INDEX_COUNT ; ++iDebugLog)
			imguiDrawText(20, PerfLogHeight-30-iDebugLog*15, IMGUI_ALIGN_LEFT, m_PerfLogContent[iDebugLog].m_sText, m_PerfLogContent[iDebugLog].m_ImGuiColor);
		//--
		imguiRenderGLDraw(RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
