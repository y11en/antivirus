#ifndef _KL_MEMORY_H_
#define _KL_MEMORY_H_

#include "kldef.h"

void*   
KlAlloc(ULONG    size);

void     
KlFree(void*    memory);

ULONG
KlMemSize(void* memory);

void*    
KlRealloc(void* OldMemory, ULONG    NewSize);

#endif