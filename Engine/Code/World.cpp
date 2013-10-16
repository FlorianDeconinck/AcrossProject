//---------------
//Project
#include "World.h"
#include "./Rendering/Renderer.h"
#include "./Controller/Controller.h"
#include "./Rendering/RObject.h"
#include "Actor.h"
#include "./Rendering/Shaders.h"
//AT
#include <CodeTools.h>
#include "Vec3D.h"
//STD
#include <fstream>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	//GRID
	//---------------------------------------------------------------------------
	GRID::GRID():m_nMap(NULL){
		assert(MAP_TAG_COUNT>=sizeof(*m_nMap)); 
	}
	//---------------------------------------------------------------------------
	// Format
	//		Width Height
	//		TAG(0xHeight-1) TAG(0xHeight-2) ... TAG(0x0)
	//		...
	//		TAG(0x0) ... TAG(Width-2x0) TAG(Width-1x0)
	void GRID::LoadFromFile(const AT::I8* Filename){
		FILE* GridFile = fopen( Filename, "r");
		if(!GridFile){
			Break();
			return;
		}
		//---
		fscanf(GridFile, "%d %d", &m_nMapWidth, &m_nMapHeight);
		AT::I32 Count = m_nMapWidth*m_nMapHeight;
		//---
		m_nMap = new AT::U8[Count];
		AT::I32 c;
		for(AT::I32 iH = m_nMapHeight-1 ; iH >= 0 ; iH--){
			for(AT::I32 iW = 0 ; iW < m_nMapWidth ; iW++){
				fscanf(GridFile, "%1d ", &c);
				m_nMap[m_nMapWidth*iH+iW] = (grid_tile_t)c;
			}
		}
		//--
		fclose(GridFile);
	}
	//---------------------------------------------------------------------------
	//WORLD
	//---------------------------------------------------------------------------
	WORLD::WORLD(AT::I32F _TileSize/*=0.1f*/):m_TileSize(_TileSize),m_Player0(NULL){
#ifdef _DEBUG
		m_pRGridQuad	= NULL;
		m_pRGridLines	= NULL;
#endif
	}
	//---------------------------------------------------------------------------
	WORLD::~WORLD(){
	}
	//---------------------------------------------------------------------------
	void WORLD::LoadGridFromFile(const AT::I8* Filename){
		LoadFromFile(Filename);
	}
	//---------------------------------------------------------------------------
