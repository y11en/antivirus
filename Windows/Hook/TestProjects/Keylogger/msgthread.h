#ifndef _MSGTHREAD_H
#define _MSGTHREAD_H
//-----------------------------------------------------------------------------------------

class CMsgThread
{
private:
	HANDLE hThread;
// признак завершения messagethread
	volatile BOOL TerminateMsg;

	static DWORD WINAPI MsgThread(PVOID Param);
public:
	CMsgThread()
	{
		hThread = NULL;
	};
	
	~CMsgThread() {};

	void Start()
	{
		TerminateMsg = FALSE;
		hThread = CreateThread(NULL, 0, MsgThread, this, 0, NULL);
	}

	void Stop()
	{
		if (hThread)
		{
			TerminateMsg = TRUE;
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);

			hThread = NULL;
		}
	}
};

//-----------------------------------------------------------------------------------------
#endif