////////////////////////////////////////////////////////////////////
//
//  CmpObjs.c
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

#if defined(_UNICODE) || defined(UNICODE)
#define DTU_TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define DTU_TCSCLEN(p) (p ? strlen(p) : 0)
#endif


#define SIZEOF_TCHAR sizeof(TCHAR)

#if !defined(_countof)
	#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

// Network resource name format :
// <NWR>\Network\domain\?\\server\share\*.*

// Network rule owner format 
// <NWR>\Network\domain\?\\server\share\*.*

#define NWNS_SERVERSEPARATOR_FULL	  _T("?\\\\")
#define NWNS_LEADINGNETNAMESTR      _T("\\\\")

#define NS_PREFIX                   _T("\\\\?\\")
#define NS_UNC_PREFIX               _T("\\\\?\\UNC")

//---
static void PathExtract( const TCHAR * pszFileName, TCHAR * pszDirName, size_t uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {

    TCHAR fDrive[_MAX_DRIVE];
    TCHAR fDir  [_MAX_DIR];
		TCHAR *pLast = 0;
		size_t dLen;

		*pszDirName = 0;

  // выделить диск и каталог
    _tsplitpath( pszFileName, fDrive, fDir, NULL, NULL);
    dLen = DTU_TCSCLEN( fDir );

    if ( dLen > 1 && 
			  (pLast = _tcsninc(fDir, dLen - 1)) && 
				(_tcsnextc(pLast) == L'\\') )
      *pLast = 0;

    if ( (size_t)(DTU_TCSCLEN(fDrive) + DTU_TCSCLEN(fDir)) < uicchBufferLen ) {
			size_t uichLen;
      _tcsncpy( pszDirName, fDrive, uicchBufferLen );
      uichLen = DTU_TCSCLEN(pszDirName);
      pszDirName = _tcsninc( pszDirName, _tcsclen(pszDirName) );
      _tcsncpy( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
  }
}

/*
// ---
static AVP_dword FindSubString( const char *pFirstStr, const char *pSecondStr ) {
	AVP_word nResult = avpo_Different;
	AVP_dword dwLevel = 1;

	char pDir[_MAX_PATH];
	_tcscpy( pDir, pSecondStr );
	while ( lstrcmp(pFirstStr, pDir) > 0 ) {  
		PathExtract( pDir, pDir, sizeof(pDir) );
		if ( !_tcsicmp( pFirstStr, pDir) ) 
			return dwLevel == 1 ? avpo_FirstDescendant : avpo_Descendant;
	}

	char *pToken;
	char *pTempStr = DTUAllocator( _MAX_PATH + 1 );
	_tcscpy( pTempStr, pFirstStr );
	_tcscat( pTempStr, "\\" );
	pToken = _tcsstr( pSecondStr, pTempStr );
	if ( pToken && pToken == (char *)pSecondStr ) {
		char pDir[_MAX_PATH];
		nResult = avpo_Descendant;
		PathExtract( pSecondStr, pDir, sizeof(pDir) );
		if ( !_tcsicmp( pFirstStr, pDir) ) 
			nResult = avpo_FirstDescendant;
	}
}
*/

// ---
static TCHAR *LookForFirstSlash( const TCHAR *pSourceStr ) {
	size_t nPos = _tcscspn( pSourceStr, _T("/\\") );
	if ( nPos != DTU_TCSCLEN(pSourceStr) )
		return _tcsninc( pSourceStr, nPos );
	return NULL;
/*
	TCHAR *pSlash = (TCHAR *)pSourceStr;
	while ( pSlash && *pSlash ) {
		if ( (_tcsnicmp(pSlash, _T("/"), 1) == 0) || (_tcsnicmp(pSlash, _T("\\"), 1) == 0) )
			return pSlash;
		pSlash = _tcsninc(pSlash, 1);
	}
	return NULL;
*/	
}


//
static BOOL GetSafeShortPathName( const TCHAR *pSourceName, TCHAR **pDestName ) {
	BOOL bResult = TRUE;
	TCHAR *pSlash;
	BOOL   bNetName = FALSE;
	TCHAR *pDName;
	TCHAR *pProcessName;

	pProcessName = (TCHAR *)DTUAllocator((DTU_TCSCLEN(pSourceName) + 1) * SIZEOF_TCHAR);
	_tcscpy( pProcessName, pSourceName );

	bNetName = !_tcsnicmp(pProcessName, _T("\\\\"), 2 );
	
	pDName = *pDestName = (TCHAR *)DTUAllocator((DTU_TCSCLEN(pProcessName) + 1) * SIZEOF_TCHAR);
	_tcscpy( pDName, pProcessName );

	pSlash = LookForFirstSlash( _tcsninc(pProcessName, (bNetName ? 2 : 3)) );
	while ( pSlash ) {
		size_t nSubLen;
		*pSlash = 0;
		_tcscpy( pDName, pProcessName );
		nSubLen = DTU_TCSCLEN( pProcessName );
		if ( !bNetName ) {
			if ( !GetShortPathName((LPCTSTR)pProcessName, (LPTSTR)pDName, (int)(DTU_TCSCLEN(pDName) + 1)) ) {
				bResult = FALSE;
				break;
			}
			nSubLen = DTU_TCSCLEN( pDName );
		}
		_tcscat( pDName, _T("\\") );
		_tcscat( pDName, pSlash + 1 );

		DTULiberator( pProcessName );
		pProcessName = (TCHAR *)DTUAllocator((DTU_TCSCLEN(pDName) + 1) * SIZEOF_TCHAR);
		_tcscpy( pProcessName, pDName );

		pSlash = LookForFirstSlash( _tcsninc(pProcessName, nSubLen) + 1 );
		bNetName = FALSE;
	}

	if ( !GetShortPathName((LPCTSTR)pProcessName, (LPTSTR)pDName, (int)(DTU_TCSCLEN(pDName) + 1)) ) {
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
static int ConvertToShortName( TCHAR *pName, int nSkipLen ) {
	// This is a short name
	size_t nLen = pName ? DTU_TCSCLEN(pName) : 0;
	if ( pName && *pName && nLen > 8 ) {
		TCHAR *pShortName = 0;
		TCHAR *pProcessName = 0;
		if ( nSkipLen ) {
			pProcessName = (TCHAR *)DTUAllocator( (DTU_TCSCLEN(pName) + DTU_TCSCLEN(NWNS_LEADINGNETNAMESTR) + 1) * SIZEOF_TCHAR );
			_tcscpy( pProcessName, NWNS_LEADINGNETNAMESTR );
			_tcscat( pProcessName, _tcsninc(pName, nSkipLen) );
		}
		else {
			pProcessName = (TCHAR *)DTUAllocator((DTU_TCSCLEN(pName) + 1) * SIZEOF_TCHAR);
			_tcscpy( pProcessName, pName );
		}
		if ( GetSafeShortPathName(pProcessName, &pShortName) ) {
			if ( nSkipLen ) 
				_tcscpy( _tcsninc(pName, nSkipLen), _tcsninc(pShortName, DTU_TCSCLEN(NWNS_LEADINGNETNAMESTR)) );
			else
				_tcscpy( pName, pShortName );
			nLen = DTU_TCSCLEN( pName );
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
DTUT_PROC AVP_word  DTUT_PARAM DTUT_CompareTwoAvpObjects( HDATA hFirstObject, HDATA hSecondObject ) {
	AVP_word nResult = avpo_Different;

	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
	}
	else {
		TCHAR *pFirstStr  = DTUT_GetPropValueAsString( DATA_Find_Prop((HDATA)hFirstObject, NULL, 0), NULL );
		TCHAR *pSecondStr = DTUT_GetPropValueAsString( DATA_Find_Prop((HDATA)hSecondObject, NULL, 0), NULL );

		if ( pFirstStr && pSecondStr ) {
			TCHAR *pSkip;
			TCHAR *pTilde;
			size_t nFirstLen = DTU_TCSCLEN( pFirstStr );
			size_t nSecondLen = DTU_TCSCLEN( pSecondStr );
			size_t nFirstSkipLen = 0;
			size_t nSecondSkipLen = 0;
			
			if ( nFirstLen > 1 && 
					(pSkip = _tcsninc(pFirstStr, nFirstLen - 1)) && 
					(_tcsnicmp(pSkip, _T("\\"), 1) == 0) )
				*pSkip = 0;

			if ( nSecondLen > 1 && 
					(pSkip = _tcsninc(pSecondStr, nSecondLen - 1)) && 
					(_tcsnicmp(pSkip, _T("\\"), 1) == 0) )
				*pSkip = 0;

			_tcsupr( pFirstStr );
			_tcsupr( pSecondStr );
			
			pSkip = _tcsstr( pFirstStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NWNS_SERVERSEPARATOR_FULL)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = _tcsstr( pSecondStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NWNS_SERVERSEPARATOR_FULL)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			pSkip = _tcsstr( pFirstStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_UNC_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = _tcsstr( pSecondStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_UNC_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			pSkip = _tcsstr( pFirstStr, NS_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = _tcsstr( pSecondStr, NS_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}

			if ( ((pTilde = _tcschr(pSecondStr, L'~')) && _istdigit((TCHAR)_tcsnextc(_tcsninc(pTilde, 1))/**(pTilde + SIZEOF_TCHAR)*/)) ||
				   ((pTilde = _tcschr(pFirstStr, L'~')) && _istdigit((TCHAR)_tcsnextc(_tcsninc(pTilde, 1))/**(pTilde + SIZEOF_TCHAR)*/)) ) {
				// This is a short name
				nFirstLen = ConvertToShortName( pFirstStr, (int)nFirstSkipLen );
				nSecondLen = ConvertToShortName( pSecondStr, (int)nSecondSkipLen );
			}
			
			if ( !_tcsicmp( pFirstStr, pSecondStr) ) 
				nResult = avpo_TheSame;
			else {
				TCHAR *pToken;
				TCHAR *pTempStr = (TCHAR *)DTUAllocator((max(nFirstLen,nSecondLen) + 2) * SIZEOF_TCHAR);
				
				if ( nFirstLen ) {
					_tcscpy( pTempStr, pFirstStr );
					_tcscat( pTempStr, _T("\\") );
					pToken = _tcsstr( pSecondStr, pTempStr );
					if ( pToken && pToken == pSecondStr ) {
						TCHAR pDir[_MAX_PATH];
						nResult = avpo_Descendant;
						PathExtract( pSecondStr, pDir, _countof(pDir) );
						if ( !_tcsicmp( pFirstStr, pDir) ) 
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
							if ( _tcsnextc(_tcsninc(pSecondStr, 1)) == L':'/**(pSecondStr + 1) == L':'*/ ) {
								nResult = avpo_Descendant;

								pToken = _tcschr( pSecondStr, L'\\' );
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
DTUT_PROC AVP_word   DTUT_PARAM  DTUT_CompareTwoAvpObjectsStr( const TCHAR *pszFirstObject, const TCHAR *pszSecondObject ) {
	AVP_word nResult = avpo_Different;

	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("DTUT Library is not initialised") );
	}
	else {
		TCHAR *pFirstStr  = _tcscpy( DTUAllocator((DTU_TCSCLEN(pszFirstObject) + 1) * SIZEOF_TCHAR), pszFirstObject );
		TCHAR *pSecondStr = _tcscpy( DTUAllocator((DTU_TCSCLEN(pszSecondObject) + 1) * SIZEOF_TCHAR), pszSecondObject );

		if ( pFirstStr && pSecondStr ) {
			TCHAR *pSkip;
			TCHAR *pTilde;
			size_t nFirstLen = DTU_TCSCLEN( pFirstStr );
			size_t nSecondLen = DTU_TCSCLEN( pSecondStr );
			size_t nFirstSkipLen = 0;
			size_t nSecondSkipLen = 0;
			
			if ( nFirstLen > 1 && 
					(pSkip = _tcsninc(pFirstStr, nFirstLen - 1)) && 
					(_tcsnextc(pSkip) == L'\\') )
					*pSkip = 0;
			
			if ( nSecondLen > 1 && 
					(pSkip = _tcsninc(pSecondStr, nSecondLen - 1)) && 
					(_tcsnextc(pSkip) == L'\\') )
				*pSkip = 0;
			
			_tcsupr( pFirstStr );
			_tcsupr( pSecondStr );

			pSkip = _tcsstr( pFirstStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				_tcscpy( pSkip, pSkip + DTU_TCSCLEN(NWNS_SERVERSEPARATOR_FULL) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
		
			pSkip = _tcsstr( pSecondStr, NWNS_SERVERSEPARATOR_FULL );
			if ( pSkip ) {
				_tcscpy( pSkip, pSkip + DTU_TCSCLEN(NWNS_SERVERSEPARATOR_FULL) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			pSkip = _tcsstr( pFirstStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_UNC_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = _tcsstr( pSecondStr, NS_UNC_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_UNC_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			pSkip = _tcsstr( pFirstStr, NS_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_PREFIX)) );
				nFirstSkipLen = pSkip	- pFirstStr;
			}
			
			pSkip = _tcsstr( pSecondStr, NS_PREFIX );
			if ( pSkip ) {
				_tcscpy( pSkip, _tcsninc(pSkip, DTU_TCSCLEN(NS_PREFIX)) );
				nSecondSkipLen = pSkip	- pSecondStr;
			}
			
			if ( ((pTilde = _tcschr(pSecondStr, L'~')) && _istdigit((TCHAR)_tcsnextc(_tcsninc(pTilde, 1))/**(pTilde + SIZEOF_TCHAR)*/)) ||
				   ((pTilde = _tcschr(pFirstStr, L'~')) && _istdigit((TCHAR)_tcsnextc(_tcsninc(pTilde, 1))/**(pTilde + SIZEOF_TCHAR)*/)) ) {
					// This is a short name
				nFirstLen = ConvertToShortName( pFirstStr, (int)nFirstSkipLen );
				nSecondLen = ConvertToShortName( pSecondStr, (int)nSecondSkipLen );
			}

			if ( !_tcsicmp( pFirstStr, pSecondStr) ) 
				nResult = avpo_TheSame;
			else {
				TCHAR *pToken;
				TCHAR *pTempStr = (TCHAR *)DTUAllocator((max(nFirstLen,nSecondLen) + 2) * SIZEOF_TCHAR);
				
				if ( nFirstLen ) {
					_tcscpy( pTempStr, pFirstStr );
					_tcscat( pTempStr, _T("\\") );
					pToken = _tcsstr( pSecondStr, pTempStr );
					if ( pToken && pToken == pSecondStr ) {
						TCHAR pDir[_MAX_PATH];
						nResult = avpo_Descendant;
						PathExtract( pSecondStr, pDir, _countof(pDir) );
						if ( !_tcsicmp( pFirstStr, pDir) ) 
							nResult = avpo_FirstDescendant;
						goto processed;
					}
				}

				if ( nSecondLen ) {
					_tcscpy( pTempStr, pSecondStr );
					_tcscat( pTempStr, _T("\\") );
					pToken = _tcsstr( pFirstStr, pTempStr );
					if ( pToken && pToken == pFirstStr ) {
						TCHAR pDir[_MAX_PATH];
						nResult = avpo_Ancestor;
						PathExtract( pFirstStr, pDir, _countof(pDir) );
						if ( !_tcsicmp( pSecondStr, pDir) ) 
							nResult = avpo_FirstAncestor;
						goto processed;
					}
				}

				if ( !nFirstLen /*NS_OBJECT_TYPE_MYCOMPUTER*/ ) {
					if ( !nSecondLen /*NS_OBJECT_TYPE_MYCOMPUTER*/ ) {
						nResult = avpo_TheSame;
					}
					else {
						if ( _tcsnextc(_tcsninc(pSecondStr, 1)) == L':'/**(pSecondStr + 1) == L':'*/ ) {
							nResult = avpo_Descendant;

							pToken = _tcschr( pSecondStr, L'\\' );
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

						pToken = _tcschr( pFirstStr, L'\\' );
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
