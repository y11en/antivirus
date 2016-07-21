// "DupMem.h"

#ifdef __cplusplus
extern "C" {
#endif

BOOL  DupInit(BOOL multithread);
void  DupDestroy();
	
void* DupAlloc(unsigned size, BOOL static_, BOOL* allocated_by_new);
void  DupFree(void* ptr);

BOOL  DupEnter(BOOL* exclusive);
BOOL  DupExit(BOOL* exclusive);

DWORD DupSetCustomDWord(unsigned index, DWORD data);
DWORD DupGetCustomDWord(unsigned index);

#ifdef __cplusplus
} //extern "C" 
#endif

