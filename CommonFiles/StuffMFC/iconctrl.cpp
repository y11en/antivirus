// iconctrl.cpp : implementation file
//

#include "stdafx.h"
#include "iconctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconCtrl

CIconCtrl::CIconCtrl()
{
	m_pilIcons = NULL;
}

CIconCtrl::~CIconCtrl()
{
	if (m_pilIcons) delete m_pilIcons;
}


BEGIN_MESSAGE_MAP(CIconCtrl, CStatic)
	//{{AFX_MSG_MAP(CIconCtrl)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconCtrl message handlers

void CIconCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_pilIcons && (m_pilIcons->GetImageCount() > 0))
		m_pilIcons->Draw(&dc, 0, CPoint(0, 0), ILD_TRANSPARENT);
}

BOOL CIconCtrl::SetIcon(HINSTANCE hInstance, LPCTSTR lpIconName, DWORD dwCX, DWORD dwCY, BOOL bRedraw)
{
	BOOL rc = FALSE;

	if (m_pilIcons) delete m_pilIcons;

	if (lpIconName)
	{
		m_pilIcons = new CImageList();
		m_pilIcons->Create(dwCX, dwCY, ILC_COLOR32 | ILC_MASK, 1, 1);
		m_pilIcons->SetBkColor(CLR_NONE); 
		rc = (m_pilIcons->Add(::LoadIcon(hInstance, lpIconName)) != -1)?TRUE:FALSE;

		if (bRedraw) Redraw();
	}

	return rc;
}

BOOL CIconCtrl::SetIcon(HINSTANCE hInstance, int nID, DWORD dwCX, DWORD dwCY, BOOL bRedraw)
{
	BOOL rc = FALSE;

	if (m_pilIcons) delete m_pilIcons;

	m_pilIcons = new CImageList();
	m_pilIcons->Create(dwCX, dwCY, ILC_COLOR32 | ILC_MASK, 1, 1);
	m_pilIcons->SetBkColor(CLR_NONE); 
	rc = (m_pilIcons->Add(::LoadIcon(hInstance, MAKEINTRESOURCE(nID))) != -1)?TRUE:FALSE;

	if (bRedraw) Redraw();

	return rc;
}

void CIconCtrl::Redraw()
{
	Invalidate(TRUE);
	UpdateWindow();
}

