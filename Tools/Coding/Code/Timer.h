#pragma once
//---------------------------------------------------------------------------
// ---------
//  <profileapi.h> include <winnt.h> that need some defines without including the proper file (Microsoft logic...)
//  instead of including <windows.h> that includes the entire world, two times, I found a workaround on the internet
//  that specifies the correct includes needed, based on what have already been included
#ifndef __wtypes_h__
#include <wtypes.h>
#endif

#ifndef __WINDEF_
#include <windef.h>
#endif
#include <profileapi.h>
// ---------
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