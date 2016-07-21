#include "StdAfx.h"
#include "NtBased.h"
#include "HOOK\PIDExternalAPI.H"
/*
struct tagVerdictToName{
	VERDICT	m_vrd;
	char*	m_vrdName;
}VerdictName[] = {{Verdict_Default, "Verdict_Default"},
					{Verdict_Pass, "Verdict_Pass"},
					{Verdict_Discard, "Verdict_Discard"},
					{Verdict_Kill, "Verdict_Kill"},
					{Verdict_WDog, "Verdict_WDog"}};
*/

char* VerdictName[] = {"Verdict_Default", 
						"Verdict_Pass", 
						"Verdict_Discard", 
						"Verdict_Kill",
						"Verdict_WDog"
						};


#ifdef _CONSOLE
PARSE_REPLY ProPacket(LPParseShuttle ps){
	Message(("...ok\n"));
	ADD_KEY(Verdict_WDog)
	return ok;
}
#else

/*
NTSTATUS
KLDeviceIoControl(PDEVICE_OBJECT	Device,
				  ULONG		IoControlCode,
				  PVOID		InData,
				  ULONG		InDataSize,
				  PVOID		OutData,
				  ULONG		OutDataSize,
				  ULONG		Returned)
{
	PIRP				Irp;
	KEVENT				Event;
	IO_STATUS_BLOCK		ioStatus;
	NTSTATUS			ntStatus;

	KeInitializeEvent(&Event,NotificationEvent,FALSE);
	Irp=IoBuildDeviceIoControlRequest(IoControlCode,
										Device,
										InData,
										InDataSize,
										OutData,
										OutDataSize,
										FALSE,
										&Event,
										&ioStatus);	
	
	if (Irp) {
		ntStatus=IoCallDriver (Device, Irp);
		if(ntStatus==STATUS_PENDING) {
			KeWaitForSingleObject(&Event,Executive,KernelMode,FALSE,NULL);
			ntStatus = ioStatus.Status;
		}
	}
	else {
		ntStatus = STATUS_UNSUCCESSFUL;
		Message ("Unable to Build Request \n");
	}
	return ntStatus;
}

*/
PARSE_REPLY ProPacket(LPParseShuttle ps){
	VERDICT Verdict;
	ps->pEventTransmit->HookID			= FLTTYPE_PID;
	ps->pEventTransmit->FunctionMj		= ps->pIH->m_wDirection;
	ps->pEventTransmit->FunctionMi		= 0;
	ps->pEventTransmit->ProcessingType	= PreProcessing;
	ps->pEventTransmit->ThreadID		= 1;
	ps->pEventTransmit->ProcName[0]		= 0;
	ps->pEventTransmit->ProcessID		= 0;
	ps->pEventTransmit->UnsafeValue		= 1;
	Message(DL_BRIEF, DM_UNWIND, ("TRUE: Send params to avpg...\n"));

/*	KLDeviceIoControl(ps->pIH->pAVPGObject, 
					IOCTLHOOK_FilterEvent, 
					ps->pEventTransmit, 
					MAX_EXCHANGE_AREA_SIZE, 
					&Verdict, 
					sizeof(Verdict),
					0);
*/
	Verdict = FilterEventExt(ps->pEventTransmit, NULL);
		
	Message(DL_BRIEF, DM_UNWIND, ("TRUE: ...Verdict %d\n", Verdict));
	ADD_KEY(Verdict)

	return ok;
}

PARSE_REPLY test(LPParseShuttle ps){

	ADD_KEY(Verdict_Pass)
		return ok;
}

#endif