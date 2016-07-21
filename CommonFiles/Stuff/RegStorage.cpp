////////////////////////////////////////////////////////////////////
//
//  RegStorage.cpp
//  Implementation of the CRegStorage class.
//  Interface for the CRegStorage class.
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <Stuff\RegStorage.h>
#include <Stuff\PathStr.h>
#include <Stuff\PathStrA.h>
#include <Stuff\PathStrW.h>
#include <AVPRegID.h>

static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
class CStorageRegKey {
	HKEY m_hKey;
public :
	CStorageRegKey( HKEY hOpenedKey, LPCTSTR pKeyName, BOOL bStore );
	~CStorageRegKey();

	BOOL IsOpened() { return m_hKey != NULL; }

	BOOL SetValue( LPCTSTR pValueName, DWORD dwValue );
	BOOL SetValue( LPCTSTR pValueName, LPCTSTR pValue );
	BOOL SetValueW( LPCTSTR pValueName, LPCWSTR pValue );
	BOOL SetValue( LPCTSTR pValueName, const BYTE  *ucValue, DWORD dwSize );

	BOOL GetValue( LPCTSTR pValueName, DWORD &dwValue );
	BOOL GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD dwSize );
	BOOL GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize );
	BOOL GetValue( LPCTSTR pValueName, BYTE  *ucValue, DWORD &dwSize );

	BOOL RemoveValue( LPCTSTR pValueName );
};


// ---
CStorageRegKey::CStorageRegKey( HKEY hOpenedKey, LPCTSTR pKeyName, BOOL bStore ) {
	m_hKey = NULL;

	if ( bStore )
		::RegCreateKeyEx( hOpenedKey, pKeyName, 0, NULL, 0, KEY_WRITE, NULL, &m_hKey, NULL );
	else
		::RegOpenKeyEx( hOpenedKey, pKeyName, 0, KEY_READ, &m_hKey );
}

// ---
CStorageRegKey::~CStorageRegKey() {
	if ( m_hKey )
		::RegCloseKey( m_hKey );
}

// ---
BOOL CStorageRegKey::SetValue( LPCTSTR pValueName, DWORD dwValue ) {
	if ( m_hKey ) 
		return ::RegSetValueEx(m_hKey, pValueName, 0, REG_DWORD, (BYTE *)&dwValue, sizeof(dwValue)) == ERROR_SUCCESS;

	return FALSE;
}


