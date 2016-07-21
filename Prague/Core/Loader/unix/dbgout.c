#if defined (__unix__)

#include <Prague/prague.h>
#include <stdarg.h>
#include "loader.h"
#include <Prague/iface/e_ktrace.h>

#define IMPEX_EXPORT_LIB
#include <Prague/iface/e_loader.h>

#include <pthread.h>
#include <stdio.h>

tCHAR            g_buffer[0x1000];

pthread_mutex_t g_output_mutex = PTHREAD_MUTEX_INITIALIZER;

tERROR pr_call PrOutputDebugString( tCHAR* format, ... ) {
  va_list args;
  pthread_mutex_lock ( &g_output_mutex );
  va_start( args, format );
  pr_vsprintf( g_buffer, countof(g_buffer), format, args );
  va_end( args );
  fprintf ( stderr, g_buffer );
  pthread_mutex_unlock ( &g_output_mutex );
  return errOK;
}

// ---
tERROR pr_call PrOutputDebugString_v( tCHAR* format, tPTR stack_addr ) {
  pthread_mutex_lock ( &g_output_mutex );
  pr_vsprintf( g_buffer, countof(g_buffer), format, stack_addr );
  fprintf ( stderr, g_buffer );
  pthread_mutex_unlock ( &g_output_mutex );
  return errOK;
}

// ---

#endif //__unix__
