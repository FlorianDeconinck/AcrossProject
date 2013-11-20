//-----------------------------------------------------------------------------
//Project
#include "GridGenerator.h"
//Tools
#include <imgui.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	GRID_GENERATOR::GRID_GENERATOR():m_Width(0),m_Height(0){
		m_ID = IMPORT_ASSET_MODE;
		sprintf_s(m_ModuleName, "Grid Generator");
	}
	//-----------------------------------------------------------------------------
	ENGINE_API_ENTRYPOINTS::API_MSG	GRID_GENERATOR::Update(AE::ENGINE& Engine, AE::WORLD& World){
		return ENGINE_API_ENTRYPOINTS::NO_MSG;
	}
	//-----------------------------------------------------------------------------
	void GRID_GENERATOR::UpdateGUI(const AE::ENGINE& Engine){

	}
	//-----------------------------------------------------------------------------
	void GRID_GENERATOR::UpdateNavBarGUI(){
		AT::I32F W = static_cast<AT::I32F>(m_Width);
		AT::I32F H = static_cast<AT::I32F>(m_Height);
		//--
		imguiSlider("Offset UV : x", &W, 0.f, 1024.f, 0.f);
		imguiSlider("Offset UV : y", &H, 0.f, 1024.f, 0.f);
		m_Width		= static_cast<AT::U32>(W);
		m_Height	= static_cast<AT::U32>(H);
		//--
		bool toggle = imguiButton("Generate grid");
		if(toggle){
			std::ofstream gridFile("GeneratedGrid.aegd");
			//--
			gridFile << m_Width << " " << m_Height << std::endl;
			for(AT::U32 iR = 0 ; iR < m_Height; ++iR){
				for(AT::U32 iR = 0 ; iR < m_Height; ++iR){
					gridFile << "1";
				}
				gridFile << std::endl;
			}
			//--
			gridFile.close();
			m_Width = 0;
			m_Height = 0;
		}
		//--
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
