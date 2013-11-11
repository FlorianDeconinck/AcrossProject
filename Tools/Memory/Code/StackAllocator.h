//----------------------------------------------------------
// A pretty basic stack-like allocator for runtime heap allocation. No reallocation will be done in either SAFE
// or UNSAFE mode. SAFE mode will refuse the allocation ans send back an error.s
//  Parameters :
//   bSAFE_MODE = test will be made on out-of-bound call. Default is false, for speed purposes.
//
// IMPORTANT : Implementation is based on Jason Gregory's "Game Engine Architecture" book
//             page 205 and following. (Go read it. Now. I'll wait here.)
//             The book quote for more insight http://www.swedishcoding.com/2008/08/31/are-we-out-of-memory
//----------------------------------------------------------

#ifndef __STACK_ALLOCATOR_H__
#define __STACK_ALLOCATOR_H__
//-----------------------------------------------------------------
#include <CommonTypes.h>
//-----------------------------------------------------------------
namespace AT{
	template<U8 bSAFE_MODE=true> class STACK_ALLOCATOR{
	public:
			// Stack marker: Represents the current top of the
			// stack. You can only roll back to a marker, not to
			// arbitrary locations within the stack.
			typedef U32 Marker;
			// Errors for the allocator
			// 
			enum STACK_ALLOCATOR_ERROR{
				UNSAFE_MODE=0,
				NO_STACK_ALLOCATOR_ERROR,
				OUT_OF_BOUND,
				ZERO_BYTES_ALLOCATOR,
				//--
				STACK_ALLOCATOR_ERROR_COUNT
			};
			// Constructs a stack allocator with the given total
			// size.
			explicit STACK_ALLOCATOR(U32 stackSize_bytes, STACK_ALLOCATOR_ERROR* pError=NULL);
			~STACK_ALLOCATOR();
			// Allocates a new block of the given size from stack
			// top.
			void* alloc(U32 size_bytes, STACK_ALLOCATOR_ERROR* pErr=NULL);
			// Returns a marker to the current stack top. 
			Marker getMarker();
			// Get size left on the stack
			U32 getFreeSize();
			// Rolls the stack back to a previous marker. 
			void freeToMarker(Marker marker);
			// Clears the entire stack (rolls the stack back to
			// zero).
			void clear();
			// Draw human error message for STACK_ALLOCATOR_ERROR
			char* printError(STACK_ALLOCATOR_ERROR err);
	private:
			U32		m_stackSize_bytes;
			Marker	m_stackTop_marker;
			I8*		m_pBuffer;						// untyped pointer to the buffer
	};
	typedef STACK_ALLOCATOR<true> STACK_ALLOCATOR_SAFE;
	typedef STACK_ALLOCATOR<false> STACK_ALLOCATOR_UNSAFE;
}//namespace AT
//-----------------------------------------------------------------
#endif //__STACK_ALLOCATOR_H__
