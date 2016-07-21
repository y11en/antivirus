// ProcessEnumerator.cpp: implementation of the CProcessEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <Stuff\ProcessEnumerator.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ---
CProcessEnumerator::CProcessEnumerator() {
	m_bInited = FALSE;
	memset( &m_Data, 0, sizeof(m_Data) );

	m_Data.ENUM_NT.m_pInfoBuff = NULL;
	m_Data.ENUM_NT.m_pNextProcess = NULL;
	m_Data.m_ObjectNew = FALSE;
	m_Data.m_ThreadsNum = 0;
	m_Data.m_PID = (DWORD) -1;
	m_Data.m_ParentPID = (DWORD) -1;
	
	m_Data.ENUM_9x.m_hProcessSnap = (HANDLE) -1;
	m_Data.ENUM_9x.m_PE32.dwSize = 0;
	
	m_Data.m_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if ( ::GetVersionEx(&m_Data.m_OsVersionInfo) == TRUE ) {
		::InitWinMemManagement(	m_Data.m_OsVersionInfo.dwPlatformId, malloc, free );
		switch ( m_Data.m_OsVersionInfo.dwPlatformId ) {
			case VER_PLATFORM_WIN32_NT:
				if ( ::GetNTProcessInfo(&m_Data.ENUM_NT.m_pInfoBuff) == TRUE ) {
					m_Data.ENUM_NT.m_pNextProcess = NULL;
					m_bInited = TRUE;
				}
				break;
			case VER_PLATFORM_WIN32_WINDOWS:
				m_Data.ENUM_9x.m_hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
				if ( m_Data.ENUM_9x.m_hProcessSnap != (HANDLE)-1 )
					m_bInited = TRUE;
				break;
		}
		m_bInited = TRUE;
	}
}

// ---
CProcessEnumerator::~CProcessEnumerator() {
	if ( m_Data.ENUM_NT.m_pInfoBuff != NULL )
		free( m_Data.ENUM_NT.m_pInfoBuff );
	
	m_Data.ENUM_NT.m_pInfoBuff = NULL;
	
	if ( m_Data.ENUM_9x.m_hProcessSnap != (HANDLE) -1 )
		::CloseHandle( m_Data.ENUM_9x.m_hProcessSnap );
	
	m_Data.ENUM_9x.m_hProcessSnap = (HANDLE) -1;
	m_Data.ENUM_9x.m_PE32.dwSize = 0;
}

