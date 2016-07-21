// SubclassedMsgSourceDlg.h: interface for the CSubclassedMsgSourceDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCLASSEDMSGSOURCEDLG_H__6BC62F08_5B1C_41BA_908D_F5603DB00F6B__INCLUDED_)
#define AFX_SUBCLASSEDMSGSOURCEDLG_H__6BC62F08_5B1C_41BA_908D_F5603DB00F6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SubclassedBase.h"

class CSubclassedMsgSourceDlg : public CSubclassedBase  
{
public:
	CSubclassedMsgSourceDlg() : m_hFinishedEvent(NULL) {}

	virtual void AfterInitDialog();
	void SetFinishedEvent(HANDLE hEvent)
	{
		m_hFinishedEvent = hEvent;
	}

	void SetIsSpam(bool bIsSpam)
	{
		m_bIsSpam = bIsSpam;
	}

private:
	HANDLE m_hFinishedEvent;
	bool m_bIsSpam;
};

#endif // !defined(AFX_SUBCLASSEDMSGSOURCEDLG_H__6BC62F08_5B1C_41BA_908D_F5603DB00F6B__INCLUDED_)
