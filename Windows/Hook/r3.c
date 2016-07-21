//Полную хистори файла ищи в каталоге ".\w9x"
#include "r3.h"
#include "kldefs.h"
#include "Hook/HookSpec.h"

#include "debug.h"

#ifdef _HOOK_NT_
#include "nt/glbenvir.h"
#endif

PERESOURCE	g_pR3Mutex = NULL;
void EnableWriteToSections(BOOL bEnable);

typedef struct _R3HOOKDLL {
	PCHAR DllName;
	PVOID DllBase;
	BOOLEAN IsNeedHook;
}R3HOOKDLL;

typedef struct _R3HOOK {
	PVOID Real;
	PVOID Hook;
	PCHAR FuncName;
} R3HOOK;

BOOLEAN R3K32ApiInited=FALSE;
BOOLEAN R3Hooked=FALSE;

//------------- R3 Part  ----- ATTENTION !!! This code runs in Ring3 mode --------------------------
#pragma intrinsic(memcpy,strcpy,strlen,strcat)
#ifdef _HOOK_NT_
#pragma code_seg(".userc")
#pragma data_seg(".userd")
#pragma const_seg(".userd")
#pragma bss_seg(".userd")

/*Linker additional options
 /SECTION:.user,ERW,ALIGN=4096 /MERGE:.userd=.user /MERGE:.userc=.user
*/

#define _STACKCHECK

#endif // _HOOK_NT_



//+ ------------------------------------------------------------------------------------------


BOOL		__stdcall HCreateProcessA(LPCTSTR lpApplicationName,LPTSTR lpCommandLine,PVOID lpProcessAttributes,PVOID lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,PVOID lpEnvironment,LPCTSTR lpCurrentDirectory,PVOID lpStartupInfo,PVOID lpProcessInformation);
BOOL		__stdcall HCreateProcessW(PWCHAR lpApplicationName,PWCHAR lpCommandLine,PVOID lpProcessAttributes,PVOID lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,PVOID lpEnvironment,PWCHAR lpCurrentDirectory,PVOID lpStartupInfo,PVOID lpProcessInformation);
DWORD		__stdcall HWinExec(LPCSTR lpCmdLine,DWORD uCmdShow);
DWORD		__stdcall HLoadModule(LPCSTR lpModuleName,PVOID lpParameterBlock);
HANDLE	__stdcall HLoadLibraryA(PCHAR lpFileName);
HANDLE	__stdcall HLoadLibraryW(PWCHAR lpFileName);
HANDLE	__stdcall HLoadLibraryExA(PCHAR lpFileName,HANDLE hFile,DWORD dwFlags);
HANDLE	__stdcall HLoadLibraryExW(PWCHAR lpFileName,HANDLE hFile,DWORD dwFlags);
HANDLE	__stdcall HCreateFileA(PCHAR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,PVOID lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
HANDLE	__stdcall HCreateFileW(PWCHAR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,PVOID lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
//DWORD		__stdcall HNtCreateFile(OUT PHANDLE  FileHandle,IN ACCESS_MASK  DesiredAccess,IN POBJECT_ATTRIBUTES  ObjectAttributes,OUT PIO_STATUS_BLOCK  IoStatusBlock,IN PLARGE_INTEGER  AllocationSize  OPTIONAL,IN ULONG  FileAttributes,IN ULONG  ShareAccess,IN ULONG  CreateDisposition,IN ULONG  CreateOptions,IN PVOID  EaBuffer  OPTIONAL,IN ULONG  EaLength);
HANDLE	__stdcall HOpenFile(LPCSTR lpFileName,PVOID lpReOpenBuff,DWORD uStyle);
HANDLE	__stdcall H_lopen(LPCSTR lpPathName,DWORD iReadWrite);
HANDLE	__stdcall H_lcreat(LPCSTR lpPathName,DWORD iAttribute);
BOOL		__stdcall HCopyFileA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists);
BOOL		__stdcall HCopyFileW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName,BOOL bFailIfExists);
BOOL		__stdcall HCopyFileExA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,PVOID lpProgressRoutine,PVOID lpData,PBOOL pbCancel,DWORD dwCopyFlags);
BOOL		__stdcall HCopyFileExW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName,PVOID lpProgressRoutine,PVOID lpData,PBOOL pbCancel,DWORD dwCopyFlags);
BOOL		__stdcall HMoveFileA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName);
BOOL		__stdcall HMoveFileW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName);
BOOL		__stdcall HMoveFileExA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,DWORD dwFlags);
BOOL		__stdcall HMoveFileExW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName,DWORD dwFlags);
BOOL		__stdcall HDeleteFileA(LPCTSTR lpFileName);
BOOL		__stdcall HDeleteFileW(PWCHAR lpFileName);
BOOL		__stdcall HSetFileAttributesA(LPCTSTR lpFileName,DWORD dwFileAttributes);
BOOL		__stdcall HSetFileAttributesW(PWCHAR lpFileName,DWORD dwFileAttributes);
DWORD		__stdcall HGetPrivateProfileIntA(LPCTSTR lpAppName,LPCTSTR lpKeyName,INT nDefault,LPCTSTR lpFileName);
DWORD		__stdcall HGetPrivateProfileIntW(PWCHAR lpAppName,PWCHAR lpKeyName,INT nDefault,PWCHAR lpFileName);
DWORD		__stdcall HGetProfileIntA(LPCTSTR lpAppName,LPCTSTR lpKeyName,INT nDefault);
DWORD		__stdcall HGetProfileIntW(PWCHAR lpAppName,PWCHAR lpKeyName,INT nDefault);
BOOL		__stdcall HWritePrivateProfileSectionA(LPCTSTR lpAppName,LPCTSTR lpString,LPCTSTR lpFileName);
BOOL		__stdcall HWritePrivateProfileSectionW(PWCHAR lpAppName,PWCHAR lpString,PWCHAR lpFileName);
DWORD		__stdcall HGetPrivateProfileSectionA(LPCTSTR lpAppName,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName);
DWORD		__stdcall HGetPrivateProfileSectionW(PWCHAR lpAppName,PWCHAR lpReturnedString,DWORD nSize,PWCHAR lpFileName);
BOOL		__stdcall HWriteProfileSectionA(LPCTSTR lpAppName,LPCTSTR lpString);
BOOL		__stdcall HWriteProfileSectionW(PWCHAR lpAppName,PWCHAR lpString);
DWORD		__stdcall HGetProfileSectionA(LPCTSTR lpAppName,LPTSTR lpReturnedString,DWORD nSize);
DWORD		__stdcall HGetProfileSectionW(PWCHAR lpAppName,PWCHAR lpReturnedString,DWORD nSize);
DWORD		__stdcall HGetPrivateProfileSectionNamesA(LPTSTR lpszReturnBuffer,DWORD nSize,LPCTSTR lpFileName);
DWORD		__stdcall HGetPrivateProfileSectionNamesW(PWCHAR lpszReturnBuffer,DWORD nSize,PWCHAR lpFileName);
BOOL		__stdcall HWritePrivateProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName);
BOOL		__stdcall HWritePrivateProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpString,PWCHAR lpFileName);
DWORD		__stdcall HGetPrivateProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName);
DWORD		__stdcall HGetPrivateProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpDefault,PWCHAR lpReturnedString,DWORD nSize,PWCHAR lpFileName);
BOOL		__stdcall HWriteProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString);
BOOL		__stdcall HWriteProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpString);
DWORD		__stdcall HGetProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize);
DWORD		__stdcall HGetProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpDefault,PWCHAR lpReturnedString,DWORD nSize);
BOOL		__stdcall HWritePrivateProfileStructA(LPCTSTR lpszSection,LPCTSTR lpszKey,PVOID lpStruct,UINT uSizeStruct,LPCTSTR szFile);
BOOL		__stdcall HWritePrivateProfileStructW(PWCHAR lpszSection,PWCHAR lpszKey,PVOID lpStruct,UINT uSizeStruct,PWCHAR szFile);
BOOL		__stdcall HGetPrivateProfileStructA(LPCTSTR lpszSection,LPCTSTR lpszKey,PVOID lpStruct,UINT uSizeStruct,LPCTSTR szFile);
BOOL		__stdcall HGetPrivateProfileStructW(PWCHAR lpszSection,PWCHAR lpszKey,PVOID lpStruct,UINT uSizeStruct,PWCHAR szFile);
BOOL 		__stdcall HTerminateProcess(HANDLE hProcess,UINT uExitCode);
DWORD		__stdcall HGetProcAddress(DWORD hModule, LPCSTR lpProcName);

#ifndef _HOOK_NT_


R3HOOK R3HTable[]= {
	{NULL, HCreateProcessA,				"CreateProcessA"},
	{NULL, HWinExec,					"WinExec"},
	{NULL, HLoadModule,					"LoadModule"},
	{NULL, HLoadLibraryA,				"LoadLibraryA"},
	{NULL, HLoadLibraryExA,				"LoadLibraryExA"},
	{NULL, HCreateFileA,				"CreateFileA"},
	{NULL, HOpenFile,					"OpenFile"},
	{NULL, H_lopen,						"_lopen"},
	{NULL, H_lcreat,					"_lcreat"},// Для монитора не нужна
	{NULL, HCopyFileA,					"CopyFileA"},
	{NULL, HCopyFileExA,				"CopyFileExA"},
	{NULL, HMoveFileA,					"MoveFileA"},
	{NULL, HMoveFileExA,				"MoveFileExA"},
	{NULL, HDeleteFileA,				"DeleteFileA"},// Для монитора не нужна
	{NULL, HSetFileAttributesA,			"SetFileAttributesA"},// Для монитора не нужна
	{NULL, HGetPrivateProfileIntA,		"GetPrivateProfileIntA"},
	{NULL, HGetProfileIntA,				"GetProfileIntA"},
	{NULL, HWritePrivateProfileSectionA,"WritePrivateProfileSectionA"},
	{NULL, HGetPrivateProfileSectionA,	"GetPrivateProfileSectionA"},
	{NULL, HWriteProfileSectionA,		"WriteProfileSectionA"},
	{NULL, HGetProfileSectionA,			"GetProfileSectionA"},
	{NULL, HGetPrivateProfileSectionNamesA, "GetPrivateProfileSectionNamesA"},
	{NULL, HWritePrivateProfileStringA,	"WritePrivateProfileStringA"},
	{NULL, HGetPrivateProfileStringA,	"GetPrivateProfileStringA"},
	{NULL, HWriteProfileStringA,		"WriteProfileStringA"},
	{NULL, HGetProfileStringA,			"GetProfileStringA"},
	{NULL, HWritePrivateProfileStructA,	"WritePrivateProfileStructA"},
	{NULL, HGetPrivateProfileStructA,	 "GetPrivateProfileStructA"},
	{NULL, HTerminateProcess,			 "TerminateProcess"},
};
#else
R3HOOK R3HTable[]= {
	{NULL, HLoadLibraryA,		"LoadLibraryA"},
	{NULL, HLoadLibraryExA,		"LoadLibraryExA"},
	{NULL, HLoadLibraryW,		"LoadLibraryW"},
	{NULL, HLoadLibraryExW,		"LoadLibraryExW"},
	{NULL, HGetProcAddress,		"GetProcAddress"},
};
#endif

#define INVALID_HANDLE_VALUE (HANDLE)-1

BOOLEAN R3HookUsed = FALSE;

void SetR3HookUsed(BOOLEAN bEnabled)
{
	AcquireResource(g_pR3Mutex, TRUE);

	EnableWriteToSections(TRUE);

	R3HookUsed = bEnabled;

	EnableWriteToSections(FALSE);

	ReleaseResource(g_pR3Mutex);
}

typedef struct _tMEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
    DWORD RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} tMEMORY_BASIC_INFORMATION, *tPMEMORY_BASIC_INFORMATION;


BOOL (__stdcall *DeviceIoControl)(HANDLE hDevice, DWORD dwIoControlCode, PVOID lpInBuffer, DWORD nInBufferSize, PVOID lpOutBuffer,DWORD nOutBufferSize, DWORD *lpBytesReturned, PVOID lpOverlapped);
HANDLE	(__stdcall *CreateFile)(PCHAR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,PVOID lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
BOOL (__stdcall *CloseHandle)(HANDLE hObject);
DWORD (__stdcall *GetFullPathName)(LPCTSTR lpFileName,DWORD nBufferLength,LPTSTR lpBuffer,LPTSTR *lpFilePart);
VOID (__stdcall *SetLastError)(DWORD dwErrCode);
DWORD (__stdcall *GetLastError)(void);
DWORD (__stdcall *SearchPath)(LPCTSTR lpPath,LPCTSTR lpFileName,LPCTSTR lpExtension,DWORD nBufferLength,LPTSTR lpBuffer,LPTSTR *lpFilePart);
UINT  (__stdcall *GetWindowsDirectory)(LPTSTR lpBuffer,UINT uSize);
BOOL (__stdcall *AreFileApisANSI)(VOID);
DWORD (__stdcall *VirtualQuery)(PVOID lpAddress, tPMEMORY_BASIC_INFORMATION lpBuffer, DWORD dwLength);


#ifdef _HOOK_VXD_	
	DWORD (_stdcall *GetCurrentProcessId)(VOID);
	BOOL (__stdcall *OemToChar)(LPCSTR lpszSrc,LPTSTR lpszDst); //!!! KERNEL32_12 for w9x
	BOOLEAN VxdCallsInited=FALSE;
	DWORD (__stdcall *VxDCall1)(enum AVPG_W32_Services ServiceNum, DWORD arg);
	typedef DWORD (__stdcall *VxDCall0)(enum AVPG_W32_Services ServiceNum);
	typedef DWORD (__stdcall *VxDCall3)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3);

	HANDLE (__stdcall *GetProcessHeap)(void);
	PVOID (__stdcall *R3HeapAlloc)(HANDLE hHeap,DWORD dwFlags,DWORD dwBytes);
	BOOL (__stdcall *R3HeapFree)(HANDLE hHeap,DWORD dwFlags,PVOID lpMem);

	PVOID GlobalAlloc(DWORD Flags,DWORD Size)
	{
		return R3HeapAlloc(GetProcessHeap(),0,Size);
	}
	
	BOOL GlobalFree(PVOID Addr)
	{
		return R3HeapFree(GetProcessHeap(),0,Addr);
	}
	
