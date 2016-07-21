// Graphics.cpp
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include <ProductCore/ItemRoot.h>
#include "Graphics.h"

extern tBOOL osd_InflateRect(RECT * rc, int x, int y);
extern tBOOL osd_OffsetRect(RECT *, int, int);

//////////////////////////////////////////////////////////////////////
// CBackgroundBase

void CBackgroundBase::Clear()
{
#ifndef __unix__
	m_XPbrush.Clear();
#endif
	m_clBackround = -1;
	m_clFrameColor = -1;
	m_nRadius = 0;
}

bool CBackgroundBase::Init(COLORREF nBGColor, COLORREF nFrameColor, int nRadius, int nStyle)
{
	Clear();
	
	m_clBackround = nBGColor;
	m_nRadius = nRadius;
	m_nStyle = nStyle;
	m_clFrameColor = nFrameColor;

	return true;
}

bool CBackgroundBase::Draw(HDC dc, RECT& rc, CBgContext * pCtx, tDWORD nIdx)
{
	m_pDrawCtx = pCtx;
	
	if( m_clBackround != -1 && nIdx==0)
	{
		COLORREF clBg = (m_pDrawCtx && GUI_ISSTYLE_SIMPLE(m_pRoot) && (m_nStyle & CBackground::System)) ?
			RGB(230,230,230) : m_clBackround;
		
		COLORREF cl = m_nStyle != None ? OffsetColor(clBg, -128) : clBg;
		COLORREF clFrom = (m_nStyle & Top || m_nStyle & Left) ? clBg : cl;
		COLORREF clTo = (m_nStyle & Top || m_nStyle & Left) ? cl : clBg;
		DrawEx(dc, clFrom, clTo, rc, (m_nStyle & (Left|Right)) != 0);
	}
	
	return true;
}

void CBackgroundBase::DrawEx(HDC dc, COLORREF clFrom, COLORREF clTo, RECT& rcDraw, bool bHor)
{
	RECT rcFrame = rcDraw;

	if( m_nRadius || m_clFrameColor!=-1 )
	{
 		DrawFR(dc, clFrom, clTo, rcFrame);

		int nInflate = -1;
		int nInflate_y = -1;
		if (m_clFrameColor==-1)
		{
			nInflate = -1;
			nInflate_y = -1;
		}
		else if (m_nRadius>0)
		{
			if (m_nRadius>5)
			{
				nInflate = -1;
				nInflate_y = -1;
			}
			else
			{
				nInflate = -3;
				nInflate_y = -3;
			}
		}
		
		osd_InflateRect(&rcFrame, nInflate, nInflate_y);
	}

	DrawBG(dc, clFrom, clTo, rcFrame, bHor);
}

