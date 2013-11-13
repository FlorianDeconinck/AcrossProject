#pragma once
//-----------------------------------------------------------------------------
//Engine
#include <Engine.h>
//Tools
#include <Singleton.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class ACROSS_EDITOR:public AT::SINGLETON<ACROSS_EDITOR>, public ENGINE_API_ENTRYPOINTS{
		friend class AT::SINGLETON<ACROSS_EDITOR>;
	private:
		ACROSS_EDITOR();
		~ACROSS_EDITOR();
	public :
		API_MSG InitCallback(AE::ENGINE& E, AE::WORLD& World, AE::CONTROLLER& C){ return ENGINE_API_ENTRYPOINTS::NO_MSG; }
		API_MSG UpdateCallback(AE::ENGINE& E, AE::WORLD& World, AE::CONTROLLER& C){return ENGINE_API_ENTRYPOINTS::NO_MSG; }
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------