#if !defined(AFX_TRACKLOOKBUTTON_H__24F3B8E1_7579_11D1_BC08_0080C825700A__INCLUDED_)
#define AFX_TRACKLOOKBUTTON_H__24F3B8E1_7579_11D1_BC08_0080C825700A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TrackLookButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// TrackLookButton window

//DrawBitmap flags;
#define	DB_HCENTER	0x0001L
#define DB_VCENTER	0x0002L

class CTrackLookButton : public CButton
{
// Construction
public:
	CTrackLookButton();
	virtual ~CTrackLookButton();

// Attributes
public:
	typedef enum TextAlign {AlignLeft=0,AlignRight,AlignBelow,AlignAbove};
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TrackLookButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL
	void	OnMouseEnter(UINT nFlags,CPoint point);
	void	OnMouseLeave(UINT nFlags,CPoint point);

// Implementation
public:
	BOOL LoadBitmaps(LPCTSTR lpszBitmap,LPCTSTR lpszBitmapFocus=NULL,LPCTSTR lpszBitmapDisabled=NULL);
	void SetTextAlignment(TextAlign nTextAlign);
	void SetBorderAlways( BOOL bSet );
	void SetFocusAsRect( BOOL bSet );
	TextAlign GetTextAlignment() const;
	BOOL LoadBitmaps(UINT nBitmap,UINT nBitmapFocus=0, UINT nBitmapDisabled=0);
	void Redraw();

	// Generated message map functions
	BOOL m_bRaised;
	UINT		m_nBorder;
protected:
	void DrawBitmap(CDC* pDC,CRect rc,UINT nFlags,CBitmap* pBitmap);
	CBitmap m_bitmap;
	CBitmap m_bitmapFocus;
	CBitmap m_bitmapDisabled;
	CBitmap m_bitmapRaised;
	
	TextAlign	m_TextAlign;
	BOOL		m_bDisabled;
	BOOL		m_bHasFocus;
	void Draw3DBorder(CDC* pDC,CRect rc,UINT nOptions);
	
	BOOL		m_bLButtonDown;
	BOOL		m_bLButtonWasDown;
	BOOL		m_bMouseCaptured;
	BOOL    m_bBorderAlways;
	BOOL    m_bFocusAsRect;
	//{{AFX_MSG(TrackLookButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRACKLOOKBUTTON_H__24F3B8E1_7579_11D1_BC08_0080C825700A__INCLUDED_)
