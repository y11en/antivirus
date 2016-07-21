#ifndef __ichdrv_h
#define __ichdrv_h

#include "pch.h"

NTSTATUS
IchDrv_FillFileId (
	__in PFLT_INSTANCE pInstance,
	__in PFILE_OBJECT pFileObject,
	__inout PMKAV_OBJ_ID pFileId
	);

void
IchDrv_ResetValid (
	__in PFLT_INSTANCE pInstance,
	__in PFLT_CALLBACK_DATA Data,
	__in PMKAV_INSTANCE_CONTEXT pInstanceContext,
	__in PMKAV_STREAM_CONTEXT pcontext
	);

void
IchDrv_AttachFileId (
	__in PFLT_INSTANCE pInstance,
	__in PFILE_OBJECT pFileObject,
	__in PMKAV_STREAM_CONTEXT pcontext
	);

#endif __ichdrv_h