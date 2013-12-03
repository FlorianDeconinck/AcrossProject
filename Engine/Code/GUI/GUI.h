#ifndef __GUI_H__
#define __GUI_H__
//-----------------------------------------------------------------------------
//Tool
#include <AcrossTypes.h>
//STD
#include <vector>
//-----------------------------------------------------------------------------
namespace AE{
	class CONTROLLER;
	class OPENGL_RENDERER;
	class WORLD_ABC;
	class GUI{
	public:
		 		GUI();
				~GUI();
		void	Init();
		void	Update(CONTROLLER& C, OPENGL_RENDERER& R, WORLD_ABC& W);
		//--
		AT::I8 m_bDraw;
		//--
		struct GUI_PERF_LOG{
			enum GUI_PERF_LOG_INDEX{
				GUI_PERF_LOG_INDEX_PATHFIND_MEAN=0,
				GUI_PERF_LOG_INDEX_PATHFIND_MAX,
				GUI_COMMON_DEBUG_1,
				GUI_COMMON_DEBUG_2,
				GUI_COMMON_DEBUG_3,
				//--
				GUI_PERF_LOG_INDEX_COUNT,
			};
			AT::I8				m_sText[128];
			AT::U32				m_ImGuiColor;
			//--
								GUI_PERF_LOG(){ SetColor(255, 255, 255, 128); }
			void				SetColor(AT::U8 R, AT::U8 G, AT::U8 B, AT::U8 A){ m_ImGuiColor = R | (G << 8) | (B << 16) | (A << 24); }
		}; 
		//--
		static GUI_PERF_LOG		m_PerfLogContent[GUI_PERF_LOG::GUI_PERF_LOG_INDEX_COUNT];
	private:
		AT::I32					m_PerfLogScrollArea;
	};
}//namespace AE
//-----------------------------------------------------------------------------
#endif //__GUI_H__