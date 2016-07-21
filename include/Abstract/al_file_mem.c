#include <string.h>
#include <minmax.h>
#include "al_file_mem.h"

#define REALLOC_GRANULARITY (16*1024)

typedef struct tag_alFileMemi {
	alFileMem iface;
	long  m_RefCount;
	alHeap* pHeap;
	bool   bAutoReleaseHeap;
	size_t pos;
	size_t size;
	size_t allocated;
	char*  ptr;
} alFileMemi;

bool AL_CALLTYPE alFileMem_Read(void* _this, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFileMemi* _data = (alFileMemi*)_this;
	size_t nRead = 0;
	bool bRes;
	if (_data->size > _data->pos)
	{
		nRead = min(nSize, _data->size - _data->pos);
		memcpy(pData, _data->ptr + _data->pos, nRead);
		_data->pos += nRead;
	}
	if (pnBytesRead)
		*pnBytesRead = nRead;
	if (pnBytesRead && !nRead)
		bRes = false;
	else if (!pnBytesRead && nRead != nSize)
		bRes = false;
	else 
		bRes = true;
	return bRes;
}

bool AL_CALLTYPE alFileMem_Write(void* _this, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFileMemi* _data = (alFileMemi*)_this;
	size_t nWritten;
	bool bRes;
	if (_data->pos + nSize > _data->allocated)
	{
		size_t new_alloc_size = _data->pos + nSize + REALLOC_GRANULARITY;
		void* new_ptr = _data->pHeap->vtbl->Realloc(_data->pHeap, _data->ptr, new_alloc_size);
		if (!new_ptr)
		{
			if (pnBytesWritten)
				*pnBytesWritten = 0;
			return false;
		}
		_data->ptr = new_ptr;
		_data->allocated = new_alloc_size;
	}
	memcpy(_data->ptr + _data->pos, pData, nSize);
	nWritten = nSize;
	_data->pos += nSize;
	if (_data->size < _data->pos)
		_data->size = _data->pos;
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	if (pnBytesWritten && !nWritten)
		bRes = false;
	else if (!pnBytesWritten && nWritten != nSize)
		bRes = false;
	else
		bRes = true;
	return bRes;
}

bool AL_CALLTYPE alFileMem_Seek(void* _this, int64 nSeek, int nSeekMethod, uint64* pnNewPos)
{
	alFileMemi* _data = (alFileMemi*)_this;
	switch (nSeekMethod)
	{
	case SEEK_CUR:
		nSeek += _data->pos;
		break;
	case SEEK_END:
		nSeek += _data->size;
		break;
	case SEEK_SET:
		break;
	default:
		return false;
	}
	if (nSeek < 0)
		nSeek = 0;
	_data->pos = (size_t)nSeek;
	if (pnNewPos)
		*pnNewPos = nSeek;
	return true;
}

bool AL_CALLTYPE alFileMem_SeekRead(void* _this, uint64 nPos, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFileMemi* _data = (alFileMemi*)_this;
	size_t nRead = 0;
	bool bRes;
	if (_data->size > nPos)
	{
		nRead = min(nSize, _data->size - (size_t)nPos);
		memcpy(pData, _data->ptr + nPos, nRead);
	}
	if (pnBytesRead)
		*pnBytesRead = nRead;
	if (pnBytesRead && !nRead)
		bRes = false;
	else if (!pnBytesRead && nRead != nSize)
		bRes = false;
	else 
		bRes = true;
	return bRes;
}

bool AL_CALLTYPE alFileMem_SeekWrite(void* _this, uint64 nPos, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFileMemi* _data = (alFileMemi*)_this;
	size_t nWritten;
	bool bRes;
	if (nPos + nSize > _data->allocated)
	{
		size_t new_alloc_size = (size_t)nPos + nSize + REALLOC_GRANULARITY;
		void* new_ptr = _data->pHeap->vtbl->Realloc(_data->pHeap, _data->ptr, new_alloc_size);
		if (!new_ptr)
		{
			if (pnBytesWritten)
				*pnBytesWritten = 0;
			return false;
		}
		_data->ptr = new_ptr;
		_data->allocated = new_alloc_size;
	}
	memcpy(_data->ptr + nPos, pData, nSize);
	nWritten = nSize;
	if (_data->size < (size_t)nPos)
		_data->size = (size_t)nPos;
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	if (pnBytesWritten && !nWritten)
		bRes = false;
	else if (!pnBytesWritten && nWritten != nSize)
		bRes = false;
	else
		bRes = true;
	return bRes;
}

