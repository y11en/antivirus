//////////////////////////////////////////////////////////////////////////

struct CThreadContext
{
	CThreadContext();
	~CThreadContext();

	void Delegate();

	static void Init();
	static void Done();

	static void SetThreadContext(bool fOn);
	static CThreadContext * GetThreadContext();

	static void OpenBaseDone();

	static void ExpandEnvironmentStringsEx(const char *p_string, char *p_buff, long p_size);

public:
	static CRITICAL_SECTION m_Lock;

	PVOID	pThreadArg;
	LPTHREAD_START_ROUTINE	pThreadProc;
	BOOL    m_bMapCheck;
	LPDWORD m_pMapAddr;

private:
	LONG	m_nRef;
	HDESK	m_hDesk;
	HANDLE	m_hToken;

	static DWORD m_tlsVal;
};

//////////////////////////////////////////////////////////////////////////
