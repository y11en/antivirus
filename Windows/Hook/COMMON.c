BOOLEAN			UnloadInProgress=FALSE;

PCHAR _STR_LOG_FMT		= "%s\t%s\t%s\t%s - %s";
PCHAR _STR_LOG_FMT_U	= "%s\t%s\t%S\t%s - %s";

/*
CHAR *ParamOperStr[]={
	"== ",
	" & ",
	" > ",
	" < "
};
*/

#define		PARTITION_TYPE_UNKNOWN	(BYTE)-1

#include "logging.c"

extern
NTSTATUS
DoExternalDrvRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutRequest,
	ULONG OutRequestSize,
	ULONG *pRetSize
	);

#include "syscalls.h"

#include "ioctl.c"

#ifdef _HOOK_NT_
	#include "fidbox.h"
#endif

BOOLEAN GetDriverInfo(PADDITIONALSTATE pAddState)
{
	pAddState->DeadlockCount = gDeadLocks;
	pAddState->FilterEventEntersCount = gFilterEventEntersCount;
	pAddState->ReplyWaiterEnters = gReplyWaiterEnters;
	pAddState->DrvFlags = _DRV_FLAG_NONE;
#ifdef __DBG__
	pAddState->DrvFlags |= _DRV_FLAG_DEBUG;
#endif
	if (TSPInitedOk == TRUE)
		pAddState->DrvFlags |= _DRV_FLAG_TSP_OK;
	if(SysCallsInited==TRUE)
		pAddState->DrvFlags |= _DRV_FLAG_DIRECTSYSCALL;
	
	pAddState->DrvFlags |=_DRV_FLAG_HAS_DISPATCH_FLT;
	pAddState->DirectSysCallStartId = DirectSysCallStartId;

	pAddState->DrvFlags |= _DRV_FLAG_SUPPORT_NET_SAFESTREAMS;
	pAddState->DrvFlags |= gDrvFlags;

	return TRUE;
}

// -----------------------------------------------------------------------------------------
// located in "INIT" section, look for #pragma alloc_text("INIT",...)
NTSTATUS CommonInit(VOID) 
{
	DbPrint(DC_SYS, DL_IMPORTANT, (AVPGNAME" compiled at " __DATE__ " " __TIME__"\n\n"));
	
	GlobalInvThreadInit();
	
	TSPInit();
#ifdef _HOOK_IMPERSONATE_ 	
	InitImpersonate();
#endif
#ifdef _HOOK_NT_
	// disabled из-за у€звимостей, если включить - будет падать
#else
	InitSysCalls();
#endif
#ifdef _HOOK_NT_
	FidBox_GlobalInit();
#endif
	GlobalProcCheckInit();
	GlobalCacheInit();
	GlobalClientsInit();

	GlobalFiltersInit();

	return STATUS_SUCCESS;
}

VOID CommonDone(VOID)
{
	UnloadInProgress = TRUE;//This line MUST be First in this function

	StopAWDog();
	GlobalFiltersDone();
	InvisibleReleaseAll();
	GlobalCacheDone();

	DoneSysCalls();
	TSPDone();

	GlobalInvThreadDone();

	GlobalProcCheckDone();
	GlobalClientsDone();
}
