#include "syscalls.h"
#include "debug.h"
#include "client.h"

#ifdef _HOOK_NT_
#pragma data_seg(".userd")
#pragma bss_seg(".userd")
#endif // _HOOK_NT_

// located in ".user" section
BOOLEAN SysCallsInited=FALSE;

#ifdef _HOOK_NT_
// located in ".user" section
ULONG		DirectSysCallStartId=0;
#pragma bss_seg()
#pragma data_seg()
#endif // _HOOK_NT_

#ifdef _HOOK_VXD_
ULONG		DirectSysCallStartId=(KLIF_DeviceID<<16);
#define SysSrvAddParamVOIDFunc volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM
#define SysSrvAddParamFunc volatile PCLIENT_STRUCT pClientRegs,VMHANDLE hVM,

#define StubCheck(_outsize)
#define StubExit(x) {pClientRegs->CRS.Client_EAX=x; return 0;}

#else //NT
#define SysSrvAddParamVOIDFunc 
#define SysSrvAddParamFunc

#define StubCheck(_pinbuf, _insize, _poutbuf, _outsize) \
	PMDL pmdl = NULL;\
	PMDL pmdlout = NULL;\
	if (_pinbuf || _insize)\
	{\
		if (!_pinbuf || !_insize)\
			return STATUS_UNSUCCESSFUL;\
		pmdl = IoAllocateMdl(_pinbuf, _insize, FALSE, FALSE, NULL);\
		if (!pmdl)\
			return STATUS_UNSUCCESSFUL;\
		__try\
		{\
			MmProbeAndLockPages(pmdl, UserMode, IoWriteAccess);\
		}\
		__except(EXCEPTION_EXECUTE_HANDLER)\
		{\
			IoFreeMdl(pmdl);\
			return STATUS_UNSUCCESSFUL;\
		};\
		MmUnlockPages(pmdl);\
		IoFreeMdl(pmdl);\
	}\
	if (_poutbuf || _outsize)\
	{\
		if (!_poutbuf || !_outsize)\
			return STATUS_UNSUCCESSFUL;\
		pmdlout = IoAllocateMdl(_poutbuf, _outsize, FALSE, FALSE, NULL);\
		if (!pmdlout)\
			return STATUS_UNSUCCESSFUL;\
		__try\
		{\
			MmProbeAndLockPages(pmdlout, UserMode, IoWriteAccess);\
		}\
		__except(EXCEPTION_EXECUTE_HANDLER)\
		{\
			IoFreeMdl(pmdlout);\
			MmUnlockPages(pmdl);\
			IoFreeMdl(pmdl);\
			return STATUS_UNSUCCESSFUL;\
		};\
	}

#define StubExit(x) {\
	NTSTATUS status_x = x;\
	if(pmdl)\
	{\
		MmUnlockPages(pmdl);\
		IoFreeMdl(pmdl);\
	}\
	if(pmdlout)\
	{\
		MmUnlockPages(pmdlout);\
		IoFreeMdl(pmdlout);\
	}\
	return status_x;};
/*
#define SysStubCheck(_pinbuf, _insize, _poutbuf, _outsize) 	PMDL pmdl = IoAllocateMdl(_pbuf, _outsize, FALSE, FALSE, NULL);\
	if (!pmdl)\
		return STATUS_UNSUCCESSFUL;\
	__try\
	{\
		MmProbeAndLockPages(pmdl, UserMode, IoWriteAccess);\
	}\
	__except(EXCEPTION_EXECUTE_HANDLER)\
	{\
		IoFreeMdl(pmdl);\
		return STATUS_UNSUCCESSFUL;\
	};
#define SysSrvRetFuncStub(x) {NTSTATUS status_x = x; MmUnlockPages(pmdl); IoFreeMdl(pmdl); return status_x;};
*/

#endif // _HOOK_VXD_

//--------------------------------------------------------------------------------------------------------------------
// Services implementation
ULONG __stdcall
W32ServGetVersion(SysSrvAddParamVOIDFunc)
{
	StubCheck(NULL, 0, NULL, 0);
	StubExit(HOOK_INTERFACE_NUM);
}

