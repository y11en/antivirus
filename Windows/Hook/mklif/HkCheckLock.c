#include "HkCheckLock.h"
#include "inc/commdata.h"
#include "HkCheckLock.tmh"

FAST_MUTEX g_fmInternalSync;

//////////////////////////////////////////////////////////////////////////
extern ULONG ClearWP();
extern VOID RestoreWP(
			   __in ULONG OldCR0
			   );

//////////////////////////////////////////////////////////////////////////

typedef struct _CHECKLOCK_LISTITEM
{
	IMPLIST_ENTRY	m_List;
	PFILE_OBJECT	m_FileObject;
	HANDLE			m_ThreadId;
} CHECKLOCK_LISTITEM, *PCHECKLOCK_LISTITEM;

IMPLIST_ENTRY	gCheckLockList;

BOOLEAN IsSkipLock(PIRP Irp)
{
	BOOLEAN bSkipLock = FALSE;
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFILE_OBJECT		fileObject      = currentIrpStack->FileObject;

	//DbPrint(DC_SYS, DL_NOTIFY, ("IsSkipLock irp 0x%x, fo 0x%x\n", Irp, fileObject));

	if (fileObject)
	{
		if (!IsListEmpty((PLIST_ENTRY)&gCheckLockList))
		{
			PCHECKLOCK_LISTITEM LockItemTmp;
			PIMPLIST_ENTRY Flink;
			ExAcquireFastMutex(&g_fmInternalSync);

			if (!IsListEmpty((PLIST_ENTRY)&gCheckLockList))
			{
				Flink = gCheckLockList.Flink;

				while(Flink != &gCheckLockList)
				{
					LockItemTmp = CONTAINING_RECORD(Flink, CHECKLOCK_LISTITEM, m_List);

					Flink = Flink->Flink;

					if ((LockItemTmp->m_FileObject == fileObject) &&
						(LockItemTmp->m_ThreadId == PsGetCurrentThreadId()))
					{
						bSkipLock = TRUE;
						break;
					}
				}
			}

			ExReleaseFastMutex(&g_fmInternalSync);
		}
	}

	return bSkipLock;
}

VOID
AddSkipLockItem(PFILE_OBJECT pFileObject)
{
	PCHECKLOCK_LISTITEM pItem = ExAllocatePoolWithTag(NonPagedPool, sizeof(CHECKLOCK_LISTITEM), 'LbOS');
	if (pItem)
	{
		ExAcquireFastMutex(&g_fmInternalSync);

		pItem->m_FileObject = pFileObject;
		pItem->m_ThreadId = PsGetCurrentThreadId();
		_impInsertTailList(&gCheckLockList, &pItem->m_List);

		ExReleaseFastMutex(&g_fmInternalSync);
	}
}