void CBackgroundBase::DrawFR(HDC dc, COLORREF clFrom, COLORREF clTo, RECT& rcDraw)
{
	if( m_clFrameColor == -2 )
	{
		RECT& rc = rcDraw;
		/*top*/
		CAlphaBrush(clFrom, RGB(50,50,50), 0.5).Draw(dc, rc.left+1, rc.top, rc.right, rc.top+1);
		/*left*/
		CAlphaBrush(clFrom, RGB(50,50,50), 0.5).Draw(dc, rc.left, rc.top, rc.left+1, rc.bottom);
		/*bottom*/
		CAlphaBrush(clFrom, RGB(200,200,200), 0.5).Draw(dc, rc.left+1, rc.bottom-1, rc.right, rc.bottom);
		/*right*/
		CAlphaBrush(clFrom, RGB(150,150,150), 0.5).Draw(dc, rc.right-1, rc.top, rc.right, rc.bottom-1);
		return;
	}

	COLORREF cl1 = GetPixel(dc, rcDraw.left+1, rcDraw.top+1);
	COLORREF cl2 = GetPixel(dc, rcDraw.right-3, rcDraw.bottom);
	double fAlpha = 0.5;

	if( m_pDrawCtx )
	{
		if( GUI_ISSTYLE_SIMPLE(m_pRoot) )
			cl1 = cl2 = RGB(100,100,100);
		else
		{
			cl1 != CLR_INVALID ? m_pDrawCtx->m_clBgTop = cl1 : cl1 = m_pDrawCtx->m_clBgTop;
			cl2 != CLR_INVALID ? m_pDrawCtx->m_clBgBottom = cl2 : cl2 = m_pDrawCtx->m_clBgBottom;
		}
	}

	if (m_clFrameColor!=-1)
	{
		cl1 = cl2 = m_clFrameColor;
		fAlpha = 0.0;
	}


	if( m_nRadius > 0 )
	{
		if (m_nRadius>5)
		{
			CAlphaBrush brush1(cl1, clTo, fAlpha);
			brush1.Draw(dc, rcDraw.left, rcDraw.top+5, rcDraw.left+1, rcDraw.bottom-5);
			brush1.Draw(dc, rcDraw.left+5, rcDraw.top, rcDraw.right-5, rcDraw.top+1);
			brush1.Draw(dc, rcDraw.right-1, rcDraw.top+5, rcDraw.right, rcDraw.bottom-5);
			
			brush1.Draw(dc, rcDraw.left+1, rcDraw.top+3, rcDraw.left+2, rcDraw.top+5);
			brush1.Draw(dc, rcDraw.left+2, rcDraw.top+2, rcDraw.left+3, rcDraw.top+3);
			brush1.Draw(dc, rcDraw.left+3, rcDraw.top+1, rcDraw.left+5, rcDraw.top+2);
			
			brush1.Draw(dc, rcDraw.right-1, rcDraw.top+3, rcDraw.right-2, rcDraw.top+5);
			brush1.Draw(dc, rcDraw.right-2, rcDraw.top+2, rcDraw.right-3, rcDraw.top+3);
			brush1.Draw(dc, rcDraw.right-3, rcDraw.top+1, rcDraw.right-5, rcDraw.top+2);
			
			CAlphaBrush brush2(cl2, clFrom, fAlpha);
			brush2.Draw(dc, rcDraw.left+5, rcDraw.bottom-1, rcDraw.right-5, rcDraw.bottom);
			
			brush2.Draw(dc, rcDraw.left+1, rcDraw.bottom-3, rcDraw.left+2, rcDraw.bottom-5);
			brush2.Draw(dc, rcDraw.left+2, rcDraw.bottom-2, rcDraw.left+3, rcDraw.bottom-3);
			brush2.Draw(dc, rcDraw.left+3, rcDraw.bottom-1, rcDraw.left+5, rcDraw.bottom-2);
			
			brush2.Draw(dc, rcDraw.right-1, rcDraw.bottom-3, rcDraw.right-2, rcDraw.bottom-5);
			brush2.Draw(dc, rcDraw.right-2, rcDraw.bottom-2, rcDraw.right-3, rcDraw.bottom-3);
			brush2.Draw(dc, rcDraw.right-3, rcDraw.bottom-1, rcDraw.right-5, rcDraw.bottom-2);
		}
		else
		{
#ifndef __unix__
			if(!m_XPbrush.IsInited())
				m_XPbrush.Init(cl2, RGB(255,255,255), true);

			RECT rcFrame = rcDraw;
			::InflateRect(&rcFrame, -1, -1);
			::FrameRect(dc, &rcFrame, m_XPbrush.m_hFrame);

			::InflateRect(&rcFrame, 1, 1);

			m_XPbrush.DrawLeftBorder(dc, rcFrame);
			m_XPbrush.DrawTopBorder(dc, rcFrame);
			m_XPbrush.DrawRightBorder(dc, rcFrame);
			m_XPbrush.DrawBottomBorder(dc, rcFrame);
#endif
		}
	}
	else
	{
		CAlphaBrush brush1(cl1, clTo, fAlpha);
		brush1.Draw(dc, rcDraw);
	}
}

inline void TransponRect(RECT& rect)
{
	swap(rect.top, rect.left);
	swap(rect.bottom, rect.right);
}

