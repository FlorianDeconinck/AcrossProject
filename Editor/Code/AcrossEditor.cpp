//---------------------------------------------------------------------------
//Std
#include <string>
//Tools
#include <AcrossTypes.h>
//Project
#include "Resources/Translater.h"
#include "Editor.h"
//---------------------------------------------------------------------------
int __stdcall WinMain(  _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow){
	for(AT::I32 iArg = 0 ; iArg < __argc ; ++iArg){
		//--
		if(!strcmp(__argv[iArg], "--ts3d")){  //3D translate batch mode
			if(iArg+1 < __argc)
				AE::TRANSLATER_3D_SCENE::TranslateWithAssImp(__argv[iArg+1], __argv[iArg+2]);
			return 0;
		}
		//--
	}
	//--
	AE::ACROSS_EDITOR* pEditor = AE::ACROSS_EDITOR::getInstance();
	//--
	AE::ENGINE	E(hInstance);
	E.Loop(pEditor, NULL);
	//--
	return 0;
}
//---------------------------------------------------------------------------

