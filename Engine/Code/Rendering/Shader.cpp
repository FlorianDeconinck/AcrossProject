//Tool
#include <CodeTools.h>
//Project 
#include "Shader.h"
#include "Renderer.h"
#include "RObject.h"
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//STD
#include <string.h>
#include <cstdio>
#include <fstream>
#include <iostream>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	SHADER_ABC::SHADER_ABC():m_Vertex(0),m_Fragment(0),m_Geometry(0),m_Program(0),m_ID(NO_SHADERS){
		m_VertexFilename[0]=0;
		m_FragmentFilename[0]=0;
		m_GeometryFilename[0]=0;
	}
	//---------------------------------------------------------------------------
	SHADER_ABC::~SHADER_ABC(){
		glDeleteShader(m_Fragment);
		glDeleteShader(m_Vertex);
		glDeleteProgram(m_Program);
	}
	//---------------------------------------------------------------------------
	void SHADER_ABC::Load(RENDERER& Renderer, char* _VertexFilename, char* _FragmentFilename, AT::I8* _GeometryFilename/*=NULL*/){
		//Vertex
		strcpy_s(m_VertexFilename, _VertexFilename);
		m_Vertex = glCreateShader(GL_VERTEX_SHADER);
		LoadAndCompileShaderFromFile(m_VertexFilename, m_Vertex);
		//Fragment
		strcpy_s(m_FragmentFilename, _FragmentFilename);
		m_Fragment	= glCreateShader(GL_FRAGMENT_SHADER);
		LoadAndCompileShaderFromFile(m_FragmentFilename	, m_Fragment);
		//Geometry
		if(_GeometryFilename){
			strcpy_s(m_GeometryFilename, _GeometryFilename);
			m_Geometry = glCreateShader(GL_GEOMETRY_SHADER);
			LoadAndCompileShaderFromFile(m_GeometryFilename, m_Geometry);
		}
		//Build program
		m_Program = glCreateProgram();
		glAttachShader(m_Program, m_Vertex);
		glAttachShader(m_Program, m_Fragment);
		if(_GeometryFilename)
			glAttachShader(m_Program, m_Geometry);
		glBindFragDataLocation(m_Program, 0, "outColor");
		glLinkProgram(m_Program);
		Init(Renderer);
		GL_TOOL::CheckGLError();
	}
	//---------------------------------------------------------------------------
	void SHADER_ABC::LoadAndCompileShaderFromFile(const char* Name, GLuint& Shader){
		char ShaderFilename[512];
		sprintf_s(ShaderFilename, "../../../Engine/Code/Rendering/Shaders/%s",Name);
		std::ifstream ShaderFile(ShaderFilename);
		if(!ShaderFile.is_open()){
			Break();
		}
		ShaderFile.seekg (0, ShaderFile.end);
		int length = (int)ShaderFile.tellg();
		ShaderFile.seekg (0, ShaderFile.beg);
		char * buffer = new char [length];
		memset(buffer, 0, length*sizeof(char));
		ShaderFile.read (buffer,length);
		ShaderFile.close();
		glShaderSource(Shader, 1, (const GLchar**)&buffer, NULL);
		glCompileShader(Shader);
		char LogBuffer[512];
		glGetShaderInfoLog(Shader, 512, NULL, LogBuffer); 
		printf(LogBuffer);
		delete buffer;
	}
	//---------------------------------------------------------------------------
	void SHADER_ABC::Use(){
		glUseProgram(m_Program);
	}
	//---------------------------------------------------------------------------
	POST_PROCESSING_SCREEN_SHADER::POST_PROCESSING_SCREEN_SHADER(){
		verticesQuad = new float[6*4];
		verticesQuad[0]=-1.0f; verticesQuad[1]=1.0f;  verticesQuad[2]=0.0f; verticesQuad[3]=1.0f;
		verticesQuad[4]=1.0f;	 verticesQuad[5]=1.0f;	verticesQuad[6]=1.0f;	verticesQuad[7]=1.0f,
		verticesQuad[8]=1.0f;	 verticesQuad[9]=-1.0f; verticesQuad[10]=1.0f;verticesQuad[11]=0.0f;
		verticesQuad[12]=-1.0f; verticesQuad[13]=-1.0f;verticesQuad[14]=0.0f;verticesQuad[15]=0.0f;
	}
	//---------------------------------------------------------------------------
	void POST_PROCESSING_SCREEN_SHADER::Init(RENDERER& Renderer){
		glGenVertexArrays(1, &vaoQuad);
		glBindVertexArray(vaoQuad);
		GL_TOOL::CheckGLError();
		glGenBuffers(1, &vboQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);
		glBufferData(GL_ARRAY_BUFFER, 16*sizeof(verticesQuad), verticesQuad, GL_STATIC_DRAW);
		GL_TOOL::CheckGLError();
		DeferedInit(Renderer);
	}
	//---------------------------------------------------------------------------
	void POST_PROCESSING_SCREEN_SHADER::ApplyPostProcess(RENDERER& Renderer){
		Use();
		glDisable( GL_DEPTH_TEST );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Renderer.m_texScreenShaderColorBuffer);
		glBindVertexArray(vaoQuad);
		BindDynamicFragmentAttrib();
		glDrawArrays(GL_QUADS, 0, 4);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------