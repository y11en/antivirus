#include <winbase.h>

class CTimer
{
	LARGE_INTEGER m_freq;
	LARGE_INTEGER m_start;
	__int64 m_acc;
public:
	CTimer()
		: m_acc(0)
	{
		QueryPerformanceFrequency(&m_freq);
		m_freq.QuadPart /= 1000;
	}
	void Start()
	{
		QueryPerformanceCounter(&m_start);
	}
	void Stop()
	{
		LARGE_INTEGER stop;
		QueryPerformanceCounter(&stop);
		m_acc += stop.QuadPart - m_start.QuadPart;
	}
	void Reset()
	{
		m_acc = 0;
	}
	int GetCurrentTimeMs() const
	{
		return m_acc / m_freq.QuadPart;
	}
};


class CTimerLock
{
	CTimer& m_timer;
public:
	CTimerLock(CTimer& timer): m_timer(timer) { timer.Start(); }
	~CTimerLock() { m_timer.Stop(); }
};