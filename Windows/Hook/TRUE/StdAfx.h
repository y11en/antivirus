// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AF2BADFD_1FB2_47EC_B8A4_CD57DDBBDB59__INCLUDED_)
#define AFX_STDAFX_H__AF2BADFD_1FB2_47EC_B8A4_CD57DDBBDB59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#ifdef _CONSOLE

#define _WINSOCKAPI_ //prevent include winsock.h

typedef void*	PDEVICE_OBJECT;

#include <windows.h>
#include <stdio.h>

#ifdef _DEBUG
/*
#define Message( _DebugLevel, _DebugMask, _Message )	\
if (((_DebugMask) & DebugTrueMask ) && DebugTrueLevel >= _DebugLevel ) { \
	printf _Message;  \
}
*/
#define Message( _DebugLevel, _DebugMask, _Message )

#else
#define Message( _DebugLevel, _DebugMask, _Message )
#endif // _DEBUG

#else //_CONSOLE

#include <ntddk.h>
#include "defs.h"

#if defined(_DEBUG) || defined(_TEST_RELEASE)
/*
#define	Message( _DebugLevel, _DebugMask, _Message )	\
if (((_DebugMask) & DebugTrueMask ) && DebugTrueLevel >= _DebugLevel) { \	
	DBL_LEVELS _xTemp = _DebugLevel; \
	DbgPrint _Message;  \
}
*/
#define Message( _DebugLevel, _DebugMask, _Message )
#else
#define Message( _DebugLevel, _DebugMask, _Message )
#endif // _DEBUG || _TEST_RELEASE

#endif //_CONSOLE

#include <hook\avpgcom.h>
#include <hook\hookspec.h>
#include "Common.h"
#include "types.h"
#include "NetParser.h"
#include "FilterEvent.h"
#include <hook\NETID.h>

#pragma pack(1)





// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AF2BADFD_1FB2_47EC_B8A4_CD57DDBBDB59__INCLUDED_)
