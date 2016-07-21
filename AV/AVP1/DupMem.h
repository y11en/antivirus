// "DupMem.h"

#ifdef __cplusplus
extern "C" {
#endif

void* DupInit(BOOL multithread);
void* DupDestroy(void* pdata);
	
void* DupAlloc(unsigned size, BOOL static_, BOOL* allocated_by_new);
void  DupFree(void* ptr);

BOOL  DupEnter(BOOL* exclusive);
BOOL  DupExit(BOOL* exclusive);

void  DupRegisterThread(void* pdata);
BOOL  DupGetDebugData( DWORD* MtDebug, DWORD* count);

DWORD DupSetCustomDWord(unsigned index, DWORD data);
DWORD DupGetCustomDWord(unsigned index);

#define DupCallbackCall(_call) \
	{ BOOL recursed=-1; DupExit(&recursed); _call; DupEnter(&recursed); }

#ifdef __cplusplus
} //extern "C" 
#endif

