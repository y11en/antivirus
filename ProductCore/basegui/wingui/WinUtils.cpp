// WinUtils.cpp: implementation of the CWinUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "winroot.h"
#include <shellapi.h>
#include <shlobj.h>
#include <mapi.h>
#include <locale>
#include <algorithm>
#include "..\Formats.h"
#include "..\Graphics.h"
#include "..\SScale\ClassMatrix.h"

//////////////////////////////////////////////////////////////////////

TSTRING GetShortTimeFormat(LPCSTR szTimeFmt, LPCSTR szSeparator)
{
	TSTRING strTimeFmt(szTimeFmt);

	bool b24hours = strTimeFmt.find(_T("H")) != TSTRING::npos;
	bool bMarker = strTimeFmt.find(_T("t")) != TSTRING::npos;

	strTimeFmt = b24hours ? "H" : "h";
	strTimeFmt += szSeparator;
	strTimeFmt += "mm";
	if(bMarker && !b24hours)
		strTimeFmt += " t";

	return strTimeFmt;
}

LPCSTR GetDateTimeFormat(DTT_TYPE eType)
{
	static bool g_fInit = false;
	static TCHAR g_szFmtDate[MAX_PATH] = "";
	static TCHAR g_szFmtTime[MAX_PATH] = "";
	static TCHAR g_szFmtTStm[MAX_PATH] = "";
	static TCHAR g_szFmtTShrt[MAX_PATH] = "";
	static TCHAR g_szFmtDT[MAX_PATH] = "";

	if( !g_fInit )
	{
		if( !::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, g_szFmtDate, countof(g_szFmtDate)) )
			::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SSHORTDATE, g_szFmtDate, countof(g_szFmtDate));
		
		if( !::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, g_szFmtTime, countof(g_szFmtTime)) )
			::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_STIMEFORMAT, g_szFmtTime, countof(g_szFmtTime));
		
		TCHAR l_szSepTime[MAX_PATH] = ":";

		if( !::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIME, l_szSepTime, countof(l_szSepTime)) )
			::GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_STIME, l_szSepTime, countof(l_szSepTime));
		
		_sntprintf(g_szFmtTStm, countof(g_szFmtTStm), _T("HH%smm%sss"), l_szSepTime, l_szSepTime);
		
		// !!!!!!!! Temp! Must be derived from g_szFmtTime!
		_sntprintf(g_szFmtTShrt, countof(g_szFmtTStm), GetShortTimeFormat(g_szFmtTime, l_szSepTime).c_str(), l_szSepTime);
		
		_sntprintf(g_szFmtDT, countof(g_szFmtDT), "%s %s", g_szFmtDate, g_szFmtTime);
		
		g_fInit = true;
	}

	switch( eType )
	{
	case DTT_DT:             return g_szFmtDT;
	case DTT_TIMESTAMP:      return g_szFmtTStm;
	case DTT_TIMESHORT:      return g_szFmtTShrt;
	case DTT_DATE:           return g_szFmtDate;
	case DTT_TIME:           return g_szFmtTime;
	}

	return "";
}

LPCSTR GetDateTimeStr(CRootItem* pRoot, cDateTime& prDt, tString& str, DTT_TYPE eType, bool bSeconds, bool bLocal, LPCSTR strEstDaysFmt)
{
	switch( eType )
	{
	case DTT_TIMESTAMP:
	case DTT_TIME:
	{
		time_t tTime = prDt;
		LPCSTR strFmt = GetDateTimeFormat(DTT_TIME);
		str = "";
		
		DWORD      l_day;
		SYSTEMTIME l_sysTime; memset(&l_sysTime, 0, sizeof(l_sysTime));
		l_day             = (((unsigned long)tTime)/(60*60*24));
		l_sysTime.wHour   = (WORD)((((unsigned long)tTime)/(60*60))%24);
		l_sysTime.wMinute = (WORD)((((unsigned long)tTime)/(60))%60);
		l_sysTime.wSecond = (WORD)((((unsigned long)tTime)/(1))%60);
		
		CHAR buff[MAX_PATH];
		if( eType == DTT_TIMESTAMP )
		{
			if( l_day )
			{
				cGuiParams _Data;
				_Data.m_nVal1 = (tPTR)l_day;
				str = pRoot->GetFormatedText(str, pRoot->GetString(tString(), PROFILE_LOCALIZE, NULL, strEstDaysFmt), &_Data);
				str += " ";
			}
			strFmt = GetDateTimeFormat(DTT_TIMESTAMP);
		}
		
		if( ::GetTimeFormatA(LOCALE_USER_DEFAULT, bSeconds ? 0 : TIME_NOSECONDS, &l_sysTime, strFmt, buff, countof(buff)) )
			str += buff;
	} break;
	
	case DTT_TIMESHORT:
	{
		time_t tTime = prDt;

		SYSTEMTIME l_sysTime; memset(&l_sysTime, 0, sizeof(l_sysTime));
		l_sysTime.wHour   = (WORD)((((unsigned long)tTime)/(60*60))%24);
		l_sysTime.wMinute = (WORD)((((unsigned long)tTime)/(60))%60);
		l_sysTime.wSecond = 0;
		
		str = _T("");

		CHAR buff[MAX_PATH];
		if( ::GetTimeFormatA(LOCALE_USER_DEFAULT, bSeconds ? 0 : TIME_NOSECONDS, &l_sysTime, GetDateTimeFormat(DTT_TIME), buff, countof(buff)) )
			str += buff;
	} break;

	default:
	{
		tDWORD dwYear, dwMonth, dwDay, dwHour, dwMinute, dwSecond;
		if( PR_FAIL(prDt.GetUTC(&dwYear, &dwMonth, &dwDay, &dwHour, &dwMinute, &dwSecond)) )
			return "";

		SYSTEMTIME l_sysTime;
		l_sysTime.wYear         = dwYear;
		l_sysTime.wMonth        = dwMonth;
		l_sysTime.wDayOfWeek    = 0;
		l_sysTime.wDay          = dwDay;
		l_sysTime.wHour         = dwHour;
		l_sysTime.wMinute       = dwMinute;
		l_sysTime.wSecond       = dwSecond;
		l_sysTime.wMilliseconds = 0;

		if( bLocal )
		{
			FILETIME _ft1, _ft2;
			if( !::SystemTimeToFileTime(&l_sysTime, &_ft1) )
				return "";
			if( !::FileTimeToLocalFileTime(&_ft1, &_ft2) )
				return "";
			if( !::FileTimeToSystemTime(&_ft2, &l_sysTime) )
				return "";
		}

		TCHAR buff[MAX_PATH];
		
		str = _T("");

		if( ::GetDateFormatA(LOCALE_USER_DEFAULT, 0 , &l_sysTime, GetDateTimeFormat(DTT_DATE), buff, countof(buff)) )
			str += buff;
		
		if( eType == DTT_DT )
			if( ::GetTimeFormatA(LOCALE_USER_DEFAULT, bSeconds ? 0 : TIME_NOSECONDS, &l_sysTime, GetDateTimeFormat(DTT_TIME), buff, countof(buff)) )
			{
				if( !str.empty() )
					str += _T(" ");
				str += buff;
			}
	}
	}

	return str.c_str();
}

//////////////////////////////////////////////////////////////////////

#define ELLIPSE _T("...")
#ifndef _countof
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif // _countof

struct CLimitInfo {
	int m_nLenLimit;
	HDC m_hDC;
};

typedef bool (*CheckStrFunc)( const TCHAR *pStr, CLimitInfo &rcLimitInfo );

// ---
const TCHAR * FindDivider(const TCHAR *pSource)
{
	for( ; *pSource; pSource++ )
	{
		switch(*pSource)
		{
		case _T('\\'):
		case _T('/'): return pSource;
		}
	}

	return NULL;
}

const TCHAR * FindDividerR(const TCHAR *pSource)
{
	size_t len = _tcslen(pSource);
	for(pSource = pSource + len - 1; len; len--, pSource--)
	{
		switch(*pSource)
		{
		case _T('\\'):
		case _T('/'): return pSource;
		}
	}

	return NULL;
}

tCHAR*  CompressPath(const tCHAR *pStr, HWND hWnd, bool bExclusive = true, tCHAR *pDst = NULL, DWORD dwDstLen = 0);
tCHAR*  CompressPath(const tCHAR *pStr, int len, bool bExclusive = true, tCHAR *pDst = NULL, DWORD dwDstLen = 0 );

