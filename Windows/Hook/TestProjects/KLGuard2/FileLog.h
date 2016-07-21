// FileLog.h: interface for the CFileLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILELOG_H__79DDC9B3_5A81_41A8_9A44_6E3A35BDB816__INCLUDED_)
#define AFX_FILELOG_H__79DDC9B3_5A81_41A8_9A44_6E3A35BDB816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OwnSync.h"

class CFileLog : public COwnSync
{
public:
	CFileLog();
	virtual ~CFileLog();

	bool InitLog(CString* pFileName, bool bOverwrite);

	bool AddString(CString &LogString, bool bAddTime = true);
	bool AddString(PTCHAR ptchLogString, bool bAddTime = true);
private:
	bool IsValid();
	bool AddStringInternal(CString* pLogString);

	void ResetState();
	HANDLE m_hFile;
};

#endif // !defined(AFX_FILELOG_H__79DDC9B3_5A81_41A8_9A44_6E3A35BDB816__INCLUDED_)
