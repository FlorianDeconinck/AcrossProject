#pragma once
//---------------------------------------------------------------------------
//Tool
#include <AcrossTypes.h>
//---------------------------------------------------------------------------
typedef unsigned int GLuint;
typedef int GLint;
namespace AE{
	//---------------------------------------------------------------------------
	class RENDERER_ABC;
	class R_OBJECT;
	class SCENE;
	class SHADER_ABC{
	protected :
		AT::I8 m_FragmentFilename[256];
		AT::I8 m_VertexFilename[256];
		AT::I8 m_GeometryFilename[256];
		GLuint m_Vertex;
		GLuint m_Fragment;
		GLuint m_Geometry;
		GLuint m_Program;
		//----------------------------
	public : 
		enum SHADERS_ID{
			NO_SHADERS=0,
			COLOR_3D_SHADER,
			TEXTURE_3D_SHADER,
			THICK_LINES_3D_SHADER,
			FXAA_SCREEN_SHADER,
			BLUR_SCREEN_SHADER,
			//--
			SHADERS_ID_COUNT
		};
		SHADERS_ID m_ID;
		//----------------------------
		SHADER_ABC();
		~SHADER_ABC();
		AT::I8 /*bSuccess*/ Load(RENDERER_ABC& Renderer, AT::I8* _VertexFilename, AT::I8* _FragmentFilename, AT::I8* _GeometryFilename=NULL);
		void Use();
		virtual void Init(RENDERER_ABC& Renderer)=0;
		virtual void InitObject(const SCENE& Scene, R_OBJECT& Object)=0;
		virtual void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject)=0;
		virtual void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject=NULL)=0;
	protected :
		AT::I8 /*bSuccess*/ LoadAndCompileShaderFromFile(const AT::I8* Name, GLuint& Shader);
	};
	//---------------------------------------------------------------------------
	class POST_PROCESSING_SCREEN_SHADER:public SHADER_ABC{
	protected :
		GLint texFrameBufferUniform;
		float* verticesQuad;
		GLuint vaoQuad;
		GLuint vboQuad;
	public :
		POST_PROCESSING_SCREEN_SHADER();
		~POST_PROCESSING_SCREEN_SHADER(){
			delete verticesQuad; 
		}
		virtual void Init(RENDERER_ABC& Renderer);
		virtual void DeferedInit(RENDERER_ABC& Renderer)=0;
		void ApplyPostProcess(RENDERER_ABC& Renderer);
	};
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
