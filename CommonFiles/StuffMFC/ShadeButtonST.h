#ifndef _SHADEBUTTONST_H_
#define _SHADEBUTTONST_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BtnST.h"
#include "xDib.h"

class CShadeButtonST : public CButtonST  
{
public:
	CShadeButtonST();
	virtual ~CShadeButtonST();

	enum	{	SHS_NOISE = 0,
				SHS_DIAGSHADE,
				SHS_HSHADE,
				SHS_VSHADE,
				SHS_HBUMP,
				SHS_VBUMP,
				SHS_SOFTBUMP,
				SHS_HARDBUMP,
				SHS_METAL	};

	void SetShade(UINT shadeID=0,BYTE granularity=8,BYTE highlight=10,BYTE coloring=0,COLORREF color=0);
	void SetShadeColor(COLORREF crhicr, COLORREF crmidcr, COLORREF crlocr);

private:
	CxDib m_dNormal,m_dDown,m_dDisabled,m_dOver,m_dh,m_dv;
	COLORREF hicr, midcr, locr;
	
protected:
	virtual DWORD OnDrawBorder(CDC* pDC, LPCRECT pRect);
	virtual DWORD OnDrawBackground(CDC* pDC, LPCRECT pRect);
};

#endif
