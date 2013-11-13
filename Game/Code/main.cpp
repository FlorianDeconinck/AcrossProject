//---------------------------------------------------------------------------
//WIN32
#include <Windows.h>
#include <stdio.h>
//Engine
#include <Engine.h>
#include <Camera.h>
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
	API_MSG InitCallback(AE::ENGINE& E, AE::WORLD& World, AE::CONTROLLER& C){ 
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
		World.SpawnPlayer(AT::VEC2Di(30,30));
#endif
		return AE::ENGINE_API_ENTRYPOINTS::NO_MSG;
	
	}
	//--------------------------------------------------------------------------
	API_MSG UpdateCallback(AE::ENGINE& E, AE::WORLD& World, AE::CONTROLLER& C){
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
					if(Npc->IsCollisionFree(World, Dest)==AE::WORLD::WALKABLE){
						World.SetTileStatus(Npc->m_Destination, AE::WORLD::WALKABLE);
						Npc->SetDestination(Dest);
						World.SetTileStatus(Dest, AE::WORLD::DEBUG_PATHFIND);
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
		return AE::ENGINE_API_ENTRYPOINTS::NO_MSG;	
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
	AE::ENGINE	E(hInstance);
	E.Loop(pGame, "DummyWorld.aeworlddb");
	//---------------
	FreeConsole();
	return 1;
}
//---------------------------------------------------------------------------
