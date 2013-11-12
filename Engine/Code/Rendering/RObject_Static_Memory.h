//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tools
#include <AcrossTypes.h>
//STD
#include <cstdlib>
//---------------------------------------------------------------------------
namespace AE{
	template <typename sreal_t, AT::I32 __VertexMemSize, AT::I32 __PositionOffset, AT::I32 __ColOrTextOffset> class STATIC_VERTICES_POOL{
	protected :
		sreal_t*		Buffer;
		int				MemSize;		
	public :
		int				Count;
		static const AT::I32 PositionOffset		= __PositionOffset;
		static const AT::I32 ColOrTextOffset	= __ColOrTextOffset;
		static const AT::I32 VertexMemSize		= __VertexMemSize;
		//------------------------------------
		//------------------------------------
	public :
		STATIC_VERTICES_POOL():Buffer(0), MemSize(0), Count(0){}
		//--------------
		STATIC_VERTICES_POOL::~STATIC_VERTICES_POOL(){
			FullReset();
		}
		//--------------
		void AllocateFromScratch(AT::I32 _MemSize){
			if(Buffer)
				free(Buffer);
			MemSize = _MemSize;
			Buffer = (sreal_t*)malloc(sizeof(sreal_t)*MemSize);
			Count = 0;
		}
		//--------------
		sreal_t* /**/ AddOneVertex(sreal_t* Vertex){
			if((Count+1)*VertexMemSize > MemSize)
				ReAlloc();
			//--
			sreal_t* r = &Buffer[Count];
			memcpy(&Buffer[Count], Vertex, VertexMemSize*sizeof(sreal_t));
			//--
			Count++;
			return r;
		}
		//--------------
		sreal_t* /**/ AddVertices(sreal_t* Vertices, AT::I32 VerticesCount){
			if((Count+1)*VertexMemSize+VerticesCount*VertexMemSize > MemSize ){
				ReAlloc();
				return AddVertices(Vertices, VerticesCount);
			}
			//--
			int OnBufferIdx = Count * VertexMemSize;
			sreal_t* r = &Buffer[OnBufferIdx];
			memcpy(r, Vertices, VerticesCount*VertexMemSize*sizeof(sreal_t));
			//--
			Count+=VerticesCount;
			return r;
		}
		//--------------
		void ReAlloc(){
			sreal_t* tmpPool = new sreal_t[2*MemSize];
			memcpy(tmpPool, Buffer, MemSize*sizeof(sreal_t));
			free(Buffer);
			Buffer = tmpPool;
			MemSize *= 2;
		}
		//--------------
		sreal_t* GetBuffer(){return Buffer;}
		//--------------
		void QuickReset(){
			Count = 0;
		}
		//--------------
		void FullReset(){
			free(Buffer);
			MemSize = 0;
			Count = 0;
		}
		//--------------
	};
	//---------------------------------------------------------------------------
	typedef STATIC_VERTICES_POOL<AT::I32F, 7, 0, 3> STATIC_VERTICES_COLOR_POOL_AE;
	typedef STATIC_VERTICES_POOL<AT::I32F, 5, 0, 3> STATIC_VERTICES_TEXT_POOL_AE;
}//namespace AE
//---------------------------------------------------------------------------
