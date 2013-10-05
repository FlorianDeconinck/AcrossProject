//---------------------------------------------------------------------------
//WIN32
#include <Windows.h>
#include <stdio.h>
//Engine
#include <Engine.h>
//--------------------------------------------------------------------------
AE::ENGINE::GAME_MSG GameLogicInit(AE::ENGINE& E, AE::WORLD& World){
	//--
	AT::I32 H = World.GetWorldHeight();
	AT::I32 W = World.GetWorldWidth();
	AT::I32 NPC_COUNT = 16;
	for(AT::I32 iNPC = 0 ; iNPC < NPC_COUNT ; ++iNPC){
		AT::I32F Color[] = { (AT::I32F)E.RollRealDice(), (AT::I32F)E.RollRealDice(), (AT::I32F)E.RollRealDice(), 0.8f };
		World.SpawnNPC(AT::VEC2Di((AT::I32)(E.RollRealDice()*(W-1)), (AT::I32)(E.RollRealDice()*(H-1))), Color);
	}
	for(AT::I32 iNPC = 0 ; iNPC < NPC_COUNT ; ++iNPC){
		AE::NPC* Npc = World.GetNPC(iNPC);
		Npc->SetDestination(Npc->GetPosition());
	}
	//--
	return AE::ENGINE::NO_MSG;
}
//--------------------------------------------------------------------------
AE::ENGINE::GAME_MSG GameLogicUpdate(AE::ENGINE& E, AE::WORLD& World){
	AT::I32 H = World.GetWorldHeight();
	AT::I32 W = World.GetWorldWidth();
	for(AT::I32 iNPC = 0 ; iNPC < World.GetNPCCount() ; ++iNPC){
		AE::NPC* Npc = World.GetNPC(iNPC);
		AT::I8 bFoundDest = false;
		if(Npc->IsAtDestination()){
			AT::VEC2Di Dest;
			AT::I32 iTry = 0;
			while(iTry < 100){
				Dest.Set((AT::I32)(E.RollRealDice()*(W-1)), (AT::I32)(E.RollRealDice()*(H-1)));
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
	return AE::ENGINE::NO_MSG;
}
//--------------------------------------------------------------------------
int __stdcall WinMain(  _In_  HINSTANCE hInstance, _In_  HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_  int nCmdShow){
	//---------------
	//Console
	AllocConsole();
	//---------------
	AE::ENGINE	E(hInstance);
	E.Loop(GameLogicInit, GameLogicUpdate);
	//---------------
	FreeConsole();
	return 1;
}
//---------------------------------------------------------------------------