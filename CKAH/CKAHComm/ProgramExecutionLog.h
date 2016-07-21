// ProgramExecutionLog.h: interface for the CProgramExecutionLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRAMEXECUTIONLOG_H__7F4290E2_61FF_11D3_83C8_0000E816586C__INCLUDED_)
#define AFX_PROGRAMEXECUTIONLOG_H__7F4290E2_61FF_11D3_83C8_0000E816586C__INCLUDED_

#include "defs.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

#define PEL_ERROR		EVENTLOG_ERROR_TYPE
#define PEL_INFO		EVENTLOG_INFORMATION_TYPE
#define PEL_WARNING		EVENTLOG_WARNING_TYPE

//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// class CProgramExecutionLog
class CKAHCOMMEXPORT CProgramExecutionLog
{
public:
	CProgramExecutionLog ();

	typedef void (CALLBACK *EXTERNALLOGGERPROC) (DWORD dwEventType, LPCSTR szString);

	void Initialize (EXTERNALLOGGERPROC logger);

	void Write(LPCTSTR pszMsg, DWORD dwEventType = PEL_INFO) const;
	void WriteStr(LPCTSTR pszMsgFormat, LPCTSTR pszParam, DWORD dwEventType = PEL_INFO) const;
	void WriteDWORD(LPCTSTR pszMsgFormat, DWORD dwParam, DWORD dwEventType = PEL_INFO) const;
	void WriteFormat(LPCTSTR pszMsgFormat, DWORD dwEventType, ...) const;
private:
	EXTERNALLOGGERPROC m_logger;
};

#endif // !defined(AFX_PROGRAMEXECUTIONLOG_H__7F4290E2_61FF_11D3_83C8_0000E816586C__INCLUDED_)
