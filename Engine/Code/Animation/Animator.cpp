//---------------------------------------------------------------------------
//Project
#include "Animator.h"
#include "../World/Actor.h"
#include "../World/World.h"
//---------------------------------------------------------------------------
// DEFAULT ANIMATOR
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
namespace AE{
	void DEFAULT_ANIMATOR::Update(const WORLD& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder){
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
