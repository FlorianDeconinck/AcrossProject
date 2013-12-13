//-----------------------------------------------------------------------------
//Engine
#include "World_3DGrid.h"
#include "GameObject.h"
#include "../../Manager_Interface.h"
//Tool
#include <pugixml.hpp>
//std
#include <fstream>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	WORLD_3DGRID::WORLD_3DGRID():m_pRenderer(NULL), m_pResourceManager(NULL), m_DynamicAllocator(52428800){

	}
	//-----------------------------------------------------------------------------
	void WORLD_3DGRID::Init(const AT::I8* sWorldDBFilename, RENDERER_ABC* Renderer, RESOURCE_MANAGER_ABC* pResourceManager){
		m_pRenderer = Renderer;
		m_pResourceManager = pResourceManager;
		m_pResourceManager->InitResourceDB(sWorldDBFilename);
		strcpy(m_sWorldDBFilename, sWorldDBFilename);
	}
	//-----------------------------------------------------------------------------
	void WORLD_3DGRID::Update(AT::I64F elapsedTime_ms, const CONTROLLER& C){
		AT::I32 Count = m_GameObjectPtrs.size();
		for (AT::I32 iGameObj=0 ; iGameObj < Count ; ++iGameObj){
			m_GameObjectPtrs[iGameObj]->Update();
		}
	}
	//-----------------------------------------------------------------------------
	void WORLD_3DGRID::UpdatePreRender(){
	
	}
	//-----------------------------------------------------------------------------
	AT::I32 WORLD_3DGRID::GetPlayerCount() const{
		return 0;
	}
	//-----------------------------------------------------------------------------
	AT::VEC3Df WORLD_3DGRID::GetPlayerWorldPosition(AT::I32 idx) const{
		return AT::VEC3Df(0, 0, 0);
	}
	//-----------------------------------------------------------------------------
#ifdef _DEBUG
	void WORLD_3DGRID::DebugDraw(RENDERER_ABC& R){

	}
#endif
	//-----------------------------------------------------------------------------
	GAME_OBJECT* WORLD_3DGRID::AddGameObject(const AT::I8* sResourceName, AT::VEC3Di InitialPos /*= AT::VEC3Di(0, 0, 0)*/){
		AT::STACK_ALLOCATOR_UNSAFE::Marker Marker = m_DynamicAllocator.getMarker();
		GAME_OBJECT* pGameObject = (GAME_OBJECT*)m_DynamicAllocator.alloc(sizeof(GAME_OBJECT));
		//--
		if (!pGameObject->LoadFromResource(*m_pRenderer, *m_pResourceManager, sResourceName)){
			m_DynamicAllocator.freeToMarker(Marker);
			return NULL;
		}
		pGameObject->Init(InitialPos);
		//--
		m_GameObjectPtrs.push_back(pGameObject);
		return pGameObject;
	}
	//-----------------------------------------------------------------------------
	void WORLD_3DGRID::LoadLevel(const AT::I8* sLevelName){
		//--
		pugi::xml_document doc;
		std::ifstream stream(m_sWorldDBFilename);
		if (!stream.is_open())
			return;
		if (!doc.load(stream))
			return;
		//--
		AT::I8 sQuery[128];
		sprintf(sQuery, ".//level[@name='%s']", sLevelName);
		pugi::xml_node level = doc.select_single_node(sQuery).node();
		for (pugi::xml_node node = level.first_child(); node; node = node.next_sibling()){
			//-- GAME OBJECT
			if (!strcmp(node.name(), "gameobject")){
				pugi::xml_node pos = node.child("position");
				pugi::xml_node resource = node.child("resource");
				AddGameObject(resource.attribute("path").value(), AT::VEC3Di(pos.attribute("x").as_int(), pos.attribute("y").as_int(), pos.attribute("z").as_int()));
				continue;
			}
		}
		stream.close();
	}
	//-----------------------------------------------------------------------------
	void WORLD_3DGRID::UnloadLevel(){
		m_DynamicAllocator.clear();
		m_GameObjectPtrs.clear();
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------