// ---
// Сжатие пути
// c:\aaa\bbb\ccc\ddd\file.ext
// c:\...\ddd\file.ext
static TCHAR * CompressPathImp( const TCHAR *pSource, CheckStrFunc pCheckFunc, bool bExclusive, CLimitInfo &rcLimitInfo, TCHAR *pDst, DWORD dwDstLen ) {
  static TCHAR pFinishStr[_MAX_PATH << 2];

	*pFinishStr = 0;
	if ( pDst )
		*pDst = 0;

	if ( pSource ) {
		TCHAR *pStr = (TCHAR *)_alloca(((_tcslen(pSource) + 1) << 1) * sizeof(TCHAR) );
		if ( pStr ) {
			_tcscpy( pStr, pSource );

			if( !pCheckFunc(pSource, rcLimitInfo) ) {
				TCHAR *p = _tcschr( pStr, L':' );
				if ( p ) 
					p = _tcsinc(p);
				else
				{ 
					p = _tcsstr( pStr, _T("\\\\") );
					if ( p )
					{
						p = _tcsninc(p, 2);
						p = (TCHAR *)::FindDivider( p );
					}
					else
						p = pStr;
				}  

				const TCHAR *p1 = p ? ::FindDivider( p = _tcsinc(p) ) : NULL;

				size_t nElLen = _tcsclen( ELLIPSE );
				if ( p1 )
				{
					if ( (_tcsclen(p) - _tcsclen(p1)) < nElLen ) 
						memmove( _tcsninc(p, nElLen), p1, _tcsclen(p1) + 1 );

					_tcsnset( p, L'.', nElLen );
					p = _tcsninc( p, nElLen );

					p1 = ::FindDivider( p1 );
					if ( p1 ) {
						while( !pCheckFunc(pStr, rcLimitInfo) ) {
							_tcscpy( p, p1 );
							if ( !(p1 = ::FindDivider( p+1)) )
								break;
						}
					}
				}
				if ( bExclusive ) {
					if ( !pCheckFunc(pStr, rcLimitInfo) && _tcsclen(pStr) > nElLen ) 
						_tcscpy( _tcsninc(pStr, _tcsclen(pStr) - nElLen), ELLIPSE );
					while ( !pCheckFunc(pStr, rcLimitInfo) && _tcsclen(pStr) > 4 ) 
						_tcscpy( _tcsninc(pStr, _tcsclen(pStr) - 4), ELLIPSE );
				}    
			}
			if ( pDst && dwDstLen ) {
				if ( _tcsclen(pStr) > dwDstLen ) 
					_tcscpy( _tcsninc(pStr, _tcsclen(pStr) - dwDstLen), ELLIPSE );
				_tcsncpy( pDst, pStr, dwDstLen );
			}
			else {
				if ( _tcsclen(pStr) > _countof(pFinishStr) ) 
					_tcscpy( _tcsninc(pStr, _tcsclen(pStr) - _countof(pFinishStr)), ELLIPSE );
				_tcsncpy( pFinishStr, pStr, _countof(pFinishStr) );
			}
		}
		else {
			if ( pDst && dwDstLen )
				_tcsncpy( pDst, pSource, dwDstLen );
			else
				_tcsncpy( pFinishStr, pSource, _countof(pFinishStr) );
		}
	}
	if ( pDst && dwDstLen )
		return pDst;
	else	
		return pFinishStr;
}

// ---
static bool CheckLenOfSymbols( const TCHAR *pStr, CLimitInfo &rcLimitInfo ) {
  return !(_tcsclen(pStr) > (size_t)rcLimitInfo.m_nLenLimit);
}

// ---
static bool CheckLenOfSize( const TCHAR *pStr, CLimitInfo &rcLimitInfo ) {
	SIZE size;
	::GetTextExtentPoint32( rcLimitInfo.m_hDC, pStr, (int)_tcsclen(pStr), &size );
  return !(size.cx > rcLimitInfo.m_nLenLimit);
}

// ---
TCHAR * CompressPath( const TCHAR *pStr, int len, bool bExclusive/* = true*/, TCHAR *pDst/* = NULL*/, DWORD dwDstLen/* = 0*/ ) {
	CLimitInfo rcLimitInfo;
	memset( &rcLimitInfo, 0, sizeof(rcLimitInfo) );
  rcLimitInfo.m_nLenLimit = len;
  return ::CompressPathImp( pStr, CheckLenOfSymbols, bExclusive, rcLimitInfo, pDst, dwDstLen );
}


// ---
TCHAR *CompressPath( const TCHAR *pStr, HFONT hFont, int len, bool bExclusive/* = true*/, TCHAR *pDst/* = NULL*/, DWORD dwDstLen/* = 0*/ ) {
	CLimitInfo rcLimitInfo;
	memset( &rcLimitInfo, 0, sizeof(rcLimitInfo) );
  rcLimitInfo.m_nLenLimit = len;
	rcLimitInfo.m_hDC = ::CreateCompatibleDC( NULL );

  HFONT hOld = hFont ? (HFONT)::SelectObject( rcLimitInfo.m_hDC, hFont ) : NULL;

  TCHAR *ret = ::CompressPathImp( pStr, CheckLenOfSize, bExclusive, rcLimitInfo, pDst, dwDstLen );

  if ( hOld )
    ::SelectObject( rcLimitInfo.m_hDC, hOld );

	::DeleteDC( rcLimitInfo.m_hDC );

  return ret;
}

// ---
TCHAR *CompressPath( const TCHAR *pStr, HWND hWnd, bool bExclusive/* = true*/, TCHAR *pDst/* = NULL*/, DWORD dwDstLen/* = 0*/ ) {
  RECT rcRect;
  if (::GetWindowRect(hWnd, &rcRect) ) {
		HFONT hFont = (HFONT)(UINT)::SendMessage( hWnd, WM_GETFONT, 0, 0L );
		return ::CompressPath( pStr, hFont, rcRect.right - rcRect.left, bExclusive, pDst, dwDstLen );
	}
	return NULL;
}


// Сжать путь до заданной длины в пикселях для заданного DC на его фонте
// Если bExclusive = true и путь не удается сжать, то он укорачивается принудительно
// и дописывается "..."
TCHAR *CompressPath( const TCHAR *pStr, HDC hDC, int len, bool bExclusive/* = true*/, TCHAR *pDst/* = NULL*/, DWORD dwDstLen/* = 0*/ ) {
	CLimitInfo rcLimitInfo;
	memset( &rcLimitInfo, 0, sizeof(rcLimitInfo) );
	rcLimitInfo.m_nLenLimit = len;
	rcLimitInfo.m_hDC = hDC;
	return ::CompressPathImp( pStr, CheckLenOfSize, bExclusive, rcLimitInfo, pDst, dwDstLen );
}

void osd_CompressName(CRootItem * pRoot, OSDCN_TYPE eType, const tCHAR * pStr, HDC hDc, HFONT hFont, int nCX, tString& strDst)
{
	strDst = pStr ? pStr : "";
	if( strDst.empty() )
		return;
	
	/*
	if( eType == OSDCN_PATH )
	{
		CLimitInfo rcLimitInfo;
		memset(&rcLimitInfo, 0, sizeof(rcLimitInfo));
		rcLimitInfo.m_nLenLimit = nCX;
		rcLimitInfo.m_hDC = hDc;
		::CompressPathImp(pStr, CheckLenOfSize, true, rcLimitInfo, &strDst[0], strDst.size());
		strDst.resize(strlen(strDst.c_str()));
	}
	*/

	RECT rcRect = {0, 0, nCX, MAX_CTL_SIZE};
	UINT fmtf = (eType == OSDCN_PATH ? DT_PATH_ELLIPSIS : 0)|DT_END_ELLIPSIS|DT_MODIFYSTRING|DT_SINGLELINE|DT_CALCRECT;
	
	HFONT hFontOld = (HFONT)::SelectObject(hDc, hFont);

	tObjPath _str; pRoot->LocalizeStr(_str, strDst.c_str(), strDst.size());

	int nLen = _str.size(); _str.resize(nLen + 4);
	::DrawTextW(hDc, _str.data(), _str.size(), &rcRect, fmtf);

	pRoot->LocalizeStr(strDst, _str.data(), _str.size());
	strDst.resize(strlen(strDst.c_str()));

	::SelectObject(hDc, hFontOld);
}

void ConvertPath(PATHOP_TYPE eType, LPCSTR strText, tString& strRes)
{
	strRes.clear();
	
	if( !strText )
		return;

	switch(eType)
	{
	case PATHOP_FILE:
	{
		LPCSTR l_sep = ::FindDividerR(strText);
		strRes = l_sep ? &l_sep[1] : strText;
		if( strRes.find_first_of(MBCS_UTF8_SEP) != tString::npos )
			strRes.insert(strRes.begin(), MBCS_UTF8_SEP);

	} break;

	case PATHOP_DIR:
	{
		LPCSTR l_sep = ::FindDividerR(strText);
		if( l_sep )
			strRes.erase(), strRes.append(strText, l_sep - strText + 1);
		else
			strRes = strText;
	} break;
	
	default: strRes = strText;
	}
}

void CompressText(LPWSTR strDst, int nDstSize, LPCWSTR strSrc, int nSrcLen)
{
	if( nDstSize < 4 )
		return;
	
	if( !strSrc )
		strSrc = L"";
	if( !nSrcLen )
		nSrcLen = (int)wcslen(strSrc);

	if( nSrcLen <= nDstSize - 1 )
		memcpy(strDst, strSrc, sizeof(WCHAR)*(nSrcLen + 1));
	else
	{
		memcpy(strDst, strSrc, sizeof(WCHAR)*(nDstSize - 4));
		memcpy(strDst + nDstSize - 4, "...", sizeof(WCHAR)*4);
	}
}

//////////////////////////////////////////////////////////////////////
// CFontStyleImpl

class CFontStyleImpl : public CFontStyle
{
public:
	CFontStyleImpl() : m_pRoot(NULL), m_hFont(NULL), m_hFontU(NULL){}
	~CFontStyleImpl(){ Clear(); }

	void Destroy(){ delete this; };

	void Clear()
	{
		if( m_hFont )
			DeleteObject(m_hFont), m_hFont = NULL;
		if( m_hFontU )
			DeleteObject(m_hFontU), m_hFontU = NULL;
	}

	void Create(bool bU = false)
	{
		CDesktopDC desktopDC;
		int nLOGPIXELSY = GetDeviceCaps(desktopDC, LOGPIXELSY);

		(bU ? m_hFontU : m_hFont) = 
			CreateFont(-MulDiv(m_nHeight, nLOGPIXELSY, 72),
			0, 0, 0, m_nBold, m_bItalic, bU ? TRUE : m_bUnderline, FALSE,
			((CWinRoot *)m_pRoot)->m_nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, m_strName.c_str());
	}

	bool Init(CRootItem * pRoot, LPCSTR strName, tDWORD nHeight, COLORREF nColor, tDWORD nBold, bool bItalic, bool bUnderline, tDWORD nCharExtra)
	{
		Clear();

		m_pRoot = pRoot;
		m_clColor = nColor;
		m_nHeight = nHeight;
		m_nCharExtra = nCharExtra;
		m_nBold = nBold;
		m_bItalic = bItalic;
		m_bUnderline = bUnderline;
		m_strName = strName;

		Create();
		return true;
	}

