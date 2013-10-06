#ifndef __GLOBAL_DEBUG_H__
#define __GLOBAL_DEBUG_H__
//---------------------------------------------------------------------------
#ifdef _DEBUG
	//---------------------------------------------------------------------------
	//Project	
	#include "./Rendering/Renderer.h"
	#include "World.h"
	//Tool
	#include <CommonTypes.h>
	//---------------------------------------------------------------------------
	namespace AE{
		//--
		extern RENDERER*		g_pRenderer;
		extern WORLD*			g_pWorld;
		extern CONTROLLER*		g_pController;
		//--
		extern AT::I64F			g_PathfinderDebug_MeanProcessTime;
		extern AT::I64F			g_PathfinderDebug_MaxProcessTime;
		//--
	}
	//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#endif//__GLOBAL_DEBUG_H__
