#ifndef __debug_h__
#define __debug_h__

#ifdef __cplusplus
extern "C" {
#endif

#define ODS OUTPUTDEBUGSTRING
#define ODSN OUTPUTDEBUGSTRINGN

#if defined(_DEBUG) || defined(ENABLE_ODS)

#define OUTPUTDEBUGSTRING(x) (DbgOutputString x, DbgOutputString("\n"))
#define OUTPUTDEBUGSTRINGN(x) (DbgOutputString x)
#define DBG_STOP {__asm int 3}
#if !defined(_USE_TRY_)
#define __try
#define __except(x) if(0)
#endif

#else

#pragma message ("debug output disabled")
#define OUTPUTDEBUGSTRING(x) {}
#define OUTPUTDEBUGSTRINGN(x) {}
#define DBG_STOP {}


#endif	

void DbgOutputString(char* szFormat, ...);
void DbgPrintGUID(REFIID riid);

#ifdef __cplusplus
} // extern "C"
#endif

#endif
