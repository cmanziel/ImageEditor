#pragma once

#include <windows.h>
#include <profileapi.h>
#include <winnt.h>

class Timer
{
public:

	Timer();
	~Timer();

	void Update();

	long GetTimeMilliseconds();
	long GetDeltaMilliSeconds();
	long GetElapsedTicks();
	long GetTicksFrequency();

	long m_LastTime;

private:
	 long m_DeltaMilliSeconds;
	 LARGE_INTEGER m_PerformanceFreq;
	 LARGE_INTEGER m_LastCount; // last value of QueryPerformace counter
};

