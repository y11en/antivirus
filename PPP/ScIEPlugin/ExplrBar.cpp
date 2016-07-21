/**************************************************************************

   File:          ExplorerBar.cpp
   
   Description:   Implements CExplorerBar

**************************************************************************/

/**************************************************************************
   #include statements
**************************************************************************/

#include "ExplrBar.h"
#include "Guid.h"
#include <comdef.h>
#include "resource.h"

hROOT g_root = NULL;

extern HINSTANCE g_hInst;

#define MC_STATISTIC_TIMER_ID                        100
#define MC_STATISTIC_TIMER_PERIOD	                 333


/**************************************************************************

   CExplorerBar::CExplorerBar()

**************************************************************************/

CExplorerBar::CExplorerBar() :
	m_pRemote(g_hInst, PR_LOADER_FLAG_FORCE_PLUGIN_SEARCH|PR_FLAG_LOW_INTEGRITY_PROCESS),
	m_pSCProxy(m_pRemote, "SCTask")
{
	m_pRemote.LoadProxyStub("pxstub.ppl");

	m_pSite = NULL;

	m_hWnd = NULL;
	m_hwndParent = NULL;

	m_bFocus = FALSE;

	m_dwViewMode = 0;
	m_dwBandID = 0;

	m_ObjRefCount = 1;
	g_DllRefCount++;

	m_pWebBrowser = NULL;
}

/**************************************************************************

   CExplorerBar::~CExplorerBar()

**************************************************************************/

CExplorerBar::~CExplorerBar()
{
	//this should have been freed in a call to SetSite(NULL), but just to be safe
	if(m_pSite)
	{
		m_pSite->Release();
		m_pSite = NULL;
	}

	if(m_pWebBrowser)
	{
		m_pWebBrowser->Release();
		m_pWebBrowser = NULL;
	}

	g_DllRefCount--;
}

///////////////////////////////////////////////////////////////////////////
//
// IUnknown Implementation
//

/**************************************************************************

   CExplorerBar::QueryInterface

**************************************************************************/

