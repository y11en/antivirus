// kavshell.cpp : Defines the entry point for the console application.
//
//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <stdio.h>
#include <malloc.h>
#include <time.h>

#include "parsecmd.h"
#include <tchar.h>

#include <windows.h>

#include "..\Kav\ver_mod.h"

#ifndef countof
#define countof(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

void DisplayError(char *pszAPI);
bool PrepAndLaunchRedirectedChild(HANDLE hChildStdOut,
								  HANDLE hChildStdIn,
								  HANDLE hChildStdErr,
								  char* cmdline,
								  int argc,
								  char* argv[]);
DWORD WINAPI GetAndSendInputThread(LPVOID lpvThreadParam);
DWORD WINAPI ReadAndHandleOutputThread(LPVOID lpvThreadParam);
BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType);

PROCESS_INFORMATION child_info = {0};
HANDLE g_hStdIn = NULL;  // Handle to parents std input.
HANDLE g_hStdOut = NULL; // Handle to parents std output.
BOOL g_bRunThread = TRUE;

HANDLE g_hStopEvents[5]= {0};
#define g_hThreadInput g_hStopEvents[0]
#define g_hThreadOutput g_hStopEvents[1]
#define g_hStopEventLocal g_hStopEvents[2]
#define g_hStopEventCloseConsole g_hStopEvents[3]
#define g_hChildProcess g_hStopEvents[4]
HANDLE g_hBreakEvent = NULL;
HANDLE g_hCommReadEvent = INVALID_HANDLE_VALUE;
HANDLE g_hCommWriteEvent = INVALID_HANDLE_VALUE;
HANDLE g_hCommDevice = INVALID_HANDLE_VALUE;
time_t g_tBreakLastTime = 0;
int    g_nBreakCount = 0;
BOOL   g_bWin9x = FALSE;

int _printf(CHAR* format, ...)
{
	CHAR szPrintBuffer[0x400];
	DWORD nCharsWritten;
	va_list args;
	va_start( args, format );
	int len = vsprintf(szPrintBuffer, format, args);
	WriteFile(g_hStdOut, szPrintBuffer, len, &nCharsWritten, NULL);
	return len;
}

