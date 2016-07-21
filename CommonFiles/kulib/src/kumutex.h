

#ifndef __KUMUTEX_H
#define __KUMUTEX_H

#include "kulib/ku_defs.h"

KU_CDECL_BEGIN

typedef void* kumutex_t;

int kumutex_init( kumutex_t* mutex );
int kumutex_destroy( kumutex_t* mutex );
int kumutex_lock( kumutex_t* mutex );
int kumutex_unlock( kumutex_t* mutex );

KU_CDECL_END

#endif // __KUMUTEX_H
