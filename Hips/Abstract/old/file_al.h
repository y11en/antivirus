#ifndef __FILE_ABSTRACT_H__
#define __FILE_ABSTRACT_H__

#include <minmax.h>
#include <memory.h>

#ifdef __cplusplus
//extern "C" {
#endif

#ifndef _SIZE_T_DEFINED
typedef unsigned int size_t;
#define _SIZE_T_DEFINED
#endif

#ifndef EXTERN_C
#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif // __cplusplus
#endif // !EXTERN_C

// define "bool" type
#ifndef __cplusplus
#ifndef bool
#define bool int
#define true 1
#define false 0
#endif
#endif // !__cplusplus


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

typedef struct tag_tFile tFile;

typedef struct tag_tFile {
	tFileHandle hHandle;
	tFilePos nPos; // some implementation has no own position
	bool (__stdcall * Read)(tFile* pFile, void* pData, size_t nSize, size_t* pnBytesRead);
	bool (__stdcall * Write)(tFile* pFile, void* pData, size_t nSize, size_t* pnBytesWritten);
	bool (__stdcall * Seek)(tFile* pFile, tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos);
	bool (__stdcall * GetSize)(tFile* pFile, tFileSize* pnSize);
	bool (__stdcall * SetSize)(tFile* pFile, tFileSize nSize);
	bool (__stdcall * Flush)(tFile* pFile);
	bool (__stdcall * Close)(tFile* pFile);
} tFile;

#if defined(USE_FILE_TYPE_WIN32)
#include <windows.h>
EXTERN_C bool __stdcall InitFileWin32(tFile* pFile, HANDLE hHandle);
#endif

#if defined(USE_FILE_TYPE_STDIO)
#include <stdio.h>
#include <share.h>
EXTERN_C bool __stdcall InitFileStdio(tFile* pFile, FILE* hHandle);
EXTERN_C bool __stdcall InitFileStdioN(tFile* pFile, char* fname, char* mode);
EXTERN_C bool __stdcall InitFileStdioNS(tFile* pFile, char* fname, char* mode, int shflag);
#endif

#if defined(USE_FILE_TYPE_MEMIO)
#include "heap_al.h"
EXTERN_C bool __stdcall InitFileMemio(tFile* pFile, tHeap* pHeap);
#endif

#if defined(USE_FILE_TYPE_PRAGUE)
#include <prague.h>
#include <iface/i_io.h>
EXTERN_C bool __stdcall InitFilePrague(tFile* pFile, hIO io);
EXTERN_C bool __stdcall InitFilePragueTemp(tFile* pFile, hOBJECT dad);
#endif

EXTERN_C bool CopyTFile(tFile* pSrcFile, tFile* pDstFile);
EXTERN_C bool CompareTFile(tFile* pSrcFile, tFile* pDstFile);

EXTERN_C bool CopyData(tFile* pFileSrc, tFile* pFileDst, size_t nSize);
EXTERN_C bool CompareData(tFile* pFileSrc, tFile* pFileDst, size_t nSize);

EXTERN_C bool PutNum(tFile* pFile, unsigned long num);
EXTERN_C bool PutData(tFile* pFile, unsigned char* data, size_t size);
EXTERN_C bool GetNum(tFile* pFile, unsigned long* pnNum);
EXTERN_C bool GetData(tFile* pFile, unsigned char* data, size_t size);

#if defined(__cplusplus)
class cFile
{
private:
	tFile m_File;
public:
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
#ifdef USE_FILE_TYPE_WIN32
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
#ifdef USE_FILE_TYPE_STDIO
	cFile()
	{
		m_pFile = 0;
	};
	
	cFile(char* fname, char* mode)
	{
		Open(fname, mode);
	};
	
