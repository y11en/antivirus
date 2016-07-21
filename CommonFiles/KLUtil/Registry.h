#if !defined(__KLUTIL_REGISTRY_H)
#define __KLUTIL_REGISTRY_H

#if _MSC_VER > 1000
#	pragma once
#endif // _MSC_VER > 1000

#include "defs.h"
#include <string>

namespace KLUTIL
{
// ---
class KLUTIL_API CRegistry
{
	HKEY    m_hRootKey;
	TCHAR  *m_pKeyName;
public:
	CRegistry( HKEY hRootKey, LPCTSTR pKeyName );
	~CRegistry();

	bool IsKeyExist() const;
	
	bool PutValue( LPCTSTR pValueName, LPCTSTR pValue );
	bool PutValueW( LPCTSTR pValueName, LPCWSTR pValue );
	bool PutValue( LPCTSTR pValueName, DWORD dwValue );
	bool PutValue( LPCTSTR pValueName, const BYTE  *pucValue, DWORD dwSize );

	bool GetValue( LPCTSTR pValueName, LPTSTR pValue, DWORD dwSize ) const;
	bool GetValueW( LPCTSTR pValueName, LPWSTR pValue, DWORD dwSize ) const;
	bool GetValue( LPCTSTR pValueName, DWORD &dwValue ) const;
	bool GetValue( LPCTSTR pValueName, BYTE  *pucValue, DWORD &dwSize ) const;

    bool GetString (LPCTSTR pValueName, std::string& pValue) const;
	DWORD GetDWORD(LPCTSTR pValueName, DWORD dwDef = 0) const;

	bool RemoveValue( LPCTSTR pValueName );
};
} // namespace KLUTIL

#endif // !defined(__KLUTIL_REGISTRY_H)
