#ifndef __lldio
#define __lldio

#include "..\hook\avpgcom.h"

NTSTATUS
DoDiskRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutBuffer, 
	ULONG OutBufferSize,
	PULONG pRetSize
	);

NTSTATUS
LLD_QueryNames (
	PVOID pOutBuffer,
	ULONG OutBufferSize,
	PULONG pRetSize
	);

NTSTATUS
LLD_GetInfo (
	PUNICODE_STRING pusName,
	PMKLIF_LLD_INFO pInfo
	);

NTSTATUS
LLD_GetGeometry (
	PUNICODE_STRING pusName,
	PMKLIF_LLD_GEOMETRY pGeometry
	);

NTSTATUS
LLD_GetDiskId (
	PUNICODE_STRING pusName,
	PDISK_ID_INFO pDiskIdInfo
	);

NTSTATUS
LLD_Read (
	PUNICODE_STRING pusName,
	__int64 Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytes
	);

NTSTATUS
LLD_Write (
	PUNICODE_STRING pusName,
	__int64 Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytes
	);

#endif __lldio