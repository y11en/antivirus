// ThNewProcess.h: interface for the CThModifiedProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ThModifiedProcess_H__FC3FA010_7B09_43CB_8806_AA054371B923__INCLUDED_)
#define AFX_ThModifiedProcess_H__FC3FA010_7B09_43CB_8806_AA054371B923__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskThread.h"
#include "WinFileInfo.h"

//+ ------------------------------------------------------------------------------------------

typedef struct _tRequest_StartModifiedProcess
{
	HANDLE m_hEventComplete;
	bool m_bVerdict;
	PWCHAR m_pwchImagePath;
	CWinFileInfo wininfo;
	CTime m_StartTime;
}Request_StartModifiedProcess;

//+ ------------------------------------------------------------------------------------------

class CThModifiedProcess : public HookTask
{
public:
	CThModifiedProcess();
	virtual ~CThModifiedProcess();

	bool StartLoop();
	void StopLoop();

	bool IsAllowStart(PWCHAR pwchImagePath);
	
	HWND GetHWND() {return m_hWnd;};

	bool Init();
	void BeforeExit();
private:
	HWND m_hWnd;
};

#endif // !defined(AFX_ThModifiedProcess_H__FC3FA010_7B09_43CB_8806_AA054371B923__INCLUDED_)
