//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Engine
#include <AcrossTypes.h>
//OpenGL
#include <GL/glew.h>
#include <GL/gl.h>
//-----------------------------------------------------------------------------
namespace AE{
	struct GBUFFER{
		enum GBUFFER_TEXTURES{
			GBUFFER_TEXTURES_POSITION,
			GBUFFER_TEXTURES_DIFFUSE,
			GBUFFER_TEXTURES_NORMAL,
			GBUFFER_TEXTURES_TEXCOORD,
			//--
			GBUFFER_TEXTURES_COUNT
		};
		//-------------------------------------------------------------------------
		GBUFFER();
		//-------------------------------------------------------------------------
		AT::I8 Init(AT::U32 WinWidth, AT::U32 WinHeight);
		void BindToWrite();
		void BindToRead();
		void Read(GBUFFER_TEXTURES TexID);
		//-------------------------------------------------------------------------
		GLuint m_fbo;
		GLuint m_textures[GBUFFER_TEXTURES_COUNT];
		GLuint m_depthTexture;
	};
}//namespace AE
//-----------------------------------------------------------------------------