STDMETHODIMP CExplorerBar::QueryInterface(REFIID riid, LPVOID *ppReturn)
{
	*ppReturn = NULL;

	//IUnknown
	if(IsEqualIID(riid, IID_IUnknown))
	{
		*ppReturn = this;
	}

	//IOleWindow
	else if(IsEqualIID(riid, IID_IOleWindow))
	{
		*ppReturn = (IOleWindow*)this;
	}

	//IDockingWindow
	else if(IsEqualIID(riid, IID_IDockingWindow))
	{
		*ppReturn = (IDockingWindow*)this;
	}   

	//IInputObject
	else if(IsEqualIID(riid, IID_IInputObject))
	{
		*ppReturn = (IInputObject*)this;
	}   

	//IObjectWithSite
	else if(IsEqualIID(riid, IID_IObjectWithSite))
	{
		*ppReturn = (IObjectWithSite*)this;
	}   

	//IDeskBand
	else if(IsEqualIID(riid, IID_IDeskBand))
	{
		*ppReturn = (IDeskBand*)this;
	}   

	//IPersist
	else if(IsEqualIID(riid, IID_IPersist))
	{
		*ppReturn = (IPersist*)this;
	}   

	//IPersistStream
	else if(IsEqualIID(riid, IID_IPersistStream))
	{
		*ppReturn = (IPersistStream*)this;
	}   

	if(*ppReturn)
	{
		(*(LPUNKNOWN*)ppReturn)->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}                                             

/**************************************************************************

   CExplorerBar::AddRef

**************************************************************************/

STDMETHODIMP_(DWORD) CExplorerBar::AddRef()
{
	return ++m_ObjRefCount;
}


/**************************************************************************

   CExplorerBar::Release

**************************************************************************/

STDMETHODIMP_(DWORD) CExplorerBar::Release()
{
	if(--m_ObjRefCount == 0)
	{
		delete this;
		return 0;
	}
	return m_ObjRefCount;
}

///////////////////////////////////////////////////////////////////////////
//
// IOleWindow Implementation
//

/**************************************************************************

   CExplorerBar::GetWindow()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::GetWindow(HWND *phWnd)
{
	*phWnd = m_hWnd;

	return S_OK;
}

/**************************************************************************

   CExplorerBar::ContextSensitiveHelp()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////
//
// IDockingWindow Implementation
//

/**************************************************************************

   CExplorerBar::ShowDW()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::ShowDW(BOOL fShow)
{
	if(m_hWnd)
		ShowWindow(m_hWnd, fShow ? SW_SHOW : SW_HIDE);
	
	return S_OK;
}

/**************************************************************************

   CExplorerBar::CloseDW()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::CloseDW(DWORD dwReserved)
{
	ShowDW(FALSE);

	if(IsWindow(m_hWnd))
		DestroyWindow(m_hWnd);

	m_hWnd = NULL;
   
	return S_OK;
}

/**************************************************************************

   CExplorerBar::ResizeBorderDW()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::ResizeBorderDW(   LPCRECT prcBorder, 
                                          IUnknown* punkSite, 
                                          BOOL fReserved)
{
	//This method is never called for Band Objects.
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////
//
// IInputObject Implementation
//

/**************************************************************************

   CExplorerBar::UIActivateIO()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::UIActivateIO(BOOL fActivate, LPMSG pMsg)
{
	if(fActivate)
		SetFocus(m_hWnd);

	return S_OK;
}

/**************************************************************************

   CExplorerBar::HasFocusIO()
   
   If this window or one of its decendants has the focus, return S_OK. Return 
   S_FALSE if we don't have the focus.

**************************************************************************/

STDMETHODIMP CExplorerBar::HasFocusIO(void)
{
	if(m_bFocus)
		return S_OK;

	return S_FALSE;
}

/**************************************************************************

   CExplorerBar::TranslateAcceleratorIO()
   
   If the accelerator is translated, return S_OK or S_FALSE otherwise.

**************************************************************************/

STDMETHODIMP CExplorerBar::TranslateAcceleratorIO(LPMSG pMsg)
{
	return S_FALSE;
}

///////////////////////////////////////////////////////////////////////////
//
// IObjectWithSite implementations
//

/**************************************************************************

   CExplorerBar::SetSite()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::SetSite(IUnknown* punkSite)
{
	//If a site is being held, release it.
	if(m_pSite)
	{
		m_pSite->Release();
		m_pSite = NULL;
	}

	//If punkSite is not NULL, a new site is being set.
	if(punkSite)
	{
		//Get the parent window.
		IOleWindow  *pOleWindow;

		m_hwndParent = NULL;

		if(SUCCEEDED(punkSite->QueryInterface(IID_IOleWindow, (LPVOID*)&pOleWindow)))
		{
			pOleWindow->GetWindow(&m_hwndParent);
			pOleWindow->Release();
		}
		
		if(!m_hwndParent)
			return E_FAIL;

        IServiceProviderPtr pServiceProvider = punkSite;
        if(FAILED(pServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&m_pWebBrowser)))
			return E_FAIL;


		if(!RegisterAndCreateWindow())
			return E_FAIL;

		//Get and keep the IInputObjectSite pointer.
		if(SUCCEEDED(punkSite->QueryInterface(IID_IInputObjectSite, (LPVOID*)&m_pSite)))
		{
			return S_OK;
		}

		return E_FAIL;
	}

	return S_OK;
}

/**************************************************************************

   CExplorerBar::GetSite()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::GetSite(REFIID riid, LPVOID *ppvReturn)
{
	*ppvReturn = NULL;

	if(m_pSite)
		return m_pSite->QueryInterface(riid, ppvReturn);

	return E_FAIL;
}

///////////////////////////////////////////////////////////////////////////
//
// IDeskBand implementation
//

/**************************************************************************

   CExplorerBar::GetBandInfo()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::GetBandInfo(DWORD dwBandID, DWORD dwViewMode, DESKBANDINFO* pdbi)
{
	if(pdbi)
	{
		m_dwBandID = dwBandID;
		m_dwViewMode = dwViewMode;

		if(pdbi->dwMask & DBIM_MINSIZE)
		{
			pdbi->ptMinSize.x = MIN_SIZE_X;
			pdbi->ptMinSize.y = MIN_SIZE_Y;
		}

		if(pdbi->dwMask & DBIM_MAXSIZE)
		{
			pdbi->ptMaxSize.x = -1;
			pdbi->ptMaxSize.y = -1;
		}

		if(pdbi->dwMask & DBIM_INTEGRAL)
		{
			pdbi->ptIntegral.x = 1;
			pdbi->ptIntegral.y = 1;
		}

		if(pdbi->dwMask & DBIM_ACTUAL)
		{
			pdbi->ptActual.x = 0;
			pdbi->ptActual.y = 0;
		}

		if(pdbi->dwMask & DBIM_TITLE)
		{
			lstrcpyW(pdbi->wszTitle, m_Res.m_wsCaption.c_str());
		}

		if(pdbi->dwMask & DBIM_MODEFLAGS)
		{
			pdbi->dwModeFlags = DBIMF_VARIABLEHEIGHT;
		}

		if(pdbi->dwMask & DBIM_BKCOLOR)
		{
			//Use the default background color by removing this flag.
			pdbi->dwMask &= ~DBIM_BKCOLOR;
		}

		return S_OK;
	}

	return E_INVALIDARG;
}

///////////////////////////////////////////////////////////////////////////
//
// IPersistStream implementations
// 
// This is only supported to allow the desk band to be dropped on the 
// desktop and to prevent multiple instances of the desk band from showing 
// up in the context menu. This desk band doesn't actually persist any data.
//

/**************************************************************************

   CExplorerBar::GetClassID()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::GetClassID(LPCLSID pClassID)
{
	*pClassID = CLSID_SampleExplorerBar;

	return S_OK;
}

/**************************************************************************

   CExplorerBar::IsDirty()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::IsDirty(void)
{
	return S_FALSE;
}

/**************************************************************************

   CExplorerBar::Load()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::Load(LPSTREAM pStream)
{
	return S_OK;
}

/**************************************************************************

   CExplorerBar::Save()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::Save(LPSTREAM pStream, BOOL fClearDirty)
{
	return S_OK;
}

/**************************************************************************

   CExplorerBar::GetSizeMax()
   
**************************************************************************/

STDMETHODIMP CExplorerBar::GetSizeMax(ULARGE_INTEGER *pul)
{
	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////
//
// private method implementations
//

/**************************************************************************

   CExplorerBar::WndProc() / CExplorerBar::WndProcLink()
   
**************************************************************************/

LRESULT CALLBACK CExplorerBar::WndProcLink(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	CExplorerBar *pThis = (CExplorerBar*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if(uMessage == WM_NCCREATE)
	{
		LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
		pThis = (CExplorerBar*)(lpcs->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

		//set the window handle
		pThis->m_hWnd = hWnd;
	}
	if(pThis)
		return pThis->WndProc(hWnd, uMessage, wParam, lParam);
	return 0;
}

LRESULT CExplorerBar::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch (uMessage)
	{
	case WM_CREATE:
		return OnCreate(wParam, lParam);

	case WM_PAINT:
		return OnPaint();

	case WM_COMMAND:
		return OnCommand(wParam, lParam);

	case WM_SETFOCUS:
		return OnSetFocus();

	case WM_KILLFOCUS:
		return OnKillFocus();

	case WM_TIMER:
		return OnTimer(wParam, lParam);
	
	case WM_CTLCOLORSTATIC:
		return (LRESULT)m_hBgBrush;//(LRESULT)(HBRUSH)GetStockObject(NULL_BRUSH);
	}

	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

/**************************************************************************

   CExplorerBar::OnPaint()
   
**************************************************************************/

LRESULT CExplorerBar::OnPaint(void)
{
	PAINTSTRUCT ps;
	RECT        rc;

	BeginPaint(m_hWnd, &ps);

	GetClientRect(m_hWnd, &rc);

	// Draw bitmap
	POINT pt;
	pt.x = rc.right - m_szBmp.cx;
	pt.y = rc.bottom - m_szBmp.cy;

	HDC hBmpDC = ::CreateCompatibleDC(ps.hdc);
	if(hBmpDC)
	{
		HGDIOBJ hObj = ::SelectObject(hBmpDC, m_hBmp);
		if(hObj)
		{
			::BitBlt(ps.hdc, pt.x, pt.y, m_szBmp.cx, m_szBmp.cy, hBmpDC, 0, 0, SRCCOPY);
			::SelectObject(hBmpDC, hObj);
		}
		::DeleteDC(hBmpDC);
	}

	EndPaint(m_hWnd, &ps);

	return 0;
}

/**************************************************************************

   CExplorerBar::OnCommand()
   
**************************************************************************/

LRESULT CExplorerBar::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(m_StaticLink == (HWND)lParam && HIWORD(wParam) == BN_CLICKED)
	{
		CPRRemoteProxy<cTaskManager *> pTM(m_pRemote, "TaskManager");
		if(pTM.Attach())
		{
			tERROR err = pTM->AskAction(TASKID_TM_ITSELF, cProfileAction::SHOW_REPORT, &cProfileAction(AVP_PROFILE_WEBMONITOR));
		}
	}
	
	return 0;
}

/**************************************************************************

   CExplorerBar::FocusChange()
   
**************************************************************************/

void CExplorerBar::FocusChange(BOOL bFocus)
{
	m_bFocus = bFocus;

	//inform the input object site that the focus has changed
	if(m_pSite)
	{
		m_pSite->OnFocusChangeIS((IDockingWindow*)this, bFocus);
	}
}

/**************************************************************************

   CExplorerBar::OnSetFocus()
   
**************************************************************************/

LRESULT CExplorerBar::OnSetFocus(void)
{
	FocusChange(TRUE);

	return 0;
}

/**************************************************************************

   CExplorerBar::OnKillFocus()
   
**************************************************************************/

LRESULT CExplorerBar::OnKillFocus(void)
{
	FocusChange(FALSE);

	return 0;
}

LRESULT CExplorerBar::OnCreate(WPARAM wParam, LPARAM lParam)
{
	m_Res.Load(g_hInst, g_root);

	LOGFONT lf = {0,};
	lf.lfHeight = -12;
	lf.lfWeight = 400;
	
	lf.lfCharSet = 204;
	if( m_Res.m_nLocale )
	{
		CHAR szLInfo[10];
		int nCodePage = 0;
		
		if( ::GetLocaleInfo(m_Res.m_nLocale, LOCALE_IDEFAULTANSICODEPAGE, szLInfo, sizeof(szLInfo)) )
		{
			nCodePage = atoi(szLInfo);

			CHARSETINFO ci;
			if( ::TranslateCharsetInfo((DWORD *)nCodePage, &ci, TCI_SRCCODEPAGE) )
				lf.lfCharSet = ci.ciCharset;
		}
	}

	_tcscpy(lf.lfFaceName, _T("Tahoma"));
	m_Font.CreateFontIndirect(&lf);
	
	{
		RECT rt = {10, 10, 150, 120};
		m_StaticText.Create(m_hWnd, rt, TEXT(""), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		m_StaticText.SetFont(m_Font);
	}
	{
		RECT rt = {10, 150, 150, 180};
		m_StaticLink.Create(m_hWnd, rt, m_Res.m_sViewReport.c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		m_StaticLink.m_dwExtendedStyle |= HLINK_COMMANDBUTTON;
		m_StaticLink.m_hFont = m_StaticLink.m_hFontNormal = m_Font;
	}

	m_hBmp = (HBITMAP)LoadImage(g_hInst, MAKEINTRESOURCE(IDB_WATERMARK), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
	ATLASSERT(m_hBmp != NULL);
	
	BITMAP bm;
	::GetObject(m_hBmp, sizeof(BITMAP), &bm);
	m_szBmp.cx = bm.bmWidth;
	m_szBmp.cy = bm.bmHeight;


	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR];

	::GetModuleFileName(g_hInst, m_szPath, ARRAYSIZE(m_szPath));
	_tsplitpath(m_szPath, szDrive, szDir, NULL, NULL);
	_tmakepath (m_szPath, szDrive, szDir, NULL, NULL);


	SetTimer(m_hWnd, MC_STATISTIC_TIMER_ID, MC_STATISTIC_TIMER_PERIOD, NULL);
	OnTimer((WPARAM)MC_STATISTIC_TIMER_ID, (LPARAM)NULL);

	return 0;
}

LRESULT CExplorerBar::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if(wParam == MC_STATISTIC_TIMER_ID)
	{
		BOOL bProductLoaded = FALSE;

		if(m_pSCProxy.Refresh())
		{
			bProductLoaded = TRUE;
			
			cProtectionStatistics Stat;
			tERROR err = m_pSCProxy->GetStatistics(&Stat);

			TCHAR szText[512];
			_sntprintf(szText, countof(szText), m_Res.m_sStatistics.c_str(), Stat.m_nNumScaned, Stat.m_nNumDetected);
			m_StaticText.SetWindowText(szText);
		}
		else
		{
			m_StaticText.SetWindowText(m_Res.m_sProductNotLoaded.c_str());
		}
		
		m_StaticLink.EnableWindow(bProductLoaded);
	}
	return 0;
}


/**************************************************************************

   CExplorerBar::RegisterAndCreateWindow()
   
**************************************************************************/

BOOL CExplorerBar::RegisterAndCreateWindow(void)
{
	//If the window doesn't exist yet, create it now.
	if(!m_hWnd)
	{
		//Can't create a child window without a parent.
		if(!m_hwndParent)
		{
			return FALSE;
		}

		COLORREF clr = RGB(255, 255, 255);//GetSysColor(COLOR_WINDOW);
		m_hBgBrush = CreateSolidBrush(clr);

		//If the window class has not been registered, then do so.
		WNDCLASS wc;
		if(!GetClassInfo(g_hInst, EB_CLASS_NAME, &wc))
		{
			ZeroMemory(&wc, sizeof(wc));
			wc.style          = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS;
			wc.lpfnWndProc    = (WNDPROC)WndProcLink;
			wc.cbClsExtra     = 0;
			wc.cbWndExtra     = 0;
			wc.hInstance      = g_hInst;
			wc.hIcon          = NULL;
			wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground  = m_hBgBrush;
			wc.lpszMenuName   = NULL;
			wc.lpszClassName  = EB_CLASS_NAME;

			if(!RegisterClass(&wc))
			{
				//If RegisterClass fails, CreateWindow below will fail.
			}
		}

		RECT  rc;

		GetClientRect(m_hwndParent, &rc);

		//Create the window. The WndProc will set m_hWnd.
		CreateWindowEx(0,
			EB_CLASS_NAME,
			NULL,
			WS_CHILD | WS_CLIPSIBLINGS /*| WS_BORDER*/,
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			m_hwndParent,
			NULL,
			g_hInst,
			(LPVOID)this);

	}

	return (NULL != m_hWnd);
}