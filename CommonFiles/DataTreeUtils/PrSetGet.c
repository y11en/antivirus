////////////////////////////////////////////////////////////////////
//
//  PrSetGet.c
//  Property set/get function module
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <limits.h>
#include "datatreeutils/dtutils.h"
#include "AVPPort.h"
#include "avpprpid.h"

#define VALID_STR(a) (a && *a)
#if !defined(_countof)
	#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif
extern void* (* DTUAllocator)(AVP_size_t);
extern void  (* DTULiberator)(void*);

#if defined(_UNICODE) || defined(UNICODE)
#define DTU_TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define DTU_TCSCLEN(p) (p ? strlen(p) : 0)
#endif

#define SIZEOF_TCHAR sizeof(TCHAR)


// ---
static int GetConvertBase( const TCHAR *pS ) {
	int nBase = -1;
	if ( pS && *pS ) {
		while ( _istspace((TCHAR)_tcsnextc(pS)) || _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
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
			TCHAR nNext = _tcsnextc( pS );
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
static long A2I( const TCHAR *pS, TCHAR **pEndPtr ) {
	int nBase;
	if ( pEndPtr )
		*pEndPtr = (TCHAR *)pS;
  while ( _istspace((TCHAR)_tcsnextc(pS)) )
		pS = _tcsinc(pS);
	nBase = GetConvertBase( pS );
	if ( nBase > 0 )
		return _tcstol( pS, pEndPtr, nBase );
	return 0;
}

// ---
static unsigned long A2UI( const TCHAR *pS, TCHAR **pEndPtr ) {
	int nBase;
	if ( pEndPtr )
		*pEndPtr = (TCHAR *)pS;
  while ( _istspace((TCHAR)_tcsnextc(pS)) )
		pS = _tcsinc(pS);
	nBase = GetConvertBase( pS );
	if ( nBase > 0 )
		return _tcstoul( pS, pEndPtr, nBase );
	return 0;
}

// ---
static AVP_bool IsHexDigit( int c ) {
	if ( !_istdigit((TCHAR)c) )
		return tolower(c) >= L'a' && tolower(c) <= L'f';
	else
		return TRUE;
}

// ---
static AVP_longlong  A2I64( const TCHAR *pS, TCHAR **ppLast, int nBase ) {
	__int64 iR = 0;
	if ( pS && *pS ) {
		while ( _istspace((TCHAR)_tcsnextc(pS)) )
			pS = _tcsinc(pS);
		if ( nBase == 16 ) {
			while ( _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
				pS = _tcsinc(pS);
			if ( _tcsnextc(pS) == L'0' && (_tcsnextc(_tcsinc(pS)) == L'x' || _tcsnextc(_tcsinc(pS)) == L'X') )
				pS = _tcsninc(pS, 2);
			while( IsHexDigit(_tcsnextc(pS)) ) {
				if ( !_istdigit((TCHAR)_tcsnextc(pS)) )
					iR = 16 * iR + (tolower(_tcsnextc(pS)) - L'a' + 10);
				else
					iR = 16 * iR + (_tcsnextc(pS) - L'0');
				pS = _tcsinc(pS);
			}
		}
		else {
			if ( nBase == 10 ) {
				TCHAR c;              /* current TCHAR */
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
					while( _istdigit((TCHAR)_tcsnextc(pS)) ) {
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
static AVP_qword  A2UI64( const TCHAR *pS, TCHAR **ppLast, int nBase ) {
	unsigned __int64 iR = 0;
	if ( pS && *pS ) {
		while ( _istspace((TCHAR)_tcsnextc(pS)) )
			pS = _tcsinc(pS);
		if ( nBase == 16 ) {
			while ( _tcsnextc(pS) == L'-' || _tcsnextc(pS) == L'+' )
				pS = _tcsinc(pS);
			if ( _tcsnextc(pS) == L'0' && (_tcsnextc(_tcsinc(pS)) == L'x' || _tcsnextc(_tcsinc(pS)) == L'X') )
				pS = _tcsninc(pS, 2);
			
			while( IsHexDigit(_tcsnextc(pS)) ) {
				if ( !_istdigit((TCHAR)_tcsnextc(pS)) )
					iR = 16 * iR + (tolower(_tcsnextc(pS)) - L'a' + 10);
				else
					iR = 16 * iR + (_tcsnextc(pS) - L'0');
				pS = _tcsinc(pS);
			}
		}
		else {
			if ( nBase == 10 ) {
				TCHAR c;              /* current TCHAR */
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
					while( _istdigit((TCHAR)_tcsnextc(pS)) ) {
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
	AVP_dword dwSize = 0;
	AVP_byte nValue;
	TCHAR *pBegin = (TCHAR *)pStr;
	TCHAR *pEnd;
	while ( *pBegin && !_istdigit((TCHAR)_tcsnextc(pBegin)) )
		pBegin = _tcsinc(pBegin);
	nValue = (AVP_byte)A2UI( pBegin, &pEnd );
	while( pEnd != pBegin ) {
		AVP_byte *pNewValue = DTUAllocator(SIZEOF_TCHAR * (dwSize + 1));
		if ( !pNewValue )
			break;

		if ( pValue )
			memcpy( pNewValue, pValue, dwSize );

		*(pNewValue + dwSize) = nValue;
		dwSize++;

		DTULiberator( pValue );
		pValue = pNewValue;

		pBegin = pEnd;
		while ( *pBegin && !_istdigit((TCHAR)_tcsnextc(pBegin)) )
			pBegin = _tcsinc(pBegin);
		nValue = (AVP_byte)A2UI( pBegin, &pEnd );
	}
	*pdwSize = dwSize;

	return pValue;
}

// ---
static TCHAR *Bin2Str( AVP_byte *pValue, AVP_dword dwSize ) {
	TCHAR *pStr = NULL;
	AVP_dword i;
	for ( i=0; i<dwSize; i++ ) {
		TCHAR *pNewStr;
		TCHAR pBuff[10];

		_stprintf( pBuff, _T("%#x"), pValue[i] );
		pNewStr = DTUAllocator(SIZEOF_TCHAR * (DTU_TCSCLEN(pStr) + DTU_TCSCLEN(pBuff) + 2));
		if ( !pNewStr )
			break;

		*pNewStr = L'\0';

		if ( pStr ) {
			_tcscpy( pNewStr, pStr );
			_tcscat( pNewStr, _T(" ") );
		}
		_tcscat( pNewStr, pBuff );

		DTULiberator( pStr );
		pStr = pNewStr;
	}
	return pStr;
}




// ---
DTUT_PROC int DTUT_PARAM DTUT_GetDataDatasCount( HDATA hData ) {
	int nCount = 0;
	HDATA hCurrData = DATA_Get_First( hData, 0 );
	while ( hCurrData ) {
		nCount++;
		hCurrData = DATA_Get_Next( hCurrData, 0 );
	}
	return nCount;
}

// ---
DTUT_PROC int DTUT_PARAM DTUT_GetDataPropsCount( HDATA hData ) {
	int nCount = 0;
	HPROP hCurrProp = DATA_Get_First_Prop( hData, 0 );
	while ( hCurrProp ) {
		nCount++;
		hCurrProp = PROP_Get_Next( hCurrProp );
	}
	return nCount;
}

// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetDataPropertyString( HDATA hData, AVP_dword nPID ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return 0;
	}
	else {
		TCHAR *pStr = 0;

		HPROP hProp;
		if ( (hProp = DATA_Find_Prop(hData, NULL, nPID)) != NULL ) 
			return DTUT_GetPropValueAsString( hProp, NULL );

		pStr = DTUAllocator( SIZEOF_TCHAR * 1 );
		if ( pStr )
			*pStr = L'\0';

		return pStr;
	}
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropValue( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat ) {
		// Заменить Value узла
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return;
	}
	else {
		const TCHAR *pMinStr = _T("Min");
		const TCHAR *pMaxStr = _T("Max");

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
#ifdef _UNICODE
				AVP_char pChar[2] = { 0, 0 };
				WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pChar, sizeof(pChar), NULL, NULL );
				PROP_Set_Val( hProp, (AVP_size_t)pChar[0], 0 );
#else
				AVP_char nValue;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else
						nValue = (AVP_char)pStr[0];
				PROP_Set_Val( hProp, nValue, 0 );
#endif
			}
				break;
			case avpt_wchar  : {
#ifdef _UNICODE
				TCHAR nValue;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					nValue = pStr[0];
				PROP_Set_Val( hProp, nValue, 0 );
#else
				AVP_wchar pWChar[2] = { 0, 0 };
				MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pWChar, sizeof(pWChar) );
				PROP_Set_Val( hProp, (AVP_size_t)pWChar[0], 0 );
#endif
			}
				break;
			case avpt_short  : {
				AVP_short nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = SHRT_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = SHRT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_short)A2I(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_long   : {
				AVP_long nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = LONG_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = LONG_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2I(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = _I64_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = _I64_MAX;
					else 
						nValue = A2I64(pStr, NULL, GetConvertBase(pStr));
				PROP_Set_Val( hProp, (AVP_size_t)&nValue, sizeof(nValue) );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_byte)A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_word   : {
				AVP_word nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = USHRT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_word)A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = ULONG_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = _UI64_MAX;
					else 
						nValue = A2UI64(pStr, NULL, GetConvertBase(pStr));
				PROP_Set_Val( hProp, (AVP_size_t)&nValue, sizeof(nValue) );
			}
				break;
			case avpt_int   : {
				AVP_int nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = INT_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = INT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2I(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = UINT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_group   : {
				AVP_group nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = (AVP_group)A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_date   : {
				AVP_date nValue;
				SYSTEMTIME rcSysTime;
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_tcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
				}
				else
					if ( !_tcsicmp(pMaxStr, pStr) ) {
						rcSysTime.wYear  = 2038;
						rcSysTime.wMonth = 1;
						rcSysTime.wDay	 = 18;
					}
				CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
				PROP_Set_Val( hProp, (AVP_size_t)nValue, 0 );
			}
				break;
			case avpt_time   : {
				AVP_time nValue;
				SYSTEMTIME rcSysTime;
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_tcsicmp(pMinStr, pStr) ) {
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_tcsicmp(pMaxStr, pStr) ) {
						rcSysTime.wHour  = 23;
						rcSysTime.wMinute = 59;
						rcSysTime.wSecond	 = 59;
						rcSysTime.wMilliseconds = 1;
					}
				CopyMemory( &nValue, &rcSysTime.wHour, sizeof(nValue) );
				PROP_Set_Val( hProp, (AVP_size_t)nValue, 0 );
			}
				break;
			case avpt_datetime   : {
				AVP_datetime nValue;
				SYSTEMTIME rcSysTime;
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_tcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_tcsicmp(pMaxStr, pStr) ) {
						rcSysTime.wYear  = 2038;
						rcSysTime.wMonth = 1;
						rcSysTime.wDay	 = 18;
						rcSysTime.wHour  = 23;
						rcSysTime.wMinute = 59;
						rcSysTime.wSecond	 = 59;
						rcSysTime.wMilliseconds = 1;
					}
				CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
				PROP_Set_Val( hProp, (AVP_size_t)nValue, 0 );
			}
				break;
			case avpt_str    : {
#ifdef _UNICODE
				AVP_str pMStr;
				int nSize = WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0, NULL, NULL );
				pMStr = DTUAllocator( SIZEOF_TCHAR * nSize);
				if ( pMStr ) {
					WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pMStr, nSize, NULL, NULL );
					PROP_Set_Val( hProp, (AVP_size_t)pMStr, 0 );
					DTULiberator( pMStr );
				}
#else
				PROP_Set_Val( hProp, (AVP_size_t)pStr, 0 );
#endif
			}
				break;
			case avpt_wstr   : {
#ifdef _UNICODE
				PROP_Set_Val( hProp, (AVP_size_t)pStr, 0 );
#else
				AVP_wstr pWStr;
				int nSize = MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0 ) + 1;
				pWStr = DTUAllocator( sizeof(AVP_wchar) * nSize );
				if ( pWStr ) {
					*pWStr = 0;
					MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pWStr, nSize );
					PROP_Set_Val( hProp, (AVP_size_t)pWStr, 0 );
					DTULiberator( pWStr );
				}
#endif
			}
				break;
			case avpt_bin : {
				AVP_dword dwSize;
				AVP_byte *pValue = Str2Bin( pStr, &dwSize );
				PROP_Set_Val( hProp, (AVP_size_t)pValue, dwSize );
				DTULiberator( pValue );
			}
				break;
		}
	}
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropNumericValue( HPROP hProp, AVP_dword nValueValue ) {
		// Заменить Value узла
	AVP_dword nID = PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue = (AVP_short)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_long   : {
			AVP_long nValue = (AVP_long)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue = (AVP_byte)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_word   : {
			AVP_word nValue = (AVP_word)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue = (AVP_size_t)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_int   : {
			AVP_int nValue = (AVP_int)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_uint   : {
			AVP_uint nValue = (AVP_uint)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue = (AVP_bool)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
		case avpt_group   : {
			AVP_group nValue = (AVP_group)nValueValue;
			PROP_Set_Val( hProp, nValue, 0 );
		}
			break;
	}
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropDateValue( HPROP hProp, const SYSTEMTIME *pSysTime ) {
		// Заменить Value узла
	AVP_dword nID = PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_date   : {
			AVP_date nValue;
			CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			PROP_Set_Val( hProp, (AVP_size_t)nValue, 0 );
		}
			break;
		case avpt_time   : {
			AVP_time nValue;
			CopyMemory( &nValue, &pSysTime->wHour, sizeof(nValue) );
			PROP_Set_Val( hProp, (AVP_size_t)nValue, 0 );
		}
			break;
		case avpt_datetime   : {
			AVP_datetime nValue;
			CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			PROP_Set_Val( hProp, (AVP_size_t)nValue, 0 );
		}
			break;
	}
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropArrayValue( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat ) {
		// Заменить Value узла
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "DTUT Library is not initialised" );
		return;
	}
	else {
		const TCHAR *pMinStr = _T("Min");
		const TCHAR *pMaxStr = _T("Max");

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
#ifdef _UNICODE
				AVP_char pChar[2] = { 0, 0 };
				WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pChar, sizeof(pChar), NULL, NULL );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &pChar[0], sizeof(AVP_char) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &pChar[0], sizeof(AVP_char) );
#else
				AVP_char nValue;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else
						nValue = (AVP_char)pStr[0];
					if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
						PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
					else
						PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
#endif
				}
				break;
			case avpt_wchar  : {
#ifdef _UNICODE
				TCHAR nValue;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else
						nValue = pStr[0];
					if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
						PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
					else
						PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
#else
					AVP_wchar pWChar[2] = { 0, 0 };
					MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pWChar, sizeof(pWChar) );
					if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
						PROP_Arr_Set_Items( hProp, nPos, &pWChar[0], sizeof(AVP_wchar) );
					else
						PROP_Arr_Insert( hProp, PROP_ARR_LAST, &pWChar[0], sizeof(AVP_wchar) );
#endif
				}
				break;
			case avpt_short  : {
				AVP_short nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = SHRT_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = SHRT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_short)A2I(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_long   : {
				AVP_long nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = LONG_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = LONG_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2I(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = _I64_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = _I64_MAX;
					else 
						nValue = A2I64(pStr, NULL, GetConvertBase(pStr));
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_byte)A2UI(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_word   : {
				AVP_word nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = USHRT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_word)A2UI(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = ULONG_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2UI(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = ULONG_MAX;
					else 
						nValue = A2UI64(pStr, NULL, GetConvertBase(pStr));
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_int   : {
				AVP_int nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = INT_MIN;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = INT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2I(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue = 0;
				if ( !_tcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_tcsicmp(pMaxStr, pStr) )
						nValue  = UINT_MAX;
					else 
						//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							_stscanf( pStr, pFormat, &nValue );
						else
							nValue = A2UI(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = A2UI(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_group   : {
				AVP_group nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = (AVP_group)A2UI(pStr, NULL);
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_date   : {
				AVP_date nValue;
				SYSTEMTIME rcSysTime;
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_tcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
				}
				else
					if ( !_tcsicmp(pMaxStr, pStr) ) {
						rcSysTime.wYear  = 2038;
						rcSysTime.wMonth = 1;
						rcSysTime.wDay	 = 18;
					}
				CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
			case avpt_time   : {
				AVP_time nValue;
				SYSTEMTIME rcSysTime;
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_tcsicmp(pMinStr, pStr) ) {
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_tcsicmp(pMaxStr, pStr) ) {
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
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_tcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_tcsicmp(pMaxStr, pStr) ) {
						rcSysTime.wYear  = 2038;
						rcSysTime.wMonth = 1;
						rcSysTime.wDay	 = 18;
						rcSysTime.wHour  = 23;
						rcSysTime.wMinute = 59;
						rcSysTime.wSecond	 = 59;
						rcSysTime.wMilliseconds = 1;
					}
				CopyMemory( &nValue, &rcSysTime, sizeof(nValue) );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
			case avpt_str    : {
#ifdef _UNICODE
				AVP_str pMStr;
				int nSize = WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0, NULL, NULL );
				pMStr = DTUAllocator( SIZEOF_TCHAR * nSize);
				WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pMStr, nSize, NULL, NULL );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, (void *)pMStr, 0 );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pMStr, 0 );
				DTULiberator( pMStr );
#else
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, (void *)pStr, 0 );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pStr, 0 );
#endif
			}
				break;
			case avpt_wstr   : {
#ifdef _UNICODE
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, (void *)pStr, 0 );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pStr, 0 );
#else
				AVP_wstr pWStr;
				int nSize = MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0 ) + 1;
				pWStr = DTUAllocator( sizeof(AVP_wchar) * nSize);
				if ( pWStr ) {
					*pWStr = 0;
					MultiByteToWideChar( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pWStr, nSize );
					if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
						PROP_Arr_Set_Items( hProp, nPos, (void *)pWStr, 0 );
					else
						PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pWStr, 0 );
					DTULiberator( pWStr );
				}
#endif
			}
				break;
			case avpt_bin : {
				AVP_dword dwSize;
				AVP_byte *pValue = Str2Bin( pStr, &dwSize );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, (void *)pValue, dwSize );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pValue, dwSize );
				DTULiberator( pValue );
			}
				break;
		}
	}
}

// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropArrayNumericValue( HPROP hProp, int nPos, AVP_dword nValueValue ) {
		// Заменить Value узла
	AVP_dword nID = PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue = (AVP_short)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_long   : {
			AVP_long nValue = (AVP_long)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue = (AVP_byte)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_word   : {
			AVP_word nValue = (AVP_word)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue = (AVP_size_t)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_int   : {
			AVP_int nValue = (AVP_int)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue = (AVP_uint)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue = (AVP_bool)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
		case avpt_group   : {
			AVP_group nValue = (AVP_group)nValueValue;
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
	}
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropDateArrayValue( HPROP hProp, int nPos, const SYSTEMTIME *pSysTime ) {
		// Заменить Value узла
	AVP_dword nID = PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_date   : {
			AVP_date nValue;
			CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
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
			CopyMemory( &nValue, pSysTime, sizeof(nValue) );
			if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
			  PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
			else
			  PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
		}
			break;
	}
}


// ---
static TCHAR *AnalizeDate( AVP_date *nValue ) {
	AVP_date nEmptyValue;
	SYSTEMTIME rcSysTime;
	time_t rcTime = 0L;
	TCHAR szBuffer[128] = {0};
	struct tm* ptmTemp;
	TCHAR *pResult;
	TCHAR pDateFormat[80];

	ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
	memcpy( &rcSysTime, *nValue, sizeof(*nValue) );

	if ( memcmp(&nValue, &nEmptyValue, sizeof(*nValue)) ) {
		if (rcSysTime.wYear >= 1900) {
			struct tm atm;
			atm.tm_sec = (int)rcSysTime.wSecond;
			atm.tm_min = (int)rcSysTime.wMinute;
			atm.tm_hour = (int)rcSysTime.wHour;
			_ASSERT((int)rcSysTime.wDay >= 1 && (int)rcSysTime.wDay <= 31);
			atm.tm_mday = (int)rcSysTime.wDay;
			atm.tm_wday = (int)rcSysTime.wDayOfWeek;
			_ASSERT((int)rcSysTime.wMonth >= 1 && (int)rcSysTime.wMonth <= 12);
			atm.tm_mon = (int)rcSysTime.wMonth - 1;        // tm_mon is 0 based
			_ASSERT(rcSysTime.wYear >= 1900);
			atm.tm_year = rcSysTime.wYear - 1900;     // tm_year is 1900 based
			atm.tm_isdst = -1;
			rcTime = mktime( &atm );
			_ASSERT(rcTime != -1);       // indicates an illegal input time
		}
	}
	else
		rcTime = time(0);

	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE , pDateFormat, _countof(pDateFormat) );
	if ( DTU_TCSCLEN(pDateFormat) ) {
		GetDateFormat( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, szBuffer, _countof(szBuffer) );
	}
	else {
		ptmTemp = localtime(&rcTime);
		if (ptmTemp == 0 ||
#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__LINUX__)))
			!_tcsftime(szBuffer, _countof(szBuffer), _T("%H:%M"), ptmTemp))
#else
			!_tcsftime(szBuffer, _countof(szBuffer), _T("%#x"), ptmTemp))
#endif
			szBuffer[0] = L'\0';
	}
	pResult = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(szBuffer) + 1) );
	if ( pResult )
		_tcscpy( pResult, szBuffer );

	return pResult;
}