	bool Draw(HDC dc, LPCSTR strText, RECT& rc, tDWORD nFlags)
	{
		if( !strText || *strText == '\0' )
		{
			if( nFlags & TEXT_CALCSIZE )
			{
				::DrawText(dc, "W", 1, &rc, DT_CALCRECT);
				rc.right = rc.left;
			}
			
			return true;
		}
		
		HFONT hFont = m_hFont;
		COLORREF clColor = m_clColor;

		if( nFlags & TEXT_GRAYED )
			clColor = ::GetSysColor(COLOR_BTNSHADOW);
		else if( nFlags & TEXT_HOTLIGHT )
		{
			if( m_bUnderline )
				clColor = RGB(248,151,48);
			else
			{
				if( !m_hFontU ) Create(true); 
				hFont = m_hFontU;
			}
		}

		HFONT hOldFont = (HFONT)SelectObject(dc, hFont);

		SetTextColor(dc, clColor);

		SetBkMode(dc, (nFlags & TEXT_OPAQUE) ? OPAQUE : TRANSPARENT);
		int nCharExtraOld = SetTextCharacterExtra(dc,m_nCharExtra);

		DWORD nDrawFlags = DT_WORDBREAK;
		if( nFlags & ALIGN_RIGHT )
			nDrawFlags |= DT_RIGHT;
		else if( nFlags & ALIGN_HCENTER )
			nDrawFlags |= DT_CENTER;
		
		if( nFlags & ALIGN_BOTTOM )
			nDrawFlags |= DT_BOTTOM;
		else if( nFlags & ALIGN_VCENTER )
			nDrawFlags |= DT_VCENTER;

		if( nFlags & TEXT_SINGLELINE )
			nDrawFlags |= DT_SINGLELINE;

		if( nFlags & TEXT_CALCSIZE )
		{
			nDrawFlags |= DT_CALCRECT;

			if( !rc.right )
				rc.right = 1;
		}
		else
			nDrawFlags |= DT_END_ELLIPSIS|DT_WORD_ELLIPSIS;

		if( nFlags & TEXT_MODIFY )
		{
			nDrawFlags |= DT_MODIFYSTRING;
		}

		if( !(nFlags & (TEXT_CALCSIZE|TEXT_SINGLELINE)) && (nFlags & (ALIGN_VCENTER|ALIGN_BOTTOM)) )
		{
			RECT rcTxt = {rc.left, 0, rc.right, MAX_CTL_SIZE};
			CWindowL::DrawText(m_pRoot, dc, strText, rcTxt, nDrawFlags|DT_CALCRECT);

			int cy = RECT_CY(rcTxt);

			rcTxt.left = rc.left;
			rcTxt.right = rc.right;
			if( nFlags & ALIGN_BOTTOM )
			{
				rcTxt.top = rc.bottom - cy;
				rcTxt.bottom = rc.bottom;
			}
			else if( nFlags & ALIGN_VCENTER )
			{
				rcTxt.top = (rc.top + rc.bottom - cy)/2;
				rcTxt.bottom = rcTxt.top + cy;
			}
			
			CWindowL::DrawText(m_pRoot, dc, strText, rcTxt, nDrawFlags);
		}
		else
			CWindowL::DrawText(m_pRoot, dc, strText, rc, nDrawFlags);

		SelectObject(dc, hOldFont);
		SetTextCharacterExtra(dc,nCharExtraOld);
		return true;
	}

	HFONT    Handle(){ return m_hFont; }
	COLORREF& GetColor(){ return m_clColor; }
	
	tString  GetName()
	{
		CDesktopDC desktopDC;
		HGDIOBJ hOldFont = ::SelectObject(desktopDC, (HGDIOBJ)m_hFont);
		char strFontName[256]; ::GetTextFace(desktopDC, countof(strFontName), strFontName);
		::SelectObject(desktopDC, hOldFont);
		
		return strFontName;
	}
	
	void     GetInfo(INFO& pInfo)
	{
		CDesktopDC desktopDC;
		HGDIOBJ hOldFont = ::SelectObject(desktopDC, (HGDIOBJ)m_hFont);
		TEXTMETRIC cTm; ::GetTextMetrics(desktopDC, &cTm);
		::SelectObject(desktopDC, hOldFont);
		
		pInfo.m_nHeight    = m_nHeight;
		pInfo.m_nBold      = cTm.tmWeight;
		pInfo.m_bItalic    = cTm.tmItalic ? true : false;
		pInfo.m_bUnderline = cTm.tmUnderlined ? true : false;
		pInfo.m_nCharExtra = m_nCharExtra;
	}

private:
	CRootItem * m_pRoot;
	HFONT       m_hFont;
	HFONT       m_hFontU;
	COLORREF    m_clColor;
	tDWORD      m_nHeight;
	tDWORD      m_nCharExtra;
	tDWORD      m_nBold;
	tString     m_strName;
	bool        m_bItalic;
	bool        m_bUnderline;
};

//////////////////////////////////////////////////////////////////////

CFontStyle * CWinRoot::CreateFont()
{
	return new CFontStyleImpl();
}

//////////////////////////////////////////////////////////////////////
// CAlphaBrush

void CAlphaBrush::Draw(HDC dc, RECT& rc)
{
	::SetBkColor(dc, m_cl);
	::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}

//////////////////////////////////////////////////////////////////////
// CBitmapHndEx

void CBitmapHndEx::Clear()
{
	if( m_hDC && m_hOldBitmap )
		::SelectObject(m_hDC, m_hOldBitmap), m_hOldBitmap = NULL;

	if( m_hMaskDC && m_hOldMask )
		::SelectObject(m_hMaskDC, m_hOldMask), m_hOldMask = NULL;

	m_hDC.Cleanup();
	m_hMaskDC.Cleanup();
	m_hBitmap.Cleanup();
	m_hMask.Cleanup();
	m_clTransparent = -1;
	m_szSize.cy = m_szSize.cx = 0;
}

bool CBitmapHndEx::Init(HBITMAP hBmp, DWORD nImageSize, bool bMask)
{
	Clear();
	m_hBitmap = hBmp;
	if( m_hBitmap )
	{
		BITMAP bmp; ::GetObject((HBITMAP)m_hBitmap, sizeof(BITMAP), &bmp);
		m_szSize.cx = bmp.bmWidth;
		m_szSize.cy = bmp.bmHeight;

		if( nImageSize )
			m_szSize.cx = nImageSize;
	}

	m_hDC = ::CreateCompatibleDC(CDesktopDC());
	if( !m_hDC )
		return false;

	if( m_hBitmap )
		m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_hBitmap);

	if( bMask )
		InitMask();

	return true;
}

void CBitmapHndEx::InitMask()
{
	m_clTransparent = ::GetPixel(m_hDC, 1, 1);
	if( !m_hMaskDC )
	{
		m_hMaskDC = ::CreateCompatibleDC(CDesktopDC());
		m_hMask = ::CreateBitmap(m_szSize.cx, m_szSize.cy, 1, 1, NULL);
		m_hOldMask = (HBITMAP)::SelectObject(m_hMaskDC, m_hMask);
	}

	COLORREF bkOld = SetBkColor(m_hDC, m_clTransparent);
	BitBlt(m_hMaskDC, 0, 0, m_szSize.cx, m_szSize.cy, m_hDC, 0, 0, NOTSRCCOPY);
	SetBkColor(m_hDC, bkOld);

	BitBlt(m_hDC, 0, 0, m_szSize.cx, m_szSize.cy, m_hMaskDC, 0, 0, SRCAND);
	BitBlt(m_hMaskDC, 0, 0, m_szSize.cx, m_szSize.cy, NULL, 0, 0, DSTINVERT);
}

bool CBitmapHndEx::IsTransparent()
{
	return m_clTransparent != -1 && m_hMask;
}

void CBitmapHndEx::Draw(HDC dc, RECT& rc, int nOffset)
{
	DWORD    nBitFlag = SRCCOPY;
	COLORREF l_clrTextOld;
	COLORREF l_clrBkOld;
	RECT     rcDraw = {rc.left, rc.top, rc.left + m_szSize.cx, rc.top + m_szSize.cy};
	
	if( IsTransparent() )
	{
		nBitFlag = SRCPAINT;
		l_clrTextOld = ::SetTextColor(dc, 0x00000000L);
		l_clrBkOld = ::SetBkColor(dc, 0x00FFFFFFL);

		::BitBlt(dc, rcDraw.left, rcDraw.top, RECT_CX(rcDraw), RECT_CY(rcDraw), m_hMaskDC, nOffset, 0, SRCAND);
	}

	if( m_hBitmap )
		::BitBlt(dc, rcDraw.left, rcDraw.top, RECT_CX(rcDraw), RECT_CY(rcDraw), m_hDC, nOffset, 0, nBitFlag);
	else
		DrawCustom(dc, rcDraw, m_hDC, nOffset);

	if( nBitFlag != SRCCOPY )
	{
		::SetTextColor(dc, l_clrTextOld);
		::SetBkColor(dc, l_clrBkOld);
	}
}

bool CBitmapHndEx::Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx)
{
	Draw(dc, rc, m_szSize.cx*nIdx);
	return true;
}

//////////////////////////////////////////////////////////////////////
// CIconImpl

void CIconImpl::Clear()
{
	if( m_hIcon )
		::DestroyIcon(m_hIcon), m_hIcon = NULL;
}

bool CIconImpl::Init(HICON hIcon, bool bRef, bool bRepare)
{
	Clear();
	m_hIcon = !bRef ? ::CopyIcon(hIcon) : hIcon;

	if( !m_hIcon )
		return false;

	IconInfo iInfo;
	::GetIconInfo(m_hIcon, &iInfo);
	InitSize(iInfo);

	if( !bRepare )
		return true;

	CDesktopDC desktopDC;
	CDcHnd hDcSrc = ::CreateCompatibleDC(desktopDC);
	CDcHnd hDcBmp = ::CreateCompatibleDC(desktopDC);

	CBitmapHnd hMask = ::CreateBitmap(m_szDef.cx, m_szDef.cy, 1, 1, NULL);

	HBITMAP hOldSrc = (HBITMAP)::SelectObject(hDcSrc, iInfo.hbmColor);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDcBmp, hMask);

	COLORREF bkOld = SetBkColor(hDcSrc, 0);
	BitBlt(hDcBmp, 0, 0, m_szDef.cx, m_szDef.cy, hDcSrc, 0, 0, SRCCOPY);
	SetBkColor(hDcSrc, bkOld);

	::SelectObject(hDcBmp, hOldBmp);
	::SelectObject(hDcSrc, hOldSrc);

	ICONINFO ii; ii = iInfo;
	ii.hbmMask = hMask;

	::DestroyIcon(m_hIcon);
	m_hIcon = ::CreateIconIndirect(&ii);

	return !!m_hIcon;
}

