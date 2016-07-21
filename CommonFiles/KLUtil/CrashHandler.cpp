#include "stdafx.h"
#include "../Stuff/StdString.h"
#include "CrashHandler.h"

#include <dbghelp.h>

namespace KLUTIL
{
	bool WINAPI NotifyAboutCrashDump(LPCTSTR pszMiniDumpFile, LPCTSTR pszFullDumpFile)
	{
		CStdString text;
		text.Format(_T("\
Error has occurred while operating.\n\n\
The application's state at the moment this error occurred was recorded to allow troubleshooting.\n\
A full description is stored at:\n\t%s.\n\n\
A brief description is stored at:\n\t%s.\n\n\
We recommend that you contact Kaspersky Lab's technical support service.")
						  , pszFullDumpFile, pszMiniDumpFile);
		
		TCHAR szApp[MAX_PATH];
		GetModuleFileName(NULL, szApp, MAX_PATH);
		
		::MessageBox(NULL, text, szApp, MB_ICONERROR);
		
		return true;
	}
	
	static fnNotifyAboutCrashDump g_pfnNotifyAboutCrashDump = NotifyAboutCrashDump;
	static HMODULE g_hCrashedModule = NULL;

	void SetCrashedModule(HMODULE hCrashedModule)
	{
		g_hCrashedModule = hCrashedModule;
	}
	
	void SetNotifyAboutCrashDump(fnNotifyAboutCrashDump pfnNotifyAboutCrashDump)
	{
		g_pfnNotifyAboutCrashDump =
			pfnNotifyAboutCrashDump ? pfnNotifyAboutCrashDump : NotifyAboutCrashDump;
	}
	
	
#define VECT_EXCEPT_ONE_SLEEP_DELAY	1000
#define VECT_EXCEPT_CONTINUE_DELAY	20000
	
	typedef BOOL (WINAPI *fnMiniDumpWriteDump)(IN HANDLE hProcess,
		IN DWORD ProcessId,
		IN HANDLE hFile,
		IN MINIDUMP_TYPE DumpType,
		IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
		IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
		IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL);
	
	static fnMiniDumpWriteDump pfnMiniDumpWriteDump;
	
