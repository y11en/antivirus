// Localizer.cpp: implementation of the CLocalizer class.
//
//////////////////////////////////////////////////////////////////////

#include "Localizer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalizer::CLocalizer():m_hLocalizeDll(NULL)
{
}

CLocalizer::~CLocalizer()
{
}

void CLocalizer::Init(HINSTANCE hThisDll, HINSTANCE hLocalizeDll)
{
	m_hLocalizeDll = hLocalizeDll;
	m_hThisDll = hThisDll;
}

int CLocalizer::LoadString(DWORD nStrID, LPTSTR pszString, DWORD dwStringSize)
{
	if(!pszString)
		return 0;
	
	*pszString = 0;
	if ( m_hLocalizeDll )
		return ::LoadString(m_hLocalizeDll, nStrID, pszString, dwStringSize);
	else
		return ::LoadString(m_hThisDll, nStrID, pszString, dwStringSize);
}

tstring CLocalizer::LoadString(DWORD nStrID)
{
	TCHAR str[512];
	*str = 0;
	
	LoadString(nStrID, str, 512);

	return str;

}

int CLocalizer::MessageBox(HWND hWnd, DWORD nStrIDText, DWORD nStrIDCaption, UINT nType)
{
	TCHAR sCaption[512] = "";
	TCHAR sText[512] = "";
	
	LoadString(nStrIDCaption, sCaption, 512);
	LoadString(nStrIDText, sText, 512);
	
	return ::MessageBox(hWnd, sText, sCaption, nType);
}

/////////////////////////////////////////////////////////////////////////////
// Localizing window

BOOL __stdcall CLocalizer::EnumChildProcLink(HWND hWnd, LPARAM lParam)
{
	return ((CLocalizer *)(lParam))->EnumChildProc(hWnd);
}

BOOL CLocalizer::EnumChildProc(HWND hWnd)
{
	DWORD dwCtrlID = ::GetDlgCtrlID(hWnd);
	if(dwCtrlID)
	{
		tstring str = LoadString(::GetDlgCtrlID(hWnd));
		if(!str.empty())
			SetWindowText(hWnd, str.c_str());
	}
	
	return TRUE;
}

void CLocalizer::LocalizeDlg(HWND hWnd)
{
	// enumerate all child windows
	if(!IsWindow(hWnd))
		return;

	EnumChildWindows(hWnd, EnumChildProcLink, (LPARAM)this);
}

