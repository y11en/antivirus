// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#ifdef __MWERKS__
	#include <nwtypes.h>
	#include "ntypedef.h"
	#include "_carray.h"
#elif defined (__unix__)
    #define NOAFX
	#include <AVPPort.h>
	#include <compat_unix.h>	
	#include "_CARRAY.h"
#else
#if !defined(STDAFX_H__C83ACE4E_44DB_11D3_96B0_00104B5B66AA__INCLUDED_)
#define STDAFX_H__C83ACE4E_44DB_11D3_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

#define NOAFX

#endif // !defined(STDAFX_H__C83ACE4E_44DB_11D3_96B0_00104B5B66AA__INCLUDED_)
#endif
