////////////////////////////////////////////////////////////////////
//
//  FFind.cpp
//  CAvpFileFindW class implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Stuff\FFindW.h>
#include <Stuff\PathStr.h>
#include <Stuff\PathStrW.h>

#if !defined(_countof)
	#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#if !defined(_UNICODE)

static BOOL g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
static char *UnicodeToMbcs( const wchar_t *pSourceStr ) {
	DWORD dwSize = ::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (wchar_t *)pSourceStr, -1, NULL, 0, NULL, NULL );
	char *pDestStr = new char[dwSize];
	::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, (wchar_t *)pSourceStr, -1, pDestStr, dwSize, NULL, NULL );
	return pDestStr;
}

// ---
static void MbcsToUnicode( char *pSourceStr, wchar_t *pDestStr, DWORD dwDestStrSize ) {
	::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pSourceStr, -1, pDestStr, dwDestStrSize );
}


// ---
static void WContext2AContext( LPWIN32_FIND_DATAW pWCont, LPWIN32_FIND_DATAA pACont ) {
	memcpy( pACont, pWCont, ((char *)&pWCont->cFileName - (char *)pWCont) );
	// MAC! memcpy( &pACont->dwFileType, &pWCont->dwFileType, ((char *)pWCont + sizeof(*pWCont) - &pWCont->dwFileType) );
	::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWCont->cFileName, -1, pACont->cFileName, _countof(pACont->cFileName), NULL, NULL );
	::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWCont->cAlternateFileName, -1, pACont->cAlternateFileName, _countof(pACont->cAlternateFileName), NULL, NULL );
}


// ---
static void AContext2WContext( LPWIN32_FIND_DATAA pACont, LPWIN32_FIND_DATAW pWCont ) {
	memcpy( pWCont, pACont, ((char *)&pACont->cFileName - (char *)pACont) );
	// MAC! memcpy( &pWCont->dwFileType, &pACont->dwFileType, ((char *)pACont + sizeof(*pACont) - &pACont->dwFileType) );
	::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pACont->cFileName, -1, pWCont->cFileName, _countof(pWCont->cFileName) );
	::MultiByteToWideChar( CP_ACP, 0/*WC_SEPCHARS*/, pACont->cAlternateFileName, -1, pWCont->cAlternateFileName, _countof(pWCont->cAlternateFileName) );
}
#endif




// ---
CAvpFileFindW::CAvpFileFindW() {
	m_pFoundInfo = NULL;
	m_pNextInfo = NULL;
	m_hContext = NULL;
	m_chDirSeparator = L'\\';
	m_bThisSemantic = FALSE;
	*m_strRoot = 0;
}

// ---
CAvpFileFindW::~CAvpFileFindW() {
	Close();
}

// ---
void CAvpFileFindW::Close() {
	if (m_pFoundInfo != NULL)	{
		delete m_pFoundInfo;
		m_pFoundInfo = NULL;
	}

	if (m_pNextInfo != NULL) {
		delete m_pNextInfo;
		m_pNextInfo = NULL;
	}

	if (m_hContext != NULL && m_hContext != INVALID_HANDLE_VALUE)	{
		CloseContext();
		m_hContext = NULL;
	}
}

// ---
void CAvpFileFindW::CloseContext() {
	::FindClose(m_hContext);
}

// ---
LPWIN32_FIND_DATAW CAvpFileFindW::GetContext() const { 
	return m_bThisSemantic 
		? (LPWIN32_FIND_DATAW)m_pNextInfo 
		: (LPWIN32_FIND_DATAW)m_pFoundInfo;
}

