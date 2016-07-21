#include "file_al.h"
#include "../heap_al/heap_al.h"
#include <memory.h>

// ====================================================================================

#if defined(USE_FILE_TYPE_WIN32)
bool __stdcall win32_FileRead(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesRead)
{
	size_t nRead;
	if (!pnBytesRead)
		pnBytesRead = &nRead;
	return ReadFile((HANDLE)pFile->hHandle, pData, nSize, pnBytesRead, NULL);
}

bool __stdcall win32_FileWrite(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesWritten)
{
	size_t nWritten;
	if (!pnBytesWritten)
		pnBytesWritten = &nWritten;
	return WriteFile((HANDLE)pFile->hHandle, pData, nSize, pnBytesWritten, NULL);
}

bool __stdcall win32_FileSeek(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos)
{
	tFilePos nNewPos;
	nNewPos = SetFilePointer((HANDLE)pFile->hHandle, nPos, NULL, nSeekMethod);
	if (pnNewPos)
		*pnNewPos = nNewPos;
	return true;
}

bool __stdcall win32_FileGetSize(tFile* pFile, tFileSize* pnSize)
{
	tFileSize nSize;
	nSize = GetFileSize((HANDLE)pFile->hHandle, NULL);
	if (pnSize)
		*pnSize = nSize;
	return true;
}

bool __stdcall win32_FileSetSize(tFile* pFile, tFileSize nSize)
{
	tFilePos nPos;
	bool bResult;
	if (!win32_FileSeek(pFile, 0, SEEK_CUR, &nPos))
		return false;
	if (!win32_FileSeek(pFile, nSize, SEEK_SET, NULL))
		return false;
	bResult = SetEndOfFile((HANDLE)pFile->hHandle);
	if (!win32_FileSeek(pFile, nPos, SEEK_SET, NULL))
		return false;
	return bResult;
}

bool __stdcall win32_FileFlush(tFile* pFile)
{
	return FlushFileBuffers((HANDLE)pFile->hHandle);
}

bool __stdcall win32_FileClose(tFile* pFile)
{
	return CloseHandle((HANDLE)pFile->hHandle);
}


bool __stdcall InitFileWin32(tFile* pFile, tFileHandle hHandle)
{
	if (!pFile || !hHandle)
		return false;
	pFile->hHandle = hHandle;
	pFile->Read = win32_FileRead;
	pFile->Write = win32_FileWrite;
	pFile->Seek = win32_FileSeek;
	pFile->GetSize = win32_FileGetSize;
	pFile->SetSize = win32_FileSetSize;
	pFile->Flush = win32_FileFlush;
	pFile->Close = win32_FileClose;
	return true;
}

#endif // USE_FILE_TYPE_WIN32

// ====================================================================================

#if defined(USE_FILE_TYPE_STDIO)
#include <io.h>
bool __stdcall stdio_FileRead(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesRead)
{
	size_t nRead;
	nRead = fread(pData, 1, nSize, (FILE*)pFile->hHandle);
	if (pnBytesRead)
		*pnBytesRead = nRead;
	return (nRead != 0);
}

bool __stdcall stdio_FileWrite(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesWritten)
{
	size_t nWritten;
	nWritten = fwrite( pData, 1, nSize, (FILE*)pFile->hHandle);
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	return nWritten == nSize;
}

bool __stdcall stdio_FileSeek(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos)
{
	if (fseek((FILE*)pFile->hHandle, nPos, nSeekMethod))
		return false;
	if (pnNewPos)
		*pnNewPos = ftell((FILE*)pFile->hHandle);
	return true;
}

bool __stdcall stdio_FileGetSize(tFile* pFile, tFileSize* pnSize)
{
	tFilePos nPos;
	tFileSize nSize;
	nPos = ftell((FILE*)pFile->hHandle);
	fseek((FILE*)pFile->hHandle, 0, SEEK_END);
	nSize = ftell((FILE*)pFile->hHandle);
	fseek((FILE*)pFile->hHandle, nPos, SEEK_SET);
	if (pnSize)
		*pnSize = nSize;
	return true;
}

bool __stdcall stdio_FileSetSize(tFile* pFile, tFileSize nSize)
{
	return (0 == _chsize(fileno((FILE*)pFile->hHandle), nSize));
}

bool __stdcall stdio_FileFlush(tFile* pFile)
{
	return (!fflush((FILE*)pFile->hHandle));
}

bool __stdcall stdio_FileClose(tFile* pFile)
{
	return (!fclose((FILE*)pFile->hHandle));
}

