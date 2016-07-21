#ifndef __JOB_THREAD__
#define __JOB_THREAD__

#include "TaskThread.h"

#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/plugin/p_thpoolimp.h>

#include <OAS/iface/i_avpg.h>
#include <OAS/plugin/p_avpg.h>

#include <ProductCore/structs/s_trmod.h>

#include "EvCache.h"

#include <vector>

#include <ntsecapi.h>
#include <Lm.h>

#define _IGNORE_HASH_LEN	32

typedef enum _teEventOperation{
	_op_other			= 0,
	_op_create			= 1,
	_op_write			= 2,
	_op_close			= 3,
	_op_volume_verify	= 4,
	_op_delayed			= 5,
	_op_execute			= 8,
} _eEventOperation;
//+ ------------------------------------------------------------------------------------------

typedef struct _AVPGS_EVENT_DESCRIBE
{
	HANDLE				m_hDevice;
	SET_VERDICT			m_SetVerdict;
	PEVENT_TRANSMIT		m_pEvt;
	_eEventOperation	m_eop;
	__int64				m_Timeout;
	HANDLE				m_hTokenImpersonate;
	bool				m_bProcessing;
	bool				m_bBreakProcessing;
	bool				m_bMiniKav;
	LUID				m_CreatorLuid;
	LUID				m_UserOverrideLuid;
	PVOID				m_PtrFN;
}AVPGS_EVENT_DESCRIBE, *PAVPGS_EVENT_DESCRIBE;

class CJob;

class CEvSync
{
private:
	CRITICAL_SECTION m_CrSec;
public:

	CEvSync()
	{
		InitializeCriticalSection(&m_CrSec);
	}

	~CEvSync()
	{
		DeleteCriticalSection(&m_CrSec);
	}

	inline void Lock(){EnterCriticalSection(&m_CrSec);}
	inline void UnLock(){LeaveCriticalSection(&m_CrSec);}
};

typedef enum _eContextType
{
	_e_tht_Work = 0,
	_e_tht_Delayed = 1,
	_e_tht_Notify = 2,
};

class cThreadContext
{
public:
	cThreadContext()
	{
		m_pJob = 0;
		m_pNotifyParamData = 0;
		m_NotifyParamDataSize = 0;

		m_pCommon = 0;
		m_ContextType = _e_tht_Work;
	}
	
	~cThreadContext()
	{
	}
	
	_eContextType	m_ContextType;
	CJob*			m_pJob;
	tDWORD*			m_pNotifyParamData;
	tDWORD			m_NotifyParamDataSize;
	cThreadContext* m_pCommon;
};

typedef struct _sIGNORE_PROC_LIST
{
	ULONG	m_ProcessId;
	bool	m_bIgnoreTis;
}IGNORE_PROC_LIST, *PIGNORE_PROC_LIST;

typedef std::vector<IGNORE_PROC_LIST> _tIgnorePidList;


typedef std::vector<cUserInfo> _tLuidInfoList;

class CJob : public HookTask, public ThreadParam
{
public:
	CJob::CJob(hOBJECT hTask);
	virtual ~CJob();

	bool	Start();

	void	BeforeExit();
	void	FiltersChanged(){};
	
	void	SingleEvent();
	bool	AddFilters();
	bool	RestartFilters();

	//+ ------------------------------------------------------------------------------------------
	ULONG	GetCacheSize(){return m_CacheSize;};
	void	SetMaxThreadCount(DWORD maxthreads);
	//+ ------------------------------------------------------------------------------------------

	ULONG			m_CacheSize;
	volatile bool	m_bIsSkipEvents;
	cThreadContext	m_CommonThreadContext;
	cThreadContext	m_DelayedThreadContext;
	cThreadContext	m_NotifyThreadContext;
	DrvEventList	m_DrvEventList;

	DWORD			m_WorkingMode;
	bool			m_bSkipBackupFiles;
	
	void SendMessage(tDWORD msg_cls, tDWORD msg_id, tPTR buf, tDWORD* pblen);
	tERROR AddInvisibleFile(tSTRING pcz);

	void ProcessEvent(PAVPGS_EVENT_DESCRIBE pEvDescr);

	void ProcessDelayedEvent(bool bForce);
	void ForceDelayedEvent();

	void FreeEvDescr(PAVPGS_EVENT_DESCRIBE pEvDescr);
	void DriveConnect_Message(PEVENT_TRANSMIT pEvt);

	void IgnoreProcess_ResetList();

	tERROR GetBanList(cUserBanList* pBanList);
	tERROR SetBanList(cUserBanList* pBanList);
	tERROR AddUserBan(cUserBanItem* pBanItem, bool bReSend);
	tERROR DeleteUserBan(cUserBanItem* pBanItem, bool bReSend);

