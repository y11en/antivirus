#include "stdafx.h"
#include "Registry.h"
#include "UnicodeConv.h"

using namespace KLUTIL;

namespace KLUTIL
{
	static bool g_bUnicodePlatform = !(::GetVersion() & 0x80000000);

// ---
class CStorageRegKey {
	HKEY m_hKey;
public :
	CStorageRegKey( HKEY hOpenedKey, LPCTSTR pKeyName, bool bStore );
	~CStorageRegKey();

	bool IsOpened() { return m_hKey != NULL; }

	bool SetValue( LPCTSTR pValueName, DWORD dwValue );
	bool SetValue( LPCTSTR pValueName, LPCTSTR pValue );
	bool SetValueW( LPCTSTR pValueName, LPCWSTR pValue );
	bool SetValue( LPCTSTR pValueName, const BYTE  *ucValue, DWORD dwSize );

	bool GetValue( LPCTSTR pValueName, DWORD &dwValue );
	bool GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD& dwSize );
	bool GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize );
	bool GetValue( LPCTSTR pValueName, BYTE  *ucValue, DWORD &dwSize );

	bool RemoveValue( LPCTSTR pValueName );
};


// ---
CStorageRegKey::CStorageRegKey( HKEY hOpenedKey, LPCTSTR pKeyName, bool bStore ) {
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
bool CStorageRegKey::SetValue( LPCTSTR pValueName, DWORD dwValue ) {
	if ( m_hKey ) 
		return ::RegSetValueEx(m_hKey, pValueName, 0, REG_DWORD, (BYTE *)&dwValue, sizeof(dwValue)) == ERROR_SUCCESS;

	return false;
}


// ---
bool CStorageRegKey::SetValue( LPCTSTR pValueName, LPCTSTR pValue ) {
	if ( m_hKey ) 
		return ::RegSetValueEx(m_hKey, pValueName, 0, REG_SZ, (BYTE *)pValue, (_tcsclen(pValue) + 1) * sizeof(TCHAR)) == ERROR_SUCCESS;
	
	return false;
}

// ---
bool CStorageRegKey::SetValueW( LPCTSTR pValueName, LPCWSTR pValue ) {
#if defined(_UNICODE)
	return SetValue( pValueName, pValue );
#endif
	if ( m_hKey ) {
		if ( g_bUnicodePlatform )
			return ::RegSetValueExW(m_hKey, __LPCWSTR(pValueName), 0, REG_SZ, (BYTE *)pValue, (wcslen(pValue) + 1) * sizeof(wchar_t)) == ERROR_SUCCESS;
		else {
			return ::RegSetValueExA(m_hKey, __LPCSTR(pValueName), 0, REG_SZ, (BYTE *)__LPSTR(pValue), (wcslen(pValue) + 1) * sizeof(char)) == ERROR_SUCCESS;
		}
	}
	
	return false;
}

// ---
bool CStorageRegKey::SetValue( LPCTSTR pValueName, const BYTE  *pucValue, DWORD dwSize ) {
	if ( m_hKey ) 
		return ::RegSetValueEx(m_hKey, pValueName, 0, REG_BINARY, pucValue, dwSize) == ERROR_SUCCESS;

	return false;
}

// ---
bool CStorageRegKey::GetValue( LPCTSTR pValueName, DWORD &dwValue ) {
	if ( m_hKey ) {
		DWORD dwSize = sizeof(dwValue);
		return ::RegQueryValueEx(m_hKey, pValueName, 0, NULL, (BYTE *)&dwValue, &dwSize) == ERROR_SUCCESS;
	}
	
	return false;
}


// ---
bool CStorageRegKey::GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD& dwSize ) {
	if ( m_hKey ) {
		return ::RegQueryValueEx(m_hKey, pValueName, 0, NULL, (BYTE *)pValue, &dwSize) == ERROR_SUCCESS;
	}
	
	return false;
}


