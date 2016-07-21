#include "avpgimpl.h"

HANDLE	g_hHeap;
LONG	g_HeapCounter = 0;

class stcHeap
{
public:
	stcHeap()
	{
		g_hHeap = GetProcessHeap();
	}
	~stcHeap()
	{
	}
};

stcHeap gstcHeap;

void* _MM_Alloc(unsigned int nSize)
{
	void* p = HeapAlloc(g_hHeap, 0, nSize);

	InterlockedIncrement(&g_HeapCounter);

	return p;
}

void _MM_Free(void* p)
{
	if (p)
		InterlockedDecrement(&g_HeapCounter);

	HeapFree(g_hHeap, 0, p);
}

int _MM_Compare(void* p1, void* p2, int len)
{
	return memcmp((const char*) p1, (const char*) p2, len);	
}

//+ ------------------------------------------------------------------------------------------


tBOOL CheckNetworkName(hSTRING hObjectName)
{
	tBOOL bFound = cFALSE;
	
	cString* hString = (cString*) hObjectName;
	cString* hStringTmp = 0;

	tDWORD dwLen = 0;
	tPTR buffer = 0;

	tCODEPAGE cp;
	if (PR_FAIL(CALL_String_GetCP(hObjectName, &cp)))
		return cFALSE;

	if (cCP_UNICODE != cp)
		return cFALSE;
	
	hString->Length(&dwLen);
	hString->heapAlloc(&buffer, dwLen);

	if (0 == buffer)
		return cFALSE;

	try
	{
		tERROR err;
		if (PR_SUCC(CALL_SYS_ObjectCreateQuick(hObjectName, (hOBJECT*) &hStringTmp, IID_STRING, PID_ANY, 0)))
		{
			tSTR_RANGE pos = cSTRING_WHOLE;
			hStringTmp->SetCP( cp);
			if (PR_SUCC(hStringTmp->ImportFromStr(NULL, hObjectName, cSTRING_WHOLE)))
			{
				if (PR_SUCC(hStringTmp->ChangeCase(cSTRING_TO_LOWER)))
				{
					tDWORD     bsize;
					tSTR_RANGE range = cSTRING_WHOLE;
					tUINT      symbol_size = sizeof(WCHAR);

					err = hStringTmp->FindBuff(&range, cSTRING_WHOLE, L"\\\\", 2*symbol_size, cp, fSTRING_FIND_FORWARD);

					if (PR_SUCC(err))
					{
						pos = STR_RANGE( STR_RANGE_END(range), cSTRING_WHOLE_LENGTH );
						err = hStringTmp->FindBuff(&pos, pos, L"\\", symbol_size, cp, fSTRING_FIND_FORWARD);
					}
					if (PR_SUCC(err))
					{
						pos = STR_RANGE( STR_RANGE_END(pos), cSTRING_WHOLE_LENGTH );
						err = hStringTmp->FindBuff(&pos, pos, L"\\", symbol_size, cp, fSTRING_FIND_FORWARD);
					}
					if (PR_SUCC(err))
					{
						tUINT offset = STR_RANGE_POS(range) + 1;
						tUINT length = STR_RANGE_POS( pos ) - offset;
						range = STR_RANGE( offset, length );
						err = hStringTmp->ExportToBuff( &bsize, range, buffer, dwLen, cp, cSTRING_Z);
					}
					
					if (PR_SUCC(err))
					{
						pos = STR_RANGE( STR_RANGE_POS(pos), cSTRING_WHOLE_LENGTH );
						err = hStringTmp->FindBuff( &pos, pos, buffer, bsize, cp, fSTRING_FIND_FORWARD );
					}

					if (PR_SUCC(err))
					{
						err = hObjectName->Cut( pos, fSTRING_INNER );
						bFound = cTRUE;
					}
				}
			}

			CALL_SYS_ObjectClose(hStringTmp);
		}
	}
	catch(...)
	{
	}

	hString->heapFree(buffer);

	return bFound;
}

