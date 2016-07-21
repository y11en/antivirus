////////////////////////////////////////////////////////////////////
//
//  COMUtils.h
//  COM utility functions definition
//  AVP generic purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __COMUTILS_H__
#define __COMUTILS_H__

#include <windows.h>
#include <wtypes.h>

// Convert GUID to string
// rcGUID - reference to GUID
// pBuffer - pointer to buffer for string representation of rcGUID. 
//           Should be at least 40 characters long
// dwBufferSize - size of buffer
HRESULT StringFromGUID( const GUID &rcGUID, TCHAR *pBuffer, DWORD dwBufferSize );

// Convert string to GUID 
// pStrGUID - pointer to string representation of GUID. 
// rcGUID		- reference to GUID
HRESULT GUIDFromString( const TCHAR *pStrGUID, GUID &rcGUID );

// Load COM object and retrieve interface
// pszCLSID - pointer to string representation of object's CLSID
//            f.e. "{5BD0C6A2-A6A5-11D3-96B0-00104B5B66AA}"
// pszIID		- pointer to string representation of interface's IID
//            f.e. "{DE27B547-5164-11D3-96B0-00104B5B66AA}"
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// 
void *LoadCOMObject( const TCHAR *pszCLSID, const TCHAR *pszIID, HRESULT *phResult = NULL, BOOL bInitCOM = TRUE );

// Load COM object fromwithin COM object and retrieve interface
// pszCLSID - pointer to string representation of object's CLSID
//            f.e. "{5BD0C6A2-A6A5-11D3-96B0-00104B5B66AA}"
// pszIID		- pointer to string representation of interface's IID
//            f.e. "{DE27B547-5164-11D3-96B0-00104B5B66AA}"
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// 
void *COMLoadCOMObject( const TCHAR *pszCLSID, const TCHAR *pszIID, HRESULT *phResult = NULL, BOOL bInitCOM = TRUE );

// Load COM object fromwithin COM object and retrieve interface
// rCLSID   - reference to object's CLSID
// rIID		  - reference to interface's IID
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// 
void *COMLoadCOMObjectID( REFCLSID rCLSID, REFIID rIID, HRESULT *phResult = NULL, BOOL bInitCOM = TRUE );

// Load COM object and retrieve interface
// pszCLSID - pointer to string representation of object's CLSID
//            f.e. "{5BD0C6A2-A6A5-11D3-96B0-00104B5B66AA}"
// IntID		- interface's IID
// Return value - pointer to interface presented by pszIID, or 0 if error occurs
// DON'T forget to call Release after interface no longer needed
// ---
void *LoadCOMObject2( const TCHAR *pszCLSID, const IID &IntID, HRESULT *phResult = NULL);

#endif //__COMUTILS_H__