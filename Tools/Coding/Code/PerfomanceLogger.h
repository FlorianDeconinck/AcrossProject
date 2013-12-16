#pragma once
//---------------------------------------------------------------------------
//STD
#include <fstream>
#include <stdarg.h>
#include <string.h>
//Tools
#include "Timer.h"
#include "AcrossTypes.h"
namespace AT{
	//---------------------------------------------------------------------------
	// DIRECT TO FILE PERF LOGGER
	//---------------------------------------------------------------------------
	// DOC
	// Perf logger as expected.
	//   Start(), save time;
	//	 Pause(), pause the timer;
	//   EndAndRecord(...), will end the time recording. The args could be anything that can 
	//						be aggregated to a string to printf on file. Use the string _TIME_
	//						to print the time recorded by the perf logger
	//---------------------------------------------------------------------------
	struct DIRECT_TO_FILE_PERFORMANCE_LOGGER{
		DIRECT_TO_FILE_PERFORMANCE_LOGGER():m_Time(0),m_TimeSum(0)							{ m_logFileStream = std::ofstream("default_perf.log"); }
		DIRECT_TO_FILE_PERFORMANCE_LOGGER(const char* logFilename):m_Time(0),m_TimeSum(0)	{ m_logFileStream = std::ofstream(logFilename); }
		~DIRECT_TO_FILE_PERFORMANCE_LOGGER()													{ m_logFileStream.close(); }
		//--
		AT::I64F Start()	{ m_Time=m_Timer.GetTime(); return m_Time; }
		//--
		AT::I64F Pause()	{ m_TimeSum+=m_Timer.GetTime()-m_Time; return m_TimeSum; }
		//--
		AT::I64F EndAndRecord(std::string var){
			Pause();
			AT::I32 TimePos = var.find("_TIME_");
			std::string str1 = var.substr(0, TimePos);
			std::string str2 = var.substr(TimePos+6);
			m_logFileStream << str1.c_str() << m_TimeSum << str2.c_str() << std::endl;
			AT::I64F Time = m_TimeSum;
			m_TimeSum=0;
			return Time;
		}
		//--
		std::ofstream	m_logFileStream;
		TIMER			m_Timer;
		AT::I64F		m_Time;
		AT::I64F		m_TimeSum;
	};
	//---------------------------------------------------------------------------
}//namespace AT
//---------------------------------------------------------------------------