_eEventOperation GetFileOperation(DWORD HookID, DWORD FunctionMj, DWORD FunctionMi, DWORD PlatformID)
{
	_eEventOperation FileOp = _op_create;
	
	if (PlatformID == VER_PLATFORM_WIN32_NT)
	{
		if (HookID == FLTTYPE_SYSTEM)
		{
			switch (FunctionMj)
			{
			case MJ_SYSTEM_CREATE_SECTION:
				PR_TRACE((g_root, prtNOTIFY, "avpgs\texecute request"));
				FileOp = _op_execute;
				break;
			
			default:
				FileOp = _op_other;
				break;
			}
		}

		if (HookID == FLTTYPE_NFIOR)
		{
			switch (FunctionMj)
			{
			case IRP_MJ_CREATE:
				//FileOp = _op_create;
				break;
			case IRP_MJ_CLEANUP:
				FileOp = _op_close;
				break;
			case IRP_MJ_WRITE:
				FileOp = _op_write;
				break;
			case IRP_MJ_FILE_SYSTEM_CONTROL:
				{
					switch (FunctionMi)
					{
					case IRP_MN_VERIFY_VOLUME:
					case IRP_MN_MOUNT_VOLUME:
						FileOp = _op_volume_verify;
						break;
					default:
						//DbgPrint(1, "i_avpg: unknown FILE_SYSTEM_CONTROL!\n");
						break;
					}
				}
			}
		}
		if ((HookID == FLTTYPE_FIOR) && (FunctionMj == FastIoWrite))
			FileOp = _op_write;
	}
	else
	{
		if (((HookID == FLTTYPE_IFS) && (FunctionMj == IFSFN_CLOSE)) ||
			((HookID == FLTTYPE_I21) && (FunctionMj == 0x10)) ||		//CLOSE FCB
			((HookID == FLTTYPE_I21 ) && (FunctionMj == 0x3e)))			//CLOSE
			FileOp = _op_close;
		if (((HookID == FLTTYPE_IFS) && (FunctionMj == IFSFN_WRITE)) ||
			((HookID == FLTTYPE_I21) && (FunctionMj == 0x15)) ||		//SEQUENTIAL WRITE FCB
			((HookID == FLTTYPE_I21) && (FunctionMj == 0x22)) ||		//WRITE RECORD FCB
			((HookID == FLTTYPE_I21) && (FunctionMj == 0x28)) ||		////WRITE BLOCK FCB
			((HookID == FLTTYPE_I21) && (FunctionMj == 0x40)))			//WRITE
			FileOp = _op_write;
		
		if ((HookID == FLTTYPE_IFS) && (FunctionMj == IFSFN_CONNECT) && (FunctionMi == 0))
			FileOp = _op_volume_verify;
	}
	return FileOp;
}


//swap - //\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management

//%USERPROFILE%\ntuser.dat
//%USERPROFILE%\ntuser.dat.log

#include <windows.h>
#include <stuff\parray.h>

typedef CPArray <char> TListSwap;

TListSwap ListSwaps;

BOOL GetListSwaps(OSVERSIONINFO* pOsVer, TListSwap* pListSwaps);

DWORD GetListNonOpFiles(OSVERSIONINFO* pOsVer)
{
	if (GetListSwaps(pOsVer, &ListSwaps) == TRUE)
		return ListSwaps.Count();

	return 0;
}

void NonOpFilesDone()
{
	ListSwaps.RemoveAll();
}

char* GetNonOpFile(DWORD idx)
{
	return ListSwaps[idx];
}

// -----------------------------------------------------------------------------------------

void AddString(TListSwap* pListSwaps, char* pszStr)
{
	char* pstr;
	if (pszStr == NULL)
		return;
	
	pstr = new char[lstrlen(pszStr) + 1];
	pstr[0] = 0;
	lstrcpy(pstr, pszStr);

	pListSwaps->Add(pstr);
}

// -----------------------------------------------------------------------------------------

