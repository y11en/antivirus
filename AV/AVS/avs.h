// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Wednesday,  04 April 2007,  19:18 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Doukhvalow
// File Name   -- avs.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __avs_cpp__de7d4dd9_c91a_436e_80f0_9599d726821a )
#define __avs_cpp__de7d4dd9_c91a_436e_80f0_9599d726821a
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_reg.h>

#include <AV/plugin/p_avs.h>
// AVP Prague stamp end



#include <stdio.h>

#define IMPEX_IMPORT_LIB

#include <Prague/pr_time.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_threadpool.h>
#include <Prague/iface/e_loader.h> 
#include <Prague/iface/i_event.h>
#include <Prague/plugin/p_fsdrvplgn.h>
#include <Prague/plugin/p_win32_nfio.h>
#include <Prague/plugin/p_dtreg.h>

#include <AV/plugin/p_avspm.h> 
#include <AV/plugin/p_avlib.h> 
#include <AV/plugin/p_avpmgr.h> 
#include <AV/structs/s_avs.h>
#include <AV/structs/s_avp3info.h>

#include <ProductCore/iface/i_cryptohelper.h>
#include <ProductCore/iface/i_processmonitor.h>
#include <ProductCore/iface/i_taskmanager.h>
#include <ProductCore/plugin/p_report.h> 
#include <ProductCore/plugin/p_icheckersa.h>

#include <Extract/plugin/p_windiskio.h>

#include <ISLite/IChecker2/ichecker2.h>
#include <ISLite/IChecker2/plugin_ichecker2.h>
#include <ISLite/iCheckerStreams/plugin_ichstrms.h>

#include <Extract/iface/i_minarc.h>
#include "../../Extract/Wrappers/Dmap/plugin_dmap.h"

#include "enginehnd.h"
#include "avstreats.h"
#include "scan.h"

#ifdef _WIN32
#include <windows.h>
#endif


#define MAX_ENGINES 5

//public cAVS , public cThreadPoolBase
// AVP Prague stamp begin( C++ class declaration,  )
struct AVSImpl : public cAVS , public cThreadPoolBase
{
private:
	AVSImpl(): cThreadPoolBase("AVS") {}

// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call SetSettings( const cSerializable* p_settings );
	tERROR pr_call GetSettings( cSerializable* p_settings );
	tERROR pr_call AskAction( tActionId p_actionId, cSerializable* p_actionInfo );
	tERROR pr_call SetState( tTaskState p_state );
	tERROR pr_call GetStatistics( cSerializable* p_statistics );
	tERROR pr_call Update( const cSerializable* p_update_info );
	tERROR pr_call CreateSession( hAVSSESSION* p_result, hTASK p_task, tDWORD p_engine_flags, tDWORD p_scan_origin );
	tERROR pr_call GetTreats( tDWORD p_taskid, hAVSTREATS* p_treats );
	tERROR pr_call ReleaseTreats( hAVSTREATS p_treats );
	tERROR pr_call MatchExcludes( const cSerializable* p_object_info );
	tERROR pr_call ProcessCancel( tDWORD p_process_id );

// Data declaration
	hREGISTRY     m_config;      // --
	cTaskManager* m_tm;          // --
	tDWORD        m_taskId;      // --
	tDWORD        m_parentId;    // --
	tDWORD        m_state;       // --
	tDWORD        m_sessionId;   // --
	hOBJECT       m_persStorage; // --
// AVP Prague stamp end
public:
	cOS *         m_nos;
	cAVSPM *      m_avspm;
	cAVSSettings  m_settings;
	cAVSStatistics m_statistics;
	tLONG         m_origin_times[3];
	tBOOL         m_bIsRemote;

public:
	tERROR on_command(tDWORD command, void* ctx);

	tERROR UpdateStatistics();
	virtual tERROR GetPerformance(cAVSPerformance* pData);

	CAVSEngineMng& GetEngineMng(enEngineType eType);
	tERROR CreateEngine(enEngineType eType, CAVSEngine** p_engine, cAVSUpdateInfo* pInfo);
	tERROR DestroyEngine(CAVSEngine* hEngine);
	tERROR LockEngines(tDWORD eMask, tBOOL bLock, enEngineType* eBaseEngine=NULL);
	tERROR LockEngine(enEngineType eType, CAVSEngine** p_engine);
	tERROR UnlockEngine(CAVSEngine* hEngine);

	tERROR MatchExcludesEx(cDetectObjectInfo* pInfo, bool bShort=false);
	tERROR AddThreatToQuaranine(AVSSession* pSession, cInfectedObjectInfo* pInfInfo);

	void   ProcessBegin(CScanContext& pScanCtx);
	void   ProcessEnd(CScanContext& pScanCtx);
	bool   ProcessYield(CScanContext& pScanCtx, tDWORD nTime);

	void   AddCancelProcessRequest(CScanContext& pScanCtx);
	void   SendCancelProcessRequest(CScanContext& pScanCtx, tDWORD nAction);

