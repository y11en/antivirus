// CheckWindow.h: interface for the CCheckWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#if !defined(AFX_CHECKWINDOW_H__16C2BB26_40B1_43CF_9C4B_FE8541FBADAF__INCLUDED_)
#define AFX_CHECKWINDOW_H__16C2BB26_40B1_43CF_9C4B_FE8541FBADAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCheckWindow  
{
public:
	CCheckWindow();
	virtual ~CCheckWindow();
	
	virtual void AlertInvisilbe(ULONG ProcessId);
	virtual void CheckWindows(ULONG ProcessId);
	virtual bool HasVisibleWindow(ULONG ProcessId);
};

#endif // !defined(AFX_CHECKWINDOW_H__16C2BB26_40B1_43CF_9C4B_FE8541FBADAF__INCLUDED_)
