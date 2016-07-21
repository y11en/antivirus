/*!
*		
*		
*		(C) 2002 Kaspersky Lab 
*		
*		\file	FSSync.h
*		\brief	взаимодействие с драйвером перехватчиком
*			враппер над FSDrvLib
*		
*		\author Andrew Sobko
*		\date	04.01.2003 12:08:54
*		
*		Example:	
*		if (FSDrv_Init())							// проверка регистрации в драйвере
*		{
*			if (FSDrv_RegisterInvisibleThread())	// регитсрация текущего потока как невидимого для системы фильтрации
*				FSDrv_UnRegisterInvisibleThread();	// разрегистрация
*
*			FSDrv_Done();							// отключение от перехватчика
*		}
*		
*		
*		использование защиты (реализованно тольок для NT!):
*		1. защита от TerminateProcess - используются парные вызовы:
*			FSDrv_ProtectCurrentProcess/FSDrv_UnProtectCurrentProcess
*		2. защита от любого вида попытки остановить процесс (lsas сервису разрешен доступ к адресному пространнству 
*			защищаемого процесса) - используются парные вызовы:
*			FSDrv_ProtectCurrentProcessEx/FSDrv_UnProtectCurrentProcessEx
*		
*		
*/		


#ifndef __FSSYNC_H
#define __FSSYNC_H

#include "windows.h"
#include "winioctl.h"
#include "avpgcom.h"

#define FS_PROC __cdecl
#define FS_PROC_EXPORT __declspec(dllexport)

