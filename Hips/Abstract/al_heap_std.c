#include <malloc.h>
#include <memory.h>
#include "al_heap_std.h"

typedef struct tag_alHeapStdi {
	alHeapStd iface;
	long   m_RefCount;
} alHeapStdi;

bool AL_CALLTYPE alHeapStd_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	return false;
}

long AL_CALLTYPE alHeapStd_AddRef(void* _this)
{
	alHeapStdi* _data = (alHeapStdi*)_this;
	return ++_data->m_RefCount;
}

long AL_CALLTYPE alHeapStd_Release(void* _this)
{
	alHeapStdi* _data = (alHeapStdi*)_this;
	long count = --_data->m_RefCount;
	if (count == 0)
	{
		free(_this);
	}
	return count;
}

void* AL_CALLTYPE alHeapStd_Alloc(void* _this, size_t size)
{
	void* ptr = malloc(size);
	if (ptr)
		memset(ptr, 0, size);
	return ptr;
}

void* AL_CALLTYPE alHeapStd_Realloc(void* _this, void* pMem, size_t size)
{
	size_t prev_size = 0;
	void* ptr;
	if (!pMem)
	{
		ptr = malloc(size);
	}
	else
	{
		prev_size = _msize(pMem);
		ptr = realloc(pMem, size);
	}
	if (ptr && prev_size < size)
		memset((char*)ptr + prev_size, 0, size - prev_size);
	return ptr;
}

bool AL_CALLTYPE alHeapStd_Free(void* _this, void* pMem)
{
	if (pMem)
		return false;
	free(pMem);
	return true;
}

static const alHeapStd_vtbl _alHeapStd_vtbl = {
	alHeapStd_QueryInterface,
	alHeapStd_AddRef,
	alHeapStd_Release,
	alHeapStd_Alloc,
	alHeapStd_Realloc,
	alHeapStd_Free,
};

alHeapStd* AL_CALLTYPE new_alHeapStd()
{
	alHeapStdi* _data = malloc(sizeof(alHeapStdi));
	if (!_data)
		return 0;
	_data->iface.vtbl = &_alHeapStd_vtbl;
	_data->m_RefCount = 1;
	return &_data->iface;
}

