#pragma once
//---------------------------------------------------------------------------
//Project
#include "./Rendering/RObject_Static_Memory.h"
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//Tools
#include <CommonTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	class R_OBJECT;
	class RENDERER;
	class SHADER_ABC;
	class SCENE{
	protected :
		static const AT::I32 INITIAL_MEMORY_SIZE = 1000000;
		static const AT::I32 MAX_STATIC_OBJECT = 1024;
		
		STATIC_VERTICES_COLOR_POOL_AE	StaticObjectPool; // (x,y,z,r,g,b)
		AT::I32							RenderObjectIdx;
		R_OBJECT*						StaticObject[MAX_STATIC_OBJECT];
		AT::I32							StaticObjectCount;
	public : 
		SCENE();
		~SCENE();
		void Load();
		void PreRender();
		void RenderAtom(RENDERER& R);
		void Reset();
		R_OBJECT* Object(AT::I32 idx){return StaticObject[idx];}
		R_OBJECT* Object(AT::I32 idx) const {return StaticObject[idx];}
		AT::I32 ObjectCount()const  {return StaticObjectCount;}
		AT::I32 GetPositionOffset()const  {return StaticObjectPool.PositionOffset;}
		AT::I32 GetColorOffset() const {return StaticObjectPool.ColorOffset;}
		AT::I32 GetVertexSize() const {return StaticObjectPool.VertexMemSize;}
		STATIC_VERTICES_COLOR_POOL_AE& GetStaticPool() {return StaticObjectPool;}
		void SetModelUniform(AT::I32 UniformModelIndex);
		static AT::I32 SetVertexData(AT::I32F* v, AT::I32 index, AT::I32F x, AT::I32F y, AT::I32F z, AT::I32F r, AT::I32F g, AT::I32F b, AT::I32F a);
		static AT::I32 SetVertexData(AT::I32F* v, AT::I32 index, AT::I32F x, AT::I32F y, AT::I32F z, AT::I32F tx, AT::I32F ty);
		void AddStaticObject(R_OBJECT* pObject);
		//--TEMPORARY--
		R_OBJECT* ObjectZero(){
			if(StaticObjectCount>0)
				return StaticObject[0];
			return NULL;
		}
		//--END TEMPORARY--
		void SCENE::SpawnCube_Quads(AT::I32F Size, AT::I32F*& VerticesData, AT::I32& VerticesCount, GLuint*& ElementsData, AT::I32& ElementsCount, const AT::I32F* ColorRGBA);
		void SCENE::SpawnCube_Lines(AT::I32F Size, AT::I32F*& VerticesData, AT::I32& VerticesCount, GLuint*& ElementsData, AT::I32& ElementsCount);
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------