//-----------------------------------------------------------------------------
//Project
#include "Actor.h"
#include "./Rendering/Renderer_Interface.h"
#include "World.h"
#include "./AI/Pathfinder.h"
#include "./Animation/Animator.h"
#include "./Animation/SpriteAnimator.h"
//STD
#include <cmath>
#include <random>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	// ABC_ACTOR
	//-----------------------------------------------------------------------------
	const AT::VEC2Di ACTOR_ABC::m_Dir_North(0, 1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_North_East(1, 1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_East(1, 0);
	const AT::VEC2Di ACTOR_ABC::m_Dir_South_East(1, -1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_South(0, -1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_South_West(-1, -1);
	const AT::VEC2Di ACTOR_ABC::m_Dir_West(-1, 0);
	const AT::VEC2Di ACTOR_ABC::m_Dir_North_West(-1, 1);
	//-----------------------------------------------------------------------------
	ACTOR_ABC::ACTOR_ABC():m_Position(0,0),m_LastPosition(0,0),m_MeshsCount(0),m_Speed(1.0),m_InnerTilePosition(0,0),m_PreviousInnerTilePosition(0,0),m_pAnimator(NULL){
#ifdef _DEBUG
		m_bDebugPathfind = false;
#endif
	}
	//-----------------------------------------------------------------------------
	ACTOR_ABC::~ACTOR_ABC(){
		delete m_pAnimator;
	}
	//-----------------------------------------------------------------------------
	void ACTOR_ABC::LoadDefaultMeshs(GRID& Grid, RENDERER_ABC& R, const AT::I32F* ColorRGBA/*=NULL*/){
		AT::I32F* pCubeData=NULL;
		AT::I32 CubeVerticesCount;
		GLuint* pCubeDataElements=NULL;
		AT::I32 CubeElementsCount;
		//---
		m_MeshsCount = 2;
		//--
		AT::I32F ColorRGBA_Default[] = {1.f, 1.f, 1.f, 0.8f};
		//--
		R.GetScene().SpawnCube_Quads(0.5f, pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, ColorRGBA ? ColorRGBA : ColorRGBA_Default);
		m_Meshs[0] = new R_OBJECT();
		m_Meshs[0]->Build(pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, R.GetScene().GetStaticColorObjectPool(), GL_STATIC_DRAW, true, true);
		m_Meshs[0]->m_trfModel.SetT(0.f, 1.0f, 0.f);
		R.InitRObject(*m_Meshs[0], SHADER_ABC::COLOR_3D_SHADER);
		delete pCubeDataElements;
		delete pCubeData;
		//--
		R.GetScene().SpawnCube_Lines(0.5f, pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount);
		m_Meshs[1] = new R_OBJECT();
		m_Meshs[1]->m_GLDisplayMode = GL_LINES;
		m_Meshs[1]->Build(pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, R.GetScene().GetStaticColorObjectPool(), GL_STATIC_DRAW, true, true);
		m_Meshs[1]->m_trfModel.SetT(0.f, 1.0f, 0.f);
		R.InitRObject(*m_Meshs[1], SHADER_ABC::THICK_LINES_3D_SHADER);
		delete pCubeDataElements;
		delete pCubeData;	
		//--
		m_BBox.Init(Grid, m_Position, 3, 3);
	}
	//-----------------------------------------------------------------------------
	void ACTOR_ABC::Draw(const WORLD& W, RENDERER_ABC& R, AT::I32F TileSize){
		//------
		//Update animation
		m_pAnimator->Update(W, *this, ANIMATOR_ABC::WALK);
		//------
		//Draw
		for(AT::I32 iRO = 0 ; iRO < m_MeshsCount ; iRO++){
			m_Meshs[iRO]->Draw(R);
		}
	}
	//-----------------------------------------------------------------------------
	AT::I8 ACTOR_ABC::IsWithinBorders(const GRID& Grid, const AT::VEC2Di& Pos) const { 
		return Pos.x-m_BBox.m_HalfWidth >= 0 &&  Pos.x+m_BBox.m_HalfWidth < Grid.m_nMapWidth && Pos.y-m_BBox.m_HalfHeight >= 0 && Pos.y+m_BBox.m_HalfHeight < Grid.m_nMapHeight; 
	}
	//-----------------------------------------------------------------------------
	void ACTOR_ABC::SetAnimatorModule(ANIMATOR_ABC* pAnimator){
		m_pAnimator = pAnimator; m_pAnimator->PopulateAnimationDictionnary(); 
	}
	//-----------------------------------------------------------------------------
	// NPC
	//-----------------------------------------------------------------------------
	NPC::NPC():m_Destination(0, 0),m_NextMove(0,0),m_sizePath(0),m_iPath(0){
		m_bRecomputePath = false;
		//Configuration of ANIMATOR
		SetAnimatorModule((ANIMATOR_ABC*)new DEFAULT_ANIMATOR());
	}
	//-----------------------------------------------------------------------------
	AT::I32F trunc(AT::I32F a){
		return a>0 ? std::floor(a) : ceil(a);
	//-----------------------------------------------------------------------------
	}
	void NPC::Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize){
		if(m_Position == m_Destination)
			return;
		//------
		if(m_bRecomputePath){
			m_sizePath = PATHFINDER::FindPath_8Way(Grid, *this, m_Position, m_Destination, m_Path, PATH_MAX_NODES);
			if(m_sizePath<=0)
				return;
			m_iPath = 1;
			while(m_NextMove==m_Position)
				m_NextMove = m_Path[++m_iPath];
			m_bRecomputePath = false;
		}else if(m_Position == m_NextMove){
			while(m_NextMove==m_Position && (AT::I32)m_iPath < m_sizePath)
				m_NextMove = m_Path[++m_iPath];
		}
		//------
		//Update position
		AT::VEC2Di DirTiles			 = m_Position - m_NextMove;
		AT::VEC2Df DirRealWorld(DirTiles.x*tileSize, DirTiles.y*tileSize);
		if(!DirRealWorld.IsZero())
			DirRealWorld.Normalize();
		m_PreviousInnerTilePosition  = m_InnerTilePosition;
		m_InnerTilePosition			+=  DirRealWorld*(AT::I32F)(m_Speed*(elapsedTime_ms/1000.0));
		//--
		//--
		AT::VEC2Di PreviousPosition(-1, -1);
		if(m_InnerTilePosition.x >= tileSize || m_InnerTilePosition.x < 0){
			if((m_InnerTilePosition.x > 0 && m_Position.x != Grid.m_nMapWidth-1) || (m_InnerTilePosition.x < 0 && m_Position.x!=0)){
				PreviousPosition = m_Position;
				//--
				AT::I32F XOffset = m_InnerTilePosition.x/tileSize;
				AT::I32 TruncXOffset = 0;
				if(XOffset>0)
					TruncXOffset = (AT::I32)trunc(XOffset);
				else
					TruncXOffset = -(AT::I32)ceil(-XOffset);
				if(!IsCollisionFreeFromSelf(Grid, m_Position+AT::VEC2Di(TruncXOffset, 0))){
					m_InnerTilePosition.x = m_InnerTilePosition.x > tileSize/2.f ? tileSize : 0;
				}else{
					m_Position.x += TruncXOffset;
					m_InnerTilePosition.x -= TruncXOffset*tileSize;
				}
			}else{
				m_InnerTilePosition.x = m_PreviousInnerTilePosition.x;
			}
		}
		//--
		if(m_InnerTilePosition.y >= tileSize || m_InnerTilePosition.y < 0){
			if((m_InnerTilePosition.y > 0 && m_Position.y != Grid.m_nMapHeight-1) || (m_InnerTilePosition.y < 0 && m_Position.y!=0)){
				if(PreviousPosition.x < 0)
					PreviousPosition  = m_Position;
				//--
				AT::I32F YOffset = m_InnerTilePosition.y/tileSize;
				AT::I32 TruncYOffset = 0;
				if(TruncYOffset > 0)
					TruncYOffset = (AT::I32)trunc(YOffset);
				else
					TruncYOffset = -(AT::I32)ceil(-YOffset);
				if(!IsCollisionFreeFromSelf(Grid, m_Position+AT::VEC2Di(0, TruncYOffset))){
					m_InnerTilePosition.y = m_InnerTilePosition.y > tileSize/2.f ? tileSize : 0;
				}else{
					m_Position.y += TruncYOffset;
					m_InnerTilePosition.y -= TruncYOffset*tileSize;
				}
			}else{
				m_InnerTilePosition.y = m_PreviousInnerTilePosition.y;
			}
		}
		//------
		//Update grid occupation if needed
		if(PreviousPosition.x>=0){
			m_BBox.UpdateGridOccupation(Grid, PreviousPosition, m_Position);
			m_LastPosition = PreviousPosition;
		}
	}
	//-----------------------------------------------------------------------------
	void NPC::LoadMeshs(WORLD& World, void* pBufferFromFile, RENDERER_ABC& Renderer){
		char* ptr = (char*)pBufferFromFile;
		//Load bounding box & compute grid-occupation bounding box
		AT::VEC3Df Min(((AT::I32F*)ptr)[0], ((AT::I32F*)ptr)[1], ((AT::I32F*)ptr)[2]);
		ptr += 3*sizeof(AT::I32F);
		AT::VEC3Df Max(((AT::I32F*)ptr)[0], ((AT::I32F*)ptr)[1], ((AT::I32F*)ptr)[2]);
		ptr += 3*sizeof(AT::I32F);
		AT::I32 HalfWidth = (AT::I32)((Max.x - Min.x)/(2*World.GetTileSize()));
		AT::I32 HalfHeight = (AT::I32)((Max.z - Min.z)/(2*World.GetTileSize()));
		m_BBox.Init(World, m_Position, HalfWidth, HalfHeight);
		//Load UV channels count
		AT::I32 nUV = *(AT::I32*)ptr;
		ptr += sizeof(AT::I32);
		AT::I32 pixelInformationLength;
		SHADER_ABC::SHADERS_ID Shader;
		if(nUV==1){
			pixelInformationLength = 5;	//vertex 3d position + uv 
			Shader = SHADER_ABC::TEXTURE_3D_SHADER;
		}else if(nUV!=0){
			assert(false);				//multiple uv channels, not handled
			return;
		}else{
			pixelInformationLength = 7; //vertex 3d position + 4d color vector
			Shader = SHADER_ABC::COLOR_3D_SHADER;
		}
		//Load Vertices
		AT::I32 VerticeCount = *(AT::I32*)ptr;
		ptr += sizeof(VerticeCount);
		AT::I32F* pVerticesBuffer = (AT::I32F*)ptr;
		ptr += VerticeCount*pixelInformationLength*sizeof(AT::I32F);
		//Load Indices
		AT::I32 IndicesCount = *(AT::I32*)ptr;
		ptr += sizeof(IndicesCount);
		GLuint* pIndicesBuffer = (GLuint*)ptr;
		//---
		m_MeshsCount = 1;
		//--
		m_Meshs[0] = new R_OBJECT();
		m_Meshs[0]->Build(pVerticesBuffer, VerticeCount, pIndicesBuffer, IndicesCount, GL_STATIC_DRAW, "CompanionCube/mesh/companion-cube/tex/companion_cubeColor.png");
		m_Meshs[0]->m_trfModel.SetT(0.f, 1.0f, 0.f);
		m_Meshs[0]->m_trfModel.ToGL();
		m_Meshs[0]->m_GLDisplayMode = GL_TRIANGLES;
		Renderer.InitRObject(*m_Meshs[0], Shader);
		//--
// 		R.GetScene().SpawnCube_Lines(0.5f, pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount);
// 		m_Meshs[1] = new R_OBJECT();
// 		m_Meshs[1]->m_GLDisplayMode = GL_LINES;
// 		m_Meshs[1]->Build(pCubeData, CubeVerticesCount, pCubeDataElements, CubeElementsCount, R.GetScene().GetStaticColorObjectPool(), GL_STATIC_DRAW, true, true);
// 		m_Meshs[1]->m_trfModel.SetT(0.f, 1.0f, 0.f);
// 		R.InitRObject(*m_Meshs[1], SHADER_ABC::THICK_LINES_3D_SHADER);
// 		delete pCubeDataElements;
// 		delete pCubeData;	
		//--
//		m_BBox.Init(Grid, m_Position, 3, 3);
	}
	//-----------------------------------------------------------------------------
	// PLAYER
	//-----------------------------------------------------------------------------
	PLAYER::PLAYER(){
		m_DirectonInput.Set(0, 0);
		m_Position.Set(10, 10);
		//Configure animator
		SetAnimatorModule((ANIMATOR_ABC*)new SPRITE_ANIMATOR());
	}
	//-----------------------------------------------------------------------------
	void PLAYER::LoadDefaultMeshs(GRID& Grid, RENDERER_ABC& R, const AT::I32F* ColorRGBA/*=NULL*/){
		//---
		m_MeshsCount = 1;
		//--
		AT::I32   VerticesCount = 4; 
		AT::I32F* VerticesData = new AT::I32F[VerticesCount*R.GetScene().GetTextureVertexSize()];
		AT::I32F  HalfSize = 0.3f;
		AT::I32F  Height = 1.f;
		SCENE::SetVertexData(VerticesData, 0,		 -HalfSize,		0.f, 0.05f,			 0, 0.125f);		//0,1
		SCENE::SetVertexData(VerticesData, 1,			HalfSize,		0.f, 0.05f, 0.083f, 0.125f);		//1,1
		SCENE::SetVertexData(VerticesData, 2,			HalfSize,		1.f, 0.05f, 0.083f, 0);					//1,0
		SCENE::SetVertexData(VerticesData, 3,		 -HalfSize,		1.f, 0.05f,			 0, 0);					//0,0
		m_Meshs[0] = new R_OBJECT();
		m_Meshs[0]->Build(VerticesData, VerticesCount, NULL, 0, R.GetScene().GetStaticTextureObjectPool(), GL_STREAM_DRAW, "Alex_8D_zps374573dc.png", false, false);
		m_Meshs[0]->m_trfModel.SetT(0.f, 0.f, 0.f);
		m_Meshs[0]->m_GLDisplayMode = GL_QUADS;
		R.InitRObject(*m_Meshs[0], SHADER_ABC::TEXTURE_3D_SHADER);
		delete VerticesData;
		//--
		m_BBox.Init(Grid, m_Position, 2, 2);		
	}
	//-----------------------------------------------------------------------------
	void PLAYER::Update(GRID& Grid, AT::I64F elapsedTime_ms, AT::I32F tileSize){
		if(m_DirectonInput.IsZero())
			return;
		//------
		//Update position
		m_PreviousInnerTilePosition = m_InnerTilePosition;
		m_InnerTilePosition	+=  m_DirectonInput*(AT::I32F)(m_Speed*(elapsedTime_ms/1000.0));
		//--
		AT::VEC2Di PreviousPosition(-1, -1);
		if(m_InnerTilePosition.x >= tileSize || m_InnerTilePosition.x < 0){
			AT::I32 XOffset = (AT::I32)trunc(m_InnerTilePosition.x/tileSize);
			if(((m_InnerTilePosition.x > 0 && m_Position.x != Grid.m_nMapWidth-1) || (m_InnerTilePosition.x < 0 && m_Position.x!=0)) && m_BBox.IsCollisionFree(Grid, m_Position, AT::VEC2Di(m_Position.x+XOffset, m_Position.y))){
				PreviousPosition = m_Position;
				//--
				m_Position.x += (AT::I32)XOffset;
				m_InnerTilePosition.x -= XOffset*tileSize;
			}else{
				m_InnerTilePosition.x = m_PreviousInnerTilePosition.x;
			}
		}
		//--
		if(m_InnerTilePosition.y >= tileSize || m_InnerTilePosition.y < 0){
			AT::I32 YOffset = (AT::I32)trunc(m_InnerTilePosition.y/tileSize);
			if(((m_InnerTilePosition.y > 0 && m_Position.y != Grid.m_nMapHeight-1) || (m_InnerTilePosition.y < 0 && m_Position.y!=0)) && m_BBox.IsCollisionFree(Grid, m_Position, AT::VEC2Di(m_Position.x, m_Position.y+YOffset))){
				if(PreviousPosition.x < 0)
					PreviousPosition  = m_Position;
				//--
				m_Position.y += YOffset;
				m_InnerTilePosition.y -= YOffset*tileSize;
			}else{
				m_InnerTilePosition.y = m_PreviousInnerTilePosition.y;
			}
		}
		//------
		//Update grid occupation if needed
		if(PreviousPosition.x>=0){
			m_BBox.UpdateGridOccupation(Grid, PreviousPosition, m_Position);
			m_LastPosition = PreviousPosition;
		}
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-------------------------------------------------------------------------------