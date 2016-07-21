// CG: This file was added by the Splash Screen component.

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : header file
//

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

class CSplashWnd : public CWnd
{
// Attributes:
public:
	CBitmap m_bitmap;

	CSplashWnd(UINT nBitmapID, HINSTANCE hResourceHandle);

// Operations
public:
	static void EnableSplashScreen(BOOL bEnable);
	static void ShowSplashScreen(CWnd* pParentWnd, UINT nBitmapID, HINSTANCE hResourceHandle);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	static BOOL c_bShowSplashWnd;
	UINT m_nBitmapID;
	HINSTANCE m_hResourceHandle;

	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();

// Generated message map functions
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