#else //Nt
	PVOID (__stdcall *GlobalAlloc)(DWORD dwFlags,DWORD dwBytes);
	BOOL (__stdcall *GlobalFree)(PVOID lpMem);
	
	DWORD (__stdcall *R3RtlOemToUnicodeN)(PCHAR UnicodeString,ULONG MaxBytesInUnicodeString,PULONG BytesInUnicodeString,PCHAR OemString,ULONG BytesInOemString);
	WCHAR (__stdcall *R3RtlUpcaseUnicodeChar)(WCHAR SourceCharacter);
	DWORD (__cdecl *R3wcslen)(PWCHAR Str);
	PWCHAR(__cdecl *R3wcscpy)(PWCHAR Dst, PWCHAR Src);
	DWORD (__stdcall *GetFullPathNameW)(PWCHAR lpFileName,DWORD nBufferLength,PWCHAR lpBuffer,PWCHAR *lpFilePart);
	DWORD (__stdcall *SearchPathW)(PWCHAR  lpPath,PWCHAR  lpFileName,PWCHAR  lpExtension,DWORD nBufferLength,PWCHAR  lpBuffer,PWCHAR  *lpFilePart);
	UINT  (__stdcall *GetWindowsDirectoryW)(PWCHAR lpBuffer,UINT uSize);

	DWORD (__stdcall *GetProcessHeaps)(DWORD NumberOfHeaps, PHANDLE ProcessHeaps);

	typedef DWORD (__stdcall *_SysCall0)(enum AVPG_W32_Services ServiceNum);
	typedef DWORD (__stdcall *_SysCall1)(enum AVPG_W32_Services ServiceNum, DWORD arg1);
	typedef DWORD (__stdcall *_SysCall2)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2);
	typedef DWORD (__stdcall *_SysCall3)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3);
	typedef DWORD (__stdcall *_SysCall4)(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4);

NTSTATUS __stdcall NTSysCall1(enum AVPG_W32_Services ServiceNum, DWORD arg1)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}
NTSTATUS __stdcall NTSysCall3(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}
NTSTATUS __stdcall NTSysCall5(enum AVPG_W32_Services ServiceNum, DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4, DWORD arg5)
{
	_asm {
		mov eax, ServiceNum;
		lea edx, arg1;
		int 2eh;
	}
}

int __stdcall CheckCallFrom(void* pStartParam, DWORD* pdwAddr, DWORD hModule)
{
	DWORD RetAddr;
	DWORD StackTop, StackBase;

	_asm {
		mov eax, fs:[4];
		mov StackTop, eax;
		mov eax, fs:[8];
		mov StackBase, eax;
	}

	RetAddr = *((DWORD*)(pStartParam) - 1);
	*pdwAddr = RetAddr;

	if (RetAddr >= StackBase && RetAddr < StackTop)
		return CALLFROMSTACKINDEX;

	if (hModule < 0x68000000)
	{
		// skip system?
		return 0;
	}
	
	if ((0x65000000 == hModule)
		&& (0x65000000 == (RetAddr & 0xfff00000)))
	{
		// skip vba
		return 0;
	}

	{
		DWORD tmp;
		tMEMORY_BASIC_INFORMATION mbi;
		memset(&mbi, 0, sizeof(mbi));
 
		tmp = VirtualQuery((PVOID) RetAddr, &mbi, sizeof(mbi));

		if (tmp)
		{
			BOOLEAN bProtectHasExecute = FALSE;
			BOOLEAN bAllocateProtectHasExecute = FALSE;
			BOOLEAN bSkipCase = FALSE;
			switch (mbi.Protect)
			{
			case PAGE_EXECUTE:
			case PAGE_EXECUTE_READ:
			case PAGE_EXECUTE_READWRITE:
			case PAGE_EXECUTE_WRITECOPY:
				bProtectHasExecute = TRUE;
				break;
			default:
				break;
			}
			
			if (PAGE_NOACCESS == mbi.AllocationProtect)
			{
				// skip .net
				bSkipCase = TRUE;
			}

			switch (mbi.AllocationProtect)
			{
			case PAGE_EXECUTE:
			case PAGE_EXECUTE_READ:
			case PAGE_EXECUTE_READWRITE:
			case PAGE_EXECUTE_WRITECOPY:
				bAllocateProtectHasExecute = TRUE;
				break;
			default:
				break;
			}

			if (bProtectHasExecute || bAllocateProtectHasExecute || bSkipCase)
				return 0;

			return CALLFROMDATAINDEX;
		}
	}

	return 0;
}

BOOL IsNeedR3HookFilter(Index);
NTSTATUS HR3CommonEx(ULONG index,PVOID FName, PVOID SecondName, BOOL bUni, DWORD AdditionalParam);

BOOLEAN __stdcall CheckDep(void* pArg, DWORD hModule, char* pchFuncName, PVOID pchFuncArg, BOOLEAN bUni)
{
#ifdef _STACKCHECK
	DWORD dwCallAddr;
	DWORD dwCallIdx = CheckCallFrom(pArg, &dwCallAddr, hModule);
	if (dwCallIdx)
	{
		if(IsNeedR3HookFilter(dwCallIdx))
		{
			NTSTATUS ntstatus = HR3CommonEx(dwCallIdx, pchFuncArg, pchFuncName, bUni, dwCallAddr);

			if (ntstatus != ERROR_SUCCESS)
				return TRUE;
		}
	}
#endif

	return FALSE;
}

#endif



#ifdef __DBG__
VOID (__stdcall *OutputDebugString)(LPCTSTR lpOutputString);
DWORD (__stdcall *GetModuleFileName)(HANDLE hModule,LPTSTR lpFilename,DWORD nSize);
DWORD (__stdcall *GetCurrentThreadId)(VOID);
#ifdef _HOOK_NT_
int (__cdecl *NTsprintf)(char *, const char *, ...);
int (__cdecl *NTvsprintf)(char *, const char *, /*va_list*/char*);
#endif

VOID R3DbgPrint(DEBUG_INFO_LEVEL Level,LPCTSTR lpFmt,...)
{
/*char Buff[1024];
va_list ArgPtr;
PCHAR DLN;
	if(DebLev < Level)
		return;
	if(Level>DL_WARNING && !(DC_R3 & DebCat))
		return;
	if(Level<sizeof(DebugLevelName)/sizeof(CHAR*))
		DLN=DebugLevelName[Level];
	else
		DLN="-???-";
	Buff[0]=0;
#ifdef _HOOK_NT_
	NTsprintf(Buff,"(%08x)%s %s-%s: ", GetCurrentThreadId(),DLN,DBG_PREFIX,DebCatName[DCB_R3]);
  va_start(ArgPtr,lpFmt);
	NTvsprintf(Buff+strlen(Buff),lpFmt,ArgPtr);
  va_end(ArgPtr);
#else
	sprintf(Buff,"(%x)%s %s-%s: ", GetCurrentThreadId(),DLN,DBG_PREFIX,DebCatName[DCB_R3]);
  va_start(ArgPtr,lpFmt);
	vsprintf(Buff+strlen(Buff),lpFmt,ArgPtr); //!!!!!!!!!!!!!!! VTOOLSD func in 3rd ring
  va_end(ArgPtr);
#endif
	OutputDebugString(Buff);*/
}

#define R3DbPrint(arg) R3DbgPrint arg

//+ ------------------------------------------------------------------------------------------

PCHAR R3GetProcName(PCHAR ProcessName)
{
char Buff[1024];
int i;
	ProcessName[0]=0;
	if(GetModuleFileName(NULL,Buff,sizeof(Buff))) {
		for(i=strlen(Buff);i>=0;i--) {
#ifdef _HOOK_VXD_
			if(Buff[i]=='.')
				Buff[i]=0;
#endif
			if(Buff[i]=='\\') {
				strcpy(ProcessName,Buff+i+1);
				break;
			}
		}
	}
	return ProcessName;
}
#else // __DBG__
#define R3DbPrint(arg)
#endif // __DBG__

//+ ------------------------------------------------------------------------------------------
#define _SPU_SIZE			15
#define _SPU_PATH_MAX_LEN	260

PCHAR GetNamePartShiftA(PCHAR* ppName)
{
	PCHAR ptmp = *ppName;
	while (*ptmp)
	{
		if (*ptmp == '\\')
			*ppName = ptmp + 1;

		ptmp++;
	}

	return *ppName;
}

PWCHAR GetNamePartShiftW(PWCHAR* ppName)
{
	PWCHAR ptmp = *ppName;
	while (*ptmp)
	{
		if (*ptmp == L'\\')
			*ppName = ptmp + 1;

		ptmp++;
	}

	return *ppName;
}

#ifdef _HOOK_NT_
typedef struct _R3SputnikW
{
	WCHAR m_Module[_SPU_PATH_MAX_LEN];
	WCHAR m_Sputnik[_SPU_PATH_MAX_LEN];
	int m_Len;
} R3SputnikW;

R3SputnikW gSputnikW[_SPU_SIZE];

PWCHAR NeedLoadSputnikW(PWCHAR OrigName)
{
	int cou;
	int len = R3wcslen(GetNamePartShiftW(&OrigName));
	int idxOrig;
	for (cou = 0; cou < _SPU_SIZE; cou++)
	{
		WCHAR symbol;
		WCHAR symbolOrig;
		int idx;
		if (gSputnikW[cou].m_Len <= len)
		{
			idx = (gSputnikW[cou].m_Len - 1);
			idxOrig = (len - 1);
				
			while (idx > 0)
			{
				symbol = R3RtlUpcaseUnicodeChar(gSputnikW[cou].m_Module[idx]);
				symbolOrig = R3RtlUpcaseUnicodeChar(OrigName[idxOrig]);
	
				if (symbol != symbolOrig)
					break;

				idx--;
				idxOrig--;
			}

			if (!idx)
				return gSputnikW[cou].m_Sputnik;
		}
	}
	return NULL;
}

#endif // _HOOK_NT_ - Unicode

typedef struct _R3Sputnik
{
	CHAR m_Module[_SPU_PATH_MAX_LEN];
	CHAR m_Sputnik[_SPU_PATH_MAX_LEN];
	int m_Len;
} R3Sputnik;

R3Sputnik gSputnik[_SPU_SIZE];

PCHAR NeedLoadSputnik(PCHAR OrigName)
{
	int cou;
	int idxOrig;
	int len = strlen(GetNamePartShiftA(&OrigName));

	for (cou = 0; cou < _SPU_SIZE; cou++)
	{
		char symbol;
		char symbolOrig;
		int idx;

		if (gSputnik[cou].m_Len == len)
		{
			idx = (gSputnik[cou].m_Len - 1);
			idxOrig = (len - 1);
				
			while (idx > 0)
			{
				symbol = (char) gSputnik[cou].m_Module[idx];
				symbolOrig = (char) OrigName[idxOrig];
				if ((symbol | 0x20) != (symbolOrig | 0x20))
					break;

				idx--;
				idxOrig--;
			}

			if (!idx)
				return gSputnik[cou].m_Sputnik;
		}
	}
	return NULL;
}

//+ ------------------------------------------------------------------------------------------

ULONG R3GetIdex(PVOID HookFunc)
{
ULONG i;
	for(i=0;i<sizeof(R3HTable)/sizeof(R3HOOK);i++) 
		if(R3HTable[i].Hook==HookFunc)
			break;
	return i;
}

void itoa_impW(WCHAR* BuffOrdinal, unsigned int num)
{
	unsigned int d = 1000000000;
	int pos = 0;

	BuffOrdinal[pos++] = '#';

	while (num)
	{
		if (num >= d)
		{
			unsigned int n = num / d;
			BuffOrdinal[pos] = '0' + n;
			pos++;
			num -= n * d;
		}
		d /= 10;
	}

	if (1 == pos)
		BuffOrdinal[pos++] = '0';
	
	BuffOrdinal[pos] = 0;
}


void itoa_impA(char* BuffOrdinal, unsigned int num)
{
	unsigned int d = 1000000000;
	int pos = 0;
	
	BuffOrdinal[pos++] = '#';

	while (num)
	{
		if (num >= d)
		{
			unsigned int n = num / d;
			BuffOrdinal[pos] = '0' + n;
			pos++;
			num -= n * d;
		}
		d /= 10;
	}

	if (1 == pos)
		BuffOrdinal[pos++] = '0';
	
	BuffOrdinal[pos] = 0;
}

