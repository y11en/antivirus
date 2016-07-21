#ifndef __HEAP_ABSTRACT_H__
#define __HEAP_ABSTRACT_H__

#ifdef USE_WIN32_HEAP
#include <windows.h>
#endif // USE_WIN32_HEAP

#ifdef USE_RTL_HEAP
#include <malloc.h>
#endif // USE_RTL_HEAP

#ifndef _SIZE_T_DEFINED
#if defined (_WIN32)
typedef unsigned int size_t;
#elif defined (__unix__)
#include <sys/types.h>
#endif 
#define _SIZE_T_DEFINED
#endif // _SIZE_T_DEFINED

// define "bool" type
#ifndef __cplusplus
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif
#endif // __cplusplus

/* Define NULL pointer value */

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef __cplusplus
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#if defined (__unix__) && !defined (__stdcall)
#define __stdcall  __attribute__((stdcall))
#endif

typedef struct tag_tHeap tHeap;

struct tag_tHeap {
	void* hHeap;
	bool (__stdcall * Alloc)(tHeap* pHeap, size_t size, void** ppMem);
	bool (__stdcall * Realloc)(tHeap* pHeap, void* pMem, size_t size, void** ppMem);
	bool (__stdcall * Free)(tHeap* pHeap, void* pMem);
	void (__stdcall * Destroy)(tHeap* pHeap);
};

#ifdef USE_WIN32_HEAP
bool InitHeapWin32(tHeap* pHeap, HANDLE hHeap);
#endif

#ifdef USE_RTL_HEAP
bool InitHeapRtl(tHeap* pHeap);
#endif

#endif // __HEAP_ABSTRACT_H__
