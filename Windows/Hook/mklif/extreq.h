#ifndef __extreq
#define __extreq

#include "..\hook\avpgcom.h"

NTSTATUS
DoExternalDrvRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutBuffer, 
	ULONG OutBufferSize,
	PULONG pRetSize
	);

#endif __extreq