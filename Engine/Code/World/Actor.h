//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Engine
#include "../Rendering/RObject.h"
#include "GridBBox.h"
//Tools
#include "Vec2D.h"
#include <AcrossTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	class RENDERER_ABC;
	class GRID;
	class ANIMATOR_ABC;
	class WORLD;
	//---------------------------------------------------------------------------
	class ACTOR_ABC{
		friend class DEFAULT_ANIMATOR;
		friend class SPRITE_ANIMATOR;
	public :
								ACTOR_ABC();
		virtual					~ACTOR_ABC();
		//---
		void					Draw(const WORLD& W, RENDERER_ABC& R, AT::I32F TileSize);
		void					LoadDefaultMeshs(GRID& Grid, RENDERER_ABC& R, const AT::I32F* ColorRGBA=NULL);
		AT::I8					IsWithinBorders(const GRID& Grid, const AT::VEC2Di& Pos) const;
		void					SetAnimatorModule(ANIMATOR_ABC* pAnimator);
		//---
		inline R_OBJECT**		GetRenderingObjects(AT::I32& MeshsCount)										const	{ MeshsCount = m_MeshsCount ; return (R_OBJECT**)m_Meshs; }
		inline AT::I8			IsCollisionFreeFromSelf(const GRID& Grid, const AT::VEC2Di& PosToTest)			const	{ return m_BBox.IsCollisionFree(Grid, m_Position, PosToTest); }
		inline AT::I8			IsCollisionFree(const GRID& Grid, const AT::VEC2Di& From, const AT::VEC2Di To)	const	{ return m_BBox.IsCollisionFree(Grid, From, To); }
		inline AT::I8			IsCollisionFree(const GRID& Grid, const AT::VEC2Di& Pos)						const	{ return m_BBox.IsCollisionFree(Grid, Pos); }
		inline AT::I8			IsInsideBBox(const GRID& Grid, const AT::VEC2Di Pos)							const	{ return m_BBox.IsInside(m_Position, Pos); }
		inline AT::I32			GetBBoxHalfWidth()																const	{ return m_BBox.m_HalfWidth; }
		inline AT::I32			GetBBoxHalfHeight()																const	{ return m_BBox.m_HalfHeight; }
		inline void				EraseFromGrid(GRID& Grid)														const	{ m_BBox.EraseGridOccupation(Grid, m_Position); }
		inline void				AddToGrid(GRID& Grid)															const	{ m_BBox.BuildGridOccupation(Grid, m_Position); }
		inline AT::VEC2Di		GetPosition()																	const	{ return m_Position; }
		inline AT::VEC2Df		GetInnerPosition()																const	{ return m_InnerTilePosition; }
		inline void				SetUVOffset(AT::I32 idx, AT::I32F x, AT::I32F y)										{ m_Meshs[idx]->m_UVOffset.x=x; m_Meshs[idx]->m_UVOffset.y=y; }
		inline AT::VEC2Df		GetUVOffset(AT::I32 idx)														const	{ return m_Meshs[idx]->m_UVOffset; }
		//---
		virtual void			Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize)=0;
		virtual void			SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y)=0;
		virtual void			SetPosition(GRID& Grid, const AT::VEC2Di& V)=0;
		//---
		enum DIRECTIONS{
			NO_DIRECTION = 0,
			NORTH,
			NORTH_EAST,
			EAST,
			SOUTH_EAST,
			SOUTH,
			SOUTH_WEST,
			WEST,
			NORTH_WEST,
			//--
			DIRECTIONS_COUNT
		};
		AT::I64F				  m_Speed;	//m/s
		const static AT::VEC2Di	  m_Dir_North;
		const static AT::VEC2Di	  m_Dir_North_East;
		const static AT::VEC2Di	  m_Dir_East;
		const static AT::VEC2Di	  m_Dir_South_East;
		const static AT::VEC2Di	  m_Dir_South;
		const static AT::VEC2Di	  m_Dir_South_West;
		const static AT::VEC2Di	  m_Dir_West;
		const static AT::VEC2Di	  m_Dir_North_West;
	protected:
		static const AT::I32	MAX_MESHS = 10;
		AT::I32					m_MeshsCount;
		GRID_BBOX				m_BBox;
		R_OBJECT*				m_Meshs[MAX_MESHS];
		AT::VEC2Df				m_InnerTilePosition; 
		AT::VEC2Df				m_PreviousInnerTilePosition; // Vector between 0 - 1
		AT::VEC2Di				m_Position;
		AT::VEC2Di				m_LastPosition;
		//---
	private :
		ANIMATOR_ABC*			m_pAnimator;
	public:
#ifdef _DEBUG
		AT::I8					m_bDebugPathfind;
#endif
	};
	//---------------------------------------------------------------------------
	class NPC:public ACTOR_ABC{
	public:
								NPC();
		//---
		/*virtual*/ void		Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize);
		inline /*virtual*/ void	SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y){ m_BBox.UpdateGridOccupation(Grid, m_Position, AT::VEC2Di(X, Y)) ; m_Position.x = m_NextMove.x = X; m_Position.y = m_NextMove.y = Y; }
		inline /*virtual*/ void	SetPosition(GRID& Grid, const AT::VEC2Di& NewPosition){ m_BBox.UpdateGridOccupation(Grid, m_Position, NewPosition); m_Position = m_NextMove = NewPosition; }
		inline void				SetDestination(const AT::VEC2Di& Destination){ m_Destination = Destination; m_bRecomputePath = true; }
		inline AT::I8			IsAtDestination(){ return m_Position == m_Destination; }
		//---
		void					LoadMeshs(WORLD& World, void* pBufferFromFile, RENDERER_ABC& Renderer);
		//---
		static const AT::I32	PATH_MAX_NODES = 1024;
		//---
		AT::VEC2Di				m_Path[1024];
		AT::U32					m_iPath;
		AT::I32					m_sizePath;
		//---
		AT::VEC2Di				m_NextMove;
		AT::VEC2Di				m_Destination;
		AT::I8					m_bRecomputePath;
	};
	//---------------------------------------------------------------------------
	class PLAYER:public NPC{
		public:
								PLAYER();
					void		LoadDefaultMeshs(GRID& Grid, RENDERER_ABC& R, const AT::I32F* ColorRGBA=NULL);
		/*virtual*/ void		Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize);
		/*virtual*/ void		SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y){ m_BBox.UpdateGridOccupation(Grid, m_Position, AT::VEC2Di(X, Y)) ; m_LastPosition = m_Position; m_Position.x = X; m_Position.y = Y; }
		/*virtual*/ void		SetPosition(GRID& Grid, const AT::VEC2Di& NewPosition){ m_BBox.UpdateGridOccupation(Grid, m_Position, NewPosition); m_LastPosition = m_Position; m_Position = NewPosition; }
		//---
		AT::VEC2Df				m_DirectonInput;
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------