VOID
DelSkipLockItem(PFILE_OBJECT pFileObject)
{
	ExAcquireFastMutex(&g_fmInternalSync);
	if (!IsListEmpty((PLIST_ENTRY)&gCheckLockList))
	{
		PCHECKLOCK_LISTITEM LockItemTmp;
		PIMPLIST_ENTRY Flink;

		Flink = gCheckLockList.Flink;

		while(Flink != &gCheckLockList)
		{
			LockItemTmp = CONTAINING_RECORD(Flink, CHECKLOCK_LISTITEM, m_List);

			Flink = Flink->Flink;

			if (LockItemTmp->m_FileObject == pFileObject)
			{
				_impRemoveEntryList(&LockItemTmp->m_List);
				ExFreePool(LockItemTmp);
			}
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);
}

DWORD FSRtlCheckLockAccessReadNextOp = 0;
#ifndef _WIN64
VOID __declspec(naked) NakedFSRtlCheckLockAccessRead(VOID)
{

	__asm
	{
		mov     edi,edi
			push    ebp
			mov     ebp,esp

			// call menia
			//esli return
			//{
			push [ebp+0xc]		;
		call IsSkipLock		;
		cmp al, 1			;
		jnz _chla_call_original					;

		pop ebp				;
		mov al, 1			;
		ret 8				;
		//}
_chla_call_original:
		jmp [FSRtlCheckLockAccessReadNextOp];
	}
}
#endif

// NakedFSRtlCheckLockAccessRead & NakedFSRtlCheckLockAccessRead2 mogno svernut v ondu function
#ifndef _WIN64
VOID
MakeCheckLockAccess()
{

	BYTE PatchData[5];
	BYTE CheckData[5] = {0x8b, 0xff, 0x55, 0x8b, 0xec};

	DWORD* ptr = (DWORD*)(&PatchData[1]);
	DWORD pOriginalFunc = (DWORD) FsRtlCheckLockForReadAccess;
	DWORD pPatchFunc = (DWORD) NakedFSRtlCheckLockAccessRead;

	if (!memcmp((void*) pOriginalFunc, CheckData, sizeof(CheckData)))
	{
//		BOOLEAN OldProtect;
		ULONG OldCR0;

		PatchData[0] = 0xe9;	// jmp
		*ptr = (DWORD)(pPatchFunc - pOriginalFunc - 5);
		FSRtlCheckLockAccessReadNextOp = pOriginalFunc + sizeof(PatchData);

		//if (MyVirtualProtect((void*) pOriginalFunc, TRUE, &OldProtect))
		{
			
			KIRQL OldIrql;
			
			KeRaiseIrql(DISPATCH_LEVEL, &OldIrql );
			OldCR0 = ClearWP();
			memcpy((void*) pOriginalFunc, PatchData, sizeof(PatchData));
			RestoreWP( OldCR0 );
			KeLowerIrql( OldIrql );

	//		MyVirtualProtect((void*) pOriginalFunc, OldProtect, &OldProtect);
		}
	}


}
#endif

//////////////////////////////////////////////////////////////////////////
typedef struct _PATCHNAME_LISTITEM
{
	IMPLIST_ENTRY	m_List;
	PUNICODE_STRING	m_UniNameOrig;
	PUNICODE_STRING	m_UniNameShadow;
} PATCHNAME_LISTITEM, *PPATCHNAME_LISTITEM;

IMPLIST_ENTRY	gPatchNameList;

void CheckNameAndPatch(PUNICODE_STRING pUniName)
{
	if (IsListEmpty((PLIST_ENTRY)&gPatchNameList))
		return;

	ExAcquireFastMutex(&g_fmInternalSync);

	if (!IsListEmpty((PLIST_ENTRY)&gPatchNameList))
	{
		PPATCHNAME_LISTITEM PatchNameTmp;
		PIMPLIST_ENTRY Flink;

		Flink = gPatchNameList.Flink;

		while(Flink != &gPatchNameList)
		{
			PatchNameTmp = CONTAINING_RECORD(Flink, PATCHNAME_LISTITEM, m_List);
			Flink = Flink->Flink;

			if (pUniName->Length == PatchNameTmp->m_UniNameShadow->Length)
			{
				if (!memcmp(pUniName->Buffer, PatchNameTmp->m_UniNameShadow->Buffer, pUniName->Length))
					memcpy(pUniName->Buffer, PatchNameTmp->m_UniNameOrig->Buffer, pUniName->Length);
			}
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);
}

NTSTATUS
DoDirectOpen (
	PHANDLE pHandle,
	PWCHAR pOrigName,
	USHORT OrigNameLenInTchar
)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

	USHORT len;
	USHORT reallen;
	USHORT pos = 0;

	USHORT buflen;

	DWORD slashcount = 0;

	WCHAR timetmp;

	PUNICODE_STRING pUniOrig = NULL;
	PUNICODE_STRING pUniShadow = NULL;

	PPATCHNAME_LISTITEM pItem;

	if (!OrigNameLenInTchar)
		return ntStatus;

	len = (USHORT) Uwcslen(pOrigName, OrigNameLenInTchar);

	if (!len) // in TCHAR without 0
		return STATUS_INVALID_PARAMETER;

	while (pos < len)
	{
		if (pOrigName[pos++] == ':')
			break;
	}

	if (pos >= len)
	{
		_dbg_break_;
		return STATUS_INVALID_PARAMETER;
	}

	reallen = len - pos;
	buflen = (reallen) * sizeof(WCHAR);

	pUniOrig = ExAllocatePoolWithTag(PagedPool, sizeof(UNICODE_STRING) + buflen, 'SBOs');
	pUniShadow = ExAllocatePoolWithTag(PagedPool, sizeof(UNICODE_STRING) + buflen, 'SBOs');
	if (!pUniOrig || !pUniShadow)
	{
		if (pUniOrig)
			ExFreePool(pUniOrig);

		if (pUniShadow)
			ExFreePool(pUniShadow);

		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	pUniOrig->Buffer = (PWSTR) (pUniOrig + 1);
	pUniOrig->MaximumLength = buflen;
	pUniOrig->Length = buflen;

	memcpy(pUniOrig->Buffer, pOrigName + pos, buflen);
	memcpy(pUniShadow, pUniOrig, sizeof(UNICODE_STRING) + buflen);
	pUniShadow->Buffer = (PWSTR) (pUniShadow + 1);

	len = len - pos - 1;
	while (len)
	{
		if (pUniShadow->Buffer[len] == '\\')
		{
			slashcount++;
			if (2 == slashcount)
				break;
		}
		else
		{
			LARGE_INTEGER tm;
			KeQuerySystemTime(&tm);
			//timetmp = (WCHAR) GetCurTimeFast();
			timetmp = (WCHAR)tm.LowPart;
			pUniShadow->Buffer[len] = 'a' + (timetmp & 0x1f);

			if (pUniShadow->Buffer[len] > 'z')
				pUniShadow->Buffer[len] -= 15;
		}

		len--;
	}

	pItem = ExAllocatePoolWithTag(PagedPool, sizeof(PATCHNAME_LISTITEM), 'lBOs');
	if (pItem)
	{
		pItem->m_UniNameOrig = pUniOrig;
		pItem->m_UniNameShadow = pUniShadow;

		ExAcquireFastMutex(&g_fmInternalSync);
		_impInsertTailList(&gPatchNameList, &pItem->m_List);
		ExReleaseFastMutex(&g_fmInternalSync);

		// create file
		{
			UNICODE_STRING		LogFileName;
			OBJECT_ATTRIBUTES	objectAttributes;
			IO_STATUS_BLOCK		ioStatus;

			memcpy(pOrigName + pos, pUniShadow->Buffer, reallen * sizeof(WCHAR));

			RtlInitUnicodeString(&LogFileName, pOrigName);
			InitializeObjectAttributes(&objectAttributes, &LogFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);

// 			ntStatus = ZwCreateFile(
// 							pHandle, 
// 							FILE_READ_DATA | SYNCHRONIZE, 
// 							&objectAttributes, 
// 							&ioStatus, 
// 							NULL, 
// 							0, 
// 							FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 
// 							FILE_OPEN, 
// 							FILE_SYNCHRONOUS_IO_NONALERT, 
// 							NULL, 
// 							0
// 							);

			ntStatus = FltCreateFile(
							Globals.m_Filter,
							NULL,
							pHandle,
							FILE_READ_DATA | SYNCHRONIZE,
							&objectAttributes,
							&ioStatus,
							NULL,
							0,
							FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
							FILE_OPEN,
							FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING,
							NULL,
							0,
							IO_IGNORE_SHARE_ACCESS_CHECK
							);

			if (!NT_SUCCESS(ntStatus))
			{
				if (pOrigName[1] == L'\\')
				{
					pOrigName[1] = L'?';
					ntStatus = FltCreateFile(
										Globals.m_Filter,
										NULL,
										pHandle,
										FILE_READ_DATA | SYNCHRONIZE,
										&objectAttributes,
										&ioStatus,
										NULL,
										0,
										FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
										FILE_OPEN,
										FILE_SYNCHRONOUS_IO_NONALERT | FILE_NO_INTERMEDIATE_BUFFERING,
										NULL,
										0,
										IO_IGNORE_SHARE_ACCESS_CHECK
										);
				}
			}
		}

		if (NT_SUCCESS(ntStatus))
		{
			PFILE_OBJECT pFileObject = NULL;
			NTSTATUS ntStatusTmp = ObReferenceObjectByHandle(*pHandle, 0, *IoFileObjectType, KernelMode, &pFileObject, NULL);
			if (NT_SUCCESS(ntStatusTmp))
			{
				PDEVICE_OBJECT pDevice = IoGetRelatedDeviceObject(pFileObject);
				if (FILE_DEVICE_NETWORK_FILE_SYSTEM == pDevice->DeviceType)
				{
					ntStatus = STATUS_NOT_SUPPORTED;

					ZwClose(*pHandle);

					*pHandle = NULL;
				}

				ObDereferenceObject(pFileObject);
			}
		}

		ExAcquireFastMutex(&g_fmInternalSync);
		_impRemoveEntryList(&pItem->m_List);
		ExReleaseFastMutex(&g_fmInternalSync);

		ExFreePool(pItem);
	}

	ExFreePool(pUniOrig);
	ExFreePool(pUniShadow);

	return ntStatus;
}

extern DWORD NtfsStart;
extern DWORD NtfsSize;
void IoIsOperationSyncCheckPatch(DWORD RetAddr, PIRP Irp)
{
	PIO_STACK_LOCATION currentIrpStack;

	UNREFERENCED_PARAMETER (RetAddr);
	//надо доделать
	//if ((RetAddr < NtfsStart) || (RetAddr >  NtfsStart + NtfsSize))
	//	return;

	if (!Irp)
		return;

	if (IO_TYPE_IRP != Irp->Type)
	{
		_dbg_break_;
		return;
	}

	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	if (IRP_MJ_CREATE == currentIrpStack->MajorFunction)
	{
		PFILE_OBJECT fileObject = currentIrpStack->FileObject;

		if (fileObject && fileObject->FileName.Length && fileObject->FileName.Buffer)
		{
			CheckNameAndPatch(&fileObject->FileName);
		}
	}
}

DWORD callIoIsOperationSync = 0;

#ifndef _WIN64
VOID
__declspec(naked) NakedIoIsOperationSync( VOID )
{
	__asm
	{
		mov     edi,edi
			push    ebp
			mov     ebp,esp

			push [ebp+0x8]
		push [ebp+0x4]
		call IoIsOperationSyncCheckPatch
			jmp [callIoIsOperationSync];
	}

}
#endif

#ifndef _WIN64
VOID
MakeIoIsOperationSync()
{

	BYTE PatchData[5];
	BYTE CheckData[5] = {0x8b, 0xff, 0x55, 0x8b, 0xec};

	DWORD* ptr = (DWORD*)(&PatchData[1]);
	DWORD pOriginalFunc = (DWORD) IoIsOperationSynchronous;
	DWORD pPatchFunc = (DWORD) NakedIoIsOperationSync;

	if (!memcmp((void*) pOriginalFunc, CheckData, sizeof(CheckData)))
	{
//		BOOLEAN OldProtect;
		ULONG OldCR0;
		PatchData[0] = 0xe9;	// jmp
		*ptr = (DWORD)(pPatchFunc - pOriginalFunc - 5);
		callIoIsOperationSync = pOriginalFunc + sizeof(PatchData);

		//if (MyVirtualProtect((void*) pOriginalFunc, TRUE, &OldProtect))
		{
			KIRQL OldIrql;
			KeRaiseIrql(DISPATCH_LEVEL, &OldIrql );
			OldCR0 = ClearWP();
			memcpy((void*) pOriginalFunc, PatchData, sizeof(PatchData));
			RestoreWP(OldCR0);
			KeLowerIrql( OldIrql );

		//	MyVirtualProtect((void*) pOriginalFunc, OldProtect, &OldProtect);
		}
	}

}
#endif

VOID
CheckLockLists_Init()
{
	_impInitializeListHead(&gCheckLockList);
	_impInitializeListHead(&gPatchNameList);
}

BOOLEAN ChLkOn = FALSE;

VOID
SwitchOnChLkOn()
{
	ChLkOn = TRUE;
}

VOID
SwitchOnChLkOff()
{
	ChLkOn = FALSE;
}


BOOLEAN
IsChLkOn()
{
	return ChLkOn;
}

#ifndef _WIN64
VOID
HookCheckLock()
{
	ExInitializeFastMutex(&g_fmInternalSync);
	CheckLockLists_Init();
	MakeCheckLockAccess();
	MakeIoIsOperationSync();
	SwitchOnChLkOn();
}
#endif