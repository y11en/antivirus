////////////////////////////////////////////////////////////////////
//
//  UNCConvert.cpp
//  UNC path to Local path and vice-versa conversion functions implementation
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <tchar.h>
#include <Stuff\PathStr.h>
#include <Stuff\UNCConvert.h>

#define REG_KEY_W95 _T("Software\\Microsoft\\Windows\\CurrentVersion\\NetWork\\LanMan")
#define REG_KEY_WNT _T("SYSTEM\\CurrentControlSet\\Services\\LanmanServer\\Shares")
#define PATH_VALUE_NT _T("PATH=")


// ---
static TCHAR *GetServerName() {
	DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	TCHAR pComputerName[ MAX_COMPUTERNAME_LENGTH + 1 ];
	::GetComputerName( pComputerName, &dwSize );

	CPathStr pServerName( _T("\\\\") );
	pServerName ^= pComputerName;

	return _tcsupr(pServerName.Relinquish());
}



// Convert UNC path to corresponding Local path
// pszUNCPath      - source UNC path 
// pszLocalPath	   - buffer for local path
// dwLocalPathSize - size of pszLocalPath
// Return value    - TRUE - conversion performed successfully
//       					   FALSE - any errors occurs
// ---
BOOL UNCPathToLocalPath( LPCTSTR pszUNCPath, LPTSTR pszLocalPath, DWORD dwLocalPathSize ) {
  BOOL  bResult = FALSE;
	
	CPathStr pServerName = ::GetServerName();
	if ( pServerName && *pServerName ) {
		HKEY  hOpenedKey;

		OSVERSIONINFO	rcOS;
		rcOS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx( &rcOS );

		CPathStr pUprUNCPath( pszUNCPath );
		if ( pUprUNCPath ) {
			_tcsupr( pUprUNCPath );

			if ( rcOS.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
				if ( ERROR_SUCCESS == ::RegOpenKey(HKEY_LOCAL_MACHINE, REG_KEY_WNT, &hOpenedKey) ) {
					for ( int i=0; ; i++ ) {
						TCHAR pValueName[_MAX_PATH];
						DWORD dwValueSize = sizeof(pValueName);
						DWORD dwType = REG_MULTI_SZ;
						if ( ERROR_SUCCESS == ::RegEnumValue(hOpenedKey, i, pValueName, &dwValueSize, NULL, &dwType, NULL, NULL) &&
								 dwType == REG_MULTI_SZ ) {

							_tcsupr( pValueName );
							CPathStr pServerShare( pServerName );
							if ( pServerShare ) {
								pServerShare ^= pValueName;
								if ( _tcsstr(pUprUNCPath, pServerShare) ) {
									DWORD dwSize = 0;
									if ( ERROR_SUCCESS == ::RegQueryValueEx(hOpenedKey, pValueName, 0, &dwType, NULL, &dwSize) && 
											 dwType == REG_MULTI_SZ ) {
										CAPointer<TCHAR> pValue = new TCHAR[dwSize];
										if ( pValue ) {
											if ( ERROR_SUCCESS == ::RegQueryValueEx(hOpenedKey, pValueName, 0, &dwType, (LPBYTE)(LPTSTR)pValue, &dwSize) && 
													 dwType == REG_MULTI_SZ ) {
												TCHAR *pStr = pValue;
												while ( pStr && _tcsnextc(pStr) != L'\0' ) {
													_tcsupr( pStr );
													TCHAR *pPtr = _tcsstr( pStr, PATH_VALUE_NT );
													if ( pPtr && pPtr == pStr ) {
														CPathStr pFullPath( _tcsninc(pStr, _tcsclen(PATH_VALUE_NT)) );
														if ( pFullPath ) {
															pFullPath ^= _tcsninc(pszUNCPath, _tcsclen(pServerShare));
															_tcsncpy( pszLocalPath, pFullPath, dwLocalPathSize );

															bResult = TRUE;
														}
														break;
													}
													pStr = _tcsninc( pStr, _tcsclen(pStr) ) + 1;
												}
											}
										}
									}
									break;
								}
							}
						}
						else
							break;
					}
					::RegCloseKey( hOpenedKey );
				}
			}
			else {
				if ( ERROR_SUCCESS == ::RegOpenKey(HKEY_LOCAL_MACHINE, REG_KEY_W95, &hOpenedKey) ) {
					for ( int i=0; ; i++ ) {
						TCHAR pSubKeyName[_MAX_PATH];
						if ( ERROR_SUCCESS != ::RegEnumKey(hOpenedKey, i, pSubKeyName, sizeof(pSubKeyName))	)
							break;

						_tcsupr( pSubKeyName );
						CPathStr pServerShare( pServerName );
						if ( pServerShare ) {
							pServerShare ^= pSubKeyName;
							if ( _tcsstr(pUprUNCPath, pServerShare) ) {
								HKEY hShareKey;
								if ( ERROR_SUCCESS == ::RegOpenKey(hOpenedKey, pSubKeyName, &hShareKey) ) {
									DWORD dwType = REG_SZ;
									CPathStr pFullPath( _MAX_PATH );
									if ( pFullPath ) {
										DWORD dwSize = _MAX_PATH;
										if ( ERROR_SUCCESS == ::RegQueryValueEx(hShareKey, _T("Path"), 0, &dwType, (LPBYTE)(LPTSTR)pFullPath, &dwSize) && 
												 dwType == REG_SZ ) {

											pFullPath ^= _tcsninc(pszUNCPath, _tcsclen(pServerShare));
											_tcsncpy( pszLocalPath, pFullPath, dwLocalPathSize );

											bResult = TRUE;
										}
									}
									::RegCloseKey( hShareKey );
								}
								break;
							}
						}
					}
					::RegCloseKey( hOpenedKey );
				}
			}
		}
	}
	return bResult;
}


