//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include "GBuffer.h"
#include "../Shader.h"
//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
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
	class R_OBJECT;
	class DEFERRED_RENDERER{
	public :
		//-------------------------------------------------------------------------
		DEFERRED_RENDERER(){}
		//-------------------------------------------------------------------------
		void Init();
		void Render(RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects);
		//-------------------------------------------------------------------------
		DEFERRED_TEXTURE_SHADER		m_TexShader;
		//DEFERRED_LIGHT_SHADER       m_LightShader;
	private:
		GBUFFER m_GBuffer;
		//-------------------------------------------------------------------------
		void GeometryPass(RENDERER_ABC& Renderer, const std::vector<R_OBJECT*>& Objects);
		void LightingPass();
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------
