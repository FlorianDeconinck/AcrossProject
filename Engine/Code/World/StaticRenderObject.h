#pragma once
//---------------------------------------------------------------------------
//Project
#include "../Rendering/RObject_Static_Memory.h"
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//Tools
#include <AcrossTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	class R_OBJECT;
	class RENDERER_ABC;
	class SHADER_ABC;
	class STATIC_RENDER_OBJECT{
	protected :
		static const AT::I32 INITIAL_MEMORY_SIZE = 1000000;
		static const AT::I32 MAX_STATIC_OBJECT = 1024;
		
		STATIC_VERTICES_COLOR_POOL_AE	StaticColorObjectPool; // (x,y,z,r,g,b)
		STATIC_VERTICES_TEXT_POOL_AE	StaticTextureObjectPool; // (x,y,z,tx,ty)
		R_OBJECT*						StaticObject[MAX_STATIC_OBJECT];
		AT::I32							StaticObjectCount;
	public : 
		STATIC_RENDER_OBJECT();
		~STATIC_RENDER_OBJECT();
		//--
		inline STATIC_VERTICES_COLOR_POOL_AE& GetStaticColorObjectPool() {return StaticColorObjectPool;}
		inline AT::I32 GetColorPositionOffset()const					 {return StaticColorObjectPool.PositionOffset;}
		inline AT::I32 GetColorVertexSize() const						 {return StaticColorObjectPool.VertexMemSize;}
		inline AT::I32 GetColorOffset() const							 {return StaticColorObjectPool.ColOrTextOffset;}
		//--
		inline STATIC_VERTICES_TEXT_POOL_AE& GetStaticTextureObjectPool()	{return StaticTextureObjectPool;}
		inline AT::I32 GetTexturePositionOffset()const						{return StaticTextureObjectPool.PositionOffset;}
		inline AT::I32 GetTextureVertexSize() const							{return StaticTextureObjectPool.VertexMemSize;}
		inline AT::I32 GetTextureOffset()const								{return StaticTextureObjectPool.ColOrTextOffset;}
		//--
		static AT::I32 SetVertexData(AT::I32F* v, AT::I32 index, AT::I32F x, AT::I32F y, AT::I32F z, AT::I32F r, AT::I32F g, AT::I32F b, AT::I32F a);
		static AT::I32 SetVertexData(AT::I32F* v, AT::I32 index, AT::I32F x, AT::I32F y, AT::I32F z, AT::I32F tx, AT::I32F ty);
		void STATIC_RENDER_OBJECT::SpawnCube_Quads(AT::I32F Size, AT::I32F*& VerticesData, AT::I32& VerticesCount, GLuint*& ElementsData, AT::I32& ElementsCount, const AT::I32F* ColorRGBA);
		void STATIC_RENDER_OBJECT::SpawnCube_Lines(AT::I32F Size, AT::I32F*& VerticesData, AT::I32& VerticesCount, GLuint*& ElementsData, AT::I32& ElementsCount);
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------