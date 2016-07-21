#ifndef	__REGSOPS_H
#define __REGSOPS_H

NTSTATUS
DoRegsRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutBuffer, 
	ULONG OutBufferSize,
	PULONG pRetSize
	);

#endif