#ifndef __KLDRV_DEBUG_H_
#define __KLDRV_DEBUG_H_

#include "windows.h"
#include "OwnSync.h"

#define __ulong unsigned long

typedef enum _DEBUG_INFO_LEVEL
{
	DL_FATAL_ERROR,		//0
	DL_ERROR			,//1
	DL_WARNING			,//2
	DL_IMPORTANT		,//3
	DL_NOTIFY			,//4
	DL_INFO				,//5
	DL_SPAM				 //6
}DEBUG_INFO_LEVEL;

//+ ----------------------------------------------------------------------------------------

#define DCB_NOCAT		0	// 
#define DCB_MEMORY		1	// 

//+ ----------------------------------------------------------------------------------------
#if defined (__DBGOUT) || defined (_DEBUG)
	#define DbgBreakPoint() 	__asm int 3
	extern COwnSync gDbgSync;
	
	void __DbgPrint(LPCWSTR lpFmt,...);
	bool __DbPrint(__ulong Category, DEBUG_INFO_LEVEL Level);
	#define DbPrint(_category, _level, _arg) \
	{\
		CAutoLock _dbgout(gDbgSync);\
		if (__DbPrint(_category, _level)) {__DbgPrint _arg;}\
	}

	#define __alertif(_condition) if (_condition)\
	{\
		DbPrint(DCB_NOCAT, DL_ERROR, L"alert: , %S. Line %d in '%S'", #_condition, __LINE__, __FILE__)\
		throw "__check failed";\
	}
#else
	#define DbgBreakPoint() {}
	#define DbPrint(Category,Level,arg) {}

	#define __alertif(_condition) {}
#endif

#endif //__KLDRV_DEBUG_H_