void CBackgroundBase::DrawBG(HDC dc, COLORREF clFrom, COLORREF clTo, RECT& rcDraw, bool bHor)
{
	if (bHor) TransponRect(rcDraw);
	RECT rcGrad = rcDraw;

	int nHeight = rcGrad.bottom - rcGrad.top, h;
	int nStep = max(abs(GetRValue(clTo)-GetRValue(clFrom)), abs(GetGValue(clTo)-GetGValue(clFrom)));
	nStep = max(nStep, abs(GetBValue(clTo)-GetBValue(clFrom)));

	nStep = (int)(!nStep ? nHeight : nHeight / nStep);
	if( !nStep ) nStep = 1; 

	rcGrad.bottom = rcGrad.top + nStep;
	for(int i = 0; i < nHeight; i += nStep, osd_OffsetRect(&rcGrad, 0, nStep))
	{
		if (rcGrad.bottom > rcDraw.bottom)
			rcGrad.bottom = rcDraw.bottom;
		RECT rcFill = rcGrad;
		CAlphaBrush brush(clTo, clFrom, i/(double)nHeight);

		int nRaduis = (m_nRadius != -1) ? m_nRadius : 0;
		if( nRaduis )
		{
			if( rcGrad.top - rcDraw.top < nRaduis )
			{
				RECT rcTemp;
				for(rcTemp = rcGrad; rcTemp.top < rcGrad.bottom; rcTemp.top++)
				{
					if( !(h=GetRoundShift(rcTemp.top - rcDraw.top)) ) break;
					rcTemp.left = rcGrad.left + h;
					rcTemp.right = rcGrad.right - h;
					rcTemp.bottom = rcTemp.top + 1;
					if (bHor) TransponRect(rcTemp);
					brush.Draw(dc, rcTemp);
					if (bHor) TransponRect(rcTemp);
				}
				if( rcTemp.top >= rcGrad.bottom )
					continue;
				rcFill.top = rcTemp.top;
			}
			if( rcGrad.bottom - rcDraw.bottom < nRaduis )
			{
				RECT rcTemp;
				for(rcTemp = rcGrad; rcTemp.bottom > rcGrad.top; rcTemp.bottom--)
				{
					if( !(h=GetRoundShift(rcDraw.bottom-rcTemp.bottom)) ) break;
					rcTemp.left = rcGrad.left + h;
					rcTemp.right = rcGrad.right - h;
					rcTemp.top = rcTemp.bottom - 1;
					if (bHor) TransponRect(rcTemp);
					brush.Draw(dc, rcTemp);
					if (bHor) TransponRect(rcTemp);
				}
				if( rcTemp.bottom <= rcGrad.top )
					continue;
				rcFill.bottom = rcTemp.bottom;
			}
		}
		if (bHor) TransponRect(rcFill);
		brush.Draw(dc, rcFill);
	}
	if (bHor) TransponRect(rcDraw);
}

//////////////////////////////////////////////////////////////////////

COLORREF CBackgroundBase::OffsetColor(COLORREF cl, int nShift)
{
	int clRgb[3] = {GetRValue(cl), GetGValue(cl), GetBValue(cl)};
	for(int i = 0; i < countof(clRgb); i++)
	{
		clRgb[i] -= nShift;
		if( clRgb[i] < 0 ) clRgb[i] = 0;
		if( clRgb[i] > 255 ) clRgb[i] = 255;
	}
	
	return RGB(clRgb[0], clRgb[1], clRgb[2]);
}

int CBackgroundBase::GetRoundShift(int x)
{
	switch(x)
	{
		case 0: return 4;
		case 1: return 2;
		case 2: return 1;
		case 3: return 1;
		case 4: return 0;
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////
// CMergedImage

bool CMergedImage::Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx)
{
	if( nIdx )
		return false;

	for(tDWORD i = 0; i < m_aItems.size(); i++)
	{
		I& item = m_aItems[i];
		if( !item.p->Draw(dc, rc, pCtx, item.nIdx) )
			return false;
	}

	return true;
}

bool CMergedImage::Merge(CImageBase * pImage, tDWORD nIdx)
{
	if( !pImage )
		return false;
	
	I& item = m_aItems.push_back();
	item.p = pImage;
	item.nIdx = nIdx;
	pImage->AddRef();
	return true;
}

bool CMergedImage::IsTransparent()
{
	return true;
}

SIZE CMergedImage::Size()
{
	if( !m_aItems.size() )
	{
		SIZE sz = {0, 0};
		return sz;
	}
	
	return m_aItems[0].p->Size();
}

HICON CMergedImage::IconHnd()
{
	if( !m_pIcon )
	{
		m_pIcon = m_aItems.size() ? m_pRoot->CreateIcon(m_aItems[0].p->IconHnd()) : NULL;
		if( !m_pIcon )
			return NULL;

		for(tDWORD i = 1; i < m_aItems.size(); i++)
		{
			I& item = m_aItems[i];
			m_pIcon->Merge(item.p, item.nIdx);
		}
	}

	return m_pIcon->IconHnd();
}

void CMergedImage::MergeImagePixel(BYTE * dstRGB, BYTE * dstA, BYTE * srcRGB1, BYTE * srcA1, BYTE * srcRGB2, BYTE * srcA2)
{
	*dstA = 0xFF - (0xFF - *srcA1)*(0xFF - *srcA2);

	for(tDWORD i = 0; i < 3; i++)
		dstRGB[i] = (BYTE)((((*srcA2)*srcRGB2[i] + (0xFF - *srcA2)*(*srcA1)*srcRGB1[i])/(*dstA))>>8);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
