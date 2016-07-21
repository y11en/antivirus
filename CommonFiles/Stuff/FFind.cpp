////////////////////////////////////////////////////////////////////
//
//  FFind.cpp
//  CAvpFileFind class implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <Stuff\FFind.h>
#include <Stuff\PathStr.h>

#if !defined(_countof)
	#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

// ---
CAvpFileFind::CAvpFileFind() {
	m_pFoundInfo = NULL;
	m_pNextInfo = NULL;
	m_hContext = NULL;
	m_chDirSeparator = L'\\';
	m_bThisSemantic = FALSE;
	*m_strRoot = 0;
}

// ---
CAvpFileFind::~CAvpFileFind() {
	Close();
}

// ---
void CAvpFileFind::Close() {
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
void CAvpFileFind::CloseContext() {
	::FindClose(m_hContext);
}

// ---
LPWIN32_FIND_DATA CAvpFileFind::GetContext() const { 
	return m_bThisSemantic 
		     ? (LPWIN32_FIND_DATA)m_pNextInfo 
				 : (LPWIN32_FIND_DATA)m_pFoundInfo;
}

// ---
BOOL CAvpFileFind::FindFile(LPCTSTR pstrName /* = NULL */,	DWORD dwUnused /* = 0 */) {
	Close();
	m_pNextInfo = new WIN32_FIND_DATA;
	m_bGotLast = FALSE;

	if (pstrName == NULL)
		pstrName = _T("*.*");
/*
	if ( (_tcsclen(pstrName) + 1) > _countof(((WIN32_FIND_DATA*) m_pNextInfo)->cFileName) )
		// Ёто ошибка, конечно. ¬ этом случае мы все равно ничего не найдем. Ќо, наверное, нужно выполн€ть
		// поиск с переходом в каталог
		return FALSE;

	_tcscpy(((WIN32_FIND_DATA*) m_pNextInfo)->cFileName, pstrName);
*/
	m_hContext = ::FindFirstFile(pstrName, (WIN32_FIND_DATA*) m_pNextInfo);

	if (m_hContext == INVALID_HANDLE_VALUE)	{
		DWORD dwTemp = ::GetLastError();
		Close();
		::SetLastError(dwTemp);
		return FALSE;
	}

#if 0 // Dont use this technique - \\?\ problem
	LPTSTR pstrRoot = m_strRoot;
	LPCTSTR pstr = _tfullpath(pstrRoot, pstrName, _countof(m_strRoot));

	// passed name isn't a valid path but was found by the API
	if (pstr == NULL)	{
		Close();
		::SetLastError(ERROR_INVALID_NAME);
		return FALSE;
	}
	else {
		// find the last forward or backward whack
		LPTSTR pstrBack  = _tcsrchr(pstrRoot, L'\\');
		LPTSTR pstrFront = _tcsrchr(pstrRoot, L'/');

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
BOOL CAvpFileFind::MatchesMask(DWORD dwMask) const {
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL)
		return (!!(pContext->dwFileAttributes & dwMask));
	else
		return FALSE;
}

// ---
BOOL CAvpFileFind::GetLastAccessTime(FILETIME* pTimeStamp) const {
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL && pTimeStamp != NULL)	{
		*pTimeStamp = pContext->ftLastAccessTime;
		return TRUE;
	}
	else
		return FALSE;
}

// ---
BOOL CAvpFileFind::GetLastWriteTime(FILETIME* pTimeStamp) const {
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL && pTimeStamp != NULL)	{
		*pTimeStamp = pContext->ftLastWriteTime;
		return TRUE;
	}
	else
		return FALSE;
}

// ---
BOOL CAvpFileFind::GetCreationTime(FILETIME* pTimeStamp) const {
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL && pTimeStamp != NULL)	{
		*pTimeStamp = pContext->ftCreationTime;
		return TRUE;
	}
	else
		return FALSE;
}

// ---
BOOL CAvpFileFind::IsDots() const {
	// return TRUE if the file name is "." or ".." and
	// the file is a directory

	BOOL bResult = FALSE;
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL && IsDirectory()) {
		TCHAR *pFileName = pContext->cFileName;
		int n0 = _tcsnextc( _tcsninc(pFileName, 0) );
		int n1 = _tcsnextc( _tcsninc(pFileName, 1) );
		int n2 = _tcsnextc( _tcsninc(pFileName, 2) );
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
BOOL CAvpFileFind::FindNextFile() {
	if (m_hContext == NULL)
		return FALSE;
	if (m_pFoundInfo == NULL)
		m_pFoundInfo = new WIN32_FIND_DATA;

	void* pTemp = m_pFoundInfo;
	m_pFoundInfo = m_pNextInfo;
	m_pNextInfo = pTemp;

	return ::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo);
}

// ---
TCHAR *CAvpFileFind::GetFileURL() const {
	CPathStr strResult( _T("file://") );
	strResult += CAPointer<TCHAR>(GetFilePath());
	return strResult.Relinquish();
}

// ---
TCHAR *CAvpFileFind::GetRoot() const {
	return CPathStr(m_strRoot).Relinquish();
}


// ---
TCHAR *CAvpFileFind::GetFilePath() const {
	CPathStr strResult( m_strRoot );
	strResult ^= CAPointer<TCHAR>(GetFileName());
	return strResult.Relinquish();
}

// ---
TCHAR *CAvpFileFind::GetFileTitle() const {
	CPathStr strFullName( (CAPointer<TCHAR>(GetFileName())) );
	CPathStr strResult(_MAX_PATH);

	_tsplitpath(strFullName, NULL, NULL, strResult, NULL);
	
	return strResult.Relinquish();
}

// ---
TCHAR *CAvpFileFind::GetFileName() const {
	CPathStr ret;

	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL)
		ret = pContext->cFileName;

	return ret.Relinquish();
}

// ---
DWORD CAvpFileFind::GetLength() const {
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL)
		return pContext->nFileSizeLow;
	else
		return 0;
}

// ---
#if defined(_X86_) || defined(_ALPHA_)
__int64 CAvpFileFind::GetLength64() const {
	LPWIN32_FIND_DATA pContext = GetContext();
	if (pContext != NULL) {
		__int64 size = pContext->nFileSizeHigh;
		size <<= 32;
		return size + pContext->nFileSizeLow;
	}
	return 0;
}
#endif

