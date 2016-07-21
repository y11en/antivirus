#include "fidbox.h"
#include "oidb.h"
#include "structures.h"

#include "fidbox.tmh"

volatile BOOLEAN gbFidBoxIniting = FALSE;

tHeap gFidBoxHeap;

typedef struct _sFidBase
{
	OBJID_CTX*	m_pFidBox;
	tFile		m_FidBoxFileData;
	tFile		m_FidBoxFileIndex;
	ULONG		m_FidBoxItemSize;
}FidBase;

FidBase gFidBox1;
FidBase gFidBox2;

ULONG
Fid_GetItemSize1 (
	)
{
	return gFidBox1.m_FidBoxItemSize;
}

//+  heap funcions

bool
__stdcall
Fid_HeapAlloc (
	tHeap* pHeap,
	size_t size,
	void** ppMem,
	ULONG tag
	)
{
	void* pMem = NULL;
	
	UNREFERENCED_PARAMETER( pHeap );

	if (!ppMem)
		return false;

	pMem = ExAllocatePoolWithTag( PagedPool, size, tag );
	if (pMem)
		memset(pMem, 0, size);
	
	*ppMem = pMem;

	return (pMem != NULL);
}

bool
__stdcall
Fid_HeapFree (
	tHeap* pHeap,
	void* pMem
	)
{
	UNREFERENCED_PARAMETER( pHeap );

	if (!pMem)
		return false;
	
	ExFreePool(pMem);

	return true;
}

bool
__stdcall
Fid_HeapRealloc (
	tHeap* pHeap,
	void* pMem,
	size_t size,
	void** ppMem
	)
{
	UNREFERENCED_PARAMETER( pHeap );
	UNREFERENCED_PARAMETER( pMem );
	UNREFERENCED_PARAMETER( size );
	UNREFERENCED_PARAMETER( ppMem );

	_dbg_break_;
	return false;
}

void Fid_InitHeap(tHeap* pHeap, HANDLE hHeap)
{
	pHeap->hHeap = hHeap;
	pHeap->Alloc = Fid_HeapAlloc;
	pHeap->Realloc = Fid_HeapRealloc;
	pHeap->Free = Fid_HeapFree;
}

// end heap funcions

void
Fid_GenericInit (
	)
{
	RtlZeroMemory( &gFidBox1, sizeof(gFidBox1) );
	RtlZeroMemory( &gFidBox2, sizeof(gFidBox1) );
	gFidBox2.m_FidBoxItemSize = 4;
	
	Fid_InitHeap( &gFidBoxHeap, 0 );
}

bool
__stdcall
Fid_FileRead (
	tFile* pFile,
	unsigned char* pData,
	size_t nSize,
	size_t* pnBytesRead
	)
{
	NTSTATUS status;
	ULONG Rb = 0;

	status = FltReadFile( 
		Globals.m_SystemVolumeInstance,
		pFile->FileAccess,
		NULL,
		nSize,
		pData,
		0,
		&Rb,
		NULL,
		NULL
		);

	if (pnBytesRead)
		*pnBytesRead = Rb;

	if (NT_SUCCESS( status ))
		return true;

	return false;
}

bool
__stdcall
Fid_FileWrite (
	tFile* pFile,
	unsigned char* pData,
	size_t nSize,
	size_t* pnBytesWritten
	)
{
	NTSTATUS status;
	ULONG Wb = 0;

	status = FltWriteFile( 
		Globals.m_SystemVolumeInstance,
		pFile->FileAccess,
		NULL,
		nSize,
		pData,
		0,
		&Wb,
		NULL,
		NULL
		);

	if (pnBytesWritten)
		*pnBytesWritten = Wb;

	if (NT_SUCCESS( status ))
		return true;

	return false;
}

bool
__stdcall
Fid_FileSeek (
	tFile* pFile,
	tFilePos nPos,
	int nSeekMethod,
	tFilePos* pnNewPos
	)
{
	NTSTATUS status;
	
	FILE_POSITION_INFORMATION filePositionInformation;

	if (SEEK_SET != nSeekMethod)
	{
		_dbg_break_;
		return false;
	}
	
	filePositionInformation.CurrentByteOffset.LowPart = nPos;
	filePositionInformation.CurrentByteOffset.HighPart = 0;
	
	status = FltSetInformationFile (
		Globals.m_SystemVolumeInstance, 
		pFile->FileAccess, 
		&filePositionInformation,
		sizeof(filePositionInformation),
		FilePositionInformation
		);

	if (NT_SUCCESS( status ))
	{
		if (pnNewPos)
			*pnNewPos = nPos;

		return true;
	}

	return false;
}