NTSTATUS HR3CommonEx(ULONG index,PVOID FName, PVOID SecondName, BOOL bUni, DWORD AdditionalParam)
{
#ifdef __DBG__
	CHAR ProcName[PROCNAMELEN];
#endif // __DBG__
	VERDICT Verdict=Verdict_NotFiltered;
	DWORD BytesRet;
	PSINGLE_PARAM pSingleParam;
	HANDLE		hDevice = NULL;
	PBYTE			pByte;
	DWORD			tmp;
	BOOLEAN		OEMCharSet=FALSE;
	PBYTE			RequestData;
	PFILTER_EVENT_PARAM pParam;
	ULONG			Param1Size;
	ULONG			Param2Size=0;
	BYTE			Buff[sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM)*4 + MAXPATHLEN * 2 + 1 + sizeof(DWORD) + sizeof(HANDLE)*64];
	WCHAR			BuffOrdinal[20];

	if(FName==NULL)
		return ERROR_SUCCESS;

#ifdef _HOOK_NT_
	if(bUni)
	{
		if ((DWORD) FName < 0x10000)
		{
			itoa_impW(BuffOrdinal, (DWORD) FName);
			FName = BuffOrdinal;
		}

		if(*(PCHAR)FName==0)
			return ERROR_SUCCESS;

		if (index == CALLFROMSTACKINDEX)
		{
			R3DbPrint((DL_INFO,"%s %s (%S %S)\n", R3GetProcName(ProcName),
				"CallFromStack",
				*(PWCHAR)FName?FName:"", SecondName?SecondName:""));
		}
		else if (index == CALLFROMDATAINDEX)
		{
			R3DbPrint((DL_INFO,"%s %s (%S %S)\n", R3GetProcName(ProcName),
				"DEP",
				*(PWCHAR)FName?FName:"", SecondName?SecondName:""));
		}
		else
		{
			R3DbPrint((DL_INFO,"%s %s (%S %S)\n",R3GetProcName(ProcName),
				R3HTable[index].FuncName,
				*(PWCHAR)FName?FName:"", SecondName?SecondName:""));
		}

		Param1Size=(R3wcslen(FName) + 1)*sizeof(WCHAR);
		if(SecondName!=NULL) {
			Param2Size=(R3wcslen(SecondName) + 1)*sizeof(WCHAR);
		}
	} else 
#endif
	{ //ansi
		/*R3DbPrint((DL_INFO,"%s %s (%s %s)\n",R3GetProcName(ProcName),
			index == CALLFROMSTACKINDEX ? "CallFromStack" : R3HTable[index].FuncName,
			*(PCHAR)FName?FName:"", SecondName?SecondName:""));*/

		if ((DWORD) FName < 0x10000)
		{
			itoa_impA((char*) BuffOrdinal, (DWORD) FName);
			FName = BuffOrdinal;
		}

		if(*(PWCHAR)FName==0)
			return ERROR_SUCCESS;

		Param1Size=strlen(FName) + 1;
		if(SecondName!=NULL) {
			Param2Size=strlen(SecondName) + 1;
		}
		OEMCharSet=!AreFileApisANSI();
#ifdef _HOOK_NT_
		if(OEMCharSet){
			Param1Size=Param1Size*sizeof(WCHAR);
			Param2Size=Param2Size*sizeof(WCHAR);
		}
#endif
	}

	if(Param1Size+Param2Size >= MAXPATHLEN * 2) {
		DbgBreakPoint();
		RequestData=GlobalAlloc(0,sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM)*2 + Param1Size + Param2Size + 4);
		if(RequestData==NULL) {
			R3DbPrint((DL_WARNING,"HR3Common fail alloc %d bytes for RequestData\n",sizeof(FILTER_EVENT_PARAM) + sizeof(SINGLE_PARAM)*2 + Param1Size + Param2Size + 1));
			return ERROR_SUCCESS;//?? А может deny?
		}
	} else {
		RequestData=Buff;
	}
	pParam = (PFILTER_EVENT_PARAM)RequestData;
	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_R3, index, 0, PreProcessing, 0);
	pSingleParam = (PSINGLE_PARAM) pParam->Params;

	pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE;
	pSingleParam->ParamSize = Param1Size;
	
	if(bUni)
		pSingleParam->ParamID = _PARAM_OBJECT_URL_W; 
	else
		pSingleParam->ParamID = _PARAM_OBJECT_URL; 

	if(OEMCharSet){
#ifdef _HOOK_VXD_
		OemToChar(FName,pSingleParam->ParamValue); //KERNEL32_12
#else
		NTSTATUS Status;
		if((Status=R3RtlOemToUnicodeN(pSingleParam->ParamValue,Param1Size,NULL,FName,Param1Size/sizeof(WCHAR)))==STATUS_SUCCESS) {
			pSingleParam->ParamID = _PARAM_OBJECT_URL_W; 
		} else {
			R3DbPrint((DL_WARNING,"HR3Common fail convert %s to unicode Status=%x\n",Status));
			*(WCHAR*)pSingleParam->ParamValue=0;
			pSingleParam->ParamSize =0;
		}
#endif
	} else {
		memcpy(pSingleParam->ParamValue, FName, pSingleParam->ParamSize);
	}


	if(SecondName!=NULL) {
		tmp = sizeof(SINGLE_PARAM) + pSingleParam->ParamSize;
		pByte = (BYTE*)pSingleParam + tmp;
		pSingleParam = (PSINGLE_PARAM) pByte;
		pSingleParam->ParamFlags = _SINGLE_PARAM_FLAG_NONE; 
		if(bUni)
			pSingleParam->ParamID = _PARAM_OBJECT_URL_DEST_W; 
		else
			pSingleParam->ParamID = _PARAM_OBJECT_URL_DEST; 
		pSingleParam->ParamSize = Param2Size;

		if(OEMCharSet){
#ifdef _HOOK_VXD_
			OemToChar(SecondName,pSingleParam->ParamValue); //KERNEL32_12
#else
			NTSTATUS Status;
			if((Status=R3RtlOemToUnicodeN(pSingleParam->ParamValue,Param2Size,NULL,SecondName,Param2Size/sizeof(WCHAR)))==STATUS_SUCCESS) {
				pSingleParam->ParamID = _PARAM_OBJECT_URL_DEST_W; 
			} else {
				R3DbPrint((DL_WARNING,"HR3Common fail convert %s to unicode Status=%x\n",Status));
				*(WCHAR*)pSingleParam->ParamValue=0;
				pSingleParam->ParamSize =0;
			}
#endif
		} else {
			memcpy(pSingleParam->ParamValue,SecondName, pSingleParam->ParamSize);
		}
		pParam->ParamsCount = 2;
	} else {
		pParam->ParamsCount = 1;
	}

#ifdef _HOOK_NT_
	if (AdditionalParam)
	{
		pParam->ParamsCount++;

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR1, AdditionalParam);

		if ((index == CALLFROMDATAINDEX) && GetProcessHeaps)
		{
			DWORD dwRet;
			SINGLE_PARAM_SHIFT(pSingleParam);
			dwRet = GetProcessHeaps(64, (PHANDLE) pSingleParam->ParamValue);
			if (dwRet)
			{
				pParam->ParamsCount++;
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE);
				pSingleParam->ParamSize = sizeof(HANDLE) * dwRet;
			}
		}
	}
#endif

#ifdef _HOOK_VXD_	
	if(VxdCallsInited) {
		Verdict=VxDCall1((KLIF_DeviceID<<16) + _W32ServFilterEvent,(DWORD)RequestData);
#else // Nt
	if(SysCallsInited) {
		Verdict=NTSysCall1((enum AVPG_W32_Services)(_W32ServFilterEvent + DirectSysCallStartId),(DWORD)RequestData);
#endif
	} else {
		if((hDevice = CreateFile("\\\\.\\"AVPGNAME, 0, 0, NULL, 0, 0, NULL)) != INVALID_HANDLE_VALUE) {
			if(!DeviceIoControl(hDevice, IOCTLHOOK_FilterEvent, RequestData, sizeof(RequestData), &Verdict, sizeof(Verdict), &BytesRet, NULL)) {
				R3DbPrint((DL_ERROR, "deviceIoControl failed in R3 hook\n"));
			}
			CloseHandle(hDevice);
		} else {
			R3DbPrint((DL_ERROR, "Can't open driver in R3 hook\n"));
		}
	}

	if(RequestData!=Buff)
		GlobalFree(RequestData);

	if(!_PASS_VERDICT(Verdict)) {
		if(Verdict == Verdict_Kill) {
			R3DbPrint((DL_NOTIFY, "Kill %s\n", pParam->ProcName?pParam->ProcName:""));
		} else if(Verdict == Verdict_Discard) {
			R3DbPrint((DL_NOTIFY, "Discard %s - %s %s\n", pParam->ProcName?pParam->ProcName:"", FName, SecondName?SecondName:""));
		}
		return ERROR_ACCESS_DENIED;
	}
	return ERROR_SUCCESS;
}

NTSTATUS HR3Common(ULONG index,PVOID FName, PVOID SecondName, BOOL bUni)
{
	return HR3CommonEx(index, FName, SecondName, bUni, 0);
}

PCHAR GetExecNameFromCommandLine(PCHAR lpCommandLine)
{
PCHAR	StartPos;
PCHAR	EndPos;
PCHAR Buff;
PCHAR Tmp;
DWORD	Len;
PCHAR	lpFilePart;
	if(lpCommandLine==NULL || *lpCommandLine==0) {
		R3DbPrint((DL_ERROR, "GetExecNameFromCommandLine. Null CommandLine\n"));
		return NULL;
	}
	StartPos=lpCommandLine;
	if(*StartPos=='\"') {
		StartPos++;
		EndPos=StartPos;
		Len=0;
		while(*EndPos!=0) {
			if(*EndPos=='\"') {
				break;
			}
			Len++;
			EndPos++;
		}
		Buff=GlobalAlloc(0,Len+1);
		if(Buff) {
			memcpy(Buff,StartPos,Len);
			Buff[Len]=0;
		}
		return Buff;
	} 
	Len=strlen(lpCommandLine);
	Tmp=GlobalAlloc(0,Len+1);
	Buff=GlobalAlloc(0,MAXPATHLEN);
	if(Tmp && Buff) {
		EndPos=StartPos;
		do {
			EndPos++;
			if(*EndPos==' ' || *EndPos==0) {
				memcpy(Tmp,StartPos,Len=EndPos-StartPos);
				Tmp[Len]=0;
				Len=SearchPath(NULL,Tmp,".EXE",MAXPATHLEN,Buff,&lpFilePart);
				if(Len>=MAXPATHLEN) {
					GlobalFree(Buff);
					Len++;
					Buff=GlobalAlloc(0,Len+1);
					if(Buff==NULL) {
						GlobalFree(Tmp);
						return NULL;
					}
					Len=SearchPath(NULL,Tmp,".EXE",Len,Buff,&lpFilePart);
				}
				if(Len) {
					GlobalFree(Tmp);
					return Buff;
				}
			}
		} while (*EndPos);
	}
	if(Buff)
		GlobalFree(Buff);
	if(Tmp)
		GlobalFree(Tmp);
	R3DbPrint((DL_ERROR, "GetExecNameFromCommandLine. Exec not found in: %s\n",lpCommandLine));
	return NULL;
}

#ifdef _HOOK_NT_

PWCHAR  GetExecNameFromCommandLineW(PWCHAR lpCommandLine)
{
PWCHAR	StartPos;
PWCHAR	EndPos;
PWCHAR Buff;
PWCHAR Tmp;
DWORD	Len;
PWCHAR	lpFilePart;
	if(lpCommandLine==NULL || *lpCommandLine==0) {
		R3DbPrint((DL_ERROR, "GetExecNameFromCommandLine. Null CommandLine\n"));
		return NULL;
	}
	StartPos=lpCommandLine;
	if(*StartPos=='\"') {
		StartPos++;
		EndPos=StartPos;
		Len=0;
		while(*EndPos!=0) {
			if(*EndPos=='\"') {
				break;
			}
			Len++;
			EndPos++;
		}
		Buff=GlobalAlloc(0,(Len+1)*sizeof(WCHAR));
		if(Buff) {
			memcpy(Buff,StartPos,Len*sizeof(WCHAR));
			Buff[Len]=0;
		}
		return Buff;
	} 
	Len=R3wcslen(lpCommandLine);
	Tmp=GlobalAlloc(0,(Len+1)*sizeof(WCHAR));
	Buff=GlobalAlloc(0,MAXPATHLEN*sizeof(WCHAR));
	if(Tmp && Buff) {
		EndPos=StartPos;
		do {
			EndPos++;
			if(*EndPos==' ' || *EndPos==0) {
				memcpy(Tmp,StartPos,(Len=EndPos-StartPos)*sizeof(WCHAR));
				Tmp[Len]=0;
				Len=SearchPathW(NULL,Tmp,L".EXE",MAXPATHLEN*sizeof(WCHAR),Buff,&lpFilePart);
				if(Len>MAXPATHLEN) {
					GlobalFree(Buff);
					Len+=sizeof(WCHAR);
					Buff=GlobalAlloc(0,(Len+1)*sizeof(WCHAR));
					if(Buff==NULL) {
						GlobalFree(Tmp);
						return NULL;
					}
					Len=SearchPathW(NULL,Tmp,L".EXE",Len*sizeof(WCHAR),Buff,&lpFilePart);
				}
				if(Len) {
					GlobalFree(Tmp);
					return Buff;
				}
			}
		} while (*EndPos);
	}
	if(Buff)
		GlobalFree(Buff);
	if(Tmp)
		GlobalFree(Tmp);
	R3DbPrint((DL_ERROR, "GetExecNameFromCommandLine. Exec not found in: %s\n",lpCommandLine));
	return NULL;
}
#endif