void CIconImpl::InitSize(IconInfo& iInfo)
{
	BITMAP bmpInfo; ::GetObject(iInfo.hbmMask, sizeof(BITMAP), &bmpInfo);

	m_szDef.cx = bmpInfo.bmWidth;
	m_szDef.cy = iInfo.hbmColor ? bmpInfo.bmHeight : bmpInfo.bmHeight / 2;
}

bool CIconImpl::Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx)
{
	::DrawIconEx(dc, rc.left, rc.top, m_hIcon, 0, 0, 0, NULL, DI_NORMAL);
	return true;
}

bool CIconImpl::Merge(CImageBase * pIcon, tDWORD nIdx)
{
	if( !pIcon )
		return false;

	IconInfo iInfo, iInfo1;
	::GetIconInfo(m_hIcon, &iInfo);
	::GetIconInfo(pIcon->IconHnd(), &iInfo1);

	BITMAP bInfo, bInfo1;
	::GetObject(iInfo.hbmMask,  sizeof(BITMAP), &bInfo);
	::GetObject(iInfo1.hbmMask,  sizeof(BITMAP), &bInfo1);

	SIZE sz = {bInfo.bmWidth, bInfo.bmHeight};
	SIZE sz1 = {bInfo1.bmWidth, bInfo1.bmHeight};

	CDesktopDC desktopDC;
	CDcHnd hDcSrc = ::CreateCompatibleDC(desktopDC);
	CDcHnd hDcBmp = ::CreateCompatibleDC(desktopDC);

	// make mask
	HBITMAP hOldSrc = (HBITMAP)::SelectObject(hDcSrc, iInfo.hbmMask);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hDcBmp, iInfo1.hbmMask);
	::StretchBlt(hDcSrc, 0, 0, sz.cx, sz.cy, hDcBmp, 0, 0, sz1.cx, sz1.cy, SRCAND);

	// make icon
	::SelectObject(hDcSrc, iInfo.hbmColor);

	::SelectObject(hDcBmp, iInfo1.hbmMask);
	::StretchBlt(hDcSrc, 0, 0, sz.cx, sz.cy, hDcBmp, 0, 0, sz1.cx, sz1.cy, SRCAND);

	::SelectObject(hDcBmp, iInfo1.hbmColor);
	::StretchBlt(hDcSrc, 0, 0, sz.cx, sz.cy, hDcBmp, 0, 0, sz1.cx, sz1.cy, SRCPAINT);

	::SelectObject(hDcBmp, hOldBmp);
	::SelectObject(hDcSrc, hOldSrc);

	Clear();
	m_hIcon = ::CreateIconIndirect(&iInfo);
	return !!m_hIcon;
}

//////////////////////////////////////////////////////////////////////

CIcon* CWinRoot::CreateIcon(HICON hIcon, bool bRef, CImageBase* pImage)
{
	if( !pImage )
		pImage = new CIconImpl();

	if( pImage )
		pImage->m_pRoot = this;

	((CIconImpl*)pImage)->Init(hIcon, bRef);
	return pImage;
}

//////////////////////////////////////////////////////////////////////
// CIconList

void CIconList::Clear()
{
	CIconImpl::Clear();

	if( m_hIL )
		::ImageList_Destroy(m_hIL), m_hIL = NULL;
}

bool CIconList::Init(HICON hIcon, int nImageSizeX)
{
	m_nImageSizeX = nImageSizeX;
	return CIconImpl::Init(hIcon, true);
}

void CIconList::InitSize(IconInfo& iInfo)
{
	CIconImpl::InitSize(iInfo);
	if( m_nImageSizeX )
	{
		m_nCount = m_szDef.cx / m_nImageSizeX;
		m_szDef.cx = m_nImageSizeX;
		m_hIL = ImageList_Create(m_szDef.cx, m_szDef.cy, ILC_COLORDDB|ILC_MASK, m_nCount, m_nCount);
		ImageList_Add(m_hIL, iInfo.hbmColor, iInfo.hbmMask);
	}
}

bool CIconList::Draw(HDC dc, RECT& rc, CBgContext* pCtx, tDWORD nIdx)
{
	if( (tINT)nIdx >= 0 && nIdx < m_nCount )
		ImageList_Draw(m_hIL, nIdx, dc, rc.left, rc.top, ILD_NORMAL);
	return true;
}

//////////////////////////////////////////////////////////////////////
// CBackgroundImpl

class CBackgroundImpl : public CBackgroundBase
{
public:
	bool Init(COLORREF nBGColor, COLORREF nFrameColor, int nRadius, int nStyle)
	{
		m_hBrush.Cleanup();

		if( !CBackgroundBase::Init(nBGColor, nFrameColor, nRadius, nStyle) )
			return false;

		if( m_clBackround != -1 )
			m_hBrush = CreateSolidBrush(m_clBackround);
		
		return true;
	}

	HBRUSH BrushHnd() { return m_hBrush; }

private:
	CBrushHnd m_hBrush;
};

//////////////////////////////////////////////////////////////////////

