#ifndef _wavpg_h
#define _wavpg_h

#include <windows.h>
#include "avpgimpl.h"
#include <prague/iface/i_threadpool.h>
#include <prague/plugin/p_thpoolimp.h>
#include <prague/pr_oid.h>

#include "../../windows/hook/hook/avpgcom.h"
#include "../../windows/hook/hook/fssync.h"
#include "../../windows/hook/hook/hookspec.h"
#include "../../windows/hook/hook/Funcs.h"

#include "wevqueue.h"
#include "wusers.h"
#include "trustpr.h"
#include "prclist.h"

extern tPROPID propid_EventDefinition;

//+ common functions
void* __cdecl
MemAlloc (
	PVOID pOpaquePtr,
	size_t size,
	ULONG tag
	);

void __cdecl
MemFree (
	PVOID pOpaquePtr,
	void** pptr
	);

int __cdecl
Hash_Compare (
	void* pContext,
	void* pData1,
	void* pData2,
	int DataLen
	);

//+ messages and params
void
FillParamNOP (
	PFILTER_PARAM Param,
	ULONG ParamId,
	DWORD ParamFlags
	);

void
AnswerMessage (
	hOBJECT pPragueContext,
	PVOID pDrvContext,
	PVOID *ppMessage,
	BOOL bAllow,
	BOOL bCacheable,
	ULONG ExpTime
	);

//+ Callbacks
class cContext;

typedef void
(__cdecl *_pfProceedEvent) (
	cContext* pContext,
	PVOID pMessage
	);

class cContext
{
public:
	cContext( hOBJECT pPragueContext, tDWORD ThreadMaxCount );
	~cContext();
	
	bool Init();

	HRESULT VolumeCache_Get(	PWCHAR pwchVolume, ULONG VolumeNameLen, PWCHAR pwchDosName);
	void VolumeCache_Save( PWCHAR pwchVolume, ULONG VolumeNameLen, PWCHAR pwchDosName );
	void VolumeCache_Clean( );

public:
	
	hOBJECT		m_pPragueContext;
	tDWORD		m_ThreadMaxCount;
	PVOID		m_pDrvContext;
	
	PVOID		m_AvpgImp;
	_pfProceedEvent m_pfProceed;
	bool		m_bMessageQueueInited;

	cThreadPool* m_pThPoolWork;
	cThreadPool* m_pThPoolDelayed;

	cEventQueue m_EvQueue;
	cWUsers		m_Users;
	cTrustedProcessesCache m_TrustedProcesses;
	cProcessingList m_ProcessingList;

	PHashTree	m_pVolumeNamesCache;
	CRITICAL_SECTION m_SyncVolume;
};

tERROR
pr_call
ThreadCallback (
	tThreadCallType CallType,
	tPTR pCommonThreadContext,
	tPTR* ppThreadContext
	);

tERROR
pr_call
WorkCallback (
	tThreadCallType CallType,
	tPTR pThreadContext,
	tPTR* ppTaskContext,
	tPTR pTaskData,
	tDWORD TaskDataLen
	);

cString*
GenerateContext (
	cContext* pContext,
	hOBJECT pPragueContext,
	PVOID pDrvContext,
	PVOID pMessage,
	ULONG ProcessId
	);

void
AttachUserInfo (
	cWUsers* pUsers,
	cString* hCtx,
	PVOID pMessage,
	cUserInfo* pUser
	);

bool
IsModifiedObject (
	PVOID pMessage
	);

void
SendVolumeObject (
	cContext* pContext,
	PVOID* ppMessage
	);

void
SendObject (
	cContext* pContext,
	tAVPG2_WORKING_MODE WorkingMode,
	cString* hCtx,
	PVOID* ppMessage,
	ULONG ProcessId,
	bool bExecute
	);

void
AddToDelayed (
	cContext* pContext,
	cString* hCtx,
	PVOID pMessage
	);

//- Callbacks
//- common functions

class cWinAvpgImpl: public cAvpgImpl
{
public:
	cWinAvpgImpl( hOBJECT pPragueContext, tAVPG2_WORKING_MODE WorkingMode, tDWORD ThreadMaxCount );
	~cWinAvpgImpl();

	tERROR SetActivityMode ( tAVPG2_CLIENT_ACTIVITY_MODE ActivityMode );
	tERROR ChangeWorkingMode( tAVPG2_WORKING_MODE WorkingMode );

	tERROR ResetDriverCache();
	tERROR CreateIO( hOBJECT* p_phObject, hSTRING p_hObjectName );
	tERROR YieldEvent( hOBJECT EventObject, tDWORD YieldTimeout );
	tERROR ClearTrustedProcessesCache();

	tERROR ResetBanList();
	tERROR AddBan( cUserInfo* pUserInfo );

public:
	cContext m_Context;
	cAvpgImpl* m_pInstance;

private:
	ULONG m_DriverFlags;
};

#endif // _wavpg_h