	void EnumUsers();
	void AddCrInfo(cAvpgsDelayedNotify* pCrInfo);
	
	void SetMultiProcessMode();

	void SkipEvents();

private:
	tERROR SetObjectContext (
		PAVPGS_EVENT_DESCRIBE pEvDescr,
		PDRV_EVENT_DESCRIBE pDrvEvDescr,
		cString* hctx,
		PSINGLE_PARAM* ppSingleObjectName
		);

	bool CreateThreadPool(cThreadPool** ppThPool, tDWORD MaxLen, tThreadPriority Priority, tDWORD MinThreads, 
		tDWORD MaxThreads, tDWORD IdleCount, tPTR pContext);

	bool CheckDelayedList(PAVPGS_EVENT_DESCRIBE pEvDescr, bool bFreeItem);

	bool IsExistInPostPone(PAVPGS_EVENT_DESCRIBE pEvDescr, bool bStopPostpone);
	
	bool
	AddToDelayedList (
		PAVPGS_EVENT_DESCRIBE pEvDescr,
		PHashTree pTree,
		PHashTreeItem* pHashItemNew,
		DWORD dwTimeShift,
		bool bIsPostPone);

	bool IsExistFilters();

	hOBJECT			m_hTask;
	cThreadPool*	m_ThPoolWork;
	cThreadPool*	m_ThPoolConnect;
	cThreadPool*	m_ThPoolDelayed;
	cThreadPool*	m_ThPoolNotify;
	DWORD			m_MaxThreadCount;

	UINT			m_DrivesType['Z' - 'A' + 1];

	CEvSync			m_EvSync;
	PHashTree		m_PHashTree_Delayed;
	PHashTree		m_PHashTree_PostPone;

	__int64			m_PerfomanceFreq;

	BOOL (WINAPI *m_pfOpenThreadToken)(HANDLE ThreadHandle, DWORD DesiredAccess, BOOL OpenAsSelf, PHANDLE TokenHandle);
	BOOL (WINAPI *m_pfSetThreadToken)(PHANDLE Thread, HANDLE Token);

	HMODULE m_hFltDll;
	HRESULT (WINAPI *m_FilterGetDosName)(LPCWSTR  lpVolumeName, LPWSTR  lpDosName, DWORD  dwDosNameBufferSize); 
	tERROR FillObjectName(cString* hctx, PEVENT_TRANSMIT pEvt, PSINGLE_PARAM pSingleObjectName, ULONG ContextFlags);

	// this functions and data are using only in 1 thread. no need to synchronize
	_tIgnorePidList m_IgnorePidList;
	bool IgnoreProcess_IsSkipThis(ULONG ProcessId);
	
	// 
	HMODULE m_hSecLib;
	NET_API_STATUS (NTAPI* m_fnLsaGetLogonSessionData)(PLUID LogonId, PSECURITY_LOGON_SESSION_DATA* ppLogonSessionData);
	NET_API_STATUS (NTAPI* m_fnLsaFreeReturnBuffer)(PVOID Buffer);

	CEvSync m_LuidListSync;

	HMODULE m_hNetApi;
	NET_API_STATUS (NET_API_FUNCTION* m_pfNetSessionEnum) (
		IN  LPWSTR       servername OPTIONAL,
		IN  LPWSTR       UncClientName OPTIONAL,
		IN  LPWSTR       username OPTIONAL,
		IN  DWORD       level,
		OUT LPBYTE      *bufptr,
		IN  DWORD       prefmaxlen,
		OUT LPDWORD     entriesread,
		OUT LPDWORD     totalentries,
		IN OUT LPDWORD  resume_handle OPTIONAL
	);

	NET_API_STATUS (NET_API_FUNCTION* m_pfNetApiBufferFree) (
		IN LPVOID Buffer
	);

	bool SetUserAndBanInfo(cString* hctx, cUserInfo* pUserInfo, cUserInfo* pUserBanInfo, PLUID pCreatorLuid, PLUID pUserOverrideLuid);

	bool IsUserRemote( PLUID pLuid );

	bool IsUserBaned(cUserInfo* pUserInfo);
	
	bool FillLuidInfo(cUserInfo* pUserInfo, bool bTryToResolve);
	bool FillUserInfoByLuid(PLUID pLuid, cUserInfo* pUserInfo, bool bTryToResolve);

	bool FindExistingLuidInfo(cUserInfo* pUserInfo);
	void AddNewLuidInfo(cUserInfo* pUserInfo);

	_tLuidInfoList	m_LuidInfoList;
	ULONG			m_LuidCount;

// ban list
	cUserBanList m_BanList;

	void PrintUserName(SID* pSid, char* pszmsg);
	

	bool m_bMultiProcessMode;
};

#endif // __JOB_THREAD__