#include "..\hook\PIDExternalAPI.H"
#include "..\hook\klpf_api.H"
#include "..\hook\klmc_api.H"

#define bool char
#define wchar_t unsigned short
#include "..\hook\klick_api.h"
#include "..\hook\klin_api.h"

LONG __stdcall SendDevIoCtl2DrvByName(PCHAR pDrvName, ULONG IoctlCode,PVOID InputBuffer,ULONG SizeOfInputBuffer,PVOID OutputBuffer,ULONG SizeOfOutputBuffer, ULONG* pRetSize)
{
ALLREGS regs;	// register struct passed to Directed_Sys_Ctrl
IOCTLPARAMS io;	// Ioctl param struct passed to target device
PVOID pDdb;
ULONG BytesReturned;
	pDdb=Get_DDB(0, pDrvName);
	if (pDdb == NULL ) {
		DbPrint(DC_FWALL,DL_ERROR, ("SendDevIoCtl2DrvByName with NULL PDDB failed\n"));
		return 0;
	}
	
	// Set up the ioctl params 
	io.dioc_IOCtlCode = IoctlCode;
	io.dioc_InBuf = InputBuffer;
	io.dioc_cbInBuf = SizeOfInputBuffer;
	io.dioc_OutBuf = OutputBuffer;
	io.dioc_cbOutBuf = SizeOfOutputBuffer;
	io.dioc_bytesret = &BytesReturned;	
	
	// Clear the regs structure
	memset(&regs, 0, sizeof(regs));
	
	// Put a pointer to the ioctl param struct in the register struct
	regs.RESI = (DWORD)&io;
	
	// Send the message to the target device
	Directed_Sys_Control(pDdb, W32_DEVICEIOCONTROL, &regs);

	*pRetSize =BytesReturned;
	
	// Return the error code received back from the target device.
	return regs.REAX;
}

// -----------------------------------------------------------------------------------------
BOOLEAN GeneralStart_ExternalDrv(ULONG Ioctl_GetVersion, ULONG Ioctl_StartFiltering, PCHAR pcDrvNAME, ULONG ApiVersion, ULONG HookID, BOOLEAN *pbStarted)
{
	ULONG ExtDrvApiVersion = 0;
	EXT_START_FILTERING	StartFiltering;
	
	if(*pbStarted == TRUE)
	{
		DbPrint(DC_FWALL,DL_WARNING, ("%s is already started.\n", pcDrvNAME));
		return TRUE;
	}
	
	if(SendDevIoCtl2DrvByName(pcDrvNAME, Ioctl_GetVersion, NULL, 0, &ExtDrvApiVersion, sizeof(ExtDrvApiVersion), NULL) == 0)
	{
		if(ExtDrvApiVersion == ApiVersion)
		{
			StartFiltering.m_HookID = HookID;
			InitStartFiltering(&StartFiltering);
			if(SendDevIoCtl2DrvByName(pcDrvNAME, Ioctl_StartFiltering, &StartFiltering, sizeof(StartFiltering), NULL,0, NULL) == 0)
			{
				DbPrint(DC_FWALL,DL_NOTIFY, ("%s succes.\n", pcDrvNAME));
				*pbStarted = TRUE;
				return TRUE;
			}
			else
			{
				DbPrint(DC_FWALL,DL_ERROR, ("%s start filtering failed.\n", pcDrvNAME));
			}
		}
		else
		{
			DbPrint(DC_FWALL,DL_ERROR, ("%s/avpg API versions different. %d!=%d\n",pcDrvNAME, ExtDrvApiVersion, ApiVersion));
		}
	}
	else
	{
		DbPrint(DC_FWALL,DL_ERROR, ("SendDevIoCtl2DrvByName failed.\n"));
	}
	return FALSE;
	
}

