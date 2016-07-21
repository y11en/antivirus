#if !defined(AFX_PNGIMAGE_H__11B41F2F_52FA_4098_B77E_A6EFB46C8F16__INCLUDED_)
#define AFX_PNGIMAGE_H__11B41F2F_52FA_4098_B77E_A6EFB46C8F16__INCLUDED_

#include "defs.h"
#include "cimage\imapng.h"

//******************************************************************************
// 
//******************************************************************************
class COMMONEXPORT CPNGImage : public CStatic
{
public:
	CPNGImage ();
    virtual ~CPNGImage ();
    
    bool LoadImage ( HINSTANCE hInstance, HRSRC hPngRes, COLORREF bkColor = ::GetSysColor ( COLOR_3DFACE ) );
	bool LoadImage ( LPCTSTR szFile, COLORREF bkColor = ::GetSysColor ( COLOR_3DFACE ) );
	bool LoadOEMImage ( LPCTSTR szPngFileName, LPCTSTR lpResName, LPCTSTR lpResType, COLORREF bkColor = ::GetSysColor ( COLOR_3DFACE ));
    int GetWidth ();
    int GetHight ();
    void AdjustImage ();
    
    void SetBkColor ( COLORREF cr );

	void DrawCtrl(CDC* pDC);

	
	//{{AFX_VIRTUAL(CPNGImage)
	//}}AFX_VIRTUAL
	
protected:
	//{{AFX_MSG(CPNGImage)
	afx_msg void OnPaint ();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	
	LRESULT OnPrintClient (WPARAM wParam, LPARAM lParam);

private:
	CImagePNG * m_pImg;
	int		m_nWidth;
	int		m_nHight;
};

#endif // !defined(AFX_PNGIMAGE_H__11B41F2F_52FA_4098_B77E_A6EFB46C8F16__INCLUDED_)
