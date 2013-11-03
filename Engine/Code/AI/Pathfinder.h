//-----------------------------------------------------------------------------
//Project
#include "Vec2D.h"
//Tools
#include "CodeTools.h"
//STD
#include <memory.h>
#include <vector>
//--
#define STACKALLOCATOR()	1
#if STACKALLOCATOR()
#include <StackAllocator.h>
#endif
//-----------------------------------------------------------------------------
namespace AE{
	class GRID;
	class RENDERER;
	class ACTOR_ABC;
	//-----------------------------------------------------------------------------
#if STACKALLOCATOR()
	extern AT::STACK_ALLOCATOR_SAFE		Pathfinder_Stack_Allocator;		
#endif
	class PATHFINDER{
	public :
		PATHFINDER();
		~PATHFINDER();
		static AT::I32 FindPath_8Way(GRID& Grid, const ACTOR_ABC& Actor ,const AT::VEC2Di& Start, const AT::VEC2Di& Target,  AT::VEC2Di* pOutBuffer, const int nOutBufferSize);
		class NODE{
		public:
			NODE():m_nX(-1),m_nY(-1),m_nScore(INT_MAX),m_nEstimatedDistToTarget(INT_MAX){}
			NODE(const NODE& T):m_nX(T.m_nX),m_nY(T.m_nY),m_nScore(T.m_nScore),m_pFromNode(T.m_pFromNode),m_nEstimatedDistToTarget(0){}
			NODE(int _x, int _y, int _score, int _manhDistFromLastTile, const NODE* _from):m_nX(_x),m_nY(_y),m_nScore(_score),m_nEstimatedDistToTarget(_manhDistFromLastTile),m_pFromNode(_from){}
			NODE operator=(const NODE& N){ Break(); m_nX = N.m_nX ; m_nY = N.m_nY; m_nScore = N.m_nScore; m_nEstimatedDistToTarget = N.m_nEstimatedDistToTarget; m_pFromNode = N.m_pFromNode; }
			//--
			AT::I32						m_nX;
			AT::I32						m_nY;
			AT::I32						m_nScore;
			AT::I32						m_nEstimatedDistToTarget;
			const NODE*					m_pFromNode;
		};
		static const AT::I32 NORTH	= 1;
		static const AT::I32 SOUTH	= -1;
		static const AT::I32 EAST	= 1;
		static const AT::I32 WEST	= -1;
	};
	//-----------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------