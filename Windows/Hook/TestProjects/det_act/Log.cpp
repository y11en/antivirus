// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Log.h"
#include "stuff/mm.h"

int CurrentTimeToString(PWCHAR pstrtime)
{
	SYSTEMTIME SysTime;
	GetSystemTime(&SysTime);

	return wsprintf(pstrtime, _T("%4d.%02d.%02d %02d:%02d:%02d\t"), 
		SysTime.wYear, SysTime.wMonth, SysTime.wDay, SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CLog::CLog()
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_pwchFileName = NULL;
	m_FilterCat = DCB_ALL;
	m_FilterLevel = DL_MAX;
	m_DbgFilterCat = 0;
	m_DbgFilterLevel = DL_MAX;
}

CLog::CLog(PWCHAR pFileName, CLog* pParentLog, __ulong FltCat, DEBUG_INFO_LEVEL FltLevel, __ulong DbgFltCat, DEBUG_INFO_LEVEL DbgFltLevel)
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_pwchFileName = NULL;
	m_FilterCat = FltCat;
	m_FilterLevel = FltLevel;
	m_DbgFilterCat = DbgFltCat;
	m_DbgFilterLevel = DbgFltLevel;
	InitLog(pFileName, pParentLog);
}

CLog::~CLog()
{
	FreeStr(m_pwchFileName);
}

bool CLog::OpenFile()
{
	m_hFile = CreateFile(m_pwchFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;

	SetFilePointer(m_hFile, 0, NULL, FILE_END);

	return true;
}

void CLog::ResetState()
{
	CloseHandle(m_hFile);
	m_hFile = INVALID_HANDLE_VALUE;
}

bool CLog::InitLog(PWCHAR pFileName, CLog* pParentLog)
{
	m_pParentLog = pParentLog;
	m_pwchFileName = (PWCHAR) global_Alloc(_WSTR_LEN_B(pFileName));
	if (0 == m_pwchFileName)
		return false;
	lstrcpy(m_pwchFileName, pFileName);

	return AddString(_T("Start session"));
}

bool CLog::IsValid()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		return true;

	return false;
}

bool CLog::AddString(PTCHAR ptchLogString)
{
	return AddString(DCB_ALL, DL_ALWAYS_REPORTED, ptchLogString);
}


bool CLog::AddString(__ulong Category, DEBUG_INFO_LEVEL Level, PTCHAR ptchLogString)
{
	bool bClose = false;
	bool bRet = false;
	PWCHAR pwchLogStr;
	DWORD Len;
	
	DWORD dwWr;
	
	if (m_pParentLog)
		m_pParentLog->AddFormatedString(Category, Level, _T("%s"), ptchLogString);
	if ((Category & m_DbgFilterCat) && (Level <= m_DbgFilterLevel))
		__DbPrintEx(Category, Level, _T("%s"), ptchLogString);
	//__DbPrintEx(DCB_SYSTEM, DL_INFO, L"m_FilterCat=%02X m_FilterLevel=%02X", m_FilterCat, m_FilterLevel);
	if ((Category & m_FilterCat)==0 || (Level > m_FilterLevel))
		return true;

	CAutoLock auto_lock(&m_Sync);

	if (!IsValid())
	{
		if (!OpenFile())
			return false;
		bClose = true;
	}

	pwchLogStr = (PWCHAR) global_Alloc(_WSTR_LEN_B(ptchLogString) + 0x100);
	if (NULL != pwchLogStr)
	{
		CurrentTimeToString(pwchLogStr);
		lstrcat(pwchLogStr, ptchLogString);
		lstrcat(pwchLogStr, _T("\r\n"));

		Len = lstrlen(pwchLogStr);
		WriteFile(m_hFile, pwchLogStr, Len * sizeof(WCHAR), &dwWr, NULL);

		FreeStr(pwchLogStr);

		if (0 != dwWr)
			bRet = true;
	}

	if (bClose)
		ResetState();
	return bRet;
}

void CLog::AddFormatedStringV(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt, va_list arg_list)
{
	WCHAR Buff[4096];
	wvsprintf(Buff, lpFmt, arg_list);
	AddString(Category, Level, Buff);
}

void CLog::AddFormatedString(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt,...)
{
	va_list arg_list;
	va_start(arg_list, lpFmt);
	AddFormatedStringV(Category, Level, lpFmt, arg_list);
	va_end(arg_list);
}
void CLog::AddFormatedStringV(LPCTSTR lpFmt, va_list arg_list)
{
	AddFormatedStringV(DCB_ALL, DL_ALWAYS_REPORTED, lpFmt, arg_list);
}

void CLog::AddFormatedString(LPCTSTR lpFmt,...)
{
	va_list arg_list;
	va_start(arg_list, lpFmt);
	AddFormatedStringV(DCB_ALL, DL_ALWAYS_REPORTED, lpFmt, arg_list);
	va_end(arg_list);
}

