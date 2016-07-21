// ThNewProcess.h: interface for the CThNewProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THNEWPROCESS_H__FC3FA010_7B09_43CB_8806_AA054371B923__INCLUDED_)
#define AFX_THNEWPROCESS_H__FC3FA010_7B09_43CB_8806_AA054371B923__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskThread.h"
#include "WinFileInfo.h"
//+ ------------------------------------------------------------------------------------------

typedef enum _eStartNewProcess_Verdict
{
	_start_allowed = 0,
	_start_allowed_once = 1,
	_start_disabled =2
};

//+ ------------------------------------------------------------------------------------------

typedef struct _tRequest_StartProcess
{
	HANDLE m_hEventComplete;
	_eStartNewProcess_Verdict m_Verdict;
	CString* m_pstrInfo;
	PWCHAR m_pwchImagePath;
	CWinFileInfo wininfo;
	CTime m_StartTime;
	bool m_bSandBoxed;
}Request_StartProcess;

//+ ------------------------------------------------------------------------------------------

class CThNewProcess : public HookTask
{
public:
	CThNewProcess();
	virtual ~CThNewProcess();

	bool StartLoop();
	void StopLoop();

	_eStartNewProcess_Verdict IsAllowStart(CString *pStrInfo, PWCHAR pwchImagePath, bool bSandBoxed);
	
	HWND GetHWND() {return m_hWnd;};

	bool Init();
	void BeforeExit();
private:
	HWND m_hWnd;
};

#endif // !defined(AFX_THNEWPROCESS_H__FC3FA010_7B09_43CB_8806_AA054371B923__INCLUDED_)
