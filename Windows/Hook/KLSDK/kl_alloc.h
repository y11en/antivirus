#ifndef _KL_ALLOC_H_
#define _KL_ALLOC_H_

void* KlAlloc(size_t size, unsigned long tag = 'NeG-');

void* KlRealloc(void* OldMemory, size_t NewSize, unsigned long tag = 'NeG-');

void  KlFree(void*    memory);

size_t KlMemSize(void* memory);


#endif //_KL_ALLOC_H_