#pragma once
//---------------------------------------------------------------------------
#include <profileapi.h>
//---------------------------------------------------------------------------
class TIMER{
protected:
	double Freq;
	__int64 CounterStart;
public :
	TIMER(){
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		Freq = li.QuadPart/1000.0;
		QueryPerformanceCounter(&li);
		CounterStart = li.QuadPart;
	}
	~TIMER(){}
	double GetTime(){
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return double(li.QuadPart-CounterStart)/Freq;
	}
};
//---------------------------------------------------------------------------