// ---
static TCHAR *AnalizeTime( AVP_time *nValue ) {
	AVP_time nEmptyValue;
	SYSTEMTIME rcSysTime;
	time_t rcTime = 0L;
	TCHAR szBuffer[128] = {0};
	struct tm* ptmTemp;
	TCHAR *pResult;
	TCHAR pDateFormat[80];

	ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	ZeroMemory( &rcSysTime, sizeof(rcSysTime) );

	rcSysTime.wYear  = 1970;
	rcSysTime.wMonth = 12;
	rcSysTime.wDay	 = 31;

	memcpy( &rcSysTime.wHour, *nValue, sizeof(*nValue) );

	if ( memcmp(&nValue, &nEmptyValue, sizeof(*nValue)) ) {
		struct tm atm;
		atm.tm_sec = (int)rcSysTime.wSecond;
		atm.tm_min = (int)rcSysTime.wMinute;
		atm.tm_hour = (int)rcSysTime.wHour;
		_ASSERT((int)rcSysTime.wDay >= 1 && (int)rcSysTime.wDay <= 31);
		atm.tm_mday = (int)rcSysTime.wDay;
		atm.tm_wday = (int)rcSysTime.wDayOfWeek;
		_ASSERT((int)rcSysTime.wMonth >= 1 && (int)rcSysTime.wMonth <= 12);
		atm.tm_mon = (int)rcSysTime.wMonth - 1;        // tm_mon is 0 based
		_ASSERT(rcSysTime.wYear >= 1900);
		atm.tm_year = rcSysTime.wYear - 1900;     // tm_year is 1900 based
		atm.tm_isdst = -1;
		rcTime = mktime( &atm );
		_ASSERT(rcTime != -1);       // indicates an illegal input time
	}
	else
		rcTime = time(0);


	GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, pDateFormat, _countof(pDateFormat) );
	if ( DTU_TCSCLEN(pDateFormat) ) {
		GetTimeFormat( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, szBuffer, _countof(szBuffer) );
	}
	else {
		ptmTemp = localtime(&rcTime);
		if (ptmTemp == 0 ||
#if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || (defined (__LINUX__)))
			!_tcsftime(szBuffer, _countof(szBuffer), _T("%d.%m.%y"), ptmTemp))
#else
			!_tcsftime(szBuffer, _countof(szBuffer), _T("%X"), ptmTemp))
