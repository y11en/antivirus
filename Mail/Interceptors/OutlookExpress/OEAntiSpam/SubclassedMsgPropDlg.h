// SubclassedMsgPropDlg.h: interface for the CSubclassedMsgPropDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCLASSEDMSGPROPDLG_H__3D13C4FD_F028_4C27_9AFD_62ED63C85260__INCLUDED_)
#define AFX_SUBCLASSEDMSGPROPDLG_H__3D13C4FD_F028_4C27_9AFD_62ED63C85260__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SubclassedBase.h"
#include "SubclassedMsgSourceDlg.h"
#include "../../TheBatAntispam/EnsureCleanup.h"

class CSubclassedMsgPropDlg : public CSubclassedBase
{
public:
	void SetIsSpam(bool bIsSpam)
	{
		m_bIsSpam = bIsSpam;
	}

	virtual void AfterInitDialog();
	virtual void BeforeDestroy();
	virtual void BeforeTimer(UINT nTimerID);

private:
	CEnsureCloseHandle m_hSyncEvent;
	bool	m_bIsSpam;
	
	static CSubclassedMsgSourceDlg m_SourceDlg;
	static HHOOK m_hLocalHook;

	static LRESULT CALLBACK LocalCBTProc(int nCode, WPARAM wParam, LPARAM lParam);
};

#endif // !defined(AFX_SUBCLASSEDMSGPROPDLG_H__3D13C4FD_F028_4C27_9AFD_62ED63C85260__INCLUDED_)
