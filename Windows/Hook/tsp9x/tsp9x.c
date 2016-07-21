
#define   DEVICE_MAIN
#include  "tsp9x.h"
#undef    DEVICE_MAIN

Declare_Virtual_Device(TSP)


#if DEBUG
#define __DBG__
#endif


//NT specific
#define PAGE_SIZE											0x1000L
#define NTSTATUS											LONG
#define STATUS_SUCCESS									ERROR_SUCCESS
#define STATUS_INVALID_PARAMETER						ERROR_INVALID_PARAMETER
#define STATUS_INSUFFICIENT_RESOURCES				ERROR_NO_SYSTEM_RESOURCES
#define STATUS_MEMORY_NOT_ALLOCATED					ERROR_NOT_ENOUGH_MEMORY
#define STATUS_NOT_FOUND								ERROR_FILE_NOT_FOUND
#define STATUS_NOT_IMPLEMENTED					ERROR_CALL_NOT_IMPLEMENTED

#define _HOOK_VXD_
#include "..\osspec.h"
#define DBG_PREFIX "TSP"
#include "..\debug.c"
#include "..\hook\avpgcom.h"
#include "..\Hook\HookSpec.h"
#include "..\osspec.c"
#include "..\TSPv2.c"

/******************************** ioctl stubs ***************************************************/

BOOLEAN RegisterApplication(PAPP_REGISTRATION pReg, PCLIENT_INFO pClientInfo)
{
	if(pWin16Mutex==NULL) {
		pWin16Mutex=(PWIN16MUTEX)pReg->m_SyncAddr;
	}
	SetObsfucator(pReg->m_CurProcId);
	return TRUE;
}

BOOLEAN ClientActivity (PHDSTATE In, PHDSTATE Out) 
{
	return TRUE;
}

//!!!!!!!!!!! надо бы возвращать boolean
VERDICT __stdcall FilterEvent(PFILTER_EVENT_PARAM pEventParam, PEVENT_OBJECT_INFO pEventObject)
{
	//return FALSE;
	return Verdict_NotFiltered;
}

typedef struct {
	UCHAR Drive;
	UCHAR   Len;
	UCHAR   Head;
	UCHAR Sector;
	ULONG Cylinder;
} READ_INT13,*PREAD_INT13;

BOOLEAN ReadI13(PREAD_INT13 Req, PVOID OutBuff)
{
	return FALSE;
}

void ClientSetFlowTimeout(PFLOWTIMEOUT pFlow)
{
}

void GetEventSizeForClient (ULONG AppID, DWORD *pSize) 
{
}

NTSTATUS SkipEventForClient (ULONG AppID) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS YeldEventForClient (PYELD_EVENT pYeldEvent, ULONG InYeldSize, PCHAR OutputBuffer, DWORD* pOutDataLen) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS ClientResetCache (ULONG AppID) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS EventSetVerdict (PSET_VERDICT pSetVerdict) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS InvisibleTransmit (PINVISIBLE_TRANSMIT pInvTransmit, PINVISIBLE_TRANSMIT pInvTransmitOut) 
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS FilterTransmit(PFILTER_TRANSMIT pFilterTransmitIn, PFILTER_TRANSMIT pFilterTransmitOut, ULONG OutputBufferLength)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DoClientCommonMemoryOp(PCOMMON_MEMORY pInMem, PCOMMON_MEMORY pOutMem)
{
	return STATUS_NOT_IMPLEMENTED;
}

BOOL _DiskIO(PDISK_IO pdio, BYTE* Buffer)
{
	return FALSE;
}

NTSTATUS ImpersonateThreadForClient(PIMPERSONATE_REQUEST pImperonate)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DoExternalDrvRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DoEventRequest(PEVENT_OBJECT_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, DWORD* pOutDataLen)
{
	return STATUS_NOT_IMPLEMENTED;
}

BOOLEAN GetEventForClient(PGET_EVENT pGetEvent, PEVENT_TRANSMIT pEventTransmit, DWORD* pOutputBufferLength)
{
	return FALSE;
}

#include "..\ioctl.c"