#endif
			szBuffer[0] = L'\0';
	}

	pResult = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(szBuffer) + 1) );
	if ( pResult )
		_tcscpy( pResult, szBuffer );

	return pResult;
}


// ---
static TCHAR *AnalizeDateTime( AVP_datetime *nValue ) {
	TCHAR *pDateValue = AnalizeDate( (AVP_date *)nValue );
	TCHAR *pTimeValue = AnalizeTime( (AVP_time *)&(((SYSTEMTIME *)nValue)->wHour) );

	TCHAR *pResult = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pDateValue) + DTU_TCSCLEN(pTimeValue) + 5) );
	if ( pResult ) {
		_tcscpy( pResult, pDateValue );
		_tcscat( pResult, _T("  ") );
		_tcscat( pResult, pTimeValue );
	}
	DTULiberator(pDateValue);
	DTULiberator(pTimeValue);

	return pResult;
}


// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetPropValueAsString( HPROP hProp, const TCHAR *pFormat ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return 0;
	}
	else {
		TCHAR *pResult = 0;

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
#ifdef _UNICODE
				int nSize;
				AVP_char pChar[2] = { 0, 0 };
				PROP_Get_Val( hProp, pChar, 2 );
				nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pChar, -1, NULL, 0 ) + 1;
				pResult = DTUAllocator(SIZEOF_TCHAR * nSize);
				if ( pResult ) {
					*pResult = 0;
					MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pChar, -1, pResult, nSize );
				}