BOOLEAN GeneralStop_ExternalDrv(ULONG Ioctl_StopFiltering, PCHAR pcDrvNAME, ULONG HookID, BOOLEAN *pbStarted)
{
	if(*pbStarted == FALSE)
	{
		DbPrint(DC_FWALL,DL_WARNING, ("%s is not started.\n", pcDrvNAME));
		return FALSE;
	}
	
	if(SendDevIoCtl2DrvByName(pcDrvNAME, Ioctl_StopFiltering, &HookID, sizeof(HookID), NULL,0, NULL) == 0)
	{
		DbPrint(DC_FWALL,DL_NOTIFY, ("%s succes.\n", pcDrvNAME));
		*pbStarted = FALSE;
		return TRUE;
	}
	
	return FALSE;
}

// -----------------------------------------------------------------------------------------

BOOLEAN KLMCStarted = FALSE;
BOOLEAN KLMCStart()
{
	return GeneralStart_ExternalDrv(KLMC_GET_VERSION, KLMC_START_FILTER, KLMC_NAME_9x, KLMC_BUILDNUM, FLTTYPE_MCHECK, &KLMCStarted);
}

BOOLEAN KLMCStop()
{
	return GeneralStop_ExternalDrv(KLMC_STOP_FILTER, KLMC_NAME_9x, FLTTYPE_MCHECK, &KLMCStarted);
}

// -----------------------------------------------------------------------------------------

BOOLEAN IDSStarted = FALSE;
BOOLEAN IDSStart()
{
	return GeneralStart_ExternalDrv(PID_IOCTL_GETVERSION, PID_IOCTL_START_FILTER, KLPID_NAME_9x, KLPID_BUILDNUM, FLTTYPE_IDS, &IDSStarted);
}

BOOLEAN IDSStop()
{
	return GeneralStop_ExternalDrv(PID_IOCTL_STOP_FILTER, KLPID_NAME_9x, FLTTYPE_IDS, &IDSStarted);
}

// -----------------------------------------------------------------------------------------

BOOLEAN PIDStarted = FALSE;
BOOLEAN PIDStart()
{
	return GeneralStart_ExternalDrv(PID_IOCTL_GETVERSION, PID_IOCTL_START_FILTER, KLPID_NAME_9x, KLPID_BUILDNUM, FLTTYPE_PID, &PIDStarted);
}

BOOLEAN PIDStop()
{
	return GeneralStop_ExternalDrv(PID_IOCTL_STOP_FILTER, KLPID_NAME_9x, FLTTYPE_PID, &PIDStarted);
}

// -----------------------------------------------------------------------------------------
BOOLEAN KLPFStarted = FALSE;
BOOLEAN KLPFStart(VOID)
{
	return GeneralStart_ExternalDrv(KLPF_GET_VERSION, KLPF_START_FILTER, KLPF_NAME_9x, KLPF_BUILDNUM, FLTTYPE_KLPF, &KLPFStarted);
}

BOOLEAN KLPFStop(VOID)
{
	return GeneralStop_ExternalDrv(KLPF_STOP_FILTER, KLPF_NAME_9x, FLTTYPE_KLPF, &KLPFStarted);
}


//+ ------------------------------------------------------------------------------------------
BOOLEAN Klick = FALSE;
BOOLEAN KlickStart(VOID)
{
	return GeneralStart_ExternalDrv(KLICK_GET_VERSION, KLICK_START_FILTER, KLICK_NAME_9x, KLICK_API_VERSION, FLTTYPE_KLICK, &Klick);
}

BOOLEAN KlickStop(VOID)
{
	return GeneralStop_ExternalDrv(KLICK_STOP_FILTER, KLICK_NAME_9x, FLTTYPE_KLICK, &Klick);	
}

VOID
KlickRefreshData()
{
	ULONG HookID = FLTTYPE_KLICK;
	SendDevIoCtl2DrvByName(KLICK_NAME_9x, KLICK_RESET_TRUSTED_STREAMS, &HookID, sizeof(HookID), NULL,0, NULL);
}


BOOLEAN Klin = FALSE;
BOOLEAN KlinStart(VOID)
{
	return GeneralStart_ExternalDrv(KLIN_GET_VERSION, KLIN_START_FILTER, KLIN_NAME_9x, KLIN_API_VERSION, FLTTYPE_KLIN, &Klin);
}

