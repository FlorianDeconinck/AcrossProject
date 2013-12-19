//-----------------------------------------------------------------------------
//Engine
#include "World_3DGrid.h"
#include "GameObject.h"
#include "./GameObjects/GO_Light.h"
#include "../../Manager_Interface.h"
#include "../../Renderer_Interface.h"
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
			GAME_OBJECT& GObject = *m_GameObjectPtrs[iGameObj];
			GObject.Update(elapsedTime_ms);
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
	GAME_OBJECT* WORLD_3DGRID::AddGameObject(const AT::I8* sResourceName, AT::VEC3Df InitialPos /*= AT::VEC3Df(0, 0, 0)*/){
 		AT::STACK_ALLOCATOR_UNSAFE::Marker Marker = m_DynamicAllocator.getMarker();
 		GAME_OBJECT* pGameObject = (GAME_OBJECT*)m_DynamicAllocator.alloc(sizeof(GAME_OBJECT));
		pGameObject = (GAME_OBJECT*)new(pGameObject)GAME_OBJECT();
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
	GAME_OBJECT* WORLD_3DGRID::AddGameObject_Light(const AT::I8* sResourceName, AT::VEC3Df InitialPos /*= AT::VEC3Df(0, 0, 0)*/, AT::I32F Radius/* = 1.f*/, const GLfloat Diffuse[4] /*= { 1.f, 1.f, 1.f, 0.7f }*/){
		AT::STACK_ALLOCATOR_UNSAFE::Marker Marker = m_DynamicAllocator.getMarker();
		GO_LIGHT* pGameObject = (GO_LIGHT*)m_DynamicAllocator.alloc(sizeof(GO_LIGHT));
		pGameObject = (GO_LIGHT*)new(pGameObject)GO_LIGHT();
		//--
// 		if (!pGameObject->LoadFromResource(*m_pRenderer, *m_pResourceManager, sResourceName)){
// 			m_DynamicAllocator.freeToMarker(Marker);
// 			return NULL;
// 		}
		pGameObject->Init(*m_pRenderer, *m_pResourceManager, sResourceName, InitialPos, Radius, Diffuse);
		
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
				AddGameObject(resource.attribute("path").value(), AT::VEC3Df(pos.attribute("x").as_float(), pos.attribute("y").as_float(), pos.attribute("z").as_float()));
				continue;
			}
			//-- LIGHT
			if (!strcmp(node.name(), "light")){
				//--
				pugi::xml_node resourceNode = node.child("resource");
				AT::I8 bResourceExist = !resourceNode.empty();
				//--
				GLfloat Diffuse[4];
				pugi::xml_node diffuseNode = node.child("diffuse");
				Diffuse[0] = diffuseNode.attribute("r").as_float();
				Diffuse[1] = diffuseNode.attribute("g").as_float();
				Diffuse[2] = diffuseNode.attribute("b").as_float();
				Diffuse[3] = diffuseNode.attribute("i").as_float();
				//--
				GLfloat Specular[4];
				pugi::xml_node specularNode = node.child("specular");
				Specular[0] = specularNode.attribute("r").as_float();
				Specular[1] = specularNode.attribute("g").as_float();
				Specular[2] = specularNode.attribute("b").as_float();
				Specular[3] = specularNode.attribute("i").as_float();
				if (!strcmp(node.attribute("type").value(), "DIRECTIONAL")){
					AT::VEC3Df Direction;
					pugi::xml_node directionNode = node.child("direction");
					Direction.x = directionNode.attribute("x").as_float();
					Direction.y = directionNode.attribute("y").as_float();
					Direction.z = directionNode.attribute("z").as_float();
					//--
					m_pRenderer->AddLight(RENDERER_ABC::RENDERER_LIGHT_DIRECTIONAL, Diffuse, Specular, Direction);
				}
				else if (!strcmp(node.attribute("type").value(), "POINT")){
					AT::VEC3Df Position;
					pugi::xml_node positionNode = node.child("position");
					Position.x = positionNode.attribute("x").as_float();
					Position.y = positionNode.attribute("y").as_float();
					Position.z = positionNode.attribute("z").as_float();
					pugi::xml_node radiusNode = node.child("radius");
					AT::I32F Radius = radiusNode.text().as_float();
					//--
					if (bResourceExist)
						AddGameObject_Light(resourceNode.attribute("path").value(), Position, Radius, Diffuse);
					else
						m_pRenderer->AddLight(RENDERER_ABC::RENDERER_LIGHT_POINT, Diffuse, Specular, Position, Radius);
				}

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