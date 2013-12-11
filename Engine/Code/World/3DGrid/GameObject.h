//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Tools
#include <VEC3D.h>
#include <VEC2D.h>
#include <AcrossTypes.h>
//Engine
#include "../../Rendering/RObject.h"
//-----------------------------------------------------------------------------
namespace AE{
	class RENDERER_ABC;
	class RESOURCE_MANAGER_ABC;
	class GAME_OBJECT{
	public :
		//---
		struct BBOX{
			AT::VEC3Df m_Min;
			AT::VEC3Df m_Max;
			BBOX() :m_Min(0, 0, 0), m_Max(0, 0, 0){}
		};
		//--
		GAME_OBJECT();
		void Init(AT::VEC3Di InitialPos);
		AT::I8 LoadFromResource(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& Manager, const AT::I8* sResourceName);
		//--
		inline AT::VEC2Df GetUVOffset()							const	{ return m_pRObject->m_UVOffset; }
		inline void		  SetUVOffset(AT::I32F X, AT::I32F Y)			{ m_pRObject->m_UVOffset.Set(X, Y); }
	private:
		AT::VEC3Di	m_GridPosition;
		AT::VEC3Df	m_InnerTilePosition;
		BBOX		m_BBox;
		R_OBJECT*	m_pRObject;
	};
}
//-----------------------------------------------------------------------------
