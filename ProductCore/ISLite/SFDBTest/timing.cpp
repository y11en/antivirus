#include "windows.h"
#include "cio.h"

LARGE_INTEGER liStartTime;
LARGE_INTEGER liStopTime;
LARGE_INTEGER liElapsedTime;
LARGE_INTEGER liPerformanceFrequency;

void StartTiming()
{
	QueryPerformanceCounter(&liStartTime);
	QueryPerformanceFrequency(&liPerformanceFrequency);
	liElapsedTime.HighPart = -1;
	liElapsedTime.LowPart = -1;
}

DWORD StopTiming()
{
	QueryPerformanceCounter(&liStopTime);
	liElapsedTime.HighPart = liStopTime.HighPart - liStartTime.HighPart;
	if (liStopTime.LowPart < liStartTime.LowPart)
		liElapsedTime.HighPart--; 
	liElapsedTime.LowPart = liStopTime.LowPart - liStartTime.LowPart;
	return liElapsedTime.LowPart;
}

DWORD GetElapsedTime()
{
	return liElapsedTime.LowPart;
}

void DisplayTiming()
{
	float sec = (float)liElapsedTime.LowPart;
	sec /= liPerformanceFrequency.LowPart;
	OUTPUT(("Time elapsed %f sec", sec));
}

