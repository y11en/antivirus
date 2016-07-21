////////////////////////////////////////////////////////////////////
//
//  UNCConvert.h
//  UNC path to Local path and vice-versa conversion functions definition
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __UNCCONVERT_H__
#define __UNCCONVERT_H__

#include <windows.h>

// Follow functions perform conversion from UNC path to Local path and vice-versa.
// In fact Win32 API has no docunented, legal functions for such conversion.
// Exception is the function WNetGetUniversalName, but it works with 
// network shared and connected resources only. (Implementation of such conversion
// for WinNT and Win95 platforms situated in module "StuffIO\IOUtil.cpp")
// This point we are interested in conversion from UNC path pointing to computer's local object
// to really local path (and vice-cersa too). 
// This is why follow functions were born.
// They perform light-weight, so-called "registry" conversion, i.e. they are looking for shared 
// resource path through registry key discrabing such resources and preparing full converted path.
// Registry key enumerated by this functions are
// Win95/98   : "HKLM\Software\Microsoft\Windows\CurrentVersion\NetWork\LanMan"
// WinNT/2000 : "HKLM\SYSTEM\CurrentControlSet\Services\LanmanServer\Shares"
// Functions do not have a dealing with registry permissions, so if keys above have no permissions
// to access the conversion will not be performed.


// Convert UNC path to corresponding Local path
// pszUNCPath      - source UNC path 
// pszLocalPath	   - buffer for local path
// dwLocalPathSize - size of pszLocalPath
// Return value    - TRUE - conversion performed successfully
//       					   FALSE - any errors occurs
// ---
BOOL UNCPathToLocalPath( LPCTSTR pszUNCPath, LPTSTR pszLocalPath, DWORD dwLocalPathSize );


// Convert Local path to corresponding UNC path
// pszLocalPath    - source Local path 
// pszUNCPath	     - buffer for UNC path
// dwUNCPathSize   - size of pszUNCPath
// Return value    - TRUE - conversion performed successfully
//       					   FALSE - any errors occurs
// ---
BOOL LocalPathToUNCPath( LPCTSTR pszLocalPath, LPTSTR pszUNCPath, DWORD dwUNCPathSize );


#endif //__UNCCONVERT_H__