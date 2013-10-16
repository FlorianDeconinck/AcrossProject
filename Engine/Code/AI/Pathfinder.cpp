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
	inline PATHFINDER::NODE* PushOrUpdate(const GRID& Grid, const ACTOR_ABC& Actor, const PATHFINDER::NODE* pBestNode, const AT::VEC2Di& Target, const AT::I32 nNewX, const AT::I32 nNewY, std::vector<PATHFINDER::NODE*>& ClosedNodes, std::vector<PATHFINDER::NODE*>& OpenNodes, const AT::I32 moveScore=1){
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
	inline AT::I8 CanActorGoNorth_BorderWise(const AE::GRID& G, const AT::I32 Y, const AE::ACTOR_ABC& A){
		return Y + A.GetBBoxHalfHeight() < G.m_nMapHeight-1;
	}
	//-----------------------------------------------------------------------------
	inline AT::I8 CanActorGoSouth_BorderWise(const AT::I32 Y, const AE::ACTOR_ABC& A){
		return Y - A.GetBBoxHalfHeight() > 0;
	}
	//-----------------------------------------------------------------------------
	inline AT::I8 CanActorGoEast_BorderWise(const AE::GRID& G, const AT::I32 X, const AE::ACTOR_ABC& A){
		return X + A.GetBBoxHalfWidth() < G.m_nMapWidth-1;
	}
	//-----------------------------------------------------------------------------
	inline AT::I8 CanActorGoWest_BorderWise(const AT::I32 X, const AE::ACTOR_ABC& A){
		return X - A.GetBBoxHalfWidth() > 0;
	}
	//-----------------------------------------------------------------------------
	inline AT::I8 JPS_MoveStraight(const GRID& Grid, const ACTOR_ABC& Actor, PATHFINDER::NODE* pBestNode, const AT::VEC2Di Target, const AT::I32 StartX, const AT::I32 StartY, const AT::I32 xOffset, const AT::I32 yOffset, std::vector<PATHFINDER::NODE*>& ClosedNodes, std::vector<PATHFINDER::NODE*>& OpenNodes, const AT::I32 DiagonalMove=-1){
		//--
		//Move straight, one natural successor in direction of previous move
		//--
		AT::I32 NaturalSuccessorX = StartX/*+xOffset*/;
		AT::I32 NaturalSuccessorY = StartY/*+yOffset*/;
		//--
		//Test for target
		PATHFINDER::NODE* pFromNode = pBestNode;
		AT::I32 CurrentScore = 0;
		if(NaturalSuccessorX == Target.x && NaturalSuccessorY == Target.y){
			//-- If not pBestNode build node
			if(pBestNode->m_nX != StartX && pBestNode->m_nY != StartY){
				if(DiagonalMove>=0)
					pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
				pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, CurrentScore);
				assert(pFromNode);
			}
			//--
			PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
			return true;
		}
		//--
		//Try jumping
		while(true){
			//--
			//Test natural successor freedom state (necessary for test from diagonal move sub_routine)
			if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(pBestNode->m_nX, pBestNode->m_nY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY))) //Next move isn't possible ->FAIL
				return false;
			//--
			//Test for force node in both perpendicular movement
			// Test tile behind NaturalSucessor first (add two forced nodes) then directly in perpendicular (add one force node)
			AT::I8 bForcedSuccessorFound = false;
			if(yOffset!=0){ //NORTH-SOUTH
				//--
				if(yOffset > 0){					//NORTH
					//--
					//Test for near-grid north border
					if(!CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor)) 
						return false;
					//--
					if(	CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor)		&&
						CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor)	&&	
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH))){ //Test SOUTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);								//Add EAST
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH EAST
						bForcedSuccessorFound=true;
					}else if(CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor) &&
							 !Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY))){			//Test EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);	//Add NORTH EAST
						bForcedSuccessorFound=true;
					}
					//--
					if( CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor)	&&
						CanActorGoWest_BorderWise(NaturalSuccessorX, Actor)		&&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH))){//Test SOUTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);								//Add WEST
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}else if(	CanActorGoWest_BorderWise(NaturalSuccessorX, Actor) &&
								!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY))){			//Test WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}
					//--
				}else{
					//SOUTH
					//--
					//Test for near-grid south border
					if( !CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor) ) 
						return false;
					//--
					if( CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor)	&&
						CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor)	&&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);							//Add EAST
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}else if(	CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor) &&
								!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY))){			//Test EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}
					//--
					if( CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor)	&&
						CanActorGoWest_BorderWise(NaturalSuccessorX, Actor)			&&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);								//Add WEST
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH WEST
						bForcedSuccessorFound=true;
					}else if(CanActorGoWest_BorderWise(NaturalSuccessorX, Actor) &&
							 !Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY))){			//Test WEST
						//-- If not pBestNode build node
						 if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add WEST SOUTH
						bForcedSuccessorFound=true;
					}
					//--
				}
				//--
			}else{			//EAST-WEST
				//--
				if(xOffset > 0 ){
					//EAST
					//--
					//Test for near-grid east border
					if( !CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor) ) 
						return false;
					//--
					if( CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor)	&&
						CanActorGoWest_BorderWise(NaturalSuccessorX, Actor)			&&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);								//Add NORTH
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH EAST
						bForcedSuccessorFound=true;
					}else if(CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor)	&&
							 !Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH))){			//Test NORTH
						 if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH EAST
						bForcedSuccessorFound=true;
					}
					//--
					if( CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor)  &&
						CanActorGoWest_BorderWise(NaturalSuccessorX, Actor)  < 0  &&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH))){//Test SOUTH WEST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);								//Add SOUTH
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::EAST, ClosedNodes, OpenNodes, CurrentScore);			//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}else if( CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor) &&
							  !Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH))){			//Test SOUTH
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH EAST
						bForcedSuccessorFound=true;
					}
					//--
				}else{
					//WEST
					//--
					//Test for near-grid west border
					if( !CanActorGoWest_BorderWise(NaturalSuccessorX, Actor) ) 
						return false;
					//--
					if(	CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor)	&&
						CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor)	&&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::NORTH))){//Test NORTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);								//Add NORTH
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}else if(	CanActorGoNorth_BorderWise(Grid, NaturalSuccessorY, Actor) &&
								!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::NORTH))){			//Test NORTH
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::NORTH, ClosedNodes, OpenNodes, CurrentScore);		//Add NORTH WEST
						bForcedSuccessorFound=true;
					}
					//--
					if(	CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor)		&&
						CanActorGoEast_BorderWise(Grid, NaturalSuccessorX, Actor)	&&
						!Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX+PATHFINDER::EAST, NaturalSuccessorY+PATHFINDER::SOUTH))){//Test SOUTH EAST
						//-- If not pBestNode build node
						if(!bForcedSuccessorFound && (pBestNode->m_nX != NaturalSuccessorX || pBestNode->m_nY != NaturalSuccessorY)){
							if(DiagonalMove>=0)
								pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
							assert(pFromNode);
						}
						//--
						PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);								//Add SOUTH
						if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH WEST
						bForcedSuccessorFound=true;
					}else if(CanActorGoSouth_BorderWise(NaturalSuccessorY, Actor) &&
							 !Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY), AT::VEC2Di(NaturalSuccessorX, NaturalSuccessorY+PATHFINDER::SOUTH))){			//Test SOUTH
						 if( Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH)) )																								
							PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+PATHFINDER::WEST, NaturalSuccessorY+PATHFINDER::SOUTH, ClosedNodes, OpenNodes, CurrentScore);		//Add SOUTH WEST
						bForcedSuccessorFound=true;
					}
					//--
				}
			}
			//--
			if(bForcedSuccessorFound){	//Forced successor found, add newX/newY to OpenNodes and move on
				if(Actor.IsCollisionFree(Grid, AT::VEC2Di(NaturalSuccessorX+xOffset, NaturalSuccessorY+yOffset)))
					PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX+xOffset, NaturalSuccessorY+yOffset, ClosedNodes, OpenNodes, CurrentScore);
				return true;
			}else{						//All tests OK -> move along Natural Successor (test for Target reaching)
				NaturalSuccessorX += xOffset;
				NaturalSuccessorY += yOffset;
				CurrentScore++;
				if(NaturalSuccessorX == Target.x && NaturalSuccessorY == Target.y){
					//-- If not pBestNode build node
					if(!bForcedSuccessorFound && pBestNode->m_nX != StartX && pBestNode->m_nY != StartY){
						if(DiagonalMove>=0)
							pFromNode = PushOrUpdate(Grid, Actor, pFromNode, Target, StartX, StartY, ClosedNodes, OpenNodes, DiagonalMove);
						pFromNode = PushOrUpdate(Grid, Actor, pBestNode, Target, StartX, StartY, ClosedNodes, OpenNodes, CurrentScore);
						assert(pFromNode);
					}
					//--
					PushOrUpdate(Grid, Actor, pFromNode, Target, NaturalSuccessorX, NaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
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
		if(Actor.m_bDebugPathfind){
			for(AT::I32 iH = 0 ; iH < Grid.m_nMapHeight ; ++iH){
				for(AT::I32 iW = 0 ; iW < Grid.m_nMapWidth ; ++iW){
					if(Grid.GetTile(iW, iH) == GRID::DEBUG_PATHFIND || Grid.GetTile(iW, iH) == GRID::PATH)
						Grid.SetTile(iW, iH, GRID::WALKABLE);
				}
			}
		}
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
									bForceNodes=true;
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+SOUTH))){ //South
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									 //South east
									bForceNodes=true;
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, NORTH, ClosedNodes, OpenNodes, CurrentScore)) //NORTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, EAST, 0, ClosedNodes, OpenNodes, CurrentScore)) //EASTward
									bStraightFailed = false;
								if(!bStraightFailed/* || bForceNodes*/)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += EAST;
								DiagonalNaturalSuccessorY += NORTH;
								//--
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY)))
									break;
								//--
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
								//--
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY))){ //East
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									//North east
									bForceNodes=true;
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+SOUTH))){ //South
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									 //South west
									bForceNodes=true;
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, NORTH, ClosedNodes, OpenNodes, CurrentScore)) //NORTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, WEST, 0, ClosedNodes, OpenNodes, CurrentScore)) //WESTward
									bStraightFailed = false;
								if(!bStraightFailed/* || bForceNodes*/)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += WEST;
								DiagonalNaturalSuccessorY += NORTH;
								//--
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY)))
									break;
								//--
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
								//--
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+NORTH))){ //North 
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									 //North east
									bForceNodes=true;
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY))){ //West
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									//South west
									bForceNodes=true;
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, SOUTH, ClosedNodes, OpenNodes, CurrentScore)) //SOUTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, EAST, 0, ClosedNodes, OpenNodes, CurrentScore)) //EASTward
									bStraightFailed = false;
								if(!bStraightFailed/* || bForceNodes*/)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += EAST;
								DiagonalNaturalSuccessorY += SOUTH;
								//--
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY)))
									break;
								//--
								if(DiagonalNaturalSuccessorX == Target.x && DiagonalNaturalSuccessorY == Target.y){
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, ClosedNodes, OpenNodes, CurrentScore);
									break;
								}
								CurrentScore++;
							}
						}else{														//SOUTH-WESTward movement
							//Natural successors : SOUTH(straight), SOUTH-WEST(diagonal), WEST(straight)
							while(true){
								AT::I8 bForceNodes = false;
								//Test for forced nodes (we first test that the node being evaluated isn't in the actor.bbox at the time)
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY+NORTH))){ //North
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+WEST, DiagonalNaturalSuccessorY+NORTH, ClosedNodes, OpenNodes);									 //North west
									bForceNodes=true;
								}
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY), AT::VEC2Di(DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY))){ //East
									PushOrUpdate(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX+EAST, DiagonalNaturalSuccessorY+SOUTH, ClosedNodes, OpenNodes);									//South east
									bForceNodes=true;
								}
								//Test straight movement, if one of them don't fail stop recursion
								AT::I8 bStraightFailed = true;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, 0, SOUTH, ClosedNodes, OpenNodes, CurrentScore)) //SOUTHward
									bStraightFailed = false;
								if(JPS_MoveStraight(Grid, Actor, pBestNode, Target, DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY, WEST, 0, ClosedNodes, OpenNodes, CurrentScore))	//WESTward
									bStraightFailed = false;
								if(!bStraightFailed || bForceNodes)
									break;
								//All test ok move along diagonal
								DiagonalNaturalSuccessorX += WEST;
								DiagonalNaturalSuccessorY += SOUTH;
								//--
								if(!Actor.IsCollisionFree(Grid, AT::VEC2Di(DiagonalNaturalSuccessorX, DiagonalNaturalSuccessorY)))
									break;
								//--
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
		}
		//-----
	clean :