bool AL_CALLTYPE alFileMem_GetSize(void* _this, uint64* pnSize)
{
	alFileMemi* _data = (alFileMemi*)_this;
	if (!pnSize)
		return false;
	*pnSize = _data->size;
	return true;
}

bool AL_CALLTYPE alFileMem_SetSize(void* _this, uint64 nSize)
{
	alFileMemi* _data = (alFileMemi*)_this;
	void* new_ptr = _data->pHeap->vtbl->Realloc(_data->pHeap, _data->ptr, (size_t)nSize);
	if (!new_ptr)
		return false;
	_data->ptr = new_ptr;
	_data->allocated = _data->size = (size_t)nSize;
	if (_data->pos > (size_t)nSize)
		_data->pos = (size_t)nSize;
	return true;
}


bool AL_CALLTYPE alFileMem_GetTime(void* _this, uint64* pnCreateTime, uint64* pnLastWriteTime, uint64* pnLastAccessTime)
{
	// time as Windows FILETIME
	// NOT implemented
	return false;
}

bool AL_CALLTYPE alFileMem_SetTime(void* _this, uint64 nCreateTime, uint64 nLastWriteTime, uint64 nLastAccessTime)
{
	// time as Windows FILETIME
	// NOT implemented
	return false;
}

bool AL_CALLTYPE alFileMem_Flush(void* _this)
{
	return true;
}

bool AL_CALLTYPE alFileMem_Close(void* _this)
{
	return alFileMem_SetSize(_this, 0);
}

bool AL_CALLTYPE alFileMem_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	alFileMemi* _data = (alFileMemi*)_this;
	if (!szIFaceName || !ppIFace)
		return false;
	if (0 == strcmp(szIFaceName, "alHeap"))
	{
		_data->pHeap->vtbl->base.AddRef(_data->pHeap);
		*ppIFace = _data->pHeap;
		return true;
	}
	return false;
}

long AL_CALLTYPE alFileMem_AddRef(void* _this)
{
	alFileMemi* _data = (alFileMemi*)_this;
	return ++_data->m_RefCount;
}

long AL_CALLTYPE alFileMem_Release(void* _this)
{
	alFileMemi* _data = (alFileMemi*)_this;
	long count = --_data->m_RefCount;
	if (count == 0)
	{
		alHeap* pHeap = _data->pHeap;
		bool bReleaseHeap = _data->bAutoReleaseHeap;
		alFileMem_Close(_this);
		pHeap->vtbl->Free(pHeap, _this);
		if (bReleaseHeap)
			pHeap->vtbl->base.Release(pHeap);
	}
	return count;
}

static const alFileMem_vtbl _alFileMem_vtbl = {
	alFileMem_QueryInterface,
	alFileMem_AddRef,
	alFileMem_Release,
	alFileMem_SeekRead,
	alFileMem_SeekWrite,
	alFileMem_Read,
	alFileMem_Write,
	alFileMem_Seek,
	alFileMem_GetSize,
	alFileMem_SetSize,
	alFileMem_GetTime,
	alFileMem_SetTime,
	alFileMem_Flush,
	alFileMem_Close,
};

alFileMem* AL_CALLTYPE new_alFileMem(alHeap* pHeap, bool bAutoReleaseHeap)
{
	alFileMemi* _data = pHeap->vtbl->Alloc(pHeap, sizeof(alFileMemi));
	if (!_data)
		return 0;
	_data->iface.vtbl = &_alFileMem_vtbl;
	_data->pHeap = pHeap;
	_data->bAutoReleaseHeap = bAutoReleaseHeap;
	_data->m_RefCount = 1;
	return &_data->iface;
}

