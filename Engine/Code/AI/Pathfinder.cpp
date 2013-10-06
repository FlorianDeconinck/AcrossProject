//-----------------------------------------------------------------------------
//Project
#include "Pathfinder.h"
#include "../World.h"
#include "../GlobalDebug.h"
#ifdef _DEBUG
	#include "../Engine.h"
#endif
//STD
#include <cstdlib>
#include <assert.h>
#include <algorithm>
#include <iostream>
//Defines
#define DEBUG_PATHFINDER()  0
#define JPS()				1 //Based on the paper by Harabor and Grastien (http://users.cecs.anu.edu.au/~dharabor/data/papers/harabor-grastien-socs12.pdf)
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	//PATHFINDER
	//-----------------------------------------------------------------------------
	PATHFINDER::PATHFINDER(){}
	PATHFINDER::~PATHFINDER(){}
	//-----------------------------------------------------------------------------
	int ChebyshevDistance(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY){
		//We assume here that diagonal movement has the same cost has longitudinal movement
		//If not, formula should be change in (with 14 and 10 as adjustable weights): 
		//  xDistance = abs(currentX-targetX)
		//  yDistance = abs(currentY-targetY)
		//	if xDistance > yDistance
		//		H = 14*yDistance + 10*(xDistance-yDistance)
		//	else
		//	    H = 14*xDistance + 10*(yDistance-xDistance)
		return max(abs(nStartX - nTargetX), abs(nStartY - nTargetY));
	}
	//-----------------------------------------------------------------------------
	int ManhanttanDistance(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY){
		return abs(nStartX - nTargetX) + abs(nStartY - nTargetY);
	}
	//-----------------------------------------------------------------------------
	int compNodess_f(const void* NodeA, const void* NodeB){ 
		PATHFINDER::NODE* A = (PATHFINDER::NODE*)NodeA;
		PATHFINDER::NODE* B = (PATHFINDER::NODE*)NodeB;
		if(A->m_nEstimatedDistToTarget == B->m_nEstimatedDistToTarget)
			return A->m_nScore < B->m_nScore; //prefer the most advanced node
		return A->m_nEstimatedDistToTarget > B->m_nEstimatedDistToTarget;
	}
	//-----------------------------------------------------------------------------
	inline PATHFINDER::NODE* PushOrUpdate(const GRID& Grid, const ACTOR_ABC& Actor, const PATHFINDER::NODE* pBestNode, const AT::VEC2Di Target, const AT::I32 nNewX, const AT::I32 nNewY, std::vector<PATHFINDER::NODE*>& ClosedNodes, std::vector<PATHFINDER::NODE*>& OpenNodes, const AT::I32 moveScore=1){
		//Test open list
		std::vector<PATHFINDER::NODE*>::iterator Iterator;
		AT::I8 bFindInOpenList = false;
		for(Iterator = OpenNodes.begin() ; Iterator != OpenNodes.end() ; ++Iterator){
			PATHFINDER::NODE* OpenNode = *Iterator;
			if(OpenNode->m_nX == nNewX && OpenNode->m_nY == nNewY){ // Is on open list
				if(pBestNode->m_nScore+1 < OpenNode->m_nScore){// New node has better score update open list
					OpenNodes.erase(Iterator);
					make_heap(OpenNodes.begin(), OpenNodes.end(), compNodess_f);
					bFindInOpenList = true;
					break;
				}else											// Open node has better score
					return OpenNode;
			}
		}
		//Test on close list
		for(Iterator = ClosedNodes.begin() ; Iterator != ClosedNodes.end() ; ++Iterator){
			PATHFINDER::NODE* ClosedNode = *Iterator;
			if(ClosedNode->m_nX == nNewX && ClosedNode->m_nY == nNewY){// Is on closed list
				if(bFindInOpenList){ // Already update on open list, update on closed and open list
					ClosedNode->m_nScore = pBestNode->m_nScore+1;
					ClosedNode->m_pFromNode = pBestNode;
					ClosedNode->m_nEstimatedDistToTarget = pBestNode->m_nScore+moveScore+ChebyshevDistance(nNewX, nNewY, Target.x, Target.y);
					OpenNodes.push_back(ClosedNode);
					push_heap(OpenNodes.begin(), OpenNodes.end(), compNodess_f);
				}
				return ClosedNode;
			}
		}
		//New node to add in both lists
		ClosedNodes.push_back(new PATHFINDER::NODE(nNewX, nNewY, pBestNode->m_nScore+1, pBestNode->m_nScore+1+ChebyshevDistance(nNewX, nNewY, Target.x, Target.y), pBestNode));
		OpenNodes.push_back(ClosedNodes.back());
		PATHFINDER::NODE* pNew = OpenNodes.back();
		std::push_heap(OpenNodes.begin(), OpenNodes.end(), compNodess_f);
		return pNew;
	}
	//-----------------------------------------------------------------------------
	inline void TestAndPush(const GRID& Grid, const ACTOR_ABC& Actor, const PATHFINDER::NODE* pBestNode, const AT::VEC2Di Target, const AT::I32 nNewX, const AT::I32 nNewY, std::vector<PATHFINDER::NODE*>& ClosedNodes, std::vector<PATHFINDER::NODE*>& OpenNodes){
		if(Actor.IsCollisionFree(Grid, AT::VEC2Di(pBestNode->m_nX, pBestNode->m_nY), AT::VEC2Di(nNewX, nNewY))){
			PATHFINDER::NODE* pFrom_Unused=NULL;
			PushOrUpdate(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
		}
	}
	//-----------------------------------------------------------------------------
	inline AT::I8 JPS_MoveStraight(const GRID& Grid, const ACTOR_ABC& Actor, PATHFINDER::NODE* pBestNode, const AT::VEC2Di Target, const AT::I32 StartX, const AT::I32 StartY, const AT::I32 xOffset, const AT::I32 yOffset, std::vector<PATHFINDER::NODE*>& ClosedNodes, std::vector<PATHFINDER::NODE*>& OpenNodes){
		//--
		//Move straight, one natural successor in direction of previous move
		//--
		AT::I32 NaturalSuccessorX = StartX;
		AT::I32 NaturalSuccessorY = StartY;
		//--
		//Try jumping
		AT::I32 CurrentScore = 0;
		while(true){
			//--
			//Test for grid border
			if( NaturalSuccessorX > Grid.m_nMapWidth-1 || NaturalSuccessorX < 1 || NaturalSuccessorY > Grid.m_nMapHeight-1 || NaturalSuccessorY < 1 )//Reach grid border -> FAIL
				return false;
			//--
			//Test natural successor freedom state
			if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(pBestNode->m_nX, pBestNode->m_nY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY))) //Next move isn't possible ->FAIL
				return false;
			//--
			//Test for force node in both perpendicular movement
			// Test tile behind NaturalSucessor first (add two forced nodes) then directly in perpendicular (add one force node)
			AT::I8 bForcedSuccessorFound = false;
			PATHFINDER::NODE* pFrom = pBestNode;
			if(yOffset!=0){ //NORTH-SOUTH
				//--
				if(yOffset > 0){					//NORTH
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH))){ //Test SOUTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);								//Add EAST
						if( NaturalSuccessorY+1 < Grid.m_nMapHeight )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH EAST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY))){			//Test EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						if( NaturalSuccessorY+1 < Grid.m_nMapHeight )																								
							PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);	//Add NORTH EAST
						bForcedSuccessorFound=true;
					}
					//--
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH))){//Test SOUTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);								//Add WEST
						if( NaturalSuccessorY+1 < Grid.m_nMapHeight )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY))){			//Test WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						if( NaturalSuccessorY+1 < Grid.m_nMapHeight )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}
					//--
				}else{
					//SOUTH
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);							//Add EAST
						if( NaturalSuccessorY-1 >= 0 )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY))){			//Test EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						if( NaturalSuccessorY-1 >=  0 )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}
					//--
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);								//Add WEST
						if( NaturalSuccessorY >= 0 )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH WEST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY))){			//Test WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						if( NaturalSuccessorY >= 0 )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add WEST SOUTH
						bForcedSuccessorFound=true;
					}
					//--
				}
				//--
			}else{			//EAST-WEST
				//--
				if(xOffset > 0 ){
					//EAST
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);								//Add NORTH
						if( NaturalSuccessorX+1 < Grid.m_nMapWidth )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH EAST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH))){			//Test NORTH
						if( NaturalSuccessorX+1 < Grid.m_nMapWidth )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH EAST
						bForcedSuccessorFound=true;
					}
					//--
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH))){//Test SOUTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);								//Add SOUTH
						if( NaturalSuccessorX+1 < Grid.m_nMapWidth )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::EAST, ClosedNodes, OpenNodes, CurrentScore);			//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH))){			//Test SOUTH
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						if( NaturalSuccessorX+1 < Grid.m_nMapWidth )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}
					//--
				}else{
					//WEST
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);								//Add NORTH
						if( NaturalSuccessorX-1 >= 0 )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH))){			//Test NORTH
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						if( NaturalSuccessorX-1 >= 0 )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}
					//--
					if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH))){//Test SOUTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && pBestNode->m_nX != NaturalSuccessorX && pBestNode->m_nY != NaturalSuccessorY){
							pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFrom);
						}
						//--
						PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);								//Add SOUTH
						if( NaturalSuccessorX-1 >= 0 )
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH WEST
						bForcedSuccessorFound=true;
					}else if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH))){			//Test SOUTH
						if( NaturalSuccessorX-1 >= 0 )																								
							PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH WEST
						bForcedSuccessorFound=true;
					}
					//--
				}
			}
			//--
			if(bForcedSuccessorFound){	//Forced successor found, add newX/newY to OpenNodes and move on
				return true;
			}else{						//All tests OK -> move along Natural Successor (test for Target reaching)
				NaturalSuccessorX += xOffset;
				NaturalSuccessorY += yOffset;
				CurrentScore++;
				if(NaturalSuccessorX == Target.x && NaturalSuccessorY == Target.y){
					//-- If not pBestNode build node
					if(!bForcedSuccessorFound && pBestNode->m_nX != StartX && pBestNode->m_nY != StartY){
						pFrom = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, CurrentScore);
						assert(pFrom);
					}
					//--
					PushOrUpdate(Grid, Actor, pFrom, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
					return true;
				}
			}
		}
		//--
	}
	//-----------------------------------------------------------------------------
	// Assumption are made :
	//   * pMap origin is bottom-left
	//	 * pMap row and column are zero-based
	//	 * nOutBufferSize contains the number of int (and not the number of "position" x/y)
	AT::I32 PATHFINDER::FindPath_8Way(GRID& Grid, const ACTOR_ABC& Actor ,const AT::VEC2Di& Start, const AT::VEC2Di& Target,  AT::VEC2Di* pOutBuffer, const int nOutBufferSize){
		//-----
#ifdef _DEBUG
		AT::I64F startTime = ENGINE::m_Timer.GetTime();
#endif
		//-----
		//Check that destination is attainable
		if(!Actor.IsCollisionFree(Grid, Start, Target))
			return -1;
		//-----
		//Remove self from grid (to avoid self-crossing tests)
		Actor.EraseFromGrid(Grid);
		//-----
		AT::I32				Result = -1; // -1 : failure (target is not reachable) , >0 size of path
		//-----
		std::vector<NODE*> ClosedNodes;
		ClosedNodes.push_back(new NODE(Start.x, Start.y, 0, ChebyshevDistance(Start.x, Start.y, Target.x, Target.y), NULL));
		//-----
		std::vector<NODE*> OpenNodes;
		OpenNodes.push_back(ClosedNodes.back());
		std::make_heap(OpenNodes.begin(), OpenNodes.end(), compNodess_f);
		//-----	
		while(OpenNodes.size()){
			//----
			//Choose best node to build upon
			NODE* pBestNode = OpenNodes.front();
			pop_heap(OpenNodes.begin(), OpenNodes.end(), compNodess_f);
			OpenNodes.pop_back();
			//---
			if(pBestNode->m_nX == Target.x && pBestNode->m_nY == Target.y){//Target found build back path
				//Count path nodes
				AT::I32 nPathCount = 0;
				const NODE* pPath = pBestNode;
				while(pPath){
					nPathCount++;
					pPath = pPath->m_pFromNode;
				}
				if(nPathCount > nOutBufferSize){
					return -1;
					goto clean;
				}
				//Fill up pOutBuffer and clean
				AT::I32 idx = nPathCount-1;
				Result = nPathCount;
				pPath = pBestNode;
				while(pPath && idx >= 0){
					//Fill out buffer from start until full. Fill full position only.
					pOutBuffer[idx].y	= pPath->m_nY;
					pOutBuffer[idx].x	= pPath->m_nX;
					idx --;
					pPath = pPath->m_pFromNode;
				}
				goto clean;
			}
			//-----------------------------------------------
			//----
#if JPS()
			if(!pBestNode->m_pFromNode){
				//----
				//North
				AT::I32 nNewX = pBestNode->m_nX;
				AT::I32 nNewY = pBestNode->m_nY+NORTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//NorthEast
				nNewX = pBestNode->m_nX+EAST;
				nNewY = pBestNode->m_nY+NORTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//East
				nNewX = pBestNode->m_nX+EAST;
				nNewY = pBestNode->m_nY;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//SouthEast
				nNewX = pBestNode->m_nX+EAST;
				nNewY = pBestNode->m_nY+SOUTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//South
				nNewX = pBestNode->m_nX;
				nNewY = pBestNode->m_nY+SOUTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//SouthWest
				nNewX = pBestNode->m_nX+WEST;
				nNewY = pBestNode->m_nY+SOUTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//West
				nNewX = pBestNode->m_nX+WEST;
				nNewY = pBestNode->m_nY;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//NorthWest
				nNewX = pBestNode->m_nX+WEST;
				nNewY = pBestNode->m_nY+NORTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			}else{
				if(pBestNode->m_pFromNode->m_nX - pBestNode->m_nX !=0 && pBestNode->m_pFromNode->m_nY - pBestNode->m_nY !=0){  //Diagonal move
					AT::I32 DiagonalNaturalSuccessorX = pBestNode->m_nX;
					AT::I32 DiagonalNaturalSuccessorY = pBestNode->m_nY;
					AT::I32 CurrentScore = 0;
					if(pBestNode->m_nY - pBestNode->m_pFromNode->m_nY > 0){
						if(pBestNode->m_nX - pBestNode->m_pFromNode->m_nX > 0){	//NORTH-EASTward movement
							//Natural successors : NORTH(straight), NORTH-EAST(diagonal), EAST(straight)
							while(true){																	
								AT::I8 bForceNodes= false;
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY))){ //West
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									//North west
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+SOUTH))){ //South
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									 //South east
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, NORTH, ClosedNodes, OpenNodes)) //NORTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, EAST, 0, ClosedNodes, OpenNodes)) //EASTward
									bStraightFailed = false;
								if(!bStraightFailed)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += EAST;
								DiagonalNaturalSuccessorY += NORTH;
								if(DiagonalNaturalSuccessorX == Target.x && DiagonalNaturalSuccessorY == Target.y){
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
									break;
								}
								CurrentScore++;
							}
						}else{														//NORTH-WESTward movement
							//Natural successors : NORTH(straight), NORTH-WEST(diagonal), WEST(straight)
							while(true){																	
								AT::I8 bForceNodes= false;
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY))){ //East
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									//North east
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+SOUTH))){ //South
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									 //South west
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, NORTH, ClosedNodes, OpenNodes)) //NORTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, WEST, 0, ClosedNodes, OpenNodes)) //WESTward
									bStraightFailed = false;
								if(!bStraightFailed)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += WEST;
								DiagonalNaturalSuccessorY += NORTH;
								if(DiagonalNaturalSuccessorX == Target.x && DiagonalNaturalSuccessorY == Target.y){
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
									break;
								}
								CurrentScore++;
							}
						}
					}else{															
						if(pBestNode->m_nX - pBestNode->m_pFromNode->m_nX > 0){	//SOUTH-EASTward movement
							//Natural successors : SOUTH(straight), SOUTH-EAST(diagonal), EAST(straight)
							while(true){																	
								AT::I8 bForceNodes= false;
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+NORTH))){ //North 
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									 //North east
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY))){ //West
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									//South west
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, SOUTH, ClosedNodes, OpenNodes)) //SOUTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, EAST, 0, ClosedNodes, OpenNodes)) //EASTward
									bStraightFailed = false;
								if(!bStraightFailed)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += EAST;
								DiagonalNaturalSuccessorY += SOUTH;
								if(DiagonalNaturalSuccessorX == Target.x && DiagonalNaturalSuccessorY == Target.y){
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
									break;
								}
								CurrentScore++;
							}
						}else{														//SOUTH-WESTward movement
							//Natural successors : SOUTH(straight), SOUTH-WEST(diagonal), WEST(straight)
							while(true){																	
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+NORTH))){ //North
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									 //North west
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY))){ //East
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									//South east
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, SOUTH, ClosedNodes, OpenNodes)) //SOUTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, WEST, 0, ClosedNodes, OpenNodes))	//WESTward
									bStraightFailed = false;
								if(!bStraightFailed)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += WEST;
								DiagonalNaturalSuccessorY += SOUTH;
								if(DiagonalNaturalSuccessorX == Target.x && DiagonalNaturalSuccessorY == Target.y){
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
									break;
								}
								CurrentScore++;
							}
						}
					}
				}else{ //Straight move
					//NORTHward movement
					if(pBestNode->m_nY - pBestNode->m_pFromNode->m_nY > 0)
						JPS_MoveStraight(Grid, Actor, pBestNode, Target, pBestNode->m_nX, pBestNode->m_nY, 0, NORTH, ClosedNodes, OpenNodes);
					//EASTward movement 
					else if(pBestNode->m_nX - pBestNode->m_pFromNode->m_nX > 0)
						JPS_MoveStraight(Grid, Actor, pBestNode, Target, pBestNode->m_nX, pBestNode->m_nY, EAST, 0, ClosedNodes, OpenNodes);
					//SOUTHward movement
					else if(pBestNode->m_nY - pBestNode->m_pFromNode->m_nY < 0)
						JPS_MoveStraight(Grid, Actor, pBestNode, Target, pBestNode->m_nX, pBestNode->m_nY, 0, SOUTH, ClosedNodes, OpenNodes);
					//WESTward movement 
					else if(pBestNode->m_nX - pBestNode->m_pFromNode->m_nX < 0)
						JPS_MoveStraight(Grid, Actor, pBestNode, Target, pBestNode->m_nX, pBestNode->m_nY, WEST, 0, ClosedNodes, OpenNodes);
				}
			}
