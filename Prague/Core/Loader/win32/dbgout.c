#if defined (_WIN32)

#include <Prague/prague.h>
#include <Prague/iface/e_ktrace.h>

#include "loader.h"

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>

#include <windows.h>
#include <stdarg.h>

tCHAR            g_buffer[0x1000];
CRITICAL_SECTION g_output_cs;

tERROR pr_call PrOutputDebugString( tCHAR* format, ... ) {
	va_list args;

	EnterCriticalSection( &g_output_cs );
        va_start( args, format );
        pr_vsprintf( g_buffer, countof(g_buffer), format, args );
        va_end( args );
        OutputDebugString( g_buffer );
	LeaveCriticalSection( &g_output_cs );
	return errOK;
}

// ---
tERROR pr_call PrOutputDebugString_v( tCHAR* format, tPTR stack_addr ) {
	EnterCriticalSection( &g_output_cs );
	pr_vsprintf( g_buffer, countof(g_buffer), format, stack_addr );
	OutputDebugString( g_buffer );
	LeaveCriticalSection( &g_output_cs );
	return errOK;
}

#endif // _WIN32