bool
__stdcall
Fid_FileSeekRead (
	tFile* pFile,
	tFilePos nPos,
	unsigned char* pData,
	size_t nSize,
	size_t* pnBytesRead
	)
{
	if (!Fid_FileSeek(pFile, nPos, SEEK_SET, NULL))
		return false;

	return Fid_FileRead(pFile, pData, nSize, pnBytesRead);
}

bool
__stdcall
Fid_FileSeekWrite (
	tFile* pFile,
	tFilePos nPos,
	unsigned char* pData,
	size_t nSize,
	size_t* pnBytesWritten
	)
{
	if (!Fid_FileSeek(pFile, nPos, SEEK_SET, NULL))
		return false;
	
	return Fid_FileWrite(pFile, pData, nSize, pnBytesWritten);
}


bool
__stdcall
Fid_FileGetSize (
	tFile* pFile,
	tFileSize* pnSize
	)
{
	NTSTATUS status;
	
	FILE_STANDARD_INFORMATION StandartInformation;
	ULONG RetLen = 0;

	status = MySynchronousQueryInformationFile (
		Globals.m_SystemVolumeInstance,
		pFile->FileAccess,
		&StandartInformation,
		sizeof(StandartInformation),
		FileStandardInformation,
		&RetLen
		);

	if (NT_SUCCESS( status ))
	{
		*pnSize = StandartInformation.EndOfFile.LowPart;
		return true;
	}

	return false;
}

bool
__stdcall
Fid_FileSetSize (
	tFile* pFile,
	tFileSize nSize
	)
{
	UNREFERENCED_PARAMETER( pFile );
	UNREFERENCED_PARAMETER( nSize );

	_dbg_break_;
	return false;
}

bool
__stdcall
Fid_FileFlush (
	tFile* pFile
	)
{
	NTSTATUS status;

	status = FltFlushBuffers( Globals.m_SystemVolumeInstance, pFile->FileAccess );

	if (NT_SUCCESS( status ))
		return true;

	return false;
}

bool
__stdcall
Fid_FileClose (
	tFile* pFile
	)
{
	if (pFile->FileAccess)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "close fidbox file handle" );

		ObDereferenceObject( pFile->FileAccess );
		FltClose( pFile->hFile );

		pFile->FileAccess = NULL;
	}
	
	return true;
}

void
__stdcall
Fid_InitFile (
	tFile* pFile,
	tFileHandle pFO,
	HANDLE hFile
	)
{
	pFile->FileAccess = pFO;
	pFile->hFile = hFile;
	pFile->SeekRead = Fid_FileSeekRead;
	pFile->SeekWrite = Fid_FileSeekWrite;
	pFile->Read = Fid_FileRead;
	pFile->Write = Fid_FileWrite;
	pFile->Seek = Fid_FileSeek;
	pFile->GetSize = Fid_FileGetSize;
	pFile->SetSize = Fid_FileSetSize;
	pFile->Flush = Fid_FileFlush;
	pFile->Close = Fid_FileClose;
}

/*void
FidCloseAllFiles (
	)
{
	gFileAccessDbf = NULL;
	gFileAccessIdx = NULL;

	ObDereferenceObject( gFidBoxFileData.FileAccess );
	ObDereferenceObject( gFidBoxFileIndex.FileAccess );

	FltClose( gFidBoxFileData.hFile );
	FltClose( gFidBoxFileIndex.hFile );
}*/

// end init functions