// ---
BOOL CAvpFileFindW::FindFile(LPCWSTR pstrName /* = NULL */,	DWORD dwUnused /* = 0 */) {
	Close();
	m_pNextInfo = new WIN32_FIND_DATAW;
	m_bGotLast = FALSE;

	if (pstrName == NULL)
		pstrName = L"*.*";
/*
	if ( (wcslen(pstrName) + 1) > _countof(((WIN32_FIND_DATAW*) m_pNextInfo)->cFileName) )
		// Ёто ошибка, конечно. ¬ этом случае мы все равно ничего не найдем. Ќо, наверное, нужно выполн€ть
		// поиск с переходом в каталог
		return FALSE;

	wcscpy(((WIN32_FIND_DATAW*) m_pNextInfo)->cFileName, pstrName);
*/
#if defined(_UNICODE)
	m_hContext = ::FindFirstFile(pstrName, (WIN32_FIND_DATAW*) m_pNextInfo);
#else
	if ( g_bUnicodePlatform )
		m_hContext = ::FindFirstFileW(pstrName, (WIN32_FIND_DATAW*) m_pNextInfo);
	else {
		WIN32_FIND_DATAA rcContext;
		//::WContext2AContext( (WIN32_FIND_DATAW*) m_pNextInfo, &rcContext );
		CAPointer<char> pConverted = ::UnicodeToMbcs( pstrName );
		m_hContext = ::FindFirstFileA( pConverted, &rcContext );
		::AContext2WContext( &rcContext, (WIN32_FIND_DATAW*) m_pNextInfo );
	}
#endif

	if (m_hContext == INVALID_HANDLE_VALUE)	{
		DWORD dwTemp = ::GetLastError();
		Close();
		::SetLastError(dwTemp);
		return FALSE;
	}

#if 0 // Dont use this technique - \\?\ problem
	LPWSTR pstrRoot = m_strRoot;
	LPCWSTR pstr = NULL;
#if defined(_UNICODE)
	pstr = _tfullpath(pstrRoot, pstrName, _countof(m_strRoot));
#else
	if ( g_bUnicodePlatform )
		pstr = _wfullpath(pstrRoot, pstrName, _countof(m_strRoot));
	else {
		CAPointer<char> pConverted = ::UnicodeToMbcs( pstrName );
		CHAR strRoot[_MAX_PATH];
		LPCSTR pAStr = _fullpath(strRoot, pConverted, _countof(strRoot));
		if ( pAStr ) {
			::MbcsToUnicode( strRoot, pstrRoot, _countof(m_strRoot) );
			pstr = pstrRoot;
		}
	}
#endif

	// passed name isn't a valid path but was found by the API
	if (pstr == NULL)	{
		Close();
		::SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
	else {
		// find the last forward or backward whack
		LPWSTR pstrBack  = wcsrchr(pstrRoot, L'\\');
		LPWSTR pstrFront = wcsrchr(pstrRoot, L'/');

		if (pstrFront != NULL || pstrBack != NULL) {
			if (pstrFront == NULL)
				pstrFront = pstrRoot;
			if (pstrBack == NULL)
				pstrBack = pstrRoot;

			// from the start to the last whack is the root

			if (pstrFront >= pstrBack)
				*pstrFront = L'\0';
			else
				*pstrBack = L'\0';
		}
	}
#endif
	return TRUE;
}

// ---
BOOL CAvpFileFindW::MatchesMask(DWORD dwMask) const {
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL)
		return (!!(pContext->dwFileAttributes & dwMask));
	else
		return FALSE;
}

// ---
BOOL CAvpFileFindW::GetLastAccessTime(FILETIME* pTimeStamp) const {
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL && pTimeStamp != NULL)	{
		*pTimeStamp = pContext->ftLastAccessTime;
		return TRUE;
	}
	else
		return FALSE;
}

// ---
BOOL CAvpFileFindW::GetLastWriteTime(FILETIME* pTimeStamp) const {
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL && pTimeStamp != NULL)	{
		*pTimeStamp = pContext->ftLastWriteTime;
		return TRUE;
	}
	else
		return FALSE;
}

// ---
BOOL CAvpFileFindW::GetCreationTime(FILETIME* pTimeStamp) const {
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL && pTimeStamp != NULL)	{
		*pTimeStamp = pContext->ftCreationTime;
		return TRUE;
	}
	else
		return FALSE;
}

