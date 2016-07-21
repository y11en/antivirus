// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#ifdef __MWERKS__
	#include <nwtypes.h>	
	#include "_carray.h"
	#include "ntypedef.h"
	#include <dupmem/dupmem.h>
#elif defined (__unix__)
	#include <stdio.h>	// NULL
	#include <stdarg.h>
	#include <sys/types.h>	
	#include <sys/stat.h>
	#include <string.h>

	#if (defined (__sparc__) || defined (__hpux__)) && !defined (BASE_EMULATOR)
		#error You should define BASE_EMULATOR on non-Intel platforms!
	#endif
	
	#if defined (BASE_EMULATOR)
		#include "emulator/emulator.h"
		#include "emulator/wrappers.h"
	#endif /* defined BASE_EMULATOR */

	#include <unix/compatutils/compatutils.h>
	#include <AVPPort.h>
	#include <compat_unix.h>	
	#include <Stuff/_CARRAY.h>
	#include <ScanAPI/Avp_msg.h>
	#include <Bases/Retcode3.h>
	#include <DupMem/DupMem.h>
	#include <stdarg.h>
	#include <stdlib.h>
	#define strnicmp strncasecmp
#else

//#ifndef _DEBUG
#define NOAFX
//#endif

#ifdef NOAFX

#ifndef SYSTOS2
#include <memory.h>
#include <string.h>
#include <windows.h>

#ifndef MULTITHREAD
#define MULTITHREAD
#endif

#else
#define INCL_DOSMODULEMGR
#define INCL_DOSPROCESS
#define INCL_WIN
#include <os2.h>
#include <bases/format/typedef.h>
#endif

#include <tchar.h>
#define AFXAPI
#include <stuff/_carray.h>
#include <scanapi/avp_msg.h>
#include <bases/retcode3.h>
#include <dupmem/dupmem.h>


#else // AFX

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

#endif //AFX

#endif


