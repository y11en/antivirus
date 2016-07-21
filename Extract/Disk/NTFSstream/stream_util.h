#ifndef _stream_util_h
#define _stream_util_h

#include <Prague/prague.h>

#define UNI(a) ((tWCHAR*)(a))
#define MB(a)  ((tCHAR*)(a))
#define WS(a)  (sizeof(tWCHAR)*(a))


tERROR pr_call _ConvertWin32Err( tDWORD win32err );
tERROR pr_call _is_stream_name( const tWSTRING name, tBOOL* preceding_colon );
tERROR pr_call _not_name_of_the_stream( const tWSTRING name );


extern HINSTANCE g_ntdll;
extern NQIF      g_qif;
extern tPROPID   g_pid_this_is_the_stream;
extern tPROPID   g_pid_streams;


#endif