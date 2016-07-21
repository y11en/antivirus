#ifndef __port_palm_h_
#define __port_palm_h_

#define PILOT_PRECOMPILED_HEADERS_OFF
#include <pilot.h>
#define __cdecl
typedef unsigned int size_t;

#define memcpy MemMove
#define memset MemSet
#define strlen StrLen
#define strcpy StrCopy

#endif