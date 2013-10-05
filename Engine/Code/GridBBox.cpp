//---------------------------------------------------------------------------
//Project
#include "GridBBox.h"
#include "World.h"
//---------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------
	void GRID_BBOX::Init(GRID& Grid, const AT::VEC2Di BBoxCenter, AT::U32 _HalfWidth, AT::U32 _HalfHeight){
		m_HalfWidth = _HalfWidth;
		m_HalfHeight = _HalfHeight;
		//Update grid occupation
		for(AT::I32 iH = -m_HalfHeight ; iH <= m_HalfHeight; iH++){
			for(AT::I32 iW = -m_HalfWidth ; iW <= m_HalfWidth; iW++){
				AT::I32 X = BBoxCenter.x+iW;
				X = X < 0 ? 0 : X >= Grid.m_nMapWidth ? Grid.m_nMapWidth-1 : X ;
				AT::I32 Y = BBoxCenter.y+iH;
				Y = Y < 0 ? 0 : Y >= Grid.m_nMapHeight ? Grid.m_nMapHeight-1 : Y;
				Grid.SetTile(X, Y, GRID::STATIC_OBSTACLE);
			}
		}
	}
	//-----------------------------------------------------------------------
	void GRID_BBOX::UpdateTileStatus(GRID& Grid, const AT::VEC2Di& FromBottomLeft, const AT::VEC2Di& FromTopRight, const AT::VEC2Di& ToBottomLeft, const AT::VEC2Di& ToTopRight, AT::I32 Status) const{
		AT::I32 Width = 0;
		AT::I32 Height = 0;
		//------
		// X
		if(ToTopRight.x > FromTopRight.x){
			Width = ToTopRight.x - FromTopRight.x + 1;
			Height = 2*m_HalfHeight+1 + ToBottomLeft.y >= Grid.m_nMapHeight ? Grid.m_nMapHeight-ToBottomLeft.y : (ToTopRight.y >= 2*m_HalfHeight+1 ? 2*m_HalfHeight+1 : ToTopRight.y);
			for(AT::I32 iH = 0 ; iH < Height; iH++)
				for(AT::I32 iW = 0 ; iW < Width; iW++)
					Grid.SetTile(FromTopRight.x+iW, ToBottomLeft.y+iH, (GRID::MAP_TAG)Status);
		}
		//--
		if(ToBottomLeft.x < FromBottomLeft.x){
			Width = FromBottomLeft.x - ToBottomLeft.x + 1;
			Height = 2*m_HalfHeight+1 + ToBottomLeft.y >= Grid.m_nMapHeight ? Grid.m_nMapHeight-ToBottomLeft.y : (ToTopRight.y >= 2*m_HalfHeight+1 ? 2*m_HalfHeight+1 : ToTopRight.y);
			for(AT::I32 iH = 0 ; iH < Height; iH++)
				for(AT::I32 iW = 0 ; iW < Width; iW++)
					Grid.SetTile(ToBottomLeft.x+iW, ToBottomLeft.y+iH, (GRID::MAP_TAG)Status);
		}
		//------
		// Y
		if(ToTopRight.y > FromTopRight.y){
			Width = 2*m_HalfWidth+1 + ToBottomLeft.x >= Grid.m_nMapWidth ? Grid.m_nMapWidth - ToBottomLeft.x : (ToTopRight.x >= 2*m_HalfWidth+1 ? 2*m_HalfWidth+1 : ToTopRight.x);
			Height =  ToTopRight.y - FromTopRight.y + 1;
			for(AT::I32 iH = 0 ; iH < Height; iH++)
				for(AT::I32 iW = 0 ; iW < Width; iW++)
					Grid.SetTile(ToBottomLeft.x+iW, FromTopRight.y+iH, (GRID::MAP_TAG)Status);
		}
		//--
		if(ToBottomLeft.y < FromBottomLeft.y){
			Width = 2*m_HalfWidth+1 + ToBottomLeft.x >= Grid.m_nMapWidth ? Grid.m_nMapWidth - ToBottomLeft.x : (ToTopRight.x >= 2*m_HalfWidth+1 ? 2*m_HalfWidth+1 : ToTopRight.x);
			Height = FromBottomLeft.y - ToBottomLeft.y + 1;
			for(AT::I32 iH = 0 ; iH < Height; iH++)
				for(AT::I32 iW = 0 ; iW < Width; iW++)
					Grid.SetTile(ToBottomLeft.x+iW, ToBottomLeft.y+iH, (GRID::MAP_TAG)Status);
		}

	}
	void GRID_BBOX::UpdateGridOccupation(GRID& Grid, const AT::VEC2Di& NewBBoxCenter, const AT::VEC2Di& PreviousBBoxCenter) const{
	//-----------------------------------------------------------------------
#if 1 //Brute force : wipe old position, fill new one
		for(AT::I32 iH = -m_HalfHeight ; iH <= m_HalfHeight; iH++){
			for(AT::I32 iW = -m_HalfWidth ; iW <= m_HalfWidth; iW++){
				AT::I32 X = PreviousBBoxCenter.x+iW;
				X = X < 0 ? 0 : X >= Grid.m_nMapWidth ? Grid.m_nMapWidth-1 : X ;
				AT::I32 Y = PreviousBBoxCenter.y+iH;
				Y = Y < 0 ? 0 : Y >= Grid.m_nMapHeight ? Grid.m_nMapHeight-1 : Y;
				if(Grid.GetTile(X, Y) == GRID::STATIC_OBSTACLE)
					Grid.SetTile(X, Y, GRID::WALKABLE);
			}
		}
		for(AT::I32 iH = -m_HalfHeight ; iH <= m_HalfHeight; iH++){
			for(AT::I32 iW = -m_HalfWidth ; iW <= m_HalfWidth; iW++){
				AT::I32 X = NewBBoxCenter.x+iW;
				X = X < 0 ? 0 : X >= Grid.m_nMapWidth ? Grid.m_nMapWidth-1 : X ;
				AT::I32 Y = NewBBoxCenter.y+iH;
				Y = Y < 0 ? 0 : Y >= Grid.m_nMapHeight ? Grid.m_nMapHeight-1 : Y;
				if(Grid.GetTile(X, Y) == GRID::WALKABLE)
					Grid.SetTile(X, Y, GRID::STATIC_OBSTACLE);
			}
		}
#else //Cleverer : compute only affected tiles (NOT WORKING)
		AT::VEC2Di NewBBoxBottomLeft(	NewBBoxCenter.x < m_HalfWidth ? 0 : NewBBoxCenter.x - m_HalfWidth,
										NewBBoxCenter.y < m_HalfHeight ? 0 : NewBBoxCenter.y - m_HalfHeight);
		
		AT::VEC2Di NewBBoxTopRight(		NewBBoxCenter.x + m_HalfWidth >= Grid.m_nMapWidth ? Grid.m_nMapWidth-1 : NewBBoxCenter.x + m_HalfWidth,
										NewBBoxCenter.y + m_HalfHeight >= Grid.m_nMapHeight ? Grid.m_nMapHeight-1 : NewBBoxCenter.y + m_HalfHeight);
	
		AT::VEC2Di PreviousBBoxBottomLeft(	PreviousBBoxCenter.x < m_HalfWidth ? 0 : PreviousBBoxCenter.x - m_HalfWidth, 
											PreviousBBoxCenter.y < m_HalfHeight ? 0 : PreviousBBoxCenter.y - m_HalfHeight);
		
		AT::VEC2Di PreviousBBoxTopRight(	PreviousBBoxCenter.x + m_HalfWidth >= Grid.m_nMapWidth ? Grid.m_nMapWidth-1 : PreviousBBoxCenter.x + m_HalfWidth, 
											PreviousBBoxCenter.y + m_HalfHeight >= Grid.m_nMapHeight ? Grid.m_nMapHeight-1 : PreviousBBoxCenter.y + m_HalfHeight);
		//---
		UpdateTileStatus(Grid, PreviousBBoxBottomLeft, PreviousBBoxTopRight, NewBBoxBottomLeft, NewBBoxTopRight, GRID::STATIC_OBSTACLE);
		UpdateTileStatus(Grid, NewBBoxBottomLeft, NewBBoxTopRight, PreviousBBoxBottomLeft, PreviousBBoxTopRight, GRID::WALKABLE);
#endif
	}
	//-----------------------------------------------------------------------
	AT::I8 GRID_BBOX::IsCollisionFree(const GRID& Grid, const AT::VEC2Di& NewBBoxCenter) const{
		for(AT::I32 iH = -(AT::I32)m_HalfHeight ; iH <= (AT::I32)m_HalfHeight; iH++){
			for(AT::I32 iW = -(AT::I32)m_HalfWidth ; iW <= (AT::I32)m_HalfWidth; iW++){
				if(!Grid.IsTileWalkable(NewBBoxCenter.x+iW, NewBBoxCenter.y+iH))
					return false;
			}
		}
		return true;
	}
	//-----------------------------------------------------------------------
	AT::I8 GRID_BBOX::IsCollisionFree(const GRID& Grid, const AT::VEC2Di& OldBBoxCenter, const AT::VEC2Di& NewBBoxCenter) const{
		for(AT::I32 iH = -(AT::I32)m_HalfHeight ; iH <= (AT::I32)m_HalfHeight; iH++){
			for(AT::I32 iW = -(AT::I32)m_HalfWidth ; iW <= (AT::I32)m_HalfWidth; iW++){
				if(!Grid.IsTileWalkable(NewBBoxCenter.x+iW, NewBBoxCenter.y+iH) && !IsInside(OldBBoxCenter, NewBBoxCenter.x+iW, NewBBoxCenter.y+iH))
					return false;
			}
		}
		return true;
	}
	//-----------------------------------------------------------------------
	inline AT::I8 GRID_BBOX::IsInside(const AT::VEC2Di& BBoxCenter, const AT::I32& TileX, const AT::I32& TileY) const{
		return BBoxCenter.x-m_HalfWidth <= TileX && BBoxCenter.x+m_HalfWidth >= TileX && BBoxCenter.y-m_HalfHeight <= TileY && BBoxCenter.y+m_HalfHeight >= TileY;
	}
	inline AT::I8 GRID_BBOX::IsInside(const AT::VEC2Di& BBoxCenter, const AT::VEC2Di& Tile) const{
		IsInside(BBoxCenter, Tile.x, Tile.y);
	}
	//-----------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------