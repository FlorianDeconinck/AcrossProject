//-----------------------------------------------------------------------------
//Project
#include "GUI.h"
#include "../Renderer.h"
#include "../Controller.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imguiRenderGL3.h"
//Tool
#include <CodeTools.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	GUI::GUI():m_bDraw(true){

	}
	//-----------------------------------------------------------------------------
	void GUI::Init(){
		if (!imguiRenderGLInit("../../../Asset/DroidSans.ttf"))
			Break();
	}
	//-----------------------------------------------------------------------------
	GUI::~GUI(){
		imguiRenderGLDestroy();
	}
	//-----------------------------------------------------------------------------
	void GUI::Update(CONTROLLER& C, RENDERER& R, WORLD& W){
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
		imguiBeginScrollArea("--- Debug Log ---", 10, 10, 200, 100, &m_DebugLogScrollArea);
		imguiSeparatorLine();
		//--
		//imguiRenderGLDraw(RENDERER::WIDTH, RENDERER::HEIGHT);
		imguiRenderGLDraw(RENDERER::WIDTH, RENDERER::HEIGHT);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