CBackground * CWinRoot::CreateBackground(COLORREF nBGColor, COLORREF nFrameColor, int nRadius, int nStyle, CImageBase * pImage)
{
	if( !pImage )
		pImage = new CBackgroundImpl();

	if( pImage )
		pImage->m_pRoot = this;

	((CBackgroundImpl *)pImage)->Init(nBGColor, nFrameColor, nRadius, nStyle);
	return pImage;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

HANDLE LoadImageFromIO(hIO pSource, DWORD nType, DWORD nFlags)
{
	bool bNative = pSource->propGetDWord(pgPLUGIN_ID) == PID_NATIVE_FIO;

	cStringObj strPath;
	if( bNative )
	{
		strPath.assign(pSource, pgOBJECT_FULL_NAME);
	}
	else
	{
		tCHAR tmpBuff[0x1000], tmpName[MAX_PATH];
		if( !GetTempPath(sizeof(tmpBuff), tmpBuff) )
			return NULL;

		if( !GetTempFileName(tmpBuff, "IMG", 0 , tmpName) )
			return NULL;

		cIOObj pIO((hOBJECT)pSource, cAutoString (cStringObj(tmpName)), fACCESS_RW, fOMODE_CREATE_ALWAYS);

		tQWORD nOffset = 0;
		tDWORD nSize = sizeof(tmpBuff);
		tERROR nError = pIO.last_error();

		while( PR_SUCC(nError) && nSize == sizeof(tmpBuff) )
		{
			nError = pSource->SeekRead(&nSize, nOffset, tmpBuff, sizeof(tmpBuff));
			if( PR_SUCC(nError) )
				nError = pIO->SeekWrite(NULL, nOffset, tmpBuff, nSize);
			nOffset += nSize;
		}

		if( PR_FAIL(nError) )
			return NULL;

		strPath = tmpName;
	}

	HANDLE hImage = NULL;
	if( CWindowL::m_bAnsi )
	{
		const cStrBuff& strImagePath = strPath.c_str(cCP_ANSI);
		hImage = ::LoadImageA(NULL, strImagePath, nType, 0, 0, LR_LOADFROMFILE|nFlags);
		if( !bNative )
			::DeleteFileA(strImagePath);
	}
	else
	{
		LPCWSTR strImagePath = strPath.data();
		hImage = ::LoadImageW(NULL, strImagePath, nType, 0, 0, LR_LOADFROMFILE|nFlags);
		if( !bNative )
			::DeleteFileW(strImagePath);
	}
	
	return hImage;
}

bool IsIconNeedToRepare(hIO pIo)
{
	DWORD dwVersion = GetVersion();
	if( (dwVersion & 0x80000000) || (DWORD)(LOBYTE(LOWORD(dwVersion))) != 4 )
		return false;

	BYTE count = 0, bits = 0;
	pIo->SeekRead(NULL, 4, &count, 1);
	if( count != 1 )
		return false;

	pIo->SeekRead(NULL, 0x24, &bits, 1);
	if( bits <= 8 )
		return false;

	return true;
}

CImageBase * CWinRoot::CreateImage(eImageType eType, hIO pIo, int nImageSizeX, int nImageNotStretch, int nStyle, CImageBase * pImage)
{
	if( eType == eImageImage )
	{
		CBitmapHndEx* pImage = new CBitmapHndEx();
		pImage->Init(::CreateCompatibleBitmap(GetDesktopDC(), nImageSizeX, nImageNotStretch), 0, false);
		return pImage;
	}

	if( !pIo )
		return NULL;
	
	tString strName;
	{
		cStringObj sName; sName.assign(pIo, pgOBJECT_FULL_NAME);
		m_pRoot->LocalizeStr(strName, sName.data(), sName.size());
	}
	
	bool bInited = true;
	bool bNew = !pImage;
	if( strstr(strName.c_str(), ".bmp") )
	{
		HBITMAP hBitmap = (HBITMAP)LoadImageFromIO(pIo, IMAGE_BITMAP, LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
		if( !hBitmap )
			return NULL;

		if( !pImage )
			pImage = new CBitmapHndEx();
		bInited = ((CBitmapHndEx*)pImage)->Init(hBitmap, nImageSizeX, !!(nStyle & CImageBase::Transparent));
	}
	else if( strstr(strName.c_str(), ".ico") )
	{
		HICON hIcon = (HICON)LoadImageFromIO(pIo, IMAGE_ICON, 0);
		if( !hIcon )
			return NULL;

		if( nImageSizeX )
		{
			if( !pImage )
				pImage  = new CIconList();
			bInited = ((CIconList*)pImage)->Init(hIcon, nImageSizeX);
		}
		else
		{
			if( !pImage )
				pImage = new CIconImpl();
			bInited = ((CIconImpl*)pImage)->Init(hIcon, true, IsIconNeedToRepare(pIo));
		}
	}
	else if( strstr(strName.c_str(), ".png") )
	{
		if( !pImage )
			pImage = new CImagePng();

		CImagePng* pImagePng = (CImagePng*)pImage;

		std::vector<tBYTE> pData;
		tQWORD nSize = 0; pIo->GetSize(&nSize, IO_SIZE_TYPE_EXPLICIT);
		pData.resize((unsigned)nSize);
		
		bInited = false;

		do
		{
			if( pData.empty() || PR_FAIL(pIo->SeekRead(NULL, 0, &pData[0], (tDWORD)pData.size())) )
				break;

			CImagePng Image;

			if (m_pRoot->m_dwScaleY != 96 && !nImageNotStretch)
				pImagePng = &Image;

			if (!(bInited = pImagePng->Init(&pData[0], (long)pData.size(), nImageSizeX, !!(nStyle & CImageBase::Mozaic)))
					|| m_pRoot->m_dwScaleY == 96 || nImageNotStretch)
				break;

//			assert(nImageSizeX == 0 || pImagePng->m_head.biWidth == nImageSizeX);

			CColorMatrix32 MatrixSrc;
			if(!MatrixSrc.Create(pImagePng->m_head.biHeight, pImagePng->m_head.biWidth))
				break;

			for(int iy = 0; iy<MatrixSrc.GetY(); ++iy)
			{
				for(int ix = 0; ix<MatrixSrc.GetX() && ix*3<pImagePng->m_info.dwEffWidth; ++ix)
				{
					MatrixSrc[iy][ix].B = *(pImagePng->m_info.pImage+iy*pImagePng->m_info.dwEffWidth+3*ix);
					MatrixSrc[iy][ix].G = *(pImagePng->m_info.pImage+iy*pImagePng->m_info.dwEffWidth+3*ix+1);
					MatrixSrc[iy][ix].R = *(pImagePng->m_info.pImage+iy*pImagePng->m_info.dwEffWidth+3*ix+2);
					MatrixSrc[iy][ix].A = pImagePng->AlphaGet(ix,iy);
				}
			}
			
//			assert(nImageSizeX==0 || pImagePng->m_head.biWidth%nImageSizeX==0);
			int nScaledWidth = nImageSizeX ?(ScaledSize(nImageSizeX)*pImagePng->m_head.biWidth)/nImageSizeX : ScaledSize(pImagePng->m_head.biWidth);
			if (!((CImagePng*)pImage)->CreateCompatibleImage(pImagePng, nScaledWidth, ScaledSize(pImagePng->m_head.biHeight), ScaledSize(nImageSizeX)))
				break;

			CColorMatrix32 MatrixDst;
			if(!MatrixDst.Create(((CImagePng*)pImage)->m_head.biHeight, ((CImagePng*)pImage)->m_head.biWidth))
				break;

			if (!SuperRescaleBitmapWithAlpha(MatrixSrc, NULL, MatrixDst, NULL))
				break;

			for(int iy = 0; iy<MatrixDst.GetY(); ++iy)
			{
				for(int ix = 0; ix<MatrixDst.GetX() && ix*3<((CImagePng*)pImage)->m_info.dwEffWidth; ++ix)
				{
					*(((CImagePng*)pImage)->m_info.pImage+iy*((CImagePng*)pImage)->m_info.dwEffWidth+3*ix) = MatrixDst[iy][ix].B;
					*(((CImagePng*)pImage)->m_info.pImage+iy*((CImagePng*)pImage)->m_info.dwEffWidth+3*ix+1) = MatrixDst[iy][ix].G;
					*(((CImagePng*)pImage)->m_info.pImage+iy*((CImagePng*)pImage)->m_info.dwEffWidth+3*ix+2) = MatrixDst[iy][ix].R;
					((CImagePng*)pImage)->AlphaSet(ix,iy, MatrixDst[iy][ix].A);
				}
			}

		}while(0);

	}

	if( bNew && !bInited && pImage )
		pImage->Destroy(), pImage = NULL;

	if( pImage )
		pImage->m_pRoot = this;

	return pImage;
}


//////////////////////////////////////////////////////////////////////
// CBorderImpl

class CBorderImpl : public CBorder, public CXPBrush
{
public:
	CBorderImpl(): m_hBr(NULL){}
	~CBorderImpl(){ Clear(); }

	void Destroy(){ delete this; };

	void Clear()
	{
		if( m_hBr )
			DeleteObject(m_hBr);
		m_hBr = NULL;

		CXPBrush::Clear();
	}

	bool Init(tDWORD nStyle, COLORREF clColor, tDWORD nWidth)
	{
		Clear();

		m_clColor = clColor;
		m_nStyle = nStyle;
		m_nWidth = nWidth;
		m_hBr = CreateSolidBrush(m_clColor);
		if( m_nStyle & CBorder::XPBrush )
			CXPBrush::Init(m_clColor, /*!!!*/RGB(255,255,255), true);
		return true;
	}

	bool Draw(HDC dc, RECT& rc, bool p_fCalcRect)
	{
		if( !m_hBr )
			return false;

		if( p_fCalcRect )
		{
			if( m_nStyle & CBorder::Left )    rc.left   += m_nWidth;
			if( m_nStyle & CBorder::Top )     rc.top    += m_nWidth;
			if( m_nStyle & CBorder::Right )   rc.right  -= m_nWidth;
			if( m_nStyle & CBorder::Bottom )  rc.bottom -= m_nWidth;
			return( true );
		}
		
		if( m_nStyle & CBorder::XPBrush )
		{
			RECT rcFrame = rc;
			InflateRect(&rcFrame, -1, -1);
			FrameRect(dc, &rcFrame, m_hFrame);

			if( m_nStyle & CBorder::Left )
				DrawLeftBorder(dc, rc);

			if( m_nStyle & CBorder::Top )
				DrawTopBorder(dc, rc);

			if( m_nStyle & CBorder::Right )
				DrawRightBorder(dc, rc);

			if( m_nStyle & CBorder::Bottom )
				DrawBottomBorder(dc, rc);
		}
		else
		{
			HGDIOBJ hOldBrush = SelectObject(dc, m_hBr);
			if( m_nStyle & CBorder::Left )
			{
				RECT l_rcLine = { rc.left, rc.top, rc.left + m_nWidth, rc.bottom };
				FillRect( dc, &l_rcLine, m_hBr );
			}
			if( m_nStyle & CBorder::Top )
			{
				RECT l_rcLine = { rc.left, rc.top, rc.right, rc.top + m_nWidth };
				FillRect( dc, &l_rcLine, m_hBr );
			}
			if( m_nStyle & CBorder::Right )
			{
				RECT l_rcLine = { rc.right - m_nWidth, rc.top, rc.right, rc.bottom };
				FillRect( dc, &l_rcLine, m_hBr );
			}
			if( m_nStyle & CBorder::Bottom )
			{
				RECT l_rcLine = { rc.left, rc.bottom - m_nWidth, rc.right, rc.bottom };
				FillRect( dc, &l_rcLine, m_hBr );
			}
			SelectObject(dc, hOldBrush);
		}
		return true;
	}

	tDWORD Width(){ return m_nWidth; }
	StyleFlags GetStyle(){ return (StyleFlags)m_nStyle; }

private:
	COLORREF  m_clColor;
	tDWORD    m_nStyle;
	tDWORD    m_nWidth;
	HBRUSH    m_hBr;
};

//////////////////////////////////////////////////////////////////////

CBorder* CWinRoot::CreateBorder()
{
	return new CBorderImpl();
}

//////////////////////////////////////////////////////////////////////

typedef HRESULT (STDAPICALLTYPE* defDrawThemeParentBackground)(HWND hwnd, HDC hdc, OPTIONAL RECT* prc);
typedef HRESULT (STDAPICALLTYPE* defDrawThemeBackground)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);

defDrawThemeBackground       g_fnDTB, *g_pfnDTB;
defDrawThemeParentBackground g_fnDTPB, *g_pfnDTPB;

CItemBase * g_pHookItem;
BOOL        g_hooked;

HRESULT STDAPICALLTYPE Hook_DrawThemeBackground(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect)
{
//	FillRect(hdc, pRect, g_pHookItem);
	return S_OK;
}

HRESULT STDAPICALLTYPE Hook_DrawThemeParentBackground(HWND hwnd, HDC hdc, OPTIONAL RECT* prc)
{
	if( g_pHookItem->GetWindow() != hwnd )
		return E_FAIL;

	if( FAILED(g_pHookItem->DrawParentBackground(hdc, prc)) )
		return g_fnDTPB(hwnd, hdc, prc);
	return S_OK;
}

void HookDrawBegin(CItemBase * pItem)
{
	if( !g_hooked )
	{
		*(void**)&g_pfnDTPB = GetProcDelayedPtr(GetModuleHandle("comctl32.dll"), "DrawThemeParentBackground");
		g_hooked = TRUE;
	}

	if( !g_pfnDTPB ) return;

	g_pHookItem = pItem;
	g_fnDTPB = *g_pfnDTPB;
	*g_pfnDTPB = Hook_DrawThemeParentBackground;
}

void HookDrawEnd()
{
	if( g_pfnDTPB )
		*g_pfnDTPB = g_fnDTPB;
	g_pHookItem = NULL;
}

//////////////////////////////////////////////////////////////////////

#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

typedef struct ImgDelayDescr {
        DWORD                grAttrs;     // attributes
        LPCSTR               Name;        // pointer to dll name
        HMODULE*             phmod;       // address of module handle
        DWORD*               pIAT;        // address of the IAT
        DWORD*               pINT;        // address of the INT
        DWORD                pBoundIAT;   // address of the optional bound IAT
        DWORD                pUnloadIAT;  // address of optional copy of original IAT
        DWORD                TimeDateStamp; // 0 if not bound,
                                          // O.W. date/time stamp of DLL bound to Old BIND
} *PIMAGE_IMPORT_DEALYED_DESCRIPTOR;

PVOID GetProcImportPtr(PVOID hModule, PCHAR FuncName)
{
	PIMAGE_DOS_HEADER pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	PIMAGE_NT_HEADERS pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	PIMAGE_IMPORT_DESCRIPTOR importDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR,0,pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if(importDesc==0)
		return 0;

	importDesc=MakePtr(PIMAGE_IMPORT_DESCRIPTOR,hModule,importDesc);

	PIMAGE_THUNK_DATA thunk, thunkIAT=0;
	PIMAGE_IMPORT_BY_NAME pOrdinalName;
	while(1) {
		// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
		if((importDesc->TimeDateStamp==0) && (importDesc->Name==0))
			break;

		thunk=MakePtr(PIMAGE_THUNK_DATA,0,importDesc->Characteristics);
		thunkIAT=MakePtr(PIMAGE_THUNK_DATA,0,importDesc->FirstThunk);
		if(thunk==0)
		{  // No Characteristics field?
			thunk = thunkIAT;// Yes! Gotta have a non-zero FirstThunk field then.
			if(thunk==0)   // No FirstThunk field?  Ooops!!!
				break;
		}
		thunk=MakePtr(PIMAGE_THUNK_DATA,hModule,thunk);
		thunkIAT=MakePtr(PIMAGE_THUNK_DATA,hModule,thunkIAT);
		while(1)
		{
			if(thunk->u1.AddressOfData==0)
				break;
			if(!(thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)) //Function has no name - only ordinal
			{
				pOrdinalName=MakePtr(PIMAGE_IMPORT_BY_NAME,hModule,thunk->u1.AddressOfData);
				if(strcmp(FuncName, (PCHAR)pOrdinalName->Name)==0)
				{
					DWORD dwOldProtection; 
					if( !VirtualProtect((PVOID)thunkIAT, sizeof(PVOID), PAGE_READWRITE, &dwOldProtection) )
						return 0;
					return (PVOID)thunkIAT;
				}
			}
			thunk++;
			thunkIAT++;
		}
		importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
	}
	return 0;
}

PVOID GetProcDelayedPtr(PVOID hModule, PCHAR FuncName)
{
	PIMAGE_DOS_HEADER pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	PIMAGE_NT_HEADERS pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	PIMAGE_IMPORT_DEALYED_DESCRIPTOR importDesc = MakePtr(PIMAGE_IMPORT_DEALYED_DESCRIPTOR,0,pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress);
	if(importDesc==0)
		return 0;

	importDesc=MakePtr(PIMAGE_IMPORT_DEALYED_DESCRIPTOR,hModule,importDesc);

	PDWORD thunk, thunkIAT=0;
	while(1) {
		// See if we've reached an empty IMAGE_IMPORT_DESCRIPTOR
		if((importDesc->TimeDateStamp==0) && (importDesc->Name==0))
			break;

		thunk=MakePtr(PDWORD,hModule,importDesc->pINT);
		thunkIAT=MakePtr(PDWORD,hModule,importDesc->pIAT);

		if( !thunk || !thunkIAT )
			continue;

		for(; *thunk && *thunkIAT; thunkIAT++, thunk++)
		{
			PCHAR szName = MakePtr(PCHAR,hModule,*thunk);
			if( (DWORD)szName & 0x80000000 )
				continue;

			szName += 2;
			if(strcmp(FuncName, szName)==0)
			{
				DWORD dwOldProtection; 
				if( !VirtualProtect(thunkIAT, sizeof(PVOID), PAGE_READWRITE, &dwOldProtection) )
					return 0;
				return thunkIAT;
			}
		}
		importDesc++;   // advance to next IMAGE_IMPORT_DESCRIPTOR
	}
	return 0;
}

HWND GetParentHWND(CItemBase* pItem, bool bTop)
{
	if( !pItem )
		return NULL;

	HWND hWnd = NULL;
	for(CItemBase* pOwner; pOwner = pItem->GetOwner(bTop); )
	{
		if( hWnd = pOwner->GetWindow() )
			break;

		if( !(pItem = pOwner->m_pParent) )
			break;
	}
	
	return hWnd;
}

//////////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport) void __stdcall ShellExecuter(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	ShellExecute(NULL, "open", lpszCmdLine, NULL, NULL, SW_SHOWNORMAL);
}