bool __stdcall InitFileStdioN(tFile* pFile, char* fname, char* mode)
{
	FILE* hFile = fopen(fname, mode);
	return InitFileStdio(pFile, hFile);
}

bool __stdcall InitFileStdio(tFile* pFile, FILE* hHandle)
{
	if (!pFile || !hHandle)
		return false;
	pFile->hHandle = hHandle;
	pFile->Read = stdio_FileRead;
	pFile->Write = stdio_FileWrite;
	pFile->Seek = stdio_FileSeek;
	pFile->GetSize = stdio_FileGetSize;
	pFile->SetSize = stdio_FileSetSize;
	pFile->Flush = stdio_FileFlush;
	pFile->Close = stdio_FileClose;
	return true;
}

#endif // USE_FILE_TYPE_STDIO

// ====================================================================================

#if defined(USE_FILE_TYPE_MEMIO)

#define REALLOC_GRANULARITY (16*1024)

typedef struct tag_memio_data { 
	size_t pos;
	size_t size;
	size_t allocated;
	char*  ptr;
	tHeap* pHeap;
} tMEMIO;

bool __stdcall memio_FileRead(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesRead)
{
	size_t nRead = 0;
	tMEMIO* pmemio = (tMEMIO*)pFile->hHandle;
	if (!pmemio)
		return false;
	if (pmemio->size > pmemio->pos)
	{
		nRead = min(nSize, pmemio->size - pmemio->pos);
		memcpy(pData, pmemio->ptr + pmemio->pos, nRead);
		pmemio->pos += nRead;
	}
	if (pnBytesRead)
		*pnBytesRead = nRead;
	return nRead != 0;
}

bool __stdcall memio_FileWrite(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesWritten)
{
	size_t nWritten;
	tMEMIO* pmemio = (tMEMIO*)pFile->hHandle;
	if (!pmemio)
		return false;
	if (pmemio->pos + nSize > pmemio->allocated)
	{
		pmemio->allocated = pmemio->pos + nSize + REALLOC_GRANULARITY;
		if (!pmemio->pHeap->Realloc(pmemio->pHeap, pmemio->ptr, pmemio->allocated, &pmemio->ptr))
		{
			pmemio->ptr = NULL;
			pmemio->allocated = 0;
			pmemio->size = 0;
			pmemio->pos = 0;
			return false;
		}
	}
	memcpy(pmemio->ptr + pmemio->pos, pData, nSize);
	nWritten = nSize;
	pmemio->pos += nSize;
	if (pmemio->size < pmemio->pos)
		pmemio->size = pmemio->pos;
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	return nWritten == nSize;
}

bool __stdcall memio_FileSeek(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos)
{
	tMEMIO* pmemio = (tMEMIO*)pFile->hHandle;
	if (!pmemio)
		return false;
	switch (nSeekMethod)
	{
	case SEEK_CUR:
		nPos += pmemio->pos;
		break;
	case SEEK_END:
		nPos += pmemio->size;
		break;
	case SEEK_SET:
		break;
	default:
		return false;
	}
	if (nPos < 0)
		nPos = 0;
	pmemio->pos = nPos;
	if (pnNewPos)
		*pnNewPos = nPos;
	return true;
}

bool __stdcall memio_FileGetSize(tFile* pFile, tFileSize* pnSize)
{
	tMEMIO* pmemio = (tMEMIO*)pFile->hHandle;
	if (!pmemio)
		return false;
	if (pnSize)
		*pnSize = pmemio->size;
	return true;
}

bool __stdcall memio_FileSetSize(tFile* pFile, tFileSize nSize)
{
	tMEMIO* pmemio = (tMEMIO*)pFile->hHandle;
	if (!pmemio)
		return false;
	pmemio->size = nSize;
	if (pmemio->pos > nSize)
		pmemio->pos = nSize;
	memio_FileWrite(pFile, (unsigned char*)&pFile, 0, NULL); // dummy write for memory allocation check
	return true;
}

bool __stdcall memio_FileFlush(tFile* pFile)
{
	return true;
}

bool __stdcall memio_FileClose(tFile* pFile)
{
	tHeap* pHeap;
	tMEMIO* pmemio = (tMEMIO*)pFile->hHandle;
	if (!pmemio)
		return false;
	pHeap = pmemio->pHeap;
	pHeap->Free(pHeap, pmemio->ptr);
	pHeap->Free(pHeap, pmemio);
	pFile->hHandle = 0;
	return true;
}

