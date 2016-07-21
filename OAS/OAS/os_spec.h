// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- task.cpp
// -------------------------------------------

#include <windows.h>
#include <prague.h>
#include <pr_cpp.h>

class COSDependent
{
public:
	COSDependent();

	void Init();
	bool IsWinNT(){ return m_is_nt; }

	HANDLE	GetThreadToken();
	bool	SetThreadToken(HANDLE hToken);

	tDWORD	GetSessionId(tDWORD nPID);

	tBOOL	CopyFile(cStringObj& srcPath, cStringObj& destPath);
	tBOOL	DeleteFile(cStringObj& srcPath);
	tBOOL	GetDriveType(cStringObj& strDrive);
	tBOOL	GetFileAttributesEx(cStringObj& strName, tDWORD fInfoLevelId, tPTR lpFileInformation);

private:
	bool m_is_nt;

	BOOL (WINAPI *m_OpenThreadToken)(HANDLE ThreadHandle, DWORD DesiredAccess, BOOL OpenAsSelf, PHANDLE TokenHandle);
	BOOL (WINAPI *m_SetThreadToken)(PHANDLE Thread, HANDLE Token);
	BOOL (WINAPI *m_ProcessIdToSessionId)(DWORD dwProcessId, DWORD* pSessionId);
};

extern COSDependent g_OSDependent;

// -------------------------------------------

