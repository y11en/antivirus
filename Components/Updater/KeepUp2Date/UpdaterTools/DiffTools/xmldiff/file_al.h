#ifndef __FILE_ABSTRACT_H__
#define __FILE_ABSTRACT_H__

#ifndef _SIZE_T_DEFINED
#if defined (_WIN32)
typedef unsigned int size_t;
#elif defined (__unix__)
#include <sys/types.h>
#endif
#define _SIZE_T_DEFINED
#endif

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

typedef void* tFileHandle;
typedef long tFilePos;
typedef unsigned long tFileSize;

/* Seek method constants */
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_SET    0

#if defined (__unix__) && !defined (__stdcall)
#define __stdcall  __attribute__((stdcall))
#endif


typedef struct tag_tFile tFile;

struct tag_tFile {
	tFileHandle hHandle;
	tFilePos nPos; // some implementation has no own position
	bool (__stdcall * Read)(tFile* pFile, void* pData, size_t nSize, size_t* pnBytesRead);
	bool (__stdcall * Write)(tFile* pFile, void* pData, size_t nSize, size_t* pnBytesWritten);
	bool (__stdcall * Seek)(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos);
	bool (__stdcall * GetSize)(tFile* pFile, tFileSize* pnSize);
	bool (__stdcall * SetSize)(tFile* pFile, tFileSize nSize);
	bool (__stdcall * Flush)(tFile* pFile);
	bool (__stdcall * Close)(tFile* pFile);
};

#if defined(USE_FILE_TYPE_WIN32)
#include <windows.h>
bool __stdcall InitFileWin32(tFile* pFile, HANDLE hHandle);
#endif

#if defined(USE_FILE_TYPE_STDIO)
#include <stdio.h>
bool __stdcall InitFileStdio(tFile* pFile, FILE* hHandle);
bool __stdcall InitFileStdioN(tFile* pFile, char* fname, char* mode);
#endif

#if defined(USE_FILE_TYPE_MEMIO)
#include "heap_al.h"
bool __stdcall InitFileMemio(tFile* pFile, tHeap* pHeap);
#endif

#if defined(USE_FILE_TYPE_PRAGUE)
#include <prague.h>
#include <iface/i_io.h>
bool __stdcall InitFilePrague(tFile* pFile, hIO io);
bool __stdcall InitFilePragueTemp(tFile* pFile, hOBJECT dad);
#endif

bool CopyTFile(tFile* pSrcFile, tFile* pDstFile);
bool CompareTFile(tFile* pSrcFile, tFile* pDstFile);

bool CopyData(tFile* pFileSrc, tFile* pFileDst, size_t nSize);
bool CompareData(tFile* pFileSrc, tFile* pFileDst, size_t nSize);

bool PutNum(tFile* pFile, unsigned long num);
bool PutData(tFile* pFile, unsigned char* data, size_t size);
bool GetNum(tFile* pFile, unsigned long* pnNum);
bool GetData(tFile* pFile, unsigned char* data, size_t size);

#if defined(__cplusplus)
class cFile
{
private:
tFile m_File;
tFile* m_pFile;

public:
#ifdef USE_FILE_TYPE_PRAGUE
	cFile(hIO io)
	{
		if (InitFilePrague(&m_File, io))
			m_pFile = &m_File;
		else
			m_pFile = 0;
	}
#endif
#ifdef USE_FILE_WIN32
	CreateFile(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,

	cFile(char* fname, DWORD access, share, sec, attr)
	{
		if (InitFileWin32(&m_File, fname, access, share, sec, attr))
			m_pFile = &m_File;
		else
			m_pFile = 0;
	}
#endif
#ifdef USE_FILE_STDIO
	cFile(char* fname, char* mode)
	{
		if (InitFileStdio(&m_File, fname, mode))
			m_pFile = &m_File;
		else
			m_pFile = 0;
	}
#endif
#ifdef USE_FILE_MEMIO
	cFile(tHeap* pHeap);
	{
		if (InitFileMemio(&m_File, pHeap))
			m_pFile = &m_File;
		else
			m_pFile = 0;
	}
#endif
	cFile(tFile* pFile);
	{
		m_pFile = pFile;
	}
	bool IsInitialized()
	{
		return (m_pFile != NULL);
	}
	bool Read(void* pdata, size_t size, size_t* bytes)
	{
		if (!m_pFile)
		{
			if (bytes)
				*bytes = 0;
			return false;
		}
		return m_pFile->Read(m_pFile, pdata, size, bytes);
	}
	bool Write(void* pdata, size_t size, size_t* bytes)
	{
		if (!m_pFile)
		{
			if (bytes)
				*bytes = 0;
			return false;
		}
		return m_pFile->Write(m_pFile, pdata, size, bytes);
	}
	bool Seek(tFilePos nPos, int nSeekMethod, tFilePos* pNewPos)
	{
		if (!m_pFile)
		{
			if (pNewPos)
				*pNewPos = 0;
			return false;
		}
		return m_pFile->Seek(m_pFile, nPos, nSeelMethod, pNewPos);
	}
	bool GetSize(tFileSize* pSize)
	{
		if (!m_pFile)
		{
			if (pSize)
				*pSize = 0;
			return false;
		}
		return m_pFile->GetSize(m_pFile, pSize);
	}
	bool SetSize(tFileSize nSize)
	{
		if (!m_pFile)
			return false;
		return m_pFile->SetSize(m_pFile, nSize);
	}
	bool Flush()
	{
		if (!m_pFile)
			return false;
		return m_pFile->Flush(m_pFile);
	}
	bool Close()
	{
		bool bRes;
		if (!m_pFile)
			return true; // always ok
		bRes = m_pFile->Close(m_pFile);
		m_pFile = 0;
		return bRes;
	}
}
#endif // __cplusplus


#endif // __FILE_ABSTRACT_H__