LONG GetRegDWORD(const wchar_t* szPath, const wchar_t* szValue, DWORD& bValue)
{
	HKEY key; 
	LONG res = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		szPath,
		0,
		KEY_READ,
		&key
		);
	if ( res == ERROR_SUCCESS )
	{
		DWORD type;
		DWORD size = sizeof(DWORD);
		bValue = TRUE;
		res = RegQueryValueExW(
			key,
			szValue,
			NULL,
			&type,
			(BYTE*)&bValue,
			&size
			);
		RegCloseKey(key);
	}
	return res;
}

LONG SetRegDWORD(const wchar_t* szPath, const wchar_t* szValue, DWORD bValue, BOOL bOverwrite)
{
	if ( !bOverwrite )
	{
		DWORD bOldValue = 0;
		if ( ERROR_SUCCESS == GetRegDWORD(szPath, szValue, bOldValue) )
			return ERROR_SUCCESS;
	}

	HKEY key; 
	LONG res = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		szPath,
		0,
		KEY_WRITE,
		&key
		);
	if ( res == ERROR_SUCCESS )
	{
		DWORD size = sizeof(DWORD);
		res = RegSetValueExW(
			key,
			szValue,
			NULL,
			REG_DWORD,
			(BYTE*)&bValue,
			sizeof(DWORD)
			);
		RegCloseKey(key);
	}
	return res;
}

LONG RemoveReg(const wchar_t* szPath, const wchar_t* szValue)
{
	HKEY key; 
	LONG res = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		szPath,
		0,
		KEY_WRITE,
		&key
		);
	if ( res == ERROR_SUCCESS )
	{
		RegDeleteValueW(key, szValue);
		RegCloseKey(key);
	}
	return res;
}

