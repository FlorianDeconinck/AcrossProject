//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include "../../World_Interface.h"
#include "../../Rendering/DeferredRendering/LightColors.h"
//Tools
#include <Singleton.h>
#include <Vec3D.h>
#include <StackAllocator.h>
//STL
#include <vector>
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//-----------------------------------------------------------------------------
namespace AE{
	class GAME_OBJECT;
	class WORLD_3DGRID :public WORLD_ABC, public AT::SINGLETON<WORLD_3DGRID>{
		friend class AT::SINGLETON<WORLD_3DGRID>;
	protected :
		WORLD_3DGRID();
		~WORLD_3DGRID(){}
	public:
		//--
		/*virtual*/ void		Init(const AT::I8* sWorldDBFilename, RENDERER_ABC* Renderer, RESOURCE_MANAGER_ABC* pResourceManager);
		/*virtual*/ void		Update(AT::I64F elapsedTime_ms, const CONTROLLER& C);
		//--
		/*virtual*/ void		UpdatePreRender();
		//--
		/*virtual*/ AT::I32		GetPlayerCount() const;
		/*virtual*/ AT::VEC3Df	GetPlayerWorldPosition(AT::I32 idx) const;
#ifdef _DEBUG
		/*virtual*/ void		DebugDraw(RENDERER_ABC& R);
#endif
		//--
		GAME_OBJECT*			AddGameObject(const AT::I8* sResourceName, AT::VEC3Df InitialPos = AT::VEC3Df(0, 0, 0));
		GAME_OBJECT*			AddGameObject_Light(const AT::I8* sResourceName, AT::VEC3Df InitialPos = AT::VEC3Df(0, 0, 0), AT::I32F Radius = 1.f, const GLfloat Diffuse[4] = DPL::DiffuseWhite);
		void					LoadLevel(const AT::I8* sLevelName);
		void					UnloadLevel();
		//--
	private:
		RENDERER_ABC*				m_pRenderer;
		RESOURCE_MANAGER_ABC*		m_pResourceManager;
		AT::STACK_ALLOCATOR_UNSAFE	m_DynamicAllocator; //~50Mo
		std::vector<GAME_OBJECT*>	m_GameObjectPtrs;
		AT::I8						m_sWorldDBFilename[256];
	};
}//namespace AE
//-----------------------------------------------------------------------------