//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include "../GameObject.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	class LIGHT;
	class RESOURCE_MANAGER_ABC;
	class GO_LIGHT :public GAME_OBJECT{
	public :
		GO_LIGHT();
		//-----
		void	Init(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& Manager, const AT::I8* sResourceName, AT::VEC3Df InitialPos, AT::I32F Radius, const GLfloat Diffuse[4]);
		//-----
		/*virtual*/	void	Update(AT::I64F elapsedTime_ms);
		//-----
	private:
		LIGHT* m_pLight;
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
