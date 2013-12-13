//---------------------------------------------------------------------------
//Std
#include <string>
//Tools
#include <AcrossTypes.h>
//Project
#include "Resources/Translater.h"
#include "Editor.h"
#include <Engine_Descriptors.h>
//Engine
#include <./ResourceManager/Asset_Types.h>
//WIN32
#include <Windows.h>
#include <stdio.h>
//---------------------------------------------------------------------------
int __stdcall WinMain(  _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow){
	for(AT::I32 iArg = 0 ; iArg < __argc ; ++iArg){
		//--
		if(!strcmp(__argv[iArg], "--ts3d")){  //3D translate batch mode
			if(iArg+1 < __argc){
				AE::TRANSLATER_3D_SCENE::TEXTURE_PARAMETERS TParameters;
				TParameters.FilenameToImport=NULL;
				AE::TRANSLATER_3D_SCENE::TranslateWithAssImp(AE::ASSET_TYPE::ASSET_3D_MESH, __argv[iArg+1], TParameters, __argv[iArg+2]);
			}
			return 0;
		}
		//--
	}
	//--
	AE::ACROSS_EDITOR* pEditor = AE::ACROSS_EDITOR::getInstance();
	//--
	AE::ENGINE_CONFIGURATION_DESCRIPTOR* EngineConfiguration = AE::ENGINE_CONFIGURATION_DESCRIPTOR::getInstance();
	AE::ENGINE::AE_MSG ConfigurationReturn;
	AE::ENGINE* pEngine = EngineConfiguration->getEngine(	hInstance, 
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::RENDERER_MODULE::OPENGL, 
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::WORLD_MODULE::GRID_3D,
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::RESOURCE_MANAGER_MODULE::XML_STATIC,
															ConfigurationReturn
															);
	pEngine->SetGUIDraw(false);
#ifdef _DEBUG
	AE::g_Profiler.SetDraw(false);
#endif
	pEngine->Loop(pEditor, "Editor.aeworlddb");
	//--
	return 0;
}
//---------------------------------------------------------------------------

