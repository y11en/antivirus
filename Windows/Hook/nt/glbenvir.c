#include "glbenvir.h"
#include "..\debug.h"
#include "..\osspec.h"

PVOID BaseOfNtDllDll = NULL;
PVOID BaseOfNtOsKrnl = NULL;
PVOID BaseOfHal = NULL;
ULONG ProcessNameOffset = 0;

#ifndef PF_PAE_ENABLED
	#define PF_PAE_ENABLED                      9   // winnt
#endif

BOOLEAN gbPaeEnabled = FALSE;
ULONG	gDrvFlags = _DRV_FLAG_NONE;

BOOLEAN
InitGlobalEnvironment()
{
	DWORD OsBase = (DWORD)IoAllocateIrp;;
	gbPaeEnabled = ExIsProcessorFeaturePresent(PF_PAE_ENABLED);
	if (gbPaeEnabled)
	{
		DbPrint(DC_SYS, DL_WARNING,("PAE Enabled\n"));
	}

	BaseOfHal = GetNativeBase("HAL.DLL", 0);
	BaseOfNtDllDll = GetNativeBase("NTDLL.DLL", 0);

	BaseOfNtOsKrnl = GetNativeBase("NTOSKRNL.EXE", OsBase);
	
	if (BaseOfNtOsKrnl == NULL)
	{
		BOOLEAN bExit = FALSE;
		
		// ne ispolzovat MM_LOWEST_SYSTEM_ADDRESS or SYSTEM_BASE - exist different defines - 
		//#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0x80000000 and 
		//#define MM_LOWEST_SYSTEM_ADDRESS (PVOID)0xC0800000
#define _os_min_base 0x80000000
		
		DbPrint(DC_SYS, DL_WARNING,("NTOSKRNL with different name. Alternate Search\n"));
		
		OsBase &= ~0xFFF;
		while (!bExit)
		{
			__try
			{
				if(*(short*)OsBase == 'ZM')
					bExit = TRUE;
				else
					OsBase -= PAGE_SIZE;
				if(OsBase <= _os_min_base)
				{
					OsBase = 0;
					bExit = TRUE;
				}
				
				
			} __except(EXCEPTION_EXECUTE_HANDLER)
			{
				OsBase = 0;
				bExit = TRUE;
			}
		}
		
		if (bExit == TRUE && OsBase != 0)
		{
			if((GetExport((PVOID) OsBase,"IoCallDriver", NULL)))
			{
				BaseOfNtOsKrnl = (PVOID)OsBase;
				DbPrint(DC_SYS, DL_WARNING,("NTOSKRNL found on alternate addr %#x\n", BaseOfNtOsKrnl));
			}
		}
	}
	if(BaseOfNtDllDll == NULL || BaseOfNtOsKrnl == NULL || (ULONG)BaseOfNtDllDll >= MM_USER_PROBE_ADDRESS)
	{
		DbPrint(DC_SYS,DL_FATAL_ERROR,("Incorrect ModBase ntdll = %x, ntoskrnl = %x\n", BaseOfNtDllDll, BaseOfNtOsKrnl));
		DbgBreakPoint();
		return FALSE;
	}
	
	ProcessNameOffset = GetProcessNameOffset();

	if (*NtBuildNumber >= 2195)
	{
		PVOID *pMmUserProbeAddress;
		
		if((pMmUserProbeAddress = GetExport( BaseOfNtOsKrnl, "MmUserProbeAddress", NULL)))
		{
			_MmUserProbeAddress = *pMmUserProbeAddress;
		}
	}
	
	return TRUE;
}

void LoadDriverFlags(VOID) 
{
	HKEY				ParKeyHandle;
	UNICODE_STRING		ValueName;
	PKEY_VALUE_PARTIAL_INFORMATION pinfo = NULL;
	OBJECT_ATTRIBUTES	objectAttributes;

	NTSTATUS ntstatus;
	ULONG TmpLen;

	if((pinfo = ExAllocatePoolWithTag(PagedPool, sizeof(gDrvFlags) + sizeof(KEY_VALUE_PARTIAL_INFORMATION),'.BOS')))
	{
		InitializeObjectAttributes(&objectAttributes, &RegParams, OBJ_CASE_INSENSITIVE,NULL, NULL);
		ntstatus = ZwOpenKey(&ParKeyHandle, KEY_READ, &objectAttributes);
		if(NT_SUCCESS(ntstatus))
		{
			RtlInitUnicodeString(&ValueName, L"DrvFlagsExt");
			TmpLen = 0;

			ntstatus = ZwQueryValueKey(ParKeyHandle, &ValueName, KeyValuePartialInformation, 
				pinfo, sizeof(KEY_VALUE_PARTIAL_INFORMATION) + sizeof(gDrvFlags), &TmpLen);

			if((NT_SUCCESS(ntstatus)) && (pinfo->DataLength == sizeof(gDrvFlags)))
			{
				gDrvFlags = (ULONG) (*pinfo->Data);
				gDrvFlags = gDrvFlags & _DRV_FLAG_NO_ISTREAM_SUPPORT;
			}

			ZwClose(ParKeyHandle);
		}
		ExFreePool(pinfo);
	}
}
