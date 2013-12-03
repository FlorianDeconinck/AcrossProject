//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tools
#include <Singleton.h>
//Projet
#include "Engine.h"
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class ENGINE_CONFIGURATION_DESCRIPTOR:public AT::SINGLETON<ENGINE_CONFIGURATION_DESCRIPTOR>{
		friend class AT::SINGLETON<ENGINE_CONFIGURATION_DESCRIPTOR>;
	public:
		enum RENDERER_MODULE{
			OPENGL=0,
		};
		//-----------------------------------------------------------------------
		enum WORLD_MODULE{
			GRID_2D=0,
		};
		//-----------------------------------------------------------------------
		enum RESOURCE_MANAGER_MODULE{
			XML_STATIC=0,
		};
		//-----------------------------------------------------------------------		
		ENGINE* getEngine(PLT_HINSTANCE& hInstance, RENDERER_MODULE, WORLD_MODULE, RESOURCE_MANAGER_MODULE, ENGINE::AE_MSG&);
		//-----------------------------------------------------------------------
	protected:
		ENGINE_CONFIGURATION_DESCRIPTOR(){}
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
