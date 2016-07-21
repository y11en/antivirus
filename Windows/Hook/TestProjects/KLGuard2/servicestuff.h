#ifndef _SERVICE_STUFF
#define _SERVICE_STUFF

class CAutoLock
{
public:
	CAutoLock(LPCRITICAL_SECTION lpLock)
	{
		m_lpLock = lpLock;
		EnterCriticalSection(lpLock);
	}
	virtual ~CAutoLock()
	{
		LeaveCriticalSection(m_lpLock);
	}
private:

	LPCRITICAL_SECTION m_lpLock;
};

class CAutoFileHandle
{
public:
	CAutoFileHandle(HANDLE hHandle)
	{
		m_hHandle = hHandle;
	}
	virtual ~CAutoFileHandle()
	{
		if (m_hHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_hHandle);
		}
	}
private:

	HANDLE m_hHandle;
};

#endif // _SERVICE_STUFF