DWORD MyGetFullPathName(LPCTSTR lpFileName,PCHAR *Buff)
{
DWORD BufLen;
DWORD ret;
LPTSTR lpFilePart;
	if(lpFileName==NULL || *lpFileName==0) {
		R3DbPrint((DL_ERROR, "MyGetFullPathName. Null param\n"));
		*Buff=NULL;
		return 0;
	}
	BufLen=MAXPATHLEN;
label1:
	*Buff=GlobalAlloc(0,BufLen);
	if(*Buff==NULL) {
		return 0;
	}
	ret=GetFullPathName(lpFileName,BufLen,*Buff,&lpFilePart);
	if(ret>=BufLen) {
		GlobalFree(*Buff);
		BufLen=ret+1;
		goto label1;
	}
	if(ret==0) {
		R3DbPrint((DL_ERROR, "MyGetFullPathName fail with %s\n",lpFileName));
		GlobalFree(*Buff);
		*Buff=NULL;
	}
	return ret;
}

#ifdef _HOOK_NT_
DWORD MyGetFullPathNameW(PWCHAR lpFileName,PWCHAR *Buff)
{
	DWORD BufLen;
	DWORD ret;
	PWCHAR lpFilePart;
	if(lpFileName==NULL || *lpFileName==0) {
		R3DbPrint((DL_ERROR, "MyGetFullPathName. Null param\n"));
		*Buff=NULL;
		return 0;
	}
	BufLen=MAXPATHLEN;
label1:
	*Buff=GlobalAlloc(0,BufLen*sizeof(WCHAR));
	if(*Buff==NULL) {
		return 0;
	}
	ret=GetFullPathNameW(lpFileName,BufLen,*Buff,&lpFilePart);
	if(ret>=BufLen) {
		GlobalFree(*Buff);
		BufLen=ret+sizeof(WCHAR);
		goto label1;
	}
	if(ret==0) {
		R3DbPrint((DL_ERROR, "MyGetFullPathName fail with %S\n",lpFileName));
		GlobalFree(*Buff);
		*Buff=NULL;
	}
	return ret;
}
#endif

DWORD MySearchPath(LPCTSTR lpFileName,LPCTSTR lpExtension,PCHAR *Buff)
{
LPTSTR lpFilePart;
DWORD ret;
DWORD BufLen;
	if(lpFileName==NULL  || *lpFileName==0) {
		R3DbPrint((DL_ERROR, "MySearchPath. Null param\n"));
		*Buff=NULL;
		return 0;
	}
	BufLen=MAXPATHLEN;
label1:
	*Buff=GlobalAlloc(0,BufLen);
	if(*Buff==NULL) {
		return 0;
	}
	ret=SearchPath(NULL,lpFileName,lpExtension,BufLen,*Buff,&lpFilePart);
	if(ret>=BufLen) {
		GlobalFree(*Buff);
		BufLen=ret+1;
		goto label1;
	}
	if(ret==0) {
		R3DbPrint((DL_ERROR, "MySearchPath fail with %s %s\n",lpFileName,lpExtension));
		GlobalFree(*Buff);
		*Buff=NULL;
	}
	return ret;
}

#ifdef _HOOK_NT_
DWORD MySearchPathW(PWCHAR lpFileName,PWCHAR lpExtension,PWCHAR *Buff)
{
PWCHAR lpFilePart;
DWORD ret;
DWORD BufLen;
	if(lpFileName==NULL || *lpFileName==0) {
		R3DbPrint((DL_ERROR, "MySearchPath. Null param\n"));
		*Buff=NULL;
		return 0;
	}
	BufLen=MAXPATHLEN;
label1:
	*Buff=GlobalAlloc(0,BufLen*sizeof(WCHAR));
	if(*Buff==NULL) {
		return 0;
	}
	ret=SearchPathW(NULL,lpFileName,lpExtension,BufLen,*Buff,&lpFilePart);
	if(ret>=BufLen) {
		GlobalFree(*Buff);
		BufLen=ret+sizeof(WCHAR);
		goto label1;
	}
	if(ret==0) {
		R3DbPrint((DL_ERROR, "MySearchPath fail with %S %S\n",lpFileName,lpExtension));
		GlobalFree(*Buff);
		*Buff=NULL;
	}
	return ret;
}
#endif


CHAR WinIniStr[]="win.ini";
PCHAR AppendWinDir(LPCTSTR Src)
{
DWORD len;
DWORD SrcLen;
PCHAR Buf;
	if(Src==NULL)
		return NULL;
	SrcLen=strlen(Src)+1;
	if(*(WORD*)(Src+1)=='\\:' || *(WORD*)(Src)=='\\\\') {
		Buf=GlobalAlloc(0,SrcLen);
		if(Buf)
			strcpy(Buf,Src);
		return Buf;
	}
	Buf=GlobalAlloc(0,MAXPATHLEN+SrcLen);
	if(!Buf)
		return NULL;
	len=GetWindowsDirectory(Buf,MAXPATHLEN);
	if(len>MAXPATHLEN) {
		GlobalFree(Buf);
		Buf=GlobalAlloc(0,len+SrcLen+1);
		if(Buf==NULL) {
			return NULL;
		}
		len=GetWindowsDirectory(Buf,len);
	}
	if(!len) {
		GlobalFree(Buf);
		return NULL;
	}
	if(Buf[len-1]!='\\')
		*(WORD*)(Buf+ (len++))='\\';
	strcpy(Buf+len,Src);
	return Buf;
}

#ifdef _HOOK_NT_
WCHAR WinIniStrW[]=L"win.ini";
PWCHAR AppendWinDirW(PWCHAR Src)
{
DWORD len;
DWORD SrcLen;
PWCHAR Buf;
	if(Src==NULL)
		return NULL;
	SrcLen=R3wcslen(Src)+1;
	if(*(DWORD*)(Src+1)==0x5c003a //'\\:'
		|| *(DWORD*)(Src)==0x5c005c //'\\\\'
		) {
		Buf=GlobalAlloc(0,SrcLen*sizeof(WCHAR));
		if(Buf)
			R3wcscpy(Buf,Src);
		return Buf;
	}
	Buf=GlobalAlloc(0,(MAXPATHLEN+SrcLen)*sizeof(WCHAR));
	if(!Buf)
		return NULL;
	len=GetWindowsDirectoryW(Buf,MAXPATHLEN);
	if(len>MAXPATHLEN) {
		GlobalFree(Buf);
		Buf=GlobalAlloc(0,(len+SrcLen+1)*sizeof(WCHAR));
		if(Buf==NULL) {
			return NULL;
		}
		len=GetWindowsDirectoryW(Buf,len);
	}
	if(!len) {
		GlobalFree(Buf);
		return NULL;
	}
	if(Buf[len-1]!=L'\\')
		*(DWORD*)(Buf+ (len++))=L'\\';
	R3wcscpy(Buf+len,Src);
	return Buf;
}
#endif

BOOL IsNeedR3HookFilter(Index) 
{
	if(!R3HookUsed)
		return FALSE;
#ifdef _HOOK_VXD_	
	if(!VxdCallsInited)
		return TRUE;
	return ((VxDCall3)VxDCall1)((KLIF_DeviceID<<16) + _W32ServIsNeedFltEvAndNotInvisTh,FLTTYPE_R3,Index,0);
#else //Nt
	if(!SysCallsInited) 
		return TRUE;
	return NTSysCall3((enum AVPG_W32_Services)(_W32ServIsNeedFltEvAndNotInvisTh + DirectSysCallStartId),FLTTYPE_R3,Index,0);
#endif
}

BOOL		__stdcall HCreateProcessA(LPCTSTR lpApplicationName,LPTSTR lpCommandLine,PVOID lpProcessAttributes,PVOID lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,PVOID lpEnvironment,LPCTSTR lpCurrentDirectory,PVOID lpStartupInfo,PVOID lpProcessInformation)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//	InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCreateProcessA);
	if(IsNeedR3HookFilter(index)) {
		if(lpApplicationName)
			MyGetFullPathName(lpApplicationName,&fullname);
		else
			fullname=GetExecNameFromCommandLine(lpCommandLine);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPTSTR,PVOID,PVOID,BOOL,DWORD,PVOID,LPCTSTR,PVOID,PVOID))(R3HTable[index].Real))(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPTSTR,PVOID,PVOID,BOOL,DWORD,PVOID,LPCTSTR,PVOID,PVOID))(R3HTable[index].Real))(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HCreateProcessW(PWCHAR lpApplicationName,PWCHAR lpCommandLine,PVOID lpProcessAttributes,PVOID lpThreadAttributes,BOOL bInheritHandles,DWORD dwCreationFlags,PVOID lpEnvironment,PWCHAR lpCurrentDirectory,PVOID lpStartupInfo,PVOID lpProcessInformation)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//	InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCreateProcessW);
	if(IsNeedR3HookFilter(index)) {
		if(lpApplicationName)
			MyGetFullPathNameW(lpApplicationName,&fullname);
		else
			fullname=GetExecNameFromCommandLineW(lpCommandLine);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,PVOID,BOOL,DWORD,PVOID,PWCHAR,PVOID,PVOID))(R3HTable[index].Real))(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,PVOID,BOOL,DWORD,PVOID,PWCHAR,PVOID,PVOID))(R3HTable[index].Real))(lpApplicationName,lpCommandLine,lpProcessAttributes,lpThreadAttributes,bInheritHandles,dwCreationFlags,lpEnvironment,lpCurrentDirectory,lpStartupInfo,lpProcessInformation);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HWinExec(LPCSTR lpCmdLine,DWORD uCmdShow)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//	InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWinExec);
	if(IsNeedR3HookFilter(index)) {
		fullname=GetExecNameFromCommandLine((PCHAR)lpCmdLine); //!! See MSDN comments for CreateProcess
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCSTR,DWORD))(R3HTable[index].Real))(lpCmdLine,uCmdShow);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCSTR,DWORD))(R3HTable[index].Real))(lpCmdLine,uCmdShow);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

DWORD		__stdcall HLoadModule(LPCSTR lpModuleName,PVOID lpParameterBlock)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//	InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HLoadModule);
	if(IsNeedR3HookFilter(index)) {
		MySearchPath(lpModuleName,".EXE",&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCSTR,PVOID))(R3HTable[index].Real))(lpModuleName,lpParameterBlock);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCSTR,PVOID))(R3HTable[index].Real))(lpModuleName,lpParameterBlock);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifndef _HOOK_NT_
BOOLEAN __stdcall CheckDep(void* pArg, DWORD hModule, char* pchFuncName, PVOID pchFuncArg, BOOLEAN bUni)
{
	return FALSE;
}
#endif


#ifdef _HOOK_NT_
//--------------------------------------------------------------------------------------------------------------
#define WINAPI	NTAPI

typedef PVOID HMODULE;

typedef
HMODULE (WINAPI *t_fLoadLibraryW)(
  LPCWSTR lpFileName
);

typedef
PVOID (WINAPI *t_fGetProcAddress)(
  HMODULE hModule,
  LPCSTR lpProcName
);

typedef
HMODULE (WINAPI *t_fGetModuleHandleA)(
  LPCSTR lpModuleName
);

typedef
HMODULE (WINAPI *t_fGetModuleHandleW)(
  LPCWSTR lpModuleName
);

typedef VOID (WINAPI *t_fhme)();

VOID WINAPI HookExportsA( IN PCHAR cModuleName )
{
	ULONG Index;
	t_fLoadLibraryW fLoadLibraryW;
	HMODULE hKernel32;

	if ( !cModuleName )
		return;

	Index = R3GetIdex( (PVOID)HLoadLibraryW );
	fLoadLibraryW = (t_fLoadLibraryW) R3HTable[Index].Real;
	hKernel32 = fLoadLibraryW( L"kernel32.dll" );
	if ( hKernel32 )
	{
		t_fGetProcAddress fGetProcAddress;
		t_fGetModuleHandleA fGetModuleHandleA;

		Index = R3GetIdex( (PVOID)HGetProcAddress );
		fGetProcAddress = (t_fGetProcAddress) R3HTable[Index].Real;

		fGetModuleHandleA = (t_fGetModuleHandleA) fGetProcAddress( hKernel32, "GetModuleHandleA" );
		if ( fGetModuleHandleA )
		{
			HMODULE hMod;

			hMod = fGetModuleHandleA( cModuleName );
			if ( hMod )
			{
				t_fhme fhme;

				fhme = (t_fhme) fGetProcAddress( hMod, "hme" );
				if ( fhme )											// here we call Hook Module Exports
					fhme();											// 
			}
		}
	}
}