NTSTATUS
FidBox_Stub (
	FidBase* pFidBox,
	PWSTR pwstrUniNameDat,
	PWSTR pwstrUniNameIdx
	)
{
	NTSTATUS status;

	PFLT_INSTANCE pSystemVolumeInstance = NULL;

	HANDLE FileHandleDat;
	PFILE_OBJECT pFileObjectDat = NULL;
	HANDLE FileHandleIdx;
	PFILE_OBJECT pFileObjectIdx = NULL;

	ULONG FidBoxItemSize;

	OBJECT_ATTRIBUTES	ObjectAttributes;
	IO_STATUS_BLOCK		ioStatus;
	UNICODE_STRING		usFileName;

	HRESULT hResult;

	pSystemVolumeInstance = GetSystemVolumeInstance();
	
	if (!pSystemVolumeInstance)
		return STATUS_UNSUCCESSFUL;

	RtlInitUnicodeString( &usFileName, pwstrUniNameDat );
	InitializeObjectAttributes( &ObjectAttributes, &usFileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

	status = FltCreateFile (
		Globals.m_Filter,
		pSystemVolumeInstance, 
		&FileHandleDat,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE | DELETE,
		&ObjectAttributes,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM,
		0,
		FILE_OPEN_IF,
		FILE_RANDOM_ACCESS | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		0
		); 

	if (!NT_SUCCESS( status ))
	{
		FltObjectDereference( pSystemVolumeInstance );	

		DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "%S open failed. status %!STATUS!",
			pwstrUniNameDat, status );

		return STATUS_UNSUCCESSFUL;
	}

	ObReferenceObjectByHandle (
		FileHandleDat,
		0,
		*IoFileObjectType,
		KernelMode,
		&pFileObjectDat,
		NULL
		);

	RtlInitUnicodeString( &usFileName, pwstrUniNameIdx );

	status = FltCreateFile (
		Globals.m_Filter,
		pSystemVolumeInstance, 
		&FileHandleIdx,
		FILE_GENERIC_READ | FILE_GENERIC_WRITE | DELETE,
		&ObjectAttributes,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM,
		0,
		FILE_OPEN_IF,
		FILE_RANDOM_ACCESS | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		0
		);

	FltObjectDereference( pSystemVolumeInstance );
	pSystemVolumeInstance = NULL; // no more need

	if (!NT_SUCCESS( status ))
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "%S open failed. status %!STATUS!",
			pwstrUniNameDat, status );

		FltClose( FileHandleDat );

		return STATUS_UNSUCCESSFUL;
	}

	ObReferenceObjectByHandle (
		FileHandleIdx,
		0,
		*IoFileObjectType,
		KernelMode,
		&pFileObjectIdx,
		NULL
		);

	Fid_InitFile( &pFidBox->m_FidBoxFileData, pFileObjectDat, FileHandleDat );
	Fid_InitFile( &pFidBox->m_FidBoxFileIndex, pFileObjectIdx, FileHandleIdx );

	FidBoxItemSize = pFidBox->m_FidBoxItemSize;
	hResult = OBJID_Init( &gFidBoxHeap, &pFidBox->m_FidBoxFileData, &pFidBox->m_FidBoxFileIndex, pFidBox->m_FidBoxItemSize, &pFidBox->m_pFidBox, &FidBoxItemSize );
	pFidBox->m_FidBoxItemSize = FidBoxItemSize;

	if (ERROR_SUCCESS == hResult)
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "GlobalFidBoxInit: item size %d", pFidBox->m_FidBoxItemSize );

		return STATUS_SUCCESS;
	}

	Fid_FileClose( &pFidBox->m_FidBoxFileData );
	Fid_FileClose( &pFidBox->m_FidBoxFileIndex );

	return STATUS_NOT_SUPPORTED;
}


NTSTATUS
FidBox_InitImp (
	)
{
	PWSTR pwstrUniNameDat = L"\\SystemRoot\\system32\\drivers\\fidbox.dat";
	PWSTR pwstrUniNameIdx = L"\\SystemRoot\\system32\\drivers\\fidbox.idx";

	NTSTATUS status = FidBox_Stub( &gFidBox1, pwstrUniNameDat, pwstrUniNameIdx );

	return status;
}

NTSTATUS
FidBox2_InitImp (
	)
{
	PWSTR pwstrUniNameDat = L"\\SystemRoot\\system32\\drivers\\fidbox2.dat";
	PWSTR pwstrUniNameIdx = L"\\SystemRoot\\system32\\drivers\\fidbox2.idx";

	NTSTATUS status = FidBox_Stub( &gFidBox2, pwstrUniNameDat, pwstrUniNameIdx );

	return status;
}

void
GlobalFidBoxInit (
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	BOOLEAN bNeedInit = TRUE;

	AcquireResourceExclusive( &Globals.m_FidBoxLock );

	if (gbFidBoxIniting || gFidBox1.m_pFidBox || FlagOn( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED ))
		bNeedInit = FALSE;
	else
		gbFidBoxIniting = TRUE;

	ReleaseResource( &Globals.m_FidBoxLock );

	if (!bNeedInit)
		return;

	DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "GlobalFidBoxInit..." );

	status = FidBox_InitImp();
	DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "GlobalFidBoxInit status %!STATUS!", status );
	status = FidBox2_InitImp();
	DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "GlobalFidBox2Init status %!STATUS!", status );

	AcquireResourceExclusive( &Globals.m_FidBoxLock );
	gbFidBoxIniting = FALSE;
	ReleaseResource( &Globals.m_FidBoxLock );
}

void
FidBoxDoneImp (
	FidBase* pFidBox,
	BOOLEAN bDeleteFiles
	)
{
	if (!pFidBox->m_pFidBox)
		return;

	if (bDeleteFiles)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		FILE_DISPOSITION_INFORMATION Disposition;
		
		OBJID_Done( pFidBox->m_pFidBox, OBJID_FLAG_DONTCLOSE_FILE );

		Disposition.DeleteFile = TRUE;

		status = FltSetInformationFile (
			Globals.m_SystemVolumeInstance,
			pFidBox->m_FidBoxFileData.FileAccess,
			&Disposition,
			sizeof(Disposition),
			FileDispositionInformation
		);

		status = FltSetInformationFile (
			Globals.m_SystemVolumeInstance,
			pFidBox->m_FidBoxFileIndex.FileAccess,
			&Disposition,
			sizeof(Disposition),
			FileDispositionInformation
		);

		Fid_FileClose( &pFidBox->m_FidBoxFileData );
		Fid_FileClose( &pFidBox->m_FidBoxFileIndex );
	}
	else
		OBJID_Done( pFidBox->m_pFidBox, OBJID_FLAG_FLUSH );
	
	pFidBox->m_pFidBox = NULL;
}