// ---
BOOL CProcessEnumerator::Next( CProcessInfo *pProcessInfo ) {
	if ( m_bInited ) {
		m_Data.m_ThreadsNum = 0;
		m_Data.m_PID = (DWORD) -1;
		m_Data.m_ParentPID = (DWORD) -1;
		
		switch ( m_Data.m_OsVersionInfo.dwPlatformId ) {
			case VER_PLATFORM_WIN32_NT : {
				if ( m_Data.ENUM_NT.m_pNextProcess == NULL )
					m_Data.ENUM_NT.m_pNextProcess = (PROCESS_INFO*) m_Data.ENUM_NT.m_pInfoBuff;
				
				if ( m_Data.ENUM_NT.m_pInfoBuff != NULL )	{
					if ( m_Data.ENUM_NT.m_pNextProcess->Len != -1 )	{
						m_Data.m_PID = m_Data.ENUM_NT.m_pNextProcess->PID;
						m_Data.m_ParentPID = m_Data.ENUM_NT.m_pNextProcess->ParentPID;
						m_Data.m_ThreadsNum = m_Data.ENUM_NT.m_pNextProcess->ThreadsNum;
						
						if ( m_Data.ENUM_NT.m_pNextProcess->PID == 0 )
							lstrcpynA( m_Data.m_ObjectName, "System Idle", sizeof(m_Data.m_ObjectName) );
						else
							//cNAME_MAX_LEN
							wsprintfA(m_Data.m_ObjectName, "%S", (const char*) m_Data.ENUM_NT.m_pNextProcess->ProcessName);
						
						if ( m_Data.ENUM_NT.m_pNextProcess->Len == 0 )
							m_Data.ENUM_NT.m_pNextProcess->Len = -1;
						else
							m_Data.ENUM_NT.m_pNextProcess = (PROCESS_INFO*)((CHAR*)m_Data.ENUM_NT.m_pNextProcess + m_Data.ENUM_NT.m_pNextProcess->Len);

						if ( pProcessInfo ) {
							pProcessInfo->m_dwProcessID = m_Data.m_PID;
#ifdef UNICODE
							MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, m_Data.m_ObjectName, -1, pProcessInfo->m_pProcessName, sizeof(pProcessInfo->m_pProcessName) );
#else
							lstrcpynA( pProcessInfo->m_pProcessName, m_Data.m_ObjectName, sizeof(pProcessInfo->m_pProcessName) );
#endif
						}
						return TRUE;
					}
				}
			}
				break;
			case VER_PLATFORM_WIN32_WINDOWS : {
				if ( m_Data.ENUM_9x.m_hProcessSnap != (HANDLE) -1 ) {
					BOOL bResult;
					if ( m_Data.ENUM_9x.m_PE32.dwSize == 0 ) {
						m_Data.ENUM_9x.m_PE32.dwSize = sizeof(PROCESSENTRY32);
						bResult = pProcess32First( m_Data.ENUM_9x.m_hProcessSnap, &m_Data.ENUM_9x.m_PE32 );
					}
					else
						bResult = pProcess32Next(m_Data.ENUM_9x.m_hProcessSnap, &m_Data.ENUM_9x.m_PE32);
					
					if ( bResult == TRUE ) {
						DWORD len;
						char *pch;
						m_Data.m_PID = m_Data.ENUM_9x.m_PE32.th32ProcessID;
						m_Data.m_ParentPID = m_Data.ENUM_9x.m_PE32.th32ParentProcessID;
						m_Data.m_ThreadsNum = m_Data.ENUM_9x.m_PE32.cntThreads;
						//!! name
						lstrcpynA(m_Data.m_ObjectName, m_Data.ENUM_9x.m_PE32.szExeFile, cNAME_MAX_LEN);
						
						len = (DWORD)strlen(m_Data.ENUM_9x.m_PE32.szExeFile) + 1;
						for (pch = m_Data.ENUM_9x.m_PE32.szExeFile + len; len != 0 && *pch != '\\'; len--, pch--);
						pch++;
						len = (DWORD)strlen(pch) + 1;
						lstrcpynA( m_Data.m_ObjectName, pch, len );
						
						if ( pProcessInfo ) {
							pProcessInfo->m_dwProcessID = m_Data.m_PID;
#ifdef UNICODE
							MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, m_Data.m_ObjectName, -1, pProcessInfo->m_pProcessName, sizeof(pProcessInfo->m_pProcessName) );
#else
							lstrcpynA( pProcessInfo->m_pProcessName, m_Data.m_ObjectName, sizeof(pProcessInfo->m_pProcessName) );
#endif
						}
						return TRUE;
					}
				}
			}
			break;
		}
	}
	return FALSE;
}




