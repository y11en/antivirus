// Localizer.h: interface for the CLocalizer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOCALIZER_H__7C5D4F81_47B4_49CC_9883_35E488975581__INCLUDED_)
#define AFX_LOCALIZER_H__7C5D4F81_47B4_49CC_9883_35E488975581__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <string>

typedef std::basic_string<TCHAR> tstring;


class CLocalizer  
{
public:
	CLocalizer();
	virtual ~CLocalizer();

	void     Init(HINSTANCE hThisDll, HINSTANCE hLocalizeDll);

	int      LoadString(DWORD nStrID, LPTSTR pszString, DWORD dwStringSize);
	tstring  LoadString(DWORD nStrID);
	int      MessageBox(HWND hWnd, DWORD nStrIDText, DWORD nStrIDCaption, UINT nType = MB_OK);


	void     LocalizeDlg(HWND hWnd);
protected:
	BOOL     EnumChildProc(HWND hWnd);
	static BOOL __stdcall EnumChildProcLink(HWND hWnd, LPARAM lParam);

protected:
	HINSTANCE m_hThisDll;
	HINSTANCE m_hLocalizeDll;
};


#endif // !defined(AFX_LOCALIZER_H__7C5D4F81_47B4_49CC_9883_35E488975581__INCLUDED_)
