// SubclassedBase.h: interface for the CSubclassedBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCLASSEDBASE_H__3EBBBA82_2DB2_49C7_9D0F_80F0402D8124__INCLUDED_)
#define AFX_SUBCLASSEDBASE_H__3EBBBA82_2DB2_49C7_9D0F_80F0402D8124__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace
{
	const UINT WM_AFTER_INITDIALOG = WM_APP + 10;
}

class CSubclassedBase : public CWindowImpl<CSubclassedBase, CWindow>
{
public:
	virtual ~CSubclassedBase() {}
	virtual void AfterInitDialog() {}	// redefine it for derived classes
	virtual void BeforeDestroy() {}
	virtual void BeforeTimer(UINT nTimerID) {}

	DECLARE_WND_CLASS(TEXT("7BAAD23E-FOOD-4f9c-B7A3-A2898BB11532"))
	
	BEGIN_MSG_MAP(CSubclassedBase)
		MESSAGE_HANDLER(WM_NCPAINT, OnNcPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
//		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_AFTER_INITDIALOG, OnAfterInitDialog)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
	END_MSG_MAP()

private:
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnAfterInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		AfterInitDialog();
		return S_OK;
	}

	LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

#endif // !defined(AFX_SUBCLASSEDBASE_H__3EBBBA82_2DB2_49C7_9D0F_80F0402D8124__INCLUDED_)
