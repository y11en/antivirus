#include <windows.h>
#include "al_file_win.h"

typedef struct tag_alFileWin32i {
	alFileWin32 iface;
	LONG   m_RefCount;
	HANDLE handle;
} alFileWin32i;

bool AL_CALLTYPE alFileWin32_Read(void* _this, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	DWORD nRead;
	BOOL bRes = ReadFile(_data->handle, pData, (DWORD)nSize, &nRead, NULL);
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

bool AL_CALLTYPE alFileWin32_Write(void* _this, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	DWORD nWritten;
	BOOL bRes = WriteFile(_data->handle, pData, (DWORD)nSize, &nWritten, NULL);
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

bool AL_CALLTYPE alFileWin32_Seek(void* _this, int64 nSeek, int nSeekMethod, uint64* pnNewPos)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	LARGE_INTEGER li;
	li.QuadPart = nSeek;
	li.LowPart = SetFilePointer(_data->handle, li.LowPart, &li.HighPart, nSeekMethod);
	if (pnNewPos)
		*pnNewPos = li.QuadPart;
	if (li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
		return false;
	return true;
}

bool AL_CALLTYPE alFileWin32_SeekRead(void* _this, uint64 nPos, void* pData, size_t nSize, size_t* pnBytesRead)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	if (!alFileWin32_Seek(_this, nPos, SEEK_SET, 0))
		return false;
	return alFileWin32_Read(_this, pData, nSize, pnBytesRead);
}

bool AL_CALLTYPE alFileWin32_SeekWrite(void* _this, uint64 nPos, const void* pData, size_t nSize, size_t* pnBytesWritten)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	if (!alFileWin32_Seek(_this, nPos, SEEK_SET, 0))
		return false;
	return alFileWin32_Write(_this, pData, nSize, pnBytesWritten);
}

bool AL_CALLTYPE alFileWin32_GetSize(void* _this, uint64* pnSize)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	uint64 nSize;
	nSize = GetFileSize(_data->handle, NULL);
	if (pnSize)
		*pnSize = nSize;
	return true;
}

bool AL_CALLTYPE alFileWin32_SetSize(void* _this, uint64 nSize)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	uint64 nPos;
	bool bResult;
	if (!alFileWin32_Seek(_this, 0, SEEK_CUR, &nPos))
		return false;
	if (!alFileWin32_Seek(_this, nSize, SEEK_SET, NULL))
		return false;
	bResult = SetEndOfFile(_data->handle);
	if (!alFileWin32_Seek(_this, nPos, SEEK_SET, NULL))
		return false;
	return bResult;
}

bool AL_CALLTYPE alFileWin32_GetTime(void* _this, uint64* pnCreateTime, uint64* pnLastWriteTime, uint64* pnLastAccessTime)
{
	// time as Windows FILETIME
	alFileWin32i* _data = (alFileWin32i*)_this;
	return GetFileTime(_data->handle, 
		(LPFILETIME)pnCreateTime, 
		(LPFILETIME)pnLastAccessTime, 
		(LPFILETIME)pnLastWriteTime);
}

bool AL_CALLTYPE alFileWin32_SetTime(void* _this, uint64 nCreateTime, uint64 nLastWriteTime, uint64 nLastAccessTime)
{
	// time as Windows FILETIME
	alFileWin32i* _data = (alFileWin32i*)_this;
	return SetFileTime(_data->handle, 
		nCreateTime ? (LPFILETIME)&nCreateTime : NULL, 
		nLastAccessTime ? (LPFILETIME)&nLastAccessTime : NULL, 
		nLastWriteTime ? (LPFILETIME)&nLastWriteTime : NULL);
}

bool AL_CALLTYPE alFileWin32_Flush(void* _this)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	return FlushFileBuffers(_data->handle);
}

bool AL_CALLTYPE alFileWin32_Close(void* _this)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	if (_data->handle == INVALID_HANDLE_VALUE)
		return false;
	CloseHandle(_data->handle);
	_data->handle = INVALID_HANDLE_VALUE;
	return true;
}

bool AL_CALLTYPE alFileWin32_QueryInterface(void* _this, char* szIFaceName, void** ppIFace)
{
	return false;
}

long AL_CALLTYPE alFileWin32_AddRef(void* _this)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	return InterlockedIncrement(&_data->m_RefCount);
}

long AL_CALLTYPE alFileWin32_Release(void* _this)
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	long count = InterlockedDecrement(&_data->m_RefCount);
	if (count == 0)
	{
		alFileWin32_Close(_this);
		HeapFree(GetProcessHeap(), 0, _this);
	}
	return count;
}

bool AL_CALLTYPE alFileWin32_CreateFileA(alFileWin32* _this,
										  LPCSTR lpFileName, 
										  DWORD dwDesiredAccess,
										  DWORD dwShareMode,
										  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
										  DWORD dwCreationDisposition,
										  DWORD dwFlagsAndAttributes,
										  HANDLE hTemplateFile
										  )
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	if (_data->handle != INVALID_HANDLE_VALUE)
		alFileWin32_Close(_this);
	_data->handle = CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (_data->handle == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

bool AL_CALLTYPE alFileWin32_CreateFileW(alFileWin32* _this,
										  LPCWSTR lpFileName, 
										  DWORD dwDesiredAccess,
										  DWORD dwShareMode,
										  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
										  DWORD dwCreationDisposition,
										  DWORD dwFlagsAndAttributes,
										  HANDLE hTemplateFile
										  )
{
	alFileWin32i* _data = (alFileWin32i*)_this;
	if (_data->handle != INVALID_HANDLE_VALUE)
		alFileWin32_Close(_this);
	_data->handle = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (_data->handle == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

static const alFileWin32_vtbl _alFileWin32_vtbl = {
	alFileWin32_QueryInterface,
		alFileWin32_AddRef,
		alFileWin32_Release,
		alFileWin32_SeekRead,
		alFileWin32_SeekWrite,
		alFileWin32_Read,
		alFileWin32_Write,
		alFileWin32_Seek,
		alFileWin32_GetSize,
		alFileWin32_SetSize,
		alFileWin32_GetTime,
		alFileWin32_SetTime,
		alFileWin32_Flush,
		alFileWin32_Close,
		alFileWin32_CreateFileA,
		alFileWin32_CreateFileW,
};

alFileWin32* AL_CALLTYPE new_alFileWin32()
{
	alFileWin32i* _data = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,  sizeof(alFileWin32i));
	if (!_data)
		return NULL;
	_data->iface.vtbl = &_alFileWin32_vtbl;
	_data->handle = INVALID_HANDLE_VALUE;
	_data->m_RefCount = 1;
	return &_data->iface;
}

