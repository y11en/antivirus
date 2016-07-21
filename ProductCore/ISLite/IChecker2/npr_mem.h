#ifndef _NPR_MEM_
#define _NPR_MEM_

#if defined(_NO_PRAGUE_)

#include <stdlib.h>

#define CALL_SYS_ObjHeapAlloc(_this, pptr, dwSize ) ( *pptr = malloc(dwSize), ( *pptr ? errOK : errNOT_ENOUGH_MEMORY ) )
#define CALL_SYS_ObjHeapFree(_this, ptr) ( ( ptr ? free(ptr) : 0 ), errOK )

#endif // _NO_PRAGUE_

#endif //_NPR_MEM_
