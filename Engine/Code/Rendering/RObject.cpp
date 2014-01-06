//---------------------------------------------------------------------------
//Project
#include "RObject.h"
#include "RObject_Static_Memory.h"
#include "Shader.h"
#include "../World/StaticRenderObject.h"
#include "../ResourceManager/Asset_Types.h"
#include "../Manager_Interface.h"
#include "../Renderer_Interface.h"
//Tool
#include <CodeTools.h>
#include <new>
//Platform
#include <windows.h>
//DevIL
#include <IL/il.h>
namespace AE{
	//---------------------------------------------------------------------------
	R_OBJECT::~R_OBJECT(){
		glDeleteBuffers(1, &m_vbo);
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers( 1, &m_ebo );
		if(m_Elements)
			delete m_Elements;
		if(m_TextureId)
			glDeleteTextures(1, &m_TextureId);
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
	void R_OBJECT::Build(AT::I32F* Data, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_TEXT_POOL_AE& DataPool, AT::I32 DrawMode, const AT::I8* TextureFilename/*=NULL*/, AT::I8 bEbo/*=true*/,AT::I8 bEboCpy/*=false*/){
		//--
		m_pVerticesBuffer = DataPool.AddVertices(Data, VerticesCount);
		m_VerticesCount = VerticesCount;
		//--
		//VAO
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		GL_TOOL::CheckGLError();
		//VBO
		glGenBuffers(1, &m_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, VerticesCount*DataPool.VertexMemSize*sizeof(DataPool.GetBuffer()), m_pVerticesBuffer, DrawMode);
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
		//TRF
		m_trfModel.Identity();
		//TEXTURE
		if(TextureFilename){
			glGenTextures(1, &m_TextureId);
			ILuint ilTexid;
			ilGenImages(1, &ilTexid); 
			ilBindImage(ilTexid); 
			if(ilLoadImage(TextureFilename) && ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)){
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_TextureId);
				//clamp
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				//texture filtering
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
				glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
				glGenerateMipmap(GL_TEXTURE_2D);
			}else{
				Break();
				m_TextureId = -1;
			}
			ilDeleteImage(ilTexid);
		}
	}
	void R_OBJECT::Build(AT::I32F* DataVertices, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, STATIC_VERTICES_COLOR_POOL_AE& DataPool, AT::I32 DrawMode, AT::I8 bEbo/*=true*/, AT::I8 bEboCpy/*=false*/){
		//--
		m_pVerticesBuffer = DataPool.AddVertices(DataVertices, VerticesCount);
		m_VerticesCount = VerticesCount;
		//--
		//VAO
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		GL_TOOL::CheckGLError();
		//VBO
		glGenBuffers(1, &m_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, VerticesCount*DataPool.VertexMemSize*sizeof(DataPool.GetBuffer()), m_pVerticesBuffer, DrawMode);
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
		//TRF
		m_trfModel.Identity();
	}
	//---------------------------------------------------------------------------
	void R_OBJECT::Build(AT::I32 pixelInformationLength, AT::I32F* DataVertices, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, AT::I32 DrawMode, const AT::I8* TextureFilename/*=NULL*/){
		m_Elements = DataElements;
		m_ElementsIndexCount = ElementsCount;
		m_VerticesCount = VerticesCount;
		m_pVerticesBuffer = DataVertices;
		//VAO
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		GL_TOOL::CheckGLError();
		//VBO
		glGenBuffers(1, &m_vbo); 
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, VerticesCount*pixelInformationLength*sizeof(AT::I32F), DataVertices, DrawMode);
		GL_TOOL::CheckGLError();
		if(ElementsCount){
			glGenBuffers(1, &m_ebo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_ElementsIndexCount*sizeof(m_Elements), m_Elements, DrawMode);
		}
		//TRF
		m_trfModel.Identity();
		//TEXTURE
		if(TextureFilename){
			glGenTextures(1, &m_TextureId);
			ILuint ilTexid;
			ilGenImages(1, &ilTexid); 
			ilBindImage(ilTexid); 
			if(ilLoadImage(TextureFilename) && ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE)){
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, m_TextureId);
				//clamp
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				//texture filtering
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
				glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
				glGenerateMipmap(GL_TEXTURE_2D);
			}else{
				Break();
				m_TextureId = -1;
			}
			ilDeleteImage(ilTexid);
		}	
	}
	//---------------------------------------------------------------------------
	void R_OBJECT::Draw(RENDERER_ABC& R){
		if(!m_VerticesCount)
			return;

		if (m_pShader){
			m_pShader->Use();
			m_pShader->BindDynamicVertexAttrib(R, *this);
			m_pShader->BindDynamicFragmentAttrib(R, this);
		}
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
