#include <windows.h>
#include "al_heap_win.h"

typedef struct tag_alHeapWin32i {
	alHeapWin32 iface;
	LONG   m_RefCount;
	HANDLE heap;
} alHeapWin32i;

bool AL_CALLTYPE alHeapWin32_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	return false;
}

long AL_CALLTYPE alHeapWin32_AddRef(void* _this)
{
	alHeapWin32i* _data = (alHeapWin32i*)_this;
	return InterlockedIncrement(&_data->m_RefCount);
}

long AL_CALLTYPE alHeapWin32_Release(void* _this)
{
	alHeapWin32i* _data = (alHeapWin32i*)_this;
	long count = InterlockedDecrement(&_data->m_RefCount);
	if (count == 0)
	{
		if (_data->heap)
			HeapDestroy(_data->heap);
		HeapFree(GetProcessHeap(), 0, _this);
	}
	return count;
}

void* AL_CALLTYPE alHeapWin32_Alloc(void* _this, size_t size)
{
	alHeapWin32i* _data = (alHeapWin32i*)_this;
	if (!_data->heap)
		return NULL;
	return HeapAlloc(_data->heap, HEAP_ZERO_MEMORY, size);
}

void* AL_CALLTYPE alHeapWin32_Realloc(void* _this, void* pMem, size_t size)
{
	alHeapWin32i* _data = (alHeapWin32i*)_this;
	if (!_data->heap)
		return NULL;
	if (!pMem)
		return HeapAlloc(_data->heap, HEAP_ZERO_MEMORY, size);
	return HeapReAlloc(_data->heap, HEAP_ZERO_MEMORY, pMem, size);
}

bool AL_CALLTYPE alHeapWin32_Free(void* _this, void* pMem)
{
	alHeapWin32i* _data = (alHeapWin32i*)_this;
	if (!_data->heap || !pMem)
		return false;
	return !!HeapFree(_data->heap, 0, pMem);
}

bool AL_CALLTYPE alHeapWin32_SetLowFragmentation(void* _this, bool bEnableLowFragmantation)
{
	alHeapWin32i* _data = (alHeapWin32i*)_this;
	ULONG HeapFragValue = bEnableLowFragmantation ? 2 : 0;
	if (!_data->heap)
		return false;
	return !!HeapSetInformation(_data->heap, HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
}

static const alHeapWin32_vtbl _alHeapWin32_vtbl = {
	alHeapWin32_QueryInterface,
	alHeapWin32_AddRef,
	alHeapWin32_Release,
	alHeapWin32_Alloc,
	alHeapWin32_Realloc,
	alHeapWin32_Free,
	alHeapWin32_SetLowFragmentation,
};

alHeapWin32* AL_CALLTYPE new_alHeapWin32()
{
	alHeapWin32i* _data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,  sizeof(alHeapWin32i));
	if (!_data)
		return NULL;
	_data->iface.vtbl = &_alHeapWin32_vtbl;
	_data->heap = HeapCreate(0,0,0);
	if (!_data->heap)
	{
		HeapFree(GetProcessHeap(), 0, _data);
		return NULL;
	}
	_data->m_RefCount = 1;
	return &_data->iface;
}