#ifdef _DEBUG
	void WORLD::DebugRendererLoad(RENDERER& Renderer){
		AT::I32F* pColor;
		AT::I32F Red[] = {0.8f, 0.f, 0.f, 1.f};
		AT::I32F Green[] = {0.f, 0.8f, 0.f, 1.f};
		AT::I32 Idx = 0;
		AT::I32 VerticesCount = m_nMapWidth*m_nMapHeight*4;
		AT::I32F* GridVertices = new AT::I32F[VerticesCount*Renderer.m_Scene.GetVertexSize()];
		for(AT::I32 iH = m_nMapHeight-1  ; iH >=0 ; iH--){
			for(AT::I32 iW = 0 ; iW < m_nMapWidth ; iW++){
				if(GetTile(iW, iH) == WALKABLE)
					pColor =  Green;
				else
					pColor = Red;
				AT::I32F TextX = iW/(AT::I32F)(m_nMapWidth-1);
				AT::I32F TextY = iH/(AT::I32F)(m_nMapHeight-1);
				//One tile
				SCENE::SetVertexData(GridVertices, Idx, (AT::I32F)iW*m_TileSize,	 0.f, (AT::I32F)iH*m_TileSize,		pColor[0], pColor[1], pColor[2], pColor[3]);
				Idx++;
				SCENE::SetVertexData(GridVertices, Idx, (AT::I32F)(iW+1)*m_TileSize, 0.f, (AT::I32F)iH*m_TileSize,		pColor[0], pColor[1], pColor[2], pColor[3]);
				Idx++;
				SCENE::SetVertexData(GridVertices, Idx, (AT::I32F)(iW+1)*m_TileSize, 0.f, (AT::I32F)(iH+1)*m_TileSize,	pColor[0], pColor[1], pColor[2], pColor[3]);
				Idx++;
				SCENE::SetVertexData(GridVertices, Idx, (AT::I32F)iW*m_TileSize,	 0.f, (AT::I32F)(iH+1)*m_TileSize,	pColor[0], pColor[1], pColor[2], pColor[3]);
				Idx++;
			}
		}
		if(m_pRGridQuad)
			delete m_pRGridQuad;
		m_pRGridQuad = new R_OBJECT();
		m_pRGridQuad->m_GLDisplayMode = GL_QUADS;
		m_pRGridQuad->Build(GridVertices, VerticesCount, NULL, 0, Renderer.m_Scene.GetStaticPool(), GL_STREAM_DRAW,false);
		Renderer.InitRObject(*m_pRGridQuad, SHADER_ABC::COLOR_3D_SHADER);
		delete GridVertices;
		//--
		VerticesCount = 2*(m_nMapWidth+1)+2*(m_nMapHeight+1);
		GridVertices = new AT::I32F[VerticesCount*Renderer.m_Scene.GetVertexSize()];
		Idx = 0;
		for(AT::I32 iW = 0  ; iW <= m_nMapWidth ; iW++){
			SCENE::SetVertexData(GridVertices, Idx, iW*m_TileSize, 0.f, 0.f, 0.f, 0.f, 0.f, 0.8f);
			Idx++;
			SCENE::SetVertexData(GridVertices, Idx, iW*m_TileSize, 0.f, m_nMapHeight*m_TileSize, 0.2f, 0.2f, 0.2f, 0.8f);
			Idx++;
		}
		for(AT::I32 iH = 0  ; iH <= m_nMapHeight ; iH++){
			SCENE::SetVertexData(GridVertices, Idx, 0.f, 0.f, iH*m_TileSize, 0.f, 0.f, 0.f, 1.f);
			Idx++;
			SCENE::SetVertexData(GridVertices, Idx, m_nMapWidth*m_TileSize, 0.f, iH*m_TileSize, 0.2f, 0.2f, 0.2f, 0.8f);
			Idx++;
		}
		if(m_pRGridLines)
			delete m_pRGridLines;
		m_pRGridLines = new R_OBJECT();
		m_pRGridLines->m_GLDisplayMode = GL_LINES;
		m_pRGridLines->Build(GridVertices, VerticesCount, NULL, 0, Renderer.m_Scene.GetStaticPool(), GL_STREAM_DRAW, false);
		Renderer.InitRObject(*m_pRGridLines, SHADER_ABC::THICK_LINES_3D_SHADER);
		delete GridVertices;
		//--
	}
