#pragma once
//-----------------------------------------------------------------------------
//Tool
#include <CommonTypes.h>
//-----------------------------------------------------------------------------
// Resource manager has to be initialized with the resource database name.
//  Database format and stocking is up to the child Resource manager
//-----------------------------------------------------------------------------
namespace AE{
	class RESOURCE_MANAGER_ABC{
	public:
						RESOURCE_MANAGER_ABC(){}
		virtual			~RESOURCE_MANAGER_ABC(){}
		virtual void	InitResourceDB(const AT::I8* sResourceDataBaseName) = 0;
		virtual void*	LoadResource(const AT::I8* sResourceName) = 0;
		virtual void	ResetAllLevels() = 0;
	};
	class RESOURCE{
	public :
				RESOURCE():m_pBuffer(NULL),m_bInMemory(false){}
		void*	m_pBuffer;
		AT::I8	m_bInMemory;
	};
}//namespace AE
//-----------------------------------------------------------------------------