bool __stdcall InitFileMemio(tFile* pFile, tHeap* pHeap)
{
	tMEMIO* pmemio;
	if (!pFile || !pHeap)
		return false;
	if (!pHeap->Alloc(pHeap, sizeof(tMEMIO), &pmemio))
		return false;
	pmemio->pHeap = pHeap;
	pFile->hHandle = pmemio;
	pFile->Read = memio_FileRead;
	pFile->Write = memio_FileWrite;
	pFile->Seek = memio_FileSeek;
	pFile->GetSize = memio_FileGetSize;
	pFile->SetSize = memio_FileSetSize;
	pFile->Flush = memio_FileFlush;
	pFile->Close = memio_FileClose;
	return true;
}

#endif // USE_FILE_TYPE_MEMIO

// ====================================================================================

#if defined(USE_FILE_TYPE_PRAGUE)

bool __stdcall prague_FileRead(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesRead)
{
	tERROR error;
	tFileSize nRead;
	error = CALL_IO_SeekRead((hIO)pFile->hHandle, &nRead, pFile->nPos, pData, nSize);
	pFile->nPos += nRead;
	if (pnBytesRead)
		*pnBytesRead = nRead;
	return PR_SUCC(error);
}

bool __stdcall prague_FileWrite(tFile* pFile, unsigned char* pData, size_t nSize, size_t* pnBytesWritten)
{
	tERROR error;
	tFileSize nWritten;
	error = CALL_IO_SeekWrite((hIO)pFile->hHandle, &nWritten, pFile->nPos, pData, nSize);
	pFile->nPos += nWritten;
	if (pnBytesWritten)
		*pnBytesWritten = nWritten;
	return PR_SUCC(error);
}

bool __stdcall prague_FileSeek(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos)
{
	pFile->nPos = nPos;
	if (pnNewPos)
		*pnNewPos = nPos;
	return true;
}

bool __stdcall prague_FileGetSize(tFile* pFile, tFileSize* pnSize)
{
	tERROR error;
	tQWORD qwSize;
	tFileSize nSize;
	error = CALL_IO_GetSize((hIO)pFile->hHandle, &qwSize, IO_SIZE_TYPE_EXPLICIT);
	nSize = (tDWORD)qwSize;
	if ((tQWORD)nSize != qwSize)
		nSize = (tFileSize)-1;
	if (pnSize)
		*pnSize = nSize;
	return PR_SUCC(error);
}

bool __stdcall prague_FileSetSize(tFile* pFile, tFileSize nSize)
{
	return PR_SUCC(CALL_IO_SetSize((hIO)pFile->hHandle, nSize));
}

bool __stdcall prague_FileFlush(tFile* pFile)
{
	return true;
}

bool __stdcall prague_FileCloseDummy(tFile* pFile)
{
	return true;
}

bool __stdcall prague_FileClose(tFile* pFile)
{
	hIO io = NULL;
	if (pFile)
		io = (hIO)pFile->hHandle;
	if (io)
		CALL_SYS_ObjectClose(io);
	return true;
}

bool __stdcall InitFilePrague(tFile* pFile, hIO io)
{
	if (!pFile || !io)
		return false;
	memset(pFile, 0, sizeof(tFile));
	pFile->hHandle = io;
	pFile->Read = prague_FileRead;
	pFile->Write = prague_FileWrite;
	pFile->Seek = prague_FileSeek;
	pFile->GetSize = prague_FileGetSize;
	pFile->SetSize = prague_FileSetSize;
	pFile->Flush = prague_FileFlush;
	pFile->Close = prague_FileCloseDummy;
	return true;
}

bool __stdcall InitFilePragueTemp(tFile* pFile, hOBJECT dad)
{
	if (!pFile || !dad)
		return false;
	memset(pFile, 0, sizeof(tFile));
	if (PR_FAIL(CALL_SYS_ObjectCreateQuick(dad, (hOBJECT*)&pFile->hHandle, IID_IO, PID_TMPFILE, SUBTYPE_ANY)))
		return false;
	pFile->Read = prague_FileRead;
	pFile->Write = prague_FileWrite;
	pFile->Seek = prague_FileSeek;
	pFile->GetSize = prague_FileGetSize;
	pFile->SetSize = prague_FileSetSize;
	pFile->Flush = prague_FileFlush;
	pFile->Close = prague_FileClose;
	return true;
}
#endif // USE_FILE_TYPE_PRAGUE

// ====================================================================================