VERDICT __stdcall
W32ServFilterEvent(SysSrvAddParamFunc PFILTER_EVENT_PARAM pEventParam)
{
	StubCheck(pEventParam, sizeof(FILTER_EVENT_PARAM), NULL, 0);
	StubExit(FilterEvent(pEventParam, NULL));
}

ULONG __stdcall
W32ServIsNeedFltEvAndNotInvisTh(SysSrvAddParamFunc ULONG HookID, ULONG FuncMj, ULONG FuncMi)
{
	StubCheck(NULL, 0, NULL, 0);
	StubExit((ULONG)IsNeedFilterEvent(HookID, FuncMj, FuncMi));
}

//+ ----------------------------------------------------------------------------------------

NTSTATUS __stdcall
W32ServActivity(SysSrvAddParamFunc PVOID InputBuffer, PVOID OutputBuffer)
{
	StubCheck(InputBuffer, sizeof(HDSTATE), OutputBuffer, sizeof(HDSTATE));
	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServActivity\n"));
	if (!ClientActivity((PHDSTATE)InputBuffer, (PHDSTATE)OutputBuffer))
	{
		StubExit(STATUS_UNSUCCESSFUL);
	}

	StubExit(STATUS_SUCCESS);
}

NTSTATUS __stdcall
W32ServGetEvent(SysSrvAddParamFunc PVOID InputBuffer, PVOID OutputBuffer, PULONG pBytesRet)
{
	StubCheck(InputBuffer, sizeof(GET_EVENT), OutputBuffer, sizeof(EVENT_TRANSMIT));
	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServGetEvent\n"));

	if (!GetEventForClient((PGET_EVENT) InputBuffer, (PEVENT_TRANSMIT) OutputBuffer, pBytesRet))
		*pBytesRet = 0;

	StubExit(STATUS_SUCCESS);
}

NTSTATUS __stdcall
W32ServImpersonateThread(SysSrvAddParamFunc PVOID InputBuffer)
{
	StubCheck(InputBuffer, sizeof(IMPERSONATE_REQUEST), NULL, 0);
	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServImpersonateThread\n"));
	
	StubExit(ImpersonateThreadForClient((PIMPERSONATE_REQUEST)InputBuffer));
}

NTSTATUS __stdcall
W32ServYieldEvent(SysSrvAddParamFunc PVOID InputBuffer)
{
	StubCheck(InputBuffer, sizeof(YELD_EVENT), NULL, 0);
	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServYieldEvent\n"));
	
	// adding sizeof(EXEC_ON_HOLD) is unsafe

	StubExit(YeldEventForClient((PYELD_EVENT)InputBuffer, sizeof(YELD_EVENT) + sizeof(EXEC_ON_HOLD), NULL, NULL));
}

NTSTATUS __stdcall
W32ServSetVerdict(SysSrvAddParamFunc PVOID InputBuffer)
{
	StubCheck(InputBuffer, sizeof(SET_VERDICT), NULL, 0);

	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServSetVerdict\n"));
	
	StubExit(EventSetVerdict((PSET_VERDICT)InputBuffer));
}

NTSTATUS __stdcall
W32ServInvisibleTransmit(SysSrvAddParamFunc PVOID InputBuffer, PVOID OutputBuffer)
{
	StubCheck(InputBuffer, sizeof(INVISIBLE_TRANSMIT), OutputBuffer, sizeof(INVISIBLE_TRANSMIT));

	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServInvisibleTransmit\n"));
	
	StubExit(InvisibleTransmit((PINVISIBLE_TRANSMIT)InputBuffer, (PINVISIBLE_TRANSMIT)OutputBuffer));
}

NTSTATUS __stdcall
W32ServFilterTransmit(SysSrvAddParamFunc PVOID InputBuffer, PVOID OutputBuffer, PULONG pBytesRet)
{
	StubCheck(InputBuffer, sizeof(FILTER_TRANSMIT), OutputBuffer, sizeof(FILTER_TRANSMIT));

	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServFilterTransmit\n"));
	
	StubExit(FilterTransmit((PFILTER_TRANSMIT) InputBuffer, (PFILTER_TRANSMIT)OutputBuffer, *pBytesRet));
}

