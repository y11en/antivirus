#include "assert.hpp"

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <crtdbg.h>
#include <string.h>
#include <dbghelp.h>
#include <dbghelp.h>
#include "ErrorFactory.hpp"

#pragma comment ( lib, "dbghelp.lib" )

extern void CreateMiniDump( EXCEPTION_POINTERS* pep ); 

extern void saveSEH();
extern void restoreSEH();

extern "C" {
	unsigned int old;
}

static bool checkForDebugger() {
    __try 
    {
        DebugBreak();
    }
    __except(GetExceptionCode() == EXCEPTION_BREAKPOINT ? 
             EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) 
    {
        // No debugger is attached, so return FALSE 
        // and continue.
        return false;
    }
    return true;
}


	
void saveSEH() {
	/*__asm {
		mov eax, fs:[0]
		mov old, eax
	}*/
}

void restoreSEH() {
	/*__asm {
		mov eax, old
		mov fs:[0], eax
	}*/
}

bool assert_impl(const wchar_t* message, const wchar_t* file, unsigned line) {
	fwprintf(stderr, L"Assert[%s,%d]: %s\n", file, line, message);
	fflush(stderr);
	/*if (checkForDebugger()) {
		return true;
	}*/
	CreateMiniDump(NULL);
	return true;
	//restoreSEH();
}

HANDLE OpenDumpFile() {
	const int bufferSize = 1024;
	static TCHAR buffer[bufferSize];
	static TCHAR bufferDmp[bufferSize];
	
	if (GetModuleFileName(NULL, buffer, bufferSize) == 0) {
		printf("GetModuleFileName failed. Error: %u \n", GetLastError()); 
		return NULL;
	}


	const int maxNumberOfDumps = 99;
	for (size_t i = 0; i < maxNumberOfDumps; ++i) {
		TCHAR ext[10];
		
		_stprintf(ext, _T(".%0.2d.dmp"), i);
		
		strcpy(bufferDmp, buffer);
		strcat(bufferDmp, ext);

		HANDLE hFile = CreateFile(bufferDmp, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL); 
		
		if (hFile != NULL && ( hFile != INVALID_HANDLE_VALUE )) {
			return hFile;
		}
	}
	
	printf("OpenDumpFile failed. Max number of dump files exceeded\n"); 
	return NULL;
}


void CreateMiniDump( EXCEPTION_POINTERS* pep ) {
#if 0 
	static int dumpCount = 0;
	
	if (dumpCount > 0) {
		// minidump should not be created more than once
		return;
	}
	
	++dumpCount;
	idl::ErrorFactory::out();

	_tprintf( _T("Creating minidump...\n") ); 
		
	HANDLE hFile = OpenDumpFile();/*CreateFile( OpenDumpFile(), GENERIC_READ | GENERIC_WRITE, 
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); */

	if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
	{
		// Create the minidump 

		MINIDUMP_EXCEPTION_INFORMATION mdei; 

		mdei.ThreadId           = GetCurrentThreadId(); 
		mdei.ExceptionPointers  = pep; 
		mdei.ClientPointers     = FALSE; 

		MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithFullMemory | 
		                                          //MiniDumpWithFullMemoryInfo | 
		                                          MiniDumpWithHandleData | 
		                                          //MiniDumpWithThreadInfo | 
		                                          MiniDumpWithUnloadedModules ); 

		BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
			hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 

		if( !rv ) 
			_tprintf( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() ); 
		else 
			_tprintf( _T("Minidump created\n") ); 

		// Close the file 

		CloseHandle( hFile ); 

	}
	else 
	{
		_tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
	}
#else
	_tprintf( _T("dump...\n") ); 
#endif
}

