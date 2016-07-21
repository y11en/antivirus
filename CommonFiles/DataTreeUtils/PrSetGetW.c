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

#if !defined(_UNICODE)
#define _wcsnextc(_cpc) ((unsigned int) *(_cpc))
#define _wcsninc(_pc, _sz) (((_pc)+(_sz)))
#define _wcsinc(_pc)    ((_pc)+1)
_CRTIMP wchar_t*  __cdecl wcscat(wchar_t *, const wchar_t *);
#endif

#define CONVERT_BUFF_LEN 12

// ---
static int GetConvertBase( const wchar_t *pS ) {
	int nBase = -1;
	if ( pS && *pS ) {
		while ( _istspace((wchar_t)_wcsnextc(pS)) || _wcsnextc(pS) == L'-' || _wcsnextc(pS) == L'+' )
			pS = _wcsinc(pS);
		if ( _wcsnextc(pS) == L'0' ) {
			int nNext = _wcsnextc( _wcsninc(pS, 1) );
			if ( nNext == L'x' || nNext == L'X' )
				return 16;
			nBase = 8;
			while ( _wcsnextc(pS) != L'\0' ) {
				int nNext = _wcsnextc( pS );
				if ( nNext >= L'8' && nNext <= L'9' ) {
					if ( nBase < 10 )
						nBase = 10;
				}
				if ( (nNext >= L'a' && nNext <= L'f') || (nNext >= L'A' && nNext <= L'F') ) 
					nBase = 16;
				pS = _wcsinc(pS);
			}
			return nBase;
		}
		nBase = 10;
		while ( _wcsnextc(pS) != L'\0' ) {
			TCHAR nNext = _wcsnextc( pS );
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
static long A2I( const wchar_t *pS, wchar_t **pEndPtr ) {
	int nBase;
	if ( pEndPtr )
		*pEndPtr = (wchar_t *)pS;
  while ( _istspace((TCHAR)_wcsnextc(pS)) )
		pS = _wcsinc(pS);
	nBase = GetConvertBase( pS );
	if ( nBase > 0 )
		return wcstol( pS, pEndPtr, nBase );
	return 0;
}

// ---
static unsigned long A2UI( const wchar_t *pS, wchar_t **pEndPtr ) {
	int nBase;
	if ( pEndPtr )
		*pEndPtr = (wchar_t *)pS;
  while ( _istspace((TCHAR)_wcsnextc(pS)) )
		pS = _wcsinc(pS);
	nBase = GetConvertBase( pS );
	if ( nBase > 0 )
		return wcstoul( pS, pEndPtr, nBase );
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
static AVP_longlong  A2I64( const wchar_t *pS, wchar_t **ppLast, int nBase ) {
	__int64 iR = 0;
	if ( pS && *pS ) {
		while ( _istspace((TCHAR)_wcsnextc(pS)) )
			pS = _wcsinc(pS);
		if ( nBase == 16 ) {
			while ( _wcsnextc(pS) == L'-' || _wcsnextc(pS) == L'+' )
				pS = _wcsinc(pS);
			if ( _wcsnextc(pS) == L'0' && (_wcsnextc(_wcsinc(pS)) == L'x' || _wcsnextc(_wcsinc(pS)) == L'X') )
				pS = _wcsninc(pS, 2);
			while( IsHexDigit(_wcsnextc(pS)) ) {
				if ( !_istdigit((TCHAR)_wcsnextc(pS)) )
					iR = 16 * iR + (tolower(_wcsnextc(pS)) - L'a' + 10);
				else
					iR = 16 * iR + (_wcsnextc(pS) - L'0');
				pS = _wcsinc(pS);
			}
		}
		else {
			if ( nBase == 10 ) {
				TCHAR c;              /* current wchar_t */
				int sign;           /* if '-', then negative, otherwise positive */
				
				c = _wcsnextc(pS);
				sign = c;           /* save sign indication */
				if (c == L'-' || c == L'+') {
					c = _wcsnextc(pS = _wcsinc(pS));    /* skip sign */
				}
				
				while ( _istdigit(c) ) {
					iR = 10 * iR + (c - L'0');     /* accumulate digit */
					c = _wcsnextc(pS = _wcsinc(pS));    /* get next wchar_t */
				}
				
				if (sign == L'-')
					iR = -iR;
			}
			else {
				if ( nBase == 8 ) {
					while ( _wcsnextc(pS) == L'-' || _wcsnextc(pS) == L'+' )
						pS = _wcsinc(pS);
					pS = _wcsinc(pS);
					while( _istdigit((TCHAR)_wcsnextc(pS)) ) {
						iR = 8 * iR + (_wcsnextc(pS) - L'0');
						pS = _wcsinc(pS);
					}
				}
			}
		}
	}
	if ( ppLast )
		*ppLast = (wchar_t *)pS;
	return iR;
}

// ---
static AVP_qword  A2UI64( const wchar_t *pS, wchar_t **ppLast, int nBase ) {
	unsigned __int64 iR = 0;
	if ( pS && *pS ) {
		while ( _istspace((TCHAR)_wcsnextc(pS)) )
			pS = _wcsinc(pS);
		if ( nBase == 16 ) {
			while ( _wcsnextc(pS) == L'-' || _wcsnextc(pS) == L'+' )
				pS = _wcsinc(pS);
			if ( _wcsnextc(pS) == L'0' && (_wcsnextc(_wcsinc(pS)) == L'x' || _wcsnextc(_wcsinc(pS)) == L'X') )
				pS = _wcsninc(pS, 2);
			
			while( IsHexDigit(_wcsnextc(pS)) ) {
				if ( !_istdigit((TCHAR)_wcsnextc(pS)) )
					iR = 16 * iR + (tolower(_wcsnextc(pS)) - L'a' + 10);
				else
					iR = 16 * iR + (_wcsnextc(pS) - L'0');
				pS = _wcsinc(pS);
			}
		}
		else {
			if ( nBase == 10 ) {
				TCHAR c;              /* current wchar_t */
				int sign;           /* if '-', then negative, otherwise positive */
				
				c = _wcsnextc(pS);
				sign = c;           /* save sign indication */
				if (c == L'-' || c == L'+')
					c = _wcsnextc(pS = _wcsinc(pS));    /* skip sign */
				
				while ( _istdigit(c) ) {
					iR = 10 * iR + (c - L'0');     /* accumulate digit */
					c = _wcsnextc(pS = _wcsinc(pS));    /* get next wchar_t */
				}
				
				if ( sign == L'-' )
					iR *= -1;//-iR;
				
			}
			else {
				if ( nBase == 8 ) {
					while ( _wcsnextc(pS) == L'-' || _wcsnextc(pS) == L'+' )
						pS = _wcsinc(pS);
					pS = _wcsinc(pS);
					while( _istdigit((TCHAR)_wcsnextc(pS)) ) {
						iR = 8 * iR + (_wcsnextc(pS) - L'0');
						pS = _wcsinc(pS);
					}
				}
			}
		}
	}
	if ( ppLast )
		*ppLast = (wchar_t *)pS;
	return iR;
}

// ---
static AVP_byte *Str2Bin( const wchar_t *pStr, AVP_dword *pdwSize ) {
	AVP_byte *pValue = NULL;
	AVP_dword dwSize = 0;
	AVP_byte nValue;
	wchar_t *pBegin = (wchar_t *)pStr;
	wchar_t *pEnd;
	while ( *pBegin && !_istdigit((TCHAR)_wcsnextc(pBegin)) )
		pBegin = _wcsinc(pBegin);
	nValue = (AVP_byte)A2UI( pBegin, &pEnd );
	while( pEnd != pBegin ) {
		AVP_byte *pNewValue = DTUAllocator(sizeof(wchar_t) * (dwSize + 1));
		if ( !pNewValue )
			break;

		if ( pValue )
			memcpy( pNewValue, pValue, dwSize );

		*(pNewValue + dwSize) = nValue;
		dwSize++;

		DTULiberator( pValue );
		pValue = pNewValue;

		pBegin = pEnd;
		while ( *pBegin && !_istdigit((TCHAR)_wcsnextc(pBegin)) )
			pBegin = _wcsinc(pBegin);
		nValue = (AVP_byte)A2UI( pBegin, &pEnd );
	}
	*pdwSize = dwSize;

	return pValue;
}

// ---
static wchar_t *Bin2Str( AVP_byte *pValue, AVP_dword dwSize ) {
	wchar_t *pStr = NULL;
	AVP_dword i;
	for ( i=0; i<dwSize; i++ ) {
		wchar_t *pNewStr;
		wchar_t pBuff[10];

		swprintf( pBuff, L"%#x", pValue[i] );
		pNewStr = DTUAllocator(sizeof(wchar_t) * (wcslen(pStr) + wcslen(pBuff) + 2));
		if ( !pNewStr )
			break;

		*pNewStr = L'\0';

		if ( pStr ) {
			wcscpy( pNewStr, pStr );
			wcscat( pNewStr, L" " );
		}
		wcscat( pNewStr, pBuff );

		DTULiberator( pStr );
		pStr = pNewStr;
	}
	return pStr;
}




// ---
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetDataPropertyStringW( HDATA hData, AVP_dword nPID ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return 0;
	}
	else {
		wchar_t *pStr = 0;

		HPROP hProp;
		if ( (hProp = DATA_Find_Prop(hData, NULL, nPID)) != NULL ) 
			return DTUT_GetPropValueAsStringW( hProp, NULL );

		pStr = DTUAllocator( sizeof(wchar_t) * 1 );
		if ( pStr )
			*pStr = L'\0';

		return pStr;
	}
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropValueW( HPROP hProp, const wchar_t *pStr, const wchar_t *pFormat ) {
		// Заменить Value узла
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return;
	}
	else {
		const wchar_t *pMinStr = L"Min";
		const wchar_t *pMaxStr = L"Max";

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
				AVP_char pChar[2] = { 0, 0 };
				WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pChar, sizeof(pChar), NULL, NULL );
				PROP_Set_Val( hProp, (AVP_size_t)pChar[0], 0 );
			}
				break;
			case avpt_wchar  : {
				wchar_t nValue;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					nValue = pStr[0];
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_short  : {
				AVP_short nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = SHRT_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = SHRT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_short)A2I(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_long   : {
				AVP_long nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = LONG_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = LONG_MAX;
					else 
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = A2I(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = _I64_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = _I64_MAX;
					else 
						nValue = A2I64(pStr, NULL, GetConvertBase(pStr));
				PROP_Set_Val( hProp, (AVP_size_t)&nValue, sizeof(nValue) );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_byte)A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_word   : {
				AVP_word nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = USHRT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = (AVP_word)A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = ULONG_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = _UI64_MAX;
					else 
						nValue = A2UI64(pStr, NULL, GetConvertBase(pStr));
				PROP_Set_Val( hProp, (AVP_size_t)&nValue, sizeof(nValue) );
			}
				break;
			case avpt_int   : {
				AVP_int nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = INT_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = INT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = A2I(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = UINT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
						else
							nValue = A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue = 0;
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
				else
					nValue = A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_group   : {
				AVP_group nValue = 0;
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
				else
					nValue = (AVP_group)A2UI(pStr, NULL);
				PROP_Set_Val( hProp, nValue, 0 );
			}
				break;
			case avpt_date   : {
				AVP_date nValue;
				SYSTEMTIME rcSysTime;
				ZeroMemory( &rcSysTime, sizeof(rcSysTime) );
				if ( !_wcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
				}
				else
					if ( !_wcsicmp(pMaxStr, pStr) ) {
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
				if ( !_wcsicmp(pMinStr, pStr) ) {
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_wcsicmp(pMaxStr, pStr) ) {
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
				if ( !_wcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_wcsicmp(pMaxStr, pStr) ) {
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
				AVP_str pMStr;
				int nSize = WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0, NULL, NULL );
				pMStr = DTUAllocator( sizeof(wchar_t) * nSize);
				if ( pMStr ) {
					WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pMStr, nSize, NULL, NULL );
					PROP_Set_Val( hProp, (AVP_size_t)pMStr, 0 );
					DTULiberator( pMStr );
				}
			}
				break;
			case avpt_wstr   : {
				PROP_Set_Val( hProp, (AVP_size_t)pStr, 0 );
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
DTUT_PROC void DTUT_PARAM DTUT_SetPropArrayValueW( HPROP hProp, int nPos, const wchar_t *pStr, const wchar_t *pFormat ) {
		// Заменить Value узла
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "DTUT Library is not initialised" );
		return;
	}
	else {
		const wchar_t *pMinStr = L"Min";
		const wchar_t *pMaxStr = L"Max";

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
				AVP_char pChar[2] = { 0, 0 };
				WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, 1, pChar, sizeof(pChar), NULL, NULL );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, &pChar[0], sizeof(AVP_char) );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, &pChar[0], sizeof(AVP_char) );
			}
				break;
			case avpt_wchar  : {
				wchar_t nValue;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else
						nValue = pStr[0];
					if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
						PROP_Arr_Set_Items( hProp, nPos, &nValue, sizeof(nValue) );
					else
						PROP_Arr_Insert( hProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
			}
				break;
			case avpt_short  : {
				AVP_short nValue = 0;
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = SHRT_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = SHRT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = LONG_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = LONG_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = _I64_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = CHAR_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = CHAR_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = USHRT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = ULONG_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = INT_MIN;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = INT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) )
					nValue  = 0;
				else
					if ( !_wcsicmp(pMaxStr, pStr) )
						nValue  = UINT_MAX;
					else 
						//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
						if ( VALID_STR(pFormat) )
							swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				if ( !_wcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
				}
				else
					if ( !_wcsicmp(pMaxStr, pStr) ) {
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
				if ( !_wcsicmp(pMinStr, pStr) ) {
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_wcsicmp(pMaxStr, pStr) ) {
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
				if ( !_wcsicmp(pMinStr, pStr) ) {
					rcSysTime.wYear  = 1970;
					rcSysTime.wMonth = 12;
					rcSysTime.wDay	 = 31;
					rcSysTime.wHour  = 0;
					rcSysTime.wMinute = 0;
					rcSysTime.wSecond	 = 0;
					rcSysTime.wMilliseconds = 1;
				}
				else
					if ( !_wcsicmp(pMaxStr, pStr) ) {
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
				AVP_str pMStr;
				int nSize = WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, NULL, 0, NULL, NULL );
				pMStr = DTUAllocator( sizeof(wchar_t) * nSize);
				WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, pStr, -1, pMStr, nSize, NULL, NULL );
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, (void *)pMStr, 0 );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pMStr, 0 );
				DTULiberator( pMStr );
			}
				break;
			case avpt_wstr   : {
				if ( nPos >= 0 &&  nPos < (int)PROP_Arr_Count(hProp) )
					PROP_Arr_Set_Items( hProp, nPos, (void *)pStr, 0 );
				else
					PROP_Arr_Insert( hProp, PROP_ARR_LAST, (void *)pStr, 0 );
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
static wchar_t *AnalizeDate( AVP_date *nValue ) {
	AVP_date nEmptyValue;
	SYSTEMTIME rcSysTime;
	time_t rcTime = 0L;
	struct tm* ptmTemp;
	wchar_t *pResult;

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

	if ( GetVersion() & 0x80000000 ) {
		DWORD dwSize;
		char szBuffer[128] = {0};
		char pDateFormat[80];
		GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE , pDateFormat, _countof(pDateFormat) );
		if ( strlen(pDateFormat) ) {
			GetDateFormatA( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, szBuffer, _countof(szBuffer) );
		}
		else {
			ptmTemp = localtime(&rcTime);
			if (ptmTemp == 0 ||
				!strftime(szBuffer, _countof(szBuffer), "%#x", ptmTemp))
				szBuffer[0] = L'\0';
		}
		dwSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, szBuffer, -1, NULL, 0 ) + 1;
		pResult = DTUAllocator(sizeof(wchar_t) * dwSize);
		if ( pResult ) {
			*pResult = 0;
			MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, szBuffer, -1, pResult, dwSize );
		}
	}
	else {
		wchar_t szBuffer[128] = {0};
		wchar_t pDateFormat[80];
		GetLocaleInfoW( LOCALE_USER_DEFAULT, LOCALE_SLONGDATE , pDateFormat, _countof(pDateFormat) );
		if ( wcslen(pDateFormat) ) {
			GetDateFormatW( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, szBuffer, _countof(szBuffer) );
		}
		else {
			ptmTemp = localtime(&rcTime);
			if (ptmTemp == 0 ||
				!wcsftime(szBuffer, _countof(szBuffer), L"%#x", ptmTemp))
				szBuffer[0] = L'\0';
		}
		pResult = DTUAllocator( sizeof(wchar_t) * (wcslen(szBuffer) + 1) );
		if ( pResult )
			wcscpy( pResult, szBuffer );
	}

	return pResult;
}


// ---
static wchar_t *AnalizeTime( AVP_time *nValue ) {
	AVP_time nEmptyValue;
	SYSTEMTIME rcSysTime;
	time_t rcTime = 0L;
	struct tm* ptmTemp;
	wchar_t *pResult;

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


	if ( GetVersion() & 0x80000000 ) {
		DWORD dwSize;
		char szBuffer[128] = {0};
		char pDateFormat[80];
		GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, pDateFormat, _countof(pDateFormat) );
		if ( strlen(pDateFormat) ) {
			GetDateFormatA( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, szBuffer, _countof(szBuffer) );
		}
		else {
			ptmTemp = localtime(&rcTime);
			if (ptmTemp == 0 ||
				!strftime(szBuffer, _countof(szBuffer), "%X", ptmTemp))
				szBuffer[0] = L'\0';
		}
		dwSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, szBuffer, -1, NULL, 0 ) + 1;
		pResult = DTUAllocator(sizeof(wchar_t) * dwSize);
		if ( pResult ) {
			*pResult = 0;
			MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, szBuffer, -1, pResult, dwSize );
		}
	}
	else {
		wchar_t szBuffer[128] = {0};
		wchar_t pDateFormat[80];
		GetLocaleInfoW( LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, pDateFormat, _countof(pDateFormat) );
		if ( wcslen(pDateFormat) ) {
			GetTimeFormatW( LOCALE_USER_DEFAULT, 0, &rcSysTime, pDateFormat, szBuffer, _countof(szBuffer) );
		}
		else {
			ptmTemp = localtime(&rcTime);
			if (ptmTemp == 0 ||
				!wcsftime(szBuffer, _countof(szBuffer), L"%X", ptmTemp))
				szBuffer[0] = L'\0';
		}

		pResult = DTUAllocator( sizeof(wchar_t) * (wcslen(szBuffer) + 1) );
		if ( pResult )
			wcscpy( pResult, szBuffer );
	}
	return pResult;
}


