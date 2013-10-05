#ifndef __GUI_H__
#define __GUI_H__
//-----------------------------------------------------------------------------
//Tool
#include <CommonTypes.h>
//-----------------------------------------------------------------------------
namespace AE{
	class CONTROLLER;
	class RENDERER;
	class WORLD;
	class GUI{
	public:
		 		GUI();
				~GUI();
		void	Init();
		void	Update(CONTROLLER& C, RENDERER& R, WORLD& W);
		//--
		AT::I8 m_bDraw;
		//--
	private:
		AT::I32 m_DebugLogScrollArea;
	};
}//namespace AE
//-----------------------------------------------------------------------------
#endif //__GUI_H__