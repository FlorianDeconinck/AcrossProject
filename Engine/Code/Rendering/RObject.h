#pragma once
//---------------------------------------------------------------------------
//Project
#include "RObject_Static_Memory.h"
//Math
#include <Trf.h>
#include <Vec2D.h>
//Tools
#include <AcrossTypes.h>
#include <StackAllocator.h>
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class SHADER_ABC;
	class RENDERER_ABC;
	class RESOURCE_MANAGER_ABC;
	class R_OBJECT{
	public :
		//--------------------------------------
		AT::TRFf		m_trfModel;
		//--------------------------------------
		GLuint			m_vbo;							//Vertex buffer object
		GLuint			m_vao;							//Vertex array object
		GLuint			m_ebo;							//Element buffer object
		GLuint			m_uniformModel;					//Model matrix
		//--------------------------------------
		void*			m_pVerticesBuffer;				
		AT::U32			m_VerticesCount;				//Number of vertices
		GLuint*			m_Elements;
		AT::U32			m_ElementsIndexCount;
		AT::I32			m_GLDisplayMode;
		SHADER_ABC*		m_pShader;
		GLuint			m_TextureId;
		AT::VEC2Df		m_UVOffset;
		//--------------------------------------
		//--------------------------------------
		R_OBJECT():m_vbo(-1),m_vao(-1),m_ebo(-1),m_VerticesCount(0),m_Elements(NULL),m_ElementsIndexCount(0),m_uniformModel(-1),m_GLDisplayMode(GL_QUADS),m_pShader(NULL),m_TextureId(-1),m_pVerticesBuffer(NULL),m_UVOffset(0,0){}
		~R_OBJECT();
		void LoadMesh(GLuint* DataElements, AT::I32 ElementsCount, AT::I8 bCpyData=false);
		void Build(AT::I32F* Data, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_COLOR_POOL_AE& DataPool, AT::I32 DrawMode, AT::I8 bEbo=true,AT::I8 bEboCpy=false);
		void Build(AT::I32F* Data, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_TEXT_POOL_AE& DataPool, AT::I32 DrawMode, const AT::I8* Filename=NULL, AT::I8 bEbo=true,AT::I8 bEboCpy=false);
		void Build(AT::I32F* DataVertices, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, AT::I32 DrawMode, const AT::I8* TextureFilename=NULL);
		void Draw(RENDERER_ABC& R);
		//--------------------------------------
		static R_OBJECT* CreateRObject(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& ResourceManager, const char* sResourceName, AT::VEC3Df& BBoxMin, AT::VEC3Df& BBoxMax);
		//--------------------------------------
	protected :
		void SetNextElementIdx(AT::I32 idx);
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
