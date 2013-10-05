//-----------------------------------------------------------------------------
//Project
#include "Actor.h"
#include "Renderer.h"
#include "World.h"
#include "Pathfinder.h"
//STD
#include <cmath>
#include <random>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	// ABC_ACTOR
	//-----------------------------------------------------------------------------
	const AT::VEC2Di ACTOR_ABC::m_Dir_North(0, 1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_North_East(1, 1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_East(1, 0);
	const AT::VEC2Di ACTOR_ABC::m_Dir_South_East(1, -1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_South(0, -1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_South_West(-1, -1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_West(-1, 0);
	const AT::VEC2Di ACTOR_ABC::m_Dir_North_West(-1, 1);
	//-----------------------------------------------------------------------------
	ACTOR_ABC::ACTOR_ABC():m_Position(13,32),m_MeshsCount(0),m_Speed(1.0),m_Direction(NO_DIRECTION),m_InnerTilePosition(0,0),m_PreviousInnerTilePosition(0,0){
	}
	//-----------------------------------------------------------------------------
	void ACTOR_ABC::LoadMeshs(GRID& Grid, RENDERER& R, const AT::I32F* ColorRGBA/*=NULL*/){
		AT::I32F* pCubeData=NULL;
		AT::I32 CubeVerticesCount;
		GLuint* pCubeDataElements=NULL;
		AT::I32 CubeElementsCount;
		//---
		m_MeshsCount = 2;
		//--
		AT::I32F ColorRGBA_Default[] = {1.f, 1.f, 1.f, 0.8f};
		//--
		R.m_Scene.SpawnCube_Quads(0.5f, pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, ColorRGBA ? ColorRGBA : ColorRGBA_Default);
		m_Meshs[0] = new R_OBJECT();
		m_Meshs[0]->Build(pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, R.m_Scene.GetStaticPool(), GL_STATIC_DRAW, true, true);
		m_Meshs[0]->m_trfModel.SetT(0.f, 1.0f, 0.f);
		R.InitRObject(*m_Meshs[0], SHADER_ABC::COLOR_3D_SHADER);
		delete pCubeDataElements;
		delete pCubeData;
		//--
		R.m_Scene.SpawnCube_Lines(0.5f, pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount);
		m_Meshs[1] = new R_OBJECT();
		m_Meshs[1]->m_GLDisplayMode = GL_LINES;
		m_Meshs[1]->Build(pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, R.m_Scene.GetStaticPool(), GL_STATIC_DRAW, true, true);
		m_Meshs[1]->m_trfModel.SetT(0.f, 1.0f, 0.f);
		R.InitRObject(*m_Meshs[1], SHADER_ABC::THICK_LINES_3D_SHADER);
		delete pCubeDataElements;
		delete pCubeData;	
		//--
		m_BBox.Init(Grid, m_Position, 3, 3);
	}
	//-----------------------------------------------------------------------------
	void ACTOR_ABC::Draw(RENDERER& R, AT::I32F TileSize){
		for(AT::I32 iRO = 0 ; iRO < m_MeshsCount ; iRO++){
			R_OBJECT* pObject = m_Meshs[iRO];
			//Update RObject model from Actor position
			AT::VEC3Df T = pObject->m_trfModel.GetT();
			T.x = m_Position.x * TileSize + m_InnerTilePosition.x;
			T.z = m_Position.y * TileSize + m_InnerTilePosition.y;
			pObject->m_trfModel = T;
			//--
			pObject->Draw(R);
		}
	}
	//-----------------------------------------------------------------------------
	AT::I8 ACTOR_ABC::IsCollisionFreeFromSelf(const GRID& Grid, const AT::VEC2Di PosToTest) const{
		return m_BBox.IsCollisionFree(Grid, m_Position, PosToTest);
	}
	//-----------------------------------------------------------------------------
	AT::I8 ACTOR_ABC::IsCollisionFree(const GRID& Grid, const AT::VEC2Di From, const AT::VEC2Di To) const{
		return m_BBox.IsCollisionFree(Grid, From, To);
	}
	//-----------------------------------------------------------------------
	AT::I8 ACTOR_ABC::IsCollisionFree(const GRID& Grid, const AT::VEC2Di Pos) const{
		return m_BBox.IsCollisionFree(Grid, Pos);
	}
	//-----------------------------------------------------------------------------
	// NPC
	//-----------------------------------------------------------------------------
	NPC::NPC():m_Destination(0, 0),m_NextMove(0,0){
		m_bRecomputePath = false;
	}
	//-----------------------------------------------------------------------------
	AT::I32F trunc(AT::I32F a){
		return a>0 ? std::floor(a) : ceil(a);
	}
	//-----------------------------------------------------------------------------
	void NPC::Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize){
		if(m_Position == m_Destination)
			return;
		//------
		if(m_bRecomputePath){
			m_sizePath = PATHFINDER::FindPath_8Way(Grid, *this, m_Position, m_Destination, m_Path, PATH_MAX_NODES);
			if(m_sizePath<=0)
				return;
			m_iPath = 1;
			m_NextMove = m_Path[1];
			m_bRecomputePath = false;
		}else if(m_Position == m_NextMove){
			m_NextMove = m_Path[++m_iPath];
		}
		//------
		//Update position
		AT::VEC2Di DirTiles		= m_Position - m_NextMove;
		AT::VEC2Df DirRealWorld(DirTiles.x*tileSize, DirTiles.y*tileSize);
		DirRealWorld.Normalize();
		m_PreviousInnerTilePosition = m_InnerTilePosition;
		m_InnerTilePosition		+=  DirRealWorld*(AT::I32F)(m_Speed*(elapsedTime_ms/1000.0));
		//--
		AT::VEC2Di PreviousPosition(-1, -1);
		if(m_InnerTilePosition.x >= tileSize || m_InnerTilePosition.x < 0){
			if((m_InnerTilePosition.x > 0 && m_Position.x != Grid.m_nMapWidth-1) || (m_InnerTilePosition.x < 0 && m_Position.x!=0)){
				PreviousPosition = m_Position;
				//--
				AT::I32F XOffset = m_InnerTilePosition.x/tileSize;
				m_Position.x += (AT::I32)trunc(XOffset);
				m_InnerTilePosition.x -= trunc(XOffset)*tileSize;
			}else{
				m_InnerTilePosition.x = m_PreviousInnerTilePosition.x;
			}
		}
		//--
		if(m_InnerTilePosition.y >= tileSize || m_InnerTilePosition.y < 0){
			if((m_InnerTilePosition.y > 0 && m_Position.y != Grid.m_nMapHeight-1) || (m_InnerTilePosition.y < 0 && m_Position.y!=0)){
				if(PreviousPosition.x < 0)
					PreviousPosition  = m_Position;
				//--
				AT::I32F YOffset = m_InnerTilePosition.y/tileSize;
				m_Position.y += (AT::I32)trunc(YOffset);
				m_InnerTilePosition.y -= trunc(YOffset)*tileSize;
			}else{
				m_InnerTilePosition.y = m_PreviousInnerTilePosition.y;
			}
		}
		//------
		//Update grid occupation if needed
		if(PreviousPosition.x>=0)
			m_BBox.UpdateGridOccupation(Grid, m_Position, PreviousPosition);
	}
	//-----------------------------------------------------------------------------
	// PLAYER
	//-----------------------------------------------------------------------------
	PLAYER::PLAYER(){
		m_DirectonInput.Set(0, 0);
	}
	//-----------------------------------------------------------------------------
	void PLAYER::Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize){
		if(m_DirectonInput.IsZero())
			return;
		//------
		//Update position
		m_PreviousInnerTilePosition = m_InnerTilePosition;
		m_InnerTilePosition	+=  m_DirectonInput*(AT::I32F)(m_Speed*(elapsedTime_ms/1000.0));
		//--
		AT::VEC2Di PreviousPosition(-1, -1);
		if(m_InnerTilePosition.x >= tileSize || m_InnerTilePosition.x < 0){
			AT::I32 XOffset = (AT::I32)trunc(m_InnerTilePosition.x/tileSize);
			if(((m_InnerTilePosition.x > 0 && m_Position.x != Grid.m_nMapWidth-1) || (m_InnerTilePosition.x < 0 && m_Position.x!=0)) && m_BBox.IsCollisionFree(Grid, m_Position, AT::VEC2Di(m_Position.x+XOffset, m_Position.y))){
				PreviousPosition = m_Position;
				//--
				m_Position.x += (AT::I32)XOffset;
				m_InnerTilePosition.x -= XOffset*tileSize;
			}else{
				m_InnerTilePosition.x = m_PreviousInnerTilePosition.x;
			}
		}
		//--
		if(m_InnerTilePosition.y >= tileSize || m_InnerTilePosition.y < 0){
			AT::I32 YOffset = (AT::I32)trunc(m_InnerTilePosition.y/tileSize);
			if(((m_InnerTilePosition.y > 0 && m_Position.y != Grid.m_nMapHeight-1) || (m_InnerTilePosition.y < 0 && m_Position.y!=0)) && m_BBox.IsCollisionFree(Grid, m_Position, AT::VEC2Di(m_Position.x, m_Position.y+YOffset))){
				if(PreviousPosition.x < 0)
					PreviousPosition  = m_Position;
				//--
				m_Position.y += YOffset;
				m_InnerTilePosition.y -= YOffset*tileSize;
			}else{
				m_InnerTilePosition.y = m_PreviousInnerTilePosition.y;
			}
		}
		//------
		//Update grid occupation if needed
		if(PreviousPosition.x>=0)
			m_BBox.UpdateGridOccupation(Grid, m_Position, PreviousPosition);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------