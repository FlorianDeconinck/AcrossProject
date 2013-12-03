//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tool
#include <AcrossTypes.h>
#include <Vec2D.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class ANIMATION_ABC{};
	//---------------------------------------------------------------------------
	template <typename WORLD, typename ACTOR> class ANIMATOR_ABC{
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
			virtual void Update(const WORLD& World, const ACTOR& Actor, const ANIMATION_ORDER AnimationOrder)=0;
			//----------------------------------------------------------------------
			typedef ANIMATION_ABC* ANIMATION_DICTIONNARY[LAST_ANIM_PLAYED_COUNT];
			ANIMATION_DICTIONNARY m_AnimationDictionnary;
			//----------------------------------------------------------------------
			virtual void PopulateAnimationDictionnary()=0;
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
