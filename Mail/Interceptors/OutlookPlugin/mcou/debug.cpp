/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	debug.cpp
*		\brief	Trace debug support
*		
*		\author Nick Lebedev
*		\date	19.06.2003 15:25:21
*		
*		Example:	
*		
*		
*		
*/		

#include "stdafx.h"
#include "debug.h"
#include "dbghelp.h"
#include "mcou.h"
#include "util.h"

#include <PPP/common/SpecialName.h>

void MCOU::TranslFunc (unsigned int n, LPEXCEPTION_POINTERS lpExceptPointers)
{
	PEXCEPTION_RECORD er = lpExceptPointers->ExceptionRecord;
	
	PR_TRACE((0, prtIMPORTANT, "Unhandled exception 0x%08x occured", n));
	PR_TRACE((0, prtIMPORTANT, "EXCEPTION_RECORD ===================="));
	PR_TRACE((0, prtIMPORTANT, "Code = %#08x", er->ExceptionCode));
	PR_TRACE((0, prtIMPORTANT, "Flags = %#08x", er->ExceptionFlags));
	PR_TRACE((0, prtIMPORTANT, "Address = %#08x", (LONG)er->ExceptionAddress));
	for (DWORD i = 0; i < er->NumberParameters; ++i)
		PR_TRACE((0, prtIMPORTANT, "Parameter %d = %#08x", i, er->ExceptionInformation[i]));
	PR_TRACE((0, prtIMPORTANT, "====================================="));

	PR_TRACE((0, prtIMPORTANT, "Trying to write crash dump..."));

	HINSTANCE hDbgHelp = ::LoadLibrary (_T("dbghelp.dll"));

	if (hDbgHelp)
	{
		typedef BOOL
		(WINAPI *fnMiniDumpWriteDump)(
			IN HANDLE hProcess,
			IN DWORD ProcessId,
			IN HANDLE hFile,
			IN MINIDUMP_TYPE DumpType,
			IN CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, OPTIONAL
			IN CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, OPTIONAL
			IN CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam OPTIONAL
			);

		fnMiniDumpWriteDump MiniDumpWriteDump = (fnMiniDumpWriteDump)::GetProcAddress (hDbgHelp, _T("MiniDumpWriteDump"));

		if (MiniDumpWriteDump)
		{
			tCHAR strModulePath[MAX_PATH] = {0};
			MCOU::GetThisModuleFileName(strModulePath, MAX_PATH);
			tCHAR *pLastSlash = strrchr(strModulePath, '\\');
			tCHAR *strModule;
			if(pLastSlash)
				strModule = pLastSlash + 1;
			else
				strModule = strModulePath;

			TCHAR szDumpPath[MAX_PATH] = {0};
			MakeSpecialName(szDumpPath, MAX_PATH, strModule, "dmp");

			HANDLE hFile = CreateFile (szDumpPath,
											GENERIC_WRITE,
											0,
											NULL,
											CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL,
											NULL);
			
			if (hFile != INVALID_HANDLE_VALUE)
			{
				MCOU::CWaitCur wc;

				MINIDUMP_EXCEPTION_INFORMATION mexi;

				mexi.ThreadId = GetCurrentThreadId();
				mexi.ExceptionPointers = lpExceptPointers;
				mexi.ClientPointers = FALSE;

				if (MiniDumpWriteDump (GetCurrentProcess (), GetCurrentProcessId (),
										hFile, MiniDumpWithFullMemory, &mexi, NULL, NULL))
				{
					PR_TRACE((0, prtIMPORTANT, "Crash dump '%hs' successfully created", szDumpPath));
					TCHAR szMessage[256];
					_sntprintf_s(szMessage, _countof(szMessage), _TRUNCATE, _T("Kaspersky Anti-Virus has encountered a problem.\nWe are sorry for the inconvenience.\n\nCrash dump has been written to '%s'\n"), szDumpPath);
					::MessageBox(GetForegroundWindow(), szMessage, "Kaspersky Anti-Virus", MB_OK | MB_ICONSTOP);
				}
				else
				{
					PR_TRACE((0, prtIMPORTANT, "Failed to create crash dump '%hs'", szDumpPath));
				}

				CloseHandle (hFile);
			}
			else
			{
				PR_TRACE((0, prtIMPORTANT, "Unable to create dump file (0x%08x)", GetLastError ()));
			}
		}
		else
		{
			PR_TRACE((0, prtIMPORTANT, "MiniDumpWriteDump function not found in dbghelp.dll"));
		}
		::FreeLibrary (hDbgHelp);
	}
	else
	{
		PR_TRACE((0, prtIMPORTANT, "Unable to load dbghelp.dll (0x%08x)", GetLastError ()));
	}
}

void MCOU::HandleException (LPCTSTR szFileName, DWORD dwLine, LPEXCEPTION_POINTERS lpExceptPointers)
{
	PR_TRACE((0, prtIMPORTANT, "Unhandled exception occured: %hs (%d)", szFileName, dwLine));
}