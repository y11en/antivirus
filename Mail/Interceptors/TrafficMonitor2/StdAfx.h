// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__91D9FF2D_60E8_4BBB_B211_610366F4F7D8__INCLUDED_)
#define AFX_STDAFX_H__91D9FF2D_60E8_4BBB_B211_610366F4F7D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_DCOM // for COINIT_MULTITHREADED macro

// TODO: reference additional headers your program requires here
#include <stdio.h>
#include <prague\prague.h>
#include <prague\pr_cpp.h>
#include <prague\pr_vector.h>
#include <prague\iface\i_root.h>
#include <prague\iface\i_threadpool.h>

#include <winsock2.h>
#include "Ws2tcpip.h"
#include "sock_util.h"

#include "inline.h"

#include <map>
#include "vector"
#include "list"
#include "algorithm"

#include <mail/structs/s_mc_trafficmonitor.h>

#include "TrafficProtocollerTypes.h"
#include "TrafficProtocoller.h"

// #include "TransparentSocket.h"
// #include "ReadWriteSocket.h"
// #include "TrafficProtocoller.h"

// #include "SimpleBufferWindow.h"
// #include "ProxySession.h"
// #include "ProxySessionManager.h"

#include <CKAH/CKAHUM.h>
#include <CKAH/ckahcr.h>

//#define KLSTD_TRACE0(_level, _tmlstr)	                            { printf( "proxyTracer[0x%X]:: " _tmlstr "\n", GetCurrentThreadId() ) ; }
//#define KLSTD_TRACE1(_level, _tmlstr, arg1)                       { printf( "proxyTracer[0x%X]:: " _tmlstr "\n", GetCurrentThreadId(), arg1) ; }
//#define KLSTD_TRACE2(_level, _tmlstr, arg1, arg2)                 { printf( "proxyTracer[0x%X]:: " _tmlstr "\n", GetCurrentThreadId(), arg1,arg2) ; }
//#define KLSTD_TRACE3(_level, _tmlstr, arg1, arg2, arg3)           { printf( "proxyTracer[0x%X]:: " _tmlstr "\n", GetCurrentThreadId(), arg1,arg2,arg3) ; }
//#define KLSTD_TRACE4(_level, _tmlstr, arg1, arg2, arg3, arg4)     { printf( "proxyTracer[0x%X]:: " _tmlstr "\n", GetCurrentThreadId(), arg1,arg2,arg3,arg4) ; }

#define MC_TRACE_LEVEL	prtNOTIFY // prtALWAYS_REPORTED_MSG // prtIMPORTANT
#define KLMC_TRACELEVEL MC_TRACE_LEVEL
#define KLSTD_IF_TRACE_ENABLED \
    if (OBJ_TRACE_LEVEL(g_root)->min > prtALWAYS_REPORTED_MSG || OBJ_TRACE_LEVEL(g_root)->max > prtALWAYS_REPORTED_MSG)

#define KLSTD_TRACE0(_level, _tmlstr)	                          { KLSTD_IF_TRACE_ENABLED { PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\t" _tmlstr) ); } }
#define KLSTD_TRACE1(_level, _tmlstr, arg1)                       { KLSTD_IF_TRACE_ENABLED { PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\t" _tmlstr,arg1) ); }}
#define KLSTD_TRACE2(_level, _tmlstr, arg1, arg2)                 { KLSTD_IF_TRACE_ENABLED { PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\t" _tmlstr,arg1,arg2 )); }}
#define KLSTD_TRACE3(_level, _tmlstr, arg1, arg2, arg3)           { KLSTD_IF_TRACE_ENABLED { PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\t" _tmlstr,arg1,arg2,arg3) ); }}
#define KLSTD_TRACE4(_level, _tmlstr, arg1, arg2, arg3, arg4)     { KLSTD_IF_TRACE_ENABLED { PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\t" _tmlstr,arg1,arg2,arg3,arg4) ); }}
#define KLSTD_TRACE5(_level, _tmlstr, arg1, arg2, arg3, arg4, arg5 ) { KLSTD_IF_TRACE_ENABLED { PR_TRACE( (NULL, MC_TRACE_LEVEL, "mc\t" _tmlstr,arg1,arg2,arg3,arg4,arg5) ); }}

#define IS_VALID_HANDLE(hHandle) !(hHandle == NULL || hHandle == INVALID_HANDLE_VALUE)
#define CLOSEHANDLE(h)	                        \
{	                                            \
	if ( (h) && ((h) != INVALID_HANDLE_VALUE) )	\
	{                                           \
		CloseHandle((h));	                    \
		(h) = NULL;	                            \
	}	                                        \
}

#define TRACE_LASTERROR( _txt_ )	            \
{	                                            \
    DWORD   dwStatus;	                        \
    TCHAR   *pBuf;	                            \
	DWORD   err = GetLastError();	            \
    dwStatus = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM     |	     \
		                     FORMAT_MESSAGE_ALLOCATE_BUFFER |        \
		                     FORMAT_MESSAGE_IGNORE_INSERTS,          \
		                     NULL,	                                 \
		                     err,                                    \
		                     LANG_NEUTRAL,                           \
		                     (LPTSTR)&pBuf,                          \
		                     64,                                     \
		                     NULL);                                  \
    if (dwStatus != 0)                          \
	{                                           \
		PR_TRACE( (NULL, MC_TRACE_LEVEL, "MC:: " _txt_ ". LastError = \"%s\"", pBuf) );	\
        LocalFree(pBuf);                        \
    } else                                      \
	{                                           \
		PR_TRACE( (NULL, MC_TRACE_LEVEL, "MC:: " _txt_ ". LastError = \"%d\"", err) );  \
    }                                           \
}

#define INTERLOCKED_INC( _lock, _v_ ) \
(_lock).lock(); ++(_v_); (_lock).unlock();

#define INTERLOCKED_DEC( _lock, _v_ ) \
(_lock).lock(); --(_v_); (_lock).unlock();

#define INTERLOCKED_ASSIGN( _lock, _v_, _a_ ) \
(_lock).lock(); (_v_) = (_a_); (_lock).unlock();
    

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__91D9FF2D_60E8_4BBB_B211_610366F4F7D8__INCLUDED_)
