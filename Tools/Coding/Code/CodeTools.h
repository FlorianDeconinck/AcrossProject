#pragma once
//---------------------------------------------------------------------------
#include <assert.h>
//---------------------------------------------------------------------------
#define TODO(__MSG__) __pragma(message("Warning : "__MSG__))
//---------------------------------------------------------------------------
#if _DEBUG
	#define Break() __asm {int 3}
#else
	#define Break()
#endif
//---------------------------------------------------------------------------
#ifndef __GLEW_H__
	#include <GL/glew.h>
	#include <GL/gl.h>
#endif
namespace GL_TOOL{
	static void CheckGLError(){
#if _DEBUG
		GLenum err = glGetError();
		if(err != GL_NO_ERROR)
			Break();
#endif
	}
}
//---------------------------------------------------------------------------
