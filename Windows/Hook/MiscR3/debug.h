#ifndef __debug_h__
#define __debug_h__
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ODST
#define _ODST(text) text
#endif

#define ODS OUTPUTDEBUGSTRING
#define ODSN OUTPUTDEBUGSTRINGN

#if defined(_DEBUG) || defined(ENABLE_ODS) || defined(ENABLE_KLTR)

#define OUTPUTDEBUGSTRING(x) (DbgOutputStringLF x)
#define OUTPUTDEBUGSTRINGN(x) (DbgOutputString x)
#define DBG_STOP assert(0)//{__asm int 3}
#define __try
#define __except(x) if(0)

#else

#pragma message ("debug output disabled")
#define OUTPUTDEBUGSTRING(x) 
#define OUTPUTDEBUGSTRINGN(x) 
#define DBG_STOP {}


#endif	

void DbgOutputStringLF(char* szFormat, ...);
void DbgOutputString(char* szFormat, ...);
char* DbgStringFromGUID(char* szBuff, DWORD dwBuffSize, const GUID* pGUID);
void DbgPrintGUID(REFIID riid);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
