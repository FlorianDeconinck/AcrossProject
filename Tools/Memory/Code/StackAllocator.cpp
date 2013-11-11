//STD
#include <stdlib.h>
#include <cstdio>
#include <assert.h>
//Project
#include "StackAllocator.h"
//Tools
//-----------------------------------------------------------------
namespace AT{
	template<U8 bSAFE_MODE>
	STACK_ALLOCATOR<bSAFE_MODE>::STACK_ALLOCATOR(U32 stackSize_bytes, STACK_ALLOCATOR_ERROR* pError){
		if(pError && stackSize_bytes<=0)
			*pError = STACK_ALLOCATOR_ERROR::ZERO_BYTES_ALLOCATOR;
		m_pBuffer = (I8*)malloc(stackSize_bytes);
		m_stackSize_bytes = stackSize_bytes;
		m_stackTop_marker = 0;
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	STACK_ALLOCATOR<bSAFE_MODE>::~STACK_ALLOCATOR(){
		assert(m_stackTop_marker <= m_stackSize_bytes);
		free(m_pBuffer);
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	void* STACK_ALLOCATOR<bSAFE_MODE>::alloc(U32 size_bytes, typename STACK_ALLOCATOR<bSAFE_MODE>::STACK_ALLOCATOR_ERROR* pErr){
		//-----
		//Safe mode tests
		if(bSAFE_MODE){
			if(!size_bytes && pErr){
				*pErr = STACK_ALLOCATOR_ERROR::ZERO_BYTES_ALLOCATOR;
				return NULL;
			}
			if(size_bytes+m_stackTop_marker > m_stackSize_bytes && pErr){
				*pErr = STACK_ALLOCATOR_ERROR::OUT_OF_BOUND;
				return NULL;
			}
		}
		//-----
		//Allocation from top marker
		if(bSAFE_MODE && pErr)
			*pErr = STACK_ALLOCATOR_ERROR::NO_STACK_ALLOCATOR_ERROR;
		Marker newMakerAlloc = m_stackTop_marker;
		m_stackTop_marker += size_bytes;
		return m_pBuffer+newMakerAlloc;
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	typename STACK_ALLOCATOR<bSAFE_MODE>::Marker STACK_ALLOCATOR<bSAFE_MODE>::getMarker(){
		return m_stackTop_marker;
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	U32 STACK_ALLOCATOR<bSAFE_MODE>::getFreeSize(){
		return m_stackSize_bytes - m_stackTop_marker;
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	void STACK_ALLOCATOR<bSAFE_MODE>::freeToMarker(Marker marker){
		m_stackTop_marker = marker > 0 ? marker : 0;
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	void STACK_ALLOCATOR<bSAFE_MODE>::clear(){
		m_stackTop_marker = 0;
	}
	//-----------------------------------------------------------------
	template<U8 bSAFE_MODE>
	char* STACK_ALLOCATOR<bSAFE_MODE>::printError(STACK_ALLOCATOR_ERROR err){
		switch (err){
			case NO_STACK_ALLOCATOR_ERROR :
				return "AT::StackAllocator : no error";
				break;
			case OUT_OF_BOUND :
				return"AT::StackAllocator : out-of-bound";
				break;
			case ZERO_BYTES_ALLOCATOR :
				return "AT::StackAllocator : zero-byte sized allocator";
				break;
			case UNSAFE_MODE :
				return "AT::StackAllocator : unsafe mode";
			default:
				return "AT::StackAllocator : Unknown error";
				break;
		}
	}
	//-----------------------------------------------------------------
	template class STACK_ALLOCATOR<true>;
	template class STACK_ALLOCATOR<false>;
}//namespace AT
//-----------------------------------------------------------------

