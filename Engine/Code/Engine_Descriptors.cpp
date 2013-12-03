//---------------------------------------------------------------------------
//Project
#include "Engine_Descriptors.h"
#include "Window.h"
//Modules
#include "./Rendering/OpenGL_Renderer.h"
#include "./World/2DGrid/World_2DGrid.h"
#include "./ResourceManager/XMLStaticResourceManager.h"
//---------------------------------------------------------------------------
namespace AE{
	//----------------------------------------------------------------------
	// ENGINE_CONFIGURATION_DESCRIPTOR
	//----------------------------------------------------------------------
	ENGINE* ENGINE_CONFIGURATION_DESCRIPTOR::getEngine(PLT_HINSTANCE& hInstance, RENDERER_MODULE RendererModule, WORLD_MODULE WorldModule, RESOURCE_MANAGER_MODULE ResourceManagerModule, ENGINE::AE_MSG& AEMsg){
		ENGINE* pEngine = ENGINE::getInstance();
		//--
		//Renderer config
		switch(RendererModule){
			case OPENGL:
				pEngine->m_pRenderer = OPENGL_RENDERER::getInstance();
				break;
			default:
				AEMsg = ENGINE::AE_ERROR_ENGINE_MODULE_INIT;
				return NULL;
		}
		//--
		//Resource manager config
		switch (ResourceManagerModule){
			case XML_STATIC: 
				pEngine->m_pResourceManager = XML_STATIC_RESOURCE_MANAGER::getInstance();
				break;
			default:
				AEMsg = ENGINE::AE_ERROR_ENGINE_MODULE_INIT;
				return NULL;
		}
		//--
		//World config
		switch (WorldModule){
			case GRID_2D:
				pEngine->m_pWorld = WORLD_2DGRID::getInstance();
				break;
			default:
				AEMsg = ENGINE::AE_ERROR_ENGINE_MODULE_INIT;
				return NULL;
		}
		//--
		AEMsg = pEngine->Configure(hInstance);
		return pEngine;
	}
	//----------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
