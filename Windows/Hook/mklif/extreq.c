#include "pch.h"

NTSTATUS
DoExternalDrvRequest(
					 PEXTERNAL_DRV_REQUEST pInRequest,
					 ULONG InRequestSize,
					 PVOID pOutBuffer, 
					 ULONG OutBufferSize,
					 PULONG pRetSize
					 )
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

	switch (pInRequest->m_DrvID)
	{
	case FLTTYPE_DISK:
		ntStatus = DoDiskRequest(pInRequest, InRequestSize, pOutBuffer, OutBufferSize, pRetSize);
		break;

	case FLTTYPE_REGS:
		ntStatus = DoRegsRequest(pInRequest, InRequestSize, pOutBuffer, OutBufferSize, pRetSize);
		break;

	case FLTTYPE_SYSTEM:
		ntStatus = DoSysRequest(pInRequest, pOutBuffer, OutBufferSize, pRetSize);
		break;

	default:
		//_dbg_break_;
		break;
	}

	return ntStatus;
}