//////////////////////////////////////////////////////////////////////
// CModuleEnumerator Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ---
CModuleEnumerator::CModuleEnumerator( DWORD dwPID ) {
	m_bInited = FALSE;
	memset( &m_Data, 0, sizeof(m_Data) );
	
	m_Data.ENUM_NT.m_XZ = 0;
	m_Data.m_hProcess = NULL;
	m_Data.m_hModHandle = NULL;
	m_Data.m_ModMemSize = 0;
	m_Data.m_PID = dwPID;
	
	m_Data.ENUM_9x.m_ME.dwSize = 0;
	
	m_Data.m_OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if ( ::GetVersionEx(&m_Data.m_OsVersionInfo) == TRUE ) {
		if (m_Data.m_PID != -1)	{
			::InitWinMemManagement(	m_Data.m_OsVersionInfo.dwPlatformId, malloc, free );
			switch ( m_Data.m_OsVersionInfo.dwPlatformId ) {
				case VER_PLATFORM_WIN32_NT : {
					CHAR *pBuffer = NULL;
					if ( ::GetNTProcessInfo(&pBuffer) != FALSE ) {
						BOOL bFound = FALSE;
						PROCESS_INFO* pNextProcess;

						pNextProcess = (PROCESS_INFO*) pBuffer;

						do {
							if (m_Data.m_PID == pNextProcess->PID) 
								bFound = TRUE;
						}	while( bFound == FALSE && pNextProcess->Len && (pNextProcess = (PROCESS_INFO*)((CHAR*)pNextProcess + pNextProcess->Len)) );

						if ( bFound == TRUE ) {
							//pNextProcess is nash
							PROC_INFO ProcInfoBuff;
							DWORD dwAccessMask = PROCESS_ALL_ACCESS;
							m_Data.m_hProcess = ::OpenProcess( dwAccessMask, FALSE, m_Data.m_PID );
							if( m_Data.m_hProcess != NULL )	{
								if ( !pZwQueryInformationProcess(m_Data.m_hProcess, 0, (PVOID) &ProcInfoBuff, sizeof(ProcInfoBuff),NULL) ) {
									if ( ::ReadProcessMemory(m_Data.m_hProcess, (PVOID) (ProcInfoBuff.UPEB + 0x0c), &m_Data.ENUM_NT.m_XZ, sizeof(m_Data.ENUM_NT.m_XZ), NULL) ) {
										if( ::ReadProcessMemory(m_Data.m_hProcess, (PVOID)(m_Data.ENUM_NT.m_XZ += 0x14), &m_Data.ENUM_NT.m_Mod, sizeof(m_Data.ENUM_NT.m_Mod), NULL) ) {
											wsprintfA( m_Data.m_ProcessName, "%S", (const char*) pNextProcess->ProcessName );
											m_bInited = TRUE;
										}
									}
								}
							}
							if ( !m_bInited )	{
								::CloseHandle(m_Data.m_hProcess);
								m_Data.m_hProcess = NULL;
							}
							if ( pBuffer )
								free( pBuffer );
						}
					}
				}
					break;
				case VER_PLATFORM_WIN32_WINDOWS : {
					m_Data.m_hProcess = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_Data.m_PID);
					if ( m_Data.m_hProcess == (HANDLE) -1 )	{
						m_Data.m_hProcess = NULL;
					}
					else {
						PROCESSENTRY32 PE32;
						PE32.dwSize = sizeof(PROCESSENTRY32);
						if ( pProcess32First(m_Data.m_hProcess, &PE32) ) {
							DWORD len;
							char *pch;
							
							lstrcpynA( m_Data.m_ProcessName, PE32.szExeFile, sizeof(m_Data.m_ProcessName) );
							
							len = (DWORD)strlen(PE32.szExeFile) + 1;
							for (pch = PE32.szExeFile + len; len != 0 && *pch != '\\'; len--, pch--);
							pch++;
							len = (DWORD)strlen(pch) + 1;
							lstrcpynA(m_Data.m_ProcessName, pch, len);
						}
						else
							lstrcpyA(m_Data.m_ProcessName, "<some process>"); 
						m_bInited = TRUE;
					}
				}
				break;
			}
		}
	}
}

// ---
CModuleEnumerator::~CModuleEnumerator() {
	if ( m_Data.m_hProcess != NULL )
		::CloseHandle( m_Data.m_hProcess );
}