void
GlobalFidBoxDone (
	BOOLEAN bDeleteFiles
	)
{
	DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "GlobalFidBoxDone..." );

	SetFlag( Globals.m_DrvFlags, _DRV_FLAG_ISWIFT_DISABLED );

	AcquireResourceExclusive( &Globals.m_FidBoxLock );
	while (gbFidBoxIniting)
	{
		_dbg_break_;
		ReleaseResource( &Globals.m_FidBoxLock );

		SleepImp( 1 );
		
		AcquireResourceExclusive( &Globals.m_FidBoxLock );
	}
	
	FidBoxDoneImp( &gFidBox1, bDeleteFiles );
	FidBoxDoneImp( &gFidBox2, bDeleteFiles );

	ReleaseResource( &Globals.m_FidBoxLock );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_SYSTEM, "Fidbox disabled" );
	DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "GlobalFidBoxDone" );
}

NTSTATUS
FidBox_QueryInfo (
	PFILE_OBJECT pFileObject,
	PMKAV_OBJ_ID pVolumeId,
	PMKAV_OBJ_ID pFileId,
	BOOLEAN bAllowRequestFileId,
	PBOOLEAN pbNetwork
	)
{
	NTSTATUS status;

	PMKAV_VOLUME_CONTEXT pVolumeContext = NULL;
	PMKAV_INSTANCE_CONTEXT pInstanceContext = NULL;
	BOOLEAN bFileIdReady = FALSE;

	if (pbNetwork)
		*pbNetwork = FALSE;

	{
		PFLT_VOLUME pVolume = NULL;
		status = FltGetVolumeFromFileObject( Globals.m_Filter, pFileObject, &pVolume );
		
		if (!NT_SUCCESS( status ))
			return STATUS_UNSUCCESSFUL;

		status = FltGetVolumeContext( Globals.m_Filter, pVolume, &pVolumeContext );
		FltObjectDereference( pVolume );

		if (!NT_SUCCESS( status ))
			return STATUS_UNSUCCESSFUL;
	}

	status = FltObjectReference( pVolumeContext->m_pFltInstance);

	if (!NT_SUCCESS( status ))
	{
		ReleaseFltContext( &pVolumeContext );
		return STATUS_UNSUCCESSFUL;
	}

	status = FltGetInstanceContext( pVolumeContext->m_pFltInstance, &pInstanceContext );
	if (NT_SUCCESS( status ))
	{
		if (FlagOn(pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_FLAG_NETWORK_DEVICE ))
		{
			if (pbNetwork)
				*pbNetwork = TRUE;
		}
		else if (FlagOn( pInstanceContext->m_ContextFlags, _CONTEXT_OBJECT_HAS_OBJID ))
		{
			RtlCopyMemory( pVolumeId, &pInstanceContext->m_VolumeId, sizeof(MKAV_OBJ_ID) );

			if (FsRtlSupportsPerStreamContexts( pFileObject) )
			{
				PMKAV_STREAM_CONTEXT pContext = GetStreamContext( pVolumeContext->m_pFltInstance, pFileObject );
				if (pContext)
				{
					if (pContext->m_NumberOfLinks > 1)
					{
						// exist hardlinks
					}
					if (FlagOn( pContext->m_Flags, _STREAM_FLAGS_MODIFIED )) // working during write
					{
						// no file id attached - skip 
					}
					else if (FlagOn( pContext->m_Flags, _STREAM_FLAGS_TRANSACTED )) // working during transaction
					{
						_dbg_break_;
					}
					else
					{
						if (pContext->m_pFileId)
						{
							RtlCopyMemory( pFileId, pContext->m_pFileId, sizeof(MKAV_OBJ_ID) );
							bFileIdReady = TRUE;
						}
						else
						{
							_dbg_break_;
						}
					}

					ReleaseFltContext( &pContext );
				}
				else
				{
					if (bAllowRequestFileId)
					{
						status = IchDrv_FillFileId( pVolumeContext->m_pFltInstance, pFileObject, pFileId );
						if (NT_SUCCESS( status ))
							bFileIdReady = TRUE;
					}
				}
			}
			else
			{
				_dbg_break_;
				status = IchDrv_FillFileId( pVolumeContext->m_pFltInstance, pFileObject, pFileId );
				if (NT_SUCCESS( status ))
					bFileIdReady = TRUE;
			}
		}

		ReleaseFltContext( &pInstanceContext );
	}

	FltObjectDereference( pVolumeContext->m_pFltInstance );
	ReleaseFltContext( &pVolumeContext );

	if (bFileIdReady)
		return STATUS_SUCCESS;

	return STATUS_UNSUCCESSFUL;
}