VOID WINAPI HookExportsW( IN PWCHAR wcModuleName )
{
	ULONG Index;
	t_fLoadLibraryW fLoadLibraryW;
	HMODULE hKernel32;

	if ( !wcModuleName )
		return;

	Index = R3GetIdex( (PVOID)HLoadLibraryW );
	fLoadLibraryW = (t_fLoadLibraryW) R3HTable[Index].Real;
	hKernel32 = fLoadLibraryW( L"kernel32.dll" );
	if ( hKernel32 )
	{
		t_fGetProcAddress fGetProcAddress;
		t_fGetModuleHandleW fGetModuleHandleW;

		Index = R3GetIdex( (PVOID)HGetProcAddress );
		fGetProcAddress = (t_fGetProcAddress) R3HTable[Index].Real;

		fGetModuleHandleW = (t_fGetModuleHandleW) fGetProcAddress( hKernel32, "GetModuleHandleW" );
		if ( fGetModuleHandleW )
		{
			HMODULE hMod;

			hMod = fGetModuleHandleW( wcModuleName );
			if ( hMod )
			{
				t_fhme fhme;

				fhme = (t_fhme) fGetProcAddress( hMod, "hme" );
				if ( fhme )											// here we call Hook Module Exports
					fhme();											// 
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------------
#endif

HANDLE	__stdcall HLoadLibraryA(PCHAR lpFileName)
{
	PCHAR		fullname = NULL;
	ULONG		index;
	LONG		ntstatus;
	HANDLE	ret;
	PCHAR	fullname_sputnik = NULL;

	BOOLEAN bBlock = FALSE;

	bBlock = CheckDep(&lpFileName, 0, "LoadLibraryA", lpFileName, FALSE);

	if (!bBlock)
	{
		index = R3GetIdex((PVOID)HLoadLibraryA);

		fullname_sputnik = NeedLoadSputnik(lpFileName);

		if(IsNeedR3HookFilter(index))
		{
			MySearchPath(lpFileName,".DLL",&fullname);
			if((ntstatus=HR3Common(index,fullname, NULL,FALSE)) != ERROR_SUCCESS)
				bBlock = TRUE;
		}
	}
	
	if (bBlock)
	{
		ret = NULL;
		SetLastError(ERROR_ACCESS_DENIED);
	}
	else
	{
		ret = ((HANDLE (__stdcall *)(PCHAR))(R3HTable[index].Real))(lpFileName);
		
		if (ret && fullname_sputnik)
		{
			DWORD dwLasterr = GetLastError();

#ifdef _HOOK_NT_
			HookExportsA( fullname_sputnik );
#endif

			((HANDLE (__stdcall *)(PCHAR))(R3HTable[index].Real))(fullname_sputnik);
			SetLastError(dwLasterr);
		}
	}

	if (ret)
	{
		bBlock = CheckDep(&lpFileName, (DWORD) ret, "LoadLibraryA", lpFileName, FALSE);
		if (bBlock)
		{
			ret = 0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
	}

	if(fullname)
		GlobalFree(fullname);

	return ret;
}

#ifdef _HOOK_NT_
HANDLE	__stdcall HLoadLibraryW(PWCHAR lpFileName)
{
	PWCHAR	fullname = NULL;
	ULONG		index;
	LONG		ntstatus;
	HANDLE	ret;
	PWCHAR	fullname_sputnik = NULL;

	BOOLEAN bBlock = FALSE;

	bBlock = CheckDep(&lpFileName, 0, "LoadLibraryW", lpFileName, TRUE);

	if (!bBlock)
	{
		index = R3GetIdex((PVOID)HLoadLibraryW);
	
		fullname_sputnik = NeedLoadSputnikW(lpFileName);
		
		if(IsNeedR3HookFilter(index))
		{
			MySearchPathW(lpFileName, L".DLL", &fullname);
			if((ntstatus = HR3Common(index,fullname, NULL,TRUE))!=ERROR_SUCCESS)
				bBlock = TRUE;
		}
	}
	
	if (bBlock)
	{
		ret = NULL;
		SetLastError(ERROR_ACCESS_DENIED);
	}
	else
	{
		ret = ((HANDLE (__stdcall *)(PWCHAR))(R3HTable[index].Real))(lpFileName);
	
		if (ret && fullname_sputnik)
		{
			DWORD dwLasterr = GetLastError();

			HookExportsW( fullname_sputnik );

			((HANDLE (__stdcall *)(PWCHAR))(R3HTable[index].Real))(fullname_sputnik);
			SetLastError(dwLasterr);
		}
	}

	if (ret)
	{
		bBlock = CheckDep(&lpFileName, (DWORD) ret, "LoadLibraryW", lpFileName, TRUE);
		if (bBlock)
		{
			ret = 0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
	}

	if(fullname)
		GlobalFree(fullname);

	return ret;
}
#endif

#define DONT_RESOLVE_DLL_REFERENCES   0x00000001
#define LOAD_LIBRARY_AS_DATAFILE      0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH 0x00000008
#define LOAD_IGNORE_CODE_AUTHZ_LEVEL  0x00000010

HANDLE	__stdcall HLoadLibraryExA(PCHAR lpFileName,HANDLE hFile,DWORD dwFlags)
{
	PCHAR		fullname=NULL;
	ULONG		index;
	LONG		ntstatus;
	HANDLE	ret;
	PCHAR	fullname_sputnik = NULL;

	BOOLEAN bBlock = FALSE;

	bBlock = CheckDep(&lpFileName, 0, "LoadLibraryExA", lpFileName, FALSE);

	if (!bBlock)
	{
		index = R3GetIdex((PVOID)HLoadLibraryExA);

		if ((dwFlags & (LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES)) == 0)
		{
			fullname_sputnik = NeedLoadSputnik(lpFileName);
		}

		if(IsNeedR3HookFilter(index))
		{
			MySearchPath(lpFileName,".DLL",&fullname);
			if((ntstatus=HR3Common(index,fullname, NULL,FALSE))!=ERROR_SUCCESS)
				bBlock = TRUE;
		}
	}

	if (bBlock)
	{
		ret = NULL;
		SetLastError(ERROR_ACCESS_DENIED);
	}
	else
	{
		ret=((HANDLE (__stdcall *)(PCHAR,HANDLE,DWORD))(R3HTable[index].Real))(lpFileName,hFile,dwFlags);
	
		if (ret && fullname_sputnik)
		{
			DWORD dwLasterr = GetLastError();

#ifdef _HOOK_NT_
			HookExportsA( fullname_sputnik );
#endif

			((HANDLE (__stdcall *)(PCHAR,HANDLE,DWORD))(R3HTable[index].Real))(fullname_sputnik,hFile, 0);
			SetLastError(dwLasterr);
		}
	}

	if (ret)
	{
		bBlock = CheckDep(&lpFileName, 0, "LoadLibraryExA", lpFileName, FALSE);
		if (bBlock)
		{
			ret = 0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
	}

	if(fullname)
		GlobalFree(fullname);

	return ret;
}

#ifdef _HOOK_NT_

DWORD __stdcall HGetProcAddress(DWORD hModule, LPCSTR lpProcName)
{
	ULONG		index;
	LONG		ntstatus;
	DWORD		ret;

	BOOLEAN bBlock = FALSE;

	bBlock = CheckDep(&hModule, (DWORD) hModule, "GetProcAddress", (PVOID) lpProcName, FALSE);

	if (bBlock)
	{
		ret = 0;
		SetLastError(ERROR_ACCESS_DENIED);
	}
	else
	{
		index=R3GetIdex((PVOID)HGetProcAddress);
		ret=((DWORD (__stdcall *)(DWORD, LPCSTR))(R3HTable[index].Real))(hModule, lpProcName);
	}

	return ret;
}


HANDLE	__stdcall HLoadLibraryExW(PWCHAR lpFileName,HANDLE hFile,DWORD dwFlags)
{
	PWCHAR	fullname=NULL;
	ULONG		index;
	LONG		ntstatus;
	HANDLE	ret;
	PWCHAR	fullname_sputnik = NULL;	

	BOOLEAN bBlock = FALSE;

	bBlock = CheckDep(&lpFileName, 0, "LoadLibraryExW", lpFileName, TRUE);

	if (!bBlock)
	{
		index=R3GetIdex((PVOID)HLoadLibraryExW);

		if ((dwFlags & (LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES)) == 0)
		{
			fullname_sputnik = NeedLoadSputnikW(lpFileName);
		}

		if(IsNeedR3HookFilter(index))
		{
			MySearchPathW(lpFileName,L".DLL",&fullname);
			if((ntstatus=HR3Common(index,fullname, NULL,TRUE))!=ERROR_SUCCESS)
				bBlock = TRUE;
		}
	}

	if (bBlock)
	{
		ret = NULL;
		SetLastError(ERROR_ACCESS_DENIED);
	}
	else
	{
		ret=((HANDLE (__stdcall *)(PWCHAR,HANDLE,DWORD))(R3HTable[index].Real))(lpFileName,hFile,dwFlags);
	
		if (ret && fullname_sputnik)
		{
			DWORD dwLasterr = GetLastError();

			HookExportsW( fullname_sputnik );

			((HANDLE (__stdcall *)(PWCHAR,HANDLE,DWORD))(R3HTable[index].Real))(fullname_sputnik,hFile, 0);
			SetLastError(dwLasterr);
		}
	}

	if (ret)
	{
		bBlock = CheckDep(&lpFileName, (DWORD) ret, "LoadLibraryExW", lpFileName, TRUE);
		if (bBlock)
		{
			ret = 0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
	}


	if(fullname)
		GlobalFree(fullname);

	return ret;
}
#endif

HANDLE __stdcall HCreateFileA(PCHAR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,PVOID lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
HANDLE	ret;
	index=R3GetIdex((PVOID)HCreateFileA);
	if(IsNeedR3HookFilter(index))
	{
		MyGetFullPathName(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS)
		{
			ret=((HANDLE (__stdcall *)(PCHAR,DWORD,DWORD,PVOID,DWORD,DWORD,HANDLE))(R3HTable[index].Real))(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
		}
		else
		{
			ret=INVALID_HANDLE_VALUE;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	}
	else
	{
		ret=((HANDLE (__stdcall *)(PCHAR,DWORD,DWORD,PVOID,DWORD,DWORD,HANDLE))(R3HTable[index].Real))(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
	}
	return ret;
}

#ifdef _HOOK_NT_
HANDLE __stdcall HCreateFileW(PWCHAR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,PVOID lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
HANDLE	ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCreateFileW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((HANDLE (__stdcall *)(PWCHAR,DWORD,DWORD,PVOID,DWORD,DWORD,HANDLE))(R3HTable[index].Real))(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
			//		if(ret!=INVALID_HANDLE_VALUE)
			//			CacheStore((ULONG)ret,fullname);
		} else {
			ret=INVALID_HANDLE_VALUE;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((HANDLE (__stdcall *)(PWCHAR,DWORD,DWORD,PVOID,DWORD,DWORD,HANDLE))(R3HTable[index].Real))(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

HANDLE	__stdcall HOpenFile(LPCSTR lpFileName,PVOID lpReOpenBuff,DWORD uStyle)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
HANDLE	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HOpenFile);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((HANDLE (__stdcall *)(LPCSTR,PVOID,DWORD))(R3HTable[index].Real))(lpFileName,lpReOpenBuff,uStyle);
//		if(ret!=INVALID_HANDLE_VALUE)
//			CacheStore((ULONG)ret,fullname);
		} else {
			ret=INVALID_HANDLE_VALUE;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((HANDLE (__stdcall *)(LPCSTR,PVOID,DWORD))(R3HTable[index].Real))(lpFileName,lpReOpenBuff,uStyle);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

HANDLE	__stdcall H_lopen(LPCSTR lpPathName,DWORD iReadWrite)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
HANDLE	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)H_lopen);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpPathName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((HANDLE (__stdcall *)(LPCSTR,DWORD))(R3HTable[index].Real))(lpPathName,iReadWrite);
//		if(ret!=INVALID_HANDLE_VALUE)
//			CacheStore((ULONG)ret,fullname);
		} else {
			ret=INVALID_HANDLE_VALUE;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((HANDLE (__stdcall *)(LPCSTR,DWORD))(R3HTable[index].Real))(lpPathName,iReadWrite);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

HANDLE	__stdcall H_lcreat(LPCSTR lpPathName,DWORD iAttribute)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
HANDLE	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)H_lcreat);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpPathName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((HANDLE (__stdcall *)(LPCSTR,DWORD))(R3HTable[index].Real))(lpPathName,iAttribute);
//		if(ret!=INVALID_HANDLE_VALUE)
//			CacheStore((ULONG)ret,fullname);
		} else {
			ret=INVALID_HANDLE_VALUE;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((HANDLE (__stdcall *)(LPCSTR,DWORD))(R3HTable[index].Real))(lpPathName,iAttribute);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

BOOL		__stdcall HCopyFileA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,BOOL bFailIfExists)
{
PCHAR		fullname=NULL;
PCHAR		fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCopyFileA);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpExistingFileName,&fullname);
		MyGetFullPathName(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname, fullname2,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,BOOL))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,bFailIfExists);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}	
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,BOOL))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,bFailIfExists);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HCopyFileW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName,BOOL bFailIfExists)
{
PWCHAR	fullname=NULL;
PWCHAR	fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCopyFileW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpExistingFileName,&fullname);
		MyGetFullPathNameW(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname, fullname2,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,BOOL))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,bFailIfExists);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}	
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,BOOL))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,bFailIfExists);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HCopyFileExA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,PVOID lpProgressRoutine,PVOID lpData,PBOOL pbCancel,DWORD dwCopyFlags)
{
PCHAR		fullname=NULL;
PCHAR		fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCopyFileExA);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpExistingFileName,&fullname);
		MyGetFullPathName(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname, fullname2,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,PVOID,PVOID,PBOOL,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,lpProgressRoutine,lpData,pbCancel,dwCopyFlags);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,PVOID,PVOID,PBOOL,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,lpProgressRoutine,lpData,pbCancel,dwCopyFlags);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HCopyFileExW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName,PVOID lpProgressRoutine,PVOID lpData,PBOOL pbCancel,DWORD dwCopyFlags)
{
PWCHAR	fullname=NULL;
PWCHAR	fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HCopyFileExW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpExistingFileName,&fullname);
		MyGetFullPathNameW(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname, fullname2,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,PVOID,PBOOL,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,lpProgressRoutine,lpData,pbCancel,dwCopyFlags);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,PVOID,PBOOL,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,lpProgressRoutine,lpData,pbCancel,dwCopyFlags);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HMoveFileA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName)
{
PCHAR		fullname=NULL;
PCHAR		fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HMoveFileA);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpExistingFileName,&fullname);
		MyGetFullPathName(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname,fullname2,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HMoveFileW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName)
{
PWCHAR	fullname=NULL;
PWCHAR	fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HMoveFileW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpExistingFileName,&fullname);
		MyGetFullPathNameW(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname,fullname2,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HMoveFileExA(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,DWORD dwFlags)
{
PCHAR		fullname=NULL;
PCHAR		fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HMoveFileExA);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpExistingFileName,&fullname);
		MyGetFullPathName(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname,fullname2,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,dwFlags);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,dwFlags);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HMoveFileExW(PWCHAR lpExistingFileName,PWCHAR lpNewFileName,DWORD dwFlags)
{
PWCHAR	fullname=NULL;
PWCHAR	fullname2=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HMoveFileExW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpExistingFileName,&fullname);
		MyGetFullPathNameW(lpNewFileName,&fullname2);
		if((ntstatus=HR3Common(index,fullname,fullname2,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,dwFlags);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
		if(fullname2)
			GlobalFree(fullname2);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,DWORD))(R3HTable[index].Real))(lpExistingFileName,lpNewFileName,dwFlags);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HDeleteFileA(LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HDeleteFileA);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR))(R3HTable[index].Real))(lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR))(R3HTable[index].Real))(lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HDeleteFileW(PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HDeleteFileW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR))(R3HTable[index].Real))(lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR))(R3HTable[index].Real))(lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HSetFileAttributesA(LPCTSTR lpFileName,DWORD dwFileAttributes)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HSetFileAttributesA);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathName(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,DWORD))(R3HTable[index].Real))(lpFileName,dwFileAttributes);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,DWORD))(R3HTable[index].Real))(lpFileName,dwFileAttributes);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HSetFileAttributesW(PWCHAR lpFileName,DWORD dwFileAttributes)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL	ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HSetFileAttributesW);
	if(IsNeedR3HookFilter(index)) {
		MyGetFullPathNameW(lpFileName,&fullname);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,DWORD))(R3HTable[index].Real))(lpFileName,dwFileAttributes);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,DWORD))(R3HTable[index].Real))(lpFileName,dwFileAttributes);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetPrivateProfileIntA(LPCTSTR lpAppName,LPCTSTR lpKeyName,INT nDefault,LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileIntA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,INT,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault,lpFileName);
		} else {
			ret=nDefault;
//			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,INT,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault,lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetPrivateProfileIntW(PWCHAR lpAppName,PWCHAR lpKeyName,INT nDefault,PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileIntW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,INT,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault,lpFileName);
		} else {
			ret=nDefault;
			//			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,INT,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault,lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetProfileIntA(LPCTSTR lpAppName,LPCTSTR lpKeyName,INT nDefault)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetProfileIntA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,INT))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault);
		} else {
			ret=nDefault;
//			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,INT))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetProfileIntW(PWCHAR lpAppName,PWCHAR lpKeyName,INT nDefault)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetProfileIntW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,INT))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault);
		} else {
			ret=nDefault;
			//			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,INT))(R3HTable[index].Real))(lpAppName,lpKeyName,nDefault);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HWritePrivateProfileSectionA(LPCTSTR lpAppName,LPCTSTR lpString,LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWritePrivateProfileSectionA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpString,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpString,lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HWritePrivateProfileSectionW(PWCHAR lpAppName,PWCHAR lpString,PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWritePrivateProfileSectionW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpString,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpString,lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetPrivateProfileSectionA(LPCTSTR lpAppName,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileSectionA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCTSTR,LPTSTR,DWORD,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCTSTR,LPTSTR,DWORD,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize,lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetPrivateProfileSectionW(PWCHAR lpAppName,PWCHAR lpReturnedString,DWORD nSize,PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileSectionW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,DWORD,PWCHAR))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,DWORD,PWCHAR))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize,lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HWriteProfileSectionA(LPCTSTR lpAppName,LPCTSTR lpString)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWriteProfileSectionA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpString);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpString);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HWriteProfileSectionW(PWCHAR lpAppName,PWCHAR lpString)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWriteProfileSectionW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpString);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpString);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetProfileSectionA(LPCTSTR lpAppName,LPTSTR lpReturnedString,DWORD nSize)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetProfileSectionA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCTSTR,LPTSTR,DWORD))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCTSTR,LPTSTR,DWORD))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetProfileSectionW(PWCHAR lpAppName,PWCHAR lpReturnedString,DWORD nSize)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetProfileSectionW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,DWORD))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,DWORD))(R3HTable[index].Real))(lpAppName,lpReturnedString,nSize);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetPrivateProfileSectionNamesA(LPTSTR lpszReturnBuffer,DWORD nSize,LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileSectionNamesA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(lpFileName?lpFileName:WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPTSTR,DWORD,LPCTSTR))(R3HTable[index].Real))(lpszReturnBuffer,nSize,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPTSTR,DWORD,LPCTSTR))(R3HTable[index].Real))(lpszReturnBuffer,nSize,lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetPrivateProfileSectionNamesW(PWCHAR lpszReturnBuffer,DWORD nSize,PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileSectionNamesW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(lpFileName?lpFileName:WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,DWORD,PWCHAR))(R3HTable[index].Real))(lpszReturnBuffer,nSize,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,DWORD,PWCHAR))(R3HTable[index].Real))(lpszReturnBuffer,nSize,lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HWritePrivateProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString,LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWritePrivateProfileStringA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString,lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HWritePrivateProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpString,PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWritePrivateProfileStringW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString,lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetPrivateProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize,LPCTSTR lpFileName)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileStringA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR,LPTSTR,DWORD,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR,LPTSTR,DWORD,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetPrivateProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpDefault,PWCHAR lpReturnedString,DWORD nSize,PWCHAR lpFileName)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileStringW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(lpFileName);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,PWCHAR,PWCHAR,DWORD,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,PWCHAR,PWCHAR,DWORD,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HWriteProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpString)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWriteProfileStringA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HWriteProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpString)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWriteProfileStringW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PWCHAR))(R3HTable[index].Real))(lpAppName,lpKeyName,lpString);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

DWORD		__stdcall HGetProfileStringA(LPCTSTR lpAppName,LPCTSTR lpKeyName,LPCTSTR lpDefault,LPTSTR lpReturnedString,DWORD nSize)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetProfileStringA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR,LPTSTR,DWORD))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(LPCTSTR,LPCTSTR,LPCTSTR,LPTSTR,DWORD))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
DWORD		__stdcall HGetProfileStringW(PWCHAR lpAppName,PWCHAR lpKeyName,PWCHAR lpDefault,PWCHAR lpReturnedString,DWORD nSize)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
DWORD		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetProfileStringW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,PWCHAR,PWCHAR,DWORD))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((DWORD (__stdcall *)(PWCHAR,PWCHAR,PWCHAR,PWCHAR,DWORD))(R3HTable[index].Real))(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HWritePrivateProfileStructA(LPCTSTR lpszSection,LPCTSTR lpszKey,PVOID lpStruct,UINT uSizeStruct,LPCTSTR szFile)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWritePrivateProfileStructA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(szFile?szFile:WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,PVOID,UINT,LPCTSTR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,PVOID,UINT,LPCTSTR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HWritePrivateProfileStructW(PWCHAR lpszSection,PWCHAR lpszKey,PVOID lpStruct,UINT uSizeStruct,PWCHAR szFile)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HWritePrivateProfileStructW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(szFile?szFile:WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,UINT,PWCHAR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,UINT,PWCHAR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

BOOL		__stdcall HGetPrivateProfileStructA(LPCTSTR lpszSection,LPCTSTR lpszKey,PVOID lpStruct,UINT uSizeStruct,LPCTSTR szFile)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileStructA);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDir(szFile?szFile:WinIniStr);
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,PVOID,UINT,LPCTSTR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(LPCTSTR,LPCTSTR,PVOID,UINT,LPCTSTR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
	}
//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
BOOL		__stdcall HGetPrivateProfileStructW(PWCHAR lpszSection,PWCHAR lpszKey,PVOID lpStruct,UINT uSizeStruct,PWCHAR szFile)
{
PWCHAR	fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HGetPrivateProfileStructW);
	if(IsNeedR3HookFilter(index)) {
		fullname=AppendWinDirW(szFile?szFile:WinIniStrW);
		if((ntstatus=HR3Common(index,fullname, NULL,TRUE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,UINT,PWCHAR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(PWCHAR,PWCHAR,PVOID,UINT,PWCHAR))(R3HTable[index].Real))(lpszSection,lpszKey,lpStruct,uSizeStruct,szFile);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}
#endif

#define HANDLE_TABLE_OFFSET 0x44
#define ENVIR_DATABASE_OFFSET 0x40
#define CMD_LINE_OFFSET	8
#define K32OBJ_PROCESS 6
#include <pshpack1.h>
typedef struct _HANDLE_TABLE_ENTRY {
	DWORD Flags;
	DWORD pK32Obj;
}HANDLE_TABLE_ENTRY,*PHANDLE_TABLE_ENTRY;
typedef struct _HANDLE_TABLE {
	DWORD cEntries;
	HANDLE_TABLE_ENTRY Entry[1];
}HANDLE_TABLE,*PHANDLE_TABLE;
#include <poppack.h>

BOOL 		__stdcall HTerminateProcess(HANDLE hProcess,UINT uExitCode)
{
PCHAR		fullname=NULL;
ULONG		index;
LONG		ntstatus;
BOOL		ret;
PVOID		pTrgPDB;
PVOID		pCurPDB;
PVOID		pEDB;
PHANDLE_TABLE		HandleTable;
DWORD		HTIndex;
PCHAR		CommandLine;
	//		InterlockedIncrement(&InSysCnt);
	index=R3GetIdex((PVOID)HTerminateProcess);
	if(IsNeedR3HookFilter(index)) {
#ifdef _HOOK_VXD_
		if(Obsfucator) {
		//Пляска с бубном
			pCurPDB=(PVOID)(GetCurrentProcessId() ^ Obsfucator);
			if((ULONG)hProcess==0x7FFFFFFF) { // This is a suicide
				pTrgPDB=pCurPDB;
			} else {
				HandleTable = *(PHANDLE_TABLE*)((PBYTE)pCurPDB + HANDLE_TABLE_OFFSET);
				HTIndex=(ULONG)hProcess/4;
				if(hProcess && HandleTable && HTIndex<=HandleTable->cEntries) {
					pTrgPDB=(PVOID)(HandleTable->Entry[HTIndex].pK32Obj);
					if(pTrgPDB && *(WORD*)pTrgPDB == K32OBJ_PROCESS) {
						pEDB=*(PVOID*)((BYTE*)pTrgPDB+ENVIR_DATABASE_OFFSET);
						if(pEDB) {
							CommandLine=*(PCHAR*)((BYTE*)pEDB + CMD_LINE_OFFSET);
							fullname=GetExecNameFromCommandLine(CommandLine);
						} else {
							R3DbPrint((DL_WARNING, "HTerminateProcess: something wrong with target EDB calculation\n"));
							DbgBreakPoint();
						}
					} else {
						R3DbPrint((DL_WARNING, "HTerminateProcess: something wrong with target PDB calculation\n"));
						DbgBreakPoint();
					}
				} else {
					R3DbPrint((DL_INFO, "HTerminateProcess with invalid hProc %x\n",hProcess));
				}
			}
		}
#endif //_HOOK_VXD_
		if((ntstatus=HR3Common(index,fullname, NULL,FALSE))==ERROR_SUCCESS) {
			ret=((BOOL (__stdcall *)(HANDLE ,UINT))(R3HTable[index].Real))(hProcess,uExitCode);
		} else {
			ret=0;
			SetLastError(ERROR_ACCESS_DENIED);
		}
		if(fullname)
			GlobalFree(fullname);
	} else {
		ret=((BOOL (__stdcall *)(HANDLE ,UINT))(R3HTable[index].Real))(hProcess,uExitCode);
	}
	//	InterlockedDecrement(&InSysCnt);
	return ret;
}

#ifdef _HOOK_NT_
#pragma bss_seg()
#pragma const_seg()
#pragma data_seg()
#pragma code_seg()

#endif

//------------- R0 Part  --------------------------------------------------------------------------
#ifdef _HOOK_VXD_
PVOID GetKernel32Base(VOID)
{
	DWORD a;
	for(a=0xbff70000;a>0xbff00000;a-=0x1000) 
		if(*(WORD*)a=='ZM') 
			return (PVOID)a;
		return 0;
}
#endif

#include <pshpack1.h>
typedef struct _R3HSTUB {
//	BYTE  I3Instr;
#ifdef _HOOK_NT_
	BYTE	Nops[5];
#endif	
	BYTE  JmpInstr;
	ULONG	JmpAddr;
	BYTE	align[10];
}R3HSTUB,*PR3HSTUB;
#include <poppack.h>

BOOLEAN IsAllZeroes(PVOID Addr,int Size)
{
	ULONG *Pos;
	Pos=(ULONG*)((BYTE*)Addr+Size-sizeof(ULONG));
	while((ULONG)Pos > (ULONG)Addr) {
		if(*Pos!=0)
			return FALSE;
		Pos--;
	}
	return TRUE;
}

#ifdef _HOOK_NT_

BOOLEAN MyNtProtectVirtualMemory(PVOID VAddr,ULONG Size,ULONG Protect, PULONG pOldProtect)
{
BOOLEAN ret;
PVOID Addr;
ULONG size;
	ret=FALSE;
	Addr=VAddr;
	size=Size;
	
	if(NT_SUCCESS(NTSysCall5((enum AVPG_W32_Services)ZwProtectVirtualMemoryNativeId,(ULONG)NtCurrentProcess(),(ULONG)&Addr,(ULONG)&Size,Protect,(ULONG)pOldProtect)))
		ret=TRUE;
	return ret;
}
#endif

VOID* PatchTimeStampAndGetPlace4Stub(PVOID hModule,ULONG SizeOfSpace4Stubs)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER SectHeader;
	ULONG SectNum;
	PVOID StubsArea;
	ULONG SecSize;

	ULONG OldProtect;
	ULONG OldProtect2;
	
	pDosHeader=(PIMAGE_DOS_HEADER)hModule;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DbPrint(DC_R3,DL_ERROR, ("mz header not found\n"));
		return NULL;
	}
	pNTHeader=MakePtr(PIMAGE_NT_HEADERS,hModule,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		DbPrint(DC_R3,DL_ERROR, ("PE header not found\n"));
		return NULL;
	}
#ifdef _HOOK_NT_
	if(!MyNtProtectVirtualMemory(pNTHeader,sizeof(ULONG)+sizeof(IMAGE_FILE_HEADER)+pNTHeader->FileHeader.SizeOfOptionalHeader+pNTHeader->FileHeader.NumberOfSections*sizeof(IMAGE_SECTION_HEADER),PAGE_EXECUTE_READWRITE/*PAGE_WRITECOPY*/, &OldProtect))
		return NULL;
#endif
	pNTHeader->FileHeader.TimeDateStamp++; //!!!
#ifdef _HOOK_VXD_
	if(!LinPageLock((DWORD)hModule>>12,1,0))
	{
		DbPrint(DC_R3,DL_ERROR, ("LinPageLock for kernel header failed\n"));
	}
#endif
	SectHeader=IMAGE_FIRST_SECTION(pNTHeader);
	for(SectNum=0;SectNum<pNTHeader->FileHeader.NumberOfSections;SectNum++)
	{
		SecSize=SectHeader->SizeOfRawData;
		if((SecSize & 0xfff) != 0)
		{
			SecSize |= 0xfff;
			SecSize++;						
		}
		if((int)SecSize - (int)SectHeader->Misc.VirtualSize > (int)SizeOfSpace4Stubs)
		{
			StubsArea=(BYTE*)hModule+SectHeader->VirtualAddress+SecSize-SizeOfSpace4Stubs;
			if(IsAllZeroes(StubsArea,SizeOfSpace4Stubs) && *(ULONG*)((PCHAR)SectHeader->Name+1)!='TINI')
			{
#ifdef _HOOK_NT_
				if(MyNtProtectVirtualMemory(StubsArea,SizeOfSpace4Stubs,PAGE_EXECUTE_READWRITE/*PAGE_EXECUTE_WRITECOPY*/, &OldProtect2))
#endif
				{
					SectHeader->SizeOfRawData=SecSize;//!!!
					SectHeader->Misc.VirtualSize+=SizeOfSpace4Stubs;//!!!
					return StubsArea;
				}
			}
		}
		SectHeader++;
	}

	return NULL;
}

BOOLEAN R3InitK32ApiInt(PVOID Kernel32Base)
{
#ifdef __DBG__
	if(!(OutputDebugString=GetExport(Kernel32Base,"OutputDebugStringA",NULL)))
		return FALSE;
	if(!(GetModuleFileName=GetExport(Kernel32Base,"GetModuleFileNameA",NULL)))
		return FALSE;
	if(!(GetCurrentThreadId=GetExport(Kernel32Base,"GetCurrentThreadId",NULL)))
		return FALSE;
#endif // __DBG__
#ifdef _HOOK_VXD_
	if(!(SetLastError=GetExport(Kernel32Base,"SetLastError",NULL)))
		return FALSE;
	if(!(GetLastError=GetExport(Kernel32Base,"GetLastError",NULL)))
		return FALSE;
	if(!(OemToChar=GetExport(Kernel32Base,(PCHAR)12,NULL))) ///!!!!!!!!!!!!!!!
		return FALSE;
	if(!(GetCurrentProcessId=GetExport(Kernel32Base,"GetCurrentProcessId",NULL)))
		return FALSE;
	if(!(R3HeapAlloc=GetExport(Kernel32Base,"HeapAlloc",NULL)))
		return FALSE;
	if(!(R3HeapFree=GetExport(Kernel32Base,"HeapFree",NULL)))
		return FALSE;
	if(!(GetProcessHeap=GetExport(Kernel32Base,"GetProcessHeap",NULL)))
		return FALSE;
#else //Nt
	if(*NtBuildNumber<=2195) { //Win2k
		if(!(SetLastError=GetExport(Kernel32Base,"SetLastError",NULL)))
			return FALSE;
		if(!(GetLastError=GetExport(Kernel32Base,"GetLastError",NULL)))
			return FALSE;

	}
	if(!(GetFullPathNameW=GetExport(Kernel32Base,"GetFullPathNameW",NULL)))
		return FALSE;
	if(!(SearchPathW=GetExport(Kernel32Base,"SearchPathW",NULL)))
		return FALSE;
	if(!(GetWindowsDirectoryW=GetExport(Kernel32Base,"GetWindowsDirectoryW",NULL)))
		return FALSE;

	GetProcessHeaps = GetExport(Kernel32Base,"GetProcessHeaps", NULL);

	if(!(GlobalAlloc=GetExport(Kernel32Base,"GlobalAlloc",NULL)))
		return FALSE;
	if(!(GlobalFree=GetExport(Kernel32Base,"GlobalFree",NULL)))
		return FALSE;
	if(!(VirtualQuery=GetExport(Kernel32Base,"VirtualQuery",NULL)))
		return FALSE;
#endif
	if(!(DeviceIoControl=GetExport(Kernel32Base,"DeviceIoControl",NULL)))
		return FALSE;
	if(!(AreFileApisANSI=GetExport(Kernel32Base,"AreFileApisANSI",NULL)))
		return FALSE;
	if(!(CreateFile=GetExport(Kernel32Base,"CreateFileA",NULL)))
		return FALSE;
	if(!(CloseHandle=GetExport(Kernel32Base,"CloseHandle",NULL)))
		return FALSE;
	if(!((DWORD*)GetFullPathName=GetExport(Kernel32Base,"GetFullPathNameA",NULL)))
		return FALSE;
	if(!(SearchPath=GetExport(Kernel32Base,"SearchPathA",NULL)))
		return FALSE;
	if(!(GetWindowsDirectory=GetExport(Kernel32Base,"GetWindowsDirectoryA",NULL)))
		return FALSE;

	R3K32ApiInited=TRUE;
	DbPrint(DC_R3,DL_IMPORTANT, ("R3InitK32Api ok\n"));
	return TRUE;
}

BOOLEAN R3InitK32Api(PVOID Kernel32Base)
{
	BOOLEAN bret;

	if(R3K32ApiInited)
		return TRUE;

	AcquireResource(g_pR3Mutex, TRUE);

	EnableWriteToSections(TRUE);

	bret = R3InitK32ApiInt(Kernel32Base);

	EnableWriteToSections(FALSE);

	ReleaseResource(g_pR3Mutex);

	return bret;
}


BOOLEAN R3HookDllImp(PVOID DllBase)
{
DWORD ExportAddr;
PVOID ExportFunc;
PR3HSTUB StubsArea;
ULONG SizeOfSpace4Stubs;
ULONG i;
	ULONG OldProtect;
	SizeOfSpace4Stubs=sizeof(R3HTable)/sizeof(R3HOOK)*sizeof(R3HSTUB);
	StubsArea=PatchTimeStampAndGetPlace4Stub(DllBase,SizeOfSpace4Stubs);
	if(StubsArea==NULL) {
		DbPrint(DC_R3,DL_ERROR, ("PatchTimeStampAndGetPlace4Stub failed\n"));
		return FALSE;
	}
	for(i=0;i<sizeof(R3HTable)/sizeof(R3HOOK);i++)
	{
		if(ExportFunc=GetExport(DllBase,R3HTable[i].FuncName,&ExportAddr))
		{
#ifdef _HOOK_NT_
			if(MyNtProtectVirtualMemory((PVOID)ExportAddr,sizeof(ULONG),PAGE_EXECUTE_READWRITE/*PAGE_EXECUTE_WRITECOPY*/, &OldProtect))
#endif
			{
				if (!R3HTable[i].Real)
					R3HTable[i].Real=ExportFunc;

				memset(&StubsArea[i],0x90,sizeof(R3HSTUB));//!!!

#ifdef _HOOK_NT_
				StubsArea[i].Nops[0] = 0x55; //push ebp
				StubsArea[i].Nops[1] = 0x8b; //mov ebp, esp
				StubsArea[i].Nops[2] = 0xec;
				StubsArea[i].Nops[3] = 0x90;
				StubsArea[i].Nops[4] = 0x5d;// pop ebp
#endif	
				//				StubsArea[i].I3Instr=0xcc;
				StubsArea[i].JmpInstr=0xe9;
				StubsArea[i].JmpAddr=(DWORD)(R3HTable[i].Hook) - (DWORD)(&StubsArea[i].align);
				
				*(DWORD*)ExportAddr=(DWORD)(&StubsArea[i])-(DWORD)DllBase;//!!!
				//				*(DWORD*)ExportAddr=(DWORD)(R3HTable[i].Hook)-(DWORD)DllBase;// old variant without stubs
				DbPrint(DC_R3,DL_SPAM, ("%s (%x/%x/%x) hooked\n",R3HTable[i].FuncName, ExportAddr,&StubsArea[i].JmpInstr,R3HTable[i].Real));
				continue;
			}
		} 
		DbPrint(DC_R3,DL_ERROR, ("!!! %s (%x/%x) hook failed\n",R3HTable[i].FuncName, ExportAddr, R3HTable[i].Real));
	}
	return TRUE;
}

BOOLEAN R3HookDll(PVOID DllBase)
{
	BOOLEAN bret;

	AcquireResource(g_pR3Mutex, TRUE);

	EnableWriteToSections(TRUE);

	bret = R3HookDllImp(DllBase);

	EnableWriteToSections(FALSE);

	ReleaseResource(g_pR3Mutex);

	return bret;
}

ULONG	UserSecAddr;
ULONG	UserSecEnd;
void EnableWriteToSections(BOOL bEnable)
{
#ifdef _HOOK_VXD_
#else
	ULONG Va;
	ULONG *Pt;
	for(Va=UserSecAddr & (~(PAGE_SIZE-1));Va<UserSecEnd;Va+=PAGE_SIZE)
	{
		Pt = (ULONG*) GetPageDescriptorNativeLite((PVOID)Va);
		if (bEnable)
		{
			__asm
			{
				mov eax,[Pt];
				lock or dword ptr [eax],2; // RW
				mov eax,[Va];
				invlpg [eax];
			}
		}
		else
		{
			__asm
			{
				mov eax,[Pt];
				lock and dword ptr [eax],0xFFFFFFFD;// RO
				mov eax,[Va];
				invlpg [eax];
			}
		}
	}
#endif
}
	
#ifdef _HOOK_NT_

VOID R3HApcNormal(PVOID NormalContext,PVOID SystemArgument1,PVOID SystemArgument2)
{
	ULONG *Pd;
	ULONG *Pt;
	ULONG Va;
	DbPrint(DC_R3,DL_SPAM, ("R3HApcNormal\n"));

	R3HookDll(NormalContext);

	for(Va=UserSecAddr & (~(PAGE_SIZE-1));Va<UserSecEnd;Va+=PAGE_SIZE)
	{
		Pd = (ULONG*) GetPDEEntryNativeLite((PVOID)Va);
		Pt = (ULONG*) GetPageDescriptorNativeLite((PVOID)Va);
		__asm {
			mov eax,[Pd];
			lock or dword ptr [eax],4;//user addressable 
			lock and dword ptr [eax],0xFFFFFEFF;// 100h-Global
			mov eax,[Pt];
			invlpg [eax];
			lock or dword ptr [eax],4;//user addressable 
			lock and dword ptr [eax],0xFFFFFEFD;// 100h-Global
			mov eax,[Va];
			invlpg [eax];
		}
	}
}

VOID R3HApcKernel(PKAPC Apc,PKNORMAL_ROUTINE *NormalRoutine,PVOID *NormalContext,PVOID *SystemArgument1,PVOID *SystemArgument2)
{
	DbPrint(DC_R3,DL_SPAM, ("R3HApcKernel\n"));
	ExFreePool(Apc);
}
	
VOID R3HApcRundown(PKAPC Apc)
{
	DbPrint(DC_R3,DL_SPAM, ("R3HApcRundown\n"));
	ExFreePool(Apc);
}

BOOLEAN R3Hook(PWCHAR DllName,PVOID DllBase)
{
	PKAPC Apc;
	if(!R3Hooked)
		return FALSE;
	if(_wcsicmp(DllName,L"kernel32.dll")==0)
	{
		if(!R3K32ApiInited)
		{
			if(!R3InitK32Api(DllBase))
			{
				DbPrint(DC_R3,DL_ERROR, ("R3InitK32Api failed\n"));
				return FALSE;
			}
		}
		
		Apc=ExAllocatePoolWithTag(NonPagedPool,sizeof(KAPC),'3SeB');
		if(Apc)
		{
			KeInitializeApc(Apc,PsGetCurrentThread(),0,R3HApcKernel,R3HApcRundown,R3HApcNormal,KernelMode,DllBase);
			if(!_pfKeInsertQueueApc(Apc,NULL,NULL,KernelMode))
			{
				DbPrint(DC_R3,DL_ERROR, ("KeInsertQueueApc fail\n"));
				DbgBreakPoint();
				ExFreePool(Apc);
				return FALSE;
			}
			else
			{
	#ifdef __DBG__
			{
				CHAR ProcName[PROCNAMELEN];
				DbPrint(DC_R3,DL_NOTIFY, ("%s R3HookDll %S at %x\n", GetProcName(ProcName,NULL),DllName,DllBase));
			}
	#endif
			return TRUE;
			}
		}
	}
	return FALSE;
}

BOOLEAN
IsR3Disabled()
{
#ifdef _HOOK_NT_
	BOOLEAN bDisabled = FALSE;

	HKEY				ParKeyHandle;
	UNICODE_STRING		ValueName;
	PKEY_VALUE_PARTIAL_INFORMATION pinfo = NULL;
	OBJECT_ATTRIBUTES	objectAttributes;

	NTSTATUS ntstatus;
	
	DWORD dwsize = sizeof(KEY_VALUE_PARTIAL_INFORMATION) + 32;

	if((pinfo = ExAllocatePoolWithTag(PagedPool, dwsize,'.BOS')))
	{
		InitializeObjectAttributes(&objectAttributes, &RegParams, OBJ_CASE_INSENSITIVE,NULL, NULL);
		ntstatus = ZwOpenKey(&ParKeyHandle, KEY_READ, &objectAttributes);
		if(NT_SUCCESS(ntstatus))
		{
			ULONG TmpLen = 0;
			RtlInitUnicodeString(&ValueName, L"R3CompatMode");
			ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
							pinfo, dwsize, &TmpLen);
			
			if(NT_SUCCESS(ntstatus) && pinfo->DataLength == sizeof(DWORD))
			{
				PULONG pTmp = (PULONG) pinfo->Data;
				if (*pTmp)
					bDisabled = TRUE;
			}

			ZwClose(ParKeyHandle);
		}

		ExFreePool(pinfo);
	}

	return bDisabled;

#else // w9x
	return FALSE;
#endif
}

