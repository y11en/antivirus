//-----------------------------------------------------------------------------
//
//  loadResString - helper function loads string from string table resource
//                  of specified language.
//
//  Developer:  Zubrilin Sergey
//  Date:       24.09.2003
//-----------------------------------------------------------------------------


#pragma once

#include <tchar.h>
#include <windows.h>

//-----------------------------------------------------------------------------
//
// Parameters:
//  hModule         - [in] handle of module containing resource
//  stringId        - [in] string id in string table
//  langId          - [in] resource language identifier (made by MAKELANGID)
//  buf             - [out] pointer to buffer to store loaded string
//  bufSize         - [in/out] pointer to variable containing size of buf.
//                             After function return contains actual string size
//                             stored in buf
//
// Return:
// ERROR_SUCCESS    - operation completed successfully
// ERROR_MORE_DATA  - buffer size too small. bufSize contains required buffer size
// 
// Notes:
//   It is possible to call loadResString with buf=0. Required buffer size will
//   be returned in this case alone with ERROR_MORE_DATA error code.

DWORD   loadResString 
    (HMODULE    hModule,        // handle of module containing resource
     UINT       stringId,       // string id
     WORD       langId,         // language identifier (by MAKELANGID)
     _TCHAR     *buf,           // pointer to buffer for output string
     DWORD      *bufSize        // pointer to variable containing buffer size
    );
