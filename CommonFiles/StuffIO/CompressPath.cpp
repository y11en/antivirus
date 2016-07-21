#include <tchar.h>
#include <malloc.h>
#include <StuffIO\Utils.h>

#if !defined(_countof)
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

struct CLimitInfo {
	int m_nLenLimit;
	HDC m_hDC;
};

typedef bool (*CheckStrFunc)( const TCHAR *pStr, CLimitInfo &rcLimitInfo );

// ---
static TCHAR *FindDivider( const TCHAR *pSource ) {
  TCHAR *pDiv = _tcschr( (TCHAR *)pSource, L'\\' );
	if ( !pDiv )
		pDiv = _tcschr( (TCHAR *)pSource, L'/' );
	return pDiv;
}


#define ELLIPSE _T("...")

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
				TCHAR *p  = _tcschr( pStr, L':' );
				if ( p ) 
					p = _tcsinc(p);
				else { 
					p = _tcsstr( pStr, _T("\\\\") );
					if ( p ) {
						p = _tcsninc(p, 2);
						p = ::FindDivider( p );
					}
					else
						p = pStr;
				}  

				TCHAR *p1 = ::FindDivider( p = _tcsinc(p) );

				size_t nElLen = _tcsclen( ELLIPSE );
				if ( p1 ) {
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