// located in "INIT" section, look for #pragma alloc_text("INIT",...)
BOOLEAN R3HookInit(VOID)
{
	ULONG MyBaseAddr;
	BOOLEAN bFound;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER SectHeader;
	ULONG SectNum;

	g_pR3Mutex = InitKlgResourse();
	if (g_pR3Mutex== NULL)
	{
		DbPrint(DC_FILTER, DL_ERROR, ("Can't init g_pR3Mutex\n"));
		return FALSE;
	}
	
	if (IsR3Disabled())
	{
		DbPrint(DC_FILTER, DL_ERROR, ("R3Hook disabled\n"));
		return FALSE;
	}

	bFound=FALSE;
	for(MyBaseAddr=(ULONG)R3HookInit & (~(PAGE_SIZE-1)); MyBaseAddr>0x80000000;MyBaseAddr-=PAGE_SIZE)	{
		if(*(WORD*)MyBaseAddr=='ZM') {
			bFound=TRUE;
			break;
		}
	}
	if(!bFound) {
		DbPrint(DC_R3,DL_ERROR, ("R3HookInit: GetMyBaseAddr failed\n"));
		return FALSE;
	}
	pDosHeader=(PIMAGE_DOS_HEADER)MyBaseAddr;
	pNTHeader=MakePtr(PIMAGE_NT_HEADERS,MyBaseAddr,pDosHeader->e_lfanew);

	__try
	{
		if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		{
			DbPrint(DC_R3,DL_ERROR, ("R3HookInit: PE header not found\n"));
			return FALSE;
		}
	}
	
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbPrint(DC_R3,DL_ERROR, ("R3HookInit: bad PE header\n"));
		return FALSE;
	}

	SectHeader=IMAGE_FIRST_SECTION(pNTHeader);
	bFound=FALSE;
	for(SectNum=0;SectNum<pNTHeader->FileHeader.NumberOfSections;SectNum++) {
		if(*(ULONG*)((PCHAR)SectHeader->Name+1)=='resu') {
			bFound=TRUE;
			break;
		}
		SectHeader++;
	}
	if(!bFound) {
		DbPrint(DC_R3,DL_ERROR, ("R3HookInit: Section \".user\" not found\n"));
		return FALSE;
	}
	UserSecAddr=MyBaseAddr+SectHeader->VirtualAddress;
	UserSecEnd=UserSecAddr+SectHeader->SizeOfRawData;

