// WinRoot.h: interface for the CWinRoot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WINROOT_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
#define AFX_WINROOT_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////

#define WM_ONEVENT        WM_USER + 100
#define WM_ONACTION       WM_USER + 102
#define WM_ONCTXHELP      WM_USER + 103
#define WM_ONEVENTQUEUE   WM_USER + 104

#define OCM_SYSCOLORCHANGE (OCM__BASE + WM_SYSCOLORCHANGE)

#define COMCTL_VER(_hi, _lo) MAKELONG(_lo, _hi)

struct cLoopParams
{
	cLoopParams() : m_hWndLoopParent(NULL), m_pLoopItem(NULL), m_bMenuShowing(false) {}
	
	HWND                 m_hWndLoopParent;
	CItemBase *          m_pLoopItem;
	bool                 m_bMenuShowing;
};

//////////////////////////////////////////////////////////////////////

class CWinRoot :
	public CWindowImpl<CWinRoot, CWindow, CWinTraits<0, 0> >,
	public CRootItem
{
public:
	DECLARE_WND_CLASS(NULL)
	
	SINK_MAP_DECLARE();
		
	CWinRoot(tDWORD nFlags, CRootSink* pSink = NULL);
	~CWinRoot();

	virtual void         ClearResources(tDWORD nFlags);

	virtual CFontStyle*  CreateFont();
	virtual CImageBase*  CreateImage(eImageType eType, hIO pIo, int nImageSizeX, int nImageNotStretch, int nStyle, CImageBase* pImage);
	virtual CIcon*       CreateIcon(HICON hIcon, bool bRef, CImageBase* pImage = NULL);
	virtual CBackground* CreateBackground(COLORREF nBGColor, COLORREF nFrameColor, int nRadius, int nStyle, CImageBase* pImage = NULL);
	virtual CBorder*     CreateBorder();

	virtual COLORREF     GetSysColor(LPCSTR strColor);
	virtual tDWORD       GetSysVersion(tString &strVersion, tDWORD * nSystemInfo = NULL, tBOOL * bWinPE = NULL);
	virtual HDC          GetDesktopDC();
	virtual HDC          GetTempDC(RECT& rcFrame, RECT* rcUpdate);
	virtual void         CopyTempDC(HDC hTempDC, HDC dc, RECT& rcFrame);
	virtual void         ClearWinResources();
	virtual void         ClipRect(HDC dc, RECT* rc, bool bExclude);
	
	virtual void         SwapMemory();

	virtual bool         OpenShellObject(LPCWSTR strUrl, LPCWSTR strParams = NULL, LPCWSTR strDir = NULL, CItemBase * pParent = NULL, bool bSync = false);
	virtual bool         OpenURL(LPCSTR strURL, CItemBase * pParent);
	virtual bool         OpenMailURL(LPCSTR strURL, CItemBase * pParent);
	virtual bool         GotoFile(LPCSTR strPath, CItemBase * pParent);
	virtual bool         ExecProgram(LPCSTR strPath, LPCSTR strArgs);
	virtual tERROR       SendMail(CSendMailInfo& pInfo, CItemBase* pParent = NULL);
	virtual bool         DoSound(LPCSTR strSoundId);

	virtual bool         ShowCpl(LPCSTR strAppletId);
	virtual bool         ShowContextHelp(tDWORD dwTopicID, CItemBase* pParent);

	virtual bool         BrowseFile(CItemBase* pParent, LPCSTR strTitle, LPCSTR strFileType, tObjPath &strPath, bool bSave = false);
	virtual bool         BrowseUserAccount(CItemBase* pParent, CObjectInfo& pObj);
	virtual bool         GetObjectInfo(CObjectInfo *pObjInfo);
	virtual bool         EnumDrives(tString& strDrive, tDWORD* pCookie);

	virtual void         PostAction(tWORD nActionId, CItemBase * pItem, void * pParam, bool bSync = false);

	virtual void         EnableToolTip(CItemBase* pItem, bool bEnable);
	virtual bool         ShowBalloon(CItemBase* pItem, LPCSTR strTextId, LPCSTR strCaptionId = NULL);
	
//	virtual CPlaceHolder* CreateHolder(tDWORD nId, LPCSTR strType);
	virtual CItemBase*   CreateItem(LPCSTR strId, LPCSTR strType);

protected:
	BEGIN_MSG_MAP(CWinRoot)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ONACTION, OnActionHandler)
		MESSAGE_HANDLER(WM_ONEVENT, OnEventHandler)
		MESSAGE_HANDLER(WM_ONEVENTQUEUE, OnEventQueue)
		MESSAGE_HANDLER(WM_ONCTXHELP, OnContextHelp)
		MESSAGE_HANDLER(WM_SYSCOLORCHANGE, OnSysColorChange)
		MESSAGE_HANDLER(WM_POWERBROADCAST, OnPower)
		MESSAGE_HANDLER(WM_QUERYENDSESSION, OnQueryEndSession)
		MESSAGE_HANDLER(WM_DEVICECHANGE, OnDeviceChange)
		MESSAGE_HANDLER(WM_WTSSESSION_CHANGE, OnSessionChanged)
		MESSAGE_HANDLER(WM_TIMECHANGE, OnTimeChange)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_HOTKEY, OnHotkey)
	END_MSG_MAP()

