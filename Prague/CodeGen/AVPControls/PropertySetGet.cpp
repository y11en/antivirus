
#include "stdafx.h"
#include <tchar.h>
#include <property/property.h>
#include "WASCRes.rh"
#include <avpprpid.h>
#include <Stuff\CPointer.h>
#include <StuffIO\Utils.h>
#include "PropertySetGet.h"

#define VALID_STR(a) (a && *a)

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


// ---
static int GetConvertBase( const TCHAR *pS ) {
	int nBase = -1;
	if ( pS && *pS ) {
		while ( _istspace(_tcsnextc(pS)) || _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
			pS = _tcsinc(pS);
		if ( _tcsnextc(pS) == L'0' ) {
			int nNext = _tcsnextc( _tcsninc(pS, 1) );
			if ( nNext == L'x' || nNext == L'X' )
				return 16;
			nBase = 8;
			while ( _tcsnextc(pS) != L'\0' ) {
				int nNext = _tcsnextc( pS );
				if ( nNext >= L'8' && nNext <= L'9' ) {
					if ( nBase < 10 )
						nBase = 10;
				}
				if ( (nNext >= L'a' && nNext <= L'f') || (nNext >= L'A' && nNext <= L'F') ) 
					nBase = 16;
				pS = _tcsinc(pS);
			}
			return nBase;
		}
		nBase = 10;
		while ( _tcsnextc(pS) != L'\0' ) {
			int nNext = _tcsnextc( pS );
			if ( _istdigit(nNext) ) {
				if ( nBase < 10 )
					nBase = 10;
			}
			if ( (nNext >= L'a' && nNext <= L'f') || (nNext >= L'A' && nNext <= L'F') ) 
				nBase = 16;
			pS++;
		}
	}
	return nBase;
}

// ---
static long A2I( const TCHAR *pS, TCHAR **pEndPtr = NULL ) {
	if ( pEndPtr )
		*pEndPtr = (TCHAR *)pS;
  while ( _istspace(_tcsnextc(pS)) )
		pS = _tcsinc(pS);
	int nBase = ::GetConvertBase( pS );
	if ( nBase > 0 )
		return _tcstol( pS, pEndPtr, nBase );
	return 0;
}

// ---
static unsigned long A2UI( const TCHAR *pS, TCHAR **pEndPtr = NULL ) {
	if ( pEndPtr )
		*pEndPtr = (TCHAR *)pS;
  while ( _istspace(_tcsnextc(pS)) )
		pS = _tcsinc(pS);
	int nBase = ::GetConvertBase( pS );
	if ( nBase > 0 )
		return _tcstoul( pS, pEndPtr, nBase );
	return 0;
}

// ---
static bool IsHexDigit( int c ) {
	if ( !_istdigit(c) )
		return tolower(c) >= L'a' && tolower(c) <= L'f';
	else
		return true;
}

// ---
static __int64  A2I64( const TCHAR *pS, TCHAR **ppLast, int nBase ) {
	__int64 iR = 0;
	if ( pS && *pS ) {
		while ( _istspace(_tcsnextc(pS)) )
			pS = _tcsinc(pS);
		if ( nBase == 16 ) {
			while ( _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
				pS = _tcsinc(pS);
			if ( _tcsnextc(pS) == L'0' && (_tcsnextc(_tcsinc(pS)) == L'x' || _tcsnextc(_tcsinc(pS)) == L'X') )
				pS = _tcsninc(pS, 2);
			while( ::IsHexDigit(_tcsnextc(pS)) ) {
				if ( !_istdigit(_tcsnextc(pS)) )
					iR = 16 * iR + (tolower(_tcsnextc(pS)) - L'a' + 10);
				else
					iR = 16 * iR + (_tcsnextc(pS) - L'0');
				pS = _tcsinc(pS);
			}
		}
		else {
			if ( nBase == 10 ) {
				int c;              /* current TCHAR */
				int sign;           /* if '-', then negative, otherwise positive */

				c = _tcsnextc(pS);
				sign = c;           /* save sign indication */
				if (c == L'-' || c == L'+') {
					c = _tcsnextc(pS = _tcsinc(pS));    /* skip sign */
				}

				while ( _istdigit(c) ) {
					iR = 10 * iR + (c - L'0');     /* accumulate digit */
					c = _tcsnextc(pS = _tcsinc(pS));    /* get next TCHAR */
				}

				if (sign == L'-')
					iR = -iR;
			}
			else {
				if ( nBase == 8 ) {
					while ( _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
						pS = _tcsinc(pS);
					pS = _tcsinc(pS);
					while( _istdigit(_tcsnextc(pS)) ) {
						iR = 8 * iR + (_tcsnextc(pS) - L'0');
						pS = _tcsinc(pS);
					}
				}
			}
		}
	}
	if ( ppLast )
		*ppLast = (TCHAR *)pS;
	return iR;
}

// ---
static unsigned __int64  A2UI64( const TCHAR *pS, TCHAR **ppLast, int nBase ) {
	unsigned __int64 iR = 0;
	if ( pS && *pS ) {
		while ( _istspace(_tcsnextc(pS)) )
			pS = _tcsinc(pS);
		if ( nBase == 16 ) {
			while ( _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
				pS = _tcsinc(pS);
			if ( _tcsnextc(pS) == L'0' && (_tcsnextc(_tcsinc(pS)) == L'x' || _tcsnextc(_tcsinc(pS)) == L'X') )
				pS = _tcsninc(pS, 2);

			while( ::IsHexDigit(_tcsnextc(pS)) ) {
				if ( !_istdigit(_tcsnextc(pS)) )
					iR = 16 * iR + (tolower(_tcsnextc(pS)) - L'a' + 10);
				else
					iR = 16 * iR + (_tcsnextc(pS) - L'0');
				pS = _tcsinc(pS);
			}
		}
		else {
			if ( nBase == 10 ) {
				int c;              /* current TCHAR */
				int sign;           /* if '-', then negative, otherwise positive */

				c = _tcsnextc(pS);
				sign = c;           /* save sign indication */
				if (c == L'-' || c == L'+')
					c = _tcsnextc(pS = _tcsinc(pS));    /* skip sign */

				while ( _istdigit(c) ) {
					iR = 10 * iR + (c - L'0');     /* accumulate digit */
					c = _tcsnextc(pS = _tcsinc(pS));    /* get next TCHAR */
				}
				
				if ( sign == L'-' )
					iR *= -1;//-iR;
				
			}
			else {
				if ( nBase == 8 ) {
					while ( _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
						pS = _tcsinc(pS);
					pS = _tcsinc(pS);
					while( _istdigit(_tcsnextc(pS)) ) {
						iR = 8 * iR + (_tcsnextc(pS) - L'0');
						pS = _tcsinc(pS);
					}
				}
			}
		}
	}
	if ( ppLast )
		*ppLast = (TCHAR *)pS;
	return iR;
}

// ---
static AVP_byte *Str2Bin( const TCHAR *pStr, AVP_dword *pdwSize ) {
	AVP_byte *pValue = NULL;
	if ( pStr ) {
		AVP_dword dwSize = 0;
		TCHAR *pBegin = (TCHAR *)pStr;
		TCHAR *pEnd;
		while ( *pBegin && !_istdigit(_tcsnextc(pBegin)) )
			pBegin = _tcsinc(pBegin);
		AVP_byte nValue = (AVP_byte)::A2UI( pBegin, &pEnd );
		while( pEnd != pBegin ) {
			AVP_byte *pNewValue = new AVP_byte[dwSize + 1];
			if ( pValue )
				memcpy( pNewValue, pValue, dwSize );

			*(pNewValue + dwSize) = nValue;
			dwSize++;

			delete [] pValue;
			pValue = pNewValue;

			pBegin = pEnd;
			while ( *pBegin && !_istdigit(_tcsnextc(pBegin)) )
				pBegin = _tcsinc(pBegin);
			nValue = (AVP_byte)::A2UI( pBegin, &pEnd );
		}
		*pdwSize = dwSize;
	}
	return pValue;
}

// ---
static TCHAR *Bin2Str( AVP_byte *pValue, AVP_dword dwSize ) {
	TCHAR *pStr = NULL;
	if ( pValue ) {
		for ( AVP_dword i=0; i<dwSize; i++ ) {
			TCHAR pBuff[10];
			wsprintf( pBuff, _T("%#x"), pValue[i] );

			TCHAR *pNewStr = new TCHAR[((pStr ? TCSCLEN(pStr) : 0) + TCSCLEN(pBuff) + 2)];
			*pNewStr = 0;

			if ( pStr ) {
				_tcscpy( pNewStr, pStr );
				_tcscat( pNewStr, _T(" ") );
			}
			_tcscat( pNewStr, pBuff );

			delete []	pStr;
			pStr = pNewStr;
		}
	}
	return pStr;
}



// ---
bool CheckBinaryValue( const TCHAR *pStr ) {
	TCHAR *pBegin = (TCHAR *)pStr;
	TCHAR *pEnd;
	while ( *pBegin && !_istdigit(_tcsnextc(pBegin)) )
		pBegin = _tcsinc(pBegin);
	int nValue = ::A2I( pBegin, &pEnd );
	while( pEnd != pBegin ) {
		if ( nValue < CHAR_MIN || nValue > CHAR_MAX )
			return false;
		pBegin = pEnd;
		while ( *pBegin && !_istdigit(_tcsnextc(pBegin)) )
			pBegin = _tcsinc(pBegin);
		nValue = ::A2I( pBegin, &pEnd );
	}
	return true;
}

// ---
int GetDataDatasCount( HDATA hData ) {
	int nCount = 0;
	HDATA hCurrData = ::DATA_Get_First( hData, NULL );
	while ( hCurrData ) {
		nCount++;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return nCount;
}

// ---
int GetDataPropsCount( HDATA hData ) {
	int nCount = 0;
	HPROP hCurrProp = ::DATA_Get_First_Prop( hData, NULL );
	while ( hCurrProp ) {
		nCount++;
		hCurrProp = ::PROP_Get_Next( hCurrProp );
	}
	return nCount;
}

// ---
TCHAR *GetDataPropertyString( HDATA hData, AVP_dword nPID ) {
	TCHAR *pStr = NULL;

	HPROP hProp;
	if ( (hProp = ::DATA_Find_Prop(hData, NULL, nPID)) != NULL ) 
		return ::GetPropValueAsString( hProp, NULL );

	pStr = new TCHAR[1];
	*pStr = L'\0';

	return pStr;
}


// ---
void SetPropValue( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat ) {
	if ( !pStr )
		return;

		// Заменить Value узла
	CString rcMinStr;
	rcMinStr.LoadString( IDS_WAS_DTYPE_SMIN );

	CString rcMaxStr;
	rcMaxStr.LoadString( IDS_WAS_DTYPE_SMAX );

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_char   : {
#ifdef UNICODE
			AVP_char pChar[2] = { 0, 0 };
			::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pChar, sizeof(pChar), NULL, NULL );
			::PROP_Set_Val( hProp, AVP_dword(pChar[0]), 0 );
#else
			AVP_char nValue;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = CHAR_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = CHAR_MAX;
				else
					nValue = (AVP_char)pStr[0];
			::PROP_Set_Val( hProp, nValue, 0 );
#endif
		}
			break;
		case avpt_wchar  : {
#ifdef UNICODE
			TCHAR nValue;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = CHAR_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = CHAR_MAX;
				else
					nValue = pStr[0];
			::PROP_Set_Val( hProp, nValue, 0 );
#else
			AVP_wchar pWChar[2] = { 0, 0 };
			::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, (LPWSTR)pWChar, sizeof(pWChar) );
			::PROP_Set_Val( hProp, AVP_dword(pWChar[0]), 0 );
#endif
		}
			break;
		case avpt_short  : {
			AVP_short nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = SHRT_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = SHRT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = (AVP_short)::A2I(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_long   : {
			AVP_long nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = LONG_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = LONG_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2I(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_longlong   : {
			AVP_longlong nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = _I64_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = _I64_MAX;
				else 
					nValue = ::A2I64(pStr, NULL, ::GetConvertBase(pStr));
			::PROP_Set_Val( hProp, AVP_dword(&nValue), sizeof(nValue) );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = CHAR_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = (AVP_byte)::A2UI(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_word   : {
			AVP_word nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = USHRT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = (AVP_word)::A2UI(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = ULONG_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2UI(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_qword  : {
			AVP_qword nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = _UI64_MAX;
				else 
					nValue = ::A2UI64(pStr, NULL, ::GetConvertBase(pStr));
			::PROP_Set_Val( hProp, AVP_dword(&nValue), sizeof(nValue) );
		}
			break;
		case avpt_int   : {
			AVP_int nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = INT_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = INT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2I(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = UINT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2UI(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue = 0;
			//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
			if ( VALID_STR(pFormat) )
				_stscanf( pStr, pFormat, &nValue );
			else
				nValue = ::A2UI(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_group   : {
			AVP_group nValue = 0;
			//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
			if ( VALID_STR(pFormat) )
				_stscanf( pStr, pFormat, &nValue );
			else
				nValue = (AVP_group)::A2UI(pStr);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_date   : {
			AVP_date nValue;
			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			if ( !rcMinStr.CompareNoCase(pStr) ) {
				rcSysTime.wYear  = 1970;
				rcSysTime.wMonth = 12;
				rcSysTime.wDay	 = 31;
			}
			else
				if ( !rcMaxStr.CompareNoCase(pStr) ) {
					rcSysTime.wYear  = 2038;
					rcSysTime.wMonth = 1;
					rcSysTime.wDay	 = 18;
				}
			::CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
			::PROP_Set_Val( hProp, AVP_dword(nValue), 0 );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			if ( !rcMinStr.CompareNoCase(pStr) ) {
				rcSysTime.wHour  = 0;
				rcSysTime.wMinute = 0;
				rcSysTime.wSecond	 = 0;
				rcSysTime.wMilliseconds = 1;
			}
			else
				if ( !rcMaxStr.CompareNoCase(pStr) ) {
					rcSysTime.wHour  = 23;
					rcSysTime.wMinute = 59;
					rcSysTime.wSecond	 = 59;
					rcSysTime.wMilliseconds = 1;
				}
				::CopyMemory( &nValue, &rcSysTime.wHour, sizeof(nValue) );
			::PROP_Set_Val( hProp, (AVP_dword)nValue, 0 );
		}
			break;
		case avpt_datetime : {
			AVP_datetime nValue;
			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			if ( !rcMinStr.CompareNoCase(pStr) ) {
				rcSysTime.wYear  = 1970;
				rcSysTime.wMonth = 12;
				rcSysTime.wDay	 = 31;
				rcSysTime.wHour  = 0;
				rcSysTime.wMinute = 0;
				rcSysTime.wSecond	 = 0;
				rcSysTime.wMilliseconds = 1;
			}
			else
				if ( !rcMaxStr.CompareNoCase(pStr) ) {
					rcSysTime.wYear  = 2038;
					rcSysTime.wMonth = 1;
					rcSysTime.wDay	 = 18;
					rcSysTime.wHour  = 23;
					rcSysTime.wMinute = 59;
					rcSysTime.wSecond	 = 59;
					rcSysTime.wMilliseconds = 1;
				}
			::CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
			::PROP_Set_Val( hProp, AVP_dword(nValue), 0 );
		}
			break;
		case avpt_str    : {
#ifdef UNICODE
			AVP_str pMStr;
			int nSize = ::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0, NULL, NULL );
			pMStr = new AVP_char[nSize];
			::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pMStr, nSize, NULL, NULL );
			::PROP_Set_Val( hProp, AVP_dword(pMStr), 0 );
			delete [] pMStr;
#else
			::PROP_Set_Val( hProp, AVP_dword(pStr), 0 );
#endif
		}
			break;
		case avpt_wstr   : {
#ifdef UNICODE
			::PROP_Set_Val( hProp, AVP_dword(pStr), 0 );
#else
			AVP_wstr pWStr;
			int nSize = ::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0 ) + 1;
			pWStr = new AVP_wchar[nSize];
			if ( pWStr ) {
				*pWStr = 0;
				::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, (LPWSTR)pWStr, nSize );
				::PROP_Set_Val( hProp, AVP_dword(pWStr), 0 );
			}
			delete [] pWStr;
#endif
		}
			break;
		case avpt_bin : {
			AVP_dword dwSize;
			AVP_byte *pValue = ::Str2Bin( pStr, &dwSize );
			::PROP_Set_Val( hProp, AVP_dword(pValue), dwSize );
			delete []	pValue;
		}
			break;
	}

}


// ---
void SetPropNumericValue( HPROP hProp, AVP_dword nValueValue ) {
		// Заменить Value узла
	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue = AVP_short(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_long   : {
			AVP_long nValue = AVP_long(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue = AVP_byte(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_word   : {
			AVP_word nValue = AVP_word(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue = AVP_dword(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_int   : {
			AVP_int nValue = AVP_int(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue = AVP_uint(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue = AVP_bool(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_group   : {
			AVP_group nValue = AVP_group(nValueValue);
			::PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
	}
}


// ---
void SetPropDateValue( HPROP hProp, const SYSTEMTIME *pSysTime ) {
		// Заменить Value узла
	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_date   : {
			AVP_date nValue;
			::CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			::PROP_Set_Val( hProp, (AVP_dword)nValue, 0 );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			CopyMemory( &nValue, &pSysTime->wHour, sizeof(nValue) );
			PROP_Set_Val( hProp, (AVP_dword)nValue, 0 );
		}
			break;
		case avpt_datetime  : {
			AVP_datetime nValue;
			::CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			::PROP_Set_Val( hProp, (AVP_dword)nValue, 0 );
		}
			break;
	}
}


// ---
void SetPropArrayValue( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat ) {
	if ( !pStr )
		return;

		// Заменить Value узла
	CString rcMinStr;
	rcMinStr.LoadString( IDS_WAS_DTYPE_SMIN );

	CString rcMaxStr;
	rcMaxStr.LoadString( IDS_WAS_DTYPE_SMAX );

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_char   : {
#ifdef UNICODE
			AVP_char pChar[2] = { 0, 0 };
			::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pChar, sizeof(pChar), NULL, NULL );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
				::PROP_Arr_Set_Items( hProp, nPos, &pChar[0], sizeof(AVP_char) );
			else
				::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &pChar[0], sizeof(AVP_char) );
#else
			AVP_char nValue;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = CHAR_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = CHAR_MAX;
				else
					nValue = (AVP_char)pStr[0];
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
				::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
				::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
#endif
		}
			break;
		case avpt_wchar  : {
#ifdef UNICODE
			TCHAR nValue;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = CHAR_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = CHAR_MAX;
				else
					nValue = pStr[0];
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
				::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
				::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
#else
			AVP_wchar pWChar[2] = { 0, 0 };
			::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, (LPWSTR)pWChar, sizeof(pWChar) );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
				::PROP_Arr_Set_Items( hProp, nPos, &pWChar[0], sizeof(AVP_wchar) );
			else
				::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &pWChar[0], sizeof(AVP_wchar) );
#endif
		}
			break;
		case avpt_short  : {
			AVP_short nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = SHRT_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = SHRT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = (AVP_short)::A2I(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_long   : {
			AVP_long nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = LONG_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = LONG_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2I(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_longlong   : {
			AVP_longlong nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = _I64_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = _I64_MAX;
				else 
					nValue = ::A2I64(pStr, NULL, ::GetConvertBase(pStr));
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = CHAR_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = (AVP_byte)::A2UI(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_word   : {
			AVP_word nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = USHRT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = (AVP_word)::A2UI(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = ULONG_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2UI(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_qword  : {
			AVP_qword nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = ULONG_MAX;
				else 
					nValue = ::A2UI64(pStr, NULL, ::GetConvertBase(pStr));
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_int   : {
			AVP_int nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = INT_MIN;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = INT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2I(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue = 0;
			if ( !rcMinStr.CompareNoCase(pStr) )
				nValue  = 0;
			else
				if ( !rcMaxStr.CompareNoCase(pStr) )
					nValue  = UINT_MAX;
				else 
					//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
					if ( VALID_STR(pFormat) )
						_stscanf( pStr, pFormat, &nValue );
					else
						nValue = ::A2UI(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue = 0;
			//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
			if ( VALID_STR(pFormat) )
				_stscanf( pStr, pFormat, &nValue );
			else
				nValue = ::A2UI(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_group   : {
			AVP_group nValue = 0;
			//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
			if ( VALID_STR(pFormat) )
				_stscanf( pStr, pFormat, &nValue );
			else
				nValue = (AVP_group)::A2UI(pStr);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_date   : {
			AVP_date nValue;
			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			if ( !rcMinStr.CompareNoCase(pStr) ) {
				rcSysTime.wYear  = 1970;
				rcSysTime.wMonth = 12;
				rcSysTime.wDay	 = 31;
			}
			else
				if ( !rcMaxStr.CompareNoCase(pStr) ) {
					rcSysTime.wYear  = 2038;
					rcSysTime.wMonth = 1;
					rcSysTime.wDay	 = 18;
				}
			::CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			SYSTEMTIME rcSysTime;
			ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			if ( !rcMinStr.CompareNoCase(pStr) ) {
				rcSysTime.wHour  = 0;
				rcSysTime.wMinute = 0;
				rcSysTime.wSecond	 = 0;
				rcSysTime.wMilliseconds = 1;
			}
			else
				if ( !rcMaxStr.CompareNoCase(pStr) ) {
					rcSysTime.wHour  = 23;
					rcSysTime.wMinute = 59;
					rcSysTime.wSecond	 = 59;
					rcSysTime.wMilliseconds = 1;
				}
			CopyMemory( &nValue, &rcSysTime.wHour, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
				PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
				PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_datetime   : {
			AVP_datetime nValue;
			SYSTEMTIME rcSysTime;
			::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
			if ( !rcMinStr.CompareNoCase(pStr) ) {
				rcSysTime.wYear  = 1970;
				rcSysTime.wMonth = 12;
				rcSysTime.wDay	 = 31;
				rcSysTime.wHour  = 0;
				rcSysTime.wMinute = 0;
				rcSysTime.wSecond	 = 0;
				rcSysTime.wMilliseconds = 1;
			}
			else
				if ( !rcMaxStr.CompareNoCase(pStr) ) {
					rcSysTime.wYear  = 2038;
					rcSysTime.wMonth = 1;
					rcSysTime.wDay	 = 18;
					rcSysTime.wHour  = 23;
					rcSysTime.wMinute = 59;
					rcSysTime.wSecond	 = 59;
					rcSysTime.wMilliseconds = 1;
				}
			::CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_str    : {
#ifdef UNICODE
			AVP_str pMStr;
			int nSize = ::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0, NULL, NULL );
			pMStr = new AVP_char[nSize];
			::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pMStr, nSize, NULL, NULL );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
				::PROP_Arr_Set_Items( hProp, nPos, (void *)pMStr, 0 );
			else
				::PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pMStr, 0 );
			delete [] pMStr;
#else
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, (void *)pStr, 0 );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pStr, 0 );
#endif
		}
			break;
		case avpt_wstr   : {
#ifdef UNICODE
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
				::PROP_Arr_Set_Items( hProp, nPos, (void *)pStr, 0 );
			else
				::PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pStr, 0 );
#else
			AVP_wstr pWStr;
			int nSize = ::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0 ) + 1;
			pWStr = new AVP_wchar[nSize];
			if ( pWStr ) {
				*pWStr = 0;
				::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, (LPWSTR)pWStr, nSize );
				if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
					::PROP_Arr_Set_Items( hProp, nPos, (void *)pWStr, 0 );
				else
					::PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pWStr, 0 );
				delete [] pWStr;
			}
#endif
		}
			break;
		case avpt_bin : {
			AVP_dword dwSize;
			AVP_byte *pValue = ::Str2Bin( pStr, &dwSize );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, (void *)pValue, dwSize );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pValue, dwSize );
			delete []	pValue;
		}
			break;
	}

}

// ---
void SetPropArrayNumericValue( HPROP hProp, int nPos, AVP_dword nValueValue ) {
		// Заменить Value узла
	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue = AVP_short(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_long   : {
			AVP_long nValue = AVP_long(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue = AVP_byte(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_word   : {
			AVP_word nValue = AVP_word(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue = AVP_dword(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_int   : {
			AVP_int nValue = AVP_int(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue = AVP_uint(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue = AVP_bool(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_group   : {
			AVP_group nValue = AVP_group(nValueValue);
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
	}
}


// ---
void SetPropDateArrayValue( HPROP hProp, int nPos, const SYSTEMTIME *pSysTime ) {
		// Заменить Value узла
	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_date   : {
			AVP_date nValue;
			::CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			CopyMemory( &nValue, &pSysTime->wHour, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_datetime   : {
			AVP_datetime nValue;
			::CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)::PROP_Arr_Count(hProp) )
			  ::PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  ::PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
	}
}


// ---
static TCHAR *AnalizeDate( AVP_date &nValue ) {
	AVP_date nEmptyValue;
	::ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	SYSTEMTIME rcSysTime;
	::ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
	memcpy( &rcSysTime, nValue, sizeof(nValue) );

	CTime rcDate( rcSysTime );
	if ( !memcmp(&nValue, &nEmptyValue, sizeof(nValue)) )
		rcDate = CTime::GetCurrentTime();

	CString rcDateString;
	TCHAR pDateFormat[80] = {0};
	::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE , pDateFormat, sizeof(pDateFormat) );

	if ( TCSCLEN(pDateFormat) ) {
		TCHAR pBuff[128];
		rcDate.GetAsSystemTime( rcSysTime );
		::GetDateFormat( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, pBuff, sizeof(pBuff) );
		rcDateString = pBuff;
	}
	else {
		CString rcDateFormat;
		rcDateFormat.LoadString( IDS_WAS_DTYPE_SDATE );

		if ( !rcDateFormat.GetLength() )
			rcDateFormat = _T("%#x");

		rcDateString = rcDate.Format( rcDateFormat );
	}

	TCHAR *pResult = new TCHAR[(rcDateString.GetLength() + 1)];
	_tcscpy( pResult, rcDateString );

	return pResult;
}


// ---
static TCHAR *AnalizeTime( AVP_time &nValue ) {
	AVP_time nEmptyValue;
	::ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	SYSTEMTIME rcSysTime;
	CTime::GetCurrentTime().GetAsSystemTime( rcSysTime );
	memcpy( &rcSysTime.wHour, nValue, sizeof(nValue) );

	CTime rcDate( rcSysTime );

	if ( !memcmp(&nValue, &nEmptyValue, sizeof(nValue)) )
		rcDate = CTime::GetCurrentTime();

	CString rcDateString;
	TCHAR pDateFormat[80] = {0};
	::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, pDateFormat, sizeof(pDateFormat) );

	if ( TCSCLEN(pDateFormat) ) {
		TCHAR pBuff[128];
		rcDate.GetAsSystemTime( rcSysTime );
		::GetTimeFormat( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, pBuff, sizeof(pBuff) );
		rcDateString = pBuff;
	}
	else {
		CString rcDateFormat;
		rcDateFormat.LoadString( IDS_WAS_DTYPE_STIME );

		if ( !rcDateFormat.GetLength() )
			rcDateFormat = _T("%X");

		rcDateString = rcDate.Format( rcDateFormat );
	}

	TCHAR *pResult = new TCHAR[(rcDateString.GetLength() + 1)];
	_tcscpy( pResult, rcDateString );

	return pResult;
}


// ---
static TCHAR *AnalizeDateTime( AVP_datetime &nValue ) {
	TCHAR *pDateValue = ::AnalizeDate( *(AVP_date *)&nValue );
	TCHAR *pTimeValue = ::AnalizeTime( *(AVP_time *)&(((SYSTEMTIME *)&nValue)->wHour) );

	TCHAR *pResult = new TCHAR[(TCSCLEN(pDateValue) + TCSCLEN(pTimeValue) + 5)];
	_tcscpy( pResult, pDateValue );
	_tcscat( pResult, _T("  ") );
	_tcscat( pResult, pTimeValue );

	delete [] pDateValue;
	delete [] pTimeValue;

	return pResult;
}


// ---
TCHAR *GetPropValueAsString( HPROP hProp, const TCHAR *pFormat ) {
	TCHAR *pResult = NULL;

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_char   : {
#ifdef UNICODE
			AVP_char pChar[2] = { 0, 0 };
			::PROP_Get_Val( hProp, pChar, 2 );
			int nSize = ::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pChar, -1, NULL, 0 ) + 1;
			pResult = new TCHAR[nSize];
			if ( pResult ) {
				*pResult = 0;
				::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pChar, -1, pResult, nSize );
			}
#else
			AVP_char nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			*(pResult = new TCHAR[2]) = nValue;
			*(pResult + 1) = 0;
#endif
		}
			break;
		case avpt_wchar  : {
#ifdef UNICODE
			TCHAR nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			*(pResult = new TCHAR[2]) = nValue;
			*(pResult + 1) = 0;
#else
			AVP_wchar pWChar[2] = { 0, 0 };
			::PROP_Get_Val( hProp, pWChar, 2 );
			int nSize = ::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, NULL, 0, NULL, NULL );
			pResult = new TCHAR[nSize];
			::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, pResult, nSize, NULL, NULL );
#endif
		}
			break;
		case avpt_short  : {
			AVP_short nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
		}
			break;
		case avpt_long   : {
			AVP_long nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
		}
			break;
		case avpt_longlong   : {
			AVP_longlong nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[35];
			_i64tot( nValue, pResult, 10 );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_word   : {
			AVP_word nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_qword  : {
			AVP_qword nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[35];
			_i64tot( nValue, pResult, 16 );
		}
			break;
		case avpt_int   : {
			AVP_int nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_group  : {
			AVP_group nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_date   : {
			AVP_date nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );

			pResult = ::AnalizeDate( nValue );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );

			pResult = ::AnalizeTime( nValue );
		}
			break;
		case avpt_datetime   : {
			AVP_datetime nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );

			pResult = ::AnalizeDateTime( nValue );
		}
			break;
		case avpt_str    : {
#ifdef UNICODE
			int nSize = ::PROP_Get_Val( hProp, NULL, 0 ) + 1;
			if ( nSize ) {
				AVP_char *pWChar = new AVP_char[nSize];
				*pWChar = 0;
				::PROP_Get_Val( hProp, pWChar, nSize );
				nSize = ::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
				pResult = new TCHAR[nSize];
				if ( pResult ) {
					*pResult = 0;
					::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
				}
				delete [] pWChar;
			}
#else
			int nSize = ::PROP_Get_Val( hProp, NULL, 0 ) + 1;
			if ( nSize ) {
				pResult = new TCHAR[nSize];
				*pResult = 0;
				::PROP_Get_Val( hProp, pResult, nSize );
			}
#endif
		}
			break;
		case avpt_wstr   : {
#ifdef UNICODE
			int nSize = ::PROP_Get_Val( hProp, NULL, 0 ) + 1;
			if ( nSize ) {
				pResult = new TCHAR[nSize];
				*pResult = 0;
				::PROP_Get_Val( hProp, pResult, nSize );
			}
#else
			int nSize = ::PROP_Get_Val( hProp, NULL, 0 ) + 1;
			if ( nSize ) {
				AVP_wchar *pWChar = new AVP_wchar[nSize];
				*pWChar = 0;
				::PROP_Get_Val( hProp, pWChar, nSize );
				nSize = ::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, NULL, 0, NULL, NULL );
				pResult = new TCHAR[nSize];
				::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, pResult, nSize, NULL, NULL );
				delete [] pWChar;
			}
#endif
		}
			break;
		case avpt_bin : {
			AVP_dword nSize = ::PROP_Get_Val( hProp, NULL, 0 );
			if ( nSize ) {
				AVP_byte *pValue = new AVP_byte[nSize];
				::PROP_Get_Val( hProp, pValue, nSize );
				pResult = ::Bin2Str( pValue, nSize );
				delete [] pValue;
			}
		}
			break;
	}
	return pResult;
}


// ---
TCHAR *GetPropArrayValueAsString( HPROP hProp, int nPos, const TCHAR *pFormat ) {
	TCHAR *pResult = NULL;

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_char   : {
#ifdef UNICODE
			AVP_char pWChar[2] = { 0, 0 };
			::PROP_Arr_Get_Items( hProp, nPos, pWChar, 2 );
			int nSize = ::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
			pResult = new TCHAR[nSize];
			if ( pResult ) {
				*pResult = 0;
				::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
			}
#else
			AVP_char nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			*(pResult = new TCHAR[1]) = nValue;
			*(pResult + 1) = L'\0';
#endif
		}
			break;
		case avpt_wchar  : {
#ifdef UNICODE
			TCHAR nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			*(pResult = new TCHAR[1]) = nValue;
			*(pResult + 1) = L'\0';
#else
			AVP_wchar pWChar[2] = { 0, 0 };
			::PROP_Arr_Get_Items( hProp, nPos, pWChar, 2 );
			int nSize = ::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, NULL, 0, NULL, NULL );
			pResult = new TCHAR[nSize];
			::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, pResult, nSize, NULL, NULL );
#endif
		}
			break;
		case avpt_short  : {
			AVP_short nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
		}
			break;
		case avpt_long   : {
			AVP_long nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
		}
			break;
		case avpt_longlong   : {
			AVP_longlong nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[35];
			_i64tot( nValue, pResult, 10 );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_word   : {
			AVP_word nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_qword  : {
			AVP_qword nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[35];
			_i64tot( nValue, pResult, 16 );
		}
			break;
		case avpt_int   : {
			AVP_int nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_group  : {
			AVP_group nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			pResult = new TCHAR[12];
			wsprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
		}
			break;
		case avpt_date   : {
			AVP_date nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );

			pResult = ::AnalizeDate( nValue );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );

			pResult = ::AnalizeTime( nValue );
		}
			break;
		case avpt_datetime   : {
			AVP_datetime nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );

			pResult = ::AnalizeDateTime( nValue );
		}
			break;
		case avpt_str    : {
#ifdef UNICODE
			int nSize = ::PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + 1;
			if ( nSize ) {
				AVP_char *pWChar = new AVP_char[nSize];
				*pWChar = 0;
				::PROP_Arr_Get_Items( hProp, nPos, pWChar, nSize );
				nSize = ::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
				pResult = new TCHAR[nSize];
				if ( pResult ) {
					*pResult = 0;
					::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
				}
				delete [] pWChar;
			}
#else
			int nSize = ::PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + 1;
			pResult = new TCHAR[nSize];
			*pResult = 0;
			::PROP_Arr_Get_Items( hProp, nPos, pResult, nSize );
#endif
		}
			break;
		case avpt_wstr   : {
#ifdef UNICODE
			int nSize = ::PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + 1;
			pResult = new TCHAR[nSize];
			*pResult = 0;
			::PROP_Arr_Get_Items( hProp, nPos, pResult, nSize );
#else
			int nSize = ::PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + 1;
			if ( nSize ) {
				AVP_wchar *pWChar = new AVP_wchar[nSize];
				*pWChar = 0;
				::PROP_Arr_Get_Items( hProp, nPos, pWChar, nSize );
				nSize = ::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, NULL, 0, NULL, NULL );
				pResult = new TCHAR[nSize];
				*pResult = 0;
				::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (LPWSTR)pWChar, -1, pResult, nSize, NULL, NULL );
				delete [] pWChar;
			}
#endif
		}
			break;
		case avpt_bin : {
			AVP_dword nSize = ::PROP_Arr_Get_Items( hProp, nPos, NULL, 0 );
			if ( nSize ) {
				AVP_byte *pValue = new AVP_byte[nSize];
				::PROP_Arr_Get_Items( hProp, nPos, pValue, nSize );
				pResult = ::Bin2Str( pValue, nSize );
				delete [] pValue;
			}
		}
			break;
	}
	return pResult;
}


// ---
TCHAR *GetDataValueAsComboString( HDATA hData ) {
	TCHAR *pResult = NULL;

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
	if ( hProp ) {
		CAPointer<TCHAR> pDataText = ::GetPropValueAsString( ::DATA_Find_Prop(hData, 0, 0), 0 );
		AVP_dword nCount = ::PROP_Arr_Count( hProp );
		for ( AVP_dword i=0; i<nCount; i++ ) {
			CAPointer<TCHAR> pArrItemText = ::GetPropArrayValueAsString( hProp, i, 0 );
			if ( !_tcscmp(pDataText, pArrItemText) ) {
				pResult = new TCHAR[(TCSCLEN(pDataText) + 1)];
				_tcscpy( pResult, pDataText );
				break;
			}
		}
	}

	return pResult;
}

// ---
TCHAR *GetDataNumericValueAsComboString( HDATA hData ) {
	TCHAR *pResult = NULL;

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		::DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			AVP_dword nValue = 0;
			HPROP hComboProp = ::DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				hProp = ::DATA_Find_Prop( hData, 0, 0 );
				nValue = ::GetPropNumericValueAsDWord( hProp );

				pResult = ::GetPropArrayValueAsString( hComboProp, nValue, 0 );
			}
		}
	}

	return pResult;
}


// ---
TCHAR *GetDataArrayNumericValueAsComboString( HDATA hData, int nPos ) {
	TCHAR *pResult = NULL;

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		::DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			AVP_dword nValue = 0;
			HPROP hComboProp = ::DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				hProp = ::DATA_Find_Prop( hData, 0, 0 );
				nValue = ::GetPropArrayNumericValueAsDWord( hProp, nPos );

				pResult = ::GetPropArrayValueAsString( hComboProp, nValue, 0 );
			}
		}
	}

	return pResult;
}


// ---
bool SetDataNumericValueAsComboString( HDATA hData, const TCHAR *pValueText ) {
	bool bResult = false;

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		::DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			AVP_dword nValue = 0;
			HPROP hComboProp = ::DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				bResult = true;
				HPROP hProp = ::DATA_Find_Prop( hData, 0, 0 );
				int nCount = ::PROP_Arr_Count( hComboProp );
				bool bFound = false;
				for ( int i=0; i<nCount && !bFound; i++ ) {
					CAPointer<TCHAR> pItemText = ::GetPropArrayValueAsString( hComboProp, i, NULL );
					bFound = !_tcscmp( pValueText, pItemText );
					if ( bFound ) 
						::SetPropNumericValue( hProp, i );
				}
				if ( !bFound ) 
					::SetPropNumericValue( hProp, 0 );
			}
		}
	}

	return bResult;
}


// ---
bool SetDataArrayNumericValueAsComboString( HDATA hData, int nPos, const TCHAR *pValueText ) {
	bool bResult = false;

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		::DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			AVP_dword nValue = 0;
			HPROP hComboProp = ::DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				bResult = true;
				HPROP hProp = ::DATA_Find_Prop( hData, 0, 0 );
				int nCount = ::PROP_Arr_Count( hComboProp );
				bool bFound = false;
				for ( int i=0; i<nCount && !bFound; i++ ) {
					CAPointer<TCHAR> pItemText = ::GetPropArrayValueAsString( hComboProp, i, NULL );
					bFound = !_tcscmp( pValueText, pItemText );
					if ( bFound ) 
						::SetPropArrayNumericValue( hProp, nPos, i );
				}
				if ( !bFound ) 
					::SetPropArrayNumericValue( hProp, nPos, 0 );
			}
		}
	}

	return bResult;
}


// ---
AVP_dword GetPropNumericValueAsDWord( HPROP hProp ) {
	AVP_dword nResult = 0;

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_long   : {
			AVP_long nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_word   : {
			AVP_word nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_int   : {
			AVP_int nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_group : {
			AVP_group nValue;
			::PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_nothing  : {
			nResult = 1;
		}
			break;
	}
	return nResult;
}

// ---
AVP_dword GetPropArrayNumericValueAsDWord( HPROP hProp, int nPos ) {
	AVP_dword nResult = 0;

	AVP_dword nID = ::PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_long   : {
			AVP_long nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_word   : {
			AVP_word nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_int   : {
			AVP_int nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_group : {
			AVP_group nValue;
			::PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_nothing  : {
			nResult = 1;
		}
			break;
	}
	return nResult;
}

// ---
int	CompareValueInRangeH( HDATA hData, const TCHAR *pStr, 
												  HPROP hMinProp, HPROP hMaxProp, const TCHAR *pFormat ) {
	int nResult = 0;

	if ( hMinProp || hMaxProp )	{
		AVP_dword nID   = ::DATA_Get_Id( hData, NULL );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);

		switch ( nType ) {
			case avpt_short  : {
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2I(pStr);
				if ( hMinProp ) {
					AVP_short nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_short nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_long   : {
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2I(pStr);
				if ( hMinProp ) {
					AVP_long nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_long nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue = 0;
				nValue = ::A2I64(pStr, NULL, ::GetConvertBase(pStr));
				if ( hMinProp ) {
					AVP_longlong nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_longlong nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_bool   : {
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2UI(pStr);
				if ( hMinProp ) {
					AVP_bool nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = (AVP_dword)nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_bool nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = (AVP_dword)nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_byte   : {
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2UI(pStr);
				if ( hMinProp ) {
					AVP_byte nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_byte nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_word   : {
				/*
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2UI(pStr);
				if ( hMinProp ) {
					AVP_word nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_word nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
				*/
				AVP_long nValue = 0;
				if ( VALID_STR(pFormat) ) 
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2I(pStr);
				if ( hMinProp ) {
					AVP_word nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= (AVP_long)nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_word nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= (AVP_long)nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_dword  : {
				/*
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2UI(pStr);
				if ( hMinProp ) {
					AVP_dword nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = (AVP_dword)nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_dword nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = (AVP_dword)nValue <= nMValue ? 0 : 1;
				}
				*/
				AVP_longlong nValue = 0;
				if ( VALID_STR(pFormat) ) {
					AVP_long n32Value = 0;
					_stscanf( pStr, pFormat, &n32Value );
					nValue = n32Value;
				}
				else 
					nValue = ::A2I64(pStr, NULL, ::GetConvertBase(pStr));
				if ( hMinProp ) {
					AVP_dword nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= (AVP_longlong)nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_dword nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= (AVP_longlong)nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue = 0;
				nValue = ::A2UI64(pStr, NULL, ::GetConvertBase(pStr));
				if ( hMinProp ) {
					AVP_qword nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_qword nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_int   : {
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2I(pStr);
				if ( hMinProp ) {
					AVP_int nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_int nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_uint  : {
				AVP_long nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = ::A2UI(pStr);
				if ( hMinProp ) {
					AVP_uint nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = (AVP_dword)nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_uint nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = (AVP_dword)nValue <= nMValue ? 0 : 1;
				}
			}
				break;
		}
	}

	return nResult;
}

// ---
int	CompareValueInRangeI( HDATA hData, const TCHAR *pValueText, 
												  AVP_dword nMinID, AVP_dword nMaxID, const TCHAR *pFormat ) {

	AVP_dword nID   = ::DATA_Get_Id( hData, NULL );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	AVP_dword nApp  = GET_AVP_PID_APP(nID);

	AVP_dword nMinPid = MAKE_AVP_PID( nMinID, nApp, nType, 0);  
	AVP_dword nMaxPid = MAKE_AVP_PID( nMaxID, nApp, nType, 0);  

	HPROP hMinProp = ::DATA_Find_Prop( hData, NULL, nMinPid ); 
	HPROP hMaxProp = ::DATA_Find_Prop( hData, NULL, nMaxPid ); 

	return ::CompareValueInRangeH( hData, pValueText, hMinProp, hMaxProp, pFormat );
}


// ---
int	CompareDateValueInRangeH( HDATA hData, const SYSTEMTIME *pValueTime, 
														  HPROP hMinProp, HPROP hMaxProp ) {
	int nResult = 0;

	if ( hMinProp || hMaxProp )	{
		AVP_dword nID   = ::DATA_Get_Id( hData, NULL );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);

		switch ( nType ) {
			case avpt_date   : {
				if ( hMinProp ) {
					AVP_date nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) >= 0 ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_date nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) <= 0  ? 0 : 1;
				}
			}
				break;
			case avpt_time   : {
				if ( hMinProp ) {
					AVP_time nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(&pValueTime->wHour, &nMValue, sizeof(nMValue)) >= 0 ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_time nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(&pValueTime->wHour, &nMValue, sizeof(nMValue)) <= 0  ? 0 : 1;
				}
			}
				break;
			case avpt_datetime   : {
				if ( hMinProp ) {
					AVP_datetime nMValue;
					::PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) >= 0 ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_datetime nMValue;
					::PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) <= 0  ? 0 : 1;
				}
			}
				break;
		}
	}

	return nResult;
}


// ---
int	CompareDateValueInRangeI( HDATA hData, const SYSTEMTIME *pValueTime, 
														  AVP_dword nMinID, AVP_dword nMaxID ) {

	AVP_dword nID   = ::DATA_Get_Id( hData, NULL );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	AVP_dword nApp  = GET_AVP_PID_APP(nID);

	AVP_dword nMinPid = MAKE_AVP_PID( nMinID, nApp, nType, 0);  
	AVP_dword nMaxPid = MAKE_AVP_PID( nMaxID, nApp, nType, 0);  

	HPROP hMinProp = ::DATA_Find_Prop( hData, NULL, nMinPid ); 
	HPROP hMaxProp = ::DATA_Find_Prop( hData, NULL, nMaxPid ); 

	return ::CompareDateValueInRangeH( hData, pValueTime, hMinProp, hMaxProp );
}


// ---
TCHAR *GetPropValueAsStringCC( HPROP hProp, const TCHAR *pFormat ) {
	HDATA hData = ::PROP_Get_Dad( hProp );

	TCHAR *pResult = ::GetDataNumericValueAsComboString( hData );

	if ( !pResult )
		pResult = ::GetPropValueAsString( hProp, pFormat );

	return pResult;
}

// ---
TCHAR *GetPropArrayValueAsStringCC( HPROP hProp, int nPos, const TCHAR *pFormat ) {
	HDATA hData = ::PROP_Get_Dad( hProp );

	TCHAR *pResult = ::GetDataArrayNumericValueAsComboString( hData, nPos );

	if ( !pResult )
		pResult = ::GetPropArrayValueAsString( hProp, nPos, pFormat );

	return pResult;
}


// ---
void SetPropValueCC( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat ) {
	HDATA hData = ::PROP_Get_Dad( hProp );

	if ( !::SetDataNumericValueAsComboString(hData, pStr) )
		SetPropValue( hProp, pStr, pFormat );
}


// ---
void SetPropArrayValueCC( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat ) {
	HDATA hData = ::PROP_Get_Dad( hProp );

	if ( !::SetDataArrayNumericValueAsComboString(hData, nPos, pStr) )
		SetPropArrayValue( hProp, nPos, pStr, pFormat );
}

// ---
static void FlushArray( HPROP hArrayProp ) {
	AVP_dword nCount = PROP_Arr_Count( hArrayProp );
	if ( nCount )
		PROP_Arr_Remove( hArrayProp, 0, nCount );
}


// ---
void  ExchangeValue( HPROP hFromProp, HPROP hToProp ) {
	AVP_dword nFromID = ::PROP_Get_Id( hFromProp );
	AVP_dword nToID   = ::PROP_Get_Id( hToProp );
	if ( nFromID == nToID ) { 
		AVP_dword nType  = GET_AVP_PID_TYPE(nFromID);
		AVP_bool  bArray = GET_AVP_PID_ARR(nFromID);
		switch ( nType ) {
			case avpt_char   : 
				if ( !bArray ) {
					AVP_char nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_char nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_wchar  : 
				if ( !bArray ) {
					AVP_wchar pWChar[2] = { 0, 0 };
					::PROP_Get_Val( hFromProp, pWChar, 1 );
					::PROP_Set_Val( hToProp, AVP_dword(pWChar), 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_wchar pWChar[2] = { 0, 0 };
						::PROP_Arr_Get_Items( hFromProp, nPos, pWChar, 1 );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &pWChar[0], sizeof(AVP_wchar) );
					}
				}
				break;
			case avpt_short  : 
				if ( !bArray ) {
					AVP_short nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_short nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_long   : 
				if ( !bArray ) {
					AVP_long nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_long nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_longlong   : 
				if ( !bArray ) {
					AVP_longlong nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, AVP_dword(&nValue), sizeof(nValue) );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_longlong nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_byte   : 
				if ( !bArray ) {
					AVP_byte nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_byte nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_word   : 
				if ( !bArray ) {
					AVP_word nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_word nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_dword  : 
				if ( !bArray ) {
					AVP_dword nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_dword nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_qword  : 
				if ( !bArray ) {
					AVP_qword nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, AVP_dword(&nValue), sizeof(nValue) );
				}
				else {
					::FlushArray( hToProp );
					AVP_qword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_qword nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_int   : 
				if ( !bArray ) {
					AVP_int nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_int nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_uint  : 
				if ( !bArray ) {
					AVP_uint nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_uint nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_bool   : 
				if ( !bArray ) {
					AVP_bool nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_bool nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_group  : 
				if ( !bArray ) {
					AVP_group nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_group nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_date   : 
				if ( !bArray ) {
					AVP_date nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, AVP_dword(nValue), 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_date nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_time   : 
				if ( !bArray ) {
					AVP_time nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, AVP_dword(nValue), 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_time nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_datetime   : 
				if ( !bArray ) {
					AVP_datetime nValue;
					::PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					::PROP_Set_Val( hToProp, AVP_dword(nValue), 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						AVP_datetime nValue;
						::PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_str    : 
				if ( !bArray ) {
					int nSize = ::PROP_Get_Val( hFromProp, NULL, 0 );
					if ( nSize ) {
						AVP_char *pResult = new AVP_char[nSize];
						::PROP_Get_Val( hFromProp, pResult, nSize );
						::PROP_Set_Val( hToProp, AVP_dword(pResult), 0 );
						delete [] pResult;
					}
					else
						::PROP_Set_Val( hToProp, AVP_dword(0), 0 );
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						int nSize = ::PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_char *pResult = new AVP_char[nSize];
							::PROP_Arr_Get_Items( hFromProp, nPos, pResult, nSize );
							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pResult, 0 );
							delete [] pResult;
						}
						else
							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, 0, 0 );
					}
				}
				break;
			case avpt_wstr   : 
				if ( !bArray ) {
					int nSize = ::PROP_Get_Val( hFromProp, NULL, 0 );
					if ( nSize ) {
						AVP_wchar *pWChar = new AVP_wchar[nSize];
						::PROP_Get_Val( hFromProp, pWChar, nSize );
						::PROP_Set_Val( hToProp, AVP_dword(pWChar), 0 );
						delete [] pWChar;
					}
					else {
						::PROP_Set_Val( hToProp, AVP_dword(0), 0 );
//						int nSize = ::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, _T(""), -1, NULL, 0 );
//						AVP_wstr pWStr = new AVP_wchar[nSize];
//						::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, _T(""), -1, pWStr, nSize );
//						::PROP_Set_Val( hToProp, AVP_dword(pWStr), 0 );
//						delete [] pWStr;
					}
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						int nSize = ::PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_wchar *pWChar = new AVP_wchar[nSize];
							::PROP_Arr_Get_Items( hFromProp, nPos, pWChar, nSize );
							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pWChar, 0 );
							delete [] pWChar;
						}
						else {
							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, 0, 0 );
//							int nSize = ::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, _T(""), -1, NULL, 0 );
//							AVP_wstr pWStr = new AVP_wchar[nSize];
//							::MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, _T(""), -1, pWStr, nSize );
//							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pWStr, 0 );
//							delete [] pWStr;
						}
					}
				}
				break;
			case avpt_bin : 
				if ( !bArray ) {
					AVP_dword nSize = ::PROP_Get_Val( hFromProp, NULL, 0 );
					if ( nSize ) {
						AVP_byte *pValue = new AVP_byte[nSize];
						::PROP_Get_Val( hFromProp, pValue, nSize );
						::PROP_Set_Val( hToProp, AVP_dword(pValue), nSize );
						delete [] pValue;
					}
				}
				else {
					::FlushArray( hToProp );
					AVP_dword nCount = ::PROP_Arr_Count( hFromProp );
					for ( AVP_dword nPos=0; nPos<nCount; nPos++ ) {
						int nSize = ::PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_char *pResult = new AVP_char[nSize];
							::PROP_Arr_Get_Items( hFromProp, nPos, pResult, nSize );
							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pResult, 0 );
							delete [] pResult;
						}
						else
							::PROP_Arr_Insert( hToProp, PROP_ARR_LAST, 0, 0 );
					}
				}
				break;
		}
	}
}

/*
// ---
HDATA CreateNodesSequence( HDATA hStartData, AVP_dword *pAddrSequence ) {
	HDATA hResult = hStartData;
  AVP_dword *pNewSeq = ::DATA_Alloc_Sequence( 0 );
	while ( *pAddrSequence ) {
		::DATA_Add_Sequence( pNewSeq, *pAddrSequence, 0 );
		HDATA hFindData = ::DATA_Find( hStartData, pNewSeq );
		if ( !hFindData ) 
			hResult = ::DATA_Add( hResult, NULL, *pAddrSequence, 0, 0 );
		else
			hResult = hFindData;
		pAddrSequence++;
	}

	::DATA_Remove_Sequence( pNewSeq );

	return hResult;
}
*/

// ---
HDATA CreateNodesSequence( HDATA hStartData, AVP_dword *pAddrSequence ) {
	HDATA hResult = hStartData;
	while ( *pAddrSequence ) {
		AVP_dword *pNewSeq = ::DATA_Alloc_Sequence( *pAddrSequence, 0 );

		HDATA hFindData = ::DATA_Find( hResult, pNewSeq );
		if ( !hFindData ) 
			hResult = ::DATA_Add( hResult, NULL, *pAddrSequence, 0, 0 );
		else
			hResult = hFindData;
		pAddrSequence++;

		::DATA_Remove_Sequence( pNewSeq );
	}

	return hResult;
}

// ---
void GetDateEditFormat( TCHAR *pFormat, DWORD nSize ) {
  HKEY  hKey;
  DWORD nType = REG_SZ;
  BOOL  bResult = FALSE;
  
	*pFormat = 0;
	bResult = !!::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE , pFormat, nSize );

	if ( !bResult ) {
		if ( ERROR_SUCCESS == ::RegOpenKey( HKEY_CURRENT_USER, _T("Control Panel\\International"), &hKey) ) {
			if ( ERROR_SUCCESS == ::RegQueryValueEx(hKey, _T("sLongDate"), 0, &nType, (LPBYTE)pFormat, &nSize) && nType == REG_SZ ) 
				bResult = TRUE;
			else
				*pFormat = 0;
			::RegCloseKey( hKey );
		}
	}

	if ( !bResult ) {
		CString rcString;
		rcString.LoadString( IDS_WAS_DATEOUTPUTFOREDIT );
		if ( !rcString.GetLength() )
			rcString = _T("dddd, MMMM dd, yyyy");
		_tcsncpy( pFormat, rcString, nSize );
	}
}


// ---
void GetTimeEditFormat( TCHAR *pFormat, DWORD nSize ) {
  HKEY  hKey;
  DWORD nType = REG_SZ;
  BOOL  bResult = FALSE;
  
	*pFormat = 0;
	bResult = !!::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, pFormat, nSize );

	if ( !bResult ) {
		if ( ERROR_SUCCESS == ::RegOpenKey( HKEY_CURRENT_USER, _T("Control Panel\\International"), &hKey) ) {
			if ( ERROR_SUCCESS == ::RegQueryValueEx(hKey, _T("sTimeFormat"), 0, &nType, (LPBYTE)pFormat, &nSize) && nType == REG_SZ ) 
				bResult = TRUE;
			else
				*pFormat = 0;
			::RegCloseKey( hKey );
		}
	}

	if ( !bResult ) {
		CString rcString;
		rcString.LoadString( IDS_WAS_TIMEOUTPUTFOREDIT );
		if ( !rcString.GetLength() )
			rcString = "h:mm:ss";
		_tcsncpy( pFormat, rcString, nSize );
	}
}

// ---
void GetDateTimeEditFormat( TCHAR *pFormat, DWORD nSize ) {
  HKEY  hKey;
  DWORD nType = REG_SZ;
  BOOL  bResult = FALSE;
  
	TCHAR	pDateFormat[80] = {0};
	DWORD nDateSize = sizeof(pDateFormat);

	bResult = !!::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE , pDateFormat, nDateSize );

	if ( !bResult ) {
		if ( ERROR_SUCCESS == ::RegOpenKey( HKEY_CURRENT_USER, _T("Control Panel\\International"), &hKey) ) {
			if ( ERROR_SUCCESS == ::RegQueryValueEx(hKey, _T("sLongDate"), 0, &nType, (LPBYTE)pDateFormat, &nDateSize) && nType == REG_SZ ) 
				bResult = TRUE;
			else
				*pFormat = 0;
			::RegCloseKey( hKey );
		}
	}

	if ( !bResult ) {
		CString rcString;
		rcString.LoadString( IDS_WAS_DATEOUTPUTFOREDIT );
		if ( !rcString.GetLength() )
			rcString = "dddd, MMMM dd, yyyy";
		_tcsncpy( pDateFormat, rcString, nDateSize );
	}

	TCHAR	pTimeFormat[80];
	DWORD nTimeSize = sizeof(pTimeFormat);

	bResult = !!::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, pTimeFormat, nTimeSize );

	if ( !bResult ) {
		if ( ERROR_SUCCESS == ::RegOpenKey( HKEY_CURRENT_USER, _T("Control Panel\\International"), &hKey) ) {
			if ( ERROR_SUCCESS == ::RegQueryValueEx(hKey, _T("sTimeFormat"), 0, &nType, (LPBYTE)pTimeFormat, &nTimeSize) && nType == REG_SZ ) 
				bResult = TRUE;
			else
				*pFormat = 0;
			::RegCloseKey( hKey );
		}
	}

	if ( !bResult ) {
		CString rcString;
		rcString.LoadString( IDS_WAS_TIMEOUTPUTFOREDIT );
		if ( !rcString.GetLength() )
			rcString = _T("h:mm:ss");
		_tcsncpy( pTimeFormat, rcString, nTimeSize );
	}

	_tcsncpy( pFormat, pDateFormat, nSize );
	nSize -= TCSCLEN(pFormat);
	pFormat = _tcsninc(pFormat, TCSCLEN(pFormat));
	_tcsncpy( pFormat, _T("  "), nSize );
	nSize -= TCSCLEN(pFormat);
	pFormat = _tcsninc(pFormat, TCSCLEN(pFormat));
	_tcsncpy( pFormat, pTimeFormat, nSize );
}



// ---
struct TypeID {
	AVP_TYPE  m_nType;
	TCHAR     *m_pName;
};


// ---
static TypeID gTypes[] = {
  { avpt_nothing,	_T("avpt_nothing")	},
	{ avpt_char,		_T("avpt_char")			},
	{ avpt_wchar,		_T("avpt_wchar")		},
	{ avpt_short,		_T("avpt_short")		},
	{ avpt_long,		_T("avpt_long")			},
	{ avpt_byte,		_T("avpt_byte")			},
	{ avpt_group,		_T("avpt_group")		},
	{ avpt_word,		_T("avpt_word")			},
	{ avpt_dword,		_T("avpt_dword")		},
	{ avpt_int, 		_T("avpt_int") 			},
	{ avpt_uint, 		_T("avpt_uint") 		},
	{ avpt_bool,		_T("avpt_bool")			},
	{ avpt_date,		_T("avpt_date")			},
	{ avpt_time,		_T("avpt_time")			},
	{ avpt_datetime,_T("avpt_datetime")	},
	{ avpt_int,			_T("avpt_int")			},
	{ avpt_uint,		_T("avpt_uint")			},
	{ avpt_qword,		_T("avpt_qword")		},
	{ avpt_longlong,_T("avpt_longlong")	},
	{ avpt_str,			_T("avpt_str")			},
	{ avpt_wstr,		_T("avpt_wstr")			},
	{ avpt_bin,			_T("avpt_bin")			},
};


static int gnTypesCount = sizeof(gTypes) / sizeof(gTypes[0]);


// ---
TCHAR *TypeID2TypeName( int nType ) {
	for ( int i=0; i<gnTypesCount; i++ )
		if ( gTypes[i].m_nType == nType )
			return gTypes[i].m_pName;
  return NULL;
}

// ---
int	TypeName2TypeID( const TCHAR *pTypeName ) {
	for ( int i=0; i<gnTypesCount; i++ )
		if ( !_tcscmp(gTypes[i].m_pName, pTypeName) )
			return gTypes[i].m_nType;
  return 0;
}

// ---
int TypeNameMaxLength() {
	size_t nLength = 0;
	for ( int i=0; i<gnTypesCount; i++ )
		nLength = max(nLength, TCSCLEN(gTypes[i].m_pName));

	return int(nLength);
}
