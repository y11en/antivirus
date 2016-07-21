#include <prague.h>
#include <iface/i_io.h>
#include <iface/i_root.h>
#include <string.h>
#include "al_file_prague.h"

typedef struct tag_alFilePraguei {
	alFilePrague iface;
	long   m_RefCount;
	hIO    handle;
	tQWORD nPos;
	bool   bAutoCloseIo;
} alFilePraguei;

bool AL_CALLTYPE alFilePrague_Read(void* _this, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	tDWORD nRead;
	bool bRes;
	bRes = PR_SUCC(CALL_IO_SeekRead(_data->handle, &nRead, _data->nPos, (tPTR)pData, (tDWORD)nSize));
	_data->nPos += nRead;
	if (pnBytesRead)
		*pnBytesRead = nRead;
	if (bRes)
	{
		if (pnBytesRead && !nRead)
			bRes = false;
		if (!pnBytesRead && nRead != nSize)
			bRes = false;
	}
	return bRes;
}

bool AL_CALLTYPE alFilePrague_Write(void* _this, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	tDWORD nWritten;
	bool bRes = PR_SUCC(CALL_IO_SeekWrite(_data->handle, &nWritten, _data->nPos, (tPTR)pData, (tDWORD)nSize));
	_data->nPos += nWritten;
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	if (bRes)
	{
		if (pnBytesWritten && !nWritten)
			bRes = false;
		if (!pnBytesWritten && nWritten != nSize)
			bRes = false;
	}
	return bRes;
}

bool AL_CALLTYPE alFilePrague_Seek(void* _this, int64 nSeek, int nSeekMethod, uint64* pnNewPos)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	switch (nSeekMethod)
	{
	case SEEK_CUR:
		_data->nPos += nSeek;
		break;
	case SEEK_END:
		CALL_IO_GetSize(_data->handle, &_data->nPos, IO_SIZE_TYPE_EXPLICIT);
		_data->nPos += nSeek;
		break;
	case SEEK_SET:
		_data->nPos = nSeek;
		break;
	default:
		return false;
	}
	if (_data->nPos < 0)
		_data->nPos = 0;
	if (pnNewPos)
		*pnNewPos = _data->nPos;
	return true;
}

bool AL_CALLTYPE alFilePrague_SeekRead(void* _this, uint64 nPos, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	tDWORD nRead;
	bool bRes;
	bRes = PR_SUCC(CALL_IO_SeekRead(_data->handle, &nRead, nPos, pData, (tDWORD)nSize));
	if (pnBytesRead)
		*pnBytesRead = nRead;
	if (bRes)
	{
		if (pnBytesRead && !nRead)
			bRes = false;
		if (!pnBytesRead && nRead != nSize)
			bRes = false;
	}
	return bRes;
}

bool AL_CALLTYPE alFilePrague_SeekWrite(void* _this, uint64 nPos, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	tDWORD nWritten;
	bool bRes = PR_SUCC(CALL_IO_SeekWrite(_data->handle, &nWritten, _data->nPos, (tPTR)pData, (tDWORD)nSize));
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	if (bRes)
	{
		if (pnBytesWritten && !nWritten)
			bRes = false;
		if (!pnBytesWritten && nWritten != nSize)
			bRes = false;
	}
	return bRes;
}

bool AL_CALLTYPE alFilePrague_GetSize(void* _this, uint64* pnSize)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	if (PR_FAIL(CALL_IO_GetSize(_data->handle, pnSize, IO_SIZE_TYPE_EXPLICIT)))
		return false;
	return true;
}

bool AL_CALLTYPE alFilePrague_SetSize(void* _this, uint64 nSize)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	if (PR_FAIL(CALL_IO_SetSize(_data->handle, nSize)))
		return false;
	return true;
}

bool AL_CALLTYPE alFilePrague_Flush(void* _this)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	if (PR_FAIL(CALL_IO_Flush(_data->handle)))
		return false;
	return true;
}

bool AL_CALLTYPE alFilePrague_Close(void* _this)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	if (_data->handle == NULL)
		return false;
	if (_data->bAutoCloseIo)
		CALL_SYS_ObjectClose(_data->handle);
	_data->handle = NULL;
	return true;
}

bool AL_CALLTYPE alFilePrague_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	if (!szIFaceName || !ppIFace)
		return false;
	if (0 == strcmp(szIFaceName, "IO"))
	{
		*ppIFace = _data->handle;
		return true;
	}
	return false;
}

long AL_CALLTYPE alFilePrague_AddRef(void* _this)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	return ++_data->m_RefCount;
}

long AL_CALLTYPE alFilePrague_Release(void* _this)
{
	alFilePraguei* _data = (alFilePraguei*)_this;
	long count = --_data->m_RefCount;
	if (count == 0)
	{
		hIO io = _data->handle;
		bool bClose = _data->bAutoCloseIo;
		_data->handle = NULL;
		CALL_SYS_ObjHeapFree(io, _data);
		if (bClose)
			CALL_SYS_ObjectClose(io);
	}
	return count;
}

static const alFilePrague_vtbl _alFilePrague_vtbl = {
	alFilePrague_QueryInterface,
	alFilePrague_AddRef,
	alFilePrague_Release,
	alFilePrague_SeekRead,
	alFilePrague_SeekWrite,
	alFilePrague_Read,
	alFilePrague_Write,
	alFilePrague_Seek,
	alFilePrague_GetSize,
	alFilePrague_SetSize,
	alFilePrague_Flush,
	alFilePrague_Close,
};

alFilePrague* AL_CALLTYPE new_alFilePrague(hIO io, bool bAutoCloseIo)
{
	alFilePraguei* _data = NULL;
	if (PR_FAIL(CALL_SYS_ObjHeapAlloc(io, &_data, sizeof(alFilePraguei))))
		return 0;
	_data->iface.vtbl = &_alFilePrague_vtbl;
	_data->handle = io;
	_data->m_RefCount = 1;
	_data->bAutoCloseIo = bAutoCloseIo;
	return &_data->iface;
}

alFilePrague* AL_CALLTYPE new_alFilePragueTemp(hOBJECT parent)
{
	alFilePrague* pFile;
	hIO io;
	if (PR_FAIL(CALL_SYS_ObjectCreateQuick(parent, (hOBJECT*)&io, IID_IO, PID_TMPFILE, SUBTYPE_ANY)))
		return false;
	pFile = new_alFilePrague(io, true);
	if (!pFile)
		CALL_SYS_ObjectClose(io);
	return pFile;
}