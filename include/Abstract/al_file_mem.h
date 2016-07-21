#ifndef _AL_FILE_MEM_H_8978923487_8923
#define _AL_FILE_MEM_H_8978923487_8923

#include "al_heap.h"
#include "al_file.h"

AL_DECLARE_INTERFACE(alFileMem) AL_INTERFACE_BASED(alFile, file)
AL_DECLARE_INTERFACE_BEGIN
AL_DECLARE_INTERFACE_END

EXTERN_C alFileMem* AL_CALLTYPE new_alFileMem(alHeap* pHeap, bool bAutoReleaseHeap);

#endif // _AL_FILE_MEM_H_8978923487_8923
