#include "envhelper_win32w.h"
#include <prague/prague.h>
#include "../pdm2imp.h"

void
HandleImp::ConnectHandle (
	PWCHAR pwchFileName
	)
{
	m_pClientContext = NULL;
	m_pMessage = NULL;
	m_MessageSize = 0;

	m_HandleType  = _ht_native;
	m_hFile = CreateFile (
		pwchFileName,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		0,
		OPEN_EXISTING,
		0,
		0
		);

	PR_TRACE(( g_root, prtERROR, TR "native handle 0x%x (error 0x%x) - '%S'",
		m_hFile,
		GetLastError(),
		pwchFileName
		));
}

void
HandleImp::ConnectHandle (
		PVOID pClientContext,
		PVOID pMessage,
		ULONG MessageSize
		)
{
	m_HandleType  = _ht_drv;

	m_pClientContext = pClientContext;
	m_pMessage = pMessage;
	m_MessageSize = MessageSize;

	m_hFile = INVALID_HANDLE_VALUE;
}


HandleImp::~HandleImp()
{
	if (_ht_native == m_HandleType)
	{
		if (INVALID_HANDLE_VALUE != m_hFile)
		{
			PR_TRACE(( g_root, prtIMPORTANT, TR "close native handle 0x%x", m_hFile ));

			CloseHandle(m_hFile);
		}
	}
}


bool
HandleImp::FileRead (
	uint64_t pos,
	void* buff,
	uint32_t buff_size,
	uint32_t* bytes_read
	)
{
	if (!IsValid())
		return false;

	switch (m_HandleType)
	{
	case _ht_native:
		{
			DWORD dwRead;
			BOOL bRead = FALSE;
			LARGE_INTEGER liPos;
			memcpy( &liPos, &pos, sizeof(LARGE_INTEGER) );
			
			SetFilePointer( m_hFile, liPos.LowPart, &liPos.HighPart, FILE_BEGIN );
			bRead = ReadFile( (HANDLE)m_hFile, buff, buff_size, &dwRead, 0 );

			/*PR_TRACE(( g_root, prtERROR, TR "0x%x read %S bytes %d (error = 0x%x)",
				m_hFile,
				bRead == TRUE ? L"ok" : L"fail",
				dwRead,
				GetLastError()
				));*/

			if (bytes_read)
				*bytes_read = dwRead;

			return bRead == TRUE ? true : false;
		}
		break;

	case _ht_drv:
		{
			HRESULT hResult;

			MKLIF_OBJECT_REQUEST Request;
			
			ULONG BufSize = buff_size;
			
			memset( &Request, 0, sizeof(Request) );
			Request.m_RequestType = _object_request_read;
			memcpy( &Request.m_Offset, &pos, sizeof(Request.m_Offset) );

			hResult = MKL_ObjectRequest( 
				m_pClientContext, 
				m_pMessage,
				&Request,
				sizeof(Request),
				buff,
				&BufSize
				);
			
			if (SUCCEEDED( hResult ) && BufSize)
			{
				if (bytes_read)
					*bytes_read = BufSize;

				return true;
			}
		}
		break;
	}
	
	return false;
}


cEnvironmentHelperWin32W::cEnvironmentHelperWin32W(PVOID pOpaquePtr, w32_pfMemAlloc pfW32Alloc, w32_pfMemFree pfW32Free)
{
	m_pOpaquePtr = pOpaquePtr;
	m_pfAlloc = pfW32Alloc;
	m_pfFree = pfW32Free;
}

cEnvironmentHelperWin32W::~cEnvironmentHelperWin32W()
{
}


void* cEnvironmentHelperWin32W::Alloc( size_t size )
{
	return m_pfAlloc( m_pOpaquePtr, size, 0 );
}

void cEnvironmentHelperWin32W::Free( void* ptr )
{
	m_pfFree( m_pOpaquePtr, &ptr );
}

