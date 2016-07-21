// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E67CDF14_D269_44B3_B1E3_227B741805E0__INCLUDED_)
#define AFX_STDAFX_H__E67CDF14_D269_44B3_B1E3_227B741805E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// TODO: reference additional headers your program requires here
#include <windows.h>
#include "hook\avpgcom.h"
#include "Hook\HookSpec.h"
#include "hook\funcs.h"

class LogEvent
{
private:
	HANDLE m_hFile;
	CRITICAL_SECTION m_Sync;
public:
	LogEvent();
	~LogEvent();
	HANDLE GetFile() {return m_hFile;};
public:
	void AddToLog(PEVENT_TRANSMIT pEvt);
	void AddToLog(LPCWSTR pwch, BOOL endofline = TRUE);
private:
	void AddToLogInt(LPCWSTR pwch, BOOL endofline = TRUE, BOOL bNeedProtect = TRUE);
	void AddToLogIntP(LPCWSTR pwch, BOOL endofline = TRUE);
};

extern LogEvent Log;
extern PCHAR gpModuleName;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E67CDF14_D269_44B3_B1E3_227B741805E0__INCLUDED_)