// ---
BOOL CAvpFileFindW::IsDots() const {
	// return TRUE if the file name is "." or ".." and
	// the file is a directory

	BOOL bResult = FALSE;
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL && IsDirectory()) {
		WCHAR *pFileName = pContext->cFileName;
		int n0 = _wcsnextc( _wcsninc(pFileName, 0) );
		int n1 = _wcsnextc( _wcsninc(pFileName, 1) );
		int n2 = _wcsnextc( _wcsninc(pFileName, 2) );
		if ( n0 == L'.' ) {
			if ( n1 == L'\0' ||
				 ( n1 == L'.' && n2 == L'\0') ) {
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

// ---
BOOL CAvpFileFindW::FindNextFile() {
	if (m_hContext == NULL)
		return FALSE;
	if (m_pFoundInfo == NULL)
		m_pFoundInfo = new WIN32_FIND_DATAW;

	void* pTemp = m_pFoundInfo;
	m_pFoundInfo = m_pNextInfo;
	m_pNextInfo = pTemp;

#if defined(_UNICODE)
	return ::FindNextFile(m_hContext, (LPWIN32_FIND_DATAW) m_pNextInfo);
#else
	if ( g_bUnicodePlatform )
		return ::FindNextFileW(m_hContext, (LPWIN32_FIND_DATAW) m_pNextInfo);
	else {
		WIN32_FIND_DATAA rcContext;
		//::WContext2AContext( (WIN32_FIND_DATAW*) m_pNextInfo, &rcContext );
		BOOL bResult = ::FindNextFileA( m_hContext, &rcContext );
		::AContext2WContext( &rcContext, (WIN32_FIND_DATAW*) m_pNextInfo );
		return bResult;
	}
#endif
}

// ---
WCHAR *CAvpFileFindW::GetFileURL() const {
	CPathStrW strResult( L"file://" );
	strResult += CAPointer<WCHAR>(GetFilePath());
	return strResult.Relinquish();
}

// ---
WCHAR *CAvpFileFindW::GetRoot() const {
	return CPathStrW(m_strRoot).Relinquish();
}


// ---
WCHAR *CAvpFileFindW::GetFilePath() const {
	CPathStrW strResult( m_strRoot );
	strResult ^= CAPointer<WCHAR>(GetFileName());
	return strResult.Relinquish();
}

// ---
WCHAR *CAvpFileFindW::GetFileTitle() const {
	CAPointer<WCHAR> strFullName = GetFileName();
	CPathStrW strResult(_MAX_PATH);

#if defined(_UNICODE)
	_tsplitpath(strFullName, NULL, NULL, strResult, NULL);
#else
	if ( g_bUnicodePlatform )
		_wsplitpath(strFullName, NULL, NULL, strResult, NULL);
	else {
		CAPointer<char> pConverted =::UnicodeToMbcs( strFullName );
		CPathStr strAResult(_MAX_PATH);
		_splitpath(pConverted, NULL, NULL, strAResult, NULL);
		::MbcsToUnicode( strAResult, strResult, _MAX_PATH );
	}
#endif
	
	return strResult.Relinquish();
}

// ---
WCHAR *CAvpFileFindW::GetFileName() const {
	CPathStrW ret;

	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL)
		ret = pContext->cFileName;

	return ret.Relinquish();
}

// ---
DWORD CAvpFileFindW::GetLength() const {
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL)
		return pContext->nFileSizeLow;
	else
		return 0;
}

// ---
#if defined(_X86_) || defined(_ALPHA_)
__int64 CAvpFileFindW::GetLength64() const {
	LPWIN32_FIND_DATAW pContext = GetContext();
	if (pContext != NULL) {
		__int64 size = pContext->nFileSizeHigh;
		size <<= 32;
		return size + pContext->nFileSizeLow;
	}
	return 0;
}
#endif