#ifdef __DBG__
	if(!(NTsprintf=GetExport(BaseOfNtDllDll,"sprintf",NULL)))
		return FALSE;
	if(!(NTvsprintf=GetExport(BaseOfNtDllDll,"vsprintf",NULL)))
		return FALSE;
#endif // __DBG__
	if(*NtBuildNumber>2195)
	{ //WinXP
		if(!(SetLastError=GetExport(BaseOfNtDllDll,"RtlSetLastWin32Error",NULL)))
			return FALSE;
		if(!(GetLastError=GetExport(BaseOfNtDllDll,"RtlGetLastWin32Error",NULL)))
			return FALSE;
	} 
	if(!(R3RtlOemToUnicodeN=GetExport(BaseOfNtDllDll,"RtlOemToUnicodeN",NULL)))
		return FALSE;

	if(!(R3RtlUpcaseUnicodeChar = GetExport(BaseOfNtDllDll,"RtlUpcaseUnicodeChar",NULL)))
		return FALSE;
	if(!(R3wcslen=GetExport(BaseOfNtDllDll,"wcslen",NULL)))
		return FALSE;
	if(!(R3wcscpy=GetExport(BaseOfNtDllDll,"wcscpy",NULL)))
		return FALSE;

	R3Hooked=TRUE;
	DbPrint(DC_R3,DL_IMPORTANT, ("R3Hook inited\n"));

	DbPrint(DC_R3, DL_IMPORTANT, ("loading sputniks\n"));
	LoadSputniks();
	DbPrint(DC_R3, DL_IMPORTANT, ("sputniks loaded\n"));

	return TRUE;
}
#endif

