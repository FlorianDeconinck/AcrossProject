#pragma once
//-----------------------------------------------------------------------------
//Project
#include "EditorModule.h"
//Tools
#include <Singleton.h>
//Engine
#include <./ResourceManager/Asset_Types.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class GRID_GENERATOR:public AT::SINGLETON<GRID_GENERATOR>,public EDITOR_MODULE_ABC{
		friend class AT::SINGLETON<GRID_GENERATOR>;
	private :
		GRID_GENERATOR();
		~GRID_GENERATOR(){}
	public :
		//--
		/*virtual*/ ENGINE_API_ENTRYPOINTS::API_MSG	Update(AE::ENGINE& Engine, AE::WORLD& World);
		/*virtual*/ void							UpdateGUI(const AE::ENGINE& Engine);
		/*virtual*/ void							UpdateNavBarGUI();
		//--
	private :
		//--
		AT::U32 m_Width;
		AT::U32 m_Height;
		//--
		static const int InputSize = 10;
		AT::I8 m_WidthTxt[InputSize];
		AT::I8 m_HeightTxt[InputSize];
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------