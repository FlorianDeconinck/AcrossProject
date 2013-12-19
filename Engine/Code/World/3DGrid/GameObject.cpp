//-----------------------------------------------------------------------------
//Engine
#include "GameObject.h"
#include "../../Manager_Interface.h"
#include "../../Renderer_Interface.h"
//STD
#include <math.h>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	GAME_OBJECT::GAME_OBJECT() :m_GridPosition(0, 0, 0), m_InnerTilePosition(0.5f, 0.5f, 0.5f){

	}
	//-----------------------------------------------------------------------------
	void GAME_OBJECT::Init(AT::VEC3Df InitialPos){
		//--
		AT::I32F intPart;
		m_InnerTilePosition.x = modf(InitialPos.x, &intPart);
		m_GridPosition.x = (AT::I32)intPart;
		m_InnerTilePosition.y = modf(InitialPos.y, &intPart);
		m_GridPosition.y = (AT::I32)intPart;
		m_InnerTilePosition.z = modf(InitialPos.z, &intPart);
		m_GridPosition.z = (AT::I32)intPart;
		//--
	}
	//-----------------------------------------------------------------------------
	void GAME_OBJECT::Update(AT::I64F elapsedTime_ms){
		m_pRObject->m_trfModel.SetT(AT::VEC3Df(m_GridPosition.x + m_InnerTilePosition.x, m_GridPosition.y + m_InnerTilePosition.y, m_GridPosition.z + m_InnerTilePosition.z));
	}
	//-----------------------------------------------------------------------------
	AT::I8 GAME_OBJECT::LoadFromResource(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& Manager, const AT::I8* sResourceName, SHADER_ABC::SHADERS_ID ShaderID /*= SHADER_ABC::NO_SHADERS*/){
		m_pRObject = Renderer.CreateRObject(Manager, sResourceName, m_BBox.m_Min, m_BBox.m_Max, ShaderID);
		return m_pRObject != NULL;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
