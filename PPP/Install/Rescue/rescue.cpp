//
#include "stdafx.h"
#include <shellapi.h>

static int parse_cmd(LPWSTR cmdParams, WCHAR** argv, int argv_size)
{

#define FINISH_ARG()				\
	{								\
	if (pBeginArg!=pCurr)		\
	{							\
	*pCurr = L'\0';			\
	argv[argc++]=pBeginArg;	\
	pBeginArg = pCurr + 1;	\
}							\
		else						\
	{							\
	++pBeginArg;			\
}							\
}

	int argc = 0;
	bool bSpaceString = false;
	LPWSTR pCurr = cmdParams, pBeginArg = cmdParams;
	for (;*pCurr && argc<argv_size; ++pCurr)
	{
		if(*pCurr == L'\"')
		{
			if (bSpaceString) FINISH_ARG()
			else pBeginArg = pCurr+1;

			bSpaceString = !bSpaceString;
		}
		else if (!bSpaceString && *pCurr == L' ') FINISH_ARG()
	}
	FINISH_ARG()

		return argc;
}
static HANDLE hex2handle(LPCWSTR szwHandle)
{
	int len = lstrlenW(szwHandle);
	if (!szwHandle && !len)
		return NULL;

	int start = 0;
	if(len>2 && (szwHandle[1]==L'x' || szwHandle[1]==L'X'))
		start = 2;

	DWORD_PTR ret = 0;
	for (int i = len-1, bit_offset = 0; i>=start && bit_offset<=sizeof(DWORD_PTR)*8-4; --i, bit_offset+=4)
	{
		if (szwHandle[i] >= L'0' && szwHandle[i] <= L'9')
			ret |= (((DWORD_PTR)(szwHandle[i]-L'0'))<<bit_offset);
		else if(szwHandle[i] >= L'a' && szwHandle[i] <= L'f')
			ret |= (((DWORD_PTR)(szwHandle[i]-L'a'+0xA))<<bit_offset);
		else if(szwHandle[i] >= L'A' && szwHandle[i] <= L'F')
			ret |= (((DWORD_PTR)(szwHandle[i]-L'A'+0xA))<<bit_offset);
		else
			return NULL;
	}

	return (HANDLE)ret;
}

static void reply(HANDLE hPipe, LPCWSTR strCommand)
{
	DWORD dwWrite = (lstrlenW(strCommand)+1)*sizeof(WCHAR), dwWritten = 0;
	WriteFile(hPipe, strCommand, dwWrite, &dwWritten, NULL);
}


static int fMain(void)
{
	WCHAR cmdParams[MAX_PATH];
	lstrcpyW(cmdParams, GetCommandLineW());
	WCHAR* argv[4];
	int argc = parse_cmd(cmdParams, argv, sizeof(argv)/sizeof(argv[0]));

	if (argc!=4)
		return -1;

	HANDLE hParentProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, (DWORD)(DWORD_PTR)hex2handle(argv[1]));
	if (!hParentProcess)
		return -1;

	HANDLE hProxyRd = NULL;
	if ( !DuplicateHandle(hParentProcess, hex2handle(argv[2]), GetCurrentProcess(), &hProxyRd, 0, FALSE, DUPLICATE_SAME_ACCESS) ||
		 !hProxyRd )
	{
		CloseHandle(hParentProcess);
		 return -1;
	}

	HANDLE hProxyWr = NULL;
	if ( !DuplicateHandle(hParentProcess, hex2handle(argv[3]), GetCurrentProcess(), &hProxyWr, 0, FALSE, DUPLICATE_SAME_ACCESS) ||
		!hProxyWr )
	{
		CloseHandle(hParentProcess);
		CloseHandle(hProxyRd);
		return -1;
	}

	reply(hProxyWr, L"00000000");

	int nRet = -1;
	while(WaitForSingleObject(hParentProcess, 0) != WAIT_OBJECT_0)
	{
		WCHAR buf[512];
		DWORD nSize = 0;
		if ( ReadFile(hProxyRd, buf, sizeof(buf), (LPDWORD)&nSize, NULL) )
		{
			if (nSize && buf[nSize/sizeof(WCHAR)-1]==L'\0')
			{
				WCHAR command[512];
				lstrcpyW(command, buf);

				WCHAR* type[1]={0};
				parse_cmd(command, type, sizeof(type)/sizeof(type[0]));
				if(lstrcmpiW(type[0], L"process")==0)
				{
					lstrcpyW(command, buf);
					WCHAR* proc_cmd[4];
					parse_cmd(command, proc_cmd, sizeof(proc_cmd)/sizeof(proc_cmd[0]));
					HANDLE hPipeWr = hex2handle(proc_cmd[1]);

					DuplicateHandle(hParentProcess, hPipeWr, GetCurrentProcess(), &hPipeWr, 0, TRUE, DUPLICATE_SAME_ACCESS);

					PROCESS_INFORMATION pi = {0,0,0,0};
					STARTUPINFOW si = {sizeof(STARTUPINFOW), 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
					si.hStdOutput = hPipeWr;
					si.hStdError = hPipeWr;
					si.dwFlags |= STARTF_USESTDHANDLES;

					HANDLE hNewProcess = NULL;
					wsprintfW(buf, L"\"%s\" %s", proc_cmd[3], proc_cmd[3]+lstrlenW(proc_cmd[3])+1);
					if( ::CreateProcessW(NULL, buf, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, proc_cmd[2], &si, &pi) )
						DuplicateHandle (GetCurrentProcess(), pi.hProcess, hParentProcess, &hNewProcess, 0, FALSE, DUPLICATE_SAME_ACCESS);

					WCHAR rpl[9];
					wsprintfW(rpl, L"%08X", hNewProcess);
					reply(hProxyWr, rpl);
				}
				else if(lstrcmpiW(type[0], L"shellexec")==0)
				{
					lstrcpyW(command, buf);
					WCHAR* proc_cmd[4];
					parse_cmd(command, proc_cmd, sizeof(proc_cmd)/sizeof(proc_cmd[0]));
					HWND hWnd = (HWND)hex2handle(proc_cmd[1]);

					SHELLEXECUTEINFOW _info = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
					_info.cbSize = sizeof(_info);
					_info.fMask = SEE_MASK_NOCLOSEPROCESS;
					_info.nShow = SW_MINIMIZE;
					_info.lpFile = proc_cmd[2];
					_info.hwnd = hWnd;
					_info.lpParameters = proc_cmd[3] + lstrlenW(proc_cmd[3])+1;
					_info.lpDirectory = proc_cmd[3];

					HANDLE hNewProcess = NULL;
					if( ::ShellExecuteExW(&_info) )
						DuplicateHandle (GetCurrentProcess(), _info.hProcess, hParentProcess, &hNewProcess, 0, FALSE, DUPLICATE_SAME_ACCESS);

					WCHAR rpl[9];
					wsprintfW(rpl, L"%08X", hNewProcess);
					reply(hProxyWr, rpl);
				}
				else if(lstrcmpiW(type[0], L"stop")==0)
				{
					nRet = 0;
					break;
				}
				
			}

			nSize = 0;
		}
		else
			break;
	}

	CloseHandle(hParentProcess);
	CloseHandle(hProxyRd);
	CloseHandle(hProxyWr);

	return nRet;
}

int APIENTRY entry_WinMain(void)
{
	int nRet = fMain();
	ExitProcess(nRet);
	return nRet;
}



