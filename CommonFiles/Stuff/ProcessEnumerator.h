// ProcessEnumerator.h: interface for the CProcessEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCESSENUMERATOR_H__25B28C51_14DD_4784_B50F_AF19B9EEC9E4__INCLUDED_)
#define AFX_PROCESSENUMERATOR_H__25B28C51_14DD_4784_B50F_AF19B9EEC9E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include "memmanag.h"

// ---
struct CProcessInfo {
	TCHAR  m_pProcessName[_MAX_PATH];
	DWORD  m_dwProcessID;
};

// ---
struct CModuleInfo {
	TCHAR  m_pModuleName[_MAX_PATH];
	HANDLE m_hModuleHandle;                 
};

// ---
struct CProcessEnumeratorPrivate {
	struct _ENUM_NT {
		CHAR*           m_pInfoBuff;    // Pointer on process list buffer
		PROCESS_INFO   *m_pNextProcess; // Internal variable. Pointer to next process info
	} ENUM_NT;
	
	struct _ENUM_9x {
		HANDLE         m_hProcessSnap; // Handle for navigate throw process list
		PROCESSENTRY32 m_PE32;         // process info structure
	} ENUM_9x;
	
	CHAR          m_ObjectName[_MAX_PATH];		 // --
	CHAR          m_ObjectPath[_MAX_PATH];		 // --
	BOOL					m_ObjectNew;                 // --
	CHAR          m_Mask[_MAX_PATH];           // --
	BOOL					m_IsFolder;                  // --
	DWORD					m_ThreadsNum;                // Number of process's threads
	DWORD					m_PID;                       // Process identificator
	DWORD					m_ParentPID;                 // ID for parent process
	OSVERSIONINFO m_OsVersionInfo;             // internal variable
};


// ---
class CProcessEnumerator {
	BOOL                      m_bInited;   
	CProcessEnumeratorPrivate m_Data;
public:
	CProcessEnumerator();
	virtual ~CProcessEnumerator();

	BOOL Next( CProcessInfo *pProcessInfo );
};

// ---
struct CModuleEnumeratorPrivate {
	struct _ENUM_NT {
		DWORD m_Mod; // --
		DWORD m_XZ;  // --
	} ENUM_NT;
	
	struct _ENUM_9x {
		MODULEENTRY32 m_ME; // --
	} ENUM_9x;
	
	DWORD         m_PID;                        // --
	CHAR          m_ModName[cNAME_MAX_LEN];     // --
	CHAR          m_ProcessName[cNAME_MAX_LEN]; // --
	CHAR          m_ObjectPath[cNAME_MAX_LEN];  // --
	CHAR          m_Mask[cNAME_MAX_LEN];        // --
	HANDLE        m_hProcess;                   // --
	HANDLE        m_hModHandle;                 // --
	DWORD					m_ModMemSize;					// --
	OSVERSIONINFO m_OsVersionInfo;              // --
};

// ---
class CModuleEnumerator {
	BOOL                     m_bInited;
	CModuleEnumeratorPrivate m_Data;
public:
	CModuleEnumerator( DWORD dwPID );
	virtual ~CModuleEnumerator();

	BOOL Next( CModuleInfo *pModuleInfo );
};

#endif // !defined(AFX_PROCESSENUMERATOR_H__25B28C51_14DD_4784_B50F_AF19B9EEC9E4__INCLUDED_)
