#pragma once
//---------------------------------------------------------------------------
//Project
#include "RObject_Static_Memory.h"
//Math
#include <Trf.h>
//Tools
#include <CommonTypes.h>
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class SHADER_ABC;
	class RENDERER;
	class R_OBJECT{
	public :
		//--------------------------------------
		AT::TRFf	m_trfModel;
		//--------------------------------------
		GLuint		m_vbo; //Vertex buffer object
		GLuint		m_vao; //Vertex array object
		GLuint		m_ebo; //Element buffer object
		GLuint		m_uniformModel;//Model matrix
		//--------------------------------------
		void*				m_pStartOfVerticesBuffer;			//Start of the vertices buffer in the STATIC data pool (see Renderer.h)
		AT::U32			m_VerticesCount;				//Number of vertices
		GLuint*			m_Elements;
		AT::U32			m_ElementsIndexCount;
		AT::I32			m_GLDisplayMode;
		SHADER_ABC*	m_pShader;
		GLuint			m_TextureId;
		//--------------------------------------
		//--------------------------------------
		R_OBJECT():m_vbo(-1),m_vao(-1),m_ebo(-1),m_VerticesCount(0),m_Elements(NULL),m_ElementsIndexCount(0),m_uniformModel(-1),m_GLDisplayMode(GL_QUADS),m_pShader(NULL),m_TextureId(-1){}
		~R_OBJECT();
		void LoadMesh(GLuint* DataElements, AT::I32 ElementsCount, AT::I8 bCpyData=false);
		void Build(AT::I32F* Data, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_COLOR_POOL_AE& DataPool, AT::I32 DrawMode, AT::I8 bEbo=true,AT::I8 bEboCpy=false);
		void Build(AT::I32F* Data, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_TEXT_POOL_AE& DataPool, AT::I32 DrawMode, const AT::I8* Filename=NULL, AT::I8 bEbo=true,AT::I8 bEboCpy=false);
		void Draw(RENDERER& R);
	protected :
		void SetNextElementIdx(AT::I32 idx);
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
