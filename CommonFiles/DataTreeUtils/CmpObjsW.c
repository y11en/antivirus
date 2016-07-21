////////////////////////////////////////////////////////////////////
//
//  CmpObjsW.c
//  Compare objects function module
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "datatreeutils/dtutils.h"
#include "avpnstid.h"
#include "AVPPort.h"

extern void* (* DTUAllocator)(AVP_size_t);
extern void  (* DTULiberator)(void*);


#if !defined(_countof)
	#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

// Network resource name format :
// <NWR>\Network\domain\?\\server\share\*.*

// Network rule owner format 
// <NWR>\Network\domain\?\\server\share\*.*

#define NWNS_SERVERSEPARATOR_FULL	  L"?\\\\"
#define NWNS_LEADINGNETNAMESTR      L"\\\\"

#define NS_PREFIX                   L"\\\\?\\"
#define NS_UNC_PREFIX               L"\\\\?\\UNC"

#if !defined(_UNICODE)
#define _wcsnextc(_cpc) ((unsigned int) *(_cpc))
#define _wcsninc(_pc, _sz) (((_pc)+(_sz)))
#define _wcsinc(_pc)    ((_pc)+1)
_CRTIMP wchar_t*  __cdecl wcscat(wchar_t *, const wchar_t *);
#endif

//---
static void PathExtract( const wchar_t * pszFileName, wchar_t * pszDirName, size_t uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {

    wchar_t fDrive[_MAX_DRIVE];
    wchar_t fDir  [_MAX_DIR];
		wchar_t *pLast = 0;
		size_t  dLen;

		*pszDirName = 0;

  // выделить диск и каталог
    _wsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
    dLen = wcslen( fDir );

    if ( dLen > 1 && 
			  (pLast = _wcsninc(fDir, dLen - 1)) && 
				(_wcsnextc(pLast) == L'\\') )
      *pLast = 0;

    if ( (size_t)(wcslen(fDrive) + wcslen(fDir)) < uicchBufferLen ) {
			size_t uichLen;
      wcsncpy( pszDirName, fDrive, uicchBufferLen );
      uichLen = wcslen(pszDirName);
      pszDirName = _wcsninc( pszDirName, uichLen );
      wcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
  }
}

/*
// ---
static AVP_dword FindSubString( const char *pFirstStr, const char *pSecondStr ) {
	AVP_word nResult = avpo_Different;
	AVP_dword dwLevel = 1;

	char pDir[_MAX_PATH];
	wcscpy( pDir, pSecondStr );
	while ( lstrcmp(pFirstStr, pDir) > 0 ) {  
		PathExtract( pDir, pDir, sizeof(pDir) );
		if ( !_wcsicmp( pFirstStr, pDir) ) 
			return dwLevel == 1 ? avpo_FirstDescendant : avpo_Descendant;
	}

	char *pToken;
	char *pTempStr = DTUAllocator( _MAX_PATH + 1 );
	wcscpy( pTempStr, pFirstStr );
	wcscat( pTempStr, "\\" );
	pToken = wcsstr( pSecondStr, pTempStr );
	if ( pToken && pToken == (char *)pSecondStr ) {
		char pDir[_MAX_PATH];
		nResult = avpo_Descendant;
		PathExtract( pSecondStr, pDir, sizeof(pDir) );
		if ( !_wcsicmp( pFirstStr, pDir) ) 
			nResult = avpo_FirstDescendant;
	}
}
*/

// ---
static wchar_t *LookForFirstSlash( const wchar_t *pSourceStr ) {
	size_t nPos = wcscspn( pSourceStr, L"/\\" );
	if ( nPos != wcslen(pSourceStr) )
		return _wcsninc( (wchar_t *)pSourceStr, nPos );
	return NULL;
/*
	wchar_t *pSlash = (wchar_t *)pSourceStr;
	while ( pSlash && *pSlash ) {
		if ( (_wcsnicmp(pSlash, _T("/"), 1) == 0) || (_wcsnicmp(pSlash, _T("\\"), 1) == 0) )
			return pSlash;
		pSlash = _wcsninc(pSlash, 1);
	}
	return NULL;
*/	
}


#if !defined(_UNICODE)
// ---
static char *UnicodeToMbcs( const wchar_t *pSourceStr ) {
	DWORD dwSize = WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (WCHAR *)pSourceStr, -1, NULL, 0, NULL, NULL );
	char *pDestStr = (char *)DTUAllocator(dwSize);
	WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (WCHAR *)pSourceStr, -1, pDestStr, dwSize, NULL, NULL );
	return pDestStr;
}

