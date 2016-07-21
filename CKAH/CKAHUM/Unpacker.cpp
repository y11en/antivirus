#include "stdafx.h"
#include "Unpacker.h"
#include "Utils.h"
#include "../uzlib/uz.h"
#include "UnicodeConv.h"
#include "hook/kl1_api.h"

static void _GetSysInfo(LPSYSTEM_INFO lpSystemInfo)
{
	typedef VOID (WINAPI *tGetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	static tGetNativeSystemInfo g_fnGetNativeSystemInfo;
	if(!g_fnGetNativeSystemInfo)
	{
		HMODULE hKernel32 = GetModuleHandle(_T("kernel32.dll"));
		if(hKernel32)
			g_fnGetNativeSystemInfo = (tGetNativeSystemInfo)GetProcAddress(hKernel32, "GetNativeSystemInfo");
	}
	if(g_fnGetNativeSystemInfo)
		g_fnGetNativeSystemInfo(lpSystemInfo);
	else
		GetSystemInfo(lpSystemInfo);
}

bool UnpackOSFile (LPCWSTR szwPackedFilePath, LPCWSTR szwBaseFileToUnpack, LPCWSTR szwDestDirectory)
{
    bool ret = false;

	std::wstring sFile = RemoveExtension (szwBaseFileToUnpack), 
                 sDir (szwDestDirectory),
                 sSearchFile,
                 sDestFile;

	if (g_bIsNT)
	{
        extern int g_bUseKL1;

        sDestFile = sDir + sFile + (g_bUseKL1 ? L".dat" : L".sys");

		SYSTEM_INFO si;
		_GetSysInfo(&si);

		OSVERSIONINFO osvi;
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

		if (GetVersionEx (&osvi) && osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 4)
		{
			WCHAR *szwOSFileSuffix[] = { L"", L"nt", L"2k", L"xp", L"" };

			// os_index: 1 - nt, 2 - 2k, 3 - xp+
			int os_index = (osvi.dwMajorVersion == 4) ? 1 : 
					(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) ? 2 : 3;

			std::wstring sArchitectureSuffix;
			if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
				sArchitectureSuffix = L"x64";
			else if(si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
				sArchitectureSuffix = L"ia64";

			for (int i = os_index; i >= 0; --i)
			{
				sSearchFile = sFile + szwOSFileSuffix[i] + sArchitectureSuffix + L".sys";

                if (UnpackIsFile(szwPackedFilePath, sSearchFile.c_str()))
                {
                    if (UnpackFile(szwPackedFilePath, sSearchFile.c_str(), sDestFile.c_str(), !!g_bUseKL1))
                    {
                        ret = true;
                    }
                    break;
                }
			}
		}
	}
	else
	{
        sDestFile = sDir + sFile + L".vxd";
		sSearchFile = sFile + L".vxd";

        if (UnpackIsFile(szwPackedFilePath, sSearchFile.c_str()))
        {
            if (UnpackFile(szwPackedFilePath, sSearchFile.c_str(), sDestFile.c_str()))
            {
                ret = true;
            }
        }
	}		

    return ret;
}

bool UnpackFile (LPCWSTR szwPackedFilePath, LPCWSTR szwFileToUnpack, LPCWSTR szwDestFile, bool bXor)
{
    bool ret = false;

    const char *buffer = 0;
    int size = 0;

    if (!UnpackFileToMemory(szwPackedFilePath, szwFileToUnpack, buffer, size))
    {
        return false;
    }

    RemoveAttribsDeleteFileW(szwDestFile);

    HANDLE hFile = INVALID_HANDLE_VALUE;
	if (g_bIsNT)
    {
        hFile = CreateFileW(szwDestFile, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    }
    else
    {
		char szDestFile[MAX_PATH];
		if(WideCharToMultiByte(CP_ACP, 0, szwDestFile, -1, szDestFile, sizeof(szDestFile), NULL, NULL))
        hFile = CreateFileA(szDestFile, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    }

    if (bXor)
    {
        // XOR file before saving to disk
        unsigned char KL1Key[] = KL1_KEY;

        for (int i = 0; i < size; ++i)
        {
            ((char*)buffer)[i] ^= KL1Key[ i % KL1_KEY_SIZE ];
        }
    }

    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD written = 0;
        if (WriteFile(hFile, buffer, size, &written, 0) && written == size)
        {
            if (SetEndOfFile(hFile))
            {
                ret = true;
            }
        }
        else
        {
		    log.WriteFormat (_T("[UNPACK] Failed to write to '%ls' (0x%08x)"), PEL_ERROR, 
			    szwDestFile, GetLastError());
        }

        CloseHandle(hFile);
    }
    else
    {
		log.WriteFormat (_T("[UNPACK] Failed to open '%ls'(0x%08x) (0x%08x)"), PEL_ERROR, 
			szwDestFile, IOSGetFileAttributesW(szwDestFile), GetLastError());

    }
    WCHAR szwUserName[0x1000];
    DWORD dwUserNameSize = 0x1000;
    GetUserNameW(szwUserName, &dwUserNameSize);
    log.WriteFormat (_T("[UNPACK] Username: '%ls'"), PEL_INFO, szwUserName);

    UnZipFreeMemory(buffer);

    return ret;
}

bool UnpackIsFile(LPCWSTR szwPackedFilePath, LPCWSTR szwFileToUnpack)
{
	if (szwPackedFilePath == NULL || wcslen (szwPackedFilePath) == 0 )
	{
		return false;
	}

	WCHAR szwPackedFileShortPath[MAX_PATH];
	
	wcscpy (szwPackedFileShortPath, szwPackedFilePath);

	if (g_bIsNT)
	{
		GetShortPathNameW (szwPackedFilePath, szwPackedFileShortPath, sizeof (szwPackedFileShortPath) / sizeof (WCHAR));
	}

    return UnZipIsFile (__LPSTR (szwPackedFileShortPath), __LPSTR (szwFileToUnpack));
}

bool UnpackFileToMemory(LPCWSTR szwPackedFilePath, LPCWSTR szwFileToUnpack, const char *& buffer, int & size)
{
    bool ret = false;

	if (szwPackedFilePath == NULL || wcslen (szwPackedFilePath) == 0 )
	{
		return false;
	}

	WCHAR szwPackedFileShortPath[MAX_PATH];
	
	wcscpy (szwPackedFileShortPath, szwPackedFilePath);

	if (g_bIsNT)
	{
		GetShortPathNameW (szwPackedFilePath, szwPackedFileShortPath, sizeof (szwPackedFileShortPath) / sizeof (WCHAR));
	}

    buffer = 0;
    size = 0;

    int unzip_res = UnZipToMemory (__LPSTR (szwPackedFileShortPath), __LPSTR (szwFileToUnpack), buffer, size);

    if (unzip_res != UZ_OK && unzip_res != UZ_WARN)
    {
		log.WriteFormat (_T("[UNPACK] Failed to unpack '%ls'(0x%08x) (0x%08x)"), PEL_ERROR, 
			szwPackedFileShortPath, IOSGetFileAttributesW(szwPackedFileShortPath),
			unzip_res);

        return false;
    }

    return true;
}

void UnpackFreeMemory(const char *buffer)
{
    UnZipFreeMemory(buffer);
}

/*
bool UnpackFile (LPCWSTR szwSourceFilePath, LPCWSTR szwDestDirectory)
{
	if (szwSourceFilePath == NULL || wcslen (szwSourceFilePath) == 0 ||
			szwDestDirectory == NULL || wcslen (szwDestDirectory) == 0)
	{
		return false;
	}


	WCHAR szwSourceFileShortPath[MAX_PATH];
	WCHAR szwDestDirectoryShortPath [MAX_PATH];
	
	wcscpy (szwSourceFileShortPath, szwSourceFilePath);
	wcscpy (szwDestDirectoryShortPath, szwDestDirectory);

	if (g_bIsNT)
	{
		GetShortPathNameW (szwSourceFilePath, szwSourceFileShortPath, sizeof (szwSourceFileShortPath) / sizeof (WCHAR));
		GetShortPathNameW (szwDestDirectory, szwDestDirectoryShortPath, sizeof (szwDestDirectoryShortPath) / sizeof (WCHAR));
	}

	int unzip_res = UnZip (__LPSTR (szwSourceFileShortPath), __LPSTR (szwDestDirectoryShortPath));

	if (unzip_res != UZ_OK)
	{
		log.WriteFormat (_T("[UNPACK] Failed to unpack '%ls'(0x%08x) to '%ls'(0x%08x) (0x%08x)"), PEL_ERROR, 
			szwSourceFileShortPath, IOSGetFileAttributesW(szwSourceFileShortPath),
			szwDestDirectoryShortPath, IOSGetFileAttributesW(szwDestDirectoryShortPath),
			unzip_res);
	}

	return unzip_res == UZ_OK;
}
*/
std::wstring RemoveExtension (const std::wstring &string)
{
	std::wstring s (string);
	std::wstring::size_type nLastDotPos;
	if ((nLastDotPos = s.rfind (L".")) != std::wstring::npos)
		s = s.erase (nLastDotPos);
	return s;
}

std::wstring RemoveExtension (LPCWSTR string)
{
	return RemoveExtension (std::wstring (string));
}
/*
std::wstring ChooseOSFile (LPCWSTR szwBaseFileName, LPCWSTR szwDestDirectory)
{
	std::wstring sResult = L"", sFile = RemoveExtension (szwBaseFileName), sDir (szwDestDirectory);

    if (!sDir.empty () && *sDir.rbegin() != L'\\')
		sDir += L'\\';

	if (!sFile.empty ())
	{
		if (g_bIsNT)
		{
			OSVERSIONINFO osvi;

			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

			if (GetVersionEx (&osvi) && osvi.dwPlatformId >= VER_PLATFORM_WIN32_NT && osvi.dwMajorVersion >= 4)
			{
				WCHAR *szwOSFileSuffix[] = { L"", L"nt", L"2k", L"xp", L"" };

				// os_index: 1 - nt, 2 - 2k, 3 - xp+
				int os_index = (osvi.dwMajorVersion == 4) ? 1 : 
						(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0) ? 2 : 3;

				for (int i = os_index; i >= 0; --i)
				{
					std::wstring sSearchFile = sDir + sFile + szwOSFileSuffix[i] + L".sys";

					if (IOSGetFileAttributesW (sSearchFile.c_str ()) != 0xffffffff)
						return sSearchFile;
				}
			}
		}
		else
		{
			sResult = sDir + sFile + L".vxd";

			if (IOSGetFileAttributesW (sResult.c_str ()) == 0xffffffff)
				sResult = L"";
		}		
	}
	
	return sResult;
}
*/