BOOLEAN GetDriverInfo(PADDITIONALSTATE pAddState)
{
	pAddState->DeadlockCount = 0;
	pAddState->FilterEventEntersCount = 0;
	pAddState->ReplyWaiterEnters = 0;
	pAddState->DrvFlags = _DRV_FLAG_NONE;
#ifdef __DBG__
	pAddState->DrvFlags |= _DRV_FLAG_DEBUG;
#endif
	if (TSPInitedOk == TRUE)
		pAddState->DrvFlags |= _DRV_FLAG_TSP_OK;
//	if(SysCallsInited==TRUE)
//		pAddState->DrvFlags |= _DRV_FLAG_DIRECTSYSCALL;
	pAddState->DirectSysCallStartId=0;//DirectSysCallStartId;
	return TRUE;
}


/****************************************** Dispatchers **********************************************/

VOID OnDestroyProcess(DWORD ProcessToken)
{
	if(Obsfucator) {
		DbPrint(DC_SYS,DL_INFO, ("OnDestroyProcess called. ProcToken=%x\n", ProcessToken));
		TSPProcCrash((PVOID)(ProcessToken ^ Obsfucator));
	}
}


VOID OnDestroyThread(THREADHANDLE hThread)
{
	DbPrint(DC_SYS,DL_INFO, ("OnDestroyThread called. hThread=%x CurTh=%x\n", hThread,Get_Cur_Thread_Handle()));
	TSPThreadCrash((PVOID)hThread);
}

BOOL OnW32Deviceiocontrol(PIOCTLPARAMS p)
{
ULONG StatStat;
	*p->dioc_bytesret=0;
	switch (p->dioc_IOCtlCode) {
	case DIOC_OPEN:
		DbPrint(DC_SYS,DL_NOTIFY, ("IOCTLHOOK_OPEN\n"));
		return FALSE;
	case DIOC_CLOSEHANDLE:
		DbPrint(DC_SYS,DL_NOTIFY, ("IOCTLHOOK_CLOSEHANDLE.\n"));
		return FALSE;
	default:
		return !HOOKDeviceControl(p->dioc_InBuf,p->dioc_cbInBuf,p->dioc_OutBuf,p->dioc_cbOutBuf,p->dioc_IOCtlCode,p->dioc_bytesret,&StatStat, TRUE);
	}
}

VOID OnSystemExit(VMHANDLE hVM)
{
	DbPrint(DC_SYS,DL_NOTIFY, ("OnSystemExit\n"));
	TSPDone();
}


BOOL OnSysDynamicDeviceExit()
{
	DbPrint(DC_SYS,DL_NOTIFY, ("OnSysDynamicDeviceExit\n"));
	TSPDone();
	DbPrint(DC_SYS,DL_IMPORTANT, ("ControlDispatcher addr=%X\n",ControlDispatcher));
	DbPrint(DC_SYS,DL_IMPORTANT, ("unloaded\n"));
	return TRUE;
}

BOOL OnSysDynamicDeviceInit()
{
//NTSTATUS				ntstatus;
	DbPrint(DC_SYS,DL_NOTIFY, ("entering DYNAMIC_DEVICE_INIT\n"));
#ifdef __DBG__
	_VolFlush(0,0);
	_VolFlush(1,0);
	_VolFlush(2,0);
	_VolFlush(3,0);
#endif //__DBG__
// Init commons NT & VxD data	
	TSPInit();
	return TRUE;
}

BOOL __cdecl ControlDispatcher(DWORD dwControlMessage,DWORD EBX,DWORD EDX,DWORD ESI,DWORD EDI,DWORD ECX)
{
	switch(dwControlMessage) {
	case SYS_DYNAMIC_DEVICE_INIT:
		OnSysDynamicDeviceInit();
		return TRUE;
//	case DEVICE_INIT:
	case INIT_COMPLETE:
		bUpperTableInited = FALSE;
		return OnSysDynamicDeviceInit();
	case SYS_DYNAMIC_DEVICE_EXIT:
		return OnSysDynamicDeviceExit();
	case W32_DEVICEIOCONTROL:
		return OnW32Deviceiocontrol((PIOCTLPARAMS)ESI);
	case DESTROY_PROCESS:
		OnDestroyProcess(EDX);
		return TRUE;
	case SYSTEM_EXIT:
		OnSystemExit((VMHANDLE)EBX);
		return TRUE;
	case DESTROY_THREAD:
		OnDestroyThread(EDI);
		break;
	default: break;
	}
	return TRUE;
}