#else
				//----
				//North
				AT::I32 nNewX = pBestNode->m_nX;
				AT::I32 nNewY = pBestNode->m_nY+NORTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//NorthEast
				nNewX = pBestNode->m_nX+EAST;
				nNewY = pBestNode->m_nY+NORTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//East
				nNewX = pBestNode->m_nX+EAST;
				nNewY = pBestNode->m_nY;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//SouthEast
				nNewX = pBestNode->m_nX+EAST;
				nNewY = pBestNode->m_nY+SOUTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//South
				nNewX = pBestNode->m_nX;
				nNewY = pBestNode->m_nY+SOUTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//SouthWest
				nNewX = pBestNode->m_nX+WEST;
				nNewY = pBestNode->m_nY+SOUTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//West
				nNewX = pBestNode->m_nX+WEST;
				nNewY = pBestNode->m_nY;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
				//NorthWest
				nNewX = pBestNode->m_nX+WEST;
				nNewY = pBestNode->m_nY+NORTH;
				TestAndPush(Grid, Actor, pBestNode, Target, nNewX, nNewY, ClosedNodes, OpenNodes);

#endif
			//----
#ifdef _DEBUG
#if 0
			for(unsigned int i = 0 ; i < TracksPool.size() ; i++){
				for(unsigned int j = i+1 ; j < TracksPool.size() ; j++){
					assert(TracksPool[i]->m_nX != TracksPool[j]->m_nX || TracksPool[i]->m_nY && TracksPool[j]->m_nY);
				}
			}