NTSTATUS __stdcall
W32ServExternalDrvRequest(SysSrvAddParamFunc PVOID InputBuffer, PVOID OutputBuffer, PULONG pBytesRet)
{
	NTSTATUS status;
	ULONG retsize;

	StubCheck(InputBuffer, sizeof(EXTERNAL_DRV_REQUEST), NULL, 0 );

	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServExternalDrvRequest\n"));
	
	retsize = *pBytesRet;
	status = DoExternalDrvRequest((PEXTERNAL_DRV_REQUEST)InputBuffer, 0, OutputBuffer, *pBytesRet, &retsize);
	if (retsize > *pBytesRet)
	{
		DbPrint(DC_IOCTL,DL_WARNING, ("External request (direct) - return buffer to big! try to return - %d (allowed %d)\n", 
			retsize, *pBytesRet));
		status = STATUS_BUFFER_TOO_SMALL;
	}
	else
		*pBytesRet = retsize;

	StubExit(status);
}

NTSTATUS __stdcall
W32ServEventRequest(SysSrvAddParamFunc PVOID InputBuffer, PVOID OutputBuffer, PULONG pBytesRet)
{
	NTSTATUS status;
	ULONG retsize;

	StubCheck(InputBuffer, sizeof(EVENT_OBJECT_REQUEST), NULL, 0 );

	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServEventRequest\n"));

	retsize = *pBytesRet;
	status = DoEventRequest((PEVENT_OBJECT_REQUEST) InputBuffer, OutputBuffer, *pBytesRet, &retsize);
	if (status == STATUS_SUCCESS)
	{
		if (retsize > *pBytesRet)
		{
			DbPrint(DC_IOCTL,DL_WARNING, ("Event request - return buffer to big! try to return - %d (allowed %d)\n", 
				retsize, *pBytesRet));
			status = STATUS_INVALID_BLOCK_LENGTH;
		}
		else
			*pBytesRet = retsize;
	}
	
	StubExit(status);
}


NTSTATUS __stdcall
W32ServTSPRequest(SysSrvAddParamFunc PVOID InputBuffer,PVOID OutBuff)
{
	NTSTATUS status;

#ifdef _HOOK_NT_
	StubCheck(NULL, 0, NULL, 0 );
	StubExit(STATUS_INVALID_PARAMETER);
#endif

	DbPrint(DC_IOCTL,DL_SPAM, ("W32ServTSPRequest\n"));
	switch(((PTSPRS_COMMON)InputBuffer)->ReqClass)
	{
	case TSPRC_Register:
		if(OutBuff==NULL)
		{
			StubExit(STATUS_INVALID_PARAMETER);
		}
		status = TSPRegister(((PTSPRS_REGISTER)InputBuffer)->MaxSize, (ORIG_HANDLE*)OutBuff);
		break;
	case TSPRC_Unregister:
		status = TSPUnregister(((PTSPRS_COMMON)InputBuffer)->hOrig);
		break;
	case TSPRC_Lock:
		status = TSPLock((PTSPRS_LOCK)InputBuffer);
		break;
	case TSPRC_ThreadEnter:
		status = TSPThreadEnter(((PTSPRS_COMMON)InputBuffer)->hOrig);
		break;
	case TSPRC_ThreadLeave:
		status = TSPThreadLeave(((PTSPRS_COMMON)InputBuffer)->hOrig);
		break;
	default:
		StubExit(STATUS_INVALID_PARAMETER);
	}
	StubExit(status);
}

//+ ----------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// Services preparing
// Когда добавляешь сервиса 
//		1.увеличить NumOfSysCallsServices
//		2.исправить AVPG_W32_Service_Table, второй параметр - число аргументов в штуках!!!!
//		3.исправить NTSCServiceTableBase
//		3.1 исправить NTSCParamTableBase в которой не число аргументов, а число байт занимаемых аргументами
//    4.добавить имя в enum AVPG_W32_Services (avpgcom.h)
#define NumOfSysCallsServices 13

