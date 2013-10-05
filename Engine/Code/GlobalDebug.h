#ifndef __GLOBAL_DEBUG_H__
#define __GLOBAL_DEBUG_H__
#ifdef _DEBUG
#include "Renderer.h"
#include "World.h"
namespace AE{
	extern RENDERER*	g_pRenderer;
	extern WORLD*		g_pWorld;
	extern CONTROLLER*	g_pController;
	extern AT::I8		g_BlockPaintAsked;
}
#endif
#endif//__GLOBAL_DEBUG_H__