// ---
BOOL CStorageRegKey::SetValue( LPCTSTR pValueName, LPCTSTR pValue ) {
	if ( m_hKey ) 
		return ::RegSetValueEx(m_hKey, pValueName, 0, REG_SZ, (BYTE *)pValue, (DWORD)(_tcsclen(pValue) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS;
	
	return FALSE;
}

// ---
BOOL CStorageRegKey::SetValueW( LPCTSTR pValueName, LPCWSTR pValue ) {
#if defined(_UNICODE)
	return SetValue( pValueName, pValue );
#endif
	if ( m_hKey ) {
		if ( g_bUnicodePlatform )
			return ::RegSetValueExW(m_hKey, CPathStrW(pValueName), 0, REG_SZ, (BYTE *)pValue, (DWORD)(wcslen(pValue) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS;
		else {
			CPathStrA pRegValue( pValue );
			CPathStrA pRegValueName( pValueName );
			return ::RegSetValueExA(m_hKey, pRegValueName, 0, REG_SZ, (BYTE *)(LPCSTR)pRegValue, (DWORD)(strlen(pRegValue) + 1) * sizeof(char)) == ERROR_SUCCESS;
		}
	}
	
	return FALSE;
}

// ---
BOOL CStorageRegKey::SetValue( LPCTSTR pValueName, const BYTE  *pucValue, DWORD dwSize ) {
	if ( m_hKey ) 
		return ::RegSetValueEx(m_hKey, pValueName, 0, REG_BINARY, pucValue, dwSize) == ERROR_SUCCESS;

	return FALSE;
}

// ---
BOOL CStorageRegKey::GetValue( LPCTSTR pValueName, DWORD &dwValue ) {
	if ( m_hKey ) {
		DWORD dwSize = sizeof(dwValue);
		return ::RegQueryValueEx(m_hKey, pValueName, 0, NULL, (BYTE *)&dwValue, &dwSize) == ERROR_SUCCESS;
	}
	
	return FALSE;
}


// ---
BOOL CStorageRegKey::GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD dwSize ) {
	if ( m_hKey ) {
		return ::RegQueryValueEx(m_hKey, pValueName, 0, NULL, (BYTE *)pValue, &dwSize) == ERROR_SUCCESS;
	}
	
	return FALSE;
}


// ---
BOOL CStorageRegKey::GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize ) {
#if defined(_UNICODE)
	return GetValue( pValueName, pValue, dwSize );
#endif
	if ( m_hKey ) {
		if ( g_bUnicodePlatform )
			return ::RegQueryValueExW(m_hKey, CPathStrW(pValueName), 0, NULL, (BYTE *)pValue, &dwSize) == ERROR_SUCCESS;
		else {
			CPathStrA pRegValue( (int)dwSize );
			CPathStrA pRegValueName( pValueName );
			if ( ::RegQueryValueExA(m_hKey, pRegValueName, 0, NULL, (BYTE *)(LPSTR)pRegValue, &dwSize) == ERROR_SUCCESS ) {
				mbstowcs( pValue, LPCSTR(pRegValue), dwSize );
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


// ---
BOOL CStorageRegKey::GetValue( LPCTSTR pValueName, BYTE *pucValue, DWORD &dwSize ) {
	if ( m_hKey ) {
		return ::RegQueryValueEx(m_hKey, pValueName, 0, NULL, pucValue, &dwSize) == ERROR_SUCCESS;
	}
	
	return FALSE;
}


// ---
BOOL CStorageRegKey::RemoveValue( LPCTSTR pValueName ) {
	if ( m_hKey ) {
		return ::RegDeleteValue(m_hKey, pValueName) == ERROR_SUCCESS;
	}
	
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ---
CRegStorage::CRegStorage( HKEY hRootKey, LPCTSTR pKeyName ) :
	m_hRootKey( hRootKey )
{
	CPathStr rcKeyPath( pKeyName );
	m_pKeyName = rcKeyPath.Relinquish();
}

// ---
CRegStorage::CRegStorage( LPCTSTR pAppName, LPCTSTR pKeyName ) :
	m_hRootKey( HKEY_CURRENT_USER )
{
	CPathStr rcKeyPath( AVP_REGKEY_ROOT );

	if ( pAppName && *pAppName )
		rcKeyPath ^= pAppName;

	if ( pKeyName && *pKeyName )
		rcKeyPath ^= pKeyName;

	m_pKeyName = rcKeyPath.Relinquish();
}

// ---
CRegStorage::CRegStorage( DWORD dwComponentID, LPCTSTR pKeyName ) :
	m_hRootKey( HKEY_LOCAL_MACHINE )
{
	CPathStr rcKeyPath( AVP_REGKEY_COMPONENTS_FULL );

	TCHAR pSubKeyName[_MAX_PATH];
	wsprintf( pSubKeyName, _T("%x"), dwComponentID );
	
	rcKeyPath ^= pSubKeyName;

	if ( pKeyName && *pKeyName )
		rcKeyPath ^= pKeyName;

	m_pKeyName = rcKeyPath.Relinquish();
}

// ---
CRegStorage::CRegStorage( DWORD dwComponentID, DWORD dwTaskID, LPCTSTR pKeyName ) :
	m_hRootKey( HKEY_LOCAL_MACHINE )
{
	CPathStr rcKeyPath( AVP_REGKEY_COMPONENTS_FULL );

	TCHAR pSubKeyName[_MAX_PATH];
	wsprintf( pSubKeyName, _T("%x\\Tasks\\%x"), dwComponentID, dwTaskID );

	rcKeyPath ^= pSubKeyName;

	if ( pKeyName && *pKeyName )
		rcKeyPath ^= pKeyName;

	m_pKeyName = rcKeyPath.Relinquish();
	m_hRootKey = HKEY_LOCAL_MACHINE;
}

// ---
CRegStorage::~CRegStorage() {
	delete [] m_pKeyName;
}



// ---
BOOL CRegStorage::PutValue( LPCTSTR pValueName, LPCTSTR pValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, TRUE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValue(pValueName, pValue);
	}
	return FALSE;
}


// ---
BOOL CRegStorage::PutValueW( LPCTSTR pValueName, LPCWSTR pValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, TRUE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValueW(pValueName, pValue);
	}
	return FALSE;
}


// ---
BOOL CRegStorage::PutValue( LPCTSTR pValueName, DWORD dwValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, TRUE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValue(pValueName, dwValue);
	}
	return FALSE;
}


// ---
BOOL CRegStorage::PutValue( LPCTSTR pValueName, const BYTE  *pucValue, DWORD dwSize ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, TRUE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValue(pValueName, pucValue, dwSize);
	}
	return FALSE;
}


// ---
BOOL CRegStorage::GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD dwSize ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, FALSE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValue( pValueName, pValue, dwSize );
	}
	return FALSE;
}


// ---
BOOL CRegStorage::GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, FALSE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValueW( pValueName, pValue, dwSize );
	}
	return FALSE;
}


// ---
BOOL CRegStorage::GetValue( LPCTSTR pValueName, DWORD &dwValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, FALSE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValue(pValueName, dwValue);
	}
	return FALSE;
}


// ---
BOOL CRegStorage::GetValue( LPCTSTR pValueName, BYTE  *pucValue, DWORD &dwSize ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, FALSE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValue(pValueName, pucValue, dwSize);
	}
	return FALSE;
}


// ---
BOOL CRegStorage::RemoveValue( LPCTSTR pValueName ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, TRUE );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.RemoveValue( pValueName );
	}
	return FALSE;
}
