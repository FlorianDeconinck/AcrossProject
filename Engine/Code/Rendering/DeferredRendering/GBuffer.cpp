//-----------------------------------------------------------------------------
//Engine
#include "GBuffer.h"
//STD
#include <iostream>
//-----------------------------------------------------------------------------
namespace AE{
	//-------------------------------------------------------------------------
	GBUFFER::GBUFFER(){
	}
	//-------------------------------------------------------------------------
	AT::I8 GBUFFER::Init(AT::U32 WinWidth, AT::U32 WinHeight){
		//FBO
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
		//GBUFFER
		glGenTextures(GBUFFER_TEXTURES_COUNT, m_textures);
		for (AT::I32 iTex = 0; iTex < GBUFFER_TEXTURES_COUNT; iTex++) {
			glBindTexture(GL_TEXTURE_2D, m_textures[iTex]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WinWidth, WinHeight, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + iTex, GL_TEXTURE_2D, m_textures[iTex], 0);
		}
		//Depth
		glGenTextures(1, &m_depthTexture);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WinWidth, WinHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);
		//Drawbuffers
		GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
		glDrawBuffers(GBUFFER_TEXTURES_COUNT, DrawBuffers);
		//Check status
		GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(Status != GL_FRAMEBUFFER_COMPLETE){
			std::cout << "GBUFFER init : framebuffer error status" << Status << std::endl;
			return false;
		}
		//Bind default fb
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return true;
	}
	//-----------------------------------------------------------------------------
	void GBUFFER::BindToWrite(){
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
	}
	//-----------------------------------------------------------------------------
	void GBUFFER::BindToRead(){
#if CHECK_GEOMETRY_PASS()
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
#else
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		for (AT::U32 i = 0; i < GBUFFER_TEXTURES_COUNT-1; i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, m_textures[GBUFFER_TEXTURES_POSITION + i]);
		}
#endif
	}
	//-----------------------------------------------------------------------------
	void GBUFFER::Read(GBUFFER_TEXTURES TexID){
		glReadBuffer(GL_COLOR_ATTACHMENT0 + TexID);
	}
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------