	bool Open(char* fname, char* mode)
	{
		if (InitFileStdioN(&m_File, fname, mode))
		{
			m_pFile = &m_File;
			return true;
		}
		m_pFile = 0;
		return false;
	};
	cFile(char* fname, char* mode, int shflag)
	{
		Open(fname, mode, shflag);
	};
	bool Open(char* fname, char* mode, int shflag)
	{
		if (InitFileStdioNS(&m_File, fname, mode, shflag))
		{
			m_pFile = &m_File;
			return true;
		}
		m_pFile = 0;
		return false;
	};
#endif
#ifdef USE_FILE_TYPE_MEMIO
	cFile(tHeap* pHeap)
	{
		if (InitFileMemio(&m_File, pHeap))
			m_pFile = &m_File;
		else
			m_pFile = 0;
	}
#endif
	cFile(tFile* pFile)
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
		return m_pFile->Seek(m_pFile, nPos, nSeekMethod, pNewPos);
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
};

class cCachedWrite 
{
	unsigned char out_buff[0x100];
	size_t out_size;
	cFile* pFile;
public:
	cCachedWrite(cFile* _pFile)
	{
		pFile = _pFile;
		out_size = 0;
#ifdef _DEBUG
		memset(out_buff,0,sizeof(out_buff));
#endif
	}
	~cCachedWrite()
	{
		if (pFile)
		{
			Flush();
		}
	}
	bool Write(unsigned char b)
	{
		out_buff[out_size++] = b;
		if (out_size == sizeof(out_buff))
			return Flush();
		return true;
	}
	bool Write(void* pData, size_t nSize, size_t* pnBytesWritten)
	{
		Flush();
		return pFile->Write(pData, nSize, pnBytesWritten);
	}
	bool Flush()
	{
		if (out_size)
		{
			pFile->Write(out_buff, out_size, NULL);
			out_size = 0;
#ifdef _DEBUG
			memset(out_buff,0,sizeof(out_buff));
#endif
		}
		return pFile->Flush();
	}
};

class cCachedRead
{
	unsigned char in_buff[0x100];
#ifdef _DEBUG
	unsigned char* read_ptr;
#endif
	size_t in_size;
	size_t in_ptr;
	tFilePos file_pos;
	cFile* pFile;
public:
	cCachedRead(cFile* _pFile)
	{
		pFile = _pFile;
		in_size = 0;
		in_ptr = 0;
		file_pos = 0;
#ifdef _DEBUG
		read_ptr = in_buff;
#endif
	}
	~cCachedRead()
	{
	}
	bool Read(unsigned char* pb)
	{
		if (in_ptr == in_size)
		{
			in_ptr = 0;
			pFile->Seek(0, SEEK_CUR, &file_pos);
			pFile->Read(in_buff, sizeof(in_buff), &in_size);
		}
		if (in_size == 0)
			return false;
#ifdef _DEBUG
		read_ptr = in_buff + in_ptr;
#endif
		*pb = in_buff[in_ptr++];
		return true;
	}
	bool Read(unsigned char* pdata, size_t size, size_t* bytes_read)
	{
		while (size)
		{
			size_t copy_size;
			if (in_ptr == in_size)
			{
				in_ptr = 0;
				pFile->Seek(0, SEEK_CUR, &file_pos);
				pFile->Read(in_buff, sizeof(in_buff), &in_size);
			}
			if (in_size == 0)
				return false;
#ifdef _DEBUG
			read_ptr = in_buff + in_ptr;
#endif
			copy_size = min(size, in_size);
			memcpy(pdata, in_buff+in_ptr, copy_size);
			pdata += copy_size;
			in_ptr += copy_size;
			size -= copy_size;
		}
		return true;
	}
	tFilePos GetPos()
	{
		return file_pos+in_ptr;
	}
	bool SetPos(tFilePos pos)
	{
		if (pos >= file_pos && pos <= file_pos + (tFilePos)in_size)
			in_ptr = pos - file_pos;
		else
		{
			if (!pFile->Seek(pos, SEEK_SET, 0))
				return false;
			file_pos = pos;
			in_ptr = 0;
			in_size = 0;
		}
		return true;
	}
};
#endif // __cplusplus

#ifdef __cplusplus
//} // extern "C" {
#endif

#endif // __FILE_ABSTRACT_H__