void GetWindowsSwapFile(char* path)
{
#ifndef VWIN32_DIOC_DOS_IOCTL

#define VWIN32_DIOC_DOS_IOCTL 1
#define VWIN32_DIOC_DOS_INT25 2
#define VWIN32_DIOC_DOS_INT26 3
#define VWIN32_DIOC_DOS_INT13 4

	typedef struct _DIOC_REGISTERS
	{
			DWORD   reg_EBX;
			DWORD   reg_EDX;
			DWORD   reg_ECX;
			DWORD   reg_EAX;
			DWORD   reg_EDI;
			DWORD   reg_ESI;
			DWORD   reg_Flags;
	}DIOC_REGISTERS;
#endif

	HANDLE  hDevice;
	DIOC_REGISTERS regin, regout;
	DWORD   cb;
	BOOL    fResult;

	lstrcpy(path, "");
	
	hDevice = CreateFile("\\\\.\\VWIN32",
		0,
		0,
		0,
		0,
		FILE_FLAG_DELETE_ON_CLOSE,
		0);
	
	if (INVALID_HANDLE_VALUE == hDevice) {
		//printf("Cannot open VWIN32 provider.\n");
		return;
	}
	
	regin.reg_EAX = 0x440d;           // ioctl general
	regin.reg_ECX = 0x486e;           // fat32, get swap file path
	regin.reg_EDX = (DWORD) path;     // buffer for path storing
	regin.reg_Flags = 1;              // set carry flag
	
	fResult = DeviceIoControl(hDevice,
		VWIN32_DIOC_DOS_IOCTL,
		&regin,
		sizeof(regin),
		&regout,
		sizeof(regout),
		&cb,
		0);
	
	if (!fResult || (regout.reg_Flags & 1)) {
		
		regin.reg_ECX = 0x086e; // fat16, get swap file path
		
		fResult = DeviceIoControl(hDevice,
			VWIN32_DIOC_DOS_IOCTL,
			&regin,
			sizeof(regin),
			&regout,
			sizeof(regout),
			&cb,
			0);
		
		if (!fResult || (regout.reg_Flags & 1)) {
			//printf("Error quering swap file path.\n");
			fResult = FALSE;
		}
	}
	
	if (fResult)
	{
		//printf("Swap file path: %s\n", path);
		lstrcpy(path, "");
	}
	
	CloseHandle(hDevice);
	return;
}

// -----------------------------------------------------------------------------------------
void SkipUserFolderName(PCHAR pPath)
{
		int cou = lstrlen(pPath);
		while (cou > 0)
		{
			if (pPath[cou] == '\\')
			{
				pPath[cou] = '*';
				pPath[cou + 1] = 0;
				break;
			}
			cou--;
		}
}