tstring cEnvironmentHelperWin32W::ExpandEnvironmentStr(tcstring value)
{
	DWORD size = ::ExpandEnvironmentStrings(value, NULL, 0);
	tstring expanded = tstralloc(size);
	if (NULL == expanded)
		return NULL;
	if (!ExpandEnvironmentStrings(value, expanded, size))	
	{
		tstrfree(expanded);
		return NULL;
	}
	return expanded;
}

long  cEnvironmentHelperWin32W::InterlockedInc(long* addend)
{
	return ::InterlockedIncrement(addend);
}

long  cEnvironmentHelperWin32W::InterlockedDec(long* addend)
{
	return ::InterlockedDecrement(addend);
}

long  cEnvironmentHelperWin32W::InterlockedExchAdd(long* addend, long value)
{
	return ::InterlockedExchangeAdd(addend, value);
}

uint32_t cEnvironmentHelperWin32W::FileAttr(tcstring fullpath)
{
	return GetFileAttributesW(fullpath);
}

tHANDLE cEnvironmentHelperWin32W::FileOpen(tcstring filename)
{
	PR_TRACE(( g_root, prtERROR, TR "cEnvironmentHelperWin32W::FileOpen %S", filename ));

	HandleImp* pHandle = (HandleImp*) m_pfAlloc(NULL, sizeof(HandleImp), 0);
	if (pHandle)
	{
		pHandle->HandleImp::HandleImp((PWCHAR) filename);

		if (pHandle->IsValid())
			return (tHANDLE) pHandle;

		m_pfFree(NULL, (void**) &pHandle );
	}
	
	return INVALID_HANDLE;
}

bool cEnvironmentHelperWin32W::FileRead(tHANDLE hFile, uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read)
{
	if (bytes_read)
		*bytes_read = 0;

	if (INVALID_HANDLE == hFile)
		return false;

	if (!buff || !buff_size)
	{
		PR_TRACE(( g_root, prtERROR, TR "wrong parameters, buf = 0x%xm size %d",
			buff,
			buff_size
			));

		return false;
	}

	HandleImp* pHandle = (HandleImp*) hFile;

	return pHandle->FileRead( pos, buff, buff_size, bytes_read );
}

uint64_t cEnvironmentHelperWin32W::FileSizeByHandle(tHANDLE hFile)
{
	if (INVALID_HANDLE == hFile)
		return false;

	HandleImp* pHandle = (HandleImp*) hFile;

	return pHandle->FileSize();

}

uint64_t cEnvironmentHelperWin32W::FileSizeByName(tcstring filename)
{
	uint64_t size;

	PR_TRACE(( g_root, prtERROR, TR "filesize by name %S", filename ));
	tHANDLE hFile = FileOpen(filename);

	PR_TRACE(( g_root, prtERROR, TR "filesize by name %S - open?", filename ));
	if (hFile == INVALID_HANDLE)
	{
		PR_TRACE(( g_root, prtERROR, TR "filesize by name %S cannot open", filename ));
		return 0;
	}

	size = FileSizeByHandle(hFile);
	FileClose(hFile);

	PR_TRACE(( g_root, prtERROR, TR "filesize by name %S result %d", filename, size ));

	return size;
}

bool cEnvironmentHelperWin32W::FileClose(tHANDLE hFile)
{
	PR_TRACE(( g_root, prtERROR, TR "FileClose 0x%x", hFile ));
	if (INVALID_HANDLE == hFile)
		return false;

	HandleImp* pHandle = (HandleImp*) hFile;
	if (_ht_native == pHandle->GetHandleType())
	{
		PR_TRACE(( g_root, prtERROR, TR "FileClose native" ));
		pHandle->~HandleImp();
		m_pfFree( NULL, (void**) &pHandle );
	}

	return true;
}

tstring cEnvironmentHelperWin32W::PathGetLong(tcstring path)
{
	DWORD len = GetLongPathName(path, NULL, 0);
	if (0 == len)
		return tstrdup(path);
	len++;
	tstring long_path = tstralloc(len);
	if (!long_path)
		return tstrdup(path);
	if (!GetLongPathNameW(path, long_path, len))
		return tstrdup(path);
	return long_path;
}