public:
	void    MessageLoop(CItemBase* pItem);
	bool    ActivateData(bool bActive);
	bool    IsOwnThread();
	bool    SetAnimationTimer(tDWORD nTimeout);
	bool    PreTranslateMessage(MSG& msg);
	bool    InitDrivesList();
	void    RegisterNotifications();
	void    TimerRefresh(tDWORD nTimerSpin);
	void    SendEvent(tSyncEventData * pEvent);
	void    PostProcessEventQueue();

	void    InitThemes();
	void    DeinitThemes(bool bFull = true);
	bool    IsThemed();

// 	void    InitDWM();
// 	void    DeinitDWM();
// 	bool    IsCompositionEnabled();

	bool    DragImage(CItemBase* pItem, CImageBase* pImage, void*& ctx, POINT& pt, enDragType type, tDWORD ipos = 0);
	bool    DrawRect(HDC dc, RECT& rcRect, tDWORD flags, tDWORD state);
	bool    DrawColumnHeader(HDC dc, RECT& rcRect, tDWORD flags);
	bool    DrawCheckBox(HDC dc, RECT& rcRect, bool bChecked, tDWORD nState = 0, bool bFlat = false);
	bool    DrawRadioBox(HDC dc, RECT& rcRect, bool bChecked, tDWORD nState = 0, bool bFlat = false);
	bool    DrawDlgGripper(HDC dc, RECT& rcRect);
	bool    DrawThemeBorder(HDC dc, RECT& rcRect);
	bool    EnableThemeDialogTexture(HWND hWnd, bool bForTab = true, bool bEnable = true);
	bool    SetLayeredWindowAttributes(HWND hWnd, BYTE nAlpha);

	void    RelayTipEvent(const MSG * pMsg);
	void    RelayBalloonEvent(const MSG * pMsg);
	LRESULT ProcessTipEvent(NMHDR * pHdr, CItemBase * pOwner);

	LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEventHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnEventQueue(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnActionHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnContextHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysColorChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDeviceChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSessionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimeChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPower(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnQueryEndSession(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = TRUE;
		return TRUE;
	}
	
	bool    RegisterHotKey(tDWORD nVK);
	LRESULT OnHotkey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	tERROR  SendSrvMgrRequest(LPCWSTR strService, tSrvMgrRequest nRequest);
	tERROR  GetUserInfo(CUserInfo *pUserInfo);
	bool    _GetObjectInfo(CObjectInfo * pObjInfo);
	
public:
	OSVERSIONINFOEX      m_osVer;
	tDWORD               m_nComCtlVer;
	cStrObj              m_bufItemTip;
	
	DWORD                m_nCharSet;
	DWORD                m_nCodePage;

protected:
	void    OnInitLocale();
	void    OnGetSysDefFont(tString& strFont);
	void    OnGetModulePath(tObjPath& strModulePath);

private:
	CDesktopDC           m_hDesktopDC;
	CDcHnd               m_hTmpDC;
	CBitmapHnd           m_hTmpBmp;
	HBITMAP              m_hTmpOldBmp;
	SIZE                 m_szTmpBmp;
	
	DWORD                m_nThreadID;
	ATOM                 m_wndAtom;
	std::vector<tString> m_setDrives;

	HWND                 m_hToolTip;
	HWND                 m_hBalloon;
	HTHEME               m_hThemeButton;
	HTHEME               m_hThemeScroll;
	HTHEME               m_hThemeHeader;
	HTHEME               m_hThemeGlobal;
	unsigned             m_nThemed : 2;

public:
	CLibWrapper          m_Theme;
	HTHEME               (APIENTRY *       _OpenThemeData)(HWND, LPCWSTR);
	HRESULT              (APIENTRY *       _CloseThemeData)(HTHEME);
	BOOL                 (APIENTRY *       _DrawThemeBackground)(HTHEME, HDC, int, int, const RECT *, const RECT *);
	HRESULT              (STDAPICALLTYPE * _EnableThemeDialogTexture)(HWND, DWORD);
	HRESULT              (STDAPICALLTYPE * _DrawThemeParentBackground)(HWND hwnd, HDC hdc, OPTIONAL RECT* prc);

	CLibWrapper          m_User;
	BOOL                 (APIENTRY *       _SetLayeredWindowAttributes)(HWND, COLORREF, BYTE, DWORD);
	BOOL                 (WINAPI *         _GetLastInputInfo)( PLASTINPUTINFO plii);
	
	CLibWrapper          m_WinMM;
	BOOL                 (APIENTRY *       _PlaySound)(LPVOID pszSound, HMODULE hmod, DWORD fdwSound);
	

// 	CLibWrapper			 m_VistaDWMLib;
// 	HRESULT				 (STDAPICALLTYPE * _DwmIsCompositionEnabled)(BOOL* pfEnabled);
// 	HRESULT				 (STDAPICALLTYPE * _DwmEnableBlurBehindWindow)(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
// 	HRESULT				 (STDAPICALLTYPE * _DwmExtendFrameIntoClientArea)(HWND hWnd, const MARGINS* pMarInset);
// 	HRESULT				 EnableBlurBehindWindow(HWND window, bool enable = true, HRGN region = NULL, bool transitionOnMaximized = false);




public:
	LRESULT GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
	LRESULT MsgFilterProc(int nCode, WPARAM wParam, LPARAM lParam);

public:
	HHOOK                m_hMsgHook;
	HHOOK                m_hFilterHook;
	cLoopParams          m_LoopStack;
};

//////////////////////////////////////////////////////////////////////

class cLoopStack : public cLoopParams
{
public:
	cLoopStack(CWinRoot *pRoot, HWND hWndLoopParent, bool bMenuShowing, CItemBase * pLoopItem) : m_pRoot(pRoot)
	{
		*(cLoopParams *)this = m_pRoot->m_LoopStack;
		m_pRoot->m_LoopStack.m_hWndLoopParent = hWndLoopParent;
		m_pRoot->m_LoopStack.m_bMenuShowing = bMenuShowing;
		m_pRoot->m_LoopStack.m_pLoopItem = pLoopItem;
	}
	~cLoopStack()
	{
		m_pRoot->m_LoopStack = *this;
	}

protected:	
	CWinRoot *           m_pRoot;
};

//////////////////////////////////////////////////////////////////////

extern BOOL  g_bWOW64;

extern int DRAW_CHKRAD_SIZE;
extern int DRAW_ICON_MARGIN;
extern int LIST_ICON_SIZE;
extern int LIST_CELL_MARGIN;
extern int LIST_COL_DEFSIZE;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_WINROOT_H__BC57CC5C_0F2D_4921_AE3B_CCCBA11BB4A8__INCLUDED_)
