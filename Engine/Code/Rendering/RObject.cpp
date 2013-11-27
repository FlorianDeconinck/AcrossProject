//---------------------------------------------------------------------------
//Project
#include "RObject.h"
#include "RObject_Static_Memory.h"
#include "../World/Scene.h"
#include "Shader.h"
#include "../ResourceManager/Manager_Interface.h"
#include "../ResourceManager/Asset_Types.h"
#include "../Rendering/Renderer_Interface.h"
//Tool
#include <CodeTools.h>
#include <new>
//Plateform
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
	void R_OBJECT::Build(AT::I32F* DataVertices, AT::I32 VerticesCount, GLuint* DataElements, AT::I32 ElementsCount, AT::I32 DrawMode, const AT::I8* TextureFilename/*=NULL*/){
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
		glBufferData(GL_ARRAY_BUFFER, VerticesCount*7*sizeof(AT::I32F), DataVertices, DrawMode);
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
	//---------------------------------------------------------------------------
	void R_OBJECT::Draw(RENDERER_ABC& R){
		if(!m_VerticesCount)
			return;
		m_pShader->Use();
		m_pShader->BindDynamicVertexAttrib(R, *this);
		m_pShader->BindDynamicFragmentAttrib(R, this);
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
	R_OBJECT* R_OBJECT::CreateRObject(RENDERER_ABC& Renderer, RESOURCE_MANAGER_ABC& ResourceManager, const char* sResourceName, AT::VEC3Df& BBoxMin, AT::VEC3Df& BBoxMax){
		void* pBuffer = ResourceManager.LoadResource(sResourceName);
		if(!pBuffer)
			return NULL;
		//--
		char* ptr = (char*)pBuffer;
		//Asset type
		ASSET_TYPE AssetType = *(ASSET_TYPE*)ptr;
		ptr+= sizeof(ASSET_TYPE);
		assert(AssetType!=ASSET_UNKNOWN_TYPE);
		//Load bounding box & compute grid-occupation bounding box
		BBoxMin.Set(((AT::I32F*)ptr)[0], ((AT::I32F*)ptr)[1], ((AT::I32F*)ptr)[2]);
		ptr += 3*sizeof(AT::I32F);
		BBoxMax.Set(((AT::I32F*)ptr)[0], ((AT::I32F*)ptr)[1], ((AT::I32F*)ptr)[2]);
		ptr += 3*sizeof(AT::I32F);
		//Mesh number
		AT::I32 MeshsCount = (AT::I32)*(AT::U32*)ptr;
		ptr += sizeof(AT::U32);
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
			return NULL;
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
		ptr += IndicesCount*sizeof(GLuint);
		//!!!!!!TMP
		//One mesh read for the moment
		for(AT::I32 iMeshToSkip=0 ; iMeshToSkip < MeshsCount-1 ; ++iMeshToSkip){
			//skip UV channels count
			AT::I32 nUV = *(AT::I32*)ptr;
			ptr += sizeof(AT::I32);
			AT::I32 pixelInformationLength;
			if(nUV==1){
				pixelInformationLength = 5;	//vertex 3d position + uv 
			}else if(nUV!=0){
				assert(false);				//multiple uv channels, not handled
				return NULL;
			}else{
				pixelInformationLength = 7; //vertex 3d position + 4d color vector
			}
			//Load Vertices
			AT::I32 VerticeCount = *(AT::I32*)ptr;
			ptr += sizeof(VerticeCount);
			ptr += VerticeCount*pixelInformationLength*sizeof(AT::I32F);
			//Load Indices
			AT::I32 IndicesCount = *(AT::I32*)ptr;
			ptr += sizeof(IndicesCount);
			ptr += IndicesCount*sizeof(GLuint);
		}
		//!!!!!!TMP
		//Load Texture
		AT::I8*		TextureFilename;
		AT::VEC2Df	UVOffset;
		size_t len = *(size_t*)ptr;
		ptr += sizeof(size_t);
		AT::I8 DefaultNoTexture[22];
		sprintf_s(DefaultNoTexture, "DefaultNoTexture.png\0");
		if(len > 0){
			TextureFilename = (AT::I8*)ptr;
			ptr += len*sizeof(AT::I8);
			UVOffset.Set(((AT::I32F*)ptr)[0], ((AT::I32F*)ptr)[1]);
			ptr += 2*sizeof(AT::I32F);
		}else{
			TextureFilename = DefaultNoTexture;
		}
		//---
		R_OBJECT* pRObject = (R_OBJECT*)Renderer.m_DynamicAllocator.alloc(sizeof(R_OBJECT));
		pRObject = new(pRObject) R_OBJECT();
		pRObject->m_UVOffset = UVOffset;
		pRObject->Build(pVerticesBuffer, VerticeCount, pIndicesBuffer, IndicesCount, GL_STATIC_DRAW, TextureFilename);
		//--
		pRObject->m_GLDisplayMode = GL_TRIANGLES;
		Renderer.InitRObject(*pRObject, Shader);
		return pRObject;
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
