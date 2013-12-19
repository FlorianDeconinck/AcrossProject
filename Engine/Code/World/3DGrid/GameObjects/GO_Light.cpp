//-----------------------------------------------------------------------------
//Engine
#include "GO_Light.h"
#include "../../../Renderer_Interface.h"
#include "../../../Rendering/DeferredRendering/Light.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	GO_LIGHT::GO_LIGHT(){
	}
	//-------------------------------------------------------------------------
	void GO_LIGHT::Init(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& Manager, const AT::I8* sResourceName, AT::VEC3Df InitialPos, AT::I32F Radius, const GLfloat Diffuse[4]){
		GAME_OBJECT::Init(InitialPos);
		//--
		LoadFromResource(Renderer, Manager, sResourceName, SHADER_ABC::DEFFERED_LIGHT_FAKE_EMIT_SHADER);
		//--
		m_pLight = Renderer.AddLight(RENDERER_ABC::RENDERER_LIGHT_POINT, Diffuse, Diffuse, InitialPos, Radius);
		m_pRObject->m_pLight = m_pLight;
		//--
	}
	//-------------------------------------------------------------------------
	static AT::I32 iLight = 0;
	void GO_LIGHT::Update(AT::I64F elapsedTime_ms){
		//--
		AT::VEC3Df WorldPos(m_GridPosition.x + m_InnerTilePosition.x, m_GridPosition.y + m_InnerTilePosition.y, m_GridPosition.z + m_InnerTilePosition.z);
		//--
		AT::I32F x = WorldPos.x;
		AT::I32F y = WorldPos.y;
		AT::I32F z = WorldPos.z;
		static const AT::I32F angularspeed = 1.f;
		AT::I32F angle = ((AT::I32F)elapsedTime_ms / 1000.f)*angularspeed;
		//--
		AT::I32F c = cos(angle);
		AT::I32F s = sin(angle);
		//--
		if (iLight < 16){
			WorldPos.x = x*c - z*s;
			WorldPos.z = x*s + z*c;
			//--
		}else{
			WorldPos.y= y*c - z*s;
 			WorldPos.z = y*s + z*c;
		}
		AT::I32F intPart;
		m_InnerTilePosition.x = modf(WorldPos.x, &intPart);
		m_GridPosition.x = (AT::I32)intPart;
		m_InnerTilePosition.y = modf(WorldPos.y, &intPart);
		m_GridPosition.y = (AT::I32)intPart;
		m_InnerTilePosition.z = modf(WorldPos.z, &intPart);
		m_GridPosition.z = (AT::I32)intPart;
		//--
		m_pLight->m_PositionOrDirection = WorldPos;
		m_pRObject->m_trfModel.SetT(WorldPos);
		//--
		iLight = (iLight+1)%32;
	}
	//-------------------------------------------------------------------------

}//namespace AE
//-----------------------------------------------------------------------------