	CStdString WriteMiniDump(EXCEPTION_POINTERS *ExceptionPointers,
		LPCTSTR pszDumpName,
		MINIDUMP_TYPE DumpType)
	{
		CStdString res;
		GetModuleFileName(g_hCrashedModule, res.GetBuffer(MAX_PATH + 30), MAX_PATH + 30);
		res.ReleaseBuffer();
		int nPos = res.ReverseFind(_T('.'));
		if (nPos == -1)
			nPos = res.ReverseFind(_T('\\')) + 1;
		res.resize(nPos);
		
		CStdString ext;
		SYSTEMTIME st;
		GetLocalTime (&st);
		ext.Format(_T("%04i-%02i-%02i %02i-%02i-%02i.%s.dmp"),
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, pszDumpName);
		
		res += _T(" ") + ext;
		
		HANDLE hFile = CreateFile (res, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (hFile == INVALID_HANDLE_VALUE && GetLastError () == ERROR_ACCESS_DENIED)
		{
			GetTempPath(MAX_PATH, res.GetBuffer(MAX_PATH));			
			res.ReleaseBuffer();
			res += ext;
			hFile = CreateFile (res, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		
		if (hFile == INVALID_HANDLE_VALUE)
			return _T("");
		
		MINIDUMP_EXCEPTION_INFORMATION mexi;
		mexi.ThreadId = GetCurrentThreadId();
		mexi.ExceptionPointers = ExceptionPointers;
		mexi.ClientPointers = FALSE;
		
		bool bDumpRes = pfnMiniDumpWriteDump (GetCurrentProcess (), GetCurrentProcessId (),
			hFile, DumpType, &mexi, NULL, NULL) != FALSE;
		
		bDumpRes = bDumpRes &&  (::GetFileSize(hFile, NULL) != INVALID_FILE_SIZE);
		
		CloseHandle (hFile);
		
		return bDumpRes ? res : _T("");
	}
	
	bool WriteMiniDump(EXCEPTION_POINTERS *ExceptionPointers)
	{
		HINSTANCE hDbgHelp = ::LoadLibrary(_T("dbghelp.dll"));
		if (hDbgHelp == NULL)
			return false;
		
		pfnMiniDumpWriteDump = (fnMiniDumpWriteDump)::GetProcAddress (hDbgHelp, "MiniDumpWriteDump");
		if (pfnMiniDumpWriteDump == NULL)
		{
			::FreeLibrary(hDbgHelp);
			return false;
		}
		
		CStdString mini = WriteMiniDump(ExceptionPointers, _T("mini"), MiniDumpWithDataSegs);
		CStdString full = WriteMiniDump(ExceptionPointers, _T("full"), MiniDumpWithFullMemory);
		
		::FreeLibrary (hDbgHelp);
		
		return (!mini.IsEmpty() || !full.IsEmpty()) && KLUTIL::g_pfnNotifyAboutCrashDump(mini, full);
	}

	DWORD WINAPI WriteMiniDumpThread(LPVOID lpParameter)
	{
		return WriteMiniDump((EXCEPTION_POINTERS*) lpParameter) ? 1 : 0;
	}
	
	
	LONG WINAPI WriteCrashDumpExceptionFilter(EXCEPTION_POINTERS *ExceptionPointers)
	{
#ifdef _DEBUG
		if (ExceptionPointers != NULL
			&& ExceptionPointers->ExceptionRecord != NULL
			&& ExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
			return EXCEPTION_CONTINUE_SEARCH;
#endif
		static DWORD dwWriteMiniDumpRes = 0;
		static DWORD dummy;
		static HANDLE hThread;

		if (ExceptionPointers != NULL
			&& ExceptionPointers->ExceptionRecord != NULL
			&& ExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW
			&& (hThread = CreateThread(NULL, 0, WriteMiniDumpThread, ExceptionPointers, 0, &dummy)) != NULL)
		{
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &dwWriteMiniDumpRes);
		}
		else		
			dwWriteMiniDumpRes = WriteMiniDump(ExceptionPointers) ? 1 : 0;

		if (dwWriteMiniDumpRes == 1)
			return EXCEPTION_EXECUTE_HANDLER;
		
		if (ExceptionPointers == NULL || ExceptionPointers->ExceptionRecord == NULL)
			return EXCEPTION_CONTINUE_SEARCH;
		
		HANDLE hFile = CreateFile(_T("exception.log"),
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		
		if (hFile == INVALID_HANDLE_VALUE)
			return EXCEPTION_CONTINUE_SEARCH;
		
		PEXCEPTION_RECORD &er = ExceptionPointers->ExceptionRecord;
		
		DWORD dwNumBytesWritten;
		char msg[512];
		wsprintf(msg, _T("Unhandled exception %#08x occured\r\n\r\n"), er->ExceptionCode);
		WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
		
		do
		{
			wsprintf (msg, _T("EXCEPTION_RECORD ===============\r\n"));
			WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			
			wsprintf (msg, _T("Code = %#08x\r\n"), er->ExceptionCode);
			WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			
			wsprintf (msg, _T("Flags = %#08x\r\n"), er->ExceptionFlags);
			WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			
			wsprintf (msg, _T("Address = %#08x\r\n"), (LONG)er->ExceptionAddress);
			WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			
			for (DWORD i = 0; i < er->NumberParameters; ++i)
			{
				wsprintf (msg, _T("Parameter %d = %#08x\r\n"), i, er->ExceptionInformation[i]);
				WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			}
			
			wsprintf (msg, _T("================================\r\n"));
			WriteFile(hFile, msg, _tcslen (msg), &dwNumBytesWritten, NULL);
			
		} while ((er = er->ExceptionRecord) != NULL);
		
		CloseHandle (hFile);
		
		return EXCEPTION_EXECUTE_HANDLER;
	}
} // namespace KLUTIL