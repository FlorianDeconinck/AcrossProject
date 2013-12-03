#pragma once
//-----------------------------------------------------------------------------
//Engine
#include <Engine.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	struct EDITOR_MODULE_ABC{
		EDITOR_MODULE_ABC():m_ID(NO_EDITOR_MODE){ m_ModuleName[0]=0; }
		~EDITOR_MODULE_ABC(){}
		//--
		enum EDITOR_MODE{
			NO_EDITOR_MODE=0,
			IMPORT_ASSET_MODE,
			GRID_GENERATOR_MODE,
			//--
			EDITOR_MODE_COUNT
		};
		//--
		EDITOR_MODE	m_ID;
		AT::I8		m_ModuleName[64];
		//--
		virtual ENGINE_API_ENTRYPOINTS::API_MSG	Update(AE::ENGINE& Engine, AE::WORLD_2DGRID& World)=0;
		virtual void							UpdateGUI(const AE::ENGINE& Engine)=0;
		virtual void							UpdateNavBarGUI()=0;
		//--
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------