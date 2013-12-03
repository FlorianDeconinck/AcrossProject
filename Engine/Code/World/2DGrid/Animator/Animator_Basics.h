//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "../../../Animation/Animator_Interface.h"
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class ACTOR_ABC;
	class WORLD_2DGRID;
	//---------------------------------------------------------------------------
	// Default animator
	//---------------------------------------------------------------------------
	class DEFAULT_ANIMATOR:ANIMATOR_ABC<WORLD_2DGRID, ACTOR_ABC>{
	public :
		DEFAULT_ANIMATOR(){}
		/*virtual*/ void Update(const WORLD_2DGRID& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder);
		/*virtual*/ void PopulateAnimationDictionnary(){};
	};
	//---------------------------------------------------------------------------
	// Hovering animator
	//---------------------------------------------------------------------------
	class HOVERING_ANIMATOR:ANIMATOR_ABC<WORLD_2DGRID, ACTOR_ABC>{
		HOVERING_ANIMATOR():m_AnimationAngle(0){}
		/*virtual*/ void Update(const WORLD_2DGRID& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder);
		/*virtual*/ void PopulateAnimationDictionnary(){};		
	protected :
		AT::I32F m_AnimationAngle;
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
