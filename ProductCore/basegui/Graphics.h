// Graphics.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICS_H__C48F2C41_CE67_4D5F_B6A3_8E3D38ECB552__INCLUDED_)
#define AFX_GRAPHICS_H__C48F2C41_CE67_4D5F_B6A3_8E3D38ECB552__INCLUDED_

//////////////////////////////////////////////////////////////////////
// CAlphaBrush
#ifndef __unix__
#include "wingui/xpbrush.h"
#endif

class CAlphaBrush
{
public:
	CAlphaBrush(COLORREF cl) { m_cl = cl; }
	CAlphaBrush(COLORREF cl1, COLORREF cl2, double k) { m_cl = Color(cl1,cl2,k); }

	void Draw(HDC dc, int x1, int y1, int x2, int y2)
	{
		RECT rc = {x1, y1, x2, y2};
		Draw(dc, rc);
	}

	void Draw(HDC dc, RECT& rc);
	
	static COLORREF Color(COLORREF cl1, COLORREF cl2, double k)
	{
		int clR = (int)((1-k)*GetRValue(cl1) + k*GetRValue(cl2));
		int clG = (int)((1-k)*GetGValue(cl1) + k*GetGValue(cl2));
		int clB = (int)((1-k)*GetBValue(cl1) + k*GetBValue(cl2));
		return RGB(clR, clG, clB);
	}

private:
	COLORREF m_cl;
};

//////////////////////////////////////////////////////////////////////

#define HT1  ((nState & ISTATE_HOTLIGHT) ? RGB(255,240,207) : CAlphaBrush::Color(m_clDraw, 0, 0.05))
#define HT2  ((nState & ISTATE_HOTLIGHT) ? RGB(253,216,137) : CAlphaBrush::Color(m_clDraw, 0, 0.15)) 
#define HT3  ((nState & ISTATE_HOTLIGHT) ? RGB(248,178,48)  : CAlphaBrush::Color(m_clDraw, 0, 0.25))
#define HT4  ((nState & ISTATE_HOTLIGHT) ? RGB(229,151,0)	: CAlphaBrush::Color(m_clDraw, 0, 0.35))

//////////////////////////////////////////////////////////////////////
// CBackgroundBase

class CBackgroundBase : public CImageBase
{
public:
	CBackgroundBase() : m_clBackround(-1), m_nRadius(0), m_clFrameColor(-1) {}
	~CBackgroundBase() { Clear(); }

	bool Init(COLORREF nBGColor, COLORREF nFrameColor, int nRadius, int nStyle);

	SIZE     Size()    { SIZE sz = {0,0}; return sz; }
	COLORREF BGColor() { return m_clBackround; }

protected:
	void Clear();
	bool Draw(HDC dc, RECT& rc, CBgContext * pCtx, tDWORD nIdx);

	void DrawEx(HDC dc, COLORREF clFrom, COLORREF clTo, RECT& rcDraw, bool bHor);
	void DrawFR(HDC dc, COLORREF clFrom, COLORREF clTo, RECT& rcDraw);
	void DrawBG(HDC dc, COLORREF clFrom, COLORREF clTo, RECT& rcDraw, bool bHor);

protected:
	static COLORREF OffsetColor(COLORREF cl, int nShift);
	static int      GetRoundShift(int x);

protected:
	CBgContext *        m_pDrawCtx;
	COLORREF            m_clBackround;
	int                 m_nRadius;
	int                 m_nStyle;
#ifndef __unix__	
	CXPBrush			m_XPbrush;
#endif
	COLORREF			m_clFrameColor;
};

//////////////////////////////////////////////////////////////////////
// CMergedImage

class CMergedImage : public CImageBase
{
public:
	bool     Draw(HDC dc, RECT& rc, CBgContext* pCtx = NULL, tDWORD nIdx = 0);
	bool     Merge(CImageBase * pImage, tDWORD nIdx = 0);
	bool     IsTransparent();
	SIZE     Size();
	HICON    IconHnd();

protected:
	static void MergeImagePixel(BYTE * dstRGB, BYTE * dstA, BYTE * srcRGB1, BYTE * srcA1, BYTE * srcRGB2, BYTE * srcA2);

	struct I
	{
		tDWORD nIdx;
		CGuiPtr<CImageBase> p;
	};
	
	cVector<I, cSimpleMover<I> > m_aItems;
	CGuiPtr<CIcon> m_pIcon;
};

//////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_GRAPHICS_H__C48F2C41_CE67_4D5F_B6A3_8E3D38ECB552__INCLUDED_)
