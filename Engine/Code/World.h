//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Project
#include "Actor.h"
//Tool
#include <CommonTypes.h>
#include <CodeTools.h>
//std
#include <vector>
//-----------------------------------------------------------------------------
namespace AE{
	class RENDERER;
	class R_OBJECT;
	class GRID{
	public :
		enum MAP_TAG{
			STATIC_OBSTACLE=0,
			WALKABLE,
			//--
			DEBUG_PATHFIND,
			PATH,
			//--
			MAP_TAG_COUNT
		};
		//--
		GRID();
		~GRID(){ if(m_nMap) delete m_nMap; }
		//--
		inline grid_tile_t		GetTile(AT::I32 x, AT::I32 y) const{
			if(x > m_nMapWidth || x < 0)
				return STATIC_OBSTACLE;
			if(y > m_nMapHeight || y < 0)
				return STATIC_OBSTACLE;
			return m_nMap[y*m_nMapWidth + x];
		}
		inline grid_tile_t		GetTile(AT::VEC2Di Pos) const { return GetTile(Pos.x, Pos.y); }
		inline AT::I8			IsTileWalkable(AT::I32 x, AT::I32 y) const{
			if(x > m_nMapWidth || x < 0 || y > m_nMapHeight || y < 0)
				return false;
			grid_tile_t T = m_nMap[y*m_nMapWidth + x];
#ifdef _DEBUG
			return T!=STATIC_OBSTACLE;
#else
			return T==WALKABLE;
#endif
		}
		inline void		SetTile(AT::I32 x, AT::I32 y, MAP_TAG Value){ if(y*m_nMapWidth + x < m_nMapWidth*m_nMapHeight) m_nMap[y*m_nMapWidth + x] = Value; }
		inline grid_tile_t*	GetMapPtr(){ return m_nMap; }
		//--
		void			LoadFromFile(const AT::I8* Filename);
		//--
		AT::I32			m_nMapWidth;
		AT::I32			m_nMapHeight;
	protected :
		grid_tile_t*	m_nMap;				
	};
	//-----------------------------------------------------------------------------
	class SHADER_ABC;
	class CONTROLLER;
	class WORLD:public GRID{
		friend class RENDERER;
		friend class ENGINE;
	public:	
		//Public interface for GAME logic
		enum STATUS{
			LOADING=0,
			RUNTIME,
			//--
			STATUS_COUNT,
		};
		//--
		inline AT::I32 GetWorldHeight()									const { return m_nMapHeight; }
		inline AT::I32 GetWorldWidth()									const { return m_nMapWidth; }
		inline AT::I32 GetNPCCount()										const { return m_NPCArrays.size(); }
		inline NPC*	GetNPC(AT::I32 index)								const { return m_NPCArrays[index]; }
		inline AT::I32F GetTileSize()										const { return m_TileSize; }
		inline AT::I32F GetElapsedTimeSinceUdpateInMS() const { return m_ElapsedTimeSinceLastUpdate_ms; }
		//--
		AT::I8 SpawnNPC(const AT::VEC2Di& Position=AT::VEC2Di(0,0), const AT::I32F* ColorRGBA=NULL);
		void SetTileStatus(AT::VEC2Di tilePos, MAP_TAG S);
		//--
	protected:
		//--
		WORLD(AT::I32F _TileSize=0.1f);
		~WORLD();
		void Init(RENDERER& R);
		void LoadGridFromFile(const AT::I8* Filename);
		void Update(AT::I64F elapsedTime_ms, const CONTROLLER& C);
		void RenderNPC(RENDERER& R, int NPCIdx);
		void RenderPlayer(RENDERER& R);
		void DebugDraw(RENDERER& R);
		//--
		STATUS GetStatus(){ return m_Status; }
		//--
#ifdef _DEBUG
		void		DebugRendererLoad(RENDERER& Renderer);
		R_OBJECT*	m_pRGridQuad;
		R_OBJECT*	m_pRGridLines;
#endif
		AT::I32F					m_TileSize; //size of a single tile in meter
		AT::I32F					m_ElapsedTimeSinceLastUpdate_ms;
		std::vector<NPC*>	m_NPCArrays;
		STATUS						m_Status;
		PLAYER*						m_Player0;
		//--
		RENDERER*					m_pRenderer;			
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------