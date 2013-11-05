//Project
#include "Scene.h"
#include "./Rendering/Shader.h"
#include "./Rendering/RObject.h"
//Tool
#include <CodeTools.h>
//STD
#include <memory.h>
namespace AE{
	//---------------------------------------------------------------------------
	SCENE::SCENE():RenderObjectIdx(0){
		StaticObjectCount = 0;
		memset(StaticObject, 0, MAX_STATIC_OBJECT*sizeof(R_OBJECT*));
		StaticColorObjectPool.AllocateFromScratch(INITIAL_MEMORY_SIZE);
		StaticTextureObjectPool.AllocateFromScratch(INITIAL_MEMORY_SIZE);
	}
	//---------------------------------------------------------------------------
	SCENE::~SCENE(){
		Reset();
	}
	//---------------------------------------------------------------------------
	AT::I32 SCENE::SetVertexData(AT::I32F* v, AT::I32 index, AT::I32F x, AT::I32F y, AT::I32F z, AT::I32F r, AT::I32F g, AT::I32F b, AT::I32F a){
		index = index*7;
		v[index] = x;	index++;
		v[index] = y;	index++;
		v[index] = z;	index++;
		v[index] = r;	index++;
		v[index] = g;	index++;
		v[index] = b;	index++;
		v[index] = a;	index++;
		return index;
	}
	//---------------------------------------------------------------------------
	AT::I32 SCENE::SetVertexData(AT::I32F* v, AT::I32 index, AT::I32F x, AT::I32F y, AT::I32F z, AT::I32F tx, AT::I32F ty){
		index = index*5;
		v[index] = x;	index++;
		v[index] = y;	index++;
		v[index] = z;	index++;
		v[index] = tx;	index++;
		v[index] = ty;	index++;
		return index;
	}
	//---------------------------------------------------------------------------
	void SCENE::Load(){
		Reset();
	}
	//---------------------------------------------------------------------------
	void SCENE::AddStaticObject(R_OBJECT* R){
		//--
		StaticObject[StaticObjectCount] = R;
		StaticObjectCount++;
		//--		
	}
	//---------------------------------------------------------------------------
	void SCENE::Reset(){
		for(AT::I32 iObj = 0 ; iObj < StaticObjectCount ; ++iObj){
			delete StaticObject[iObj];
			StaticObject[iObj] = NULL;
		}
		StaticColorObjectPool.QuickReset();
	}
	//---------------------------------------------------------------------------
	void SCENE::SetModelUniform(AT::I32 UniformModelIndex){
		for(AT::I32 iObj = 0 ; iObj < StaticObjectCount ; ++iObj){
			StaticObject[iObj]->m_uniformModel = UniformModelIndex;
		}
	}
	//---------------------------------------------------------------------------
	void SCENE::PreRender(){
		RenderObjectIdx=0;
	}
	//---------------------------------------------------------------------------
	void SCENE::RenderAtom(RENDERER_ABC& R){
		R_OBJECT& Object = *StaticObject[RenderObjectIdx];
		Object.Draw(R);
		RenderObjectIdx++;
	}
	//---------------------------------------------------------------------------
	void SCENE::SpawnCube_Quads(AT::I32F Size, AT::I32F*& VerticesData, AT::I32& VerticesCount, GLuint*& ElementsData, AT::I32& ElementsCount, const AT::I32F* ColorRGBA){
		VerticesCount = 8; 
		VerticesData = new AT::I32F[VerticesCount*StaticColorObjectPool.VertexMemSize];
		AT::I32F HalfSize = Size/2.f;
		//Bottom face
		SetVertexData(VerticesData, 0,	-HalfSize, -HalfSize, -HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //bottom left
		SetVertexData(VerticesData, 1,	 HalfSize, -HalfSize, -HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //bottom right
		SetVertexData(VerticesData, 2,	 HalfSize, -HalfSize,  HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //top right
		SetVertexData(VerticesData, 3,	-HalfSize, -HalfSize,  HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //top left
		//Top face											 
		SetVertexData(VerticesData, 4,	-HalfSize,  HalfSize, -HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //bottom left
		SetVertexData(VerticesData, 5,	 HalfSize,  HalfSize, -HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //bottom right
		SetVertexData(VerticesData, 6,	 HalfSize,  HalfSize,  HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //top right
		SetVertexData(VerticesData, 7,	-HalfSize,  HalfSize,  HalfSize, ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3]); //top left
		//---
		//Bottom
		ElementsCount = 24;
		ElementsData = new GLuint[ElementsCount];
		ElementsData[0] = 0;		ElementsData[1] = 1;		ElementsData[2] = 2;	ElementsData[3] = 3;
		//Top
		ElementsData[4] = 4;		ElementsData[5] = 5;		ElementsData[6] = 6;	ElementsData[7] = 7;
		//Front																														
		ElementsData[8] = 0;		ElementsData[9] = 1;		ElementsData[10] = 5;	ElementsData[11] = 4;
		//Back
		ElementsData[12] = 3;		ElementsData[13] = 2;		ElementsData[14] = 6;	ElementsData[15] = 7;
		//Left
		ElementsData[16] = 3;		ElementsData[17] = 0;		ElementsData[18] = 4;	ElementsData[19] = 7;
		//Right
		ElementsData[20] = 2;		ElementsData[21] = 1;		ElementsData[22] = 5;	ElementsData[23] = 6;
	}
	//---------------------------------------------------------------------------
	void SCENE::SpawnCube_Lines(AT::I32F Size, AT::I32F*& VerticesData, AT::I32& VerticesCount, GLuint*& ElementsData, AT::I32& ElementsCount){
		VerticesCount = 8;
		VerticesData = new AT::I32F[VerticesCount*StaticColorObjectPool.VertexMemSize];
		AT::I32F HalfSize = Size/2.f;
		AT::I32F Color[] = { 0.8f, 0.8f, 0.8f, 0.8f };
		//Bottom face
		SetVertexData(VerticesData, 0,	-HalfSize, -HalfSize, -HalfSize, Color[0], Color[1], Color[2], Color[3]); //bottom left
		SetVertexData(VerticesData, 1,	 HalfSize, -HalfSize, -HalfSize, Color[0], Color[1], Color[2], Color[3]); //bottom right
		SetVertexData(VerticesData, 2,	 HalfSize, -HalfSize,  HalfSize, Color[0], Color[1], Color[2], Color[3]); //top right
		SetVertexData(VerticesData, 3,	-HalfSize, -HalfSize,  HalfSize, Color[0], Color[1], Color[2], Color[3]); //top left
		//Top face											 
		SetVertexData(VerticesData, 4,	-HalfSize,  HalfSize, -HalfSize, Color[0], Color[1], Color[2], Color[3]); //bottom left
		SetVertexData(VerticesData, 5,	 HalfSize,  HalfSize, -HalfSize, Color[0], Color[1], Color[2], Color[3]); //bottom right
		SetVertexData(VerticesData, 6,	 HalfSize,  HalfSize,  HalfSize, Color[0], Color[1], Color[2], Color[3]); //top right
		SetVertexData(VerticesData, 7,	-HalfSize,  HalfSize,  HalfSize, Color[0], Color[1], Color[2], Color[3]); //top left
		//---
		ElementsCount = 32;
		ElementsData = new GLuint[ElementsCount];
		//-- Top
		ElementsData[0] = 0;		ElementsData[1] = 1;		ElementsData[2] = 2;	ElementsData[3] = 3;
		ElementsData[4] = 4;		ElementsData[5] = 5;		ElementsData[6] = 6;	ElementsData[7] = 7;
		//-- Left
		ElementsData[8] = 0;		ElementsData[9] = 4;		ElementsData[10] = 1;	ElementsData[11] = 5;
		ElementsData[12] = 2;		ElementsData[13] = 6;		ElementsData[14] = 3;	ElementsData[15] = 7;
		//-- Bottom
		ElementsData[16] = 3;		ElementsData[17] = 0;		ElementsData[18] = 4;	ElementsData[19] = 7;
		ElementsData[20] = 2;		ElementsData[21] = 1;		ElementsData[22] = 5;	ElementsData[23] = 6;
		//-- Right
		ElementsData[24] = 3;		ElementsData[25] = 2;		ElementsData[26] = 0;	ElementsData[27] = 1;
		ElementsData[28] = 4;		ElementsData[29] = 5;		ElementsData[30] = 7;	ElementsData[31] = 6;
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
