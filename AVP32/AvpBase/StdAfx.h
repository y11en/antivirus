// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

//#ifndef _DEBUG
#define NOAFX
//#endif

#ifdef NOAFX
#ifndef SYSTOS2
#include <memory.h>
#include <string.h>
#include <WINDOWS.H>
#else
#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#define INCL_WIN
#include <os2.h>
#endif

#include <TCHAR.H>
#define AFXAPI
#include <Stuff/_CARRAY.H>
#else



#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC OLE automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#endif

#include <Bases/retcode3.h>
#include <ScanAPI/avp_msg.h>