#ifdef _HOOK_VXD_
	#define DeclareW32ServiceTable(service, args) (ULONG)service, args,
	ULONG AVPG_W32_Service_Table[]={NumOfSysCallsServices, 0,
		DeclareW32ServiceTable(W32ServGetVersion, 0)
		DeclareW32ServiceTable(W32ServFilterEvent, 1)
		DeclareW32ServiceTable(W32ServIsNeedFltEvAndNotInvisTh, 3)
		DeclareW32ServiceTable(W32ServActivity, 2)
		DeclareW32ServiceTable(W32ServGetEvent, 3)
		DeclareW32ServiceTable(W32ServImpersonateThread, 1)
		DeclareW32ServiceTable(W32ServYieldEvent, 1)
		DeclareW32ServiceTable(W32ServSetVerdict, 1)
		DeclareW32ServiceTable(W32ServInvisibleTransmit, 2)
		DeclareW32ServiceTable(W32ServFilterTransmit, 3)
		DeclareW32ServiceTable(W32ServExternalDrvRequest, 3)
		DeclareW32ServiceTable(W32ServEventRequest, 3)
		DeclareW32ServiceTable(W32ServTSPRequest,2)
	};
#else //NT
	ULONG NTSCServiceTableBase[]={
		(ULONG) W32ServGetVersion,
		(ULONG) W32ServFilterEvent,
		(ULONG) W32ServIsNeedFltEvAndNotInvisTh,
		(ULONG) W32ServActivity,
		(ULONG) W32ServGetEvent,
		(ULONG) W32ServImpersonateThread,
		(ULONG) W32ServYieldEvent,
		(ULONG) W32ServSetVerdict,
		(ULONG) W32ServInvisibleTransmit,
		(ULONG) W32ServFilterTransmit,
		(ULONG) W32ServExternalDrvRequest,
		(ULONG) W32ServEventRequest,
		(ULONG) W32ServTSPRequest,
	};
	UCHAR NTSCParamTableBase[]={ //Number of bytes for arguments
		0,
		4,
		12,
		8,
		12,
		4,
		4,
		4,
		8,
		12,
		12,
		12,
		8,
	};

#pragma intrinsic(memcmp)

// located in "INIT" section, look for #pragma alloc_text("INIT",...)	
ULONG GetAddrOfShadowTable()
{
	ULONG i;
	PCHAR p;
	ULONG dwordatbyte;
	p = (PCHAR) KeAddSystemServiceTable;

	for(i=0;i<4096;i++,p++)
	{
		__try
		{
			dwordatbyte=*(unsigned int *)p;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return 0;
		}
		if(MmIsAddressValid((PVOID)dwordatbyte) && MmIsAddressValid((BYTE*)dwordatbyte + 15))
		{
            try
            {
                //ProbeForRead((PVOID)dwordatbyte, 16, sizeof(UCHAR));

			    if(memcmp((PVOID)dwordatbyte, &KeServiceDescriptorTable, 16) == 0)
			    {
				    if((PVOID)dwordatbyte==&KeServiceDescriptorTable)
					    continue;

				    return dwordatbyte;
			    }

            }

            except(EXCEPTION_EXECUTE_HANDLER)
			{
			}
		}
	}
	return 0;
}
#endif

