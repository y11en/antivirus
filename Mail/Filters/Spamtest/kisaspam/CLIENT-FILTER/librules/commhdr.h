#ifndef __commhdr_h__
#define __commhdr_h__

// commhdr.h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
 
#include "SpamTestCFG.h"

#define LOG(msg) WriteToLog(LOG_DEBUG,msg)
#define LOG2(msg1,msg2) WriteToLog(LOG_DEBUG,msg1,msg2)
#define LOG3(msg1,msg2,msg3) WriteToLog(LOG_DEBUG,msg1,msg2,msg3)

#ifdef _DEBUG
    #define LOGDEBUG(msg) WriteToLog(LOG_DEBUG,msg)
#else
    #define LOGDEBUG(msg) ((void)msg)
#endif

#include <assert.h>

#ifndef STRICT
#define STRICT // for Windows.H
#endif

#include <windows.h>
#include <tchar.h>

#include <fcntl.h>

typedef const unsigned char* LPCBYTE;

#ifndef UNREF
#define UNREF(p) ((void)(p))
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif /*__commhdr_h__*/
