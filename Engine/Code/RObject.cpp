//---------------------------------------------------------------------------
//Project
#include "RObject.h"
#include "RObject_Static_Memory.h"
#include "Scene.h"
#include "Shader.h"
//Tool
#include <CodeTools.h>
namespace AE{
	//---------------------------------------------------------------------------
	R_OBJECT::~R_OBJECT(){
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers( 1, &m_ebo );
		if(m_Elements)
			delete m_Elements;
	}
	//---------------------------------------------------------------------------
	void R_OBJECT::SetNextElementIdx(AT::I32 idx){
		m_Elements[m_ElementsIndexCount] = idx;
		m_ElementsIndexCount++;
	}
	//---------------------------------------------------------------------------
	void R_OBJECT::LoadMesh(GLuint* DataElements, AT::I32 ElementsCount, AT::I8 bCpyData/*=false*/){
		m_ElementsIndexCount = ElementsCount;
		if(!bCpyData){
			m_Elements = DataElements;
		}else{
			assert(!m_Elements);
			m_Elements = new GLuint [ElementsCount];//destroyed @ object destructor	
			memcpy(m_Elements, DataElements, ElementsCount*sizeof(DataElements));
		}
	}
	//---------------------------------------------------------------------------
	void R_OBJECT::Build(AT::I32F* DataVertices, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_COLOR_POOL_AE& DataPool, AT::I32 DrawMode, AT::I8 bEbo/*=true*/, AT::I8 bEboCpy/*=false*/){
		//--
		m_pStartOfVerticesBuffer = DataPool.AddVertices(DataVertices, VerticesCount);
		m_VerticesCount = VerticesCount;
		//--
		//VAO
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		GL_TOOL::CheckGLError();
		//VBO
		glGenBuffers(1, &m_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, VerticesCount*DataPool.VertexMemSize*sizeof(DataPool.GetBuffer()), m_pStartOfVerticesBuffer, DrawMode);
		GL_TOOL::CheckGLError();
		//EBO
		if(bEbo){
			//--
			LoadMesh(DataElements, ElementsCount, bEboCpy);
			glGenBuffers(1, &m_ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ElementsIndexCount*sizeof(m_Elements), m_Elements, DrawMode);
			GL_TOOL::CheckGLError();
		}
		m_trfModel.Identity();
	}
	//---------------------------------------------------------------------------
	void R_OBJECT::Draw(RENDERER& R){
		if(!m_VerticesCount)
			return;
		m_pShader->Use();
		m_pShader->BindDynamicVertexAttrib(R, *this);
		glBindVertexArray(m_vao);
		GL_TOOL::CheckGLError();
		if(!m_ElementsIndexCount){
			glDrawArrays(m_GLDisplayMode, 0, m_VerticesCount); //Draw regular VBO
		}else{
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			glDrawElements(m_GLDisplayMode, m_ElementsIndexCount, GL_UNSIGNED_INT, 0); //Draw VBO through element indexing
		}
		GL_TOOL::CheckGLError();
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
