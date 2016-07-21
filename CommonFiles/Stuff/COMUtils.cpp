////////////////////////////////////////////////////////////////////
//
//  COMUtils.cpp
//  COM utility functions implementation
//  AVP generic purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <tchar.h>
#include "COMUtils.h"


// Convert GUID to string
// rcGUID - reference to GUID
// pBuffer - pointer to buffer for string representation of rcGUID. 
//           Should be at least 40 characters long
// dwBufferSize - size of buffer
HRESULT StringFromGUID( const GUID &rcGUID, TCHAR *pBuffer, DWORD dwBufferSize ) {
	OLECHAR  *pWClsidStr;
	HRESULT   hResult = ::StringFromCLSID( rcGUID, &pWClsidStr );
	if ( SUCCEEDED(hResult) ) {
#ifdef UNICODE
		_tcsncpy( pBuffer, pWClsidStr, dwBufferSize );
#else
		TCHAR     pClsidStr[40] = {0};
		::WideCharToMultiByte( CP_ACP, 0/*WC_SEPCHARS*/, pWClsidStr, -1, pClsidStr, sizeof(pClsidStr), 0, 0 );
		_tcsncpy( pBuffer, pClsidStr, min(sizeof(pClsidStr), dwBufferSize) );
#endif
		::CoTaskMemFree( pWClsidStr );
	}
	return hResult;
}

// Convert string to GUID 
// pStrGUID - pointer to string representation of GUID. 
// rcGUID		- reference to GUID
HRESULT GUIDFromString( const TCHAR *pStrGUID, GUID &rcGUID ) {
#ifdef UNICODE
	return ::CLSIDFromString( (TCHAR *)pStrGUID, &rcGUID );
#else
	OLECHAR pWClsidStr[40];
	::MultiByteToWideChar( CP_ACP, 0, pStrGUID, -1, pWClsidStr, 40 );
	return ::CLSIDFromString( pWClsidStr, &rcGUID );
#endif
}


// Load COM object and retrieve interface
// pszCLSID - pointer to string representation of object's CLSID
//            f.e. "{5BD0C6A2-A6A5-11D3-96B0-00104B5B66AA}"
// pszIID		- pointer to string representation of interface's IID
//            f.e. "{DE27B547-5164-11D3-96B0-00104B5B66AA}"
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// ---
void *LoadCOMObject( const TCHAR *pszCLSID, const TCHAR *pszIID, HRESULT *phResult, BOOL bInitCOM/* = TRUE*/ ) {
	void *pInterface = NULL;
	HRESULT hResult = bInitCOM ? ::CoInitialize( NULL ) : S_OK;

	if ( SUCCEEDED(hResult) ) {
		pInterface = ::COMLoadCOMObject( pszCLSID, pszIID, &hResult );
	}

	if ( bInitCOM )
		::CoUninitialize();

	if ( phResult )
		*phResult = hResult;

	return pInterface;
}


// Load COM object fromwithin COM object and retrieve interface
// pszCLSID - pointer to string representation of object's CLSID
//            f.e. "{5BD0C6A2-A6A5-11D3-96B0-00104B5B66AA}"
// pszIID		- pointer to string representation of interface's IID
//            f.e. "{DE27B547-5164-11D3-96B0-00104B5B66AA}"
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// 
void *COMLoadCOMObject( const TCHAR *pszCLSID, const TCHAR *pszIID, HRESULT *phResult, BOOL bInitCOM/* = TRUE*/ ) {
	void *pInterface = NULL;
	CLSID clsID;
	GUID  intID;

	HRESULT hResult = bInitCOM ? ::CoInitialize( NULL ) : S_OK;

	if ( SUCCEEDED(hResult) ) {
		hResult = ::GUIDFromString( pszCLSID, clsID );
		if ( SUCCEEDED(hResult) ) { 
			hResult = ::GUIDFromString( pszIID, intID );
			if ( SUCCEEDED(hResult) ) 
				hResult = ::CoCreateInstance( clsID, NULL, CLSCTX_SERVER /*CLSCTX_INPROC_SERVER*/, intID, (void **)&pInterface );
		}
	}

	if ( bInitCOM )
		::CoUninitialize();

	if ( phResult )
		*phResult = hResult;

	return pInterface;
}

// Load COM object fromwithin COM object and retrieve interface
// rCLSID   - reference to object's CLSID
// rIID		  - reference to interface's IID
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// 
void *COMLoadCOMObjectID( REFCLSID rCLSID, REFIID rIID, HRESULT *phResult, BOOL bInitCOM/* = TRUE*/ ) {
	void *pInterface = NULL;
	
	HRESULT hResult = bInitCOM ? ::CoInitialize( NULL ) : S_OK;

	if ( SUCCEEDED(hResult) ) {
		hResult = ::CoCreateInstance( rCLSID, NULL, CLSCTX_SERVER /*CLSCTX_INPROC_SERVER*/, rIID, (void **)&pInterface );
	}

	if ( bInitCOM )
		::CoUninitialize();

	if ( phResult )
		*phResult = hResult;

	return pInterface;
}

// Load COM object and retrieve interface
// pszCLSID - pointer to string representation of object's CLSID
//            f.e. "{5BD0C6A2-A6A5-11D3-96B0-00104B5B66AA}"
// IntID		- interface's IID
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// ---
void *LoadCOMObject2( const TCHAR *pszCLSID, const IID &IntID, HRESULT *phResult) {
	void *pInterface = NULL;

	HRESULT hResult;

	CLSID clsID;
	hResult = ::GUIDFromString( pszCLSID, clsID );
	if ( SUCCEEDED(hResult) ) { 
			hResult = ::CoCreateInstance( clsID, NULL, CLSCTX_SERVER /*CLSCTX_INPROC_SERVER*/, IntID, (void **)&pInterface );
	}

	if ( phResult )
		*phResult = hResult;

	return pInterface;
}
