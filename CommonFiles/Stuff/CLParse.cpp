////////////////////////////////////////////////////////////////////
//
//  CLParse.cpp
//  CommadLine parsing utility class implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#include <string.h>
#include <tchar.h>
#include <Stuff\CLParse.h>
#include <Stuff\PathStr.h>
#include <StuffIO\IOUtil.h>

#if defined(_UNICODE) || defined(UNICODE)
#define TCSCLEN(p) (p ? _tcslen(p) : 0)
#else
#define TCSCLEN(p) (p ? strlen(p) : 0)
#endif


const TCHAR pszWhiteSpace[] = _T(" \t");
const TCHAR pszTerminator[] = _T("=/ \t");  // remove /- to dissallow separating there
const TCHAR pszTerminatorA[] = _T("\"");
const TCHAR pszTerminatorB[] = _T("%");

// ---
CCmdLineParser::CCmdLineParser( const TCHAR *cmdLine ) {
  m_pBuffer = new TCHAR[TCSCLEN(cmdLine) + 1];
  _tcscpy( m_pBuffer, cmdLine );
  Reset();
}

// ---
CCmdLineParser::~CCmdLineParser() {
  delete [] m_pBuffer;
}

// ---
void CCmdLineParser::Reset() {
  m_pToken = m_pTokenStart = m_pBuffer;
  m_iTokenLen = 0;
  m_eKind = Start;
}

// ---
CCmdLineParser::CKind CCmdLineParser::NextToken( BOOL removeCurrent ) {
  // Done parsing, no more tokens
  //
  if ( m_eKind == Done )
    return m_eKind;

  // Move m_pToken ptr to next token, by copying over current token, or by ptr
  // adjustment. m_pTokenStart stays right past previous token
  //
  if ( removeCurrent ) {
    _tcscpy( m_pTokenStart, _tcsninc(m_pToken, m_iTokenLen) );
    m_pToken = m_pTokenStart;
  }
  else {
    m_pToken = _tcsninc( m_pToken, m_iTokenLen );
    m_pTokenStart = m_pToken;
  }

  // Adjust token ptr to begining of token & determine kind
  //
  m_pToken = _tcsninc(m_pToken, _tcsspn(m_pToken, pszWhiteSpace) );  // skip leading pszWhiteSpace
	BOOL bA = FALSE;
  switch ( _tcsnextc(m_pToken) ) {
    case L'\0':
      m_eKind = Done;
      break;
    case L'=':
      m_eKind = Value;
      m_pToken = _tcsinc( m_pToken );
      break;
    case L'-':
    case L'/':
      m_eKind = Option;
      m_pToken = _tcsinc( m_pToken );
      break;
		case L'\"' :
			bA = TRUE;
    default:
      m_eKind = Name;
  }
  m_pToken = _tcsninc(m_pToken, _tcsspn(m_pToken, pszWhiteSpace) );  // skip any more pszWhiteSpace
	if ( bA )
		m_iTokenLen = (int)_tcscspn( _tcsninc(m_pToken, !!bA), pszTerminatorA ) + 2;
	else
		m_iTokenLen = (int)_tcscspn( m_pToken, pszTerminator );

  return m_eKind;
}


