#include "envhelper_win32w.h"
#include <windows.h>
#include "trace.h"

cEnvironmentHelperWin32W::cEnvironmentHelperWin32W()
{
}

cEnvironmentHelperWin32W::~cEnvironmentHelperWin32W()
{
}

void* cEnvironmentHelperWin32W::Alloc(size_t size)
{ 
	return HeapAlloc(GetProcessHeap(), 0, size); 
}

void cEnvironmentHelperWin32W::Free(void* mem)
{
	HeapFree(GetProcessHeap(), 0, mem);
}

tstring cEnvironmentHelperWin32W::ExpandEnvironmentStr(tcstring value)
{
	DWORD size = ::ExpandEnvironmentStringsW(value, NULL, 0);
	tstring expanded = tstralloc(size);
	if (NULL == expanded)
		return NULL;
	if (!ExpandEnvironmentStringsW(value, expanded, size))	
	{
		tstrfree(expanded);
		return NULL;
	}
	return expanded;
}


static size_t iGetLongPathName(tcstring lpszShortPath, tstring lpszLongPath, size_t cchBuffer)
{
	size_t retLen = 0;
	size_t copied = 0;

	tstring pwchMask = tstrdup(lpszShortPath);
	if (NULL == pwchMask)
		return 0;

	WIN32_FIND_DATA df;
	tchar wch;
	size_t cou = 0;

	tstring pwchOut = lpszLongPath;
	if (lpszLongPath)
		lpszLongPath[0] = 0;

	HANDLE hFind = INVALID_HANDLE_VALUE;
	while (true)
	{
		while (0 != pwchMask[cou])
		{
			if (('\\' == pwchMask[cou] || ':' == pwchMask[cou]) && cou > 1)
				break;
			cou++;
		}

		wch = pwchMask[cou];
		pwchMask[cou] = 0;

		tstring pwchCc = NULL;
		if (cou == 2 && pwchMask[1] == ':') // drive
			hFind = INVALID_HANDLE_VALUE;
		else
			hFind = FindFirstFile(pwchMask, &df);
		size_t lentmp;
		if (INVALID_HANDLE_VALUE == hFind)
		{
			lentmp = cou - copied;
			retLen += lentmp;
			pwchCc = pwchMask + copied;
		}
		else
		{
			retLen++;		// '\'
			lentmp = lstrlen(df.cFileName);
			retLen += lentmp;
			if (pwchOut && copied)
				*pwchOut++ = '\\';
			pwchCc = df.cFileName;
			FindClose(hFind);
		}

		if (lpszLongPath != 0)
		{
			if (cchBuffer <= retLen)
				return 0;
			else
			{
				lstrcpy(pwchOut, pwchCc);
				pwchOut += lentmp;
			}
		}

		copied = cou;


		pwchMask[cou] = wch;

		if (0 == pwchMask[cou])
			break;

		cou++;
	}
	tstrfree(pwchMask);
	
	return retLen + 1;
}

tstring cEnvironmentHelperWin32W::PathGetLong(tcstring path)
{
	bool bNotFound = false;
	DWORD len = GetLongPathNameW(path, NULL, 0);
	if (0 == len)
	{
		bNotFound = true;
		len = (DWORD)iGetLongPathName(path, NULL, 0);
		if (0 == len)
			return tstrdup(path);
	}
	len++;
	tstring long_path = tstralloc(len);
	if (!long_path)
		return tstrdup(path);
	if (bNotFound == false)
	{
		if (!GetLongPathNameW(path, long_path, len))
			return tstrdup(path);
	}
	if (!iGetLongPathName(path, long_path, len))
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

uint32_t cEnvironmentHelperWin32W::FileAttr(tcstring fullpath)
{
	return GetFileAttributesW(fullpath);
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

tHANDLE cEnvironmentHelperWin32W::FileOpen(tcstring filename)
{
	return (tHANDLE)CreateFile(filename, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 00);
}

bool cEnvironmentHelperWin32W::FileRead(tHANDLE hFile, uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read)
{
	LARGE_INTEGER liPos;
	uint32_t l_bytes_read;
	if (!bytes_read)
		bytes_read = &l_bytes_read;
	memcpy(&liPos, &pos, sizeof(LARGE_INTEGER));
	SetFilePointer((HANDLE)hFile, liPos.LowPart, &liPos.HighPart, FILE_BEGIN);
	return !!ReadFile((HANDLE)hFile, buff, buff_size, (DWORD*)bytes_read, 0);
}

uint64_t cEnvironmentHelperWin32W::FileSizeByHandle(tHANDLE hFile)
{
	LARGE_INTEGER size;
	size.LowPart = GetFileSize((HANDLE)hFile, (DWORD*)&size.HighPart);
	return (unsigned)size.QuadPart;
}

uint64_t cEnvironmentHelperWin32W::FileSizeByName(tcstring filename)
{
	uint64_t size;
	tHANDLE hFile = FileOpen(filename);
	if (hFile == INVALID_HANDLE)
		return 0;
	size = FileSizeByHandle(hFile);
	FileClose(hFile);
	return size;
}

bool cEnvironmentHelperWin32W::FileClose(tHANDLE hFile)
{
	return !!CloseHandle((HANDLE)hFile);
}

// =====================================================================================
// Registry

bool cEnvironmentHelperWin32W::Reg_OpenKey(tRegistryKey hKey, tcstring lpSubKey, tRegistryKey* phkResult)
{
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

bool cEnvironmentHelperWin32W::Reg_CloseKey(tRegistryKey hKey)
{
	return ERROR_SUCCESS == ::RegCloseKey((HKEY)hKey);
}

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
	const tcstring pathext[] = {_T(".COM"), _T(".EXE"), _T(".BAT"), _T(".CMD"), _T(".VBS"), _T(".VBE"), _T(".JS"), _T(".JSE"), _T(".WSF"), _T(".WSH")};
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

// ==== Process ===========================

bool cEnvironmentHelperWin32W::ProcessVirtualQuery(tPid pid, tAddr addr, PROCMEMINFO* pMemInfo)
{
	memset(pMemInfo, 0, sizeof(PROCMEMINFO));
	bool res = false;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, (DWORD)pid);
	if (!hProcess)
	{
		PR_TRACE((g_root, prtERROR, TR "Query memory info failed. Cannot open process WinErr %d", GetLastError()));
		return false;
	}
	SIZE_T outlen;

	MEMORY_BASIC_INFORMATION BasicMemInfo;
	memset(&BasicMemInfo, 0, sizeof(MEMORY_BASIC_INFORMATION));
	outlen = VirtualQueryEx(hProcess, (PVOID) addr, &BasicMemInfo, sizeof(MEMORY_BASIC_INFORMATION));
	if (outlen == sizeof(MEMORY_BASIC_INFORMATION))
	{
		PR_TRACE((g_root, prtIMPORTANT, TR "memory - query ok. offset %x protection %x, type=%x", 
			addr, BasicMemInfo.AllocationProtect, BasicMemInfo.Type));
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
		PR_TRACE((g_root, prtERROR, TR "Query memory info failed. WinErr %d, outlen %d", GetLastError(), outlen));
	}

	CloseHandle(hProcess);

	return res;
}

tAddr cEnvironmentHelperWin32W::ProcessGetProcAddr(tPid pid, tcstring sModuleName, const char* sProcName)
{
	// TODO: get from specified (not current) process!

	void* pProc = NULL;
	HMODULE hModule = GetModuleHandle(sModuleName);
	if (hModule)
		pProc = GetProcAddress(hModule, sProcName);
	return (tAddr)pProc;
}