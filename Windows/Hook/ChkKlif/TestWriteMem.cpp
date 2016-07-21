// TestWriteMem.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>

#ifndef _SYSTEM_MODULE_INFORMATION_DEFINED
#define _SYSTEM_MODULE_INFORMATION_DEFINED

#include <pshpack1.h>
typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG		Reserved[2];
	PVOID		Base;
	ULONG		Size;
	ULONG		Flags;
	USHORT		Index;
	USHORT		Unknown;
	USHORT		LoadCount;
	USHORT		ModNameOffset;
	CHAR		ImageName[256];
}SYSTEM_MODULE_INFORMATION,*PSYSTEM_MODULE_INFORMATION;

typedef struct _SYS_MOD_INFO {
	ULONG	NumberOfModules;
	SYSTEM_MODULE_INFORMATION Module[1];
}SYS_MOD_INFO,*PSYS_MOD_INFO;
#include <poppack.h>

#endif //_SYSTEM_MODULE_INFORMATION_DEFINED

typedef DWORD (FAR WINAPI *_pZwQuerySystemInformation)(IN ULONG, OUT PVOID, IN ULONG, OUT PULONG OPTIONAL);
static _pZwQuerySystemInformation ZwQuerySystemInformation = NULL;

#define STATUS_SUCCESS 0

BOOL CheckVulnerable(PSYSTEM_MODULE_INFORMATION pMod)
{
	DWORD i;
	DWORD readable_start = 0;
	DWORD writable_start = 0;
	BOOL bVulnerable = FALSE;
	for (i=(DWORD)(pMod->Base); i<(DWORD)(pMod->Base)+pMod->Size; i+=0x1000)
	{
		DWORD dw = 'ekim';
		__try
		{
			dw = *(DWORD*)i;
			if (!readable_start)
				readable_start = i;
		}
		__except(1)
		{
			if (readable_start)
				printf("Memory in range %08X-%08X is readable\n", readable_start, i-0x1000);
			readable_start = 0;
		}

		__try
		{
			*(DWORD*)i = dw;
			if (!writable_start)
				writable_start = i;
			bVulnerable = TRUE;
		}
		__except(1)
		{
			if (writable_start)
				printf("Memory in range %08X-%08X is writable\n", writable_start, i-0x1000);
			writable_start = 0;
		}
	}
	if (readable_start)
		printf("Memory in range %08X-%08X is readable\n", readable_start, i-0x1000);
	if (writable_start)
		printf("Memory in range %08X-%08X is writable\n", writable_start, i-0x1000);
	return bVulnerable;
}

BOOL EnumModules(PCHAR sName, BOOL* pbVulnerable)
{
ULONG		NtStatus;
ULONG		BufLen;
ULONG		i;
PVOID		ret;
PULONG	qBuff;
PSYSTEM_MODULE_INFORMATION Mod;
BOOL bNameFound = FALSE;
	if (pbVulnerable)
		*pbVulnerable = FALSE;
	ret=NULL;
	NtStatus=ZwQuerySystemInformation(11,&BufLen,0,&BufLen);//STATUS_INFO_LENGTH_MISMATCH
	qBuff=(ULONG*)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,BufLen);
	if(qBuff) 
	{
		NtStatus=ZwQuerySystemInformation(11,qBuff,BufLen,NULL);
		if(NtStatus==STATUS_SUCCESS) 
		{
			Mod=(PSYSTEM_MODULE_INFORMATION)(qBuff+1);
			for(i=0; i < *qBuff; i++) 
			{
//				DbPrint(DC_SYS,DL_INFO,("(%d) Base of %s=0x%x Size=%d Flags=0x%x LoadCnt=%d FullName: %s\n",
//					Mod[i].Index, Mod[i].ImageName+Mod[i].ModNameOffset,Mod[i].Base,Mod[i].Size,
//					Mod[i].Flags, Mod[i].LoadCount,Mod[i].ImageName ));
				if(!sName || !*sName || !_stricmp(Mod[i].ImageName+Mod[i].ModNameOffset,sName)) 
				{
					printf("(%d) Native base of %s=%x Size=0x%x Flags=0x%x LoadCnt=%d FullName: %s\n",
						Mod[i].Index, Mod[i].ImageName+Mod[i].ModNameOffset,Mod[i].Base,Mod[i].Size,
						Mod[i].Flags, Mod[i].LoadCount,Mod[i].ImageName );
					//ret=Mod[i].Base;
					//break;
					bNameFound = TRUE;
					if (CheckVulnerable(&Mod[i]))
					{
						if (pbVulnerable)
							*pbVulnerable = TRUE;
					}
				}
			}
		} 
		else 
		{
			printf("QuerySystemInformation for system modules fail with status %x\n",NtStatus);
		}
		HeapFree(GetProcessHeap(), 0, qBuff);
	}
	return bNameFound;
}

int main(int argc, char* argv[])
{
	HMODULE hModNtDll = GetModuleHandleA("ntdll.dll");
	BOOL bVulnerable = FALSE;
	if (hModNtDll == NULL)
	{
		printf("GetModuleHandle failed with %08X\n", GetLastError());
		return 1;
	}
	if(!(ZwQuerySystemInformation = (_pZwQuerySystemInformation) GetProcAddress(hModNtDll, "ZwQuerySystemInformation")))
	{
		printf("GetProcAddress failed with %08X\n", GetLastError());
		return 1;
	}
	if (!EnumModules("klif.sys", &bVulnerable))
		printf("Driver KLIF.SYS not found in memory.\n");
	else
		printf("Driver KLIF.SYS is %s.\n", bVulnerable ? "VULNERABLE" : "NOT vulnerable");
	return 0;
}
