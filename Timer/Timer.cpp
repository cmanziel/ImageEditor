#include "Timer.h"

Timer::Timer()
{
	m_DeltaMilliSeconds = 0;
	m_LastTime = GetTimeMilliseconds();

	QueryPerformanceFrequency(&m_PerformanceFreq);

	QueryPerformanceCounter(&m_LastCount);
}

Timer::~Timer()
{

}

void Timer::Update()
{
	LARGE_INTEGER ticks;

	QueryPerformanceCounter(&ticks);

	m_DeltaMilliSeconds = (ticks.QuadPart - m_LastCount.QuadPart) * 1000 / m_PerformanceFreq.QuadPart;

	m_LastCount = ticks;
}

long Timer::GetDeltaMilliSeconds()
{
	return m_DeltaMilliSeconds;
}

long Timer::GetTimeMilliseconds()
{
	LARGE_INTEGER ticks, frequency;

	QueryPerformanceCounter(&ticks);
	QueryPerformanceFrequency(&frequency);

	ticks.QuadPart *= 1000;

	return ticks.QuadPart / frequency.QuadPart;
}

long Timer::GetElapsedTicks()
{
	LARGE_INTEGER ticks;

	QueryPerformanceCounter(&ticks);

	return ticks.QuadPart;
}

long Timer::GetTicksFrequency()
{
	LARGE_INTEGER freq;

	QueryPerformanceFrequency(&freq);

	return freq.QuadPart;
}