// ---
static void MbcsToUnicode( char *pSourceStr, wchar_t *pDestStr, DWORD dwDestStrSize ) {
	MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pSourceStr, -1, pDestStr, dwDestStrSize );
}
#endif

// ---
DWORD GetShortPathNameImpW( const wchar_t *lpszLongPath, wchar_t *lpszShortPath, DWORD cchBuffer ) {
#if defined(_UNICODE)
	return GetShortPathNameW( lpszLongPath, lpszShortPath, cchBuffer );
#else
	if ( !(GetVersion() & 0x80000000) ) {
		return GetShortPathNameW( lpszLongPath, lpszShortPath, cchBuffer );
	}
	else {
		char *pLongAnsiName = UnicodeToMbcs( lpszLongPath );
		char *pShortAnsiName = (char *)DTUAllocator( cchBuffer );
		GetShortPathNameA( pLongAnsiName, pShortAnsiName, cchBuffer );
		MbcsToUnicode( pShortAnsiName, lpszShortPath, cchBuffer );
		cchBuffer = (DWORD)wcslen( lpszShortPath );
		DTULiberator( pLongAnsiName );
		DTULiberator( pShortAnsiName );
		return cchBuffer;
	}
#endif	
}

// ---
static BOOL GetSafeShortPathName( const wchar_t *pSourceName, wchar_t **pDestName ) {
	BOOL bResult = TRUE;
	wchar_t *pSlash;
	BOOL   bNetName = FALSE;
	wchar_t *pDName;
	wchar_t *pProcessName;

	pProcessName = (wchar_t *)DTUAllocator((wcslen(pSourceName) + 1) * sizeof(wchar_t));
	wcscpy( pProcessName, pSourceName );

	bNetName = !_wcsnicmp(pProcessName, L"\\\\", 2 );
	
	pDName = *pDestName = (wchar_t *)DTUAllocator((wcslen(pProcessName) + 1) * sizeof(wchar_t));
	wcscpy( pDName, pProcessName );

	pSlash = LookForFirstSlash( _wcsninc(pProcessName, (bNetName ? 2 : 3)) );
	while ( pSlash ) {
		size_t nSubLen;
		*pSlash = 0;
		wcscpy( pDName, pProcessName );
		nSubLen = wcslen( pProcessName );
		if ( !bNetName ) {
			if ( !GetShortPathNameImpW(pProcessName, pDName, (DWORD)(wcslen(pDName) + 1)) ) {
				bResult = FALSE;
				break;
			}
			nSubLen = wcslen( pDName );
		}
		wcscat( pDName, L"\\" );
		wcscat( pDName, pSlash + 1 );

		DTULiberator( pProcessName );
		pProcessName = (wchar_t *)DTUAllocator((wcslen(pDName) + 1) * sizeof(wchar_t));
		wcscpy( pProcessName, pDName );

		pSlash = LookForFirstSlash( _wcsninc(pProcessName, nSubLen) + 1 );
		bNetName = FALSE;
	}

	if ( !GetShortPathNameImpW(pProcessName, pDName, (DWORD)(wcslen(pDName) + 1)) ) {
		bResult = FALSE;
	}
		
	DTULiberator( pProcessName );
	if ( !bResult ) {
		DTULiberator( pDName );
		*pDestName = 0;
	}
	
	return bResult;
}

