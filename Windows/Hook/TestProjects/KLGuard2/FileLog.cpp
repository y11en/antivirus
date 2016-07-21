// FileLog.cpp: implementation of the CFileLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileLog.h"
#include "AutoPTCHAR.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileLog::CFileLog()
{
	m_hFile = INVALID_HANDLE_VALUE;
}

CFileLog::~CFileLog()
{
	CAutoLock auto_lock(&m_Sync);
	
	ResetState();
}

void CFileLog::ResetState()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

bool CFileLog::InitLog(CString* pFileName, bool bOverwrite)
{
	CAutoLock auto_lock(&m_Sync);
	DWORD CreateDisp;
	ResetState();

	if (bOverwrite)
		CreateDisp = CREATE_ALWAYS;
	else
		CreateDisp = OPEN_ALWAYS;

	m_hFile = CreateFile(*pFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CreateDisp, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
		return false;

	SetFilePointer(m_hFile, 0, NULL, FILE_END);
	return true;
}

bool CFileLog::IsValid()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		return true;

	return false;
}

bool CFileLog::AddStringInternal(CString* pLogString)
{
	int Len = pLogString->GetLength();
	AutoPTCHAR auptchValue(pLogString, Len);
	DWORD dwWr;

	pLogString->Replace(_T("\n"), _T("\r\n"));

	WriteFile(m_hFile, auptchValue.GetPTCH(), Len * sizeof(TCHAR), &dwWr, NULL);
	if (dwWr == 0)
		return false;

	return true;
}

bool CFileLog::AddString(CString& LogString, bool bAddTime)
{
	CAutoLock auto_lock(&m_Sync);
	if (!IsValid())
		return false;

	if (bAddTime)
	{
		CString strlog;
		CString strtime;
		CurrentTimeToString(&strtime);
		strlog.Format(_T("%s %s"), strtime, LogString);
		return AddStringInternal(&strlog);
	}

	return AddStringInternal(&LogString);
}

bool CFileLog::AddString(PTCHAR ptchLogString, bool bAddTime)
{
	CString strtmp = ptchLogString;
	
	return AddString(strtmp, bAddTime);
}