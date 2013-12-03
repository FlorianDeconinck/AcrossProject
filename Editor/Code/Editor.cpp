//---------------------------------------------------------------------------
//Project
#include "Editor.h"
#include "AssetImporter.h"
#include "GridGenerator.h"
//Tool
#include <imgui.h>
#include <imguiRenderGL3.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	ACROSS_EDITOR::ACROSS_EDITOR():m_pCurrentModule(NULL),m_bRenderNavBar(true){
		m_Modules.push_back(ASSET_IMPORTER::getInstance());
		m_Modules.push_back(GRID_GENERATOR::getInstance());
	}
	//---------------------------------------------------------------------------
	ACROSS_EDITOR::~ACROSS_EDITOR(){

	}
	//---------------------------------------------------------------------------
	ENGINE_API_ENTRYPOINTS::API_MSG ACROSS_EDITOR::InitCallback(AE::ENGINE& E, AE::WORLD& World, AE::CONTROLLER& C){ 
		World.LoadLevel("Editor");
		//--
		return ENGINE_API_ENTRYPOINTS::NO_MSG; 
	}
	//---------------------------------------------------------------------------
	ENGINE_API_ENTRYPOINTS::API_MSG ACROSS_EDITOR::UpdateCallback(AE::ENGINE& Engine, AE::WORLD& World, AE::CONTROLLER& Controller){
		API_MSG Msg = NO_MSG;
		if(m_pCurrentModule)
			Msg = m_pCurrentModule->Update(Engine, World);
		return Msg; 
	}
	//--------------------------------------------------------------------------
	ENGINE_API_ENTRYPOINTS::API_MSG ACROSS_EDITOR::RenderCallback(AE::ENGINE& Engine, AE::WORLD& World, AE::CONTROLLER& Controller){
		//--
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		//--
		//Convert mouse button into ImGUI
		AT::U8 MouseButton;
		switch(Controller.m_MouseButton){
		case CONTROLLER::LEFT:
			MouseButton = imguiMouseButton::IMGUI_MBUT_LEFT;
			break;
		case CONTROLLER::RIGHT:
			MouseButton = imguiMouseButton::IMGUI_MBUT_RIGHT;
			break;
		default:
			MouseButton = 0 ;
		}
		AT::I32 Mscroll = 0;
		if (Controller.m_Scroll < 0)
			Mscroll = 2;
		if (Controller.m_Scroll > 0)
			Mscroll = -2;
		//--
		imguiBeginFrame(Controller.m_MouseX, RENDERER_ABC::HEIGHT-38-Controller.m_MouseY, MouseButton, Mscroll, Controller.GetLastASCII());
		//--
		if(m_pCurrentModule)
			m_pCurrentModule->UpdateGUI(Engine);
		RenderNavBar();
		//--
		imguiRenderGLDraw(RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT);
		//--
		return NO_MSG; 
	}
	//---------------------------------------------------------------------------
	void ACROSS_EDITOR::RenderNavBar(){
		if(!m_bRenderNavBar)
			return;
		//--
		bool toggle;
		static AT::I32 RenderNavButtonWidth = 60;
		static AT::I32 RenderNavWidth		= 200;
		AT::I32 NavBarScrollArea=0;
		imguiBeginScrollArea("Across Editor",  0, 0 , RenderNavWidth, RENDERER_ABC::HEIGHT-38 , &NavBarScrollArea);
		imguiSeparatorLine();
		//--
		toggle = imguiCollapse("Modules", "", m_ToggleCollapseModule);
		m_ToggleCollapseModule = toggle ? !m_ToggleCollapseModule : m_ToggleCollapseModule;
		if(m_ToggleCollapseModule){
			imguiIndent();
				AT::U32 Count = m_Modules.size();
				for(AT::U32 iModule = 0 ; iModule < Count ; ++iModule){
					toggle = imguiButton(m_Modules[iModule]->m_ModuleName, m_Modules[iModule]!=m_pCurrentModule);
					if(toggle)
						m_pCurrentModule = m_Modules[iModule];
			}
			imguiUnindent();
		}
		//--
		if(m_pCurrentModule){
			imguiCollapse(m_pCurrentModule->m_ModuleName, "", true);
			imguiIndent();
			m_pCurrentModule->UpdateNavBarGUI();
			imguiUnindent();
		}
		//--
		imguiEndScrollArea();
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
