#ifdef _DEBUG
	#include "GlobalDebug.h"
	namespace AE{
		//--
		RENDERER*	g_pRenderer;
		WORLD*		g_pWorld;
		CONTROLLER*	g_pController;
		//--
		AT::I64F	g_PathfinderDebug_MeanProcessTime = -1.0;
		AT::I64F	g_PathfinderDebug_MaxProcessTime  = 0;
		//--
	}
#endif
