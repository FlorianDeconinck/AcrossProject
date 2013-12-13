//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include "GBuffer.h"
#include "../Shader.h"
//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
//Tool
#include <Vec3D.h>
#include <MathMacros.h>
//STD
#include <vector>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	/*USAGE
	VERTEX SHADER : (8 float)
	position	: 3 first float of VBO
	texels		: 2 next float of VBO
	normal      : 3 next float of VBO
	FRAGMENT SHADER ( )
	uvOffset    : 2 float added to texel coordinates
	*/
	class DEFERRED_TEXTURE_SHADER :public SHADER_ABC{
	protected:
		GLint m_posAttrib;
		GLint m_texAttrib;
		GLint m_norAttrib;
	public:
		GLint m_viewUniform;
		GLint m_projUniform;
		GLint m_uvOffsetUniform;
		/*virtual*/ void Init(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject = NULL);
	};
	//-----------------------------------------------------------------------------
	class DEFERRED_LIGHT_SPOT_SHADER :public SHADER_ABC{
	protected:
		GLint m_posAttrib;
	public:
		GLint m_viewUniform;
		GLint m_projUniform;
		//--
		GLint m_ScreenSizeUniform;
		//--
		GLint m_positionMapUniform;
		GLint m_colorMapUniform;
		GLint m_NormalMapUniform;
		//--
		GLint m_LightSpecular;
		GLint m_LightSpecularIntensity;
		GLint m_LightDiffuse;
		GLint m_LightDiffuseIntensity;
		GLint m_LightPosition;
		//--
		GLint m_EyeWorldPosition;
		//--
		/*virtual*/ void Init(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject = NULL);
	};
	//-----------------------------------------------------------------------------
	class DEFERRED_LIGHT_DIRECTIONAL_SHADER :public SHADER_ABC{
	protected:
		GLint m_posAttrib;
	public:
		GLint m_ScreenSizeUniform;
		GLint m_LightSpecular;
		GLint m_LightSpecularIntensity;
		GLint m_LightDiffuse;
		GLint m_LightDiffuseIntensity;
		GLint m_LightDirection;
		GLint m_EyeWorldPositionUniform;
		//--
		GLint m_positionMapUniform;
		GLint m_colorMapUniform;
		GLint m_NormalMapUniform;
		//--
		/*virtual*/ void Init(RENDERER_ABC& Renderer);
		/*virtual*/ void InitObject(const STATIC_RENDER_OBJECT& Scene, R_OBJECT& Object);
		/*virtual*/ void BindDynamicVertexAttrib(RENDERER_ABC& Renderer, R_OBJECT& RObject);
		/*virtual*/ void BindDynamicFragmentAttrib(const RENDERER_ABC& Renderer, const R_OBJECT* RObject = NULL);
	};
	//-----------------------------------------------------------------------------
	class R_OBJECT;
	class LIGHT;
	class RENDERER_ABC;
	class DEFERRED_RENDERER{
	public :
		enum DEFERRED_RENDERER_LIGHT_TYPE{
			DEFERRED_RENDERER_LIGHT_SPOT = 0,
			DEFFERED_RENDERER_LIGHT_DIRECTIONAL=1,
			//--
			DEFERRED_RENDERER_LIGHT_TYPE_COUNT,
		};
		//-------------------------------------------------------------------------
		DEFERRED_RENDERER() :m_Angle(AT_PI){}
		//-------------------------------------------------------------------------
		void Init();
		void Render(AT::I64F elapsedTime_ms, RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects);
		void AddLight(RENDERER_ABC& Renderer, DEFERRED_RENDERER_LIGHT_TYPE Type, GLfloat Diffuse[4], GLfloat Specular[4], AT::VEC3Df Position, AT::I32F Radius);
		//-------------------------------------------------------------------------
		DEFERRED_TEXTURE_SHADER			m_TexShader;
		DEFERRED_LIGHT_SPOT_SHADER      m_LightShaderSpot;
		DEFERRED_LIGHT_DIRECTIONAL_SHADER m_LightShaderDirectional;
	private:
		GBUFFER				m_GBuffer;
		std::vector<LIGHT*>	m_Lights;
		AT::I32F			m_Angle;
		//-------------------------------------------------------------------------
		void GeometryPass(RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects);
		void LightingPass(RENDERER_ABC& Renderer);
		void UpdateLight(AT::I64F elapsedTime_ms);
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
