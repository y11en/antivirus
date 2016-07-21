#ifndef __KLDRV_DEBUG_H_
#define __KLDRV_DEBUG_H_

#define __ulong unsigned long

#include <windows.h>
#include "OwnSync.h"
#include <tchar.h>

#ifndef countof
#define countof(array) (sizeof(array) / sizeof(array[0]))
#endif

//+ ----------------------------------------------------------------------------------------

#define DCB_NOCAT	0x00
#define DCB_MEMORY	0x01
#define DCB_DRIVER	0x02
#define DCB_SYSTEM	0x04
#define DCB_BL		0x08
#define DCB_ALL		0xFFFFFFFF

typedef enum _DEBUG_INFO_LEVEL
{
	DL_ALWAYS_REPORTED      =0,
	DL_FATAL_ERROR          =1,
	DL_ERROR                =2,
	DL_WARNING              =3,
	DL_IMPORTANT            =4,
	DL_NOTIFY               =5,
	DL_INFO                 =6,
	DL_SPAM                 =7,
	DL_MAX,
} DEBUG_INFO_LEVEL;

//+ ----------------------------------------------------------------------------------------
#if defined (__DBGOUT) || defined (_DEBUG)
	#define DbgBreakPoint() 	__asm int 3
	extern COwnSync gDbgSync;
	
	void __DbPrintEx(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt, ...);
	void __DbPrintExV(__ulong Category, DEBUG_INFO_LEVEL Level, LPCTSTR lpFmt, va_list arg_list);
	#define _DbPrint(_args) __DbPrintEx _args;
	#define DbPrint(_args) m_Log.AddFormatedString _args;
	
	#define DbPrintV(_args) __DbPrintExV _args;
		
	#define __alertif(_condition) if (_condition)\
	{\
		__DbPrintEx(DCB_SYSTEM, DL_ERROR, L"alert: , %S. Line %d in '%S'", #_condition, __LINE__, __FILE__);\
		throw "__check failed";\
	}
#else
	#define DbgBreakPoint() {}
	#define DbPrint(_args) {}
	#define DbPrintV(_args) {}

	#define __alertif(_condition) {}
#endif

#include "../log.h"

#endif //__KLDRV_DEBUG_H_
