#include <stdafx.h>
#include "msgthread.h"

// раздаем WM_NULL всем ниткам..(чтоб выгрузка не залипала)
DWORD WINAPI CMsgThread::MsgThread(PVOID Param)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return 0;

	THREADENTRY32 TE;
	TE.dwSize = sizeof(TE);
	while (!((CMsgThread *)Param)->TerminateMsg)
	{
		if (!Thread32First(hSnapshot, &TE))
			break;

		do
		{
			PostThreadMessage(TE.th32ThreadID, WM_NULL, 0, 0);
		} while(Thread32Next(hSnapshot, &TE));

		Sleep(300);
	}

	CloseHandle(hSnapshot);
	return 0;
}

