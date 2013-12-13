//---------------------------------------------------------------------------
//WIN32
#include <Windows.h>
#include <stdio.h>
//Engine
#include <Engine_Descriptors.h>
#include <Camera.h>
#include <World/3DGrid/World_3DGrid.h>
//Tool
#include <Singleton.h>
//--------------------------------------------------------------------------
class GAME:public AT::SINGLETON<GAME>, public AE::ENGINE_API_ENTRYPOINTS{
	friend class AT::SINGLETON<GAME>;
private:
	GAME(){}
	~GAME(){}
public :
	//--------------------------------------------------------------------------
	API_MSG InitCallback(AE::ENGINE& E, AE::WORLD_ABC& W, AE::CONTROLLER& C){ 
		//Regular cast because GAME is setting up the engine in main()
		AE::WORLD_3DGRID& World = (AE::WORLD_3DGRID&)W;
		World.LoadLevel("Test Bed #1");		
		return AE::ENGINE_API_ENTRYPOINTS::AE_API_OK;
	
	}
	//--------------------------------------------------------------------------
	API_MSG UpdateCallback(AE::ENGINE& E, AE::WORLD_ABC& WorldABC, AE::CONTROLLER& C){
		return AE::ENGINE_API_ENTRYPOINTS::AE_API_OK;	
	}
	//--------------------------------------------------------------------------
	API_MSG RenderCallback(AE::ENGINE& E, AE::WORLD_ABC& World, AE::CONTROLLER& C){
		return AE_API_OK;
	}
	//--------------------------------------------------------------------------
};
//--------------------------------------------------------------------------
int __stdcall WinMain(  _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow){
	//---------------
	//Console
	AllocConsole();
	//---------------
	GAME* pGame = GAME::getInstance();
	//---------------
	AE::ENGINE_CONFIGURATION_DESCRIPTOR* EngineConfiguration = AE::ENGINE_CONFIGURATION_DESCRIPTOR::getInstance();
	AE::ENGINE::AE_MSG ConfigurationReturn;
	AE::ENGINE* pEngine = EngineConfiguration->getEngine(	hInstance, 
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::RENDERER_MODULE::OPENGL, 
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::WORLD_MODULE::GRID_3D,
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::RESOURCE_MANAGER_MODULE::XML_STATIC,
															ConfigurationReturn
														);
	pEngine->Loop(pGame, "Game3DWorld.aeworlddb");
	//---------------
	FreeConsole();
	return 1;
}
//---------------------------------------------------------------------------
