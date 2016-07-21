#ifndef __SYS_IO
#define __SYS_IO

/*!
*		
*		
*		(C) 2002-3 Kaspersky Lab 
*		
*		\file	SysIO.h
*		\brief	перехватчики системнх функций
*		
*		\author Andrew Sobko, Sergey Belov
*		\date	12.09.2003 12:51:00
*		
*		
*		
*		
*/		



#include "pch.h"
#include "../kldefs.h"
#include "../hook/avpgcom.h"

extern ULONG gHashFuncVersion;

VOID
GlobalSystemDataInit (
	);

VOID
GlobalSystemDataDone (
	);

VOID
SleepImp (
	__int64 ReqInterval
	);
//+ ------------------------------------------------------------------------------------------
//! \fn				: SetSystemNotifiers
//! \brief			: установить системные перехватчики
//! \return			: 
NTSTATUS
SetSystemNotifiers ();

//! \fn				: RemoveSystemNotifiers
//! \brief			: освободить ресурсы / снять системные перехватчики (не реализовано) 
//! \return			: 
VOID
RemoveSystemNotifiers ();

NTSTATUS
DoSysRequest (
	IN PEXTERNAL_DRV_REQUEST pInRequest,
	OUT PVOID pOutBuffer, 
	IN ULONG OutBufferSize,
	OUT PULONG pRetSize
	);

PWCHAR
QueryProcessPath (
	HANDLE ProcessId,
	PULONG pProcessPathLenB
	);

VOID
ReleaseProcessPath (
	PWCHAR pwchProcessPath
	);

NTSTATUS
Sys_QueryProcessHash (
	HANDLE ProcessId,
	PVOID pBuffer,
	ULONG BufferSize
	);

NTSTATUS
FillPidList (
	__in PLIST_ENTRY pPidHead 
	);

NTSTATUS
RemovePidList (
	__in PLIST_ENTRY pPidHead 
	);

HANDLE GetParent (
	HANDLE ProcessId
	);

NTSTATUS
Sys_GetProcessesInfo (
	PVOID* ppProcesInfo,
	ULONG* pProcesInfoLen
	);

NTSTATUS
Sys_GetModulesInfo (
	HANDLE ProcessId,
	PVOID* ppModulesInfo,
	ULONG* pModulesInfoLen
	);

NTSTATUS
Sys_ReadProcessMemory (
	HANDLE ProcessId,
	LARGE_INTEGER Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytesRead
	);

VOID
Sys_RegisterExecutedModule (
	PUNICODE_STRING pVolume,
	PUNICODE_STRING pFileName
	);

NTSTATUS
Sys_IsImageActive (
	HANDLE hFile
	);

NTSTATUS
Sys_GetActiveImages (
	PVOID* ppActiveImages,
	ULONG* pActiveImagesLen
	);

VOID
Sys_FixStoredFileName (
	__in PFLT_FILE_NAME_INFORMATION pNameInfoNew,
	__in PFLT_FILE_NAME_INFORMATION pNameInfoOld
	);

NTSTATUS
Sys_QueryEnlistedObjects (
	PMKAV_TRANSACTION_CONTEXT pTransactionContext,
	PVOID* ppEnlistedObjects,
	ULONG* pEnlistedObjectsLen
	);

NTSTATUS
Sys_CalcHash (
	__in PFILE_OBJECT pFileObject,
	__in PVOID pHashBuffer,
	__in ULONG HashBufferSize
	);

NTSTATUS
Sys_CalcFileHash(
	__in PWCHAR FileName,
	__in USHORT	FileNameLen,
	__inout PVOID  pBuffer,
	__inout PULONG pBufferSize
);

NTSTATUS
Sys_CalcFileHashByName (
	__in PUNICODE_STRING pusNativePath,
	__inout PVOID  pBuffer,
	__inout PULONG pBufferSize
	);

NTSTATUS
Sys_BuildNativePath (
		PWCHAR pwchPath,
		USHORT PathLen,
		PUNICODE_STRING pusNativePath
);

NTSTATUS
Sys_GetFB2ByFO (
	PFILE_OBJECT pFileObject,
	PFIDBOX2_REQUEST_DATA pFB2
	);

#endif //__SYS_IO
