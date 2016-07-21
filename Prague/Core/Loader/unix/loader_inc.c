#if !defined (_LOADER_UNIX_INC_)
#define _LOADER_UNIX_INC_

#define call_func_on_an_invisible_thread( func, param1, param2, param3 ) ((func)(param1, param2, param3))

#if defined( GLOBAL_PLUGIN_PROTECTOR )
#include <pthread.h>
extern pthread_mutex_t g_plugin_protector;
#endif

extern void*            g_loader;
extern hROOT            g_root;
extern tSignCheckFunc   g_signCheckFunc; 
extern tSignCheckNotify g_signCheckNotify; 
extern tBOOL            g_bUnderNT; 
/* extern tDWORD           g_dwWinVerMajor; */
/* extern tDWORD           g_dwWinVerMinor; */
#endif //_LOADER_UNIX_INC_
