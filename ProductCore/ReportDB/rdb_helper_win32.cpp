#include <Abstract/al_file_win.h>
#include "rdb_helper_win32.h"
#include <windows.h>

cReportDBHelperWin32::cReportDBHelperWin32()
{
	m_sFolder = NULL;
}

cReportDBHelperWin32::~cReportDBHelperWin32()
{
	if (m_sFolder)
		free(m_sFolder);
}

bool cReportDBHelperWin32::Init(const wchar_t* sFolder, bool bClear)
{
	if (!sFolder)
		return false;
	m_sFolder = _wcsdup(sFolder);
	if (!m_sFolder)
		return false;
	CreateDirectoryW(m_sFolder, 0);
	if (bClear)
		DeleteFile(L"*.*", 0);
	return true;
}

tDateTime cReportDBHelperWin32::GetSystemTime()
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return ((((uint64)ft.dwHighDateTime)<<32) | (uint64)ft.dwLowDateTime);
}

tDateTime cReportDBHelperWin32::GetLocalTime()
{
	FILETIME ft, lt;
	GetSystemTimeAsFileTime(&ft);
	if (!FileTimeToLocalFileTime(&ft, &lt))
		return 0;
	return ((((uint64)lt.dwHighDateTime)<<32) | (uint64)lt.dwLowDateTime);
}

tERROR cReportDBHelperWin32::GetApplicationId(tNativePid PID, tAppId* pAppId)
{
	return errNOT_IMPLEMENTED;
}

tERROR cReportDBHelperWin32::OpenFile(wchar_t* sName, uint32 nSegment, bool bCreate, bool bWriter, alFile** ppFile)
{
	if (!m_sFolder)
		return errOBJECT_NOT_INITIALIZED;
	if (!sName || !ppFile)
		return errPARAMETER_INVALID;
	tERROR error = errOK;
	wchar_t* sFileName =(wchar_t*)Alloc(sizeof(wchar_t)*(wcslen(m_sFolder)+wcslen(sName)+0x40));
	if (!sFileName)
		error = errNOT_ENOUGH_MEMORY;
	else
	{
		alFileWin32* pFile = new_alFileWin32();
		*ppFile = pFile;
		if (!pFile)
			error = errNOT_ENOUGH_MEMORY;
		else
		{
			if (nSegment)
				wsprintfW(sFileName, L"%s\\%08X_%s", m_sFolder, nSegment, sName);
			else
				wsprintfW(sFileName, L"%s\\%s", m_sFolder, sName);
			bool bCreated = pFile->CreateFileW( sFileName,
				GENERIC_READ | (bWriter ? GENERIC_WRITE : 0), 
				FILE_SHARE_READ | (bWriter ? FILE_SHARE_WRITE : 0), 
				NULL, 
				bCreate ? OPEN_ALWAYS : OPEN_EXISTING, 
				0, NULL);
			if (!bCreated)
			{
				pFile->Release();
				*ppFile = NULL;
				error = errOBJECT_CANNOT_BE_INITIALIZED;
			}
		}
		Free(sFileName);
	}
	return error;
}

tERROR cReportDBHelperWin32::DeleteFile(wchar_t* sName, uint32 nSegment)
{
	if (!m_sFolder)
		return errOBJECT_NOT_INITIALIZED;
	if (!sName)
		return errPARAMETER_INVALID;
	wchar_t* sFileName =(wchar_t*)Alloc(sizeof(wchar_t)*(wcslen(m_sFolder)+wcslen(sName)+MAX_PATH+0x10));
	if (!sFileName)
		return errNOT_ENOUGH_MEMORY;
	if (nSegment)
		wsprintfW(sFileName, L"%s\\%08X_%s", m_sFolder, nSegment, sName);
	else
		wsprintfW(sFileName, L"%s\\%s", m_sFolder, sName);
	wchar_t* pFilePart = wcsrchr(sFileName, '\\');
	if (!pFilePart)
	{
		Free(sFileName);
		return errUNEXPECTED;
	}
	pFilePart++;
	WIN32_FIND_DATAW fd;
	HANDLE hFind = ::FindFirstFileW( sFileName, &fd );
	if (hFind == INVALID_HANDLE_VALUE) // no files to delete
	{
		Free(sFileName);
		return errOK;
	}
	tERROR error = errOK;
	do
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;
		wcscpy(pFilePart, fd.cFileName);
		SetFileAttributesW(sFileName, 0);
		if (!::DeleteFileW(sFileName))
			error = errOBJECT_IS_LOCKED;
	} while(FindNextFileW(hFind, &fd));
	Free(sFileName);
	return error;
}


void* cReportDBHelperWin32::Alloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void cReportDBHelperWin32::Free(void* pmem)
{
	HeapFree(GetProcessHeap(), 0, pmem);
}

bool cReportDBHelperWin32::ConvertWC2UTF8(const wchar_t* wcString, size_t cbWideChars, char* pUtf8String, size_t cbUtf8BufferSize, size_t* pcbUtf8Size)
{
	DWORD dwRes = WideCharToMultiByte(CP_UTF8, 0, wcString, (int)cbWideChars / sizeof(wchar_t), pUtf8String, (int)cbUtf8BufferSize, 0, 0);
	if (pcbUtf8Size)
		*pcbUtf8Size = dwRes;
	if (dwRes)
		return true;
	return false;
}

bool cReportDBHelperWin32::ConvertUTF82WC(const char* pUtf8String, size_t cbUtf8Size, wchar_t* wcString, size_t cbWCBufferSize, size_t* pcbWCSize)
{
	DWORD dwRes = MultiByteToWideChar(CP_UTF8, 0, pUtf8String, (int)cbUtf8Size, wcString, (int)cbWCBufferSize);
	if (pcbWCSize)
		*pcbWCSize = dwRes*sizeof(wchar_t);
	if (dwRes)
		return true;
	return false;
}

unsigned short cReportDBHelperWin32::ConvertWC2UPR(unsigned short wc)
{
	CharUpperBuffW((LPWSTR)&wc, 1);
	return wc;
}