// ---
BOOL CModuleEnumerator::Next( CModuleInfo *pModuleInfo ) {
	if ( m_bInited ) {
		if ( m_Data.m_hProcess != NULL ) {
			BOOL bError = FALSE;
			switch ( m_Data.m_OsVersionInfo.dwPlatformId ) {
				case VER_PLATFORM_WIN32_NT : {
						ModuleInfo Mi;
						if ( m_Data.ENUM_NT.m_Mod != m_Data.ENUM_NT.m_XZ ) {
							if ( ::ReadProcessMemory( m_Data.m_hProcess, (PVOID)(m_Data.ENUM_NT.m_Mod - 8), &Mi, sizeof(Mi), NULL) ) {
								// getting name
								WCHAR  *Str;
								SIZE_T len = Mi.ModNameLen;
								m_Data.m_hModHandle = Mi.hModule;
								m_Data.m_ModMemSize = Mi.VirtualImageSize;
								Str = (WCHAR *)malloc(len);
								if ( Str != NULL ) {
									if ( !::ReadProcessMemory(m_Data.m_hProcess, Mi.pModName, Str, len, &len) )	{
										bError = TRUE;
										len=0;
									}
									wcstombs( m_Data.m_ModName, Str, len );
									free(Str);
								}
								else
									bError = TRUE;
								
								len = Mi.FullFileNameLen;
								Str = (WCHAR *)malloc( len );
								if( Str != NULL )	{
									if ( !::ReadProcessMemory(m_Data.m_hProcess, Mi.pFullFileName, Str, len, &len) )	{
										bError = TRUE;
										len=0;
									}
									else
										wcstombs(m_Data.m_ObjectPath, Str, len);
									free(Str);
								}
								else
									bError = TRUE;
								
								m_Data.ENUM_NT.m_Mod = Mi.NextMod;
							}
							if ( bError == FALSE ) {
								if ( pModuleInfo ) {
									pModuleInfo->m_hModuleHandle = m_Data.m_hModHandle;
#ifdef UNICODE
									MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, m_Data.m_ObjectPath, -1, pModuleInfo->m_pModuleName, sizeof(pModuleInfo->m_pModuleName) );
#else
									lstrcpynA( pModuleInfo->m_pModuleName, m_Data.m_ObjectPath, sizeof(pModuleInfo->m_pModuleName) );
#endif
								}
								
								return TRUE;
							}
						}
					}
					break;
				case VER_PLATFORM_WIN32_WINDOWS : {
						if( m_Data.ENUM_9x.m_ME.dwSize == 0 ) {
							m_Data.ENUM_9x.m_ME.dwSize = sizeof(MODULEENTRY32);
							bError = !pModule32First(m_Data.m_hProcess, &m_Data.ENUM_9x.m_ME);
						}
						else {
							bError = !pModule32Next(m_Data.m_hProcess, &m_Data.ENUM_9x.m_ME);
						}
						if ( !bError ) {
							m_Data.m_hModHandle = m_Data.ENUM_9x.m_ME.hModule;
							m_Data.m_ModMemSize = m_Data.ENUM_9x.m_ME.modBaseSize;
							lstrcpynA( m_Data.m_ModName, m_Data.ENUM_9x.m_ME.szModule, cNAME_MAX_LEN );
							lstrcpynA( m_Data.m_ObjectPath, m_Data.ENUM_9x.m_ME.szExePath, cNAME_MAX_LEN );

							if ( pModuleInfo ) {
								pModuleInfo->m_hModuleHandle = m_Data.m_hModHandle;
#ifdef UNICODE
								MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, m_Data.m_ObjectPath, -1, pModuleInfo->m_pModuleName, sizeof(pModuleInfo->m_pModuleName) );
#else
								lstrcpynA( pModuleInfo->m_pModuleName, m_Data.m_ObjectPath, sizeof(pModuleInfo->m_pModuleName) );
#endif
							}

							return TRUE;
						}
					}
					break;
			}
		}
	}

	return FALSE;
}