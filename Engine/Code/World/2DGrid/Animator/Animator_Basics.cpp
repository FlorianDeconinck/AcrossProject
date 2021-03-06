//---------------------------------------------------------------------------
//Project
#include "Animator_Basics.h"
#include "../Actor.h"
#include "../World_2DGrid.h"
//---------------------------------------------------------------------------
// DEFAULT ANIMATOR
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	// DEFAULT_ANIMATOR
	//---------------------------------------------------------------------------
	void DEFAULT_ANIMATOR::Update(const WORLD_2DGRID& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder){
		for(AT::I32 iRO = 0 ; iRO < Actor.m_MeshsCount ; iRO++){
			R_OBJECT* pObject = Actor.m_Meshs[iRO];
			//Update RObject model from Actor position
			AT::VEC3Df T = pObject->m_trfModel.GetT();
			T.x = Actor.m_Position.x * World.GetTileSize() + Actor.m_InnerTilePosition.x; 
			T.z = Actor.m_Position.y * World.GetTileSize() + Actor.m_InnerTilePosition.y;
			//--
			pObject->m_trfModel = T;
		}
	};
	//---------------------------------------------------------------------------
	// HOVERING_ANIMATOR
	//---------------------------------------------------------------------------
	void HOVERING_ANIMATOR::Update(const WORLD_2DGRID& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder){
		for(AT::I32 iRO = 0 ; iRO < Actor.m_MeshsCount ; iRO++){
			R_OBJECT* pObject = Actor.m_Meshs[iRO];
			//Update RObject model from Actor position
			AT::VEC3Df T = pObject->m_trfModel.GetT();
			T.x = Actor.m_Position.x * World.GetTileSize() + Actor.m_InnerTilePosition.x; 
			T.z = Actor.m_Position.y * World.GetTileSize() + Actor.m_InnerTilePosition.y;
			//--
			T.y = 1.f + cos(m_AnimationAngle)/6.f;
			m_AnimationAngle += 0.005f;
			if(m_AnimationAngle > 6.28f)
				m_AnimationAngle = 0.f;
			//--
			pObject->m_trfModel = T;
		}
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
