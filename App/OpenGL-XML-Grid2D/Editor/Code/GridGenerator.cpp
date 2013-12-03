//-----------------------------------------------------------------------------
//Project
#include "GridGenerator.h"
//Tools
#include <imgui.h>
//STD
#include <fstream>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	GRID_GENERATOR::GRID_GENERATOR():m_Width(0),m_Height(0){
		m_ID = IMPORT_ASSET_MODE;
		sprintf_s(m_ModuleName, "Grid Generator");
		sprintf_s(m_WidthTxt, "32");
		sprintf_s(m_HeightTxt, "32");
	}
	//-----------------------------------------------------------------------------
	ENGINE_API_ENTRYPOINTS::API_MSG	GRID_GENERATOR::Update(AE::ENGINE& Engine, AE::WORLD_2DGRID& World){
		return ENGINE_API_ENTRYPOINTS::AE_API_OK;
	}
	//-----------------------------------------------------------------------------
	void GRID_GENERATOR::UpdateGUI(const AE::ENGINE& Engine){

	}
	//-----------------------------------------------------------------------------
	void GRID_GENERATOR::UpdateNavBarGUI(){
		imguiTextInput("Grid width :", m_WidthTxt, InputSize);
		imguiTextInput("Grid height :", m_HeightTxt, InputSize);
		m_Width		= atoi(m_WidthTxt);
		m_Height	= atoi(m_HeightTxt);
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