NTSTATUS
FidBox_SetData (
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	BOOLEAN bForce,
	BOOLEAN bResetRequest
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	MKAV_OBJ_ID VolumeId;
	MKAV_OBJ_ID FileId;

	if (!gFidBox1.m_pFidBox)
		return STATUS_UNSUCCESSFUL;

	if (bForce)
	{
		memcpy( VolumeId.m_ObjectId, pFidData->m_VolumeID, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
		memcpy( FileId.m_ObjectId, pFidData->m_FileID, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
	}
	else
	{
		ULONG inFile;
		HANDLE hFile = NULL;
		PFILE_OBJECT pFileObject;

		inFile = *(ULONG*) pFidData->m_FileID;

		*(ULONG*)&hFile = inFile;

		status = ObReferenceObjectByHandle(hFile, SYNCHRONIZE, *IoFileObjectType, UserMode, (PVOID*) &pFileObject, NULL);
		if (!NT_SUCCESS( status ))
			return STATUS_UNSUCCESSFUL;

		status = FidBox_QueryInfo( pFileObject, &VolumeId, &FileId, FALSE, NULL );
		
		ObDereferenceObject( pFileObject );

		if (!NT_SUCCESS( status ))
			return STATUS_UNSUCCESSFUL;
	}

	AcquireResourceExclusive( &Globals.m_FidBoxLock );

	status = STATUS_NOT_FOUND;

	if(gFidBox1.m_pFidBox && gFidBox1.m_FidBoxItemSize)
	{
		if (gFidBox1.m_FidBoxItemSize != FidDataBufferSize)
		{
			_dbg_break_;
		}
		else
		{
			HRESULT hResult = OBJID_SetObjectData(
				gFidBox1.m_pFidBox,
				VolumeId.m_ObjectId,
				FileId.m_ObjectId,
				pFidData->m_Buffer,
				FidDataBufferSize,
				NULL,
				bResetRequest
				);
			
			if (ERROR_SUCCESS == hResult)
				status = STATUS_SUCCESS;
		}
	}
	
	ReleaseResource( &Globals.m_FidBoxLock );

	return status;
}

NTSTATUS
FidBox_SetDataByFO (
	PFILE_OBJECT pFileObject,
	BOOLEAN bAllowRequestFileId,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	MKAV_OBJ_ID VolumeId;
	MKAV_OBJ_ID FileId;

	status = FidBox_QueryInfo( pFileObject, &VolumeId, &FileId, bAllowRequestFileId, NULL );

	if (!NT_SUCCESS( status ))
		return STATUS_UNSUCCESSFUL;

	memcpy( pFidData->m_VolumeID, VolumeId.m_ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
	memcpy( pFidData->m_FileID, FileId.m_ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );

	status = FidBox_SetData( pFidData, FidDataBufferSize, TRUE, FALSE );

	return status;
}

NTSTATUS
GetInstanceFromFO (
	IN PFILE_OBJECT pFileObject,
	OUT PFLT_INSTANCE *p_pInstance
	)
{
	NTSTATUS status;
	PFLT_VOLUME pVolume = NULL;
	PFLT_INSTANCE pInstance = NULL;
	PMKAV_VOLUME_CONTEXT pVolumeContext = NULL;

	status = FltGetVolumeFromFileObject(Globals.m_Filter, pFileObject, &pVolume);
	if (!NT_SUCCESS( status ))
		return status;

	status = FltGetVolumeContext(Globals.m_Filter, pVolume, &pVolumeContext);
	if (NT_SUCCESS( status ))
	{
		status = FltObjectReference(pVolumeContext->m_pFltInstance);
		if (NT_SUCCESS( status ))
			*p_pInstance = pVolumeContext->m_pFltInstance;

		ReleaseFltContext( &pVolumeContext );
	}

	FltObjectDereference( pVolume );

	return status;
}

#define _dword_align(_address_)			((PVOID)(((ULONG_PTR)_address_+3) & (~3)))

NTSTATUS GetDataByNetworkFO(
	PFILE_OBJECT pFileObject,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	PULONG pRetSize)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG LenRequired;
	PFLT_INSTANCE pInstance = NULL;
	PFILE_FULL_EA_INFORMATION pFidboxRawData = NULL;
	CHAR Key = 0;

// fidbox over network

	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_INETSWIFT_DISABLED ))
		return STATUS_NOT_SUPPORTED;

	DoTraceEx(TRACE_LEVEL_INFORMATION, DC_FIDBOX, "fidbox request on a network file\n");

	if (!pfFltQueryEaFile)
	{
		DoTraceEx(TRACE_LEVEL_INFORMATION, DC_FIDBOX, "fidbox request no a network file not supported\n");
		return STATUS_NOT_SUPPORTED;
	}

	RtlZeroMemory(pFidData, sizeof(FIDBOX_REQUEST_DATA_EX));

	LenRequired = FidDataBufferSize;

// crypting key
	Key = (((CHAR)KeQueryInterruptTime())%20)+'A';

	status = GetInstanceFromFO(pFileObject, &pInstance);
	if (NT_SUCCESS( status ))
	{
#define EA_LIST_LEN					(sizeof(FILE_GET_EA_INFORMATION)+_INETSWIFT_PREFIX_FULL_BLEN+1)

		__declspec(align(4)) CHAR Buf[EA_LIST_LEN];
		PFILE_GET_EA_INFORMATION pEAList = (PFILE_GET_EA_INFORMATION)Buf;
		ULONG RetLen;
		PVOID _pQueryResult = NULL;
		PVOID pQueryResult = NULL;
		ULONG FixedSizePortionLen = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])+_INETSWIFT_PREFIX_FULL_BLEN+1+
			sizeof(INETSWIFT_DATA_EX);
		ULONG QueryLen = FixedSizePortionLen+FidDataBufferSize;

		pEAList->NextEntryOffset = 0;
		pEAList->EaNameLength = _INETSWIFT_PREFIX_FULL_BLEN;
		RtlStringCbCopyA(pEAList->EaName, pEAList->EaNameLength, _INETSWIFT_PREFIX_FULL);
		pEAList->EaName[_INETSWIFT_PREFIX_FULL_BLEN-1] = Key;	// last symbol is a crypting key

		status = STATUS_INSUFFICIENT_RESOURCES;
		_pQueryResult = ExAllocatePoolWithTag(PagedPool, QueryLen+3, 'grEA');
		if (_pQueryResult)
		{
			pQueryResult = _dword_align(_pQueryResult);
			status = pfFltQueryEaFile( pInstance, pFileObject, pQueryResult, QueryLen, TRUE, pEAList,
				EA_LIST_LEN, NULL, TRUE, &RetLen);

			if (NT_SUCCESS( status ))
			{
				pFidboxRawData = (PFILE_FULL_EA_INFORMATION)pQueryResult;

				status = STATUS_INVALID_MESSAGE;
				if (pFidboxRawData->EaNameLength >= _INETSWIFT_PREFIX_BLEN)
				{
					if (memcmp(pFidboxRawData->EaName, _INETSWIFT_PREFIX, _INETSWIFT_PREFIX_BLEN) == 0)
					{
						PINETSWIFT_DATA_EX piSwiftData = NULL;
						ULONG iSwiftDataOffset;

						iSwiftDataOffset = FIELD_OFFSET(FILE_FULL_EA_INFORMATION, EaName[0])+pFidboxRawData->EaNameLength+1;
						piSwiftData = (PINETSWIFT_DATA_EX)((PCHAR)pFidboxRawData+iSwiftDataOffset);

						LenRequired = pFidboxRawData->EaValueLength-sizeof(INETSWIFT_DATA_EX);
						if ((LONG)LenRequired < 0)
							LenRequired = 0;

						if (LenRequired != 0 && LenRequired <= FidDataBufferSize)	// correct buffer size
						{
							_xor_drv_func_ex(piSwiftData, pFidboxRawData->EaValueLength, Key, _xor_dir_back);

							if (_INETSWIFT_TAG == piSwiftData->m_Tag)
							{
								if (_INETSWIFT_VERSION_3 == piSwiftData->m_Version)
								{
									memcpy(pFidData, &piSwiftData->m_Data, sizeof(FIDBOX_REQUEST_DATA_EX)+LenRequired);
									status = STATUS_SUCCESS;
								}
							}
						}
						else
							DoTraceEx(TRACE_LEVEL_INFORMATION, DC_FIDBOX, "network iSwift buffer corrupt\n");
					}
				}

			}

			ExFreePool(_pQueryResult);
		}

		FltObjectDereference( pInstance );
	}
	else
		DoTraceEx(TRACE_LEVEL_WARNING, DC_FIDBOX, "failed to get filter instance form file object\n");

	if (!NT_SUCCESS( status ))
		RtlZeroMemory(pFidData, sizeof(FIDBOX_REQUEST_DATA_EX)+FidDataBufferSize);

	*pRetSize = LenRequired + sizeof(FIDBOX_REQUEST_DATA_EX);

	return status;
}

