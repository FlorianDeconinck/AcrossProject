#pragma once
//---------------------------------------------------------------------------
//Project
#include "Shader.h"
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//-------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	/*USAGE
	VERTEX SHADER : (8 float)
			position	: 3 first float of VBO
			color		: 3 next float of VBO
	FRAGMENT SHADER (no attrib)
	*/
	class COLOR_SHADER:public SHADER_ABC{
	protected :
		GLint m_posAttrib;
		GLint m_colAttrib;
	public : 
		GLint m_viewUniform;
		GLint m_projUniform;
		/*virtual*/ void Init(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject=NULL){}
	};
	//---------------------------------------------------------------------------
	/*USAGE
	VERTEX SHADER : (8 float)
			position	: 3 first float of VBO
			texels		: 2 next float of VBO
	FRAGMENT SHADER (no attrib)
			
	*/
	class TEXTURE_SHADER:public SHADER_ABC{
	protected :
		GLint m_posAttrib;
		GLint m_texAttrib;
	public : 
		GLint m_viewUniform;
		GLint m_projUniform;
		/*virtual*/ void Init(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject=NULL);
	};
	//---------------------------------------------------------------------------
	class THICK_LINES_COLOR:public COLOR_SHADER{
	protected :
		GLint m_viewportSizeUniform;
		GLint m_lineWidthUniform;
		GLfloat m_ViewportSize[2];
		AT::I32F m_LineWidth;
	public :
		/*virtual*/ void Init(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
	};
	//---------------------------------------------------------------------------
	class BLUR_SHADER:public POST_PROCESSING_SCREEN_SHADER{
	protected :
		GLint m_posAttrib;
		GLint m_texAttrib;
		GLuint m_blurWdwUniform;
		GLuint m_blurRadiusUniform;
		AT::I32 m_Wdw;
		AT::I32 m_Radius;
	public:
		BLUR_SHADER();
		~BLUR_SHADER(){}
		/*virtual*/ void DeferedInit(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject=NULL);
	};
	//-------------------------------------------------------------------------
	class FXAA_SHADER:public POST_PROCESSING_SCREEN_SHADER{
	protected :
		GLint m_posAttrib;
		GLint m_texAttrib;
		GLint m_texCoordOffsetUniform;
		GLint m_FXAA_SPAN_MAX_Uniform;
		GLint m_FXAA_REDUCE_MUL_Uniform;
		GLint m_FXAA_REDUCE_MIN_Uniform;
		AT::I32F m_FXAA_SPAN_MAX;
		AT::I32F m_FXAA_REDUCE_MUL;
		AT::I32F m_FXAA_REDUCE_MIN;
		GLfloat textCoordOffsetArray[2];
	public : 
		/*virtual*/ void DeferedInit(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject=NULL);
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-------------------------------------------------------------------------