#else
				AVP_char nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator(SIZEOF_TCHAR * 2);
				if ( pResult ) {
					*(pResult) = nValue;
					*(pResult + 1) = 0;
				}
#endif
			}
				break;
			case avpt_wchar  : {
#ifdef _UNICODE
				TCHAR nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator(SIZEOF_TCHAR * 2);
				if ( pResult ) {
					*(pResult) = nValue;
					*(pResult + 1) = 0;
				}
#else
				int nSize;
				AVP_wchar pWChar[2] = { 0, 0 };
				PROP_Get_Val( hProp, pWChar, 2 );
				nSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0, NULL, NULL );
				pResult = DTUAllocator( SIZEOF_TCHAR * nSize );
				WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize, NULL, NULL );
#endif
			}
				break;
			case avpt_short  : {
				AVP_short nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
			}
				break;
			case avpt_long   : {
				AVP_long nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 35 );
				if ( pResult )
					_i64tot( nValue, pResult, 10 );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_word   : {
				AVP_word nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 35 );
				if ( pResult )
					_i64tot( nValue, pResult, 16 );
			}
				break;
			case avpt_int   : {
				AVP_int nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_group  : {
				AVP_group nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult )
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_date   : {
				AVP_date nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = AnalizeDate( &nValue );
			}
				break;
			case avpt_time   : {
				AVP_time nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = AnalizeTime( &nValue );
			}
				break;
			case avpt_datetime   : {
				AVP_datetime nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = AnalizeDateTime( &nValue );
			}
				break;
			case avpt_str    : {
#ifdef _UNICODE
				int nSize = PROP_Get_Val( hProp, NULL, 0 ) + SIZEOF_TCHAR;
				if ( nSize ) {
					AVP_char *pWChar = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pWChar = 0;
					PROP_Get_Val( hProp, pWChar, nSize );
					nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
					pResult = DTUAllocator( SIZEOF_TCHAR * nSize );
					if ( pResult ) {
						*pResult = 0;
						MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
					}
					DTULiberator( pWChar );
				}
#else
				int nSize = PROP_Get_Val( hProp, NULL, 0 ) + SIZEOF_TCHAR;
				if ( nSize ) {
					pResult = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pResult = 0;
					PROP_Get_Val( hProp, pResult, nSize );
				}
#endif
			}
				break;
			case avpt_wstr   : {
#ifdef _UNICODE
				int nSize = PROP_Get_Val( hProp, NULL, 0 ) + SIZEOF_TCHAR;
				if ( nSize ) {
					pResult = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pResult = 0;
					PROP_Get_Val( hProp, pResult, nSize );
				}
#else
				int nSize = PROP_Get_Val( hProp, NULL, 0 ) + sizeof(AVP_wchar);
				if ( nSize ) {
					AVP_wchar *pWChar = DTUAllocator( sizeof(AVP_wchar) * nSize );
					*pWChar = 0;
					PROP_Get_Val( hProp, pWChar, nSize );
					nSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0, NULL, NULL );
					pResult = DTUAllocator( SIZEOF_TCHAR * nSize );
					WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize, NULL, NULL );
					DTULiberator( pWChar );
				}
