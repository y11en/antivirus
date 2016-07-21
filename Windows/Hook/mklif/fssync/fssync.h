#include "../../hook/fssync.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define FSDrv_EnumProcesses				FSSync_PEn
#define FSDrv_EnumProcessModules		FSSync_PEnM
#define FSDrv_ProcessReadMemory			FSSync_PRM
#define FSDrv_IsImageActive				FSSync_ISIA
#define DRV_GetStatCounter				DRV_GSCou

#define FSDrv_UniInit					FSSync_UNIN
#define FSDrv_UniDone					FSSync_UNDO
#define FSDrv_UniRegInv					FSSync_UNRI
#define FSDrv_UniUnRegInv				FSSync_UNUR

typedef void (FS_PROC *_tpfEnumProcessesCallback) (
	PVOID /*dwContext*/,
	ULONG/* pid*/,
	ULONG/* parent_pid*/,
	LARGE_INTEGER /*start_time*/,
	PWCHAR /*pwchImagePath*/,
	PWCHAR /*pwchCmdLine*/,
	PWCHAR /*pwchCurrDir*/
	);

typedef BOOL (FS_PROC *_tpfFSDrv_EnumProcesses) (
	PVOID /*dwContext*/,
	_tpfEnumProcessesCallback/* pfCallback*/
	);

//////////////////////////////////////////////////////////////////////////

typedef void (FS_PROC *_tpfEnumModulesCallback) (
	PVOID /*dwContext*/,
	PWCHAR /*pwchImagePath*/,
	LARGE_INTEGER /*ImageBase*/,
	ULONG /*ImageSize*/,
	LARGE_INTEGER /*EntryPoint*/
	);

typedef BOOL (FS_PROC *_tpfFSDrv_EnumModules) (
	PVOID /*dwContext*/,
	ULONG /*Processid*/,
	_tpfEnumModulesCallback/* pfCallback*/
	);

typedef BOOL (FS_PROC *_tpfFSDrv_ProcessReadMemory) (
	ULONG /*Processid*/,
	LARGE_INTEGER /*Offset*/,
	PVOID /*pBuffer*/,
	ULONG /*BufferSize*/,
	PULONG /*pNumberOfBytesRead*/
	);


typedef BOOL (FS_PROC *_tpfFSDrv_AddInvThreadByHandle) (
	HANDLE /*hThread*/
	);

typedef BOOL (FS_PROC *_tpfFSDrv_IsImageActive) (
	HANDLE /*hFile*/
	);

//////////////////////////////////////////////////////////////////////////


BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_EnumProcesses (
	PVOID pContext,
	_tpfEnumProcessesCallback pcbEnum
	);

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_EnumProcessModules (
	PVOID pContext,
	ULONG ProcessId,
	_tpfEnumModulesCallback pcbEnum
	);

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_ProcessReadMemory (
	ULONG ProcessId,
	LARGE_INTEGER Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pNumberOfBytesRead
	);

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_IsImageActive (
	HANDLE hFile
	);

HRESULT
FS_PROC_EXPORT
FS_PROC
DRV_GetStatCounter (
	PVOID pContext,
	PULONGLONG pTimeCurrent,
	PULONGLONG pTotalWaitTime
	);

// ----------------------------------------------------------------------------
//+ UniDrv functions

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniInit (
	__out PVOID* ppClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniDone (
	__in PVOID* ppClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniRegInv (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
FSDrv_UniUnRegInv (
	__in PVOID pClientContext
	);

//- UniDrv functions
// ----------------------------------------------------------------------------

#ifdef __cplusplus
	}
#endif