// ---
static int ConvertToShortName( wchar_t *pName, int nSkipLen ) {
	// This is a short name
	size_t nLen = pName ? wcslen(pName) : 0;
	if ( pName && *pName && nLen > 8 ) {
		wchar_t *pShortName = 0;
		wchar_t *pProcessName = 0;
		if ( nSkipLen ) {
			pProcessName = (wchar_t *)DTUAllocator( (wcslen(pName) + wcslen(NWNS_LEADINGNETNAMESTR) + 1) * sizeof(wchar_t) );
			wcscpy( pProcessName, NWNS_LEADINGNETNAMESTR );
			wcscat( pProcessName, _wcsninc(pName, nSkipLen) );
		}
		else {
			pProcessName = (wchar_t *)DTUAllocator((wcslen(pName) + 1) * sizeof(wchar_t));
			wcscpy( pProcessName, pName );
		}
		if ( GetSafeShortPathName(pProcessName, &pShortName) ) {
			if ( nSkipLen ) 
				wcscpy( _wcsninc(pName, nSkipLen), _wcsninc(pShortName, wcslen(NWNS_LEADINGNETNAMESTR)) );
			else
				wcscpy( pName, pShortName );
			nLen = wcslen( pName );
		}
		if ( pProcessName )
			DTULiberator( pProcessName );
		if ( pShortName )
			DTULiberator( pShortName );
	}

	return (int)nLen;
}

