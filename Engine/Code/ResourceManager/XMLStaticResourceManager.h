#pragma once
//---------------------------------------------------------------------------
//Project
#include "../Manager_Interface.h"
//Tools
#include <StackAllocator.h>
//STD
#include <unordered_map>
#include <string>
//---------------------------------------------------------------------------
namespace AE{
	class XML_STATIC_RESOURCE_MANAGER:public RESOURCE_MANAGER_ABC, public AT::SINGLETON<XML_STATIC_RESOURCE_MANAGER>{
		friend class AT::SINGLETON<XML_STATIC_RESOURCE_MANAGER>;
	public:
							 XML_STATIC_RESOURCE_MANAGER();
							~XML_STATIC_RESOURCE_MANAGER(){}
	public:
		/*virtual*/ void	InitResourceDB(const AT::I8* sResourceDataBaseName);
		/*virtual*/ void	AddResrouceToDB(const AT::I8* sResourceName);
		/*virtual*/ void*	LoadResource(const AT::I8* sResourceName);		
		/*virtual*/ void	ResetAllLevels();
	private:
		//---
		typedef std::string RESOURCE_NAME_t;
		//---
		AT::STACK_ALLOCATOR_SAFE::Marker				m_MarkerAfterInit;
		AT::STACK_ALLOCATOR_SAFE						m_Stack_Allocator; //allocation done in cstrctor (~100Mo)
		std::unordered_map<RESOURCE_NAME_t, RESOURCE>	m_Database_Dictionnary;

	};
}//namespace AE
//---------------------------------------------------------------------------
