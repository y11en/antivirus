#ifndef __fidbox_h
#define __fidbox_h

#include "pch.h"

void
Fid_GenericInit (
	);

void
GlobalFidBoxInit (
	);

void
GlobalFidBoxDone (
	BOOLEAN bDeleteFiles
	);

ULONG
Fid_GetItemSize1 (
	);

NTSTATUS
FidBox_SetData (
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	BOOLEAN bForce,
	BOOLEAN bResetRequest
	);

NTSTATUS
FidBox_SetDataByFO (
	PFILE_OBJECT pFileObject,
	BOOLEAN bAllowRequestFileId,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize
	);

NTSTATUS
FidBox_GetDataByFO (
	PFILE_OBJECT pFileObject,
	BOOLEAN bAllowRequestFileId,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	PULONG pRetSize,
	BOOLEAN bResetID
	);

NTSTATUS
FidBox_GetData (
	PFIDBOX_REQUEST_GET pFidGet,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	PULONG pRetSize
	);

VOID
FidBox_ResetValid (
	PMKAV_OBJ_ID pVolumeId,
	PMKAV_OBJ_ID pFileId
	);

BOOLEAN
FidBox_IsSelf (
	PFILE_OBJECT pFO
	);

void
FidBox2_Update(
	PMKAV_OBJ_ID pVolumeId,
	PMKAV_OBJ_ID pFileId
	);

NTSTATUS
FidBox2_GetByFO (
	PFILE_OBJECT pFileObject,
	PULONG pData
	);

// serv funcs

__int64
GetTimeFast (
	);

#endif // __fidbox_h
