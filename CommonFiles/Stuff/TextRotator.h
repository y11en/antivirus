////////////////////////////////////////////////////////////////////
//
//  TextRotator.h
//  TextRotator class definition
//  Utility class for text rotation (0-90-180-270 degrees only)
//  Project
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __TEXTROTATOR_H__
#define __TEXTROTATOR_H__

#include <Stuff\PathStr.h>
#include <Stuff\PArray.h>

/////////////////////////////////////////////////////////////////////////////
// class CTextRotator

// ---
class CTextRotator {
protected :
  HDC      m_hSourceDC;
  HDC      m_hRotatedMemDC;
  RECT     m_rectRotated;
  UINT     m_uiHorzAlignment;
  UINT     m_uiVertAlignment;
  UINT     m_uiAngle;
  BOOL     m_bDisabled;
  BOOL     m_bDrawBevelLine;
  CPathStr m_strText;
  COLORREF m_clrBackground;
  COLORREF m_clrText;
	HGDIOBJ  m_hSourceBmp;
	HFONT    m_hFont;

  void     Clear();
  SIZE     GetLengthlyTextSize(HDC hDC, CPArray<TCHAR> *parrayText);
	void		 RotateBitmap( UINT degree, COLORREF clrBack, int Height, int Width, HDC sourceDC, HDC destDC );

public :
	enum {
		HORZ_LEFT			= 0x01,
		HORZ_CENTER		= 0x02,
		HORZ_RIGHT		= 0x04,
		VERT_TOP			= 0x08,
		VERT_CENTER		= 0x10,
		VERT_BOTTOM		= 0x20
	};

	CTextRotator(HDC hSourceDC, LPCTSTR lpszText = _T(""), UINT uiHorzAlignment = HORZ_LEFT, UINT uiVertAlignment = VERT_CENTER);
	virtual ~CTextRotator();

	BOOL BitBltText(int X, int Y);
	BOOL BitBltText(RECT rectSource);
	void GetRotatedRect(RECT *pstRect);
	BOOL RotateText(UINT uiAngle = 0);

	HDC    GetRotatedDC()   { return m_hRotatedMemDC; }
	RECT   GetRotatedRect() { return m_rectRotated; }

	void SetFont( HFONT hFont );
	void SetDrawBevelLine(BOOL bDraw)                     { m_bDrawBevelLine = bDraw; }
	void SetBackgroundColor(COLORREF clr)                 { m_clrBackground = clr; }
	void SetDisabledText(BOOL bDisable)                   { m_bDisabled = bDisable; }
	void SetHorzAlignment(UINT uiHorzAlignment=HORZ_LEFT) { m_uiHorzAlignment = uiHorzAlignment; }
	void SetText(LPCTSTR lpszText)                        { m_strText = lpszText; }
	void SetTextColor(COLORREF clr)                       { m_clrText = clr; }
	void SetVertAlignment(UINT uiVertAlignment=VERT_TOP)  { m_uiVertAlignment = uiVertAlignment; }
};

#endif // __TEXTROTATOR_H__