BOOLEAN KlinStop(VOID)
{
	return GeneralStop_ExternalDrv(KLIN_STOP_FILTER, KLIN_NAME_9x, FLTTYPE_KLIN, &Klin);	
}
//+ ------------------------------------------------------------------------------------------
NTSTATUS DoSystemRequest(PEXTERNAL_DRV_REQUEST pInRequest, PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;
		
	switch (pInRequest->m_IOCTL)
	{
		// -----------------------------------------------------------------------------------------
	case _AVPG_IOCTL_STAT_DISK:
		if (OutRequestSize < (sizeof(unsigned __int64) * 2))
			ntStatus = STATUS_BUFFER_TOO_SMALL;
		else
		{
			LONG cr;
			unsigned __int64* pu = (unsigned __int64*) pOutRequest;
			*pRetSize = sizeof(unsigned __int64) * 2;
			cr = InterlockedIncrement(&g_C);
			InterlockedDecrement(&g_C);
			if (cr > 1)
			{
				unsigned __int64 t = GetCurTimeFast();
				g_TW += (t - g_T); // increment wait time by delta
				g_T = t; // set new start period
				pu[0] = t;
				pu[1] = g_TW;
			}
			else
			{
				*(pu + 1) = 0;
				while (*(pu + 1) != g_TW)
					*(pu + 1) = g_TW;
				*pu = GetCurTimeFast();
			}

			ntStatus = STATUS_SUCCESS;
		}
		break;
// -----------------------------------------------------------------------------------------		
	default:
		DbPrint(DC_SYS, DL_WARNING, ("Unknown request\n"));
		DbgBreakPoint();
		break;
	}

	DbPrint(DC_SYS, DL_SPAM, ("DoSystemRequest - result %#x\n", ntStatus));	

	return ntStatus;
}


NTSTATUS
DoExternalDrvRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutRequest,
	ULONG OutRequestSize,
	ULONG *pRetSize
	)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	
	PQCLIENT pClient = AcquireClient(pInRequest->m_AppID);
	if (pClient == NULL)
	{
		DbPrint(DC_IOCTL, DL_NOTIFY, ("Request for DrvId %d. Ioctl %#x (%d) failed - can't find AppID %d\n", pInRequest->m_DrvID, pInRequest->m_IOCTL, pInRequest->m_IOCTL, pInRequest->m_AppID));
		return STATUS_NOT_FOUND;
	}

	ReleaseClient(pClient);
	
	DbPrint(DC_IOCTL, DL_NOTIFY, ("Request for DrvId %d. Ioctl %#x (%d)\n", pInRequest->m_DrvID, pInRequest->m_IOCTL, pInRequest->m_IOCTL));

	switch (pInRequest->m_DrvID)
	{
	case FLTTYPE_PID:
		ntStatus = SendDevIoCtl2DrvByName(KLPID_NAME_9x ,pInRequest->m_IOCTL, pInRequest->m_Buffer, pInRequest->m_BufferSize, pOutRequest, OutRequestSize, pRetSize);
		break;
	case FLTTYPE_KLPF:
		ntStatus = SendDevIoCtl2DrvByName(KLPF_NAME_9x ,pInRequest->m_IOCTL, pInRequest->m_Buffer, pInRequest->m_BufferSize, pOutRequest, OutRequestSize, pRetSize);
		break;
	case FLTTYPE_IOS:
		ntStatus = DoDiskRequest(pInRequest, pOutRequest, OutRequestSize, pRetSize);
		break;
	case FLTTYPE_MCHECK:
		ntStatus = SendDevIoCtl2DrvByName(KLMC_NAME_9x ,pInRequest->m_IOCTL, pInRequest->m_Buffer, pInRequest->m_BufferSize, pOutRequest, OutRequestSize, pRetSize);
		break;
	case FLTTYPE_SYSTEM:
		ntStatus = DoSystemRequest(pInRequest, pOutRequest, OutRequestSize, pRetSize);
		break;
	default:
		DbPrint(DC_IOCTL, DL_ERROR, ("Unknown DrvId %d. Ioctl %#x (%d)\n", pInRequest->m_DrvID, pInRequest->m_IOCTL, pInRequest->m_IOCTL));
		break;
	}

	return ntStatus;
}
