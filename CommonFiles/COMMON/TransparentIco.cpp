// TransparentIco.cpp : implementation file
//

#include "stdafx.h"
#include "TransparentIco.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransparentIco

CTransparentIco::CTransparentIco() : m_hIcon(NULL)
{
}

CTransparentIco::~CTransparentIco()
{
}


BEGIN_MESSAGE_MAP(CTransparentIco, CStatic)
//{{AFX_MSG_MAP(CTransparentIco)
ON_WM_PAINT()
ON_WM_ERASEBKGND()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransparentIco message handlers

void CTransparentIco::OnPaint() 
{
	
	CPaintDC dc(this); // device context for painting

	
	if ( !GetIcon  ())
		return;
	

/*	ICONINFO icon_info;
	CRect rcIco;
	
	GetWindowRect  ( rcIco );
	ScreenToClient(rcIco);
	
	::GetIconInfo  ( GetIcon  (), &icon_info );
	
	CDC MemDCMask, MemDCBitmap;
	MemDCMask.CreateCompatibleDC(&dc);
	MemDCBitmap.CreateCompatibleDC(&dc);
	
	HGDIOBJ hObjMask = MemDCMask.SelectObject ( icon_info.hbmMask);
	HGDIOBJ hObjBitmap = MemDCBitmap.SelectObject ( icon_info.hbmColor );
	
	ASSERT ( hObjMask != NULL );
	ASSERT ( hObjBitmap != NULL );
	
	
	dc.BitBlt  ( rcIco.left, rcIco.top, rcIco.Width(), rcIco.Height(), &MemDCMask, 0, 0, SRCAND);
	dc.BitBlt  ( rcIco.left, rcIco.top, rcIco.Width(), rcIco.Height(), &MemDCBitmap, 0, 0, SRCPAINT);
	
	MemDCMask.SelectObject(hObjMask);
	MemDCBitmap.SelectObject(hObjBitmap);
	
	::DeleteObject  (icon_info.hbmColor);
	::DeleteObject  (icon_info.hbmMask);
*/
	CRect rc;
	GetClientRect  (rc);
	
	VERIFY(::DrawIconEx  (dc, 0, 0, GetIcon  (), rc.Width  (), rc.Height  (), 0, NULL, DI_NORMAL));
	
}

BOOL CTransparentIco::OnEraseBkgnd(CDC* pDC) 
{
	return CStatic::OnEraseBkgnd(pDC);
}

HICON CTransparentIco::SetIcon(HICON hIcon)
{ 

	m_hIcon = hIcon;
	CWnd* pParentWnd = GetParent();

	HICON hOldIcon = (HICON)::SendMessage(m_hWnd, STM_SETICON, (WPARAM)hIcon, 0L);
	
	if  ( ::IsWindow( pParentWnd ->GetSafeHwnd  () ) )
	{
		CRect rc;
		
		GetWindowRect(rc);
		
		pParentWnd -> ScreenToClient( rc );
		
		pParentWnd -> InvalidateRect( rc, TRUE);
		
		pParentWnd -> UpdateWindow  ();
	}
	
	return hOldIcon;
}

