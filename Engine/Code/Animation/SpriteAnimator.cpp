//---------------------------------------------------------------------------
//Project
#include "SpriteAnimator.h"
#include "../Actor.h"
#include "../World.h"
//Tool
#include <AcrossTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	inline void UpdateSprite(AT::I32F* pBuffer, const AT::VEC2Df& Dir, const AT::I32F W, const AT::I32F H){
		AT::I32 Idx = 0;
		pBuffer[Idx + 3] = Dir.x;//0, 1
		pBuffer[Idx + 4] = Dir.y+H;
		Idx += 5;
		pBuffer[Idx + 3] = Dir.x+W;//1,1
		pBuffer[Idx + 4] = Dir.y+H;
		Idx += 5;
		pBuffer[Idx + 3] = Dir.x+W;//1,0
		pBuffer[Idx + 4] = Dir.y;
		Idx += 5;
		pBuffer[Idx + 3] = Dir.x;//0,0
		pBuffer[Idx + 4] = Dir.y;
		Idx += 5;
	}
	//---------------------------------------------------------------------------
	void SPRITE_ANIMATOR::Update(const WORLD& World, const ACTOR_ABC& Actor, const ANIMATION_ORDER AnimationOrder){
		//--
		//Update sprite animation
		//--
		const static AT::I32F MoveTreshold = 0.3f;
		//--
		SPRITE_ANIMATION* Animation = (SPRITE_ANIMATION*)m_AnimationDictionnary[AnimationOrder];
		//--
		AT::VEC2Df Direction; Direction.Zero();
		if(Actor.m_Position == m_CurrentTile){
			Direction = Actor.m_PreviousInnerTilePosition - Actor.m_InnerTilePosition;
			Direction.Normalize();
		}else{
			m_CurrentTile = Actor.m_Position;
			return;
		}
		//--
		glBindBuffer(GL_ARRAY_BUFFER, Actor.m_Meshs[0]->m_vbo);
		GLfloat* pBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		//--
		if(m_CurrentFrameTime_ms >= Animation->m_AnimFrameLength_ms){
			m_LastAnimIdx = (m_LastAnimIdx+1)%Animation->m_AnimFrameCount;
			m_CurrentFrameTime_ms = 0;
		}else	
			m_CurrentFrameTime_ms+=World.GetElapsedTimeSinceUdpateInMS();
		//--	
		if(Direction.x < -MoveTreshold){	//Westward
			if(Direction.y > MoveTreshold){	//North-West
				if(m_LastAnimDirPlayed != NW)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->NORTH_WEST_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);
				m_LastAnimDirPlayed = NW;
			}
			else if(Direction.y < -MoveTreshold){ //South-West
				if(m_LastAnimDirPlayed != SW)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->SOUTH_WEST_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);		
				m_LastAnimDirPlayed = SW;
			}else{	//West
				if(m_LastAnimDirPlayed != W)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->WEST_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);		
				m_LastAnimDirPlayed = W;
			}
		}else if(Direction.x > MoveTreshold){	//Eastward
			if(Direction.y > MoveTreshold){	//North-East
				if(m_LastAnimDirPlayed != NE)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->NORTH_EAST_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);
				m_LastAnimDirPlayed = NE;
			}else if(Direction.y < -MoveTreshold){ //South-East
				if(m_LastAnimDirPlayed != SE)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->SOUTH_EAST_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);
				m_LastAnimDirPlayed = SE;
			}else{	//East
				if(m_LastAnimDirPlayed != E)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->EAST_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);
				m_LastAnimDirPlayed = E;
			}
		}else{
			if(Direction.y > MoveTreshold){ //North
				if(m_LastAnimDirPlayed != N)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->NORTH_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);
				m_LastAnimDirPlayed = N;
			}else if(Direction.y < MoveTreshold){ //South
				if(m_LastAnimDirPlayed != S)
					m_LastAnimIdx = 0;
				UpdateSprite(pBuffer, Animation->SOUTH_SPRITE[m_LastAnimIdx], Animation->m_SPRITE_W, Animation->m_SPRITE_H);
				m_LastAnimDirPlayed = S;
			}
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		//--
		AT::VEC3Df T = Actor.m_Meshs[0]->m_trfModel.GetT();
		T.x = Actor.m_Position.x * World.GetTileSize() + Actor.m_InnerTilePosition.x; 
		T.z = Actor.m_Position.y * World.GetTileSize() + Actor.m_InnerTilePosition.y;
		Actor.m_Meshs[0]->m_trfModel = T;
		//--
	}
	//---------------------------------------------------------------------------
	void SPRITE_ANIMATOR::PopulateAnimationDictionnary(){
		m_AnimationDictionnary[IDLE] = (ANIMATION_ABC*)new SPRITE_ANIMATION;
		m_AnimationDictionnary[WALK] = (ANIMATION_ABC*)new SPRITE_ANIMATION;
	}
	//---------------------------------------------------------------------------
	// SPRITE ANIMATIOn
	//---------------------------------------------------------------------------
	SPRITE_ANIMATION::SPRITE_ANIMATION(){
		//TO REPLACE BY A PROPER FROMAT DESCRIPTION

		m_SPRITE_W = 0.083f;
		m_SPRITE_H = 0.125f;

		m_AnimFrameCount = 4;

		m_AnimFrameLength_ms = 200;

		NORTH_SPRITE[0] = AT::VEC2Df(1*m_SPRITE_W, 3*m_SPRITE_H);
		NORTH_SPRITE[1] = AT::VEC2Df(0*m_SPRITE_W, 3*m_SPRITE_H);
		NORTH_SPRITE[2] = AT::VEC2Df(1*m_SPRITE_W, 3*m_SPRITE_H);
		NORTH_SPRITE[3] = AT::VEC2Df(2*m_SPRITE_W, 3*m_SPRITE_H);

		NORTH_EAST_SPRITE[0] = AT::VEC2Df(4*m_SPRITE_W, 3*m_SPRITE_H);
		NORTH_EAST_SPRITE[1] = AT::VEC2Df(3*m_SPRITE_W, 3*m_SPRITE_H);
		NORTH_EAST_SPRITE[2] = AT::VEC2Df(4*m_SPRITE_W, 3*m_SPRITE_H);
		NORTH_EAST_SPRITE[3] = AT::VEC2Df(5*m_SPRITE_W, 3*m_SPRITE_H);			
		
		EAST_SPRITE[0] = AT::VEC2Df(1*m_SPRITE_W, 2*m_SPRITE_H);
		EAST_SPRITE[1] = AT::VEC2Df(0*m_SPRITE_W, 2*m_SPRITE_H);
		EAST_SPRITE[2] = AT::VEC2Df(1*m_SPRITE_W, 2*m_SPRITE_H);
		EAST_SPRITE[3] = AT::VEC2Df(2*m_SPRITE_W, 2*m_SPRITE_H);

		SOUTH_EAST_SPRITE[0] = AT::VEC2Df(4*m_SPRITE_W, 2*m_SPRITE_H);
		SOUTH_EAST_SPRITE[1] = AT::VEC2Df(3*m_SPRITE_W, 2*m_SPRITE_H);
		SOUTH_EAST_SPRITE[2] = AT::VEC2Df(4*m_SPRITE_W, 2*m_SPRITE_H);
		SOUTH_EAST_SPRITE[3] = AT::VEC2Df(5*m_SPRITE_W, 2*m_SPRITE_H);

		SOUTH_SPRITE[0] = AT::VEC2Df(1*m_SPRITE_W, 0*m_SPRITE_H);
		SOUTH_SPRITE[1] = AT::VEC2Df(0*m_SPRITE_W, 0*m_SPRITE_H);
		SOUTH_SPRITE[2] = AT::VEC2Df(1*m_SPRITE_W, 0*m_SPRITE_H);
		SOUTH_SPRITE[3] = AT::VEC2Df(2*m_SPRITE_W, 0*m_SPRITE_H);

		SOUTH_WEST_SPRITE[0] = AT::VEC2Df(4*m_SPRITE_W, 0*m_SPRITE_H);
		SOUTH_WEST_SPRITE[1] = AT::VEC2Df(3*m_SPRITE_W, 0*m_SPRITE_H);
		SOUTH_WEST_SPRITE[2] = AT::VEC2Df(4*m_SPRITE_W, 0*m_SPRITE_H);
		SOUTH_WEST_SPRITE[3] = AT::VEC2Df(5*m_SPRITE_W, 0*m_SPRITE_H);

		WEST_SPRITE[0] = AT::VEC2Df(1*m_SPRITE_W, 1*m_SPRITE_H);
		WEST_SPRITE[1] = AT::VEC2Df(0*m_SPRITE_W, 1*m_SPRITE_H);
		WEST_SPRITE[2] = AT::VEC2Df(1*m_SPRITE_W, 1*m_SPRITE_H);
		WEST_SPRITE[3] = AT::VEC2Df(2*m_SPRITE_W, 1*m_SPRITE_H);

		NORTH_WEST_SPRITE[0] = AT::VEC2Df(4*m_SPRITE_W, 1*m_SPRITE_H);
		NORTH_WEST_SPRITE[1] = AT::VEC2Df(3*m_SPRITE_W, 1*m_SPRITE_H);
		NORTH_WEST_SPRITE[2] = AT::VEC2Df(4*m_SPRITE_W, 1*m_SPRITE_H);
		NORTH_WEST_SPRITE[3] = AT::VEC2Df(5*m_SPRITE_W, 1*m_SPRITE_H);

	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
