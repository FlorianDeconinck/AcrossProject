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
	int ManhanttanDistance(const int nStartX, const int nStartY, const int nTargetX, const int nTargetY){
		return abs(nStartX - nTargetX) + abs(nStartY - nTargetY);
	}
	//-----------------------------------------------------------------------------
	int compTracks_f(const void* TrackA, const void* TrackB){ 
		PATHFINDER::TRACK* A = (PATHFINDER::TRACK*)TrackA;
		PATHFINDER::TRACK* B = (PATHFINDER::TRACK*)TrackB;
		if(A->m_nEstimatedDistToTarget == B->m_nEstimatedDistToTarget)
			return A->m_nScore > B->m_nScore; //prefer the last advanced track
		return A->m_nEstimatedDistToTarget < B->m_nEstimatedDistToTarget;
	}
	//-----------------------------------------------------------------------------
	inline void TestAndPush(const GRID& Grid, const ACTOR_ABC& Actor, const PATHFINDER::TRACK* pBestTrack, const AT::VEC2Di Target, const AT::I32 nNewX, const AT::I32 nNewY, const AT::I32 iProcessed, std::vector<PATHFINDER::TRACK*>& TracksPool, AT::I32& nTmpManhanttanDist, std::vector<PATHFINDER::TRACK*>& ToSearchNodesVector){
		if(Actor.IsCollisionFree(Grid, AT::VEC2Di(pBestTrack->m_nX, pBestTrack->m_nY), AT::VEC2Di(nNewX, nNewY))){
			//If already processed verify that the new track isn't better
			if(iProcessed<0 || pBestTrack->m_nScore+1<TracksPool[iProcessed]->m_nScore){
				nTmpManhanttanDist = ManhanttanDistance(nNewX, nNewY, Target.x, Target.y);
				TracksPool.push_back(new PATHFINDER::TRACK(nNewX, nNewY, pBestTrack->m_nScore+1, pBestTrack->m_nScore+1+nTmpManhanttanDist, pBestTrack));
				ToSearchNodesVector.push_back(TracksPool.back());
			}
		}
	}
	//-----------------------------------------------------------------------------
	// Assumption are made :
	//   * pMap origin is bottom-left
	//	 * pMap row and column are zero-based
	//	 * nOutBufferSize contains the number of int (and not the number of "position" x/y)
	AT::I32 PATHFINDER::FindPath_4Way(const GRID& Grid, const ACTOR_ABC& Actor ,const AT::VEC2Di& Start, const AT::VEC2Di& Target,  AT::I32* pOutBuffer, const int nOutBufferSize){
		//-----
		//Check that destination is attainable
		if(Actor.IsCollisionFree(Grid, Target) != GRID::WALKABLE)
			return -1;
		//-----
		int					Result = -1; // -2 : buffer too small, -1 : failure (target is not reachable) , >0 size of path
		std::vector<TRACK*> TracksPool;
		TracksPool.push_back(new TRACK(Start.x, Start.y, 0, ManhanttanDistance(Start.x, Start.y, Target.x, Target.y), NULL));
		//-----
		std::vector<TRACK*> ToSearchNodesVector;
		ToSearchNodesVector.push_back(TracksPool.back());
		//-----	
		while(ToSearchNodesVector.size()){
			//----
			//Choose best node to build upon
			std::stable_sort(ToSearchNodesVector.begin(), ToSearchNodesVector.end(), compTracks_f);
			const TRACK* pBestTrack = ToSearchNodesVector[0];
			if(pBestTrack->m_nX == Target.x && pBestTrack->m_nY == Target.y){//Target found build back path
			//if(true){
				//Count path nodes
				int nPathCount = 0;
				const TRACK* pPath = pBestTrack;
				while(pPath){
					nPathCount++;
					pPath = pPath->m_pFromTrack;
				}
				nPathCount--;
				//SEE BELOW
	// 			if(nPathCount*2 > nOutBufferSize){ //*2 for x/y coordinates to store
	// 				Result = -2; //pOutBuffer is too small
	// 				goto clean;
	// 			}
				//Fill up pOutBuffer and clean
				int idx = nPathCount*2 - 1;
				Result = nPathCount;
				pPath = pBestTrack;
				while(pPath && idx > 0){
					if(idx < nOutBufferSize){//Fill out buffer from start until full. Fill full position only.
						pOutBuffer[idx]		= pPath->m_nY;
						pOutBuffer[idx-1]	= pPath->m_nX;
					}
					idx -= 2;
					pPath = pPath->m_pFromTrack;
				}
				goto clean;
			}
			//Check that the neighbor haven't been already processed
			//tag -2 : not a grid border look for it -1 : on grid border skip
			int SearchedNodesVectorCount = TracksPool.size();
			int iNorthProcessed = pBestTrack->m_nY==Grid.m_nMapHeight-1 ? -2 : -1; 
			int iSouthProcessed = pBestTrack->m_nY==0 ? -2 : -1;
			int iEastProcessed = pBestTrack->m_nX==Grid.m_nMapWidth-1 ? -2 : -1;
			int iWestProcessed = pBestTrack->m_nX==0 ? -2 : -1;
			for(int iNodes = 0 ; iNodes < SearchedNodesVectorCount ; ++iNodes){
				const TRACK& T = *TracksPool[iNodes];
				if(iNorthProcessed>-2 && T.m_nX == pBestTrack->m_nX && T.m_nY == pBestTrack->m_nY+1){
					iNorthProcessed = iNodes;
					if(iNorthProcessed>=0  && iEastProcessed>=0 && iSouthProcessed>=0 && iWestProcessed>=0)
						break;
				}
				if(iEastProcessed>-2 && T.m_nX == pBestTrack->m_nX+1 && T.m_nY == pBestTrack->m_nY){
					iEastProcessed = iNodes;
					if(iNorthProcessed>=0  && iEastProcessed>=0 && iSouthProcessed>=0 && iWestProcessed>=0)
						break;
				}
				if(iSouthProcessed>-2 && T.m_nX == pBestTrack->m_nX && T.m_nY == pBestTrack->m_nY-1){
					iSouthProcessed = iNodes;
					if(iNorthProcessed>=0  && iEastProcessed>=0 && iSouthProcessed>=0 && iWestProcessed>=0)
						break;
				}
				if(iWestProcessed>-2 && T.m_nX == pBestTrack->m_nX-1 && T.m_nY == pBestTrack->m_nY){
					iWestProcessed = iNodes;
					if(iNorthProcessed>=0  && iEastProcessed>=0 && iSouthProcessed>=0 && iWestProcessed>=0)
						break;
				}
			}
			//----
			int nTmpManhanttanDist;
			//----
			//North
			int nNewX = pBestTrack->m_nX;
			int nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iNorthProcessed, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//East
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iEastProcessed, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//South
			nNewX = pBestTrack->m_nX;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iSouthProcessed, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//West
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iWestProcessed, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//----
			ToSearchNodesVector.erase(ToSearchNodesVector.begin()); //remove track from search nodes
		}
		//-----
	clean :
		//Clean track pool
		int TrackPoolCount = TracksPool.size();
		for(int iTrack = 0 ; iTrack < TrackPoolCount ; ++iTrack)
			delete TracksPool[iTrack];
		return Result;
	}
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
	AT::I32 PATHFINDER::FindPath_8Way(const GRID& Grid, const ACTOR_ABC& Actor ,const AT::VEC2Di& Start, const AT::VEC2Di& Target,  AT::I32* pOutBuffer, const int nOutBufferSize){
		//-----
		//Check that destination is attainable
		if(Actor.IsCollisionFree(Grid, Start, Target) != GRID::WALKABLE)
			return -1;
		//-----
		AT::I32				Result = -1; // -1 : failure (target is not reachable) , >0 size of path
		//-----
		std::vector<TRACK*> TracksPool;
		TracksPool.push_back(new TRACK(Start.x, Start.y, 0, ChebyshevDistance(Start.x, Start.y, Target.x, Target.y), NULL));
		//std::make_heap(TracksPool.begin(), TracksPool.end(), compTracks_f);
		//-----
		std::vector<TRACK*> ToSearchNodesVector;
		ToSearchNodesVector.push_back(TracksPool.back());
		//std::make_heap(ToSearchNodesVector.begin(), ToSearchNodesVector.end(), compTracks_f);
		//-----	
		while(ToSearchNodesVector.size()){
			//----
			//Choose best node to build upon
			std::sort(ToSearchNodesVector.begin(), ToSearchNodesVector.end(), compTracks_f);
			const TRACK* pBestTrack = ToSearchNodesVector[0];
			if(pBestTrack->m_nX == Target.x && pBestTrack->m_nY == Target.y){//Target found build back path
				//Count path nodes
				AT::I32 nPathCount = 0;
				const TRACK* pPath = pBestTrack;
				while(pPath){
					nPathCount++;
					pPath = pPath->m_pFromTrack;
				}
				nPathCount--;
				//Fill up pOutBuffer and clean
				int idx = nPathCount*2 - 1;
				Result = nPathCount;
				pPath = pBestTrack;
				while(pPath && idx > 0){
					if(idx < nOutBufferSize){//Fill out buffer from start until full. Fill full position only.
						pOutBuffer[idx]		= pPath->m_nY;
						pOutBuffer[idx-1]	= pPath->m_nX;
					}
					idx -= 2;
					pPath = pPath->m_pFromTrack;
				}
				goto clean;
			}
			//Check that the neighbor haven't been already processed
			//tag -2 : not a grid border look for it -1 : on grid border skip
			AT::I32 SearchedNodesVectorCount = TracksPool.size();
			AT::I32 iNorth		= pBestTrack->m_nY==Grid.m_nMapHeight-1											? -2 : -1; 
			AT::I32 iNorthEast	= pBestTrack->m_nX==Grid.m_nMapWidth-1 || pBestTrack->m_nY==Grid.m_nMapHeight-1 ? -2 : -1; 
			AT::I32 iEast		= pBestTrack->m_nX==Grid.m_nMapWidth-1											? -2 : -1;
			AT::I32 iSouthEast	= pBestTrack->m_nX==Grid.m_nMapWidth-1 || pBestTrack->m_nY==0					? -2 : -1;
			AT::I32 iSouth		= pBestTrack->m_nY==0															? -2 : -1;
			AT::I32 iSouthWest  = pBestTrack->m_nX==0 || pBestTrack->m_nY==0									? -2 : -1;
			AT::I32 iWest		= pBestTrack->m_nX==0															? -2 : -1;
			AT::I32 iNorthWest	= pBestTrack->m_nX==0 || pBestTrack->m_nY==Grid.m_nMapHeight-1					? -2 : -1;
			for(AT::I32 iNodes = 0 ; iNodes < SearchedNodesVectorCount ; ++iNodes){
				const TRACK& T = *TracksPool[iNodes];
// 				if(iNorth>=0  && iNorthEast>=0 && iEast>=0 && iSouthEast>=0 && iSouth>=0 && iSouthWest>=0 && iWest>=0 && iNorthWest>=0)
// 					break;
				if(iNorth>-2 && T.m_nX == pBestTrack->m_nX && T.m_nY == pBestTrack->m_nY+1){
					iNorth = iNodes;
					continue;
				}
				if(iNorthEast>-2 && T.m_nX == pBestTrack->m_nX && T.m_nY+1 == pBestTrack->m_nY+1){
					iNorthEast = iNodes;
					continue;
				}
				if(iEast>-2 && T.m_nX == pBestTrack->m_nX+1 && T.m_nY == pBestTrack->m_nY){
					iEast = iNodes;
					continue;
				}
				if(iSouthEast>-2 && T.m_nX == pBestTrack->m_nX+1 && T.m_nY == pBestTrack->m_nY-1){
					iSouthEast = iNodes;
					continue;
				}
				if(iSouth>-2 && T.m_nX == pBestTrack->m_nX && T.m_nY == pBestTrack->m_nY-1){
					iSouth = iNodes;
					continue;
				}
				if(iSouthWest>-2 && T.m_nX == pBestTrack->m_nX-1 && T.m_nY == pBestTrack->m_nY-1){
					iSouthWest = iNodes;
					continue;
				}
				if(iWest>-2 && T.m_nX == pBestTrack->m_nX-1 && T.m_nY == pBestTrack->m_nY){
					iWest = iNodes;
					continue;
				}
				if(iNorthWest>-2 && T.m_nX == pBestTrack->m_nX-1 && T.m_nY == pBestTrack->m_nY+1){
					iNorthWest = iNodes;
					continue;
				}			
			}
			//----
			AT::I32 nTmpManhanttanDist;
			//----
			//North
			AT::I32 nNewX = pBestTrack->m_nX;
			AT::I32 nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iNorth, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//NorthEast
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iNorthEast, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//East
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iEast, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//SouthEast
			nNewX = pBestTrack->m_nX+1;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iSouthEast, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//South
			nNewX = pBestTrack->m_nX;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iSouth, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//SouthWest
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY-1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iSouthWest, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//West
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iWest, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//NorthWest
			nNewX = pBestTrack->m_nX-1;
			nNewY = pBestTrack->m_nY+1;
			TestAndPush(Grid, Actor, pBestTrack, Target, nNewX, nNewY, iNorthWest, TracksPool, nTmpManhanttanDist, ToSearchNodesVector);
			//----
			ToSearchNodesVector.erase(ToSearchNodesVector.begin()); //remove track from search nodes
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
		int TrackPoolCount = TracksPool.size();
		for(int iTrack = 0 ; iTrack < TrackPoolCount ; ++iTrack)
			delete TracksPool[iTrack];
		return Result;
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------