//-----------------------------------------------------------------------------
//Tools
#include <AcrossTypes.h>
#include <Vec2D.h>
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
namespace AE{
	class ACTOR_INTERFACE_ABC{
		//---------------------------------------------------------------------------
		class WORLD_2DGrid;
		class RENDERER_ABC;
		class R_OBJECT;
		class ANIMATOR_ABC;
		//---------------------------------------------------------------------------
		class ACTOR_ABC{
			friend class DEFAULT_ANIMATOR;
			friend class HOVERING_ANIMATOR;
			friend class SPRITE_ANIMATOR;
		public :
			ACTOR_ABC();
			virtual					~ACTOR_ABC();
			//---
			void					Draw(const WORLD_2DGrid& W, RENDERER_ABC& R, AT::I32F TileSize);
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
			inline void				Destroy(GRID& Grid)																const	{ m_BBox.EraseGridOccupation(Grid, m_Position); }
			//---
			virtual void			Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize)=0;
			virtual void			SetPosition(GRID& Grid, AT::I32 X, AT::I32 Y)=0;
			virtual void			SetPosition(GRID& Grid, const AT::VEC2Di& V)=0;
			//---
			AT::I64F				m_Speed;	//m/s
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
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