tERROR SendSrvMgrRequest(LPCWSTR strService, tSrvMgrRequest nRequest)
{
	CLibWrapper l_Advapi32(_T("advapi32.dll"));
	SC_HANDLE (WINAPI *_OpenSCManagerW)(LPCWSTR lpMachineName, LPCWSTR lpDatabaseName, DWORD dwDesiredAccess);
	SC_HANDLE (WINAPI *_OpenServiceW)(SC_HANDLE hSCManager, LPCWSTR lpServiceName, DWORD dwDesiredAccess);
	BOOL      (WINAPI *_ChangeServiceConfigW)(SC_HANDLE hService, DWORD dwServiceType, DWORD dwStartType, DWORD dwErrorControl, LPCWSTR lpBinaryPathName, LPCWSTR lpLoadOrderGroup, LPDWORD lpdwTagId, LPCWSTR lpDependencies, LPCWSTR lpServiceStartName, LPCWSTR lpPassword, LPCWSTR lpDisplayName);
	BOOL      (WINAPI *_ControlService)(SC_HANDLE hService, DWORD dwControl, LPSERVICE_STATUS lpServiceStatus);
	BOOL      (WINAPI *_StartServiceW)(SC_HANDLE hService, DWORD dwNumServiceArgs, LPCWSTR *lpServiceArgVectors);
	BOOL      (WINAPI *_CloseServiceHandle)(SC_HANDLE hSCObject);
	BOOL      (WINAPI *_QueryServiceConfigW)(SC_HANDLE hService, LPQUERY_SERVICE_CONFIG lpServiceConfig, DWORD cbBufSize, LPDWORD pcbBytesNeeded);

	tBOOL bSucc = TRUE;
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_OpenSCManagerW,       "OpenSCManagerW");
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_OpenServiceW,         "OpenServiceW");
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_ChangeServiceConfigW, "ChangeServiceConfigW");
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_ControlService,       "ControlService");
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_StartServiceW,        "StartServiceW");
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_CloseServiceHandle,   "CloseServiceHandle");
	if( bSucc ) bSucc = l_Advapi32.GetFunc((FARPROC *)&_QueryServiceConfigW,  "QueryServiceConfigW");
	if( !bSucc )
		return errNOT_FOUND;

	SC_HANDLE hSM = _OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT);
	if( !hSM )
		return errUNEXPECTED;
		
	DWORD dwAccess = 0;
	switch(nRequest)
	{
		case SRVMGR_CHECK_ENABLE: dwAccess = SERVICE_QUERY_CONFIG; break;
		case SRVMGR_ENABLE:       dwAccess = SERVICE_QUERY_CONFIG|SERVICE_START|SERVICE_CHANGE_CONFIG; break;
		case SRVMGR_DISABLE:      dwAccess = SERVICE_QUERY_CONFIG|SERVICE_STOP|SERVICE_CHANGE_CONFIG; break;
		case SRVMGR_RESTORE:      dwAccess = SERVICE_QUERY_CONFIG|SERVICE_START|SERVICE_STOP|SERVICE_CHANGE_CONFIG; break;
	}
	if( !dwAccess )
		return errNOT_SUPPORTED;

	SC_HANDLE hS = _OpenServiceW(hSM, strService, dwAccess);
	if( !hS )
	{
		_CloseServiceHandle(hSM);
		return errNOT_FOUND;
	}

	char pBuffer[0x2000];
	DWORD nBytesNeded;
	LPQUERY_SERVICE_CONFIG pCfg = (LPQUERY_SERVICE_CONFIG)pBuffer;
	bSucc = _QueryServiceConfigW(hS, pCfg, sizeof(pBuffer), &nBytesNeded);
	
	if( nRequest == SRVMGR_CHECK_ENABLE )
	{
		bSucc = bSucc ? pCfg->dwStartType != SERVICE_DISABLED : FALSE;
	}
	else
	if( nRequest == SRVMGR_ENABLE || nRequest == SRVMGR_DISABLE )
	{
		DWORD dwOldEnabledState = pCfg->dwStartType;
		tStringW szPath = L"SYSTEM\\CurrentControlSet\\Services\\"; szPath += strService;
		SetRegDWORD(szPath.c_str(), L"EnabledBefore", dwOldEnabledState, FALSE);
		bSucc = _ChangeServiceConfigW(hS, SERVICE_NO_CHANGE, nRequest == SRVMGR_ENABLE ? SERVICE_AUTO_START : SERVICE_DISABLED, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
		if( nRequest == SRVMGR_ENABLE )
		{
			bSucc = _StartServiceW(hS, 0, NULL);
			if( !bSucc )
				bSucc = GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
		}
		else
		{
			SERVICE_STATUS nStatus;
			bSucc = _ControlService(hS, SERVICE_CONTROL_STOP, &nStatus);
			if( !bSucc )
				bSucc = GetLastError() == ERROR_SERVICE_NOT_ACTIVE;
		}
	}
	else
	if( nRequest == SRVMGR_RESTORE )
	{
		DWORD dwOldEnabledState = SERVICE_BOOT_START;
		tStringW szPath = L"SYSTEM\\CurrentControlSet\\Services\\"; szPath += strService;
		if ( ERROR_SUCCESS == GetRegDWORD(szPath.c_str(), L"EnabledBefore", dwOldEnabledState))
		{
			bSucc = _ChangeServiceConfigW(hS, SERVICE_NO_CHANGE, dwOldEnabledState, SERVICE_NO_CHANGE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
			if( dwOldEnabledState != SERVICE_DISABLED )
			{
				bSucc = _StartServiceW(hS, 0, NULL);
				if( !bSucc )
					bSucc = GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
			}
			else
			{
				SERVICE_STATUS nStatus;
				bSucc = _ControlService(hS, SERVICE_CONTROL_STOP, &nStatus);
				if( !bSucc )
					bSucc = GetLastError() == ERROR_SERVICE_NOT_ACTIVE;
			}
			RemoveReg(szPath.c_str(), L"EnabledBefore");
		}
	}

	_CloseServiceHandle(hS);
	_CloseServiceHandle(hSM);

	return bSucc ? errOK : errNOT_OK;
}

BOOL _EnableFSRedirector(BOOL bEnable)
{
	if( bEnable )
	{
		typedef BOOL (WINAPI *LPFN_Wow64EnableWow64FsRedirection)(BOOL Wow64FsEnableRedirection);
		LPFN_Wow64EnableWow64FsRedirection fnWow64EnableWow64FsRedirection = NULL;
		fnWow64EnableWow64FsRedirection = (LPFN_Wow64EnableWow64FsRedirection)GetProcAddress(GetModuleHandle("kernel32"), "Wow64EnableWow64FsRedirection");

		if (fnWow64EnableWow64FsRedirection)
		{
			if (fnWow64EnableWow64FsRedirection(TRUE))
				return TRUE;
		}
	}
	else
	{
		typedef BOOL (WINAPI *LPFN_Wow64DisableWow64FsRedirection) (PVOID* OldValue);
		LPFN_Wow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection = NULL;
		fnWow64DisableWow64FsRedirection = (LPFN_Wow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle("kernel32"), "Wow64DisableWow64FsRedirection");

		if (fnWow64DisableWow64FsRedirection)
		{
			PVOID pOldValue;
			if (fnWow64DisableWow64FsRedirection(&pOldValue))
				return TRUE;
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// cRedirectedStdio

class cRedirectedStdio
{
public:
	cRedirectedStdio() : 
		m_hChildProcess(NULL),
		m_hChildThread(NULL),
		m_hChildStdoutRd(NULL),
		m_hChildStdoutWr(NULL),
		m_hChildStdinRd(NULL),
		m_hChildStdinWr(NULL)
	{}
	~cRedirectedStdio()
	{
		Cleanup();
	}

public:
	tBOOL  CreateProcess(LPWSTR strCmdLine, LPCWSTR strBaseFolder);
	void   WaitForProcessStop();
	void   TerminateProcess();
	
	tDWORD GetReadSize();
	tBOOL  Read(tPTR pBuffer, tDWORD nBytesToRead, tDWORD *pnBytesRead = NULL);
	tBOOL  Write(tPTR pBuffer, tDWORD nBytesToWrite, tDWORD *pnBytesToWritten = NULL);
	tBOOL  IsActive();

protected:
	void   Cleanup();
	
	HANDLE m_hChildProcess;
	HANDLE m_hChildThread;
	HANDLE m_hChildStdoutRd;
	HANDLE m_hChildStdoutWr;
	HANDLE m_hChildStdinRd;
	HANDLE m_hChildStdinWr;
};

tBOOL cRedirectedStdio::CreateProcess(LPWSTR strCmdLine, LPCWSTR strBaseFolder)
{
	tBOOL res = cTRUE;
	
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	HANDLE hCurProc = ::GetCurrentProcess();

	if( res && (res = ::CreatePipe(&m_hChildStdinRd, &m_hChildStdinWr, &saAttr, 0)) )
	{
		HANDLE hChildStdinWrDup;
		::DuplicateHandle(hCurProc, m_hChildStdinWr, hCurProc, &hChildStdinWrDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
		::CloseHandle(m_hChildStdinWr);
		m_hChildStdinWr = hChildStdinWrDup;
	}

	if( res && (res = ::CreatePipe(&m_hChildStdoutRd, &m_hChildStdoutWr, &saAttr, 0)) )
	{
		HANDLE hChildStdoutRdDup;
		::DuplicateHandle(hCurProc, m_hChildStdoutRd, hCurProc, &hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
		::CloseHandle(m_hChildStdoutRd);
		m_hChildStdoutRd = hChildStdoutRdDup;
	}

	if( res )
	{
		PROCESS_INFORMATION pi = {0, };
		STARTUPINFOW si = {sizeof(STARTUPINFOW), 0, };
		si.hStdInput = m_hChildStdinRd;
		si.hStdOutput = m_hChildStdoutWr;
		si.hStdError = m_hChildStdoutWr;
		si.dwFlags |= STARTF_USESTDHANDLES;

		if( res = ::CreateProcessW(NULL, (LPWSTR)strCmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, strBaseFolder, &si, &pi) )
		{
			m_hChildProcess = pi.hProcess;
			m_hChildThread = pi.hThread;
		}
	}

	if( !res )
		Cleanup();

	return res;
}

void cRedirectedStdio::WaitForProcessStop()
{
	if( m_hChildProcess )
		::WaitForSingleObject(m_hChildProcess, INFINITE);
}

void cRedirectedStdio::Cleanup()
{
	::CloseHandle(m_hChildStdinRd);  m_hChildStdinRd = NULL;
	::CloseHandle(m_hChildStdinWr);  m_hChildStdinWr = NULL;
	::CloseHandle(m_hChildStdoutRd); m_hChildStdoutRd = NULL;
	::CloseHandle(m_hChildStdoutWr); m_hChildStdoutWr = NULL;
	::CloseHandle(m_hChildProcess);  m_hChildProcess = NULL;
	::CloseHandle(m_hChildThread);   m_hChildThread = NULL;
}

void cRedirectedStdio::TerminateProcess()
{
	if( m_hChildProcess )
		::TerminateProcess(m_hChildProcess, 0);
	Cleanup();
}

tBOOL cRedirectedStdio::IsActive()
{
	if( !m_hChildProcess )
		return cFALSE;

	DWORD nExitCode = 0;
	::GetExitCodeProcess(m_hChildProcess, (PDWORD)&nExitCode);
	return nExitCode == STILL_ACTIVE;
}

tDWORD cRedirectedStdio::GetReadSize()
{
	tDWORD nSize = 0;
	if( ::PeekNamedPipe(m_hChildStdoutRd, NULL, 0, NULL, (PDWORD)&nSize, NULL ) )
		return nSize;
	return 0;
}

tBOOL cRedirectedStdio::Read(tPTR pBuffer, tDWORD nBytesToRead, tDWORD *pnBytesRead)
{
	if( pnBytesRead )
		*pnBytesRead = 0;

	tDWORD nSize = 0;
	if( ::ReadFile(m_hChildStdoutRd, pBuffer, nBytesToRead, (PDWORD)&nSize, NULL) )
	{
		if( pnBytesRead )
			*pnBytesRead = nSize;
		return TRUE;
	}
	
	return FALSE;
}

tBOOL cRedirectedStdio::Write(tPTR pBuffer, tDWORD nBytesToWrite, tDWORD *pnBytesToWritten)
{
	if( pnBytesToWritten )
		*pnBytesToWritten = 0;

	tDWORD nSize = 0;
	if( ::WriteFile(m_hChildStdinWr, pBuffer, nBytesToWrite, (PDWORD)&nSize, NULL) )
	{
		if( pnBytesToWritten )
			*pnBytesToWritten = nSize;
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ResolveShortcat

bool ResolveShortcat(cStringObj &strShortcat)
{
 	WCHAR strBaseguiPath[MAX_PATH];
	::GetModuleFileNameW(::GetModuleHandleW(L"basegui.ppl"), strBaseguiPath, countof(strBaseguiPath));
	wcscpy(wcsrchr(strBaseguiPath, L'\\') + 1, L"x64");

	cRedirectedStdio shortcatResolver;

	_EnableFSRedirector(FALSE);
	WCHAR strCmdLine[MAX_PATH];
	::GetSystemDirectoryW(strCmdLine, MAX_PATH);
	wcscat(strCmdLine, L"\\rundll32.exe basegui.ppl,ShortcatResolver");
	tBOOL b = shortcatResolver.CreateProcess(strCmdLine, strBaseguiPath);
	_EnableFSRedirector(TRUE);
	if( !b )
		return false;

	if( shortcatResolver.Write((tPTR)strShortcat.data(), (strShortcat.size() + 1) * sizeof(WCHAR)) )
	{
		shortcatResolver.WaitForProcessStop();

		WCHAR buffer[0x4000];
		if( shortcatResolver.GetReadSize() && shortcatResolver.Read(buffer, countof(buffer)) )
		{
			strShortcat = buffer;
			return true;
		}
	}

	return false;
}

// other process

extern "C" __declspec(dllexport) void __stdcall ShortcatResolver(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	DWORD nBytes;
	WCHAR strShortcat[0x4000];
	if( !::ReadFile(::GetStdHandle(STD_INPUT_HANDLE), strShortcat, sizeof(strShortcat), &nBytes, NULL) )
		return;

	::CoInitialize(NULL);

	IShellLinkW * pIShellLink = NULL;
	static const GUID CLSID_IShellLink = { 0x00021401, 0x0000, 0x0000, { 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 } };
	if( SUCCEEDED(::CoCreateInstance(CLSID_IShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (PVOID *)&pIShellLink)) )
	{
		IPersistFile * pIPersistFile = NULL;
		if( SUCCEEDED(pIShellLink->QueryInterface( IID_IPersistFile, (PVOID *)&pIPersistFile)) )
		{
			if( SUCCEEDED(pIPersistFile->Load(strShortcat, STGM_READ)) )
			{
				if( SUCCEEDED(pIShellLink->GetPath(strShortcat, countof(strShortcat), NULL, (SLGP_RAWPATH|0xFFFFFFFF)&~(SLGP_SHORTPATH))) )
				{
					::WriteFile(::GetStdHandle(STD_OUTPUT_HANDLE), strShortcat, (wcslen(strShortcat) + 1) * sizeof(WCHAR), &nBytes, NULL);
				}
			}
			pIPersistFile->Release();
		}
		pIShellLink->Release();
	}

	::CoUninitialize();
}


//////////////////////////////////////////////////////////////////////
// ShowContextHelp

#define HELP_WINDOWNAME "AVP.Help.HiddenWindow"
#define HELP_BUFFERSIZE 2000

union cHelpInfo
{
public:
	struct
	{
		DWORD   dwTopicId;
		WCHAR   strHelpFile[HELP_BUFFERSIZE];
	};
	struct
	{
		HRESULT hrError;
		WCHAR   strErrorDescription[HELP_BUFFERSIZE];
	};
};

HANDLE DuplicateHandleByHwnd(HANDLE hSrc, HWND hWnd)
{
	if( !hSrc || !hWnd )
		return NULL;

	DWORD nHelpProcessID;
	if( !::GetWindowThreadProcessId(hWnd, &nHelpProcessID) )
		return NULL;

	HANDLE hHelpProcess = ::OpenProcess(PROCESS_DUP_HANDLE, FALSE, nHelpProcessID);
	if( !hHelpProcess )
		return NULL;

	HANDLE hDuplicate = NULL;
	if( !::DuplicateHandle(::GetCurrentProcess(), hSrc, hHelpProcess, &hDuplicate, 0, FALSE, DUPLICATE_SAME_ACCESS) )
		hDuplicate = NULL;

	::CloseHandle(hHelpProcess);

	return hDuplicate;
}

BOOL ShowContextHelp(CRootItem *pRoot, tDWORD nTopicId)
{
	cStringObj strHelpFileName;
	((CRootSink *)pRoot->m_pSink)->GetHelpStorageId(strHelpFileName);

	if( strHelpFileName.size() >= HELP_BUFFERSIZE )
		return 0;

	HWND hWndHelper = ::FindWindow(NULL, HELP_WINDOWNAME);
	if( !hWndHelper )
	{
		CHAR strBaseguiPath[MAX_PATH];
		::GetModuleFileName(GetModuleHandle("basegui.ppl"), strBaseguiPath, countof(strBaseguiPath));
		
		CHAR *pLastSlash = strrchr(strBaseguiPath, '\\');
		if( pLastSlash )
			*pLastSlash = 0;

		PROCESS_INFORMATION pi = {0, };
		STARTUPINFO si = {sizeof(STARTUPINFO), 0, };
		CHAR strCmdLine[] = "rundll32.exe basegui.ppl,ShowHelp";
		if( ::CreateProcess(NULL, strCmdLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, strBaseguiPath, &si, &pi) )
		{
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
		}
		else
			return 0;
		
		for(int i = 0; !hWndHelper && i < 20; i++, Sleep(50))
			hWndHelper = ::FindWindow(NULL, HELP_WINDOWNAME);
	}
	if( !hWndHelper )
		return 0;

	HANDLE hFile = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(cHelpInfo), NULL);
	if( !hFile )
		return 0;
	HANDLE hFileDup = DuplicateHandleByHwnd(hFile, hWndHelper);
	if( !hFileDup )
	{
		::CloseHandle(hFile);
		return 0;
	}
	
	cHelpInfo &hi = *(cHelpInfo *)::MapViewOfFile(hFile, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, sizeof(cHelpInfo));
	hi.dwTopicId = nTopicId;
	wcscpy(hi.strHelpFile, strHelpFileName.data());
	
	DWORD_PTR lResult = S_OK;
	if( ::SendMessageTimeout(hWndHelper, WM_ONCTXHELP, (WPARAM)hFileDup, 0, SMTO_BLOCK, 10000, &lResult) &&
		lResult != S_OK )
	{
		cGuiParams _ErrText;
		_ErrText.m_strVal1 = hi.strErrorDescription;
		if( !_ErrText.m_strVal1.empty() )
			pRoot->ShowMsgBox(pRoot, "HelpErrorText", "HelpError", MB_OK|MB_ICONEXCLAMATION, &_ErrText);
	}
	::UnmapViewOfFile(&hi);
	
	::CloseHandle(hFile);

	return 0;
}

// other process

//////////////////////////////////////////////////////////////////////
// CHelpShower

#include <htmlhelp.h>

#undef SubclassWindow
#undef UnsubclassWindow

class CHelpSubclasser : public CWindowImpl<CHelpSubclasser, CWindow, CWinTraits<0, 0> >
{
protected:
	BEGIN_MSG_MAP(CHelpSubclasser)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()
	
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostQuitMessage(0);
		return bHandled = FALSE, 0;
	}
};

class CHelpShower : public CWindowImpl<CHelpShower, CWindow, CWinTraits<0, 0> >
{
public:
	typedef CWindowImpl<CHelpShower, CWindow, CWinTraits<0, 0> > TBase;
	
	CHelpShower() : m_dwHelpCookie(0) {}
	~CHelpShower()
	{
		if( ::IsWindow(m_hWnd) )
			DestroyWindow();
	}

protected:
	BEGIN_MSG_MAP(CHelpShower)
		MESSAGE_HANDLER(WM_ONCTXHELP, OnContextHelp)
	END_MSG_MAP()

	LRESULT OnContextHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HRESULT ShowContextHelp(cHelpInfo &pHelpInfo);

public:
	void    DoWork();

protected:
	DWORD           m_dwHelpCookie;
	CHelpSubclasser m_HelpWnd;
};

LRESULT CHelpShower::OnContextHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HRESULT hr = E_INVALIDARG;
	HANDLE hFile = (HANDLE)wParam;
	if( hFile )
	{
		cHelpInfo &hi = *(cHelpInfo *)::MapViewOfFile(hFile, FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, sizeof(cHelpInfo));
		if( &hi )
		{
			hr = ShowContextHelp(hi);

			::UnmapViewOfFile(&hi);
		}
		::CloseHandle(hFile);
	}
	return bHandled = TRUE, hr;
}

HRESULT CHelpShower::ShowContextHelp(cHelpInfo &pHelpInfo)
{
	if( !pHelpInfo.dwTopicId )
		return *pHelpInfo.strErrorDescription = 0, E_INVALIDARG;

	if( !m_dwHelpCookie )
	{
		HH_GLOBAL_PROPERTY stProp;
		stProp.id = HH_GPROPID_SINGLETHREAD;
		stProp.var.vt = VT_BOOL;
		stProp.var.boolVal = VARIANT_TRUE;
		HtmlHelp(NULL, NULL, HH_SET_GLOBAL_PROPERTY, (DWORD_PTR)&stProp);
		
		HtmlHelp(NULL, NULL, HH_INITIALIZE, (DWORD_PTR)&m_dwHelpCookie);
	}

	USES_CONVERSION;
	HWND hHelpWnd = HtmlHelp(NULL, W2A(pHelpInfo.strHelpFile), HH_HELP_CONTEXT, pHelpInfo.dwTopicId);

	HRESULT hr = S_OK;
	*pHelpInfo.strErrorDescription = 0;
	
	if( hHelpWnd )
	{
		if( m_HelpWnd.IsWindow() )
			m_HelpWnd.UnsubclassWindow();
		m_HelpWnd.SubclassWindow(hHelpWnd);

		::SetForegroundWindow(m_HelpWnd);
		::SetActiveWindow(m_HelpWnd);
	}
	else
	{
		struct HH_LAST_ERROR
		{
			 int      cbStruct;
			 HRESULT  hr;
			 BSTR     description;
		} hhinfo;

		memset(&hhinfo,0, sizeof(hhinfo)); hhinfo.cbStruct = sizeof(hhinfo);
		HtmlHelp(NULL, NULL, HH_GET_LAST_ERROR, (DWORD_PTR)&hhinfo);
		if( hhinfo.description )
		{
			pHelpInfo.hrError = hr = hhinfo.hr;
			wcscpy(pHelpInfo.strErrorDescription, hhinfo.description);
			::SysFreeString(hhinfo.description);
		}

		PostQuitMessage(0);
	}
	return hr;
}

void CHelpShower::DoWork()
{
	if( !Create(::GetDesktopWindow(), CWindow::rcDefault, HELP_WINDOWNAME, WS_POPUP, 0, 0U, NULL) )
		return;

    MSG msg; 
    while(true) 
    {
		BOOL bRes = ::GetMessage(&msg, NULL, 0, 0);
		if( !bRes || bRes == -1 )
			break;

		if( m_dwHelpCookie && HtmlHelp(NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD_PTR)&msg) )
			continue;

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
    }

	if( m_dwHelpCookie )
	{
		HtmlHelp(NULL, NULL, HH_UNINITIALIZE, (DWORD_PTR)m_dwHelpCookie);
		m_dwHelpCookie = 0;
	}
}

extern "C" __declspec(dllexport) void __stdcall ShowHelp(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	CHelpShower().DoWork();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