#ifdef __cplusplus
	extern "C" {
#endif

//+ ----------------------------------------------------------------------------------------
// подстановка имён
#define FSDrv_Init							FSSync_Init
#define FSDrv_Done							FSSync_Done
#define FSDrv_RegisterInvisibleThread		FSSync_SetCheck
#define FSDrv_UnRegisterInvisibleThread		FSSync_Remove
#define FSDrv_ProtectCurrentProcess			FSSync_ScreeUp
#define FSDrv_UnProtectCurrentProcess		FSSync_ScreeDown
#define FSDrv_ProtectCurrentProcessEx		FSSync_ScreeUpEx
#define FSDrv_UnProtectCurrentProcessEx		FSSync_ScreeDownEx
#define FSDrv_ProactiveStart				FSSync_ScreeNotify
#define FSDrv_ProactiveStop					FSSync_ScreeActive
#define FSDrv_ProactiveChangeState			FSSync_ScreeState
#define FSDrv_ProactiveFChangeState			FSSync_ScreeFState

#define FSDrv_AddInvThreadByHandle			FSSync_RTH
#define FSDrv_DelInvThreadByHandle			FSSync_RTHU

#define DRV_Register						FSSync_DR
#define DRV_UnRegister						FSSync_DUR
#define DRV_BreakConnection					FSSync_BRC
#define DRV_QueryFlags						FSSync_DQF
#define DRV_ChangeActiveStatus				FSSync_DCS
#define DRV_ResetCache						FSSync_DRC
#define DRV_RegisterInvisibleThread			FSSync_DT
#define DRV_UnRegisterInvisibleThread		FSSync_DTT
#define DRV_PrepareMessageQueue				FSSync_PMQ
#define DRV_SetVerdict						FSSync_SV
#define DRV_PrepareMessage					FSSync_PM
#define DRV_GetMessage						FSSync_GM
#define DRV_PendingMessage					FSSync_PEM
#define DRV_AddFilter						FSSync_ADF
#define DRV_AddFilterEx						FSSync_ADFE
#define Drv_GetEventHdr						FSSync_GEH
#define DRV_GetEventParam					FSSync_GEP
#define DRV_GetVolumeName					FSSync_GVN
#define DRV_ImpersonateThread				FSSync_IPM
#define DRV_Yeild							FSSync_YLD
#define DRV_GetEventInfo					FSSYNC_GEI
#define DRV_GetFilterFirst					FSSYNC_GFF
#define DRV_FilterNext						FSSYNC_GFN
#define DRV_GetFilterInfo					FSSYNC_GFI
#define DRV_GetFilterParam					FSSYNC_GHP
#define DRV_GetHashVersion					FSSYNC_GHV
#define DRV_FiltersClear					FSSYNC_FC
#define DRV_FilterDelete					FSSYNC_FD
#define DRV_GetFileHash						FSSYNC_GFH
#define DRV_SaveFilters						FSSYNC_SAF
#define DRV_GetFilterNextParam				FSSYNC_GFNP
#define DRV_FilterEvent						FSSYNC_FE
#define DRV_VerdictFlags2Verdict			FSSYNC_EF2V
#define DRV_ChangeFilterParam				FSSYNC_CHFP
#define DRV_GetEventMark					FSSYNC_GEVM
#define DRV_GetFileFB2						FSSYNC_GFB2
#define DRV_ReloadSettings					FSSYNC_RELS
#define DRV_FidBox_GetByHandle				FSSYNC_FB_GBH
#define DRV_FidBox_SetByHandle				FSSYNC_FB_SBH
#define DRV_Disk_Enum						FSSync_D_E
#define DRV_Disk_GetGeometry				FSSync_D_GG
#define DRV_Disk_GetPartitionalInformation	FSSync_D_GPI
#define DRV_Disk_GetType					FSSync_D_GT
#define DRV_Disk_Read						FSSync_D_RE
#define DRV_Disk_Write						FSSync_D_WR
#define DRV_Disk_GetDiskId					FSSync_D_GDID
#define DRV_RegisterInvisibleProcEx			FSSync_Ripe
#define DRV_IsInvisibleProcess				FSSync_INP
#define DRV_IsInvisibleThread				FSSync_INT
#define DRV_DisconnectAllClients			FSSync_DACL
#define DRV_SpecFileRequest					FSSync_SFR
#define DRV_SpecFileGet						FSSync_SFG

//+ ----------------------------------------------------------------------------------------
/* все нижеописанные функции должны вызываться парами если первая функция завершилась успешно: 
	FSDrv_Init						- FSDrv_Done, 
	FSDrv_RegisterInvisibleThread	- FSDrv_UnRegisterInvisibleThread

	разрешаются повторные вызовы:
	FSDrv_Init, FSDrv_Init...FSDrv_Done, FSDrv_Done
	FSDrv_RegisterInvisibleThread, FSDrv_RegisterInvisibleThread... FSDrv_UnRegisterInvisibleThread, FSDrv_UnRegisterInvisibleThread
*/
		

//! \fn				: FSDrv_Init
//! \brief			:	инициализация подключения к драйверу. 
//! \return			: не 0 если успешно
BOOL FS_PROC_EXPORT FS_PROC FSDrv_Init();


//! \fn				: FSDrv_Done
//! \brief			:	разрегистрация в драйвере
//! \return			: void FS_PROC 
void FS_PROC_EXPORT FS_PROC FSDrv_Done();


//! \fn				: FSDrv_RegisterInvisibleThread
//! \brief			:	регистрация текущего потока как невидимого в системе фильтрации событий в драйвере-перехватчике
//! \return			: не 0 если успешно
BOOL FS_PROC_EXPORT FS_PROC FSDrv_RegisterInvisibleThread();

//! \fn				: FSDrv_UnRegisterInvisibleThread
//! \brief			:	разрегистрация потока
//! \return			: void FS_PROC 
void FS_PROC_EXPORT FS_PROC FSDrv_UnRegisterInvisibleThread();

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_AddInvThreadByHandle (
	HANDLE hThread
	);

BOOL
FS_PROC_EXPORT
FS_PROC
FSDrv_DelInvThreadByHandle (
	HANDLE hThread
	);

//+ ----------------------------------------------------------------------------------------

//! \fn				: FSDrv_ProtectCurrentProcess
//! \brief			:	установка защиты для текущего процесса (terminate, open, debug...)
//! \return			: TRUE если успешно
BOOL FS_PROC_EXPORT FS_PROC FSDrv_ProtectCurrentProcess();


//! \fn				: FSDrv_UnProtectCurrentProcess
//! \brief			:	снятие защиты процесса
//! \return			: BOOL FS_PROC 
BOOL FS_PROC_EXPORT FS_PROC FSDrv_UnProtectCurrentProcess();

//+ ----------------------------------------------------------------------------------------

//! \fn				: FSDrv_ProtectCurrentProcessEx
//! \brief			:	установка защиты для текущего процесса (all)
//! \return			: TRUE если успешно
BOOL FS_PROC_EXPORT FS_PROC FSDrv_ProtectCurrentProcessEx();


//! \fn				: FSDrv_UnProtectCurrentProcess
//! \brief			:	снятие защиты процесса
//! \return			: BOOL FS_PROC 
BOOL FS_PROC_EXPORT FS_PROC FSDrv_UnProtectCurrentProcessEx();


//+ ----------------------------------------------------------------------------------------
// Additional functions 


//! \fn				: FSSync_GetDriveType
//! \brief			:	execution on holded thread on 9x Windows - получения типа тома
//! \return			: не 0 если успешно 
//! \param          : pDrvDescr - описатель остановленного события
//! \param          : BYTE drv - имя тома
//! \param          : pVal - указатель, куда будет возвращен результат
DWORD FS_PROC_EXPORT FS_PROC FSSync_GetDriveType(DWORD pDrvDescr, BYTE drv, DWORD* pVal);



//! \fn				: FS_PROC FSSync_GetUserFromSidW
//! \brief			:	
//! \return			: if TRUE - success
//! \param          : pUserSid - идентификатор пользователя
//! \param          : pszDomain - домен
//! \param          : pdwDomainLen - длина имени домена (вход - длина буфера. выход - длина имени)
//! \param          : pszUser - имя пользоваетеля
//! \param          : pdwUserLen - длина имени пользователя (вход - длина буфера. выход - длина имени)
BOOL FS_PROC_EXPORT FS_PROC FSSync_GetUserFromSidW(SID* pUserSid, WCHAR* pszDomain, DWORD* pdwDomainLen, WCHAR* pszUser, DWORD* pdwUserLen);

#define _fSelfProtection_None		0x00000000
#define _fSelfProtection_Terminate	0x00000001
#define _fSelfProtection_Modify		0x00000002
#define _fSelfProtection_Delete		0x00000004
#define _fSelfProtection_QueryInfo	0x00000010
#define _fSelfProtection_SetInfo	0x00000020
#define _fSelfProtection_AccessFor	0x10000000

typedef enum _eSelfProtectionResourceType
{
	eSPA_Process	= 0,
	eSPA_File		= 1,
	eSPA_Registry	= 2,
	eSPA_Security	= 3,
}SelfProtectionResourceType;

typedef struct _sSelfProtectionEvent
{
	ULONG						m_ProcessId;
	ULONG						m_Flags;
	SelfProtectionResourceType	m_ResourceType;
	PWCHAR						m_pwchResource;
	ULONG						m_SP_ProcessId;
}SelfProtectionEvent, *PSelfProtectionEvent;

typedef BOOL (FS_PROC *_tpfProactivNotify)(PVOID /*pContext*/, ULONG Reserved, PSelfProtectionEvent);


//////////////////////////////////////////////////////////////////////////
typedef enum _eStartMode
{
	_ProtectionStartMode_DontChange = 0,
	_ProtectionStartMode_Activate = 1,
	_ProtectionStartMode_Pause = 2,
}eStartMode;

//! \fn				: FSDrv_ProactiveStart
//! \brief			:	protect with notify
//! \return			: NULL if failed, else pProContext context for stop
PVOID FS_PROC_EXPORT FS_PROC FSDrv_ProactiveStart(PVOID pUserContext, _tpfProactivNotify pfnCallback, eStartMode StartMode);

//! \fn				: FSDrv_ProactiveStop
//! \brief			:	stop
//! \return			: void
void FS_PROC_EXPORT FS_PROC FSDrv_ProactiveStop(PVOID pProContext);

void FS_PROC_EXPORT FS_PROC FSDrv_ProactiveChangeState(PVOID pProContext, BOOL bNewState);
void FS_PROC_EXPORT FS_PROC FSDrv_ProactiveFChangeState(HMODULE hMod, BOOL bNewState);

// ----------------------------------------------------------------------------
//+ FSSync advanced functions

typedef void* ( __cdecl *DRV_pfMemAlloc )(PVOID pOpaquePtr, size_t, ULONG );
typedef void ( __cdecl *DRV_pfMemFree )( PVOID pOpaquePtr, void** );

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Register (
	__out PVOID* ppClientContext,
	__in ULONG AppId,
	__in ULONG Priority,
	__in ULONG ClientFlags,
	__in ULONG CacheSize,
	__in ULONG BlueScreenTimeout,
	__in DRV_pfMemAlloc pfAlloc,
	__in DRV_pfMemFree pfFree,
	__in_opt PVOID pOpaquePtr
	);

VOID
FS_PROC_EXPORT FS_PROC
DRV_UnRegister (
	__in PVOID* ppClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_BreakConnection (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_QueryFlags (
	__in PVOID pClientContext,
	__out PULONG pDriverFlags
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ChangeActiveStatus (
	__in PVOID pClientContext,
	__in BOOL bActive
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ResetCache (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_RegisterInvisibleThread (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_UnRegisterInvisibleThread (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_RegisterInvisibleProcEx (
	__in PVOID pClientContext,
	__in ULONG Processid
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PrepareMessageQueue (
	__in PVOID pClientContext,
	__in ULONG ThreadMaxCount
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SetVerdict (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in BOOL bAllow,
	__in BOOL bCacheable,
	__in ULONG ExpTime
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PrepareMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetMessage (
	__in PVOID pClientContext,
	__out PVOID* ppMessage,
	__out PULONG pMessageSize,
	__in PVOID* pEventHdr
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_PendingMessage (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG dwWaitTime
	);


HRESULT
FS_PROC_EXPORT FS_PROC
DRV_AddFilter (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in_opt PFILTER_PARAM pParam1,
	__in_opt PFILTER_PARAM pParam2,
	__in_opt PFILTER_PARAM pParam3,
	__in_opt PFILTER_PARAM pParam4,
	__in_opt PFILTER_PARAM pParam5
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_AddFilterEx (
	__out PULONG pFilterId,
	__in PVOID pClientContext,
	__in DWORD dwTimeout,
	__in DWORD dwFlags,
	__in DWORD HookID,
	__in DWORD FunctionMj,
	__in DWORD FunctionMi,
	__in LONGLONG ExpireTime,
	__in PROCESSING_TYPE ProcessingType,
	__in ULONG AddMethod,
	__in ULONG PosFilterId,
	__in PFILTER_PARAM* pParamArray,
	__in ULONG ParamsCount
	);

PVOID
FS_PROC_EXPORT FS_PROC
Drv_GetEventHdr (
	__in PVOID pMessage
	);

PSINGLE_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetEventParam (
	__in PVOID pMessage,
	__in ULONG MessageSize,
	__in ULONG ParamId,
	__in_opt ULONG SizeCheck
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetVolumeName (
	__in PVOID pClientContext,
	__in PWCHAR pwchNativeVolumeName,
	__inout PWCHAR pwchVolumeName,
	__in ULONG VolumeNameLen
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ImpersonateThread (
	__in PVOID pClientContext,
	__in PVOID pMessage
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Yeild (
	__in PVOID pClientContext,
	__in PVOID pMessage,
	__in ULONG Timeout
	);

VOID
FS_PROC_EXPORT FS_PROC
DRV_GetEventInfo (
	__in PVOID pMessage,
	__out PULONG pHookId,
	__out PULONG pMj,
	__out PULONG pMi,
	__out PULONG pFilterId,
	__out PULONG pEventFlags,
	__out PULONG pProcessId
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFilterFirst (
	__in PVOID pClientContext,
	__out PVOID* ppFilter
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterNext (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__out PVOID* ppFilter
	);

VOID
FS_PROC_EXPORT FS_PROC
DRV_GetFilterInfo (
	__in PVOID pFilter,
	__out PULONG pFilterId,
	__out DWORD* pdwTimeout,
	__out DWORD* pdwFlags,
	__out DWORD* pHookID,
	__out DWORD* pFunctionMj,
	__out DWORD* pFunctionMi,
	__out LONGLONG* pExpireTime,
	__out PROCESSING_TYPE* pProcessingType
	);

PFILTER_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetFilterParam (
	__in PVOID pFilter,
	__in ULONG ParamId
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetHashVersion (
	__in PVOID pClientContext,
	__out PULONG pHashVersion
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FiltersClear (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterDelete (
	__in PVOID pClientContext,
	__in ULONG FilterId
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFileHash (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PVOID* ppHash,
	__out PULONG pHashSize
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SaveFilters (
	__in PVOID pClientContext
	);

PFILTER_PARAM
FS_PROC_EXPORT FS_PROC
DRV_GetFilterNextParam (
	__in PVOID pFilter,
	__in ULONG Index
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FilterEvent (
	__in PVOID pClientContext,
	__in PFILTER_EVENT_PARAM pEventParam,
	__in BOOL bTry,
	__out PULONG pVerdictFlags
	);

VERDICT
FS_PROC_EXPORT FS_PROC
DRV_VerdictFlags2Verdict (
	ULONG VerdictFlags
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ChangeFilterParam (
	__in PVOID pClientContext,
	__in ULONG FilterId,
	__in PFILTER_PARAM pParam
	);

ULONG
FS_PROC_EXPORT FS_PROC
DRV_GetEventMark (
	__in PVOID pMessage
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_GetFileFB2 (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__out PFIDBOX2_REQUEST_DATA pFidBox2
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_ReloadSettings (
	__in ULONG SettingsMask
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FidBox_GetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__in PVOID pBuffer,
	__in ULONG BufferSize,
	__out PULONG pBytesRet
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_FidBox_SetByHandle (
	__in PVOID pClientContext,
	__in HANDLE hFile,
	__in PVOID pBuffer,
	__in ULONG BufferSize
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Enum (
	__in PVOID pClientContext,
	__out PWCHAR* ppwchVolumes,
	__out PULONG pSize
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetGeometry (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PDISK_GEOMETRY pGeometry
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetPartitionalInformation (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PPARTITION_INFORMATION pPartitionalInformation
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetType
 (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out LPDWORD pType
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Read (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PVOID pBuffer,
	__in ULONG size,
	__in PLARGE_INTEGER pOffset,
	__out LPDWORD pBytesRet
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_Write (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PVOID pBuffer,
	__in ULONG size,
	__in PLARGE_INTEGER pOffset,
	__out LPDWORD pBytesRet
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_Disk_GetDiskId (
	__in PVOID pClientContext,
	__in PWCHAR pwchVolumeName,
	__out PDISK_ID_INFO pDiskId
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_IsInvisibleProcess (
	__in PVOID pClientContext,
	__in ULONG ProcessId
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_IsInvisibleThread (
	__in PVOID pClientContext,
	__in ULONG ThreadId
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_DisconnectAllClients (
	__in PVOID pClientContext
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SpecFileRequest (
	__in PVOID pClientContext,
	__in PWCHAR pwchFN1,
	__in PWCHAR pwchFN2
	);

HRESULT
FS_PROC_EXPORT FS_PROC
DRV_SpecFileGet (
	__in PVOID pClientContext,
	__in PWCHAR pwchFN,
	__out PULONG pErrCode
	);

//- FSSync advanced functions
// ----------------------------------------------------------------------------

#ifdef __cplusplus
	}
#endif

#endif //__FSSYNC_H