#endif
	//---------------------------------------------------------------------------
	void WORLD::Init(RENDERER& R){
		//Take ptr on renderer
		m_pRenderer = &R;
		//Load basic grid
		LoadGridFromFile("../../../Asset/BasicGrid.agd");
#ifdef _DEBUG
		//Load display debug for grid
		DebugRendererLoad(R);
#endif
		//Load Player 0
// 		m_Player0 = new PLAYER();
// 		m_Player0->LoadMeshs(*this, R);
	}
	//---------------------------------------------------------------------------
	void WORLD::Update(AT::I64F elapsedTime_ms, const CONTROLLER& C){
		//Update NPC
		AT::I32 NpcCount = m_NPCArrays.size();
		for(int iNPC = 0 ; iNPC < NpcCount ; ++iNPC){
			NPC& Npc = *m_NPCArrays[iNPC];
			//Update position & grid occupation
			Npc.Update(*this, elapsedTime_ms, m_TileSize);
		}
		//Update input
		if(m_Player0){
			m_Player0->m_DirectonInput.x = -C.m_Xbox.m_leftStick.x;
			m_Player0->m_DirectonInput.y = C.m_Xbox.m_leftStick.y;
			//Update player
			m_Player0->Update(*this, elapsedTime_ms, m_TileSize);
		}
	}
	//--------------------------------------------------------------------------
	void WORLD::RenderNPC(RENDERER& R, int NPCIdx){
		NPC& Npc = *m_NPCArrays[NPCIdx];
		Npc.Draw(R, m_TileSize);
		GL_TOOL::CheckGLError();
	}
	//--------------------------------------------------------------------------
	void WORLD::RenderPlayer(RENDERER& R){
		if(!m_Player0)
			return;
		m_Player0->Draw(R, m_TileSize);
		GL_TOOL::CheckGLError();
	}
	//--------------------------------------------------------------------------
	void WORLD::DebugDraw(RENDERER& R){
		if(!R.m_bDrawDebug)
			return;
		if(m_pRGridLines && m_pRGridQuad){
			glBindBuffer(GL_ARRAY_BUFFER, m_pRGridQuad->m_vbo);
			GLfloat* pBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			if(pBuffer){
				AT::I32F* pColor;
				AT::I32F Red[] = {0.8f, 0.f, 0.f, 1.f};
				AT::I32F Green[] = {0.f, 0.8f, 0.f, 1.f};
				AT::I32F LightBlue[] = {0.11f, 0.56f, 1.f, 1.f}; //30 144 255
				AT::I32F LightPurple[] = {0.8f, 0.2f, 1.f, 1.f};//204 51 255
				AT::I32F Black[] = {0.f, 0.f, 0.f, 1.f};
				AT::I32 Idx = 0;
				AT::I32F iPathFindDebugCount=0;
				for(AT::I32 iH = m_nMapHeight-1  ; iH >=0 ; iH--){
					for(AT::I32 iW = 0 ; iW < m_nMapWidth ; iW++){
						switch( GetTile(iW, iH) ){
						case WALKABLE :
							pColor = Green;
							break;
						case STATIC_OBSTACLE :
							pColor = Red;
							break;
						case DEBUG_PATHFIND :
							pColor = LightBlue;
							pColor[1] = (1.f - LightBlue[1])*iPathFindDebugCount + LightBlue[1];
							iPathFindDebugCount+=0.01f;
							break;
						case PATH:
							pColor = LightPurple;
							break;
						default:
							pColor = Black;
							break;
						}
						memcpy(&pBuffer[Idx+3], pColor, 4*sizeof(AT::I32F));
						Idx+=7;
						memcpy(&pBuffer[Idx+3], pColor, 4*sizeof(AT::I32F));
						Idx+=7;
						memcpy(&pBuffer[Idx+3], pColor, 4*sizeof(AT::I32F));
						Idx+=7;
						memcpy(&pBuffer[Idx+3], pColor, 4*sizeof(AT::I32F));
						Idx+=7;
					}
				}
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}
			assert(pBuffer);
			m_pRGridLines->Draw(R);
			m_pRGridQuad->Draw(R);
		}
		GL_TOOL::CheckGLError();		
	}
	//--------------------------------------------------------------------------
	// WORLD : Game interface
	//--------------------------------------------------------------------------
	void WORLD::SpawnNPC(const AT::VEC2Di& Position/*=AT::VEC2Di(0,0)*/, const AT::I32F* ColorRGBA/*=NULL*/){
		NPC* pNpc0 = new NPC();
		pNpc0->LoadMeshs(*this, *m_pRenderer, ColorRGBA);
		pNpc0->SetPosition(*this, Position);
		m_NPCArrays.push_back(pNpc0);
	}
	//---------------------------------------------------------------------------
	void WORLD::SetTileStatus(AT::VEC2Di tilePos, MAP_TAG S){
		SetTile(tilePos.x, tilePos.y, S);
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------