bool cEnvironmentHelperWin32W::PathIsNetwork(tcstring path)
{
	if (0 == tstrncmp(path, _T("\\\\"), 2))
		return true;
	if (0 == tstrncmp(path, _T("UNC\\"), 4))
		return true;
	return false;
}

/*tstring cEnvironmentHelperWin32W::PathFindExecutable(tcstring file, tcstring default_dir)
{
	tchar path[MAX_PATH];
	if (32 >= (uint32_t)::FindExecutable(file, default_dir, path))
		return NULL;

	return tstrdup(path);
}
*/
tstring cEnvironmentHelperWin32W::PathFindExecutable(tcstring file, tcstring default_dir)
{
	if (!file)
		return NULL;
	tstring pFoundPath = NULL;
	tcstring pFilePart = tstrrchr(file, '\\');
	if (pFilePart)
		default_dir = NULL; // has path
	else
		pFilePart = file;
	tcstring pExt = tstrrchr(pFilePart, '.');
	// PATHEXT=".COM;.EXE;.BAT;.CMD;.VBS;.VBE;.JS;.JSE;.WSF;.WSH"
	const tcstring pathext[] = {_T(".COM"), _T(".EXE"), _T(".BAT"), _T(".CMD"), _T(".VBS"), _T(".VBE"), _T(".JS"), _T(".JSE"), _T(".WSF"), _T(".WSH"), 0};
	
	for (size_t i=0; i<countof(pathext); i++)
	{
		DWORD cchPathLen = SearchPath(default_dir, file, pExt ? NULL : pathext[i], 0, 0, (tstring*)&pFilePart);
		if (cchPathLen)
		{
			pFoundPath = tstralloc(cchPathLen);
			if (pFoundPath)
			{
				if (!SearchPath(default_dir, file, pExt ? NULL : pathext[i], cchPathLen, pFoundPath, (tstring*)&pFilePart))
				{
					tstrfree(pFoundPath);
					pFoundPath = NULL;
				}
			}
			break;
		}
		if (pExt) // has ext
			break;
	}

	return pFoundPath;
}

// =====================================================================================
// Registry

bool cEnvironmentHelperWin32W::Reg_OpenKey(tRegistryKey hKey, tcstring lpSubKey, tRegistryKey* phkResult)
{
	if (!phkResult)
		return false;
	*phkResult = 0;
	return ERROR_SUCCESS == ::RegOpenKey((HKEY)hKey, lpSubKey, (HKEY*)phkResult);
}

bool cEnvironmentHelperWin32W::Reg_EnumValue(tRegistryKey hKey, uint32_t dwIndex, tstring* lpValueName, uint32_t* lpType)
{
	tchar sValueName[0x104];
	DWORD   nValueNameLen = countof(sValueName) - 1;
	if (ERROR_SUCCESS != ::RegEnumValue((HKEY)hKey, dwIndex, sValueName, &nValueNameLen, NULL, (LPDWORD)lpType, NULL, NULL))
		return false;
	if (lpValueName)
		*lpValueName = tstrdup(sValueName);
	return true;
}

tstring cEnvironmentHelperWin32W::Reg_QueryValueStr(tRegistryKey hKey, tcstring lpValueName)
{
	DWORD dwType;
	DWORD dwSize;
	if (ERROR_SUCCESS != ::RegQueryValueEx((HKEY)hKey, lpValueName, NULL, &dwType, NULL, &dwSize))
		return NULL;
	if (dwType != REG_SZ && dwType != REG_MULTI_SZ && dwType !=REG_EXPAND_SZ)
		return NULL;
	dwSize += sizeof(tchar) * 2;
	tstring data = tstralloc(dwSize / sizeof(tchar));
	if (!data)
		return NULL;
	if (ERROR_SUCCESS == ::RegQueryValueEx((HKEY)hKey, lpValueName, NULL, &dwType, (LPBYTE)data, &dwSize))
	{
		tstrfree(data);
		return NULL;
	}
	return data;
}

