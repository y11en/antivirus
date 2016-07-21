// ThCMD_Remote.h: interface for the CThCMD_Remote class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THCMD_REMOTE_H__24535123_8CE9_4F39_B335_B51F8E5091CF__INCLUDED_)
#define AFX_THCMD_REMOTE_H__24535123_8CE9_4F39_B335_B51F8E5091CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TaskThread.h"
#include "StrHistory.h"

class CThCMD_Remote : public HookTask  
{
public:
	CThCMD_Remote();
	virtual ~CThCMD_Remote();

	bool StartLoop();
	void StopLoop();

	void ShowWarning(CString *pWarning);
	
	HWND GetHWND() {return m_hWnd;};
	HANDLE GetWarningEvent(){ return m_hWarningEvent;};

	bool PopEvent(CString* pstr) {return m_Warnings.PopEvent(pstr);};

	bool Init();
	void BeforeExit();
private:
	HWND m_hWnd;
	HANDLE m_hWarningEvent;

	CStrHistory m_Warnings;
};


#endif // !defined(AFX_THCMD_REMOTE_H__24535123_8CE9_4F39_B335_B51F8E5091CF__INCLUDED_)
