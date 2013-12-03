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
	XML_STATIC_RESOURCE_MANAGER::XML_STATIC_RESOURCE_MANAGER():m_Stack_Allocator(102400000),m_MarkerAfterInit(0){
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
			RESOURCE* pResource = (RESOURCE*)m_Stack_Allocator.alloc(sizeof(RESOURCE));
			pResource = new(pResource)RESOURCE();
			m_Database_Dictionnary.insert(std::make_pair(node.node().attribute("path").value(), *pResource));
		}
		//--
		m_MarkerAfterInit = m_Stack_Allocator.getMarker();
	}
	//-----------------------------------------------------------------------------
	void XML_STATIC_RESOURCE_MANAGER::AddResrouceToDB(const AT::I8* sResourceName){
		RESOURCE& Resource = *(RESOURCE*)m_Stack_Allocator.alloc(sizeof(RESOURCE));
		Resource = *(RESOURCE*)new(RESOURCE)();
		m_Database_Dictionnary.insert(std::make_pair(sResourceName, Resource));
	}
	//-----------------------------------------------------------------------------
	void* XML_STATIC_RESOURCE_MANAGER::LoadResource(const AT::I8* sResourceName){
		std::unordered_map<std::string, RESOURCE>::iterator it = m_Database_Dictionnary.find(sResourceName);
		if(it == m_Database_Dictionnary.end())
			return NULL;
		RESOURCE& Resource = it->second;
		if(!Resource.m_bInMemory){
			AT::I8 Filename[128];
			sprintf(Filename, "%s", it->first.c_str());
			std::ifstream fstream (Filename, std::ifstream::binary);
			if(!fstream)
				return NULL;
			std::filebuf* fBuffer = fstream.rdbuf();
			std::streamoff size = fBuffer->pubseekoff(0, fstream.end, fstream.in);
			if(m_Stack_Allocator.getFreeSize() < size)
				return NULL;
			fBuffer->pubseekpos(0, fstream.in);
			Resource.m_pBuffer = m_Stack_Allocator.alloc((AT::U32)size);
			fBuffer->sgetn((char*)Resource.m_pBuffer, size-1);
			fstream.close();
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