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
			texcoord	: 2 next
	FRAGMENT SHADER (no attrib)
	*/
	class COLOR_SHADER:public SHADER_ABC{
	protected :
		GLint m_posAttrib;
		GLint m_colAttrib;
	public : 
		GLint m_viewUniform;
		GLint m_projUniform;
		/*virtual*/ void Init(RENDERER& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib();
	};
	//---------------------------------------------------------------------------
	class THICK_LINES_COLOR:public COLOR_SHADER{
	protected :
		GLint m_viewportSizeUniform;
		GLint m_lineWidthUniform;
		GLfloat m_ViewportSize[2];
		AT::I32F m_LineWidth;
	public :
		/*virtual*/ void Init(RENDERER& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER& Renderer, R_OBJECT& RObject);
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
		/*virtual*/ void DeferedInit(RENDERER& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib();
		void IncreaseBlurWdw(){++m_Wdw;}
		void IncreaseBlurRatio(){++m_Radius;}
		void DecreaseBlurWdw(){m_Wdw = m_Wdw<=0 ? 0 : --m_Wdw;}
		void DecreaseBlurRatio(){m_Radius = m_Radius<=0 ? 0 : --m_Radius;}
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
		/*virtual*/ void DeferedInit(RENDERER& Renderer);
		/*virtual*/ void InitObject(const SCENE& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib();
	};
	//-------------------------------------------------------------------------
}//namespace AE
//-------------------------------------------------------------------------