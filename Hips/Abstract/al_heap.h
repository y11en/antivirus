#ifndef _AL_HEAP_H_4541238327298_8918341
#define _AL_HEAP_H_4541238327298_8918341

#include "al_base.h"

AL_DECLARE_INTERFACE(alHeap) AL_INTERFACE_BASED(alBase, base)
AL_DECLARE_INTERFACE_BEGIN
AL_DECLARE_METHOD1(void*, Alloc, size_t size);
AL_DECLARE_METHOD2(void*, Realloc, void* pMem, size_t size);
AL_DECLARE_METHOD1(bool,  Free, void* pMem);
AL_DECLARE_INTERFACE_END

#endif // _AL_HEAP_H_4541238327298_8918341
