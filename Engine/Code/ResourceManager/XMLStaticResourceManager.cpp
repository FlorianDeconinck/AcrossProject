//-----------------------------------------------------------------------------
//Project
#include "XMLStaticResourceManager.h"
//Tool
#include <pugixml.hpp>
//STD
#include <utility>
#include <fstream>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	struct pred_ressource{
		bool operator()(pugi::xml_node node)const{
			return !strcmp(node.name(), "resource");
		}
	};
	//-----------------------------------------------------------------------------
	XML_STATIC_RESOURCE_MANAGER::XML_STATIC_RESOURCE_MANAGER():m_Stack_Allocator(100000),m_MarkerAfterInit(0){
	}
	//-----------------------------------------------------------------------------
	void XML_STATIC_RESOURCE_MANAGER::InitResourceDB(const AT::I8* sResourceDataBaseName){
		pugi::xml_document databaseDoc;
		std::ifstream stream(sResourceDataBaseName);
		pugi::xml_parse_result result = databaseDoc.load(stream);
		if(!result)
			return;
		//--
		pugi::xpath_node_set nodes = databaseDoc.select_nodes(".//resource");
		for (pugi::xpath_node_set::const_iterator it = nodes.begin(); it != nodes.end(); ++it){
			pugi::xpath_node node = *it;
			RESOURCE& Resource = *(RESOURCE*)m_Stack_Allocator.alloc(sizeof(RESOURCE));
			Resource = *(RESOURCE*)new(RESOURCE)();
			m_Database_Dictionnary.insert(std::make_pair(node.node().attribute("path").value(), Resource));
		}
		//--
		m_MarkerAfterInit = m_Stack_Allocator.getMarker();
	}
	//-----------------------------------------------------------------------------
	void* XML_STATIC_RESOURCE_MANAGER::LoadResource(const AT::I8* sResourceName){
		std::unordered_map<std::string, RESOURCE>::iterator it = m_Database_Dictionnary.find(sResourceName);
		if(it == m_Database_Dictionnary.end())
			return NULL;
		RESOURCE& Resource = it->second;
		if(!Resource.m_bInMemory){
			AT::I8 Filename[128];
			sprintf(Filename, "../../../Asset/%s", it->first.c_str());
			FILE* pFile = fopen(Filename, "rb");
			if(!pFile)
				return NULL;
			fseek(pFile, 0, SEEK_END);
			AT::I32 bytes_size = ftell(pFile);
			Resource.m_pBuffer = m_Stack_Allocator.alloc(bytes_size);
			rewind(pFile);
			fread(Resource.m_pBuffer, bytes_size, 1, pFile);
			fclose(pFile);
			Resource.m_bInMemory = true;
		}
		return Resource.m_pBuffer;
	}
	//-----------------------------------------------------------------------------
	void XML_STATIC_RESOURCE_MANAGER::ResetAllLevels(){
		m_Stack_Allocator.freeToMarker(m_MarkerAfterInit);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------