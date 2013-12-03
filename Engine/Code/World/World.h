//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Project
#include "Actor.h"
//Tool
#include <AcrossTypes.h>
#include <CodeTools.h>
#include <StackAllocator.h>
//std
#include <vector>
//-----------------------------------------------------------------------------
namespace AE{
	class RENDERER_ABC;
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
	class RESOURCE_MANAGER_ABC;
	class WORLD:public GRID{
		friend class OPENGL_RENDERER;
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
							WORLD(AT::I32F _TileSize=0.1f);
							~WORLD();
		//--
		inline	AT::I32		GetWorldHeight()								{ return m_nMapHeight; }
		inline	AT::I32		GetWorldWidth()							const	{ return m_nMapWidth; }
		inline	AT::I32		GetNPCCount()							const	{ return m_NPCArrays.size(); }
		inline	AT::I32		GetPlayerCount()						const	{ return m_Players.size(); }
		inline	NPC*		GetNPC(AT::I32 index)					const	{ return m_NPCArrays[index]; }
		inline	AT::I32F	GetTileSize()							const	{ return m_TileSize; }
		inline	AT::I32F	GetElapsedTimeSinceUdpateInMS()			const	{ return m_ElapsedTimeSinceLastUpdate_ms; }
		inline	AT::VEC2Df	GetPlayerWorldPos(AT::I32 PlayerIdx)	const	{ AT::VEC2Di& V    = m_Players[PlayerIdx]->GetPosition(); return AT::VEC2Df((AT::I32F)V.x, (AT::I32F)V.y)*m_TileSize + m_Players[PlayerIdx]->GetInnerPosition(); }
		inline	void		ClearNPC()										{ for(AT::U32 iNPC = 0 ; iNPC < m_NPCArrays.size() ; ++iNPC) delete m_NPCArrays[iNPC]; m_NPCArrays.clear(); }
		//--
				NPC*		SpawnNPC(const AT::I8* sResourceName=NULL, const AT::VEC2Di& Position=AT::VEC2Di(0,0), const AT::I32F* ColorRGBA=NULL);
				AT::I8		SpawnPlayer(const AT::VEC2Di& Position=AT::VEC2Di(0,0));
				void		LoadLevel(const AT::I8* sLevelName);
				void		SetTileStatus(AT::VEC2Di tilePos, MAP_TAG S);
		//--
		inline AT::STACK_ALLOCATOR_UNSAFE&	GetAllocator() { return m_DynamicAllocator; };
		inline RENDERER_ABC&				GetRenderer() { return *m_pRenderer; }
		inline RESOURCE_MANAGER_ABC&		GetResourceManager() { return *m_pResourceManager; }
		//--
	protected :
		void Init(const AT::I8* sWorldDBFilename, RENDERER_ABC* R, RESOURCE_MANAGER_ABC* pResourceManager);
		void Update(AT::I64F elapsedTime_ms, const CONTROLLER& C);
	private :
		void LoadGridFromFile(const AT::I8* Filename);
		void RenderNPC(RENDERER_ABC& R, AT::I32 NPCIdx);
		void RenderPlayer(RENDERER_ABC& R, AT::I32 PlayerIdx);
#ifdef _DEBUG
		void DebugDraw(RENDERER_ABC& R);
#endif
		//--
		STATUS GetStatus(){ return m_Status; }
		//--
#ifdef _DEBUG
		void		DebugRendererLoad(RENDERER_ABC* Renderer);
		R_OBJECT*	m_pRGridQuad;
		R_OBJECT*	m_pRGridLines;
#endif
		AT::I32F					m_TileSize; //size of a single tile in meter
		AT::I32F					m_ElapsedTimeSinceLastUpdate_ms;
		STATUS						m_Status;
		std::vector<NPC*>			m_NPCArrays;
		std::vector<PLAYER*>		m_Players;
		//--
		RENDERER_ABC*				m_pRenderer;
		RESOURCE_MANAGER_ABC*		m_pResourceManager;
		//--
		AT::I8						m_sWorldDBFilename[128];
		//--
		AT::STACK_ALLOCATOR_UNSAFE	m_DynamicAllocator; //~100Mo
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------