#endif
#endif
			//----
		}
		//-----
	clean :
		//Clean node pool
		int NodePoolCount = ClosedNodes.size();
		for(int iNode = 0 ; iNode < NodePoolCount ; ++iNode)
			delete ClosedNodes[iNode];
		//-----
		//Add selfback to grid (to avoid self-crossing tests)
		Actor.AddToGrid(Grid);
#ifdef _DEBUG
		AT::I64F pathfinderProcesTime = ENGINE::m_Timer.GetTime() - startTime;
		if(pathfinderProcesTime > g_PathfinderDebug_MaxProcessTime)
			g_PathfinderDebug_MaxProcessTime = pathfinderProcesTime;
		if(g_PathfinderDebug_MeanProcessTime < 0)
			g_PathfinderDebug_MeanProcessTime = pathfinderProcesTime;
		else
			g_PathfinderDebug_MeanProcessTime = (g_PathfinderDebug_MaxProcessTime + pathfinderProcesTime) / 2.0;
		GUI::GUI_PERF_LOG& PathfinderLogMean = GUI::m_PerfLogContent[GUI::GUI_PERF_LOG::GUI_PERF_LOG_INDEX_PATHFIND_MEAN];
		sprintf(PathfinderLogMean.m_sText, "Pathfinder mean : %.2lf ms", g_PathfinderDebug_MeanProcessTime);
		GUI::GUI_PERF_LOG& PathfinderLogMax = GUI::m_PerfLogContent[GUI::GUI_PERF_LOG::GUI_PERF_LOG_INDEX_PATHFIND_MAX];
		sprintf(PathfinderLogMax.m_sText, "Pathfinder max : %.2lf ms", g_PathfinderDebug_MaxProcessTime);
#endif
		return Result;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