// ---
bool CStorageRegKey::GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize ) {
#if defined(_UNICODE)
	return GetValue( pValueName, pValue, dwSize );
#endif
	if ( m_hKey ) {
		if ( g_bUnicodePlatform )
			return ::RegQueryValueExW(m_hKey, __LPCWSTR(pValueName), 0, NULL, (BYTE *)pValue, &dwSize) == ERROR_SUCCESS;
		else {
			CStdStringA pRegValue;
			if ( ::RegQueryValueExA(m_hKey, __LPCSTR(pValueName), 0, NULL, (BYTE *)pRegValue.GetBuffer(dwSize), &dwSize) == ERROR_SUCCESS ) {
				mbstowcs( pValue, LPCSTR(pRegValue), dwSize );
				return true;
			}
		}
	}
	
	return false;
}


// ---
bool CStorageRegKey::GetValue( LPCTSTR pValueName, BYTE *pucValue, DWORD &dwSize ) {
	if ( m_hKey ) {
		return ::RegQueryValueEx(m_hKey, pValueName, 0, NULL, pucValue, &dwSize) == ERROR_SUCCESS;
	}
	
	return false;
}


// ---
bool CStorageRegKey::RemoveValue( LPCTSTR pValueName ) {
	if ( m_hKey ) {
		return ::RegDeleteValue(m_hKey, pValueName) == ERROR_SUCCESS;
	}
	
	return false;
}

} // namespace KLUTIL

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// ---
CRegistry::CRegistry( HKEY hRootKey, LPCTSTR pKeyName ) :
	m_hRootKey( hRootKey )
{
	m_pKeyName = new TCHAR[_tcslen(pKeyName) + 1];
	_tcscpy(m_pKeyName, pKeyName);
}

	
// ---
CRegistry::~CRegistry() {
	delete [] m_pKeyName;
}



// ---
bool CRegistry::PutValue( LPCTSTR pValueName, LPCTSTR pValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, true );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValue(pValueName, pValue);
	}
	return false;
}


// ---
bool CRegistry::PutValueW( LPCTSTR pValueName, LPCWSTR pValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, true );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValueW(pValueName, pValue);
	}
	return false;
}


// ---
bool CRegistry::PutValue( LPCTSTR pValueName, DWORD dwValue ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, true );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValue(pValueName, dwValue);
	}
	return false;
}


// ---
bool CRegistry::PutValue( LPCTSTR pValueName, const BYTE  *pucValue, DWORD dwSize ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, true );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.SetValue(pValueName, pucValue, dwSize);
	}
	return false;
}


bool CRegistry::IsKeyExist() const {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, false );
	return rcRKey.IsOpened();
}


// ---
bool CRegistry::GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD dwSize ) const {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, false );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValue( pValueName, pValue, dwSize );
	}
	return false;
}


// ---
bool CRegistry::GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize ) const {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, false );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValueW( pValueName, pValue, dwSize );
	}
	return false;
}


// ---
bool CRegistry::GetValue( LPCTSTR pValueName, DWORD &dwValue ) const {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, false );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValue(pValueName, dwValue);
	}
	return false;
}


// ---
bool CRegistry::GetValue( LPCTSTR pValueName, BYTE  *pucValue, DWORD &dwSize ) const {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, false );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.GetValue(pValueName, pucValue, dwSize);
	}
	return false;
}


// ---
bool CRegistry::GetString( LPCTSTR pValueName, std::string& pValue ) const {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, false );
	if ( rcRKey.IsOpened() ) {
		DWORD dwSize = 0; // размер строки включая '\0'
		if (!rcRKey.GetValue( pValueName, LPTSTR(NULL), dwSize ))
			return false;
		pValue.resize(dwSize);
		if (rcRKey.GetValue( pValueName, const_cast<char*>(pValue.c_str()), dwSize))
		{
			pValue.resize(dwSize - 1); // отрежем последний символ '\0'
			return true;
		}
	}
	return false;
}

DWORD CRegistry::GetDWORD(LPCTSTR pValueName, DWORD dwDef) const
{
	GetValue(pValueName, dwDef);
	return dwDef;
}

// ---
bool CRegistry::RemoveValue( LPCTSTR pValueName ) {
	CStorageRegKey rcRKey( m_hRootKey, m_pKeyName, true );
	if ( rcRKey.IsOpened() ) {
		return rcRKey.RemoveValue( pValueName );
	}
	return false;
}
