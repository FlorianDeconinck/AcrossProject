//-----------------------------------------------------------------------------
//Tools
#include <AcrossTypes.h>
#include <Singleton.h>
#include <Vec3D.h>
//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class RENDERER_ABC;
	class RESOURCE_MANAGER_ABC;
	class CONTROLLER;
	class WORLD_ABC{
	public:
		//--
		WORLD_ABC(){}
		virtual ~WORLD_ABC(){}
		//--
		virtual void		Init(const AT::I8* sWorldDBFilename, RENDERER_ABC* R, RESOURCE_MANAGER_ABC* pResourceManager)=0;
		virtual void		Update(AT::I64F elapsedTime_ms, const CONTROLLER& C)=0;
		//--
		virtual void		UpdatePreRender()=0;
		//--
		virtual AT::I32		GetPlayerCount() const = 0;
		virtual AT::VEC3Df	GetPlayerWorldPosition(AT::I32 idx) const = 0;
#ifdef _DEBUG
		virtual void		DebugDraw(RENDERER_ABC& R)=0;
#endif
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------