bool cEnvironmentHelperWin32W::Reg_QueryValue(tRegistryKey hKey, tcstring lpValueName, uint32_t* reg_type, void* buffer, size_t buff_size, size_t* value_size)
{
	DWORD dwType;
	DWORD dwSize = buff_size;
	LONG res = ::RegQueryValueEx((HKEY)hKey, lpValueName, NULL, &dwType, (LPBYTE)buffer, &dwSize);
	if (value_size)
		*value_size = dwSize;
	if (reg_type)
		*reg_type = dwType;
	if (ERROR_SUCCESS == res)
		return true;
	return false;
}

bool cEnvironmentHelperWin32W::Reg_CloseKey(tRegistryKey hKey)
{
	return ERROR_SUCCESS == ::RegCloseKey((HKEY)hKey);
}

// ==== Process ===========================

bool cEnvironmentHelperWin32W::ProcessVirtualQuery(tPid pid, tAddr addr, PROCMEMINFO* pMemInfo)
{
	/*memset(pMemInfo, 0, sizeof(PROCMEMINFO));
	bool res = false;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (!hProcess)
	{
		//PR_TRACE((g_root, prtERROR, TR "Query memory info failed. Cannot open process WinErr %d", GetLastError()));
		return false;
	}
	DWORD outlen;

	MEMORY_BASIC_INFORMATION BasicMemInfo;
	memset(&BasicMemInfo, 0, sizeof(MEMORY_BASIC_INFORMATION));
	outlen = VirtualQueryEx(hProcess, (PVOID) addr, &BasicMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
	if (outlen == sizeof(MEMORY_BASIC_INFORMATION))
	{
		//PR_TRACE((g_root, prtIMPORTANT, TR "memory - query ok. offset %x protection %x, type=%x", 
		//	addr, BasicMemInfo.AllocationProtect, BasicMemInfo.Type));
		pMemInfo->AllocationBase = (tAddr)BasicMemInfo.AllocationBase;
		pMemInfo->AllocationProtect = (eMemProtect)BasicMemInfo.AllocationProtect;
		pMemInfo->BaseAddress = (tAddr)BasicMemInfo.BaseAddress;
		pMemInfo->Protect = (eMemProtect)BasicMemInfo.Protect;
		pMemInfo->RegionSize = BasicMemInfo.RegionSize;
		pMemInfo->State = (eMemState)BasicMemInfo.State;
		pMemInfo->Type = (eMemType)BasicMemInfo.Type;
		res = true;
	}
	else
	{
		//PR_TRACE((g_root, prtERROR, TR "Query memory info failed. WinErr %d, outlen %d", GetLastError(), outlen));
	}

	CloseHandle(hProcess);

	return res;*/

	return false;
}

uint64_t cEnvironmentHelperWin32W::GetTimeLocal()
{
	FILETIME ft, lt;
	GetSystemTimeAsFileTime(&ft);
	FileTimeToLocalFileTime(&ft, &lt);
	return *(uint64_t*)&lt;
}

bool
cEnvironmentHelperWin32W::DriverPending (
	cEvent& event,
	uint32_t nMilliseconds
	)
{
	PEventContext pEventContext = (PEventContext) event.m_context;
	if (!pEventContext)
		return false;

	if (!pEventContext->m_pInternal || !pEventContext->m_SE.m_pMessage)
		return false;

	PR_TRACE(( g_root, prtERROR, TR "driver pending request..." ));

	MKLIF_REPLY_EVENT ReplyEvent;
	memset( &ReplyEvent, 0, sizeof(ReplyEvent) );

	ReplyEvent.m_VerdictFlags = efVerdict_Pending;
	ReplyEvent.m_ExpTime = nMilliseconds / 1000;

	HRESULT hResult = MKL_SetVerdict (
		pEventContext->m_pInternal->m_pClientContext,
		pEventContext->m_SE.m_pMessage,
		&ReplyEvent
		);

	PR_TRACE(( g_root, prtERROR, TR "driver pending result 0x%x", hResult ));

	if (SUCCEEDED( hResult ))
		return true;

	return false;
}