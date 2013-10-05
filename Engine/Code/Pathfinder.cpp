//-----------------------------------------------------------------------------
//Project
#include "Pathfinder.h"
#include "World.h"
#include "GlobalDebug.h"
//STD
#include <cstdlib>
#include <assert.h>
#include <algorithm>
#include <iostream>
//Defines
#define DEBUG_PATHFINDER() 0
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
	int compTracks_f(const void* NodeA, const void* NodeB){ 
		PATHFINDER::NODE* A = (PATHFINDER::NODE*)NodeA;
		PATHFINDER::NODE* B = (PATHFINDER::NODE*)NodeB;
		if(A->m_nEstimatedDistToTarget == B->m_nEstimatedDistToTarget)
			return A->m_nScore < B->m_nScore; //prefer the most advanced track
		return A->m_nEstimatedDistToTarget > B->m_nEstimatedDistToTarget;
	}
	//-----------------------------------------------------------------------------
	inline void TestAndPush(const GRID& Grid, const ACTOR_ABC& Actor, const PATHFINDER::NODE* pBestTrack, const AT::VEC2Di Target, const AT::I32 nNewX, const AT::I32 nNewY, std::vector<PATHFINDER::NODE*>& ClosedNodes, std::vector<PATHFINDER::NODE*>& OpenNodes){
		if(Actor.IsCollisionFree(Grid, AT::VEC2Di(pBestTrack->m_nX, pBestTrack->m_nY), AT::VEC2Di(nNewX, nNewY))){
			//Test open list
			std::vector<PATHFINDER::NODE*>::iterator Iterator;
			AT::I8 bFindInOpenList = false;
			for(Iterator = OpenNodes.begin() ; Iterator != OpenNodes.end() ; ++Iterator){
				PATHFINDER::NODE* OpenNode = *Iterator;
				if(OpenNode->m_nX == nNewX && OpenNode->m_nY == nNewY){ // Is on open list
					if(pBestTrack->m_nScore+1 < OpenNode->m_nScore){// New node has better score update open list
						OpenNodes.erase(Iterator);
						make_heap(OpenNodes.begin(), OpenNodes.end(), compTracks_f);
						bFindInOpenList = true;
						break;
					}else											// Open node has better score
						return;
				}
			}
			//Test on close list
			for(Iterator = ClosedNodes.begin() ; Iterator != ClosedNodes.end() ; ++Iterator){
				PATHFINDER::NODE* ClosedNode = *Iterator;
				if(ClosedNode->m_nX == nNewX && ClosedNode->m_nY == nNewY){// Is on closed list
					if(bFindInOpenList){ // Already update on open list, update on closed and open list
						ClosedNode->m_nScore = pBestTrack->m_nScore+1;
						ClosedNode->m_pFromTrack = pBestTrack;
						ClosedNode->m_nEstimatedDistToTarget = pBestTrack->m_nScore+1+ChebyshevDistance(nNewX, nNewY, Target.x, Target.y);
						OpenNodes.push_back(ClosedNode);
						push_heap(OpenNodes.begin(), OpenNodes.end(), compTracks_f);
					}
					return;
				}
			}
			//New node to add in both lists
			ClosedNodes.push_back(new PATHFINDER::NODE(nNewX, nNewY, pBestTrack->m_nScore+1, pBestTrack->m_nScore+1+ChebyshevDistance(nNewX, nNewY, Target.x, Target.y), pBestTrack));
			OpenNodes.push_back(ClosedNodes.back());
			std::push_heap(OpenNodes.begin(), OpenNodes.end(), compTracks_f);
		}
	}
	//-----------------------------------------------------------------------------
	// Assumption are made :
	//   * pMap origin is bottom-left
	//	 * pMap row and column are zero-based
	//	 * nOutBufferSize contains the number of int (and not the number of "position" x/y)
	AT::I32 PATHFINDER::FindPath_8Way(const GRID& Grid, const ACTOR_ABC& Actor ,const AT::VEC2Di& Start, const AT::VEC2Di& Target,  AT::VEC2Di* pOutBuffer, const int nOutBufferSize){
		//-----
		//Check that destination is attainable
		if(!Actor.IsCollisionFree(Grid, Start, Target))
			return -1;
		//-----
		AT::I32				Result = -1; // -1 : failure (target is not reachable) , >0 size of path
		//-----
		std::vector<NODE*> ClosedNodes;
		ClosedNodes.push_back(new NODE(Start.x, Start.y, 0, ChebyshevDistance(Start.x, Start.y, Target.x, Target.y), NULL));
		//-----
		std::vector<NODE*> OpenNodes;
		OpenNodes.push_back(ClosedNodes.back());
		std::make_heap(OpenNodes.begin(), OpenNodes.end(), compTracks_f);
		//-----	
		while(OpenNodes.size()){
			//----
			//Choose best node to build upon
//			std::sort(ToSearchNodesVector.begin(), ToSearchNodesVector.end(), compTracks_f);
			const NODE* pBestTrack = OpenNodes.front();
			pop_heap(OpenNodes.begin(), OpenNodes.end(), compTracks_f);
			OpenNodes.pop_back();
			//---
			if(pBestTrack->m_nX == Target.x && pBestTrack->m_nY == Target.y){//Target found build back path
				//Count path nodes
				AT::I32 nPathCount = 0;
				const NODE* pPath = pBestTrack;
				while(pPath){
					nPathCount++;
					pPath = pPath->m_pFromTrack;
				}
				nPathCount--;
				if(nPathCount > nOutBufferSize){
					return -1;
					goto clean;
				}
				//Fill up pOutBuffer and clean
				AT::I32 idx = nPathCount;
				Result = nPathCount;
				pPath = pBestTrack;
				while(pPath && idx >= 0){
					//Fill out buffer from start until full. Fill full position only.
					pOutBuffer[idx].y	= pPath->m_nY;
					pOutBuffer[idx].x	= pPath->m_nX;
					idx --;
					pPath = pPath->m_pFromTrack;
				}
				goto clean;
			}
			//-----------------------------------------------
			//----
			//North
			AT::I32 nNewX = pBestTrack->m_nX;
			AT::I32 nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//NorthEast
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//East
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//SouthEast
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//South
			nNewX = pBestTrack->m_nX;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//SouthWest
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//West
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
			//NorthWest
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, ClosedNodes, OpenNodes);
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
		//Clean track pool
		int TrackPoolCount = ClosedNodes.size();
		for(int iTrack = 0 ; iTrack < TrackPoolCount ; ++iTrack)
			delete ClosedNodes[iTrack];
		return Result;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------