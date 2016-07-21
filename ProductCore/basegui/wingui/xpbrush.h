#if !defined(_XP_BRUSH_INCLUDED_)
#define _XP_BRUSH_INCLUDED_

/////////////////////////////////////////////////////////////////////////////
// CXPBrush

class CXPBrush
{
public:
	CXPBrush() : m_hBrush(NULL), m_hFrame(NULL){}
	~CXPBrush() { Clear(); }

	void Clear();
	void Init(COLORREF clDraw, COLORREF clBg, bool bEdged=false);
	bool IsInited();

	void DrawTopBorder(HDC dc, RECT& rc);
	void DrawLeftBorder(HDC dc, RECT& rc);
	void DrawBottomBorder(HDC dc, RECT& rc);
	void DrawRightBorder(HDC dc, RECT& rc);
	void DrawGrayedConers(HDC dc, RECT& rc);
	void DrawGradint(HDC dc, RECT& rcDraw, int nState, bool bGradState=false);

public:
	HBRUSH    m_hFrame;
	HBRUSH    m_hBrush;
	COLORREF  m_clDraw;
	COLORREF  m_clBg;
	COLORREF  m_clFrame;
	COLORREF  m_cl0;
	COLORREF  m_cl1;
	COLORREF  m_cl2;
	COLORREF  m_cl3;
	COLORREF  m_cl4;
	COLORREF  m_cl5;
	COLORREF  m_cl6;
	COLORREF  m_cl7;
};


#endif //_XP_BRUSH_INCLUDED_