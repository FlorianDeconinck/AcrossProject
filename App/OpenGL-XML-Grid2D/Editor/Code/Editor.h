#pragma once
//-----------------------------------------------------------------------------
//Project
#include "EditorModule.h"
//Tools
#include <Singleton.h>
//STD
#include <vector>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class AE::ENGINE;
	class ACROSS_EDITOR:public AT::SINGLETON<ACROSS_EDITOR>, public ENGINE_API_ENTRYPOINTS{
		friend class AT::SINGLETON<ACROSS_EDITOR>;
	private:
		ACROSS_EDITOR();
		~ACROSS_EDITOR();
	public :
		/*virtual*/ API_MSG InitCallback(AE::ENGINE& E, AE::WORLD_ABC& World, AE::CONTROLLER& C);
		/*virtual*/ API_MSG UpdateCallback(AE::ENGINE& E, AE::WORLD_ABC& World, AE::CONTROLLER& C);
		/*virtual*/ API_MSG RenderCallback(AE::ENGINE& E, AE::WORLD_ABC& World, AE::CONTROLLER& C);
	private :
		void	RenderNavBar();
		//--
		EDITOR_MODULE_ABC*				m_pCurrentModule;
		//--
		AT::I8							m_bRenderNavBar;
		AT::I8							m_ToggleCollapseModule;
		//--
		std::vector<EDITOR_MODULE_ABC*>	m_Modules;
	};
}//namespace AE
//-----------------------------------------------------------------------------