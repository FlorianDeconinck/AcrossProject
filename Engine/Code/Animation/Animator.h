//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tool
#include <AcrossTypes.h>
#include <Vec2D.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class ACTOR_ABC;
	class WORLD;
	//---------------------------------------------------------------------------
	class ANIMATION_ABC{};
	//---------------------------------------------------------------------------
	class ANIMATOR_ABC{
			public :
							ANIMATOR_ABC(){
								for(int iA = 0 ; iA < LAST_ANIM_PLAYED_COUNT ; iA++)
									m_AnimationDictionnary[iA] = NULL;
							}
			virtual ~ANIMATOR_ABC(){ 
				for(int iA = 0 ; iA < LAST_ANIM_PLAYED_COUNT ; iA++)
					delete m_AnimationDictionnary[iA];
			}
			enum ANIMATION_ORDER{
				IDLE = 0,
				//---
				WALK,
				//---
				LAST_ANIM_PLAYED_COUNT
			};
			virtual void Update(const WORLD& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder)=0;
			//----------------------------------------------------------------------
			typedef ANIMATION_ABC* ANIMATION_DICTIONNARY[LAST_ANIM_PLAYED_COUNT];
			ANIMATION_DICTIONNARY m_AnimationDictionnary;
			//----------------------------------------------------------------------
			virtual void PopulateAnimationDictionnary()=0;
	};
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	// Default animator
	//---------------------------------------------------------------------------
	class DEFAULT_ANIMATOR:ANIMATOR_ABC{
		public :
								DEFAULT_ANIMATOR():m_AnimationAngle(0){}
		/*virtual*/ void Update(const WORLD& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder);
		/*virtual*/ void PopulateAnimationDictionnary(){};
		protected :
			AT::I32F m_AnimationAngle;
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