	bool   IsLockedObjectProcessed(tQWORD qwUnique);
	tERROR LockedObjectProcessed(AVSSession* pSession, cInfectedObjectInfo* pInfInfo, enObjectStatus nStatus);

	bool   InitTreatsInfo();
	void   VerifyTreatsInfo();
	bool   InitTreatsData(tDWORD nTaskId, AVSTreats* pTreats);
	bool   SyncThreatsWhithQB(tDWORD nMsgId, tQWORD qwId);
	void   DiscardTreatsData(const cDwordVector& vecPos);
	void   DiscardTreatsByDetectMask();
	bool   SaveTreatsData(AVSTreats* pTreats);
	bool   UpdateTreatInfo(AVSSession* pSession, cInfectedObjectInfo* pInfInfo);
	bool   DeleteTreatInfo(tDWORD nStorePos, bool bNotify);
	bool   GetTreatInfo(tDWORD nStoredPos, cInfectedObjectInfo* pInfInfo);
	bool   AddTreatInfo(AVSSession* pSession, cInfectedObjectInfo* pInfInfo);
	bool   UpdateTreatInfo(tDWORD nStorePos, cInfectedObjectInfo* pInfInfo);
	bool   UpdateTreatStatus(AVSSession* pSession, tTreatInfo& pInfo, cInfectedObjectInfo* pInfInfo = NULL, cInfectedObjectInfo* pLocked = NULL, bool bByHash = false);
	bool   UpdateTasksThreat(AVSSession* pSession, tTreatInfo& pInfo, bool bNew);
	AVSTreats* GetTaskTreats(tDWORD nTaskId);
	void   FillExcludedList(cDwordVector& vecIdx, tQWORD qwUniqueId);

	bool   RegisterPassword(tQWORD qwUnique, cStringObj& strPswd);
	bool   GetPassword(tQWORD qwUnique, cStringObj& strPswd);

	bool   CheckActiveDetection(cStringObj& strPath);

private:
	struct tDetectedIds
	{
		tDetectedIds(cInfectedObjectInfo& info, tDWORD nStatus = 0)
		{
			m_qwUniqueId = info.m_qwUniqueId;
			m_qwParentId = info.m_qwParentId;
			m_qwQBId = info.m_qwQBObjectId;
			m_nStatus = nStatus ? nStatus : info.m_nObjectStatus;
		}

		tDetectedIds(tQWORD qwUniqueId = 0) : m_qwUniqueId(qwUniqueId){}

		bool operator == (const tDetectedIds& c) const
			{ return m_qwUniqueId == c.m_qwUniqueId; }

		tQWORD          m_qwUniqueId;
		tQWORD          m_qwParentId;
		tQWORD          m_qwQBId;
		tDWORD          m_nStatus;
	};

	struct tPassword
	{
		tQWORD          m_qwUniqueId;
		cStrObj         m_strPswd;
	};

	CAVSEngineHnd*            m_AVP3hnd;
	CAVSEngineHnd*            m_KLAVhnd;
	cCriticalSection*         m_lock;
	cCriticalSection*         m_treatslock;
	cVector<tDetectedIds>     m_vecLocked;
	cVector<tDetectedIds>     m_vecDetected;
	cVector<tPassword>        m_vecPasswords;
	cVector<tDWORD>           m_vecIdxCache;
	CAVSEngineMng             m_vecEngins[MAX_ENGINES];
	cVector<AVSTreats*>       m_vecTreats;
	cVector<CScanContext*>    m_vecContexts;
	cReport *                 m_pTreatsStore;
	cReport *                 m_pTreatsIdx;
	cAskProcessTreats *       m_pToVerify;
	tBOOL                     m_bSettings;
	cProcessMonitor *         m_hProcMon;

	tDWORD		              m_EnterToProcessTime;
	tDWORD		              m_TotalWorkingTime;
	tDWORD		              m_ProcessingCounter;

	static tPROPID            m_propIsRemote;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(AVSImpl)
};
// AVP Prague stamp end



#define IS_INFECTED(status)  ((status) == OBJSTATUS_NOTDISINFECTED)
#define IS_ARCHIVE(status)   ((status) == OBJSTATUS_ARCHIVE)
#define IS_DISCARDED(status) ((status) == (enObjectStatus)(-1))
#define IS_INVISIBLE(status) (IS_DISCARDED(status) || (status) == OBJSTATUS_ARCHIVE )
#define IS_REBOOT(status)    ((status) == OBJSTATUS_DELETED_ON_REBOOT || (status) == OBJSTATUS_DISINFECTED_ON_REBOOT || (status) == OBJSTATUS_QUARANTINED_ON_REBOOT )

#define pmc_AVS_SYNC_THREATS      0x47cf7719 //
#define pmc_AVS_QB_ACTION         0x7868928f //
#define pm_AVS_ASK_ACTION         0x4df15289 //

// AVP Prague stamp begin( Header endif,  )
#endif // avs_cpp
// AVP Prague stamp end