// Convert Local path to corresponding UNC path
// pszLocalPath    - source Local path 
// pszUNCPath	     - buffer for UNC path
// dwUNCPathSize   - size of pszUNCPath
// Return value    - TRUE - conversion performed successfully
//       					   FALSE - any errors occurs
// ---
BOOL LocalPathToUNCPath( LPCTSTR pszLocalPath, LPTSTR pszUNCPath, DWORD dwUNCPathSize ) {
  BOOL  bResult = FALSE;
	
	CPathStr pServerName = ::GetServerName();
	if ( pServerName && *pServerName ) {
		HKEY  hOpenedKey;

		OSVERSIONINFO	rcOS;
		rcOS.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx( &rcOS );

		CPathStr pUprLocalPath(pszLocalPath);
		if ( pUprLocalPath ) {
			_tcsupr( pUprLocalPath );

			if ( rcOS.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
				if ( ERROR_SUCCESS == ::RegOpenKey(HKEY_LOCAL_MACHINE, REG_KEY_WNT, &hOpenedKey) ) {
					for ( int i=0; !bResult; i++ ) {
						TCHAR pValueName[_MAX_PATH];
						DWORD dwValueSize = sizeof(pValueName);
						DWORD dwType = REG_MULTI_SZ;
						DWORD dwSize = 0;
						if ( ERROR_SUCCESS == ::RegEnumValue(hOpenedKey, i, pValueName, &dwValueSize, NULL, &dwType, NULL, &dwSize) &&
								 dwType == REG_MULTI_SZ ) {
							CAPointer<TCHAR> pValue = new TCHAR[dwSize];
							if ( pValue ) {
								if ( ERROR_SUCCESS == ::RegQueryValueEx(hOpenedKey, pValueName, 0, &dwType, (LPBYTE)(LPTSTR)pValue, &dwSize) && 
										 dwType == REG_MULTI_SZ ) {
									TCHAR *pStr = pValue;
									while ( pStr && _tcsnextc(pStr) != L'\0' ) {
										_tcsupr( pStr );
										TCHAR *pPtr = _tcsstr( pStr, PATH_VALUE_NT );
										if ( pPtr && pPtr == pStr ) {
											CPathStr pSharePath( _tcsninc(pStr, _tcsclen(PATH_VALUE_NT)) );
											if ( pSharePath ) {
												_tcsupr( pSharePath );
												if ( _tcsstr(pUprLocalPath, pSharePath) ) {
													CPathStr pFullPath(_MAX_PATH);
													if ( pFullPath ) {
														pFullPath += pServerName;
														pFullPath ^= pValueName;
														pFullPath ^= _tcsninc(pszLocalPath, _tcsclen(pSharePath));
														_tcsncpy( pszUNCPath, pFullPath, dwUNCPathSize );

														bResult = TRUE;
													}
													break;
												}
											}
										}
										pStr += _tcsclen(pStr) + 1;
									}
								}
							}
							break;
						}
					}
					::RegCloseKey( hOpenedKey );
				}
			}
			else {
				if ( ERROR_SUCCESS == ::RegOpenKey(HKEY_LOCAL_MACHINE, REG_KEY_W95, &hOpenedKey) ) {
					for ( int i=0; !bResult; i++ ) {
						TCHAR pSubKeyName[_MAX_PATH];
						if ( ERROR_SUCCESS != ::RegEnumKey(hOpenedKey, i, pSubKeyName, sizeof(pSubKeyName))	)
							break;

						HKEY hShareKey;
						if ( ERROR_SUCCESS == ::RegOpenKey(hOpenedKey, pSubKeyName, &hShareKey) ) {
							DWORD dwType = REG_SZ;
							TCHAR pSharePath[_MAX_PATH];
							DWORD dwSize = _MAX_PATH;
							if ( ERROR_SUCCESS == ::RegQueryValueEx(hShareKey, _T("Path"), 0, &dwType, (LPBYTE)(LPTSTR)pSharePath, &dwSize) && 
									 dwType == REG_SZ ) {

								_tcsupr( pSharePath );
								if ( _tcsstr(pUprLocalPath, pSharePath) ) {
									CPathStr pFullPath(_MAX_PATH);
									if ( pFullPath ) {
										pFullPath += pServerName;
										pFullPath ^= pSubKeyName;
										pFullPath ^= _tcsninc(pszLocalPath, _tcsclen(pSharePath));
										_tcsncpy( pszUNCPath, pFullPath, dwUNCPathSize );

										bResult = TRUE;
									}
								}
							}
							::RegCloseKey( hShareKey );
						}
					}
					::RegCloseKey( hOpenedKey );
				}
			}
		}
	}
	return bResult;
}

