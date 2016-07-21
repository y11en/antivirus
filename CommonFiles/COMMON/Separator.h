//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_SEPARATOR_H
#define __COMMON_SEPARATOR_H

#include "defs.h"
#include "XPThemeHelper.h"

//******************************************************************************
// class CCSeparator
//******************************************************************************
class COMMONEXPORT CCSeparator : public CStatic
{
public:
	CCSeparator ( int nIndexColor = COLOR_BTNFACE );
     virtual ~CCSeparator ();

	//{{AFX_VIRTUAL(CCSeparator)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CCSeparator)
	afx_msg void OnPaint ();
	afx_msg int OnCreate ( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy ();
     afx_msg BOOL OnEraseBkgnd ( CDC * pDC );
     //}}AFX_MSG
     afx_msg LRESULT OnWmThemeChanged ( WPARAM wParam, LPARAM lParam );
     
	DECLARE_MESSAGE_MAP()

private:
     int m_nIndexColor;
     HTHEME m_hTheme;
};

#endif // __COMMON_SEPARATOR_H

//==============================================================================
