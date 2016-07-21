#include "stdafx.h"
#include "RichText.h"
#include "RichTextImpl.h"

UINT UM_CANCANCELNOW = ::RegisterWindowMessage(_T("UM_CANCANCELNOW"));

CRichText::CRichText (){}
CRichText::~CRichText (){}

void CRichText::SetHyperTextFormat  ( HWND hCtrlWnd, DWORD dwTextHigh, LPCTSTR szFontName )
{

	_ASSERTE(::IsWindow(hCtrlWnd));
	
	if  ( ::IsWindow(hCtrlWnd) )
	{
		
		CHARFORMAT cf;
		ZeroMemory (&cf, sizeof (CHARFORMAT));
		cf.cbSize = sizeof (CHARFORMAT);

		SendMessage(hCtrlWnd, EM_GETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
		
		if ( dwTextHigh > 0 )
		{
			cf.dwMask |= CFM_SIZE;
			cf.yHeight = dwTextHigh;
		}
		
		if ( szFontName != NULL )
		{
			cf.dwMask |= CFM_FACE;
			_tcscpy( cf.szFaceName, szFontName );				
		}
		
		SendMessage ( hCtrlWnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);

	}
	
}


DWORD CRichText::GetLinkID ( HWND hCtrlWnd, WPARAM, LPARAM lParam )
{
	_ASSERTE(::IsWindow(hCtrlWnd));
	
	if  ( ::IsWindow(hCtrlWnd) )
	{
		CRichTextImpl * p = (CRichTextImpl *) GetWindowLong  (hCtrlWnd, GWL_USERDATA);
		
		if ( p )
		{
			ENLINK * pENLink = ( ENLINK * ) lParam;
			
			if ( pENLink -> msg == WM_LBUTTONUP )
				return p -> GetClickedLink  ( pENLink );
			
		}
		
	}
	
	return DWORD(LINK_NOT_PRESSED);
	
}

DWORD CRichText::GetOnBtnDownLinkID ( HWND hCtrlWnd, WPARAM, LPARAM lParam )
{
	_ASSERTE(::IsWindow(hCtrlWnd));
	
	if  ( ::IsWindow(hCtrlWnd) )
	{
		CRichTextImpl * p = (CRichTextImpl *) GetWindowLong  (hCtrlWnd, GWL_USERDATA);
		
		if ( p )
		{
			ENLINK * pENLink = ( ENLINK * ) lParam;
			
			if ( pENLink -> msg == WM_LBUTTONDOWN )
				return p -> GetClickedLink  ( pENLink );
			
		}
		
	}
	
	return DWORD(LINK_NOT_PRESSED);
	
}

/*
static int str_ensure_crlf (LPCTSTR szString, LPTSTR szOutString, int nOutStringLength)
{
	LPTSTR szLF = (LPTSTR)szString;

	int nSourceLength = _tcslen (szString);

	if (nSourceLength == 0)
		return 0;

	int nReplaceCount = 0;
	
	while (szLF && (szLF = _tcschr (szLF, _T('\n'))) != NULL)
	{
		if (szLF == szString || *_tcsdec (szString, szLF) != _T('\r'))
		{
			nReplaceCount++;
		}

		szLF = _tcsinc (szLF);
	}

	if (nReplaceCount)
	{
		if (nSourceLength + nReplaceCount * sizeof (TCHAR) + 1 > nOutStringLength)
			return nSourceLength + nReplaceCount * sizeof (TCHAR) + 1;

		szLF = (LPTSTR)szString;
		LPTSTR szFragmentStart = szLF;
				
		while (szLF && (szLF = _tcschr (szLF, _T('\n'))) != NULL)
		{
			if (szLF == szString || *_tcsdec (szString, szLF) != _T('\r'))
			{
				memcpy (szOutString, szFragmentStart, szLF - szFragmentStart);
				szOutString += szLF - szFragmentStart;
				_tcscpy (szOutString, _T("\r\n"));
				szOutString += _tcslen (_T("\r\n"));
			}

			szLF = _tcsinc (szLF);
			szFragmentStart = szLF;
		}

		_tcscpy (szOutString, szFragmentStart);
	}

	return nReplaceCount;
}
*/
bool CRichText::SetHyperText( HWND hWnd, LPCTSTR szText )
{
//	bool bStrAllocated = false;
	LPTSTR str = NULL;

	try
	{
		if (!szText || !::IsWindow(hWnd)) return false;
		
		CRichTextImpl * p = (CRichTextImpl *) GetWindowLong  (hWnd, GWL_USERDATA);
		
		if  ( p )
			p -> clear  ();
		else
			p = new CRichTextImpl(hWnd);
/*		
		int nNewStrLength = str_ensure_crlf (szText, NULL, 0);
		
		if (nNewStrLength)
		{
			str = new TCHAR [nNewStrLength];
			if (str == NULL)
				return false;

			bStrAllocated = true;

			str_ensure_crlf (szText, str, nNewStrLength);
		}
		else
*/
		{
			str = (LPTSTR)szText;
		}
		
		CRichTextImpl::TOKEN token;

		LPTSTR strtemp = str;
		
		while ( strtemp && p -> get_next_token  ( strtemp, token) )
		{
			_RPT1_ASSERT_IF ( ((token.mask & TOKEN_TYPE) && (token.mask & TOKEN_ID) && (token.type == CRichTextImpl::TOKEN::link ) && ( token.id == (DWORD)-1 )), _T("Invalid link id (%s) - value 0xffffffff reserved!"), token.start_text );
			p -> attach_rtf_text ( hWnd, token );
		}
		
		p -> colorize  ( hWnd );
		
	}
	catch(const int)
	{
		return false;
	}
/*
	if (bStrAllocated && str)
		delete []str;
*/		
	return true;
	
}

CString CRichText::GetPlainFromHyper( LPCTSTR szHyperText )
{
	CString szRet;
	try
	{
		if (!szHyperText) 
			return szRet;
		
		LPTSTR str = (LPTSTR)szHyperText;
		CRichTextImpl::TOKEN token;
		
		while ( str && CRichTextImpl::get_next_token  ( str, token) )
		{
			_RPT1_ASSERT_IF ( ((token.mask & TOKEN_TYPE) && (token.mask & TOKEN_ID) && (token.type == CRichTextImpl::TOKEN::link ) && ( token.id == (DWORD)-1 )), _T("Invalid link id (%s) - value 0xffffffff reserved!"), token.start_text );

			if ( !(token.mask & TOKEN_TEXT) || (token.start_text == NULL) || (token.type == CRichTextImpl::TOKEN::comment) )
				continue;
			
			size_t nLength = token.end_text ? token.end_text - token.start_text : _tcslen(token.start_text);
			
			if (nLength == 0)
				continue;
			
			szRet += CString(token.start_text).Left  (nLength);
		}
		
	}catch(...)
	{
		return CString();
	}

	return szRet;	
}

void CRichText::SetDragMode(HWND hWnd, bool bDrag)
{
	CRichTextImpl * p = (CRichTextImpl *) GetWindowLong  (hWnd, GWL_USERDATA);
	
	if  ( p )
		p->SetDragMode(bDrag);
}

void CRichText::Print2DC(HWND hWnd, CDC* pDC, CRect rect)
{
	rect.DeflateRect(1, 0, 1, 0);

	FORMATRANGE fr;
	fr.hdc = fr.hdcTarget = pDC->m_hDC;
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;
	// all of the rects need to be in twips

	int px = pDC->GetDeviceCaps (LOGPIXELSX);
	int py = pDC->GetDeviceCaps (LOGPIXELSY);
	fr.rc.top = fr.rcPage.top = (rect.top * 1440)/py;
	fr.rc.left = fr.rcPage.left = (rect.left * 1440)/px;
	fr.rc.right = fr.rcPage.right = (rect.right * 1440)/px;
	fr.rc.bottom = fr.rcPage.bottom = (rect.bottom * 1440)/py;
	::SendMessage(hWnd, EM_FORMATRANGE, TRUE, (LPARAM)&fr);
	::SendMessage(hWnd, EM_DISPLAYBAND, 0, (LPARAM)&rect);
	::SendMessage(hWnd, EM_FORMATRANGE, 0, (LPARAM)NULL);
	
	CRichTextImpl * p = (CRichTextImpl *) GetWindowLong  (hWnd, GWL_USERDATA);
	if  ( p )
		p->DrawLocks(hWnd, pDC->m_hDC, CPoint(rect.left, rect.top));

	
	
}