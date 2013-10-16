//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tools
#include "Vec2D.h"
#include <CommonTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	class GRID;
	class GRID_BBOX{
		//-------
		// Grid aligned
		// Defined by it's half-width & half-height
		//-------
		friend class ACTOR_ABC;
	protected :
		AT::I32 m_HalfWidth;
		AT::I32 m_HalfHeight;
		void UpdateTileStatus(GRID& Grid, const AT::VEC2Di& FromBottomLeft, const AT::VEC2Di& FromTopRight, const AT::VEC2Di& ToBottomLeft, const AT::VEC2Di& ToTopRight, AT::I32 Status) const;
	public :
		void Init(GRID& Grid, const AT::VEC2Di BBoxCenter, AT::U32 HalfWidth, AT::U32 HalfHeight);
		void UpdateGridOccupation(GRID& Grid, const AT::VEC2Di& PreviousBBoxCenter, const AT::VEC2Di& NewBBoxCenter) const;
		void EraseGridOccupation(GRID& Grid, const AT::VEC2Di& BBoxCenter) const;
		void BuildGridOccupation(GRID& Grid, const AT::VEC2Di& BBoxCenter) const;
		AT::I8 IsCollisionFree(const GRID& Grid, const AT::VEC2Di& NewBBoxCenter) const;
		AT::I8 IsCollisionFree(const GRID& Grid, const AT::VEC2Di& OldBBoxCenter, const AT::VEC2Di& NewBBoxCenter) const;
		AT::I8 IsInside(const AT::VEC2Di& BBoxCenter, const AT::VEC2Di& Tile) const;
		AT::I8 IsInside(const AT::VEC2Di& BBoxCenter, const AT::I32& TileX, const AT::I32& TileY) const;
	};
}
//---------------------------------------------------------------------------