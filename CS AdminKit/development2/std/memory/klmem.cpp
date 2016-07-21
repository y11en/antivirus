#ifdef _WIN32
#include "std/win32/klos_win32v50.h"
#endif

#include "std/base/klbase.h"
#include "std/thr/sync.h"

#include "std/memory/klmem.h"

#if defined(_WIN32) //&& !defined(_DEBUG)
    #define KLSTD_USE_TEMP_HEAP
#endif

namespace KLSTD
{
    volatile long g_rfVA = 0;

#if defined(_WIN32) && defined(KLSTD_USE_TEMP_HEAP)

    HANDLE g_hTmpHeap = NULL;

    void initTmpHeap()
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);        
        g_hTmpHeap = HeapCreate(0, si.dwPageSize*1, 0);
    }

    void deinitTmpHeap()
    {
        if(g_hTmpHeap)
            HeapDestroy(g_hTmpHeap);
    }
#else
    void initTmpHeap()
    {
        ;
    }

    void deinitTmpHeap()
    {
        ;
    }
#endif //_WIN32
};

using namespace KLSTD;

#if defined(_WIN32) && defined(KLSTD_USE_TEMP_HEAP)

void*   KLSTD_AllocTmp(size_t size)
{
    return HeapAlloc(g_hTmpHeap, 0, size);
}

void    KLSTD_FreeTmp(void* p)
{
    if(p)
    {
        BOOL bResult = HeapFree(g_hTmpHeap, 0, p);
        _ASSERTE(bResult);
        if(!bResult)
            __asm int 3;
    };
}

void*   KLSTD_AllocBuffer(size_t size)
{
    void* pResult = VirtualAlloc(   NULL, 
                                    size, 
                                    MEM_COMMIT|MEM_TOP_DOWN, 
                                    PAGE_READWRITE);
    if(pResult)
        InterlockedIncrement(&g_rfVA);
    return pResult;
}

void    KLSTD_FreeBuffer(void* p)
{
   if(p)
   {
        BOOL bResult = VirtualFree(p, 0, MEM_RELEASE);
        if(bResult)
            InterlockedDecrement(&g_rfVA);
        _ASSERTE(bResult);
        if(!bResult)
            __asm int 3;
   }
}

#else

void*   KLSTD_AllocTmp(size_t size)
{
    return malloc(size);
}

void    KLSTD_FreeTmp(void* p)
{
    if(p)
    {
        free(p);
    };
}

void*   KLSTD_AllocBuffer(size_t size)
{
    void* pResult = malloc(size);
    if(pResult)
        KLSTD_InterlockedIncrement(&g_rfVA);
    return pResult;
}

void    KLSTD_FreeBuffer(void* p)
{
   if(p)
   {
       free(p);
        KLSTD_InterlockedDecrement(&g_rfVA);
   }
}

#endif //defined(_WIN32) && defined(KLSTD_USE_TEMP_HEAP)