// located in "INIT" section, look for #pragma alloc_text("INIT",...)	
NTSTATUS InitSysCalls(VOID)
{
#ifdef _HOOK_VXD_
	Register_Win32_Services(&The_DDB,(PDDB)AVPG_W32_Service_Table); //There is the BUG in DDK with args order
	SysCallsInited=TRUE;
	DbPrint(DC_IOCTL,DL_NOTIFY, ("Syscalls inited\n"));
	return STATUS_SUCCESS;
#else
	ULONG *NewServiceTableBase; /* Pointer to new SSDT */
	UCHAR *NewParamTableBase; /* Pointer to new SSPT */
	ULONG NewNumberOfServices; /* New number of services */
	ULONG *NewServiceCounterTableBase=NULL;
	PServiceDescriptorTableEntry_t KeServiceDescriptorTableShadow;
	
	NewNumberOfServices=KeServiceDescriptorTable.NumberOfServices + NumOfSysCallsServices;
	KeServiceDescriptorTableShadow=(PServiceDescriptorTableEntry_t)GetAddrOfShadowTable();
	if(KeServiceDescriptorTableShadow==NULL) {
		DbPrint(DC_IOCTL,DL_WARNING, ("Syscalls init fail on GetAddrOfShadowTable\n"));
		return STATUS_UNSUCCESSFUL;
	}
	DirectSysCallStartId=KeServiceDescriptorTable.NumberOfServices;

	NewServiceTableBase=(ULONG*) ExAllocatePoolWithTag(NonPagedPool, NewNumberOfServices*sizeof(ULONG),'-SeB');
	if(NewServiceTableBase) {
		NewParamTableBase=(UCHAR*) ExAllocatePoolWithTag(NonPagedPool, NewNumberOfServices,'-SeB');
		if(NewParamTableBase){
			/* Backup the exising SSDT and SSPT */
			memcpy(NewServiceTableBase, KeServiceDescriptorTable.ServiceTableBase, KeServiceDescriptorTable.NumberOfServices*sizeof(ULONG));
			memcpy(NewParamTableBase, KeServiceDescriptorTable.ParamTableBase, KeServiceDescriptorTable.NumberOfServices);
			/* Append to it new SSDT and SSPT */
			memcpy(NewServiceTableBase+KeServiceDescriptorTable.NumberOfServices, NTSCServiceTableBase, sizeof(NTSCServiceTableBase));
			memcpy(NewParamTableBase+KeServiceDescriptorTable.NumberOfServices, NTSCParamTableBase, sizeof(NTSCParamTableBase));
			if(KeServiceDescriptorTable.ServiceCounterTableBase) {
				NewServiceCounterTableBase=(ULONG*) ExAllocatePoolWithTag(NonPagedPool, NewNumberOfServices*sizeof(ULONG),'-SeB');
				if(NewServiceCounterTableBase) {
					memcpy(NewServiceCounterTableBase, KeServiceDescriptorTable.ServiceCounterTableBase, KeServiceDescriptorTable.NumberOfServices*sizeof(ULONG));
					memset(NewServiceCounterTableBase+KeServiceDescriptorTable.NumberOfServices,0,sizeof(NTSCServiceTableBase));
					KeServiceDescriptorTable.ServiceCounterTableBase=NewServiceCounterTableBase;
					KeServiceDescriptorTableShadow->ServiceCounterTableBase=NewServiceCounterTableBase;
				} else {
					KeServiceDescriptorTable.ServiceCounterTableBase=NULL;
					KeServiceDescriptorTableShadow->ServiceCounterTableBase=NULL;
				}
			}
			/* Modify the KeServiceDescriptorTableEntry to point to new SSDT and SSPT */
			KeServiceDescriptorTable.ServiceTableBase=NewServiceTableBase;
			KeServiceDescriptorTable.ParamTableBase=NewParamTableBase;
			KeServiceDescriptorTable.NumberOfServices=NewNumberOfServices;
			/* Also update the KeServiceDescriptorTableShadow to point to new SSDT and SSPT */
			KeServiceDescriptorTableShadow->ServiceTableBase=NewServiceTableBase;
			KeServiceDescriptorTableShadow->ParamTableBase=NewParamTableBase;
			KeServiceDescriptorTableShadow->NumberOfServices=NewNumberOfServices;
			SysCallsInited = TRUE;
			DbPrint(DC_IOCTL,DL_IMPORTANT, ("Syscalls inited. Starting Id %x\n",DirectSysCallStartId));
			return STATUS_SUCCESS;
		}
		ExFreePool(NewServiceTableBase);
	}
	DirectSysCallStartId=0;
	DbPrint(DC_IOCTL,DL_WARNING, ("Syscalls init fail\n"));
	return STATUS_INSUFFICIENT_RESOURCES;
#endif
}

VOID DoneSysCalls(VOID)
{
	//SysCallsInited = FALSE;
}

