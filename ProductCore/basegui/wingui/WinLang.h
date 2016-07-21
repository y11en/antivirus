// WinLang.h: interface for the CWinLang class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINLANG_H__F323948F_EA07_43C6_97F5_19DCAA343A74__INCLUDED_)
#define AFX_WINLANG_H__F323948F_EA07_43C6_97F5_19DCAA343A74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

class CPointerCleaner : std::vector<void *>
{
	CRITICAL_SECTION m_cs;
public:
	CPointerCleaner()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CPointerCleaner()
	{
		EnterCriticalSection(&m_cs);
		for(iterator it = begin(); it != end(); ++it)
			free(*it);
		LeaveCriticalSection(&m_cs);
		DeleteCriticalSection(&m_cs);
	}
	void AddPointer(void *p)
	{
		EnterCriticalSection(&m_cs);
		push_back(p);
		LeaveCriticalSection(&m_cs);
	}
};

class CWindowL : public CWindow
{
public:
	static void      Init(tDWORD nOsVer);
	static HWND      CreateWnd(DWORD dwExStyle, LPVOID lpClassName, DWORD dwStyle, RECT& rcPos, HWND hWndParent, DWORD nID = 0, LPVOID lpParam = NULL);
	static BOOL      BrowseFile(CRootItem * pRoot, OPENFILENAMEA * ofn, tObjPath& sPath, bool bSave);
	static int       DrawText(CRootItem * pRoot, HDC dc, LPCSTR strText, RECT& rc, DWORD flags);
	static void      ListView_GetItemTextL(CRootItem * pRoot, HWND hWnd, DWORD item, DWORD col, cStrObj& strText);

public:
	CWindowL(HWND hWnd = NULL) : CWindow(hWnd) { }

	HTREEITEM TreeView_InsertItemL(CRootItem * pRoot, LPTVINSERTSTRUCT ts);
	BOOL      TreeView_SetItemL(CRootItem * pRoot, LPTVITEM ti);
	BOOL      ListView_SetColumnL(CRootItem * pRoot, tSIZE_T i, LPLVCOLUMN pcol);
	DWORD     ListView_InsertColumnL(CRootItem * pRoot, tSIZE_T i, LPLVCOLUMN pcol);

	ATOM      RegisterWindowA(CWndClassInfo* p, bool bEnableReRegister = false);
	ATOM      RegisterWindowW(CWndClassInfo* p, bool bEnableReRegister = false);

	BOOL      SetWindowTextW(CRootItem * pRoot, LPCWSTR strText);
	void      GetWindowTextW(CRootItem * pRoot, cStrObj& strText) const;

public:
	static bool  m_bAnsi;
	static bool  m_bAnsiOperate;
	static CPointerCleaner m_ClassNamesHolder;
};

#define ScaledSize(sz) (MulDiv(sz, m_pRoot->m_dwScaleY, 96))
#define AdjustSize(sz) sz = ScaledSize(sz)

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_WINLANG_H__F323948F_EA07_43C6_97F5_19DCAA343A74__INCLUDED_)
