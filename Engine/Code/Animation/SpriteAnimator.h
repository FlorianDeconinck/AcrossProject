//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "Animator.h"
//Gl
#include <GL/glew.h>
#include <GL/gl.h>
//Tool
#include <CommonTypes.h>
#include <Vec2D.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class ACTOR_ABC;
	//---------------------------------------------------------------------------
	class SPRITE_ANIMATION:ANIMATION_ABC{
	public :
		SPRITE_ANIMATION();
		const static AT::I32	nMaxFrame = 4;
		AT::VEC2Df						NORTH_SPRITE[nMaxFrame];
		AT::VEC2Df						NORTH_EAST_SPRITE[nMaxFrame];
		AT::VEC2Df						EAST_SPRITE[nMaxFrame];
		AT::VEC2Df						SOUTH_EAST_SPRITE[nMaxFrame];
		AT::VEC2Df						SOUTH_SPRITE[nMaxFrame];
		AT::VEC2Df						SOUTH_WEST_SPRITE[nMaxFrame];
		AT::VEC2Df						WEST_SPRITE[nMaxFrame];
		AT::VEC2Df						NORTH_WEST_SPRITE[nMaxFrame];
		AT::U32								m_AnimFrameCount;			//Animation frame count
		AT::U32								m_AnimFrameLength_ms;
		AT::I32F							m_SPRITE_W;
		AT::I32F							m_SPRITE_H;
	};
	//---------------------------------------------------------------------------
	class SPRITE_ANIMATOR:public ANIMATOR_ABC{
		public :
									SPRITE_ANIMATOR():m_LastAnimIdx(0), m_CurrentFrameTime_ms(0), m_LastAnimDirPlayed(NO_ANIM_DIR_PLAYED),m_CurrentTile(0,0){}
									~SPRITE_ANIMATOR(){}
			/*virtual*/ void Update(const WORLD& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder);
			/*virtual*/ void PopulateAnimationDictionnary();
		private:
			enum LAST_ANIM_DIR_PLAYED{
				NO_ANIM_DIR_PLAYED = 0,
				//---
				N,
				NE,
				E,
				SE,
				S,
				SW,
				W,
				NW,
				//---
				LAST_ANIM_DIR_PLAYED_COUNT
			};
			ANIMATION_ORDER					m_LastAnimPlayed;				//Last animation played
			LAST_ANIM_DIR_PLAYED		m_LastAnimDirPlayed;		//Last animation direction played
			AT::U32									m_LastAnimIdx;					//Index of last animation frame displayed
			AT::I32F								m_CurrentFrameTime_ms;	//Time elapsed since the begining of the frame played
			AT::VEC2Di							m_CurrentTile;
			//--
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
