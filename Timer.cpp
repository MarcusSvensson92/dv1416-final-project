#include "StdAfx.h"
#include "Timer.h"

Timer::Timer(void)
{
	m_deltaTime = 0;
	m_stopTime  = 0;
	m_stopped   = false;

	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	m_secondsPerCount = 1.0 / (double)countsPerSecond;

	reset();
}

void Timer::reset(void)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevTime);
}

void Timer::start(void)
{
	if (m_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_prevTime = currTime - (m_stopTime - m_prevTime);
		m_stopTime = 0;
		m_stopped = false;
	}
}

void Timer::stop(void)
{
	if (!m_stopped)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_stopTime);
		m_stopped = true;
	}
}

void Timer::tick(void)
{
	if (!m_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_deltaTime = (currTime - m_prevTime) * m_secondsPerCount;
		m_prevTime = currTime;
	}
}