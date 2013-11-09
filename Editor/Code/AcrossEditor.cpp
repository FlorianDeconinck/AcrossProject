//---------------------------------------------------------------------------
//Std
#include <string>
//Tools
#include <CommonTypes.h>
//Project
#include "Resources/Translater.h"
//---------------------------------------------------------------------------
AT::I32 main(AT::I32 argc, AT::I8* argv[]){
	for(AT::I32 iArg = 0 ; iArg < argc ; ++iArg){
		//--
		if(!strcmp(argv[iArg], "--ts3d")){  //3D translate batch mode
			if(iArg+1 < argc)
				AE::TRANSLATER_3D_SCENE::TranslateWithAssImp(argv[iArg+1], argv[iArg+2]);
			iArg+=2;
		}
		//--
	}
	//--
	return 0;
}
//---------------------------------------------------------------------------