#endif
			}
				break;
			case avpt_bin : {
				AVP_dword nSize = PROP_Get_Val( hProp, NULL, 0 );
				if ( nSize ) {
					AVP_byte *pValue = DTUAllocator(sizeof(AVP_byte) * nSize);
					if ( pResult ) {
						PROP_Get_Val( hProp, pValue, nSize );
						pResult = Bin2Str( pValue, nSize );
						DTULiberator( pValue );
					}
				}
			}
				break;
		}
		return pResult;
	}
}


// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetPropArrayValueAsString( HPROP hProp, int nPos, const TCHAR *pFormat ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "DTUT Library is not initialised" );
		return 0;
	}
	else {
		TCHAR *pResult = 0;

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
#ifdef _UNICODE
				int nSize;
				AVP_char pWChar[2] = { 0, 0 };
				PROP_Arr_Get_Items( hProp, nPos, pWChar, 2 );
				nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
				pResult = DTUAllocator( SIZEOF_TCHAR * nSize );
				if ( pResult ) {
					*pResult = 0;
					MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
				}
#else
				AVP_char nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(AVP_byte) * 2 );
				if ( pResult ) {
					*(pResult) = nValue;
					*(pResult + 1) = 0;
				}
#endif
			}
			  break;
			case avpt_wchar  : {
#ifdef _UNICODE
				TCHAR nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(AVP_byte) * 2 );
				if ( pResult ) {
					*(pResult) = nValue;
					*(pResult + 1) = 0;
				}
#else
				int nSize;
				AVP_wchar pWChar[2] = { 0, 0 };
				PROP_Arr_Get_Items( hProp, nPos, pWChar, 2 );
				nSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0, NULL, NULL );
				pResult = DTUAllocator( SIZEOF_TCHAR * nSize );
				WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize, NULL, NULL );
#endif
			}
				break;
			case avpt_short  : {
				AVP_short nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
			}
				break;
			case avpt_long   : {
				AVP_long nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 35 );
				if ( pResult ) 
					_i64tot( nValue, pResult, 10 );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_word   : {
				AVP_word nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 35 );
				if ( pResult ) 
					_i64tot( nValue, pResult, 16 );
			}
				break;
			case avpt_int   : {
				AVP_int nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%d"), nValue );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_group  : {
				AVP_group nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( SIZEOF_TCHAR * 12 );
				if ( pResult ) 
					_stprintf( pResult, VALID_STR(pFormat) ? pFormat : _T("%u"), nValue );
			}
				break;
			case avpt_date   : {
				AVP_date nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = AnalizeDate( &nValue );
			}
				break;
			case avpt_time   : {
				AVP_time nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = AnalizeDate( &nValue );
			}
				break;
			case avpt_datetime   : {
				AVP_datetime nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = AnalizeDateTime( &nValue );
			}
				break;
			case avpt_str    : {
#ifdef _UNICODE
				int nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + SIZEOF_TCHAR;
				if ( nSize ) {
					AVP_char *pWChar = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pWChar = 0;
					PROP_Arr_Get_Items( hProp, nPos, pWChar, nSize );
					nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
					pResult = DTUAllocator( SIZEOF_TCHAR * nSize );
					if ( pResult ) {
						*pResult = 0;
						MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
					}
					DTULiberator( pWChar );
				}
#else
				int nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + SIZEOF_TCHAR;
				pResult = DTUAllocator( sizeof(AVP_byte) *nSize );
				*pResult = 0;
				PROP_Arr_Get_Items( hProp, nPos, pResult, nSize );
#endif
			}
				break;
			case avpt_wstr   : {
#ifdef _UNICODE
				int nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + SIZEOF_TCHAR;
				pResult = DTUAllocator( sizeof(AVP_byte) *nSize );
				*pResult = 0;
				PROP_Arr_Get_Items( hProp, nPos, pResult, nSize );
#else
				int nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + sizeof(AVP_wchar);
				if ( nSize ) {
					AVP_wchar *pWChar = DTUAllocator( sizeof(AVP_wchar) * nSize );
					*pWChar = 0;
					PROP_Arr_Get_Items( hProp, nPos, pWChar, nSize );
					nSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0, NULL, NULL );
					pResult = DTUAllocator( SIZEOF_TCHAR *nSize );
					*pResult = 0;
					WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize, NULL, NULL );
					DTULiberator( pWChar );
				}
#endif
			}
				break;
			case avpt_bin : {
				AVP_dword nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 );
				if ( nSize ) {
					AVP_byte *pValue = DTUAllocator(sizeof(AVP_byte) * nSize);
					if ( pResult ) {
						PROP_Arr_Get_Items( hProp, nPos, pValue, nSize );
						pResult = Bin2Str( pValue, nSize );
						DTULiberator( pValue );
					}
				}
			}
				break;
		}
		return pResult;
	}
}


// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetDataValueAsComboString( HDATA hData ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return 0;
	}
	else {
		TCHAR *pResult = 0;

		HPROP hProp = DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
		if ( hProp ) {
			TCHAR *pDataText = DTUT_GetPropValueAsString( DATA_Find_Prop(hData, NULL, 0), 0 );
			AVP_dword nCount = PROP_Arr_Count( hProp );
			AVP_dword i;
			for ( i=0; i<nCount; i++ ) {
				TCHAR *pArrItemText = DTUT_GetPropArrayValueAsString( hProp, i, 0 );
				if ( !_tcscmp(pDataText, pArrItemText) ) {
					pResult = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pDataText) + 1) );
					if ( pResult ) 
					  _tcscpy( pResult, pDataText );
					break;
				}
				DTULiberator( pArrItemText );
			}
			DTULiberator( pDataText );
		}

		return pResult;
	}
}

// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetDataNumericValueAsComboString( HDATA hData ) {
	TCHAR *pResult = 0;

	HPROP hProp = DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			AVP_dword nValue = 0;
			HPROP hComboProp = DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				hProp = DATA_Find_Prop( hData, NULL, 0 );
				nValue = DTUT_GetPropNumericValueAsDWord( hProp );

				pResult = DTUT_GetPropArrayValueAsString( hComboProp, nValue, 0 );
			}
		}
	}

	return pResult;
}


// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetDataArrayNumericValueAsComboString( HDATA hData, int nPos ) {
	TCHAR *pResult = 0;

	HPROP hProp = DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			AVP_dword nValue = 0;
			HPROP hComboProp = DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				hProp = DATA_Find_Prop( hData, NULL, 0 );
				nValue = DTUT_GetPropArrayNumericValueAsDWord( hProp, nPos );

				pResult = DTUT_GetPropArrayValueAsString( hComboProp, nValue, 0 );
			}
		}
	}

	return pResult;
}


// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_SetDataNumericValueAsComboString( HDATA hData, const TCHAR *pStr ) {
	AVP_bool bResult = FALSE;

	HPROP hProp = DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			HPROP hComboProp = DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				AVP_bool bFound = FALSE;
				HPROP hProp = DATA_Find_Prop( hData, NULL, 0 );
				int nCount = PROP_Arr_Count( hComboProp );
				int i;
				for ( i=0; i<nCount && !bFound; i++ ) {
					TCHAR *pItemText = DTUT_GetPropArrayValueAsString( hComboProp, i, 0 );
					bFound = !_tcscmp( pStr, pItemText );
					if ( bFound ) 
						DTUT_SetPropNumericValue( hProp, i );

					DTULiberator( pItemText );
				}
				if ( !bFound ) 
					DTUT_SetPropNumericValue( hProp, 0 );

				bResult = TRUE;
			}
		}
	}

	return bResult;
}


// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_SetDataArrayNumericValueAsComboString( HDATA hData, int nPos, const TCHAR *pStr ) {
	AVP_bool bResult = FALSE;

	HPROP hProp = DATA_Find_Prop( hData, NULL, PP_PID_VALUEASINDEX );
	if ( hProp ) {
		AVP_bool nValueAsIndex;
		DATA_Get_Val( hData, NULL, PP_PID_VALUEASINDEX, &nValueAsIndex, sizeof(nValueAsIndex) );

		if ( nValueAsIndex ) {
			HPROP hComboProp = DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
			if ( hComboProp ) {
				AVP_bool bFound = FALSE;
				HPROP hProp = DATA_Find_Prop( hData, NULL, 0 );
				int nCount = PROP_Arr_Count( hComboProp );
				int i;
				for ( i=0; i<nCount && !bFound; i++ ) {
					TCHAR *pItemText = DTUT_GetPropArrayValueAsString( hComboProp, i, 0 );
					bFound = !_tcscmp( pStr, pItemText );
					if ( bFound ) 
						DTUT_SetPropArrayNumericValue( hProp, nPos, i );

					DTULiberator( pItemText );
				}
				if ( !bFound ) 
					DTUT_SetPropArrayNumericValue( hProp, nPos, 0 );

				bResult = TRUE;
			}
		}
	}

	return bResult;
}