// ---
static wchar_t *AnalizeDateTime( AVP_datetime *nValue ) {
	wchar_t *pDateValue = AnalizeDate( (AVP_date *)nValue );
	wchar_t *pTimeValue = AnalizeTime( (AVP_time *)&(((SYSTEMTIME *)nValue)->wHour) );

	wchar_t *pResult = DTUAllocator( sizeof(wchar_t) * (wcslen(pDateValue) + wcslen(pTimeValue) + 5) );
	if ( pResult ) {
		wcscpy( pResult, pDateValue );
		wcscat( pResult, L"  " );
		wcscat( pResult, pTimeValue );
	}
	DTULiberator(pDateValue);
	DTULiberator(pTimeValue);

	return pResult;
}


// ---
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetPropValueAsStringW( HPROP hProp, const wchar_t *pFormat ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return 0;
	}
	else {
		wchar_t *pResult = 0;

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
				int nSize;
				AVP_char pChar[2] = { 0, 0 };
				PROP_Get_Val( hProp, pChar, 2 );
				nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pChar, -1, NULL, 0 ) + 1;
				pResult = DTUAllocator(sizeof(wchar_t) * nSize);
				if ( pResult ) {
					*pResult = 0;
					MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pChar, -1, pResult, nSize );
				}
			}
				break;
			case avpt_wchar  : {
				wchar_t nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator(sizeof(wchar_t) * 2);
				if ( pResult ) {
					*(pResult) = nValue;
					*(pResult + 1) = 0;
				}
			}
				break;
			case avpt_short  : {
				AVP_short nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%d", nValue );
			}
				break;
			case avpt_long   : {
				AVP_long nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%d", nValue );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * 35 );
				if ( pResult )
					_i64tow( nValue, pResult, 10 );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_word   : {
				AVP_word nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * 35 );
				if ( pResult )
					_i64tow( nValue, pResult, 16 );
			}
				break;
			case avpt_int   : {
				AVP_int nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%d", nValue );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_group  : {
				AVP_group nValue;
				PROP_Get_Val( hProp, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult )
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
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
				int nSize = PROP_Get_Val( hProp, NULL, 0 ) + sizeof(wchar_t);
				if ( nSize ) {
					AVP_char *pWChar = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pWChar = 0;
					PROP_Get_Val( hProp, pWChar, nSize );
					nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
					pResult = DTUAllocator( sizeof(wchar_t) * nSize );
					if ( pResult ) {
						*pResult = 0;
						MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
					}
					DTULiberator( pWChar );
				}
			}
				break;
			case avpt_wstr   : {
				int nSize = PROP_Get_Val( hProp, NULL, 0 ) + sizeof(wchar_t);
				if ( nSize ) {
					pResult = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pResult = 0;
					PROP_Get_Val( hProp, pResult, nSize );
				}
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
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetPropArrayValueAsStringW( HPROP hProp, int nPos, const wchar_t *pFormat ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "DTUT Library is not initialised" );
		return 0;
	}
	else {
		wchar_t *pResult = 0;

		AVP_dword nID = PROP_Get_Id( hProp );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);
		switch ( nType ) {
			case avpt_char   : {
				int nSize;
				AVP_char pWChar[2] = { 0, 0 };
				PROP_Arr_Get_Items( hProp, nPos, pWChar, 2 );
				nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
				pResult = DTUAllocator( sizeof(wchar_t) * nSize );
				if ( pResult ) {
					*pResult = 0;
					MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
				}
			}
			  break;
			case avpt_wchar  : {
				wchar_t nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(AVP_byte) * 2 );
				if ( pResult ) {
					*(pResult) = nValue;
					*(pResult + 1) = 0;
				}
			}
				break;
			case avpt_short  : {
				AVP_short nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%d", nValue );
			}
				break;
			case avpt_long   : {
				AVP_long nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%d", nValue );
			}
				break;
			case avpt_longlong   : {
				AVP_longlong nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * 35 );
				if ( pResult ) 
					_i64tow( nValue, pResult, 10 );
			}
				break;
			case avpt_byte   : {
				AVP_byte nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_word   : {
				AVP_word nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_dword  : {
				AVP_dword nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_qword  : {
				AVP_qword nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * 35 );
				if ( pResult ) 
					_i64tow( nValue, pResult, 16 );
			}
				break;
			case avpt_int   : {
				AVP_int nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%d", nValue );
			}
				break;
			case avpt_uint  : {
				AVP_uint nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_bool   : {
				AVP_bool nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
			}
				break;
			case avpt_group  : {
				AVP_group nValue;
				PROP_Arr_Get_Items( hProp, nPos, &nValue, sizeof(nValue) );
				pResult = DTUAllocator( sizeof(wchar_t) * CONVERT_BUFF_LEN );
				if ( pResult ) 
					swprintf( pResult, VALID_STR(pFormat) ? pFormat : L"%u", nValue );
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
				int nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + sizeof(wchar_t);
				if ( nSize ) {
					AVP_char *pWChar = DTUAllocator( sizeof(AVP_byte) * nSize );
					*pWChar = 0;
					PROP_Arr_Get_Items( hProp, nPos, pWChar, nSize );
					nSize = MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, NULL, 0 ) + 1;
					pResult = DTUAllocator( sizeof(wchar_t) * nSize );
					if ( pResult ) {
						*pResult = 0;
						MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pWChar, -1, pResult, nSize );
					}
					DTULiberator( pWChar );
				}
			}
				break;
			case avpt_wstr   : {
				int nSize = PROP_Arr_Get_Items( hProp, nPos, NULL, 0 ) + sizeof(wchar_t);
				pResult = DTUAllocator( sizeof(AVP_byte) *nSize );
				*pResult = 0;
				PROP_Arr_Get_Items( hProp, nPos, pResult, nSize );
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
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetDataValueAsComboStringW( HDATA hData ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
		return 0;
	}
	else {
		wchar_t *pResult = 0;

		HPROP hProp = DATA_Find_Prop( hData, NULL, PP_PID_COMBOCONTENTS );
		if ( hProp ) {
			wchar_t *pDataText = DTUT_GetPropValueAsStringW( DATA_Find_Prop(hData, NULL, 0), 0 );
			AVP_dword nCount = PROP_Arr_Count( hProp );
			AVP_dword i;
			for ( i=0; i<nCount; i++ ) {
				wchar_t *pArrItemText = DTUT_GetPropArrayValueAsStringW( hProp, i, 0 );
				if ( !wcscmp(pDataText, pArrItemText) ) {
					pResult = DTUAllocator( sizeof(wchar_t) * (wcslen(pDataText) + 1) );
					if ( pResult ) 
					  wcscpy( pResult, pDataText );
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
DTUT_PROC AVP_bool DTUT_PARAM DTUT_SetDataNumericValueAsComboStringW( HDATA hData, const wchar_t *pStr ) {
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
					wchar_t *pItemText = DTUT_GetPropArrayValueAsStringW( hComboProp, i, 0 );
					bFound = !wcscmp( pStr, pItemText );
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
DTUT_PROC int DTUT_PARAM	DTUT_CompareValueInRangeHW( HDATA hData, const wchar_t *pStr, 
												  HPROP hMinProp, HPROP hMaxProp, const wchar_t *pFormat ) {
	int nResult = 0;

	if ( hMinProp || hMaxProp )	{
		AVP_dword nID   = DATA_Get_Id( hData, 0 );
		AVP_dword nType = GET_AVP_PID_TYPE(nID);

		switch ( nType ) {
			case avpt_short  : {
				AVP_short nValue = 0;
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%d", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
				//swscanf( pStr, VALID_STR(pFormat) ? pFormat : "%u", &nValue );
				if ( VALID_STR(pFormat) )
					swscanf( pStr, pFormat, &nValue );
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
DTUT_PROC int DTUT_PARAM	DTUT_CompareValueInRangeIW( HDATA hData, const wchar_t *pStr, 
												  AVP_dword nMinID, AVP_dword nMaxID, const wchar_t *pFormat ) {

	AVP_dword nID   = DATA_Get_Id( hData, 0 );
	AVP_dword nType = GET_AVP_PID_TYPE(nID);
	AVP_dword nApp  = GET_AVP_PID_APP(nID);

	AVP_dword nMinPid = MAKE_AVP_PID( nMinID, nApp, nType, 0);  
	AVP_dword nMaxPid = MAKE_AVP_PID( nMaxID, nApp, nType, 0);  

	HPROP hMinProp = DATA_Find_Prop( hData, NULL, nMinPid ); 
	HPROP hMaxProp = DATA_Find_Prop( hData, NULL, nMaxPid ); 

	return DTUT_CompareValueInRangeHW( hData, pStr, hMinProp, hMaxProp, pFormat );
}


// ---
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetPropValueAsStringCCW( HPROP hProp, const wchar_t *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	wchar_t *pResult = DTUT_GetDataNumericValueAsComboStringW( hData );

	if ( !pResult )
		pResult = DTUT_GetPropValueAsStringW( hProp, pFormat );

	return pResult;
}

// ---
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetDataNumericValueAsComboStringW( HDATA hData ) {
	wchar_t *pResult = 0;
	
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
				
				pResult = DTUT_GetPropArrayValueAsStringW( hComboProp, nValue, 0 );
			}
		}
	}
	
	return pResult;
}


// ---
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetDataArrayNumericValueAsComboStringW( HDATA hData, int nPos ) {
	wchar_t *pResult = 0;
	
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
				
				pResult = DTUT_GetPropArrayValueAsStringW( hComboProp, nValue, 0 );
			}
		}
	}
	
	return pResult;
}


// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_SetDataArrayNumericValueAsComboStringW( HDATA hData, int nPos, const wchar_t *pStr ) {
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
					wchar_t *pItemText = DTUT_GetPropArrayValueAsStringW( hComboProp, i, 0 );
					bFound = !wcscmp( pStr, pItemText );
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
DTUT_PROC wchar_t *DTUT_PARAM DTUT_GetPropArrayValueAsStringCCW( HPROP hProp, int nPos, const wchar_t *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	wchar_t *pResult = DTUT_GetDataArrayNumericValueAsComboStringW( hData, nPos );

	if ( !pResult )
		pResult = DTUT_GetPropArrayValueAsStringW( hProp, nPos, pFormat );

	return pResult;
}


// ---
DTUT_PROC void DTUT_PARAM DTUT_SetPropValueCCW( HPROP hProp, const wchar_t *pStr, const wchar_t *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	if ( !DTUT_SetDataNumericValueAsComboStringW(hData, pStr) )
		DTUT_SetPropValueW( hProp, pStr, pFormat );
}


// ---
DTUT_PROC void DTUT_SetPropArrayValueCCW( HPROP hProp, int nPos, const wchar_t *pStr, const wchar_t *pFormat ) {
	HDATA hData = PROP_Get_Dad( hProp );

	if ( !DTUT_SetDataArrayNumericValueAsComboStringW(hData, nPos, pStr) )
		DTUT_SetPropArrayValueW( hProp, nPos, pStr, pFormat );
}

