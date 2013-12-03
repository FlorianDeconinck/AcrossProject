//---------------------------------------------------------------------------
//WIN32
#include <Windows.h>
#include <stdio.h>
//Engine
#include <Engine_Descriptors.h>
#include <Camera.h>
#include <World/2DGrid/World_2DGrid.h>
//Tool
#include <Singleton.h>
//--------------------------------------------------------------------------
AE::ZELDA_CAMERA	g_ZeldaCam;
AT::I8				g_bSwitch=true;
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
		AE::WORLD_2DGRID& World = (AE::WORLD_2DGRID&)W;
#if 0
		//--
		AT::I32 H = World.GetWorldHeight();
		AT::I32 W = World.GetWorldWidth();
		AT::I32 NPC_COUNT = 10;
		for(AT::I32 iNPC = 0 ; iNPC < NPC_COUNT ; ++iNPC){
			AT::I32F Color[] = { (AT::I32F)E.RollRealDice(), (AT::I32F)E.RollRealDice(), (AT::I32F)E.RollRealDice(), 0.8f };
			while(!World.SpawnNPC(AT::VEC2Di((AT::I32)(E.RollRealDice()*(W-1)), (AT::I32)(E.RollRealDice()*(H-1))), Color)){}
		}
		for(AT::I32 iNPC = 0 ; iNPC < NPC_COUNT ; ++iNPC){
			AE::NPC* Npc = World.GetNPC(iNPC);
			Npc->SetDestination(Npc->GetPosition());
		}
		//--
		//E.SetCamera(&g_ZeldaCam);
		//--

		//--
#else
		World.LoadLevel("Cubes Land");
		for(AT::I32 iNPC = 0 ; iNPC < World.GetNPCCount() ; ++iNPC){
			AE::NPC* Npc = World.GetNPC(iNPC);
			Npc->SetDestination(Npc->GetPosition());
		}
		
#endif
		return AE::ENGINE_API_ENTRYPOINTS::AE_API_OK;
	
	}
	//--------------------------------------------------------------------------
	API_MSG UpdateCallback(AE::ENGINE& E, AE::WORLD_ABC& WorldABC, AE::CONTROLLER& C){
		//Regular cast because GAME is setting up the engine in main()
		AE::WORLD_2DGRID& World = (AE::WORLD_2DGRID&)WorldABC;
		//--
		AT::I32 H = World.GetWorldHeight();
		AT::I32 W = World.GetWorldWidth();
		//--
		for(AT::I32 iNPC = 0 ; iNPC < World.GetNPCCount() ; ++iNPC){
			AE::NPC* Npc = World.GetNPC(iNPC);
			AT::I8 bFoundDest = false;
			if(Npc->IsAtDestination()){
				AT::VEC2Di Dest;
				AT::I32 iTry = 0;
				while(iTry < 100){
					Dest.Set(Npc->GetBBoxHalfWidth()+(AT::I32)(E.RollRealDice()*(W-1-2*Npc->GetBBoxHalfWidth())), Npc->GetBBoxHalfHeight()+(AT::I32)(E.RollRealDice()*(H-1-2*Npc->GetBBoxHalfHeight())));
					if(Npc->IsCollisionFree(World, Dest)==AE::WORLD_2DGRID::WALKABLE){
						World.SetTileStatus(Npc->m_Destination, AE::WORLD_2DGRID::WALKABLE);
						Npc->SetDestination(Dest);
						World.SetTileStatus(Dest, AE::WORLD_2DGRID::DEBUG_PATHFIND);
						bFoundDest = true;
						break;
					}
					iTry++;
				}
			}
			if(bFoundDest) //One NPC search for his destination at a time;
				break;
		}
		//--
		if(C.m_Xbox.m_bSelectButton){
			if(g_bSwitch)
				E.SetCamera(&g_ZeldaCam);
			else
				E.SetCameraToDefault();
			g_bSwitch = !g_bSwitch;
		}
		//--
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
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::WORLD_MODULE::GRID_2D,
															AE::ENGINE_CONFIGURATION_DESCRIPTOR::RESOURCE_MANAGER_MODULE::XML_STATIC,
															ConfigurationReturn
														);
	pEngine->Loop(pGame, "DummyWorld.aeworlddb");
	//---------------
	FreeConsole();
	return 1;
}
//---------------------------------------------------------------------------
