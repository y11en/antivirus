#include "heap_al.h"
#include <memory.h>

typedef enum en_HeapType {
	HEAP_TYPE_WIN32 = 0,
	HEAP_TYPE_RTL   = 1,
} enHeapType;


// ------------------------------------------------------------------------

#if defined(USE_RTL_HEAP)
bool __stdcall rtl_HeapAlloc(tHeap* pHeap, size_t size, void** ppMem)
{
	void* pMem = NULL;
	if (!ppMem)
		return false;
	pMem = malloc(size);
	if (pMem)
		memset(pMem, 0, size);
	*ppMem = pMem;
	return (pMem != NULL);
}

bool __stdcall rtl_HeapRealloc(tHeap* pHeap, void* pMem, size_t size, void** ppMem)
{
	void* _pMem = NULL;
	size_t oldsize;;
	if (!ppMem)
		return false;
	if (!pMem)
		return pHeap->Alloc(pHeap, size, ppMem);
	oldsize = _msize(pMem);
	_pMem = realloc(pMem, size);
	if (_pMem && size > oldsize)
		memset((char*)_pMem+oldsize, 0, size-oldsize);
	*ppMem = _pMem;
	return (pMem != NULL);
}

bool __stdcall rtl_HeapFree(tHeap* pHeap, void* pMem)
{
	if (!pMem)
		return false;
	free(pMem);
	return true;
}

void __stdcall rtl_HeapDestroy(tHeap* pHeap)
{
	return;
}

bool InitHeapRtl(tHeap* pHeap)
{
	if (!pHeap)
		return false;
#if defined(USE_RTL_HEAP)
	pHeap->hHeap = 0;
	pHeap->Alloc = rtl_HeapAlloc;
	pHeap->Realloc = rtl_HeapRealloc;
	pHeap->Free = rtl_HeapFree;
	pHeap->Destroy = rtl_HeapDestroy;
	return true;
#else 
	__asm int 3; // USE_RTL_HEAP must be initialized before using this type
	return false;
#endif // USE_RTL_HEAP
}

#endif // defined(USE_RTL_HEAP)

// ------------------------------------------------------------------------

#if defined(USE_WIN32_HEAP)
bool __stdcall win32_HeapAlloc(tHeap* pHeap, size_t size, void** ppMem)
{
	void* pMem = NULL;
	if (!ppMem)
		return false;
	pMem = HeapAlloc((HANDLE)pHeap->hHeap, HEAP_ZERO_MEMORY, size);
	*ppMem = pMem;
	return (pMem != NULL);
}

bool __stdcall win32_HeapRealloc(tHeap* pHeap, void* pMem, size_t size, void** ppMem)
{
	void* _pMem = NULL;
	if (!ppMem)
		return false;
	if (!pMem)
		return pHeap->Alloc(pHeap, size, ppMem);
	_pMem = HeapReAlloc((HANDLE)pHeap->hHeap, HEAP_ZERO_MEMORY, pMem, size);
	*ppMem = _pMem;
	return (pMem != NULL);
}

bool __stdcall win32_HeapFree(tHeap* pHeap, void* pMem)
{
	if (!pMem)
		return false;
	HeapFree((HANDLE)pHeap->hHeap, 0, pMem);
	return true;
}

void __stdcall win32_HeapDestroy(tHeap* pHeap)
{
	if (!pHeap)
		return;
	HeapDestroy((HANDLE)pHeap->hHeap);
	memset(pHeap, 0, sizeof(tHeap));
}

bool InitHeapWin32(tHeap* pHeap, HANDLE hHeap)
{
	if (!pHeap || !hHeap)
		return false;
#if defined(USE_WIN32_HEAP)
	pHeap->hHeap = hHeap;
	pHeap->Alloc = win32_HeapAlloc;
	pHeap->Realloc = win32_HeapRealloc;
	pHeap->Free = win32_HeapFree;
	pHeap->Destroy = win32_HeapDestroy;
	return true;
#else 
	__asm int 3; // USE_WIN32_HEAP must be initialized before using this type
	return false;
#endif // USE_WIN32_HEAP
}

#endif // defined(USE_WIN32_HEAP)

// ------------------------------------------------------------------------