BOOL GetListSwaps(OSVERSIONINFO* pOsVer, TListSwap* pListSwaps)
{
	char szStr[MAX_PATH*4];
	
	HKEY hKey;	
	ULONG uKeyLenght;
	DWORD dwDisposition;
	DWORD TypeStr;

	NonOpFilesDone();

	switch (pOsVer->dwPlatformId)
	{
	case VER_PLATFORM_WIN32_WINDOWS:
		{
			char swp[MAX_PATH * 2];
			GetWindowsSwapFile(swp);
			//AddString(pListSwaps, swp);
			
			ExpandEnvironmentStrings("%windir%\\user.dat", szStr, sizeof(szStr));
			AddString(pListSwaps, szStr);
			ExpandEnvironmentStrings("%windir%\\system.dat", szStr, sizeof(szStr));
			AddString(pListSwaps, szStr);
		}
		break;
	case VER_PLATFORM_WIN32_NT:
		{
			/*ExpandEnvironmentStrings("%windir%\\security\\tmp.edb", szStr, sizeof(szStr));
			AddString(pListSwaps, szStr);*/

			ExpandEnvironmentStrings("%USERPROFILE%", szStr, sizeof(szStr));
			SkipUserFolderName(szStr);
			lstrcat(szStr, "\\ntuser.dat");
			AddString(pListSwaps, szStr);

			ExpandEnvironmentStrings("%USERPROFILE%", szStr, sizeof(szStr));
			SkipUserFolderName(szStr);
			lstrcat(szStr, "\\ntuser.dat.log");
			AddString(pListSwaps, szStr);
			
			AddString(pListSwaps, "\\\\*\\IPC$");
			AddString(pListSwaps, "*\\MAILSLOT\\NET\\NETLOGON");
			AddString(pListSwaps, "*"_DIRECT_VOLUME_ACCESS_NAME);
			AddString(pListSwaps, _PIPE_NAME "*");

			AddString(pListSwaps, "\\\\*\\PIPE\\*");

			if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Memory Management", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, &dwDisposition) == ERROR_SUCCESS)
			{
				TypeStr = REG_MULTI_SZ;
				uKeyLenght = sizeof(szStr);
				if (RegQueryValueEx(hKey, "PagingFiles", 0, &TypeStr, (LPBYTE) &szStr, &uKeyLenght) == ERROR_SUCCESS)
				{
					char szStrTmp[MAX_PATH * 4];
					char* pstr = szStr;
					while (lstrlen(pstr) != 0)
					{
						int iFound = 0;
						int idx = lstrlen(pstr) - 1;
						while ((idx > 0) && ((isdigit(pstr[idx])) || (iswspace(pstr[idx]))) && (iFound != 2))
						{
							if (iswspace(pstr[idx]))
								iFound++;
							if (iFound != 2)
								idx--;
						}
						lstrcpyn(szStrTmp, pstr, idx + 1);
						if (lstrlen(szStrTmp) > 0)
							AddString(pListSwaps, szStrTmp);
						pstr += lstrlen(pstr) + 1;
					}
				}
				
				RegCloseKey(hKey);
			}
		}
		break;
	}
	
	if (pListSwaps->Count() > 0)
		return TRUE;
	
	return FALSE;
}

BOOL __GetDosDeviceName(char* pRing0Name, char *pDosName)
{
	static BOOL bRequestedFunc = FALSE;
	
	typedef DWORD (WINAPI *_pQueryDosDevice)(LPCTSTR,  LPTSTR, DWORD);	
	static _pQueryDosDevice pQueryDosDevice = NULL;
	
	if (bRequestedFunc == FALSE)
	{
		bRequestedFunc = TRUE;
		HMODULE hKernel = GetModuleHandle("kernel32.dll");
		if (hKernel != NULL)
			pQueryDosDevice = (_pQueryDosDevice) GetProcAddress(hKernel, "QueryDosDeviceA");
	}
	
	char DrvName[MAX_PATH];
	char DosName[MAX_PATH];
	
	if (pQueryDosDevice != NULL)
	{
		for (char drv = 'A'; drv < 'Z'; drv++)
		{
			wsprintf(DrvName, "%c:", drv);
			if (pQueryDosDevice(DrvName, DosName, sizeof(DosName)))
			{
				if (lstrcmp(pRing0Name, DosName) == 0)
				{
					*pDosName = drv;
					
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

DWORD GetSubstringPosBM(BYTE* Str, DWORD sourcse_strlen, BYTE* SubStr, DWORD SubStrLen)
{
	BYTE Table[0x100];
	int i;
	BYTE *PStr,*PEndStr,*PSubStr,*PEndSubStr;
	BYTE *PStrCur;
	if ((SubStrLen==0) || (sourcse_strlen<SubStrLen)) 
		return -1;
	FillMemory(Table, sizeof(Table), SubStrLen);
	for(i = SubStrLen - 2; i >= 0; i--)
	{
		if (Table[SubStr[i]] == SubStrLen)
			Table[SubStr[i]] = (BYTE) (SubStrLen - i - 1);
	}
	PSubStr = PEndSubStr = SubStr+SubStrLen - 1;
	PStrCur = PStr=Str + SubStrLen - 1;
	PEndStr = Str + sourcse_strlen;
	do {
		if((*PStr) == (*PSubStr))
		{
			if (PSubStr == SubStr) 
				return PStr - Str;
			PStr--;
			PSubStr--;
		} 
		else 
		{
			PSubStr = PEndSubStr;
			PStr = PStrCur += Table[(*PStrCur)];
		}
	} while (PStr<PEndStr);
	return -1;
}
