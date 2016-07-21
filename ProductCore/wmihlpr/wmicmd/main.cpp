#include <windows.h>

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

wchar_t * __cdecl __wcsrchr (
						   const wchar_t * string,
						   wchar_t ch
						   )
{
	wchar_t *start = (wchar_t *)string;

	while (*string++)                       /* find end of string */
		;
	/* search towards front */
	while (--string != start && *string != (wchar_t)ch)
		;

	if (*string == (wchar_t)ch)             /* wchar_t found ? */
		return( (wchar_t *)string );

	return(NULL);
}

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{
	WCHAR cmdParams[MAX_PATH];
	lstrcpyW(cmdParams, GetCommandLineW());
	WCHAR* argv[6];
	int argc = parse_cmd(cmdParams, argv, sizeof(argv)/sizeof(argv[0]));
	DWORD dwexit = -1;
	PROCESS_INFORMATION procinfo = {0, 0, 0, 0};
	
	STARTUPINFOW startinfo = {	0, 0, 0, 0, 
								0, 0, 0, 0, 
								0, 0, 0, 0, 
								0, 0, 0, 0, 
								0, 0 };

	startinfo.cb = sizeof(startinfo);
	startinfo.wShowWindow = SW_HIDE;

	WCHAR pwchExe[MAX_PATH];
	GetModuleFileNameW(NULL, pwchExe, MAX_PATH);

	int len;
	if(!pwchExe || !(len = lstrlenW(pwchExe)))
		return dwexit;

	WCHAR* strName = __wcsrchr(pwchExe, L'\\');
	if( !strName )
		return dwexit;

	strName++;


	LPCWSTR szProfile = NULL;
	bool bUpdateAvail = false;
	bool bSpywareAvail = false;
	bool bFWAvail = false;

	if( !lstrcmpiW(strName, L"wmiav.exe") )
	{
		szProfile = L"File_Monitoring";
		bUpdateAvail = true;
	}
	else if( !lstrcmpiW(strName, L"wmias.exe") )
	{
		szProfile = L"File_Monitoring";
		bUpdateAvail = true;
		bSpywareAvail = true;
	}
	else if( !lstrcmpiW(strName, L"wmifw.exe") )
	{
		szProfile = L"Anti_Hacker";
		bFWAvail = true;
	}

	if( !szProfile )
		return dwexit;

	lstrcpyW(strName, L"avp.exe");

	WCHAR szwSpywareCmd[MAX_PATH];
	szwSpywareCmd[0] = L'\0';

	WCHAR CommandLine[MAX_PATH];
	CommandLine[0] = L'\0';

	for(int i = 1; i < argc; i++)
	{
		if( !lstrcmpiW(argv[i], L"/enable") )
		{
			if (bSpywareAvail)
				wsprintfW(szwSpywareCmd, L"%s -hidden spyware on", L"AV");
			else if(bFWAvail)
				wsprintfW(szwSpywareCmd, L"%s -hidden start fw", L"AV");

			wsprintfW(CommandLine, L"%s -hidden start %s", L"AV", szProfile);
			break;
		}
		else if( !lstrcmpiW(argv[i], L"/update") && bUpdateAvail )
			wsprintfW(CommandLine, L"%s -hidden start updater -show", L"AV");
	}

	if(lstrlenW(CommandLine))
	{
		HANDLE hMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, L"Global\\AVP.Mutex.Kaspersky Anti-Virus.BL");
		if(hMutex)
			CloseHandle(hMutex);
		else
		{
			CreateProcessW(pwchExe, NULL, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startinfo, &procinfo);
			CloseHandle(procinfo.hProcess);
			CloseHandle(procinfo.hThread);
		}

		if (lstrlenW(szwSpywareCmd))
		{
			PROCESS_INFORMATION procinfo2 = {0, 0, 0, 0};
			CreateProcessW(pwchExe, szwSpywareCmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startinfo, &procinfo2);
			WaitForSingleObject(procinfo2.hProcess, 12000);
			
			CloseHandle(procinfo2.hProcess);
			CloseHandle(procinfo2.hThread);
		}

		CreateProcessW(pwchExe, CommandLine, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &startinfo, &procinfo);
		CloseHandle(procinfo.hProcess);
		CloseHandle(procinfo.hThread);

		dwexit = 0;
	}


	return dwexit;
}
