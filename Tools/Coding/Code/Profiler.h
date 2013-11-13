#pragma once
//---------------------------------------------------------------------------
//Tool
#include "Timer.h"
#include "AcrossTypes.h"
//STD
#include <vector>
//---------------------------------------------------------------------------
namespace AT{
	//-----------------------------------------------------------------------
	class PROFILER{
		public:
			PROFILER();
			~PROFILER();
			//--
			inline void StartFrameTimer()					{ m_FrameTime = m_Timer.GetTime(); }
			inline void StopFrameTimer()					{ m_LastFrameTime = m_Timer.GetTime() - m_FrameTime; }
			//--
			void		StartSubTimer(const char* TimerName);
			void		StopSubTimer(const char* TimerName);
			//--
			void		AddSubTimer(const char* Name);
			void		AddSubTimer(const char** NameHierarchy, AT::I32 HierarchyDeepness); //Add a subtimer within a hierarchy of subtimers. Usage : {"DeeperLevel", "Parent", ... , "RootLevel"}
			void		RenderProfiler(AT::U32 x, AT::U32 y, AT::I32 MouseX, AT::I32 MouseY, AT::I32 MouseScroll, AT::I32 WindowWidth, AT::I32 WindowHeight);
			//--
		private :
			TIMER					m_Timer;
			AT::I64F				m_FrameTime;
			AT::I64F				m_LastFrameTime;
			struct SUB_TIMER{
				SUB_TIMER():m_Time(0.0), m_LastTime(0.0){}
				~SUB_TIMER(){ RecursiveDestructor(m_SubTimers); }
				//--
				void RecursiveDestructor(std::vector<SUB_TIMER*>& SubTimers){ 
					for(AT::U32 iSubTimers = 0 ; iSubTimers < SubTimers.size() ; ++iSubTimers){
						RecursiveDestructor(SubTimers[iSubTimers]->m_SubTimers);
						delete SubTimers[iSubTimers];
					}
				}
				//--
				AT::I64F				m_Time;
				AT::I64F				m_LastTime;
				char					m_Name[128];
				std::vector<SUB_TIMER*>	m_SubTimers;
			};
			std::vector<SUB_TIMER*>	m_SubTimers;
			//--
			static const AT::I32 LineHeight = 15;
			static const AT::U32 GrayishColor = 2164260863;//Color code for 255,255,255,128 because I'm lazy
			AT::I8		AddSubTimer_internal(std::vector<PROFILER::SUB_TIMER*>& SubTimers, const char** NameHierarchy, AT::I32 HierarchyDeepness);
			AT::I8		StartSubTimer_internal(const char* Name, std::vector<PROFILER::SUB_TIMER*>& SubTimers);
			AT::I8		StopSubTimer_internal(const char* Name, std::vector<PROFILER::SUB_TIMER*>& SubTimers);
			void		RenderSubTimers_internal(const char* sTextOffset, std::vector<PROFILER::SUB_TIMER*>& SubTimers, AT::I32 x, AT::I32& LineOffsetH, AT::I64F OverallTime);
	};
	//-----------------------------------------------------------------------
}//namespace AT
//---------------------------------------------------------------------------
