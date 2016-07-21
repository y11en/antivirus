// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#include "os_spec.h"
#include <stdio.h>

// -------------------------------------------

COSDependent::COSDependent()
{
	memset(this, 0, sizeof(COSDependent));

	OSVERSIONINFO OS;
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OS);

	m_is_nt = OS.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

void COSDependent::Init()
{
	if( !m_is_nt )
		return;

	HMODULE hModAdvAPI = GetModuleHandle("AdvAPI32.dll");
	if( hModAdvAPI )
	{
		*(void**)&m_SetThreadToken = GetProcAddress(hModAdvAPI, "SetThreadToken");
		*(void**)&m_OpenThreadToken = GetProcAddress(hModAdvAPI, "OpenThreadToken");
	}
	HMODULE hModule = GetModuleHandle("Kernel32.dll");
	if( hModule )
	{
		*(void**)&m_ProcessIdToSessionId = GetProcAddress(hModule, "ProcessIdToSessionId");
	}
}

HANDLE COSDependent::GetThreadToken()
{
	HANDLE hToken = NULL;
	if( m_OpenThreadToken )
		m_OpenThreadToken(GetCurrentThread(), TOKEN_QUERY|TOKEN_IMPERSONATE, TRUE, &hToken);
	return hToken;
}

bool COSDependent::SetThreadToken(HANDLE hToken)
{
	return m_SetThreadToken ? !!m_SetThreadToken(NULL, hToken) : false;
}

tDWORD COSDependent::GetSessionId(tDWORD nPID)
{
	DWORD nSessionId = 0;
	if( m_ProcessIdToSessionId )
		m_ProcessIdToSessionId(nPID, &nSessionId);
	return nSessionId;
}

tBOOL COSDependent::CopyFile(cStringObj& srcPath, cStringObj& destPath)
{
	if( m_is_nt )
		return ::CopyFileW(srcPath.c_str(cCP_UNICODE), destPath.c_str(cCP_UNICODE), cFALSE);
	else
		return ::CopyFileA(srcPath.c_str(cCP_ANSI), destPath.c_str(cCP_ANSI), cFALSE);
}	

tBOOL COSDependent::DeleteFile(cStringObj& srcPath)
{
	if( m_is_nt )
		return ::DeleteFileW(srcPath.c_str(cCP_UNICODE));
	else
		return ::DeleteFileA(srcPath.c_str(cCP_ANSI));
}

tBOOL COSDependent::GetDriveType(cStringObj& strDrive)
{
	if( m_is_nt )
		return ::GetDriveTypeW(strDrive.c_str(cCP_UNICODE));
	else
		return ::GetDriveTypeA(strDrive.c_str(cCP_ANSI));
}

tBOOL COSDependent::GetFileAttributesEx(cStringObj& strName, tDWORD fInfoLevelId, tPTR lpFileInformation)
{
	if( m_is_nt )
		return ::GetFileAttributesExW(strName.c_str(cCP_UNICODE), (GET_FILEEX_INFO_LEVELS)fInfoLevelId, lpFileInformation);
	else
		return ::GetFileAttributesExA(strName.c_str(cCP_ANSI), (GET_FILEEX_INFO_LEVELS)fInfoLevelId, lpFileInformation);
}
