#include "MemManag.h"
// common ----------------------------------------------------------------------------------
_pZwQuerySystemInformation pZwQuerySystemInformation; // pointer to kernel function
_pZwQueryInformationProcess pZwQueryInformationProcess; // pointer to kernel function

_pCreateToolhelp32Snapshot pCreateToolhelp32Snapshot;
_pProcess32First pProcess32First;
_pProcess32Next pProcess32Next;
_pThread32First pThread32First;
_pThread32Next pThread32Next;
_pModule32First pModule32First;
_pModule32Next pModule32Next;

HANDLE	g_hMemManage;

void* _MM__Manage_Alloc(unsigned int nSize)
{
	return HeapAlloc(g_hMemManage, 0, nSize);
}

void _MM__Manage_Free(void* p)
{
	HeapFree(g_hMemManage, 0, p);
}

void* (* _Mem_Alloc)(unsigned int) = 0;
void  (* _Mem_Free)(void*) = 0;

#define OWN_SE_DEBUG_NAME    "SeDebugPrivilege"


// Function name	: InitWinMemManagement
// Description	    : Initialztion internal data and get DEBUG privilegies for Read & Write process memory
// Return type		: BOOL TRUE - if init was successful
// Argument         : DWORD dwPlatformID - see OsVersion structure...
BOOL InitWinMemManagement(DWORD dwPlatformID, void* (*pfMemAlloc)(unsigned int), void (*pfMemFree)(void*))
{
	BOOL bResult = FALSE;
	BOOL bErr = FALSE;

	_Mem_Alloc = pfMemAlloc;
	_Mem_Free = pfMemFree;
	
	g_hMemManage = GetProcessHeap();
	
	switch (dwPlatformID)
	{
	case VER_PLATFORM_WIN32_NT:
		{
			HMODULE hModNtDll;
			HANDLE hToken;
			LUID DebugValue;
			TOKEN_PRIVILEGES tkp;
			hModNtDll = GetModuleHandleA(NTDLL_NAME);
			if (hModNtDll != NULL)
			{
				if(!(pZwQuerySystemInformation=(_pZwQuerySystemInformation) GetProcAddress(hModNtDll, "ZwQuerySystemInformation")))
					bErr = TRUE;
				if(!(pZwQueryInformationProcess=(_pZwQueryInformationProcess) GetProcAddress(hModNtDll, "ZwQueryInformationProcess")))
					bErr = TRUE;
				if (!LookupPrivilegeValueA("", OWN_SE_DEBUG_NAME, &DebugValue))
					bErr = TRUE;
				if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
					bErr = TRUE;
				
				if (bErr == FALSE)
				{
					BOOL bNeedPriv = TRUE;
					DWORD dwLength;
#define _TOKEN_BUFFER	0x500
					char TokenBuffer[_TOKEN_BUFFER];
					TOKEN_PRIVILEGES* ptkp;
					if (GetTokenInformation(hToken, TokenPrivileges, TokenBuffer, _TOKEN_BUFFER, &dwLength) != 0)
					{
						DWORD dwPrivCount = 0;
						ptkp = (TOKEN_PRIVILEGES*) TokenBuffer;
						for (; (dwPrivCount < ptkp->PrivilegeCount - 1) && (bNeedPriv == TRUE); dwPrivCount++)
						{
							if ((ptkp->Privileges[dwPrivCount].Luid.HighPart == DebugValue.HighPart) && (ptkp->Privileges[dwPrivCount].Luid.LowPart == DebugValue.LowPart))
								bNeedPriv = FALSE;
						}
					}
					if (bNeedPriv == TRUE)
					{
						tkp.PrivilegeCount = 1;
						tkp.Privileges[0].Luid = DebugValue;
						tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
						if(AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, NULL))
							bResult = TRUE;
					}
					else
						bResult = TRUE;
					CloseHandle(hToken);
				}
			}
		}
		break;
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			HMODULE hModKrnl32;
			
			hModKrnl32 = GetModuleHandleA(_KRNL32);
			if(!(pCreateToolhelp32Snapshot = (_pCreateToolhelp32Snapshot) GetProcAddress(hModKrnl32, "CreateToolhelp32Snapshot")))
				bErr = TRUE;
			if(!(pProcess32First = (_pProcess32First) GetProcAddress(hModKrnl32,"Process32First")))
				bErr = TRUE;
			if(!(pProcess32Next = (_pProcess32Next)GetProcAddress(hModKrnl32,"Process32Next")))
				bErr = TRUE;
			if(!(pThread32First = (_pThread32First)GetProcAddress(hModKrnl32,"Thread32First")))
				bErr = TRUE;
			if(!(pThread32Next = (_pThread32Next)GetProcAddress(hModKrnl32,"Thread32Next")))
				bErr = TRUE;
			if(!(pModule32First = (_pModule32First)GetProcAddress(hModKrnl32,"Module32First")))
				bErr = TRUE;
			if(!(pModule32Next = (_pModule32Next)GetProcAddress(hModKrnl32,"Module32Next")))
				bErr = TRUE;
			
			if (bErr == FALSE)
				bResult = TRUE;
		}
		break;
	default:
		{
			OutputDebugStringA("MemManagment: Unsupported OS.\n");
		}
	}

	return bResult;
}


// Function name	: GetNTProcessInfo
// Description	    : 
// Return type		: BOOL 
// Argument         : CHAR** ppInfoBuff
BOOL GetNTProcessInfo(CHAR** ppInfoBuff)
{
	DWORD dwBuffSize;
	DWORD dwResult;
	
	dwBuffSize = 1024*32;
	do
	{
		if(!(*ppInfoBuff = (CHAR*) _Mem_Alloc(dwBuffSize)))
			break;

		dwResult = pZwQuerySystemInformation((DWORD)5, *ppInfoBuff, dwBuffSize, NULL);
		if(dwResult == 0)
			break;
		else
		{
			// STATUS_BUFFER_OVERFLOW || status == STATUS_INFO_LENGTH_MISMATCH
			if(dwResult == 0x80000005L || dwResult == 0xC0000004L)
			{
				_Mem_Free(*ppInfoBuff);
				*ppInfoBuff = NULL;
				dwBuffSize += 0x2000;
			}
			else
			{
				_Mem_Free(*ppInfoBuff);
				*ppInfoBuff = NULL;
				break;
			}
		}
	} while(TRUE);
	
	if (*ppInfoBuff == NULL)
		return FALSE;

	return TRUE;
}