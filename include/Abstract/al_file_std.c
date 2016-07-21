#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include "al_file_std.h"

typedef struct tag_alFileStdi {
	alFileStd iface;
	long  m_RefCount;
	FILE* handle;
} alFileStdi;

bool AL_CALLTYPE alFileStd_Read(void* _this, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFileStdi* _data = (alFileStdi*)_this;
	size_t nRead;
	bool bRes;
	nRead = fread(pData, 1, nSize, _data->handle);
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

bool AL_CALLTYPE alFileStd_Write(void* _this, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFileStdi* _data = (alFileStdi*)_this;
	size_t nWritten;
	bool bRes;
	nWritten = fwrite( pData, 1, nSize, _data->handle);
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

bool AL_CALLTYPE alFileStd_Seek(void* _this, int64 nSeek, int nSeekMethod, uint64* pnNewPos)
{
	alFileStdi* _data = (alFileStdi*)_this;
	if (fseek(_data->handle, (long)nSeek, nSeekMethod))
		return false;
	if (pnNewPos)
		*pnNewPos = ftell(_data->handle);
	return true;
}

bool AL_CALLTYPE alFileStd_SeekRead(void* _this, uint64 nPos, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFileStdi* _data = (alFileStdi*)_this;
	if (!alFileStd_Seek(_this, nPos, SEEK_SET, 0))
		return false;
	return alFileStd_Read(_this, pData, nSize, pnBytesRead);
}

bool AL_CALLTYPE alFileStd_SeekWrite(void* _this, uint64 nPos, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFileStdi* _data = (alFileStdi*)_this;
	if (!alFileStd_Seek(_this, nPos, SEEK_SET, 0))
		return false;
	return alFileStd_Write(_this, pData, nSize, pnBytesWritten);
}

bool AL_CALLTYPE alFileStd_GetSize(void* _this, uint64* pnSize)
{
	alFileStdi* _data = (alFileStdi*)_this;
	uint64 nSize;
	long nPos;
	nPos = ftell(_data->handle);
	fseek(_data->handle, 0, SEEK_END);
	nSize = ftell(_data->handle);
	fseek(_data->handle, nPos, SEEK_SET);
	if (pnSize)
		*pnSize = nSize;
	return true;
}

bool AL_CALLTYPE alFileStd_SetSize(void* _this, uint64 nSize)
{
	alFileStdi* _data = (alFileStdi*)_this;
	return (0 == _chsize(_fileno(_data->handle), (long)nSize));
}


bool AL_CALLTYPE alFileStd_GetTime(void* _this, uint64* pnCreateTime, uint64* pnLastWriteTime, uint64* pnLastAccessTime)
{
	// time as Windows FILETIME
	// NOT implemented
	return false;
}

bool AL_CALLTYPE alFileStd_SetTime(void* _this, uint64 nCreateTime, uint64 nLastWriteTime, uint64 nLastAccessTime)
{
	// time as Windows FILETIME
	// NOT implemented
	return false;
}

bool AL_CALLTYPE alFileStd_Flush(void* _this)
{
	alFileStdi* _data = (alFileStdi*)_this;
	if (0 == fflush(_data->handle))
		return true;
	return false;
}

bool AL_CALLTYPE alFileStd_Close(void* _this)
{
	int ret;
	alFileStdi* _data = (alFileStdi*)_this;
	if (_data->handle == NULL)
		return false;
	ret = fclose(_data->handle);
	_data->handle = NULL;
	return (0 == ret);
}

bool AL_CALLTYPE alFileStd_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	return false;
}

long AL_CALLTYPE alFileStd_AddRef(void* _this)
{
	alFileStdi* _data = (alFileStdi*)_this;
	return ++_data->m_RefCount;
}

long AL_CALLTYPE alFileStd_Release(void* _this)
{
	alFileStdi* _data = (alFileStdi*)_this;
	long count = --_data->m_RefCount;
	if (count == 0)
	{
		alFileStd_Close(_this);
		free(_this);
	}
	return count;
}

bool AL_CALLTYPE alFileStd_fsopen(alFileStd* _this,
                                  const char *filename,
                                  const char *mode,
								  int shflag 
								  )
{
	alFileStdi* _data = (alFileStdi*)_this;
	if (_data->handle != NULL)
		alFileStd_Close(_this);
	_data->handle = _fsopen(filename, mode, shflag);
	if (_data->handle == NULL)
		return false;
	return true;
}

bool AL_CALLTYPE alFileStd_wfsopen(alFileStd* _this,
                                  const wchar_t *filename,
                                  const wchar_t *mode,
								  int shflag 
								  )
{
	alFileStdi* _data = (alFileStdi*)_this;
	if (_data->handle != NULL)
		alFileStd_Close(_this);
	_data->handle = _wfsopen(filename, mode, shflag);
	if (_data->handle == NULL)
		return false;
	return true;
}

static const alFileStd_vtbl _alFileStd_vtbl = {
	alFileStd_QueryInterface,
        alFileStd_AddRef,
        alFileStd_Release,
        alFileStd_SeekRead,
        alFileStd_SeekWrite,
        alFileStd_Read,
        alFileStd_Write,
        alFileStd_Seek,
        alFileStd_GetSize,
        alFileStd_SetSize,
		alFileStd_GetTime,
		alFileStd_SetTime,
        alFileStd_Flush,
        alFileStd_Close,
        alFileStd_fsopen,
        alFileStd_wfsopen,
};

alFileStd* AL_CALLTYPE new_alFileStd()
{
	alFileStdi* _data = malloc(sizeof(alFileStdi));
	if (!_data)
		return NULL;
	_data->iface.vtbl = &_alFileStd_vtbl;
	_data->handle = NULL;
	_data->m_RefCount = 1;
	return &_data->iface;
}

