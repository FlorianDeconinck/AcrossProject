#ifndef __GLOBAL_DEBUG_H__
#define __GLOBAL_DEBUG_H__
//---------------------------------------------------------------------------
#ifdef _DEBUG
	//---------------------------------------------------------------------------
	//Tool
	#include <AcrossTypes.h>
	#include <PerfomanceLogger.h>
	#include <Profiler.h>
	//---------------------------------------------------------------------------
	namespace AE{
		class RENDERER_ABC;
		class WORLD_2DGRID;
		class WORLD_ABC;
		class CONTROLLER;
		//--
		extern RENDERER_ABC*							g_pRenderer;
		extern WORLD_ABC*								g_pWorld;
		extern CONTROLLER*								g_pController;
		//--
		extern AT::I64F									g_PathfinderDebug_MeanProcessTime;
		extern AT::I64F									g_PathfinderDebug_MaxProcessTime;
		//--
		extern AT::I8									g_bUpdateWorld;
		//--
		extern AT::DIRECT_TO_FILE_PERFORMANCE_LOGGER	g_PerfFileLogger;
		//--
		extern AT::PROFILER								g_Profiler;
		//--
	}
	//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
#endif//__GLOBAL_DEBUG_H__
