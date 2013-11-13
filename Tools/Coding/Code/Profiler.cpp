//---------------------------------------------------------------------------
//Project
#include "Profiler.h"
#include "../External/ImGUI/imgui.h"
#include "../External/ImGUI/imguiRenderGL3.h"
//GL
#include <GL/glew.h>
#include <GL/gl.h>
//---------------------------------------------------------------------------
namespace AT{
	//---------------------------------------------------------------------------
	PROFILER::PROFILER(){}
	//---------------------------------------------------------------------------
	PROFILER::~PROFILER(){
		AT::I32 SubTimerCount = m_SubTimers.size();
		for(AT::I32 iSubTimers = 0 ; iSubTimers < SubTimerCount ; ++iSubTimers)
			delete m_SubTimers[iSubTimers];
	}
	//---------------------------------------------------------------------------
	void PROFILER::AddSubTimer(const char* Name){
		SUB_TIMER* pSubTimer = new SUB_TIMER();
		strcpy_s(pSubTimer->m_Name, Name);
		m_SubTimers.push_back(pSubTimer);
	}
	//---------------------------------------------------------------------------
	AT::I8 PROFILER::AddSubTimer_internal(std::vector<PROFILER::SUB_TIMER*>& SubTimers,const char* NameHierarchy[], AT::I32 HierarchyDeepness){
		if(!HierarchyDeepness){
			SUB_TIMER* pSubTimer = new SUB_TIMER();
			strcpy_s(pSubTimer->m_Name, NameHierarchy[HierarchyDeepness]);
			SubTimers.push_back(pSubTimer);
			return true;
		}
		AT::I32 Count = SubTimers.size();
		for(AT::I32 iTimer=0 ; iTimer < Count ; ++iTimer){
			if(!strcmp(NameHierarchy[HierarchyDeepness], SubTimers[iTimer]->m_Name))
				return AddSubTimer_internal(SubTimers[iTimer]->m_SubTimers, NameHierarchy, HierarchyDeepness-1);
		}
		return false;
	}
	//---------------------------------------------------------------------------
	void PROFILER::AddSubTimer(const char** NameHierarchy, AT::I32 HierarchyDeepness){
		AddSubTimer_internal(m_SubTimers, NameHierarchy, HierarchyDeepness-1);
	}
	//---------------------------------------------------------------------------
	AT::I8 PROFILER::StartSubTimer_internal(const char* Name, std::vector<PROFILER::SUB_TIMER*>& SubTimers){
		AT::I32 Count = SubTimers.size();
		for(AT::I32 iStartSub=0 ; iStartSub < Count ; ++iStartSub){
			if(!strcmp(Name, SubTimers[iStartSub]->m_Name)){
				SubTimers[iStartSub]->m_Time = m_Timer.GetTime();
				return true;
			}
			StartSubTimer_internal(Name, SubTimers[iStartSub]->m_SubTimers);
		}
		return false;
	}
	//---------------------------------------------------------------------------
	void PROFILER::StartSubTimer(const char* TimerName){
		StartSubTimer_internal(TimerName, m_SubTimers);
	}
	//---------------------------------------------------------------------------
	AT::I8 PROFILER::StopSubTimer_internal(const char* Name, std::vector<PROFILER::SUB_TIMER*>& SubTimers){
		AT::I32 Count = SubTimers.size();
		for(AT::I32 iStartSub=0 ; iStartSub < Count ; ++iStartSub){
			if(!strcmp(Name, SubTimers[iStartSub]->m_Name)){
				SubTimers[iStartSub]->m_LastTime = m_Timer.GetTime() - SubTimers[iStartSub]->m_Time;
				return true;
			}
			StopSubTimer_internal(Name, SubTimers[iStartSub]->m_SubTimers);
		}		
		return false;
	}
	//---------------------------------------------------------------------------
	void PROFILER::StopSubTimer(const char* TimerName){
		StopSubTimer_internal(TimerName, m_SubTimers);
	}
	//---------------------------------------------------------------------------
	void PROFILER::RenderSubTimers_internal(const char* sTextOffset, std::vector<PROFILER::SUB_TIMER*>& SubTimers, AT::I32 x, AT::I32& LineOffsetH, AT::I64F OverallTime){
		AT::I8 sText[256];
		AT::I32 SubTimerCount = SubTimers.size();
		for(AT::I32 iSubTimers = 0 ; iSubTimers < SubTimerCount ; ++iSubTimers){
			SUB_TIMER* pTimer = SubTimers[iSubTimers];
			sprintf_s(sText, "%s %s : %3.2lf %% ( %.2lf ms)\n", sTextOffset, pTimer->m_Name, (pTimer->m_LastTime/OverallTime)*100.0, pTimer->m_LastTime);
			imguiDrawText(x+10, LineOffsetH, IMGUI_ALIGN_LEFT, sText, GrayishColor);
			LineOffsetH-=LineHeight;
			char sTextOffsetInc[128];
			sprintf_s(sTextOffsetInc, "%s-",sTextOffset);
			RenderSubTimers_internal(sTextOffsetInc, pTimer->m_SubTimers, x, LineOffsetH, pTimer->m_LastTime);
		}
	}
	//---------------------------------------------------------------------------
	void PROFILER::RenderProfiler(AT::U32 x, AT::U32 y, AT::I32 MouseX, AT::I32 MouseY, AT::I32 MouseScroll, AT::I32 WindowWidth, AT::I32 WindowHeight){
		//--
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
		//--
		imguiBeginFrame(MouseX, MouseY, 0, MouseScroll);
		//--
		AT::I32 ProfilerHeight = 250;
		AT::I32 LineOffsetH = ProfilerHeight - 30;
		AT::I8 sText[256];
		AT::I32 ScrollArea;
		imguiBeginScrollArea("--- Profiler ---", x, y, 250, ProfilerHeight, &ScrollArea);
		sprintf_s(sText, "Global frame time : %.2lf ms\n",  m_LastFrameTime);
		imguiDrawText(x+10, LineOffsetH, IMGUI_ALIGN_LEFT, sText, GrayishColor);
		LineOffsetH-=LineHeight;
		//--
		RenderSubTimers_internal("-", m_SubTimers, x, LineOffsetH, m_LastFrameTime);
		//--
		imguiRenderGLDraw(WindowWidth, WindowHeight);
	}
	//---------------------------------------------------------------------------
}//namespace AT
//---------------------------------------------------------------------------