bool CopyData(tFile* pFileSrc, tFile* pFileDst, size_t nSize)
{
	unsigned char buff[0x200];
	size_t bytes, read, written;
	while (nSize)
	{
		bytes = min(nSize, sizeof(buff));
		if (!pFileSrc->Read(pFileSrc, &buff[0], bytes, &read))
			return false;
		if (bytes != read)
			return false;
		if (!pFileDst->Write(pFileDst, &buff[0], bytes, &written))
			return false;
		if (bytes != written)
			return false;
		nSize -= bytes;
	}
	return true;
}

bool PutNum(tFile* pFile, unsigned long num)
{
	size_t written;
	do
	{
		unsigned char b;
		b = (unsigned char)(num & 0x7F);
		num >>= 7;
		if (num)
			b |= 0x80;
		if (!pFile->Write(pFile, &b, 1, &written))
			return false;
		if (!written)
			return false;
	} while (num);	
	return true;
}

bool PutData(tFile* pFile, unsigned char* data, size_t size)
{
	size_t written;
	if (!pFile->Write(pFile, data, size, &written))
		return false;
	if (written != size)
		return false;
	return true;
}

bool GetNum(tFile* pFile, unsigned long* pnNum)
{
	unsigned long num = 0;
	size_t read;
	unsigned char b;
	size_t shift = 0;
	do
	{
		if (!pFile->Read(pFile, &b, 1, &read))
			return false;
		if (!read)
			return false;
		num |= (b & 0x7F) << shift;
		shift += 7;
	} while (b & 0x80);	
	if (pnNum)
		*pnNum = num;
	return true;
}

bool GetData(tFile* pFile, unsigned char* data, size_t size)
{
	size_t read;
	if (!pFile->Read(pFile, data, size, &read))
		return false;
	if (read != size)
		return false;
	return true;
}


bool iCompareData(tFile* pFileSrc, tFile* pFileDst, size_t nSize)
{
	unsigned char buff[0x100];
	unsigned char buff2[0x100];
	size_t bytes, read, read2;
	while (nSize)
	{
		bytes = min(nSize, sizeof(buff));
		if (!pFileSrc->Read(pFileSrc, &buff[0], bytes, &read))
			return false;
		if (bytes != read)
			return false;
		if (!pFileDst->Read(pFileDst, &buff2[0], bytes, &read2))
			return false;
		if (bytes != read2)
			return false;
		if (memcmp(buff, buff2, bytes) != 0)
			return false;
		nSize -= bytes;
	}
	return true;
}

bool CompareData(tFile* pFileSrc, tFile* pFileDst, size_t nSize)
{
	tFilePos dwOldSrcPos, dwOldDstPos;
	bool res;
	pFileSrc->Seek(pFileSrc, 0, SEEK_CUR, &dwOldSrcPos);
	pFileDst->Seek(pFileDst, 0, SEEK_CUR, &dwOldDstPos);
	res = iCompareData(pFileSrc, pFileDst, nSize);
	pFileSrc->Seek(pFileSrc, dwOldSrcPos, SEEK_SET, NULL);
	pFileDst->Seek(pFileDst, dwOldDstPos, SEEK_SET, NULL);
	return res;
}

bool CopyTFile(tFile* pSrcFile, tFile* pDstFile)
{
	tFileSize dwSize;
	tFilePos dwOldSrcPos, dwOldDstPos;
	bool res;
	pSrcFile->GetSize(pSrcFile, &dwSize);
	pSrcFile->Seek(pSrcFile, 0, SEEK_SET, &dwOldSrcPos);
	pDstFile->Seek(pDstFile, 0, SEEK_SET, &dwOldDstPos);
	res = CopyData(pSrcFile, pDstFile, dwSize);
	pSrcFile->Seek(pSrcFile, dwOldSrcPos, SEEK_SET, NULL);
	pDstFile->Seek(pDstFile, dwOldDstPos, SEEK_SET, NULL);
	return res;
}

bool CompareTFile(tFile* pSrcFile, tFile* pDstFile)
{
	tFileSize dwSize;
	tFilePos dwOldSrcPos, dwOldDstPos;
	bool res;
	pSrcFile->GetSize(pSrcFile, &dwSize);
	pSrcFile->Seek(pSrcFile, 0, SEEK_SET, &dwOldSrcPos);
	pDstFile->Seek(pDstFile, 0, SEEK_SET, &dwOldDstPos);
	res = iCompareData(pSrcFile, pDstFile, dwSize);
	pSrcFile->Seek(pSrcFile, dwOldSrcPos, SEEK_SET, NULL);
	pDstFile->Seek(pDstFile, dwOldDstPos, SEEK_SET, NULL);
	return res;
}
