// PNGImage.cpp : implementation file
//

#include "stdafx.h"
#include "PNGImage.h"
#include "../Sign/sign.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPNGImage

CPNGImage::CPNGImage()
{
	m_pImg = NULL;
	m_nWidth = 0;
	m_nHight = 0;
	
	
}

CPNGImage::~CPNGImage()
{
	delete m_pImg;
}


BEGIN_MESSAGE_MAP(CPNGImage, CStatic)
	//{{AFX_MSG_MAP(CPNGImage)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_PRINTCLIENT, OnPrintClient)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPNGImage message handlers

bool CPNGImage::LoadImage(HINSTANCE hInstance, HRSRC hPngRes, COLORREF bkColor )
{
	delete m_pImg;
	m_pImg = new CImagePNG();
	return m_pImg->LoadImage  (hInstance, hPngRes, bkColor);
}

bool CPNGImage::LoadImage ( LPCTSTR szFile, COLORREF bkColor /*= ::GetSysColor ( COLOR_3DFACE )*/ )
{
	delete m_pImg;
	m_pImg = new CImagePNG();
	return m_pImg->LoadImage  (szFile, bkColor);
}

bool CPNGImage::LoadOEMImage ( LPCTSTR szPngFileName, LPCTSTR lpResName, LPCTSTR lpResType, COLORREF bkColor /*= ::GetSysColor ( COLOR_3DFACE )*/)
{
	if (sign_check_file (szPngFileName, 1, 0, 0, 0) != SIGN_OK || !LoadImage  (szPngFileName, bkColor))
	{
		HINSTANCE hResInst = AfxFindResourceHandle(lpResName, lpResType);
		ASSERT (hResInst);
		return LoadImage  (hResInst, ::FindResource(hResInst, lpResName, lpResType), bkColor);
	}

	return true;
}


void CPNGImage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	DrawCtrl (&dc);
}

void CPNGImage::SetBkColor(COLORREF cr)
{
	if (m_pImg)
		m_pImg ->SetBackground  (cr);
	
}

int CPNGImage::GetWidth()
{
	if (m_pImg)
		return m_pImg->GetWidth();
	return 0;
}

int CPNGImage::GetHight()
{
	if (m_pImg)
		return m_pImg->GetHeight();
	return 0;
	
}

void CPNGImage::AdjustImage()
{
	if ( m_pImg )
		SetWindowPos( NULL, 0, 0, GetWidth(), GetHight(), SWP_NOZORDER | SWP_NOMOVE | SWP_FRAMECHANGED );
}

void CPNGImage::DrawCtrl(CDC* pDC)
{
	CRect rect;
	GetClientRect  ( rect );
	//	ScreenToClient(rect);	
	
	if ( m_pImg )
	{
		m_pImg -> Draw  (pDC, rect.left, rect.top, rect.Width  (), rect.Height  ());
	}
}

LRESULT CPNGImage::OnPrintClient (WPARAM wParam, LPARAM lParam)
{
	DrawCtrl( CDC::FromHandle ((HDC)wParam) );
	return 0;
}