int main ()
{
	HANDLE hOutputReadTmp,hOutputRead,hOutputWrite;
	HANDLE hInputWriteTmp,hInputRead,hInputWrite;
	HANDLE hErrorWrite;
	DWORD ThreadId;
	SECURITY_ATTRIBUTES sa;
	OSVERSIONINFOA osvi;

	memset(&osvi, 0, sizeof(OSVERSIONINFOA));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
	if( !GetVersionExA(&osvi) || osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
	{
		g_bWin9x = TRUE;
	}

	// Get std input handle so you can close it and force the ReadFile to
	// fail when you want the input thread to exit.
	if ( (g_hStdIn = GetStdHandle(STD_INPUT_HANDLE)) ==
		INVALID_HANDLE_VALUE )
		DisplayError("GetStdHandle");
	if ( (g_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE)) ==
		INVALID_HANDLE_VALUE )
		DisplayError("GetStdHandle");


	// parse command line
	char** _argv;
	char* _args;
	int size;
	int argc;
	char* cmdline = (char*)GetCommandLine();
	parse_cmdline(cmdline, NULL, NULL,  &argc, &size);
	_argv = (char**)malloc(size+argc*sizeof(char*));
	if (_argv == NULL)
		return false;
	_args = (char*)_argv + argc*sizeof(char*);
	parse_cmdline(cmdline, _argv, _args,  &argc, &size);
	
	for (int i=1;i<argc;i++)
	{
		if (!_argv[i])
			break;
		if (_argv[i][0] == '-' || _argv[i][0] == '/')
		{
			char* pComDeviceName = NULL;
			if (_strnicmp(&_argv[i][1], "con", 3) == 0)
				pComDeviceName = _argv[i]+4;
			else if (_strnicmp(&_argv[i][1], "shell", 5) == 0)
				pComDeviceName = _argv[i]+6;
			if (pComDeviceName)
			{
				if (*pComDeviceName==':' || *pComDeviceName=='=')
				{
					pComDeviceName++;
					g_hCommDevice = CreateFile(pComDeviceName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
					if (g_hCommDevice != INVALID_HANDLE_VALUE)
					{
						_printf("Console redirected to %s:\r\n\r\n", pComDeviceName);
						g_hCommReadEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
						g_hCommWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
						DCB dcb;
						COMMPROP props;
						if (GetCommProperties(g_hCommDevice, &props))
						{
							if (GetCommState(g_hCommDevice, &dcb))
							{
								dcb.BaudRate = props.dwMaxBaud;     // set max baud rate
								SetCommState(g_hCommDevice, &dcb);
								//COMMTIMEOUTS ct = {MAXDWORD, MAXDWORD, 100, 0, 0};
								//SetCommTimeouts(g_hCommDevice, &ct);
							}
						}
					}
				}
			}
		}
	}
	
	// Set up the security attributes struct.
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
	// Create the child output pipe.
	if (!CreatePipe(&hOutputReadTmp,&hOutputWrite,&sa,0))
		DisplayError("CreatePipe");
	
	
	// Create a duplicate of the output write handle for the std error
	// write handle. This is necessary in case the child application
	// closes one of its std output handles.
	if (!DuplicateHandle(GetCurrentProcess(),hOutputWrite,
		GetCurrentProcess(),&hErrorWrite,0,
		TRUE,DUPLICATE_SAME_ACCESS))
		DisplayError("DuplicateHandle");
	
	
	// Create the child input pipe.
	if (!CreatePipe(&hInputRead,&hInputWriteTmp,&sa,0))
		DisplayError("CreatePipe");
	
	
	// Create new output read handle and the input write handles. Set
	// the Properties to FALSE. Otherwise, the child inherits the
	// properties and, as a result, non-closeable handles to the pipes
	// are created.
	if (!DuplicateHandle(GetCurrentProcess(),hOutputReadTmp,
		GetCurrentProcess(),
		&hOutputRead, // Address of new handle.
		0,FALSE, // Make it uninheritable.
		DUPLICATE_SAME_ACCESS))
		DisplayError("DupliateHandle");
	
	if (!DuplicateHandle(GetCurrentProcess(),hInputWriteTmp,
		GetCurrentProcess(),
		&hInputWrite, // Address of new handle.
		0,FALSE, // Make it uninheritable.
		DUPLICATE_SAME_ACCESS))
		DisplayError("DupliateHandle");
	
	
	// Close inheritable copies of the handles you do not want to be
	// inherited.
	if (!CloseHandle(hOutputReadTmp)) DisplayError("CloseHandle");
	if (!CloseHandle(hInputWriteTmp)) DisplayError("CloseHandle");
	
	
	//GetConsoleMode(g_hStdOut, &dwConsoleMode);
	//dwConsoleMode &=  ~ENABLE_PROCESSED_INPUT;
	//SetConsoleMode(g_hStdOut, dwConsoleMode);

	DWORD dwConsoleMode;
	GetConsoleMode(g_hStdIn, &dwConsoleMode);
	dwConsoleMode &=  ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT );
	dwConsoleMode |= ENABLE_PROCESSED_INPUT;
	SetConsoleMode(g_hStdIn, dwConsoleMode);
	GetConsoleMode(g_hStdIn, &dwConsoleMode);
	
	SetConsoleCtrlHandler(ConsoleControlHandler, TRUE);
	
	if (!PrepAndLaunchRedirectedChild(hOutputWrite,hInputRead,hErrorWrite, cmdline, argc, _argv))
	{
		DisplayError("Create process");
		return -1;
	}
	g_hChildProcess = child_info.hProcess;
	
	char ev_name[0x100];
	wsprintf(ev_name,"%s.HOST.%d", VER_PRODUCTNAME_STR, child_info.dwProcessId );
	g_hStopEventLocal = CreateEvent( NULL, TRUE, FALSE, ev_name);
	if(!g_hStopEventLocal) 
		DisplayError("Create Event");
	wsprintf(ev_name,"%s.CON.%d", VER_PRODUCTNAME_STR, child_info.dwProcessId );
	g_hStopEventCloseConsole = CreateEvent( NULL, TRUE, FALSE, ev_name);
	if(!g_hStopEventCloseConsole) 
		DisplayError("Create Event");
	wsprintf(ev_name,"%s.BREAK.%d", VER_PRODUCTNAME_STR, child_info.dwProcessId );
	g_hBreakEvent = CreateEvent( NULL, TRUE, FALSE, ev_name);
	if(!g_hBreakEvent) 
		DisplayError("Create Event");

	// Close pipe handles (do not continue to modify the parent).
	// You need to make sure that no handles to the write end of the
	// output pipe are maintained in this process or else the pipe will
	// not close when the child process exits and the ReadFile will hang.
	if (!CloseHandle(hOutputWrite)) DisplayError("CloseHandle");
	if (!CloseHandle(hInputRead )) DisplayError("CloseHandle");
	if (!CloseHandle(hErrorWrite)) DisplayError("CloseHandle");
	
	
	// Launch the thread that gets the input and sends it to the child.
	g_hThreadInput = CreateThread(NULL,0,GetAndSendInputThread, (LPVOID)hInputWrite,0,&ThreadId);
	if (g_hThreadInput == NULL) DisplayError("CreateThread");
	
	// Launch the thread that read the child's output and writes it to console.
	g_hThreadOutput = CreateThread(NULL,0,ReadAndHandleOutputThread, (LPVOID)hOutputRead,0,&ThreadId);
	if (g_hThreadOutput == NULL) DisplayError("CreateThread");
	// Redirection is complete
	
	
	if (WaitForMultipleObjects(5, g_hStopEvents, FALSE, INFINITE) == WAIT_FAILED)
		DisplayError("WaitForMultiple");
	SetEvent(g_hStopEventLocal);

	// perform "fast" exiting
	DWORD nRetCode = -1;
	GetExitCodeProcess(child_info.hProcess, &nRetCode);
	if(nRetCode == STILL_ACTIVE) nRetCode = 0;
	ExitProcess(nRetCode);


//	// Force the read on the input to return by closing the stdin handle.
//	if (!CloseHandle(g_hStdIn)) DisplayError("CloseHandle");
//	//if (!CloseHandle(hOutputRead)) DisplayError("CloseHandle");
//	// Tell the thread to exit and wait for thread to die.
//	g_bRunThread = FALSE;
//	
//	//if (WaitForMultipleObjects(2, g_hStopEvents, TRUE, INFINITE) == WAIT_FAILED)
//	//	DisplayError("WaitForThreadsExit");
//	
//	if (!CloseHandle(hInputWrite)) DisplayError("CloseHandle");
//	if (!CloseHandle(g_hStopEventLocal)) DisplayError("CloseHandle");
	return nRetCode;
}


