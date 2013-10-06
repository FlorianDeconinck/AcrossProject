//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Engine
#include "./Rendering/RObject.h"
#include "GridBBox.h"
//Tools
#include "Vec2D.h"
#include <CommonTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	class RENDERER;
	class GRID;
	//---------------------------------------------------------------------------
	class ACTOR_ABC{
	public :
							ACTOR_ABC();
		R_OBJECT**			GetRenderingObjects(AT::I32& MeshsCount){ MeshsCount = m_MeshsCount ; return m_Meshs; }
		void				Draw(RENDERER& R, AT::I32F TileSize);
		AT::I8				IsCollisionFreeFromSelf(const GRID& Grid, const AT::VEC2Di PosToTest) const;
		AT::I8				IsCollisionFree(const GRID& Grid, const AT::VEC2Di From, const AT::VEC2Di To) const;
		AT::I8				IsCollisionFree(const GRID& Grid, const AT::VEC2Di Pos) const;
		//---
		void				LoadMeshs(GRID& Grid, RENDERER& R, const AT::I32F* ColorRGBA=NULL);
		//---
		virtual void		Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize)=0;
		virtual void		SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y)=0;
		virtual void		SetPosition(GRID& Grid, const AT::VEC2Di& V)=0;
		AT::VEC2Di			GetPosition(){ return m_Position; }
		//---
		AT::I64F			m_Speed;	//m/s
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
		DIRECTIONS				 m_Direction;
		const static AT::VEC2Di	 m_Dir_North;
		const static AT::VEC2Di	 m_Dir_North_East;
		const static AT::VEC2Di	 m_Dir_East;
		const static AT::VEC2Di	 m_Dir_South_East;
		const static AT::VEC2Di	 m_Dir_South;
		const static AT::VEC2Di	 m_Dir_South_West;
		const static AT::VEC2Di	 m_Dir_West;
		const static AT::VEC2Di	 m_Dir_North_West;
	protected:
		static const AT::I32 MAX_MESHS = 10;
		AT::I32				 m_MeshsCount;
		GRID_BBOX			 m_BBox;
		R_OBJECT*			 m_Meshs[MAX_MESHS];
		AT::VEC2Df			 m_InnerTilePosition; // Vector between 0 - 1
		AT::VEC2Df			 m_PreviousInnerTilePosition; // Vector between 0 - 1
		AT::VEC2Di			 m_Position;
		AT::I32F			 m_AnimationAngle;
	};
	//---------------------------------------------------------------------------
	class NPC:public ACTOR_ABC{
	public:
								NPC();
		//---
		/*virtual*/ void		Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize);
		/*virtual*/ void		SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y){ m_BBox.UpdateGridOccupation(Grid, m_Position, AT::VEC2Di(X, Y)) ; m_Position.x = m_NextMove.x = X; m_Position.y = m_NextMove.y = Y; }
		/*virtual*/ void		SetPosition(GRID& Grid, const AT::VEC2Di& Position){ m_BBox.UpdateGridOccupation(Grid, m_Position, Position); m_Position = m_NextMove = Position; }
		//---
		void		SetDestination(const AT::VEC2Di& Destination){ m_Destination = Destination; m_bRecomputePath = true; }
					AT::I8		IsAtDestination(){ return m_Position == m_Destination; }
		//---
		static const AT::I32 PATH_MAX_NODES = 1024;
		//---
		AT::VEC2Di				m_Path[1024];
		AT::U32					m_iPath;
		AT::U32					m_sizePath;
		//---
		AT::VEC2Di				m_NextMove;
		AT::VEC2Di				m_Destination;
		AT::I8					m_bRecomputePath;
	};
	//---------------------------------------------------------------------------
	class PLAYER:public NPC{
		public:
								PLAYER();
		/*virtual*/ void		Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize);
		/*virtual*/ void		SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y){ m_BBox.UpdateGridOccupation(Grid, m_Position, AT::VEC2Di(X, Y)) ; m_Position.x = X; m_Position.y = Y; }
		/*virtual*/ void		SetPosition(GRID& Grid, const AT::VEC2Di& Position){ m_BBox.UpdateGridOccupation(Grid, m_Position, Position); m_Position = Position; }
		//---
		AT::VEC2Df				m_DirectonInput;
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------