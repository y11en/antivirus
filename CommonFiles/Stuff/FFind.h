////////////////////////////////////////////////////////////////////
//
//  FFind.h
//  CAvpFileFind class definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __FFIND_H__
#define __FFIND_H__

#include <windows.h>

// ---
class CAvpFileFind {
public:
	CAvpFileFind();
	virtual ~CAvpFileFind();

// Attributes
public:
	DWORD GetLength() const;
#if defined(_X86_) || defined(_ALPHA_)
	__int64 GetLength64() const;
#endif
	virtual TCHAR *GetFileName() const;
	virtual TCHAR *GetFilePath() const;
	virtual TCHAR *GetFileTitle() const;
	virtual TCHAR *GetFileURL() const;
	virtual TCHAR *GetRoot() const;

	virtual BOOL GetLastWriteTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetLastAccessTime(FILETIME* pTimeStamp) const;
	virtual BOOL GetCreationTime(FILETIME* pTimeStamp) const;

	virtual BOOL MatchesMask(DWORD dwMask) const;

	virtual BOOL IsDots() const;
	// these aren't virtual because they all use MatchesMask(), which is
	BOOL IsReadOnly() const;
	BOOL IsDirectory() const;
	BOOL IsCompressed() const;
	BOOL IsSystem() const;
	BOOL IsHidden() const;
	BOOL IsTemporary() const;
	BOOL IsNormal() const;
	BOOL IsArchived() const;

	void SetThisSemantic( BOOL bSet = FALSE ) {	m_bThisSemantic = bSet; }
	LPWIN32_FIND_DATA GetContext() const;

// Operations
	void Close();
	virtual BOOL FindFile(LPCTSTR pstrName = NULL, DWORD dwUnused = 0);
	virtual BOOL FindNextFile();

protected:
	virtual void CloseContext();

// Implementation
protected:
	void* m_pFoundInfo;
	void* m_pNextInfo;
	HANDLE m_hContext;
	BOOL m_bGotLast;
	BOOL m_bThisSemantic;
	TCHAR m_strRoot[_MAX_PATH];
	TCHAR m_chDirSeparator;     // not '\\' for Internet classes
};

// ---
inline BOOL CAvpFileFind::IsReadOnly() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_READONLY);
}

// ---
inline BOOL CAvpFileFind::IsDirectory() const { 
	return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
}

// ---
inline BOOL CAvpFileFind::IsCompressed() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
}

// ---
inline BOOL CAvpFileFind::IsSystem() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
}

// ---
inline BOOL CAvpFileFind::IsHidden() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
}

// ---
inline BOOL CAvpFileFind::IsTemporary() const { 
	return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
}
 
// ---
inline BOOL CAvpFileFind::IsNormal() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_NORMAL);
}

// ---
inline BOOL CAvpFileFind::IsArchived() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
}

#endif //__FFIND_H__