// Compare two AVP objects 
// hFirstObject  - HDATA handle of the first object
// hSecondObject - HDATA handle of the second object
// Return values -	avpo_Different				- objects are fully different
//									avpo_TheSame					- objects are equivalent
//									avpo_Descendant				- second object is descendant of first
//									avpo_FirstDescendant	- second object is the first descendant of first
// ---
DTUT_PROC AVP_word  DTUT_PARAM DTUT_CompareTwoAvpObjectsW( HDATA hFirstObject, HDATA hSecondObject ) {
	AVP_word nResult = avpo_Different;

	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
	}
	else {
		wchar_t *pFirstStr  = DTUT_GetPropValueAsStringW( DATA_Find_Prop((HDATA)hFirstObject, NULL, 0), NULL );
		wchar_t *pSecondStr = DTUT_GetPropValueAsStringW( DATA_Find_Prop((HDATA)hSecondObject, NULL, 0), NULL );

		if ( pFirstStr && pSecondStr ) {
			wchar_t *pSkip;
			wchar_t *pTilde;
			size_t nFirstLen = wcslen( pFirstStr );
			size_t nSecondLen = wcslen( pSecondStr );
			size_t nFirstSkipLen = 0;
			size_t nSecondSkipLen = 0;
			
			if ( nFirstLen > 1 && 
					(pSkip = _wcsninc(pFirstStr, nFirstLen - 1)) && 
					(_wcsnicmp(pSkip, L"\\", 1) == 0) )
				*pSkip = 0;

			if ( nSecondLen > 1 && 
					(pSkip = _wcsninc(pSecondStr, nSecondLen - 1)) && 
					(_wcsnicmp(pSkip, L"\\", 1) == 0) )
				*pSkip = 0;

			_wcsupr( pFirstStr );
			_wcsupr( pSecondStr );
			
			pSkip = wcsstr( pFirstStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NWNS_SERVERSEPARATOR_FULL)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = wcsstr( pSecondStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NWNS_SERVERSEPARATOR_FULL)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			pSkip = wcsstr( pFirstStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_UNC_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = wcsstr( pSecondStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_UNC_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			pSkip = wcsstr( pFirstStr, NS_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = wcsstr( pSecondStr, NS_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}

			if ( ((pTilde = wcschr(pSecondStr, L'~')) && iswdigit((wchar_t)_wcsnextc(_wcsninc(pTilde, 1))/**(pTilde + sizeof(wchar_t))*/)) ||
				   ((pTilde = wcschr(pFirstStr, L'~')) && iswdigit((wchar_t)_wcsnextc(_wcsninc(pTilde, 1))/**(pTilde + sizeof(wchar_t))*/)) ) {
				// This is a short name
				nFirstLen = ConvertToShortName( pFirstStr, (int)nFirstSkipLen );
				nSecondLen = ConvertToShortName( pSecondStr, (int)nSecondSkipLen );
				_wcsupr( pFirstStr );
				_wcsupr( pSecondStr );
			}
			
			if ( !_wcsicmp( pFirstStr, pSecondStr) ) 
				nResult = avpo_TheSame;
			else {
				wchar_t *pToken;
				wchar_t *pTempStr = (wchar_t *)DTUAllocator((max(nFirstLen,nSecondLen) + 2) * sizeof(wchar_t));
				
				if ( nFirstLen ) {
					wcscpy( pTempStr, pFirstStr );
					wcscat( pTempStr, L"\\" );
					pToken = wcsstr( pSecondStr, pTempStr );
					if ( pToken && pToken == pSecondStr ) {
						wchar_t pDir[_MAX_PATH];
						nResult = avpo_Descendant;
						PathExtract( pSecondStr, pDir, _countof(pDir) );
						if ( !_wcsicmp( pFirstStr, pDir) ) 
							nResult = avpo_FirstDescendant;
						goto processed;
					}
				}
				{
					AVP_dword dwFirstType = 0;
					AVP_dword dwSecondType = 0;
					HPROP hTypeProp = DATA_Find_Prop( hFirstObject, NULL, NS_PID_OBJECT_TYPE );
					if ( hTypeProp ) 
						dwFirstType = DTUT_GetPropNumericValueAsDWord( hTypeProp );

					hTypeProp = DATA_Find_Prop( hSecondObject, NULL, NS_PID_OBJECT_TYPE );
					if ( hTypeProp ) 
						dwSecondType = DTUT_GetPropNumericValueAsDWord( hTypeProp );

					if ( dwFirstType == NS_OBJECT_TYPE_MYCOMPUTER ) {
						if ( dwSecondType == NS_OBJECT_TYPE_MYCOMPUTER ) {
							nResult = avpo_TheSame;
						}
						else {
							if ( _wcsnextc(_wcsninc(pSecondStr, 1)) == L':'/**(pSecondStr + 1) == L':'*/ ) {
								nResult = avpo_Descendant;

								pToken = wcschr( pSecondStr, L'\\' );
								if ( !pToken )
									nResult = avpo_FirstDescendant;
							}
						}
					}
				}
processed :
				DTULiberator( pTempStr );
			}
		}

		DTULiberator( pFirstStr );
		DTULiberator( pSecondStr );
	}
	return nResult;
}

// Compare two AVP objects by geven names 
// pszFirstObject  - name of the first object	 (empty string means "MyComputer" object)
// pszSecondObject - name of the second object  (empty string means "MyComputer" object)
// Return values   -	avpo_Different				- objects are fully different
//										avpo_TheSame					- objects are equivalent
//										avpo_Descendant				- second object is descendant of first
//										avpo_FirstDescendant	- second object is the first descendant of first
// ---
DTUT_PROC AVP_word   DTUT_PARAM  DTUT_CompareTwoAvpObjectsStrW( const wchar_t *pszFirstObject, const wchar_t *pszSecondObject ) {
	AVP_word nResult = avpo_Different;

	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
	}
	else {
		wchar_t *pFirstStr  = wcscpy( DTUAllocator((wcslen(pszFirstObject) + 1) * sizeof(wchar_t)), pszFirstObject );
		wchar_t *pSecondStr = wcscpy( DTUAllocator((wcslen(pszSecondObject) + 1) * sizeof(wchar_t)), pszSecondObject );

		if ( pFirstStr && pSecondStr ) {
			wchar_t *pSkip;
			wchar_t *pTilde;
			size_t nFirstLen = wcslen( pFirstStr );
			size_t nSecondLen = wcslen( pSecondStr );
			size_t nFirstSkipLen = 0;
			size_t nSecondSkipLen = 0;
			
			if ( nFirstLen > 1 && 
					(pSkip = _wcsninc(pFirstStr, nFirstLen - 1)) && 
					(_wcsnextc(pSkip) == L'\\') )
					*pSkip = 0;
			
			if ( nSecondLen > 1 && 
					(pSkip = _wcsninc(pSecondStr, nSecondLen - 1)) && 
					(_wcsnextc(pSkip) == L'\\') )
				*pSkip = 0;
			
			_wcsupr( pFirstStr );
			_wcsupr( pSecondStr );

			pSkip = wcsstr( pFirstStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				wcscpy( pSkip, pSkip + wcslen(NWNS_SERVERSEPARATOR_FULL) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
		
			pSkip = wcsstr( pSecondStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				wcscpy( pSkip, pSkip + wcslen(NWNS_SERVERSEPARATOR_FULL) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			pSkip = wcsstr( pFirstStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_UNC_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = wcsstr( pSecondStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_UNC_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			pSkip = wcsstr( pFirstStr, NS_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = wcsstr( pSecondStr, NS_PREFIX );
			if ( pSkip ) {
				wcscpy( pSkip, _wcsninc(pSkip, wcslen(NS_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			if ( ((pTilde = wcschr(pSecondStr, L'~')) && iswdigit((wchar_t)_wcsnextc(_wcsninc(pTilde, 1))/**(pTilde + sizeof(wchar_t))*/)) ||
				   ((pTilde = wcschr(pFirstStr, L'~')) && iswdigit((wchar_t)_wcsnextc(_wcsninc(pTilde, 1))/**(pTilde + sizeof(wchar_t))*/)) ) {
					// This is a short name
				nFirstLen = ConvertToShortName( pFirstStr, (int)nFirstSkipLen );
				nSecondLen = ConvertToShortName( pSecondStr, (int)nSecondSkipLen );
				_wcsupr( pFirstStr );
				_wcsupr( pSecondStr );
			}

			if ( !_wcsicmp( pFirstStr, pSecondStr) ) 
				nResult = avpo_TheSame;
			else {
				wchar_t *pToken;
				wchar_t *pTempStr = (wchar_t *)DTUAllocator((max(nFirstLen,nSecondLen) + 2) * sizeof(wchar_t));
				
				if ( nFirstLen ) {
					wcscpy( pTempStr, pFirstStr );
					wcscat( pTempStr, L"\\" );
					pToken = wcsstr( pSecondStr, pTempStr );
					if ( pToken && pToken == pSecondStr ) {
						wchar_t pDir[_MAX_PATH];
						nResult = avpo_Descendant;
						PathExtract( pSecondStr, pDir, _countof(pDir) );
						if ( !_wcsicmp( pFirstStr, pDir) ) 
							nResult = avpo_FirstDescendant;
						goto processed;
					}
				}

				if ( nSecondLen ) {
					wcscpy( pTempStr, pSecondStr );
					wcscat( pTempStr, L"\\" );
					pToken = wcsstr( pFirstStr, pTempStr );
					if ( pToken && pToken == pFirstStr ) {
						wchar_t pDir[_MAX_PATH];
						nResult = avpo_Ancestor;
						PathExtract( pFirstStr, pDir, _countof(pDir) );
						if ( !_wcsicmp( pSecondStr, pDir) ) 
							nResult = avpo_FirstAncestor;
						goto processed;
					}
				}

				if ( !nFirstLen /*NS_OBJECT_TYPE_MYCOMPUTER*/ ) {
					if ( !nSecondLen /*NS_OBJECT_TYPE_MYCOMPUTER*/ ) {
						nResult = avpo_TheSame;
					}
					else {
						if ( _wcsnextc(_wcsninc(pSecondStr, 1)) == L':'/**(pSecondStr + 1) == L':'*/ ) {
							nResult = avpo_Descendant;

							pToken = wcschr( pSecondStr, L'\\' );
							if ( !pToken )
								nResult = avpo_FirstDescendant;
						}
					}
					goto processed;
				}

				if ( !nSecondLen /*NS_OBJECT_TYPE_MYCOMPUTER*/ ) {
					if ( !nFirstLen /*NS_OBJECT_TYPE_MYCOMPUTER*/ ) {
						nResult = avpo_TheSame;
					}
					else {
						nResult = avpo_Ancestor;

						pToken = wcschr( pFirstStr, L'\\' );
						if ( !pToken )
							nResult = avpo_FirstAncestor;
					}
					goto processed;
				}
processed :
				DTULiberator( pTempStr );
			}
		}

		DTULiberator( pFirstStr );
		DTULiberator( pSecondStr );
	}
	return nResult;
}
