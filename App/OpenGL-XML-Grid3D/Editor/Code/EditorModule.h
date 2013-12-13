#pragma once
//-----------------------------------------------------------------------------
//Engine
#include <Engine.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class WORLD_3DGRID;
	struct EDITOR_MODULE_ABC{
		EDITOR_MODULE_ABC():m_ID(NO_EDITOR_MODE){ m_ModuleName[0]=0; }
		~EDITOR_MODULE_ABC(){}
		//--
		enum EDITOR_MODE{
			NO_EDITOR_MODE=0,
			IMPORT_ASSET_MODE,
			//--
			EDITOR_MODE_COUNT
		};
		//--
		EDITOR_MODE	m_ID;
		AT::I8		m_ModuleName[64];
		//--
		virtual ENGINE_API_ENTRYPOINTS::API_MSG	Update(ENGINE& Engine, WORLD_3DGRID& World)=0;
		virtual void							UpdateGUI(const ENGINE& Engine)=0;
		virtual void							UpdateNavBarGUI()=0;
		//--
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------