NTSTATUS
FidBox_GetDataByFO (
	PFILE_OBJECT pFileObject,
	BOOLEAN bAllowRequestFileId,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	PULONG pRetSize,
	BOOLEAN bResetID
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;

	MKAV_OBJ_ID VolumeId;
	MKAV_OBJ_ID FileId;

	BOOLEAN bNetwork = FALSE;

	ULONG LenRequired = 0;

	*pRetSize = 0;

	if (!gFidBox1.m_pFidBox)
		return STATUS_UNSUCCESSFUL;

	status = FidBox_QueryInfo( pFileObject, &VolumeId, &FileId, bAllowRequestFileId, &bNetwork );

	if (bNetwork)
	{
		ULONG dwRetSize = 0;

		status = GetDataByNetworkFO(
			pFileObject,
			pFidData,
			FidDataBufferSize,
			&dwRetSize);
		*pRetSize = dwRetSize;

		return status;
	}

	if (!NT_SUCCESS( status ))
		return STATUS_UNSUCCESSFUL;

	AcquireResourceExclusive( &Globals.m_FidBoxLock );
	
	status = STATUS_UNSUCCESSFUL;

	if(gFidBox1.m_pFidBox && gFidBox1.m_FidBoxItemSize)
	{
		HRESULT hResult;
		if (FidDataBufferSize != gFidBox1.m_FidBoxItemSize)
		{
			ULONG FidBoxItemSize;
			
			DoTraceEx( TRACE_LEVEL_WARNING, DC_FIDBOX, "change item size from %d to %d", gFidBox1.m_FidBoxItemSize, FidDataBufferSize );

			OBJID_Done( gFidBox1.m_pFidBox, OBJID_FLAG_DONTCLOSE_FILE );

			FidBoxItemSize = gFidBox1.m_FidBoxItemSize;
			hResult = OBJID_Init( &gFidBoxHeap, &gFidBox1.m_FidBoxFileData, &gFidBox1.m_FidBoxFileIndex, FidDataBufferSize, &gFidBox1.m_pFidBox, &FidBoxItemSize );
			gFidBox1.m_FidBoxItemSize = FidBoxItemSize;

			if (hResult)
			{
				_dbg_break_;
				Fid_FileClose( &gFidBox1.m_FidBoxFileData );
				Fid_FileClose( &gFidBox1.m_FidBoxFileIndex );
				
				gFidBox1.m_pFidBox = NULL;
			}
		}

		if (gFidBox1.m_pFidBox && gFidBox1.m_FidBoxItemSize == FidDataBufferSize)
		{
			hResult = OBJID_GetObjectData(
				gFidBox1.m_pFidBox, 
				VolumeId.m_ObjectId, 
				FileId.m_ObjectId,
				pFidData->m_Buffer,
				gFidBox1.m_FidBoxItemSize,
				NULL
				);

			*pRetSize = sizeof(FIDBOX_REQUEST_DATA_EX) + FidDataBufferSize;

			if (bResetID)
			{
				memset( pFidData->m_VolumeID, 0, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
				memset( pFidData->m_FileID, 0, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
			}
			else
			{
				RtlCopyMemory( pFidData->m_VolumeID, VolumeId.m_ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
				RtlCopyMemory( pFidData->m_FileID, FileId.m_ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
			}

			pFidData->m_DataFormatId = 0;
			if (ERROR_SUCCESS != hResult)
				memset(pFidData->m_Buffer, 0, FidDataBufferSize );

			status = STATUS_SUCCESS;
		}
	}

	ReleaseResource( &Globals.m_FidBoxLock );

	return status;
}

NTSTATUS
FidBox_GetData (
	PFIDBOX_REQUEST_GET pFidGet,
	PFIDBOX_REQUEST_DATA_EX pFidData,
	ULONG FidDataBufferSize,
	PULONG pRetSize
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;

	ULONG inFile;
	HANDLE hFile = NULL;
	PFILE_OBJECT pFileObject;

	*pRetSize = 0;

	inFile = pFidGet->m_hFile;

	if (!inFile || (inFile == (ULONG) -1))
		return STATUS_NOT_SUPPORTED;

	*(ULONG*)&hFile = inFile;

	status = ObReferenceObjectByHandle(hFile, SYNCHRONIZE, *IoFileObjectType, KernelMode, (PVOID*) &pFileObject, NULL);
	if (!NT_SUCCESS( status ))
		return STATUS_NOT_FOUND;

	status = FidBox_GetDataByFO( pFileObject, FALSE, pFidData, FidDataBufferSize, pRetSize, TRUE );
	*(ULONG*) pFidData->m_FileID = inFile;

	ObDereferenceObject( pFileObject );

	return status;
}

VOID
FidBox_ResetValid (
	PMKAV_OBJ_ID pVolumeId,
	PMKAV_OBJ_ID pFileId
	)
{
	PFIDBOX_REQUEST_DATA_EX pFidData = NULL;
	ULONG itemsize = gFidBox1.m_FidBoxItemSize;

	if (!gFidBox1.m_pFidBox || !pFileId)
		return;

	if (!itemsize)
		return;

	pFidData = ExAllocatePoolWithTag( PagedPool, sizeof(FIDBOX_REQUEST_DATA_EX) + itemsize, 'BBoS');
	if (!pFidData)
		return;

	pFidData->m_DataFormatId = 0;

	memset( pFidData->m_Buffer, 0, itemsize );

	memcpy( pFidData->m_VolumeID, pVolumeId->m_ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
	memcpy( pFidData->m_FileID, pFileId->m_ObjectId, RTL_FIELD_SIZE(MKAV_OBJ_ID, m_ObjectId) );
	
	FidBox_SetData( pFidData, itemsize, TRUE, TRUE );

	ExFreePool( pFidData );
}

// serv funcs

__int64
GetTimeFast (
	)
{
	__int64 ct = (__int64) KeQueryInterruptTime();

	return ct;
}

BOOLEAN
FidBox_IsSelf (
	PFILE_OBJECT pFO
	)
{
	BOOLEAN bRet = FALSE;

	if ((gFidBox1.m_FidBoxFileData.FileAccess == pFO) || (gFidBox1.m_FidBoxFileIndex.FileAccess == pFO))
	{
		_dbg_break_;
		bRet = TRUE;
	}

	if ((gFidBox2.m_FidBoxFileData.FileAccess == pFO) || (gFidBox2.m_FidBoxFileIndex.FileAccess == pFO))
	{
		_dbg_break_;
		bRet = TRUE;
	}


	return bRet;
}

void
FidBox2_Update(
	PMKAV_OBJ_ID pVolumeId,
	PMKAV_OBJ_ID pFileId
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	if (!gFidBox2.m_pFidBox || !pFileId || !pVolumeId)
		return;

	AcquireResourceExclusive( &Globals.m_FidBoxLock );

	if (gFidBox2.m_pFidBox)
	{
		ULONG Counter = 0;
		HRESULT hResult = OBJID_GetObjectData(
			gFidBox2.m_pFidBox, 
			pVolumeId->m_ObjectId, 
			pFileId->m_ObjectId,
			&Counter,
			gFidBox2.m_FidBoxItemSize,
			NULL
			);

		if (ERROR_SUCCESS == hResult)
		{
			Counter++;
			hResult = OBJID_SetObjectData(
				gFidBox2.m_pFidBox, 
				pVolumeId->m_ObjectId, 
				pFileId->m_ObjectId,
				&Counter,
				gFidBox2.m_FidBoxItemSize,
				NULL,
				FALSE
				);
			
			if (ERROR_SUCCESS != hResult)
			{
				_dbg_break_;
			}
		}
	}

	ReleaseResource( &Globals.m_FidBoxLock );
}

NTSTATUS
FidBox2_GetByFO (
	PFILE_OBJECT pFileObject,
	PULONG pData
	)
{
	NTSTATUS status = STATUS_NOT_FOUND;

	MKAV_OBJ_ID VolumeId;
	MKAV_OBJ_ID FileId;
	BOOLEAN bNetwork = FALSE;
	HRESULT hResult;

	if (!gFidBox2.m_pFidBox)
		return STATUS_UNSUCCESSFUL;

	status = FidBox_QueryInfo( pFileObject, &VolumeId, &FileId, TRUE, &bNetwork );

	if (!NT_SUCCESS( status ) || bNetwork)
		return STATUS_UNSUCCESSFUL;

	AcquireResourceExclusive( &Globals.m_FidBoxLock );

	if (gFidBox2.m_pFidBox)
	{
		hResult = OBJID_GetObjectData(
			gFidBox2.m_pFidBox, 
			VolumeId.m_ObjectId, 
			FileId.m_ObjectId,
			pData,
			gFidBox2.m_FidBoxItemSize,
			NULL
			);

		if (ERROR_SUCCESS != hResult || !*pData)
		{
			LARGE_INTEGER time;
			KeQuerySystemTime( &time );

			*pData = time.LowPart;
			if(!time.LowPart)
			{
				_dbg_break_;
			}

			hResult = OBJID_SetObjectData(
				gFidBox2.m_pFidBox,
				VolumeId.m_ObjectId,
				FileId.m_ObjectId,
				pData,
				gFidBox2.m_FidBoxItemSize,
				NULL,
				FALSE
				);
			
			if (ERROR_SUCCESS != hResult)
				status = STATUS_NOT_FOUND;
		}
	}

	ReleaseResource( &Globals.m_FidBoxLock );

	return status;
}