// ---
DTUT_PROC AVP_dword DTUT_PARAM DTUT_GetPropNumericValueAsDWord( HPROP hProp ) {
	AVP_dword nResult = 0;

	AVP_dword nID = PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_long   : {
			AVP_long nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_word   : {
			AVP_word nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_int   : {
			AVP_int nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_group : {
			AVP_group nValue;
			PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
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
DTUT_PROC AVP_dword DTUT_PARAM DTUT_GetPropArrayNumericValueAsDWord( HPROP hProp, int nPos ) {
	AVP_dword nResult = 0;

	AVP_dword nID = PROP_Get_Id( hProp );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	switch ( nType ) {
		case avpt_short  : {
			AVP_short nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_long   : {
			AVP_long nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_bool   : {
			AVP_bool nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_byte   : {
			AVP_byte nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_word   : {
			AVP_word nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_dword  : {
			AVP_dword nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_int   : {
			AVP_int nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_uint  : {
			AVP_uint nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
			break;
		case avpt_group : {
			AVP_group nValue;
			PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
			nResult = nValue;
		}
		case avpt_nothing  : {
			nResult = 1;
		}
			break;
	}
	return nResult;
}

// ---
DTUT_PROC int DTUT_PARAM	DTUT_CompareValueInRangeH( HDATA hData, const TCHAR *pStr, 
												  HPROP hMinProp, HPROP hMaxProp, const TCHAR *pFormat ) {
	int nResult = 0;

	if ( hMinProp || hMaxProp )	{
		AVP_dword nID   = DATA_Get_Id( hData, 0 );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);

		switch ( nType ) {
			case avpt_short  : {
				AVP_short nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = (AVP_short)A2I(pStr, NULL);
				if ( hMinProp ) {
					AVP_short nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_short nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
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
					nValue = A2I(pStr, NULL);
				if ( hMinProp ) {
					AVP_long nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_long nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue = 0;
				nValue = A2I64(pStr, NULL, GetConvertBase(pStr));
				if ( hMinProp ) {
					AVP_longlong nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_longlong nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = A2UI(pStr, NULL);
				if ( hMinProp ) {
					AVP_bool nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_bool nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = (AVP_byte)A2UI(pStr, NULL);
				if ( hMinProp ) {
					AVP_byte nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_byte nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_word   : {
				AVP_word nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = (AVP_word)A2UI(pStr, NULL);
				if ( hMinProp ) {
					AVP_word nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_word nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = A2UI(pStr, NULL);
				if ( hMinProp ) {
					AVP_dword nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_dword nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue = 0;
				nValue = A2UI64(pStr, NULL, GetConvertBase(pStr));
				if ( hMinProp ) {
					AVP_qword nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_qword nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_int   : {
				AVP_int nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = A2I(pStr, NULL);
				if ( hMinProp ) {
					AVP_int nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_int nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue = 0;
				//_stscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					_stscanf( pStr, pFormat, &nValue );
				else
					nValue = A2UI(pStr, NULL);
				if ( hMinProp ) {
					AVP_uint nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = nValue >= nMValue ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_uint nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = nValue <= nMValue ? 0 : 1;
				}
			}
				break;
		}
	}

	return nResult;
}

// ---
DTUT_PROC int DTUT_PARAM	DTUT_CompareValueInRangeI( HDATA hData, const TCHAR *pStr, 
												  AVP_dword nMinID, AVP_dword nMaxID, const TCHAR *pFormat ) {

	AVP_dword nID   = DATA_Get_Id( hData, 0 );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	AVP_dword nApp  = GET_AVP_PID_APP(nID);

	AVP_dword nMinPid = MAKE_AVP_PID( nMinID, nApp, nType, 0);  
	AVP_dword nMaxPid = MAKE_AVP_PID( nMaxID, nApp, nType, 0);  

	HPROP hMinProp = DATA_Find_Prop( hData, NULL, nMinPid ); 
	HPROP hMaxProp = DATA_Find_Prop( hData, NULL, nMaxPid ); 

	return DTUT_CompareValueInRangeH( hData, pStr, hMinProp, hMaxProp, pFormat );
}


// ---
DTUT_PROC int DTUT_PARAM	DTUT_CompareDateValueInRangeH( HDATA hData, const SYSTEMTIME *pValueTime, 
														  HPROP hMinProp, HPROP hMaxProp ) {
	int nResult = 0;

	if ( hMinProp || hMaxProp )	{
		AVP_dword nID   = DATA_Get_Id( hData, 0 );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);

		switch ( nType ) {
			case avpt_date   : {
				if ( hMinProp ) {
					AVP_date nMValue;
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) >= 0 ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_date nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
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
					PROP_Get_Val( hMinProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) >= 0 ? 0 : -1;
				}
				if ( !nResult && hMaxProp ) {
					AVP_datetime nMValue;
					PROP_Get_Val( hMaxProp, &nMValue, sizeof(nMValue) );
					nResult = memcmp(pValueTime, &nMValue, sizeof(nMValue)) <= 0  ? 0 : 1;
				}
			}
				break;
		}
	}

	return nResult;
}


// ---
DTUT_PROC int DTUT_PARAM	DTUT_CompareDateValueInRangeI( HDATA hData, const SYSTEMTIME *pValueTime, 
														  AVP_dword nMinID, AVP_dword nMaxID ) {

	AVP_dword nID   = DATA_Get_Id( hData, 0 );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	AVP_dword nApp  = GET_AVP_PID_APP(nID);

	AVP_dword nMinPid = MAKE_AVP_PID( nMinID, nApp, nType, 0);  
	AVP_dword nMaxPid = MAKE_AVP_PID( nMaxID, nApp, nType, 0);  

	HPROP hMinProp = DATA_Find_Prop( hData, NULL, nMinPid ); 
	HPROP hMaxProp = DATA_Find_Prop( hData, NULL, nMaxPid ); 

	return DTUT_CompareDateValueInRangeH( hData, pValueTime, hMinProp, hMaxProp );
}


// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetPropValueAsStringCC( HPROP hProp, const TCHAR *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	TCHAR *pResult = DTUT_GetDataNumericValueAsComboString( hData );

	if ( !pResult )
		pResult = DTUT_GetPropValueAsString( hProp, pFormat );

	return pResult;
}

// ---
DTUT_PROC TCHAR *DTUT_PARAM DTUT_GetPropArrayValueAsStringCC( HPROP hProp, int nPos, const TCHAR *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	TCHAR *pResult = DTUT_GetDataArrayNumericValueAsComboString( hData, nPos );

	if ( !pResult )
		pResult = DTUT_GetPropArrayValueAsString( hProp, nPos, pFormat );

	return pResult;
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropValueCC( HPROP hProp, const TCHAR *pStr, const TCHAR *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	if ( !DTUT_SetDataNumericValueAsComboString(hData, pStr) )
		DTUT_SetPropValue( hProp, pStr, pFormat );
}


// ---
DTUT_PROC void DTUT_SetPropArrayValueCC( HPROP hProp, int nPos, const TCHAR *pStr, const TCHAR *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	if ( !DTUT_SetDataArrayNumericValueAsComboString(hData, nPos, pStr) )
		DTUT_SetPropArrayValue( hProp, nPos, pStr, pFormat );
}

// ---
DTUT_PROC HDATA DTUT_PARAM DTUT_CreateNodesSequence( HDATA hStartData, AVP_dword *pAddrSequence ) {
	HDATA hResult = hStartData;
	while ( *pAddrSequence ) {
		AVP_dword *pNewSeq = DATA_Alloc_Sequence( *pAddrSequence, 0 );

		HDATA hFindData = DATA_Find( hResult, pNewSeq );
		if ( !hFindData ) 
			hResult = DATA_Add( hResult, 0, *pAddrSequence, 0, 0 );
		else
			hResult = hFindData;
		pAddrSequence++;

		DATA_Remove_Sequence( pNewSeq );
	}

	return hResult;
}

