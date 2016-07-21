////////////////////////////////////////////////////////////////////
//
//  RegStorage.h
//  Interface for the CRegStorage class.
//  AVP generic purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#if !defined(__REGSTORAGE_H__)
#define __REGSTORAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// ---
class CRegStorage {
	HKEY    m_hRootKey;
	TCHAR  *m_pKeyName;
public:
	CRegStorage( HKEY hRootKey, LPCTSTR pKeyName );
	CRegStorage( LPCTSTR pAppName, LPCTSTR pKeyName = NULL );
	CRegStorage( DWORD   dwComponentID, LPCTSTR pKeyName = NULL );
	CRegStorage( DWORD   dwComponentID, DWORD   dwTaskID, LPCTSTR pKeyName = NULL );
	virtual ~CRegStorage();

	BOOL PutValue( LPCTSTR pValueName, LPCTSTR pValue );
	BOOL PutValueW( LPCTSTR pValueName, LPCWSTR pValue );
	BOOL PutValue( LPCTSTR pValueName, DWORD dwValue );
	BOOL PutValue( LPCTSTR pValueName, const BYTE  *pucValue, DWORD dwSize );

	BOOL GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD dwSize );
	BOOL GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize );
	BOOL GetValue( LPCTSTR pValueName, DWORD &dwValue );
	BOOL GetValue( LPCTSTR pValueName, BYTE  *pucValue, DWORD &dwSize );

	BOOL RemoveValue( LPCTSTR pValueName );
};

#endif // !defined(__REGSTORAGE_H__)