BOOL WINAPI ConsoleControlHandler(DWORD dwCtrlType) 
{
	_printf("^C\r\n");
	switch( dwCtrlType ) {
		case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
		case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			SetEvent(g_hBreakEvent);
			if ((unsigned)time(0) - (unsigned)g_tBreakLastTime < 30)
			{
				g_nBreakCount++;
				if (g_nBreakCount == 3)
					ConsoleControlHandler(CTRL_CLOSE_EVENT);
			}
			else
			{
				g_nBreakCount = 0;
			}
			g_tBreakLastTime = time(0);
			return TRUE;
		case CTRL_CLOSE_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			_printf("Multiple ^C, exiting...\r\n");
			SetEvent(g_hStopEventLocal);
			return TRUE;
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////// 
// PrepAndLaunchRedirectedChild
// Sets up STARTUPINFO structure, and launches redirected child.
/////////////////////////////////////////////////////////////////////// 
bool PrepAndLaunchRedirectedChild(HANDLE hChildStdOut,
								  HANDLE hChildStdIn,
								  HANDLE hChildStdErr,
								  char* cmdline,
								  int argc,
								  char* argv[])
{
	STARTUPINFO si;
	char* pParams = NULL;
	char* pProgramm = NULL;
	char* pExt;

	pParams = (char*)malloc(strlen(cmdline) + 100);
	if (!pParams)
		return false;
	pProgramm = (char*)malloc(strlen(argv[0]) + 100);
	if (!pProgramm)
		return false;
	strcpy(pProgramm, argv[0]);
	pExt = strrchr(pProgramm, '.');
	if (!pExt)
		pExt = pProgramm+strlen(pProgramm);
	strcpy(pExt, ".exe");
	strcpy(pParams, "\"");
	strcat(pParams, pProgramm);
	strcat(pParams, "\" ");
//	strcat(pParams, " -con ");
	cmdline = strstr(cmdline, argv[0]);
	if (!cmdline)
		return false;
	cmdline += strlen(argv[0]);
	if (*cmdline == '\"')
		cmdline++;
	strcat(pParams, cmdline);
	
	// Set up the start up info struct.
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESTDHANDLES;
	si.hStdOutput = hChildStdOut;
	si.hStdInput  = hChildStdIn;
	si.hStdError  = hChildStdErr;
	// Use this if you want to hide the child:
	//     si.wShowWindow = SW_HIDE;
	// Note that dwFlags must include STARTF_USESHOWWINDOW if you want to
	// use the wShowWindow flags.
	
	
	// Launch the process that you want to redirect (in this case,
	// Child.exe). Make sure Child.exe is in the same directory as
	// redirect.c launch redirect from a command line to prevent location
	// confusion.
	if (!CreateProcess(NULL, pParams,NULL,NULL,TRUE,0,NULL,NULL,&si,&child_info))
	{
		//DisplayError("CreateProcess");
		return false;
	}
	
	// Close any unnecessary handles.
	// if (!CloseHandle(pi.hThread)) DisplayError("CloseHandle");
	return true;
}


/////////////////////////////////////////////////////////////////////// 
// ReadAndHandleOutputThread
// Monitors handle for input. Exits when child exits or pipe breaks.
/////////////////////////////////////////////////////////////////////// 
DWORD WINAPI ReadAndHandleOutputThread(LPVOID lpvThreadParam)
{
	CHAR lpBuffer[256];
	DWORD nBytesRead;
	DWORD nCharsWritten;
	HANDLE hPipeRead = (HANDLE)lpvThreadParam;
	OVERLAPPED ov = {0};
	ov.hEvent = g_hCommWriteEvent;
	
	while(g_bRunThread)
	{
		if (!ReadFile(hPipeRead,lpBuffer, countof(lpBuffer)-1,
			&nBytesRead,NULL) || !nBytesRead)
		{
            if (GetLastError() == ERROR_BROKEN_PIPE)
				break; // pipe done - normal exit path.
            else
				DisplayError("ReadFile"); // Something bad happened.
		}
		
		lpBuffer[nBytesRead] = '\0'; // Follow output with a NULL.

		//char msg[0x200];
		//int msg_len = sprintf(msg, "<-(%d)%s", nBytesRead, lpBuffer);
		//WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),msg,msg_len,&nCharsWritten,NULL);
		

		// Send output to COM device
		if (g_hCommDevice != INVALID_HANDLE_VALUE)
		{
			BOOL bRes = WriteFile(g_hCommDevice,lpBuffer,nBytesRead,&nCharsWritten,&ov);
			if (!bRes && GetLastError() == ERROR_IO_PENDING)
				bRes = GetOverlappedResult(g_hCommDevice, &ov, &nCharsWritten, TRUE);
		}

		// Display the character read on the screen.
		CharToOemBuffA(lpBuffer, lpBuffer, nBytesRead);
		WriteFile(g_hStdOut,lpBuffer,nBytesRead,&nCharsWritten,NULL);


		//msg_len = sprintf(msg, "%s\r\n", bRes ? "." : "<W>");
		//WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),msg,msg_len,&nCharsWritten,NULL);
	}
	return 0;
}


/////////////////////////////////////////////////////////////////////// 
// GetAndSendInputThread
// Thread procedure that monitors the console for input and sends input
// to the child process through the input pipe.
// This thread ends when the child application exits.
/////////////////////////////////////////////////////////////////////// 
DWORD WINAPI GetAndSendInputThread(LPVOID lpvThreadParam)
{
	CHAR read_buff[256];
	DWORD nBytesRead,nBytesWrote;
	HANDLE hPipeWrite = (HANDLE)lpvThreadParam;
	OVERLAPPED ov = {0};
	DWORD dwError;
	BOOL bResult;
	ov.hEvent = g_hCommReadEvent;
	HANDLE hInput = g_hCommDevice != INVALID_HANDLE_VALUE ? g_hCommDevice : g_hStdIn;
	
	// Get input from our console and send it to child through the pipe.
	while (g_bRunThread)
	{
		bResult = ReadFile(hInput,read_buff,1,&nBytesRead, hInput==g_hStdIn ? NULL : &ov);
		
		if(!bResult)
		{
			// if there was a problem, or the async. operation's still pending ... 
			// deal with the error code 
			dwError = GetLastError();
				
			if (ERROR_IO_PENDING == dwError)
			{ 
				// asynchronous i/o is still in progress 
				// check on the results of the asynchronous read 
				bResult = GetOverlappedResult(g_hStdIn, &ov, &nBytesRead, TRUE) ; 
				dwError = GetLastError();
			} // end pending
				
			if (ERROR_HANDLE_EOF == dwError) 
			{ 
				// we have reached the end of the file 
				// during the call to ReadFile 
				// code to handle that 
				dwError = 0;
				bResult = TRUE;
			} 
		}

		if (!bResult)
		{
            if (dwError == ERROR_INVALID_HANDLE)
				break; // Pipe was closed (normal exit path).
            else
			   DisplayError("ReadConsole");
		}

		if (!nBytesRead)
			continue;

		read_buff[nBytesRead] = '\0'; // Follow input with a NULL.

		//char msg[0x200];
		//int msg_len = sprintf(msg, "->(%d)%s", nBytesRead, read_buff);
		//WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),msg,msg_len,&nBytesWrote,NULL);

		if (!WriteFile(hPipeWrite,read_buff,nBytesRead,&nBytesWrote,NULL))
		{
            if (GetLastError() == ERROR_NO_DATA)
				break; // Pipe was closed (normal exit path).
            else
				DisplayError("WriteFile");
		}
		//msg_len = sprintf(msg, ".\r\n");
		//WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),msg,msg_len,&nBytesWrote,NULL);
	}
	return 1;
}


/////////////////////////////////////////////////////////////////////// 
// DisplayError
// Displays the error number and corresponding message.
/////////////////////////////////////////////////////////////////////// 
void DisplayError(char *pszAPI)
{
	LPVOID lpvMessageBuffer;
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpvMessageBuffer, 0, NULL);
	
	_printf("ERROR: API    = %s.\r\n   error code = %d.\r\n   message    = %s.\r\n",
		pszAPI, GetLastError(), (char *)lpvMessageBuffer);
	
	LocalFree(lpvMessageBuffer);
	ExitProcess(3);
}

