#ifdef _DEBUG
	#include "GlobalDebug.h"
	namespace AE{
		//--
		RENDERER_ABC*	g_pRenderer;
		WORLD*			g_pWorld;
		CONTROLLER*		g_pController;
		//--
		AT::I64F		g_PathfinderDebug_MeanProcessTime = -1.0;
		AT::I64F		g_PathfinderDebug_MaxProcessTime  = 0;
		AT::I8			g_bUpdateWorld = true;
		//--
	}
#endif
