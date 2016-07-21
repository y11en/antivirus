#include <windows.h>
#include "al_heap_winp.h"

typedef struct tag_alHeapWin32Pi {
	alHeapWin32P iface;
	LONG   m_RefCount;
	HANDLE heap;
} alHeapWin32Pi;

bool AL_CALLTYPE alHeapWin32P_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	return false;
}

long AL_CALLTYPE alHeapWin32P_AddRef(void* _this)
{
	alHeapWin32Pi* _data = (alHeapWin32Pi*)_this;
	return InterlockedIncrement(&_data->m_RefCount);
}

long AL_CALLTYPE alHeapWin32P_Release(void* _this)
{
	alHeapWin32Pi* _data = (alHeapWin32Pi*)_this;
	long count = InterlockedDecrement(&_data->m_RefCount);
	if (count == 0)
	{
		HeapFree(GetProcessHeap(), 0, _this);
	}
	return count;
}

void* AL_CALLTYPE alHeapWin32P_Alloc(void* _this, size_t size)
{
	alHeapWin32Pi* _data = (alHeapWin32Pi*)_this;
	if (!_data->heap)
		return NULL;
	return HeapAlloc(_data->heap, HEAP_ZERO_MEMORY, size);
}

void* AL_CALLTYPE alHeapWin32P_Realloc(void* _this, void* pMem, size_t size)
{
	alHeapWin32Pi* _data = (alHeapWin32Pi*)_this;
	if (!_data->heap)
		return NULL;
	return HeapReAlloc(_data->heap, HEAP_ZERO_MEMORY, pMem, size);
}

bool AL_CALLTYPE alHeapWin32P_Free(void* _this, void* pMem)
{
	alHeapWin32Pi* _data = (alHeapWin32Pi*)_this;
	if (!_data->heap || !pMem)
		return false;
	return !!HeapFree(_data->heap, 0, pMem);
}

bool AL_CALLTYPE alHeapWin32P_SetLowFragmentation(void* _this, bool bEnableLowFragmantation)
{
	alHeapWin32Pi* _data = (alHeapWin32Pi*)_this;
	ULONG HeapFragValue = bEnableLowFragmantation ? 2 : 0;
	if (!_data->heap)
		return false;
	return !!HeapSetInformation(_data->heap, HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
}

static const alHeapWin32P_vtbl _alHeapWin32P_vtbl = {
	alHeapWin32P_QueryInterface,
	alHeapWin32P_AddRef,
	alHeapWin32P_Release,
	alHeapWin32P_Alloc,
	alHeapWin32P_Realloc,
	alHeapWin32P_Free,
	alHeapWin32P_SetLowFragmentation,
};

alHeapWin32P* AL_CALLTYPE new_alHeapWin32P()
{
	alHeapWin32Pi* _data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,  sizeof(alHeapWin32Pi));
	if (!_data)
		return NULL;
	_data->iface.vtbl = &_alHeapWin32P_vtbl;
	_data->heap = GetProcessHeap();
	_data->m_RefCount = 1;
	return &_data->iface;
}

