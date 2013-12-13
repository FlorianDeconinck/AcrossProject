//-----------------------------------------------------------------------------
//Engine
#include "GameObject.h"
#include "../../Manager_Interface.h"
#include "../../Renderer_Interface.h"
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	GAME_OBJECT::GAME_OBJECT() :m_GridPosition(0, 0, 0), m_InnerTilePosition(0.5f, 0.5f, 0.5f){

	}
	//-----------------------------------------------------------------------------
	void GAME_OBJECT::Init(AT::VEC3Di InitialPos){
		m_GridPosition = InitialPos;
		m_InnerTilePosition.Set(0.5f, 0.5f, 0.5f);
	}
	//-----------------------------------------------------------------------------
	void GAME_OBJECT::Update(){
		m_pRObject->m_trfModel.SetT(m_GridPosition);
	}
	//-----------------------------------------------------------------------------
	AT::I8 GAME_OBJECT::LoadFromResource(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& Manager, const AT::I8* sResourceName){
		m_pRObject = Renderer.CreateRObject(Manager, sResourceName, m_BBox.m_Min, m_BBox.m_Max);
		return m_pRObject != NULL;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
