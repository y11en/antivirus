////////////////////////////////////////////////////////////////////
//
//  FFindW.h
//  CAvpFileFindW class definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __FFINDW_H__
#define __FFINDW_H__

#include <windows.h>

// ---
class CAvpFileFindW {
public:
	CAvpFileFindW();
	virtual ~CAvpFileFindW();

// Attributes
public:
	DWORD GetLength() const;
#if defined(_X86_) || defined(_ALPHA_)
	__int64 GetLength64() const;
#endif
	virtual WCHAR *GetFileName() const;
	virtual WCHAR *GetFilePath() const;
	virtual WCHAR *GetFileTitle() const;
	virtual WCHAR *GetFileURL() const;
	virtual WCHAR *GetRoot() const;

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
	LPWIN32_FIND_DATAW GetContext() const;

// Operations
	void Close();
	virtual BOOL FindFile(LPCWSTR pstrName = NULL, DWORD dwUnused = 0);
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
	WCHAR m_strRoot[_MAX_PATH];
	WCHAR m_chDirSeparator;     // not '\\' for Internet classes
};

// ---
inline BOOL CAvpFileFindW::IsReadOnly() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_READONLY);
}

// ---
inline BOOL CAvpFileFindW::IsDirectory() const { 
	return MatchesMask(FILE_ATTRIBUTE_DIRECTORY);
}

// ---
inline BOOL CAvpFileFindW::IsCompressed() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_COMPRESSED);
}

// ---
inline BOOL CAvpFileFindW::IsSystem() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_SYSTEM);
}

// ---
inline BOOL CAvpFileFindW::IsHidden() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_HIDDEN);
}

// ---
inline BOOL CAvpFileFindW::IsTemporary() const { 
	return MatchesMask(FILE_ATTRIBUTE_TEMPORARY);
}
 
// ---
inline BOOL CAvpFileFindW::IsNormal() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_NORMAL);
}

// ---
inline BOOL CAvpFileFindW::IsArchived() const	{ 
	return MatchesMask(FILE_ATTRIBUTE_ARCHIVE);
}

#endif //__FFIND_H__