// ---
BOOL CCmdLineParser::GetExecutableName( LPTSTR pszExeName, DWORD dwExeNameLength ) {
	BOOL bResult = FALSE;

	if ( !pszExeName || !dwExeNameLength )
		return FALSE;

	*pszExeName = L'\0';

/*
	SHFILEINFO shInfoName;
	::ZeroMemory( &shInfoName, sizeof(shInfoName) );
	::SHGetFileInfo( m_pBuffer, FILE_ATTRIBUTE_NORMAL, 
								 &shInfoName, 
								 sizeof(shInfoName), 
								 SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME );
*/
/*
	CAPointer<TCHAR> pDrive = new TCHAR[_MAX_DRIVE];
	CAPointer<TCHAR> pDir   = new TCHAR[_MAX_DIR];
	CAPointer<TCHAR> pFile  = new TCHAR[_MAX_FNAME];

	_splitpath( m_pBuffer, pDrive, pDir, pFile, NULL);
*/
/*
	CAPointer<TCHAR> pFullName  = new TCHAR[_MAX_FNAME];
	::GetFullPathName( m_pBuffer, _MAX_PATH, pFullName, NULL );
*/
/*
	HANDLE          hFinder;
	WIN32_FIND_DATA w32fd;

	UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

	hFinder = ::FindFirstFile( m_pBuffer, &w32fd );
	if ( hFinder != INVALID_HANDLE_VALUE ) {
		::FindClose( hFinder );
	}

	::SetErrorMode( uiOldMode );
*/
/*
	const TCHAR pszWhiteSpace[] = _T(" \t\"");
	const TCHAR pszTerminatorA[] = _T("\"");
	TCHAR *pBegin = LPTSTR(m_pBuffer) + strspn( m_pBuffer, pszWhiteSpace );  
	CPathStr pExeName( pBegin );

	UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

	while ( *pExeName ) {

		HANDLE          hFinder;
		WIN32_FIND_DATA w32fd;

		hFinder = ::FindFirstFile( pExeName, &w32fd );
		if ( hFinder != INVALID_HANDLE_VALUE ) {
			::FindClose( hFinder );
			break;
		}

		TCHAR *pLast = LPTSTR(pExeName) + lstrlen(pExeName);
		while ( pLast != LPTSTR(pExeName) && *pLast != L'\\' && *pLast != L'/' )
			pLast--;

		*pLast = 0;
		while ( pLast != LPTSTR(pExeName) ) {
			pLast--;
			if ( *pLast == L' ' || *pLast == L'\t' )
				*pLast = 0;
			else
				break;
		}
	}

	::SetErrorMode( uiOldMode );

	lstrcpy( pszExeName, pExeName );
*/
/*
	CPathStr pExeName( m_pBuffer );
	TCHAR *pLast = LPTSTR(pExeName) + lstrlen(pExeName);

	UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

	while ( *pExeName ) {
		HANDLE          hFinder;
		WIN32_FIND_DATA w32fd;

		hFinder = ::FindFirstFile( pExeName, &w32fd );
		if ( hFinder != INVALID_HANDLE_VALUE ) {
			::FindClose( hFinder );
			break;
		}

		while ( pLast != LPTSTR(pExeName) && *pLast != L' ' )
			pLast--;

		*pLast = 0;
	}

	::SetErrorMode( uiOldMode );

	lstrcpy( pszExeName, pExeName );
*/


	//HANDLE          hFinder;
	//WIN32_FIND_DATA w32fd;

	UINT uiOldMode = ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );

	CPathStr pExeName( m_pBuffer );
	CPathStr pEnvName( _MAX_PATH );
	TCHAR *pBegin = _tcsninc( LPTSTR(pExeName), _tcsspn(pExeName, pszWhiteSpace) );  

	if ( _tcsnextc(pBegin) == L'\"' ) { 
		pBegin = _tcsinc( pBegin );
		TCHAR *pEnd = LPTSTR(pBegin) + _tcscspn( pBegin, pszTerminatorA );  
		if ( pEnd ) {
			*pEnd = 0;
			while ( pEnd && pEnd != LPTSTR(pBegin) ) {
				pEnd = _tcsdec( pBegin, pEnd );
				if ( _tcsnextc(pEnd) == L' ' || _tcsnextc(pEnd) == L'\t' )
					*pEnd = L'\0';
				else
					break;
			}
			if ( _tcsnextc(pBegin) == L'%' ) {
				pBegin = _tcsinc( pBegin );
				pEnd = _tcsninc( LPTSTR(pBegin), _tcscspn(pBegin, pszTerminatorB) );  
				if ( pEnd ) {
					*pEnd = L'\0';
					pEnvName = _tgetenv( pBegin );
					pEnvName ^= _tcsinc( pEnd );
					pBegin = pEnvName;
				}
			}
			CPathStr pOutExeName( _MAX_PATH );
			if ( ::IOSFindFile( pBegin, pOutExeName, _MAX_PATH) == IOS_ERR_OK &&
				   ::IOSGetIsDirectory( pOutExeName ) != IOS_ERR_OK ) {
				_tcsncpy( pszExeName, pOutExeName, dwExeNameLength );
				bResult = TRUE;
			}
		}
	}
	else {
		CPathStr pExeName;
		int nLength = 0;
		if ( _tcsnextc(pBegin) == L'%' ) {
			pBegin = _tcsinc( pBegin );
			TCHAR *pEnd = _tcsninc( LPTSTR(pBegin), _tcscspn(pBegin, pszTerminatorB) );  
			if ( pEnd ) {
				*pEnd = L'\0';
				pEnvName = _tgetenv( pBegin );
				pEnvName ^= pEnd + 1;
				pBegin = pEnvName;
			}
		}
		TCHAR *pEnd = pBegin;
		while ( pEnd && _tcsnextc(pEnd) != 0 ) {
			nLength += (int)_tcscspn( pEnd, pszWhiteSpace );
			pExeName = pBegin;
			pEnd = _tcsninc( LPTSTR(pExeName), nLength );  
			if ( pEnd ) {
				*pEnd = L'\0';
				CPathStr pOutExeName( _MAX_PATH );
				if ( ::IOSFindFile( pExeName, pOutExeName, _MAX_PATH) == IOS_ERR_OK &&
					::IOSGetIsDirectory( pOutExeName ) != IOS_ERR_OK ) {
					_tcsncpy( pszExeName, pOutExeName, dwExeNameLength );
					bResult = TRUE;
					break;
				}
				if ( TCSCLEN(pExeName) == TCSCLEN(pBegin) )
					break;
				pEnd = _tcsinc( pEnd );
				nLength++;
			}
		}
	}

	::SetErrorMode( uiOldMode );

	return bResult;
}



