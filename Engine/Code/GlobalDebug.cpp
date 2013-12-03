#ifdef _DEBUG
	#include "GlobalDebug.h"
	namespace AE{
		//--
		RENDERER_ABC*							g_pRenderer;
		WORLD_ABC*								g_pWorld;
		CONTROLLER*								g_pController;
		//--
		AT::I64F								g_PathfinderDebug_MeanProcessTime = -1.0;
		AT::I64F								g_PathfinderDebug_MaxProcessTime  = 0;
		AT::I8									g_bUpdateWorld = true;
		//--
		AT::DIRECT_TO_FILE_PERFORMANCE_LOGGER	g_PerfFileLogger("Global_Perf.log");
		//--
		AT::PROFILER							g_Profiler;
		//--
	}
#endif
