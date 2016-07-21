#include "pch.h"

#include "ichdrv.tmh"

NTSTATUS
IchDrv_FillFileIndex (
	__in PFLT_INSTANCE pInstance,
	__in PFILE_OBJECT pFileObject,
	__inout PMKAV_OBJ_ID pFileId
	)
{
	NTSTATUS					status;
	FILE_INTERNAL_INFORMATION	InternalInfo;
	ULONG						RetBytes;

	int Object_Size1 = RTL_FIELD_SIZE(FILE_OBJECTID_BUFFER, ObjectId);
	int Object_Size2 = RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId);

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED ))
		return STATUS_NOT_SUPPORTED;

	if (Object_Size1 != Object_Size2)
	{
		_dbg_break_;
		return STATUS_NOT_SUPPORTED;
	}

	status = MySynchronousQueryInformationFile (
		pInstance,
		pFileObject,
		&InternalInfo,
		sizeof(InternalInfo),
		FileInternalInformation,
		&RetBytes
		);

	if (NT_SUCCESS( status ))
	{
		RtlZeroMemory( pFileId, sizeof(MKAV_OBJ_ID) );
		
		RtlCopyMemory (
			pFileId->m_ObjectId,
			&InternalInfo.IndexNumber,
			RTL_FIELD_SIZE( FILE_INTERNAL_INFORMATION, IndexNumber )
			);
	}

	return status;
}


NTSTATUS
IchDrv_FillFileId (
	__in PFLT_INSTANCE pInstance,
	__in PFILE_OBJECT pFileObject,
	__inout PMKAV_OBJ_ID pFileId
	)
{
	NTSTATUS				status;
	FILE_OBJECTID_BUFFER	oid;
	ULONG					RetBytes;

	int Object_Size1 = RTL_FIELD_SIZE(FILE_OBJECTID_BUFFER, ObjectId);
	int Object_Size2 = RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId);

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED ))
		return STATUS_NOT_SUPPORTED;

	if (Object_Size1 != Object_Size2)
	{
		_dbg_break_;
		return STATUS_NOT_SUPPORTED;
	}

	status = FltFsControlFile(
		pInstance,
		pFileObject,
		FSCTL_CREATE_OR_GET_OBJECT_ID,
		NULL,
		0,
		&oid,
		sizeof(oid),
		&RetBytes
		);

	if (NT_SUCCESS( status ))
		memcpy( pFileId->m_ObjectId, oid.ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );

	return status;
}

void
IchDrv_AttachFileId (
	__in PFLT_INSTANCE pInstance,
	__in PFILE_OBJECT pFileObject,
	__in PMKAV_STREAM_CONTEXT pcontext
	)
{
	NTSTATUS status;

	PMKAV_OBJ_ID pFileId = NULL;

	if (!pInstance || !pFileObject)
	{
		_dbg_break_;
		return;
	}

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED ))
		return;

	if (pcontext->m_pFileId)
	{
		_dbg_break_;
		return;
	}

	pFileId = ExAllocatePoolWithTag( PagedPool, sizeof(MKAV_OBJ_ID), 'fBos' );
	if (pFileId)
	{
		if (FlagOn(Globals.m_DrvFlags, _DRV_FLAG_FILEID_INDEX ))
		{
			status = IchDrv_FillFileIndex(
				pInstance,
				pFileObject,
				pFileId
				);
		}
		else
		{
			status = IchDrv_FillFileId(
				pInstance,
				pFileObject,
				pFileId
				);
		}

		if (NT_SUCCESS( status ))
			pcontext->m_pFileId = pFileId;
		else
		{
			ExFreePool( pFileId );
			pFileId = NULL;
		}
	}
}

void
IchDrv_ResetValid (
	__in PFLT_INSTANCE pInstance,
	__in PFLT_CALLBACK_DATA Data,
	__in PMKAV_INSTANCE_CONTEXT pInstanceContext,
	__in PMKAV_STREAM_CONTEXT pcontext
	)
{
	UNREFERENCED_PARAMETER( pInstance );

	if (!pcontext)
		return;

	if (!pcontext->m_pFileId)
		return;		// no file id

	if (FidBox_IsSelf(Data->Iopb->TargetFileObject))
		return;

	if (pcontext->m_pFileNameInfo)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_ICHDRV, "reset valid %wZ",
			&pcontext->m_pFileNameInfo->Name );
	}

	FidBox_ResetValid( &pInstanceContext->m_VolumeId, pcontext->m_pFileId );
}

