#ifndef __SPCALLOC_H
#define __SPCALLOC_H

#define SPCALLOC_NEW        0
#define SPCALLOC_GLOBAL     1 //used in multiengine architecture to alloc common data
#define SPCALLOC_EXECUTABLE 2 //for future support

void* SpcAlloc(unsigned size, unsigned type);
void  SpcFree(void* ptr, unsigned type);

#endif//__SPCALLOC_H
