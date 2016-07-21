// TEMP!!!!!!!!
#include <windows.h>

#include <prague.h>
#include <iface/i_io.h>
#include <Abstract/al_file_prague.h>
#include "rdb_helper_prague.h"

#define IMPEX_IMPORT_LIB
	#include <plugin\p_win32_nfio.h>

cReportDBHelperPrague::cReportDBHelperPrague()
{
}

bool cReportDBHelperPrague::Init(const wchar_t* sFolder, bool bClear)
{
	if (!sFolder)
		return false;
	m_sFolder = sFolder;
	if (m_sFolder.empty())
		return false;

	PrCreateDir(cAutoString(m_sFolder));
	if (bClear)
		DeleteFile(L"*.*", 0);
	return true;
}

tDateTime cReportDBHelperPrague::GetSystemTime()
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	return ((((uint64)ft.dwHighDateTime)<<32) | (uint64)ft.dwLowDateTime);
}

tDateTime cReportDBHelperPrague::GetLocalTime()
{
	FILETIME ft, lt;
	GetSystemTimeAsFileTime(&ft);
	if (!FileTimeToLocalFileTime(&ft, &lt))
		return 0;
	return ((((uint64)lt.dwHighDateTime)<<32) | (uint64)lt.dwLowDateTime);
}

tERROR cReportDBHelperPrague::GetApplicationId(tNativePid PID, tAppId* pAppId)
{
	return errNOT_IMPLEMENTED;
}

tERROR cReportDBHelperPrague::OpenFile(wchar_t* sName, uint32 nSegment, bool bCreate, bool bWriter, alFile** ppFile)
{
	if (m_sFolder.empty())
		return errOBJECT_NOT_INITIALIZED;
	if (!sName || !ppFile)
		return errPARAMETER_INVALID;
	tERROR error = errOK;
	wchar_t* sFileName =(wchar_t*)Alloc(sizeof(wchar_t)*(m_sFolder.size()+wcslen(sName)+0x40));
	if (!sFileName)
		error = errNOT_ENOUGH_MEMORY;
	else
	{
		if (nSegment)
			wsprintfW(sFileName, L"%s\\%08X_%s", m_sFolder.data(), nSegment, sName);
		else
			wsprintfW(sFileName, L"%s\\%s", m_sFolder.data(), sName);
		
		cIOObj pIO((hOBJECT)g_root, sFileName,
			fACCESS_READ | (bWriter ? fACCESS_WRITE : 0),
			(bCreate ? fOMODE_OPEN_ALWAYS : fOMODE_OPEN_IF_EXIST)|(bWriter ? 0 : fOMODE_SHARE_DENY_WRITE)|fOMODE_DONT_CACHE_POS);
		error = pIO.last_error();

		if( PR_SUCC(error) )
		{
			alFilePrague* pFile = new_alFilePrague(pIO, true);
			*ppFile = pFile;
			if (!pFile)
				error = errNOT_ENOUGH_MEMORY;
			else
				pIO.relinquish();
		}
		
		Free(sFileName);
	}
	
	return error;
}

tERROR cReportDBHelperPrague::DeleteFile(wchar_t* sName, uint32 nSegment)
{
	if (m_sFolder.empty())
		return errOBJECT_NOT_INITIALIZED;
	if (!sName)
		return errPARAMETER_INVALID;
	wchar_t* sFileName =(wchar_t*)Alloc(sizeof(wchar_t)*(m_sFolder.size()+wcslen(sName)+MAX_PATH+0x10));
	if (!sFileName)
		return errNOT_ENOUGH_MEMORY;
	if (nSegment)
		wsprintfW(sFileName, L"%s\\%08X_%s", m_sFolder.data(), nSegment, sName);
	else
		wsprintfW(sFileName, L"%s\\%s", m_sFolder.data(), sName);
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


void* cReportDBHelperPrague::Alloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void cReportDBHelperPrague::Free(void* pmem)
{
	HeapFree(GetProcessHeap(), 0, pmem);
}

bool cReportDBHelperPrague::ConvertWC2UTF8(const wchar_t* wcString, size_t cbWideChars, char* pUtf8String, size_t cbUtf8BufferSize, size_t* pcbUtf8Size)
{
	DWORD dwRes = WideCharToMultiByte(CP_UTF8, 0, wcString, (int)cbWideChars / sizeof(wchar_t), pUtf8String, (int)cbUtf8BufferSize, 0, 0);
	if (pcbUtf8Size)
		*pcbUtf8Size = dwRes;
	if (dwRes)
		return true;
	return false;
}

bool cReportDBHelperPrague::ConvertUTF82WC(const char* pUtf8String, size_t cbUtf8Size, wchar_t* wcString, size_t cbWCBufferSize, size_t* pcbWCSize)
{
	DWORD dwRes = MultiByteToWideChar(CP_UTF8, 0, pUtf8String, (int)cbUtf8Size, wcString, (int)cbWCBufferSize);
	if (pcbWCSize)
		*pcbWCSize = dwRes*sizeof(wchar_t);
	if (dwRes)
		return true;
	return false;
}
