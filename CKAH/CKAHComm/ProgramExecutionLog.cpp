// ProgramExecutionLog.cpp: implementation of the CProgramExecutionLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProgramExecutionLog.h"
#include <debugging.h>
#include <stdio.h>
#include <tchar.h>
#define SS_NO_BSTR
#include <stuff/stdstring.h>
#include <stuff/cpointer.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgramExecutionLog::CProgramExecutionLog () :
	m_logger (NULL)
{
}

void CProgramExecutionLog::Initialize (EXTERNALLOGGERPROC logger)
{
	m_logger = logger;
}

void CProgramExecutionLog::Write (LPCTSTR pszMsg, DWORD dwEventType) const
{
	WriteFormat (_T("%s"), dwEventType, pszMsg);
}

void CProgramExecutionLog::WriteStr (LPCTSTR pszMsgFormat, LPCTSTR pszParam, DWORD dwEventType) const
{
	WriteFormat (pszMsgFormat, dwEventType, pszParam);
}

void CProgramExecutionLog::WriteDWORD (LPCTSTR pszMsgFormat, DWORD dwParam, DWORD dwEventType) const
{
	WriteFormat (pszMsgFormat, dwEventType, dwParam);
}

void CProgramExecutionLog::WriteFormat (LPCTSTR pszMsgFormat, DWORD dwEventType, ...) const
{
	if (m_logger)
	{
		USES_CONVERSION;

		const int buflen = 1024;
		TCHAR buffer[buflen];
		va_list arglist;
		va_start (arglist, dwEventType);
		int n = _vsntprintf (buffer, buflen - 4, pszMsgFormat, arglist);
		va_end (arglist);

		if (n < 0)
		{
			buffer[buflen - 4] = buffer[buflen - 3] = buffer[buflen - 2] = _T('.');
			buffer[buflen - 1] = 0;
		}

		m_logger (dwEventType, T2A (buffer));
	}
}
