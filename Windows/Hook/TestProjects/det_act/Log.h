// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__F02B44D8_81F4_47B1_B4F0_A9C27446D8E0__INCLUDED_)
#define AFX_LOG_H__F02B44D8_81F4_47B1_B4F0_A9C27446D8E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stuff/OwnSync.h"
#include "stuff/debug.h"

class CLog : public COwnSync
{
public:
	CLog();
	CLog(PWCHAR pFileName, CLog* pParentLog = NULL, __ulong FltCat = DCB_ALL, DEBUG_INFO_LEVEL FltLevel = DL_MAX, __ulong DbgFltCat = DCB_NOCAT, DEBUG_INFO_LEVEL DbgFltLevel = DL_MAX);
	virtual ~CLog();

	bool InitLog(PWCHAR pFileName, CLog* pParentLog = NULL);
	bool AddString(PWCHAR pwchLogString);
	bool AddString(__ulong Category, DEBUG_INFO_LEVEL Level, PTCHAR ptchLogString);
	void AddFormatedStringV(LPCTSTR lpFmt, va_list arg_list);
	void AddFormatedStringV(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt, va_list arg_list);
	void AddFormatedString(LPCTSTR lpFmt,...);
	void AddFormatedString(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt,...);

	__ulong m_FilterCat;
	DEBUG_INFO_LEVEL m_FilterLevel;

	__ulong m_DbgFilterCat;
	DEBUG_INFO_LEVEL m_DbgFilterLevel;

private:
	bool OpenFile();
	bool IsValid();

	void ResetState();
	HANDLE m_hFile;

	PWCHAR m_pwchFileName;
	CLog*  m_pParentLog;
};

extern CLog m_Log;
#endif // !defined(AFX_LOG_H__F02B44D8_81F4_47B1_B4F0_A9C27446D8E0__INCLUDED_)
