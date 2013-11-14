#pragma once
//-----------------------------------------------------------------------------
//Project
#include "EditorModule.h"
//Tools
#include <Singleton.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class ASSET_IMPORTER:public AT::SINGLETON<ASSET_IMPORTER>,public EDITOR_MODULE_ABC{
		friend class AT::SINGLETON<ASSET_IMPORTER>;
	private :
		ASSET_IMPORTER();
		~ASSET_IMPORTER();
	public :
		//--
		/*virtual*/ ENGINE_API_ENTRYPOINTS::API_MSG	Update(){ return ENGINE_API_ENTRYPOINTS::NO_MSG; }
		/*virtual*/ void							UpdateGUI(){}
		/*virtual*/ void							UpdateNavBarGUI(){}
		//--
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------