#ifdef _DEBUG
		//--
		//Timer
		AT::I64F pathfinderProcesTime = ENGINE::m_Timer.GetTime() - startTime;
		if(pathfinderProcesTime > g_PathfinderDebug_MaxProcessTime)
			g_PathfinderDebug_MaxProcessTime = pathfinderProcesTime;
		if(g_PathfinderDebug_MeanProcessTime < 0)
			g_PathfinderDebug_MeanProcessTime = pathfinderProcesTime;
		else
			g_PathfinderDebug_MeanProcessTime = (g_PathfinderDebug_MeanProcessTime + pathfinderProcesTime) / 2.0;
		GUI::GUI_PERF_LOG& PathfinderLogMean = GUI::m_PerfLogContent[GUI::GUI_PERF_LOG::GUI_PERF_LOG_INDEX_PATHFIND_MEAN];
		sprintf(PathfinderLogMean.m_sText, "Pathfinder mean : %.2lf ms (%d - %d)", g_PathfinderDebug_MeanProcessTime, ClosedNodes.size(), Result);
		GUI::GUI_PERF_LOG& PathfinderLogMax = GUI::m_PerfLogContent[GUI::GUI_PERF_LOG::GUI_PERF_LOG_INDEX_PATHFIND_MAX];
		sprintf(PathfinderLogMax.m_sText, "Pathfinder max : %.2lf ms", g_PathfinderDebug_MaxProcessTime);
		//--
		//Path debug
		if(Actor.m_bDebugPathfind /*&& Start.x == 58 && Start.y==23 && Target.x == 39 && Target.y == 15*/){
			for(AT::U32 iNode = 0 ; iNode < ClosedNodes.size() ; ++iNode)
				Grid.SetTile(ClosedNodes[iNode]->m_nX, ClosedNodes[iNode]->m_nY, GRID::DEBUG_PATHFIND);
			for(AT::I32 iNode = 0 ; iNode < Result ; ++iNode)
				Grid.SetTile(pOutBuffer[iNode].x, pOutBuffer[iNode].y, GRID::PATH);
			g_bUpdateWorld = true;
		}
		//--
#endif
		//Clean node pool
		AT::I32 NodePoolCount = ClosedNodes.size();
		for(AT::I32 iNode = 0 ; iNode < NodePoolCount ; ++iNode)
			delete ClosedNodes[iNode];
		//-----
		//Add self back to grid (to avoid self-crossing tests)
		Actor.AddToGrid(Grid);
		return Result;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