#ifdef _HOOK_VXD_
BOOLEAN R3HookInit(VOID)
{
	DbPrint(DC_R3, DL_IMPORTANT, ("loading sputniks\n"));

	g_pR3Mutex = InitKlgResourse();
	if (g_pR3Mutex== NULL)
	{
		DbPrint(DC_FILTER, DL_ERROR, ("Can't init g_pR3Mutex\n"));
		return FALSE;
	}

	LoadSputniks();
	DbPrint(DC_R3, DL_IMPORTANT, ("sputniks loaded\n"));

	return TRUE;
}

PVOID Kernel32Base=NULL;
BOOLEAN R3Hook(VOID)
{
	if(!R3Hooked) {
		if(!(Kernel32Base=GetKernel32Base())) {
			DbPrint(DC_R3,DL_ERROR, ("GetKernel32Base failed\n"));
			return FALSE;
		}
		if(SysCallsInited && (VxDCall1=GetExport(Kernel32Base,(PCHAR)1,NULL))){
			VxdCallsInited=TRUE;
		} else {
			DbPrint(DC_R3,DL_WARNING, ("VxdCalls is not inited!!!\n"));
			return FALSE;
		}
		if(!R3InitK32Api(Kernel32Base)) {
			DbPrint(DC_R3,DL_ERROR, ("R3InitK32Api failed\n"));
			return FALSE;
		}
		if(!R3HookDll(Kernel32Base)){
			DbPrint(DC_R3,DL_ERROR, ("R3HookDll failed\n"));
			return FALSE;
		}
		R3Hooked=TRUE;
		DbPrint(DC_R3,DL_IMPORTANT, ("R3 Hooked\n"));
	}
	return TRUE;
}

/*BOOLEAN R3UnHook(VOID)
{
ULONG i;
	if(R3Hooked) {
		for(i=0;i<sizeof(R3HTable)/sizeof(R3HOOK);i++) {
			if(R3HTable[i].Real) {
				*(DWORD*)(R3HTable[i].ExportAddr)=(ULONG)R3HTable[i].Real-(DWORD)Kernel32Base;
				DbPrint(DC_R3,DL_NOTIFY, ("%s (%x/%x) unhooked\n",R3HTable[i].FuncName,R3HTable[i].ExportAddr,R3HTable[i].Real));
			}
		}
		R3Hooked=FALSE;
		DbPrint(DC_R3,DL_IMPORTANT, ("R3 UnHooked\n"));
	}
	return TRUE;
}*/
#endif


/*
LoadLibrary:
загружаем оригинальное - то что пришло
сравниваем со списком из реестра (на старте) - полный путь case insensetive или имя файла
если ОНО - то грузим соответствующий модуль по полному пути
*/


PCHAR CheckString(PCHAR pBase, int Len, int *ResLen, BOOLEAN bUni)
{
	int cou;
	*ResLen = 0;
	if (Len == 0)
		return NULL;

	for (cou = 0; cou < Len; cou++)
	{
		WCHAR symbol;
		(*ResLen)++;
#ifdef _HOOK_NT_
		if (bUni)
			symbol = ((PWCHAR)pBase)[cou];
		else
#endif
			symbol = ((PCHAR)pBase)[cou];
		if (symbol == 0)
		{
			if (*ResLen >= _SPU_PATH_MAX_LEN)
				return NULL;
			return pBase;
		}
	}

	return NULL;
}

void ParseSputniks(BYTE* Ptr, ULONG Len, BOOLEAN bUni)
{
	PCHAR MultiZ;
	PCHAR OrigName;
	PCHAR Spuntik;

	int pos = 0;
	int idx = 0;
	int tmplen;

	MultiZ = (PCHAR) Ptr;

	if (bUni)
		Len = Len / sizeof(WCHAR);

	while (idx < _SPU_SIZE)
	{
		OrigName = (PCHAR) CheckString((BYTE*) MultiZ, Len, &tmplen, bUni);
		if (!OrigName)
			break;
		Len -= tmplen;
		if (bUni)
			MultiZ += tmplen * sizeof(WCHAR);
		else
			MultiZ += tmplen;
		Spuntik = (PCHAR) CheckString((BYTE*) MultiZ, Len, &tmplen, bUni);
		if (!Spuntik)
			break;

		Len -= tmplen;
		if (bUni)
			MultiZ += tmplen * sizeof(WCHAR);
		else
			MultiZ += tmplen;

#ifdef _HOOK_NT_
		if (bUni)
		{
			R3wcscpy((PWCHAR) gSputnikW[idx].m_Module, (PWCHAR) OrigName);
			R3wcscpy((PWCHAR) gSputnikW[idx].m_Sputnik, (PWCHAR) Spuntik);
			gSputnikW[idx].m_Len = R3wcslen((PWCHAR) gSputnikW[idx].m_Module);
		}
		else
#endif
		{
			strcpy(gSputnik[idx].m_Module, OrigName);
			strcpy(gSputnik[idx].m_Sputnik, Spuntik);
			gSputnik[idx].m_Len = strlen(gSputnik[idx].m_Module);
		}
		
		idx++;
	}
}

void LoadSputniks()
{
	NTSTATUS ntstatus;
	ULONG TmpLen;

	if (!R3Hooked)
		return;

	AcquireResource(g_pR3Mutex, TRUE);

	EnableWriteToSections(TRUE);

	memset(gSputnik, 0, sizeof(gSputnik));

#ifdef _HOOK_NT_
	memset(gSputnikW, 0, sizeof(gSputnikW));
	{
		HKEY				ParKeyHandle;
		UNICODE_STRING		ValueName;
		PKEY_VALUE_PARTIAL_INFORMATION pinfo = NULL;
		OBJECT_ATTRIBUTES	objectAttributes;
		if((pinfo = ExAllocatePoolWithTag(PagedPool, sizeof(gSputnikW) + sizeof(KEY_VALUE_PARTIAL_INFORMATION),'.BOS')))
		{
			InitializeObjectAttributes(&objectAttributes, &RegParams, OBJ_CASE_INSENSITIVE,NULL, NULL);
			if((ntstatus = ZwOpenKey(&ParKeyHandle, KEY_READ, &objectAttributes)) == STATUS_SUCCESS)
			{
				RtlInitUnicodeString(&ValueName, L"SpuW");
				TmpLen = 0;
				if(STATUS_SUCCESS == (ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
					pinfo, sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(gSputnikW), &TmpLen)))
				{
					//! parseW values to array
					ParseSputniks(pinfo->Data, pinfo->DataLength, TRUE);
				}

				RtlInitUnicodeString(&ValueName, L"SpuA");
				TmpLen = 0;
				if(STATUS_SUCCESS == (ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
					pinfo, sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(gSputnik), &TmpLen)))
				{
					ParseSputniks(pinfo->Data, pinfo->DataLength, FALSE);
				}

				ZwClose(ParKeyHandle);
			}
			ExFreePool(pinfo);
		}
	}
#else
	{
		void* ptr;
		HKEY KeyHandle;

		ptr = ExAllocatePoolWithTag(PagedPool, sizeof(gSputnik),'.BOS');
		if (ptr)
		{
			if((ntstatus = RegCreateKey(HKEY_LOCAL_MACHINE, RegParams, &KeyHandle)) == ERROR_SUCCESS)
			{
				TmpLen = sizeof(gSputnik);
				if((ntstatus = RegQueryValueEx(KeyHandle, "SpuA", NULL, NULL, ptr, &TmpLen)) == ERROR_SUCCESS)
				{
					ParseSputniks(ptr, TmpLen, FALSE);
				}

				RegCloseKey(KeyHandle);
			}

			ExFreePool(ptr);
		}
	}
#endif // _HOOK_NT_

	EnableWriteToSections(FALSE);

	ReleaseResource(g_pR3Mutex);
}
