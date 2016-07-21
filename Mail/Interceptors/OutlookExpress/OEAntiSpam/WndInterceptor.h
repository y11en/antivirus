// WndInterceptor.h: interface for the CWndInterceptor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDINTERCEPTOR_H__CCA7132E_2BF2_43A6_BE92_7AC922D99BA0__INCLUDED_)
#define AFX_WNDINTERCEPTOR_H__CCA7132E_2BF2_43A6_BE92_7AC922D99BA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SWMRG.h"
#include <list>
#include "SubclassedCreateDlg.h"
#include "SubclassedMsgPropDlg.h"
#include "SubclassedCopyMoveDlg.h"

namespace
{
	typedef CWinTraits<WS_OVERLAPPEDWINDOW, 0> CInterceptorTraits;
}

class CWndInterceptor // : public CWindowImpl<CWndInterceptor, CWindow, CInterceptorTraits>
{
public:
	CWndInterceptor();
	~CWndInterceptor();

	bool AddCreateFolderTask(LPCTSTR szFolderName);
	void BeforeCreateFolderDialog(HWND hMainWnd);
	void AfterCreateFolderDialog(HWND hMainWnd);
	void SetCreateFolderCounter(int nCounter)
	{
		m_nCreateFolderCounter = nCounter;	// mmm.... whatever
	}

	// for the specified listview item gets its raw body and sends to the AS-core
	bool AddMsgPropTask(bool bIsSpam, HWND hListView, int nItemIndex);
	void BeforeMsgPropDialog(HWND hMainWnd);
	void AfterMsgPropDialog(HWND hMainWnd, HANDLE hStopEvent);
	void ClearMsgPropTaskArray();

	bool AddCopyMoveMsgTask(__int64 dwFolderID);
	void BeforeCopyMoveDialog(HWND hMainWnd);
	void AfterCopyMoveDialog(HWND hMainWnd);
	
	static CWndInterceptor& GetInstance();	
	static void Destroy();

private:
	static CSWMRG	m_InstGuard;
	static CWndInterceptor* m_pInstance;

	CSWMRG	m_accessor;	// to access internal variables

	typedef std::list<tstring> FOLDER_ARRAY;
	FOLDER_ARRAY	m_arrCreateFolderTasks;
	int	m_nCreateFolderCounter;

	typedef std::list<int> MSGPROP_ARRAY;
	MSGPROP_ARRAY	m_arrMsgPropTasks;
	HWND m_hMsgListView;
	bool m_bIsSpam;

	static CSubclassedCreateDlg	m_CreateDlg;
	static CSubclassedMsgPropDlg m_MsgPropDlg;
	static CSubclassedCopyMoveDlg m_CopyMoveDlg;

	static CSubclassedBase* m_pSubclass;
	static bool m_bWindowWasSubclassed;
	static LRESULT CALLBACK LocalCBTProc(int nCode, WPARAM wParam, LPARAM lParam);
	static HHOOK m_hLocalHook;

	static void _ClearLocalHook();
};

#endif // !defined(AFX_WNDINTERCEPTOR_H__CCA7132E_2BF2_43A6_BE92_7AC922D99BA0__INCLUDED_)
