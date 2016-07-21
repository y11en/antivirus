#if !defined(__tm_profile_h)
#define __tm_profile_h

#include <pr_remote.h>
#include <pr_cpp.h>
#include <pr_time.h>
#include <iface/i_productlogic.h>
#include <iface/i_threadpool.h>
#include <iface/i_csect.h>
#include <iface/i_event.h>
#include <iface/i_report.h>
#include <iface/i_token.h>
#include <structs/s_taskmanager.h>
//#include <structs/s_avs.h>
//#include <structs/s_qb.h>
#include "tm_util.h"

#define IMPEX_IMPORT_LIB
#include <plugin/p_win32_nfio.h>
#include <plugin/p_string.h>

struct TMImpl;
struct cTmProfileRequest;

// ------------------------------------------------

struct cTmProfile : public cProfileEx, public cThreadTaskBase
{
public:
	cTmProfile() :
		m_iid(IID_ANY),
		m_pid(PID_ANY),
		m_dwReleaseTimeout(0),
		m_runInSafeMode(cFALSE),
		m_bInstalled(cFALSE),
		m_bFromReport(cFALSE),
		m_bGroup(cFALSE),
		m_bClone(cFALSE),
		m_bTaskGroup(cFALSE),
		m_bFailOver(cFALSE),
		m_bAutoRestart(cFALSE),
		m_dieWithClient(cFALSE),
		m_bQueueEmpty(cTRUE),
		m_bHasTemplate(cFALSE),
		m_bWaitState(cFALSE),
		m_bDeleted(cFALSE),
		m_dad(0),
		m_host(0),
		m_client(0),
		m_task(0),
		m_taskObj(0),
		m_token(0),
		m_storage(0),
		m_remoteTask(0),
		m_lock(0),
		m_ref(1),
		m_lockRef(0),
		m_reportRef(0),
		m_timeStart(-1),
		m_timeFinish(-1),
		m_timeAccess(0),
		m_timeLastSchedRun(-1),
		m_dwSleepingMode(0),
		m_scheduleId(0),
		m_processId(PR_PROCESS_LOCAL),
		m_stateFlags(0),
		m_changedFlags(0),
		m_request(NULL)
	{}

	DECLARE_IID( cTmProfile, cProfileEx, PID_TM, tms_LAST+1 );

	enum enStateFlags
	{
		dfSchedule       = cCfg::fSchedule,
		dfRunAs          = cCfg::fRunAs,
		dfSleepMode      = cCfg::fSleepMode,
		dfSettings       = cCfg::fSettings,
		dfAKUnsupportedFields = cCfg::fAKUnsupportedFields,
		dfDescription    = 0x100,
		dfTaskSettings   = 0x200,
		dfReport         = 0x400,
		dfLevel          = 0x800,
		dfEnable         = 0x1000,
		dfInstances      = 0x2000,
		dfCfg            = dfEnable|dfLevel|dfInstances,
		dfDirtyFlags     = 0xFFFF,

		dfSendModified   = 0x10000,
		dfSettingsLoaded = 0x20000,
		dfHistoryInited  = 0x40000,
		dfTaskRestarted  = 0x80000,
	};

public:
	tDWORD         AddRef();
	tDWORD         Release();

	tERROR         LockForReport(bool bLock);
	cTmProfile*    GetChild(tDWORD pos){ return (cTmProfile*)(cProfileEx*)m_aChildren.at(pos); }
	void           UpdateChilds(cTmProfile* child, bool bAdd);

	cTmProfile*    FindProfile(const tCHAR* profile, tDWORD task_id, hOBJECT object);
	cTmProfile*    MakeRemoteProfile(tDWORD pid);
	cTmProfile*    RegisterRemoteTask(cTaskInfo* info, cObj* task, tDWORD processId);

	virtual tERROR AskActionEx(tDWORD action_id, cSerializable* params, hOBJECT sender);
	virtual tERROR OpenReport(const tCHAR* p_name, hOBJECT p_client, tDWORD access, cReport** p_report);
	virtual tERROR ProcessMessage(cSerializable* data, tDWORD msg_cls, tDWORD msg_id, cObj* sender);

	virtual tERROR InitProfile(cTmProfile* pParent);
	tERROR         InitProfileInstances();
	tERROR         InitFromReport(cTmProfile* pParent, cProfileEx& data);
	tERROR         InitFromReport(cProfileEx& data);
	tERROR         DeinitProfile();

	tERROR         Clone(cTmProfile* clone, cProfile* data, bool bTmp);
	tERROR         Delete();

	tERROR         SendMessage(tDWORD msg_cls, tDWORD msg_id);

	tERROR         GetInfo(cSerializable* info, bool bByTaskId);
	tERROR         GetInfo(cProfileBase& info, bool bByTaskId);
	tERROR         GetCfg(cCfg& cfg, bool bByTaskId);
	tERROR         GetStatistics(cTaskStatistics*& stat);
	tERROR         GetSettings(tCustomizeSettingsDirection direction, cSerializable*& settings, bool bByTaskId=false);

	tERROR         SetCfg(cCfg& cfg);
	tERROR         SetSettings(tCustomizeSettingsDirection direction, cSerializable* info, bool bStore=true);
	tERROR         Enable(tBOOL p_enabled, tDWORD flags = 0);

	tERROR         LockSettings(cSerializable** p_settings);
	tERROR         UnlockSettings(cSerializable* p_settings, tBOOL p_modified, tBOOL p_apply = cTRUE);

	tERROR         InitHost(bool bLocker);
	tERROR         InitDataStorage(cObject* obj, cRegistry** p_storage = NULL);
	tERROR         InitSchedule(cTaskSchedule* sch = NULL, bool bApplyChanges = false);
	tERROR         InitProfileKey(cStrObj& str);

	tERROR         LoadCfg(cCfg* cfg, bool bPolicy);
	tERROR         LoadSettings(cSerializable* settings, tDWORD level, bool bApply=false, bool bPolicy=true);
	tERROR         SaveCfg(cCfg& cfg, tDWORD flags, bool bPolicy);
	tERROR         SaveChanges(tDWORD flags = dfDirtyFlags);
	tERROR         SaveScheduledRun();
	tERROR         ApplyChanges();
	tERROR         CheckStopAllowed(bool bRecursive);
	tERROR         CheckSleepingMode();
	tERROR         CheckImpersonation(cTaskRunAs* pRunAs);

	tERROR         InitTask(tTaskRequestState state);
	tERROR         CreateTask();
	tERROR         AttachTask(cObj* task);
	tERROR         LockTask();
	tERROR         UnlockTask();
	tERROR         LockTaskReports(bool bLock, bool bInit = true);
	tERROR         SetTaskSettings(cSerializable* settings = NULL);
	tERROR         SetTaskState(tTaskRequestState state);
	tERROR         SetTaskImpersonation(bool bReset);
	tERROR         GetTaskSettings(cSerializable*& settings);
	tERROR         GetTaskStatistics(cSerializable* stat);

	tERROR         SetStateRequest(tTaskRequestState state, tDWORD delay=0, tDWORD flags=0, cObj* client = NULL, tTaskId* id = NULL, cTmProfileRequest* parent = NULL);
	tERROR         SetInfoRequest(cSerializable* info, tDWORD delay=0, tDWORD flags=0, cTmProfileRequest* parent = NULL);

	tERROR         StateChanged(tProfileState state, tERROR nError = errOK, cObject* task = NULL);

	bool           IsReportable(){ return m_dad && !m_dad->m_dad && !isService(); }
	bool           IsReportFiles(){ return m_dad && m_aReports.size(); }
	bool           IsActive(){ return !!(m_nState & STATE_FLAG_ACTIVE); }
	bool           IsPaused(){ return m_nState == PROFILE_STATE_PAUSED; }
	bool           IsPersistent(){ return persistent() && !isService(); }
	bool           IsEnabled(){ return (!IsPersistent() || !m_dad || !enabled()) ? !!enabled() : m_dad->IsEnabled(); }
	bool           IsChildsDisabled();
	bool           IsEnableLocked();
	bool           IsLevelLocked();

protected:
	static tERROR  MsgReceive(hOBJECT _this, tDWORD msg_cls, tDWORD msg_id, hOBJECT obj, hOBJECT ctx, hOBJECT recieve_point, tPTR pbuff, tDWORD* blen);
	tERROR         ReportMessage(cSerializable* data, tDWORD msg_cls, cTaskHeader* header);
	tERROR         AdjustInactiveState(tProfileState& state);
	tERROR         UpdateGroupState();

	cTmProfileRequest* GetRequest();
	tERROR         PutRequest(cTmProfileRequest* request);
	tERROR         ProcessRequest(cTmProfileRequest* request);
	tERROR         ScheduleRequest(tDWORD delay, tDWORD& flags, tTaskRequestState state, cSerializable* info);

	tERROR         SetState();
	tERROR         SetInfo();

	virtual void   on_exit(){ Release(); }
	virtual void   do_work();

public:
	tIID           m_iid;
	tPID           m_pid;

	tBOOL          m_bInstalled;
	tBOOL          m_bGroup;
	tBOOL          m_bTaskGroup;
	tBOOL          m_bClone;
	tBOOL          m_bAutoRestart;
	tBOOL          m_bFailOver;
	tBOOL          m_runInSafeMode;
	tBOOL          m_bFromReport;
	tBOOL          m_bQueueEmpty;
	tBOOL          m_dieWithClient;
	tBOOL          m_bDeleted;
	tBOOL          m_bImpersonated;

	tDWORD         m_dwReleaseTimeout;
	tDWORD         m_scheduleId;
	tDWORD         m_processId;
	tDWORD         m_stateFlags;
	tDWORD         m_changedFlags;

	tLONG          m_ref;
	tLONG          m_lockRef;
	cRefLocker     m_enumRef;
	cRefLocker     m_taskRef;

	cStrObj        m_sProfileKey;
	cStrObj        m_sTypeKey;
	tBOOL          m_bHasTemplate;

	tDWORD         m_timeStart;
	tDWORD         m_timeFinish;
	tDWORD         m_timeAccess;
	tDWORD         m_timeLastSchedRun;
	tLONG          m_dwSleepingMode;

	TMImpl*        m_tm;
	cTmProfile*    m_dad;
	cMsgReceiver*  m_host;
	cRegistry*     m_storage;
	cToken*        m_token;
	cObject*       m_client;
	cObject*       m_task;
	cTask*         m_taskObj;
	cObject*       m_remoteTask;

	tBOOL          m_bWaitState;
	tProfileState  m_nNewState;

	cRefLocker         m_reportLock;
	tLONG              m_reportRef;
	cVector<cReport*>  m_vecReports;

	cCriticalSection*  m_lock;
	cTmProfileRequest* m_request;
	cVector<cTmProfileRequest*> m_queue;
};

// ------------------------------------------------

struct cTmTaskType : public cSerializable
{
	cTmTaskType() : 
			m_nSerIdSettings(0), 
			m_nSerIdStatistics(0), 
			m_iid(IID_ANY),
			m_pid(PID_ANY),
			m_settings(0) {}

	DECLARE_IID( cTmTaskType, cSerializable, PID_TM, tms_LAST+2 );

	cStrObj         m_sType;
	cStrObj         m_serviceId;
	tSerializableId m_nSerIdSettings;
	tSerializableId m_nSerIdStatistics;
	tIID            m_iid;
	tPID            m_pid;
	cSerObj<cTaskSettings> m_settings;
	cTaskReportDscList     m_aReports;
};

// ------------------------------------------------

struct cScheduleParams : public cSerializable
{
	cScheduleParams() {}
	cScheduleParams(const cProfile& tp, tTaskRequestState state, const cSerializable* info) : 
		m_sName(tp.m_sName),
		m_bWaitForInet(cFALSE),
		m_nFlags(0),
		m_state(state),
		m_info(info)
	{
	}

	DECLARE_IID( cScheduleParams, cSerializable, PID_TM, tms_LAST+3 );

	cStrObj                m_sName; 
	tBOOL                  m_bWaitForInet;
	tDWORD                 m_nFlags;
	tTaskRequestState      m_state;
	cSerObj<cSerializable> m_info;
};

// ------------------------------------------------

enum enProfileRequest
{
	PROFILE_REQUEST_SETSTATE,
	PROFILE_REQUEST_SETINFO,
	PROFILE_REQUEST_DELETE,
};

struct cTmProfileRequest
{
	cTmProfileRequest(cTmProfileRequest* parent, tDWORD flags, tTaskRequestState requestState, hOBJECT client, tDWORD* taskId);
	cTmProfileRequest(cTmProfileRequest* parent, tDWORD flags, cSerializable* data, tDWORD serid = 0);
	cTmProfileRequest(enProfileRequest request);
	~cTmProfileRequest();

	void init(cTmProfileRequest* parent, tDWORD flags);
	void signal(tERROR err = errOK);
	void set_inited();
	void wait_for_childs(tBOOL& flag);
	void wait_for_childs_inited();
	void wait_for_parent_inited();

	enProfileRequest   m_request;
	tDWORD             m_flags;
	tTaskRequestState  m_requestState;
	cSerializableObj   m_data;
	cTmProfileRequest* m_pParent;
	tLONG              m_nChilds;
	hOBJECT            m_client;
	tDWORD*            m_taskId;
	cEvent*            m_waiter;
	tERROR*            m_pResult;
	cToken*            m_token;
	tBOOL              m_bInited;
	tLONG              m_nChildsInited;
};

struct cTmProfilePtr
{
	cTmProfilePtr(TMImpl* tm, const tCHAR* name, cObj* client=NULL);
	cTmProfilePtr(TMImpl* tm, tDWORD task_id);
	cTmProfilePtr(TMImpl* tm, hOBJECT object);
	cTmProfilePtr(cTmProfile* profile) : m_obj(profile){};
	~cTmProfilePtr(){ if( m_obj ) m_obj->Release(); }

	cTmProfile* operator->() { return m_obj; }
	operator cTmProfile*(){ return m_obj; }

private:
	cTmProfile*        m_obj;
};

struct cTmProfileEnum
{
	cTmProfileEnum(cTmProfile* profile) : m_profile(profile), m_obj(NULL), m_pos(0){ m_profile->m_enumRef.lock(); }
	~cTmProfileEnum(){ done(); }

	cTmProfile* operator->() { return m_obj; }
	operator cTmProfile*(){ return m_obj; }

	void done()
	{
		if( m_profile )
			m_profile->m_enumRef.unlock();
		m_profile = NULL;
	}

	bool next(bool bPersistent = false)
	{
		m_obj = m_pos < m_profile->m_aChildren.size() ? m_profile->GetChild(m_pos++) : NULL;

		if( !m_obj )
			done();
		else if( bPersistent && !m_obj->IsPersistent() )
			return next(true);

		return !!m_obj;
	}

private:
	cTmProfile*        m_profile;
	cTmProfile*        m_obj;
	tDWORD             m_pos;
};

struct cTmTaskPtr
{
	cTmTaskPtr(cTmProfile* profile, bool active = false) : m_profile(profile), m_active(active){ m_profile->m_taskRef.lock(); }
	~cTmTaskPtr(){ m_profile->m_taskRef.unlock(); }

	cTask* operator->() { return m_profile->m_taskObj; }
	operator cTask*(){ return (!m_active || m_profile->IsActive()) ? m_profile->m_taskObj : NULL; }

private:
	cTmProfile*        m_profile;
	bool			   m_active;
};

// ------------------------------------------------

#define REQUEST_FLAG_WAIT             ((tDWORD)(0x00000001L))
#define REQUEST_FLAG_LOCK_TASK        ((tDWORD)(0x00000002L))
#define REQUEST_FLAG_UNLOCK_TASK      ((tDWORD)(0x00000004L))
#define REQUEST_FLAG_FORALL_TASKS     ((tDWORD)(0x00000008L))
#define REQUEST_FLAG_BY_SCHEDULE      ((tDWORD)(0x00000010L))
#define REQUEST_FLAG_FOR_TASK_ONLY    ((tDWORD)(0x00000020L))
#define REQUEST_FLAG_SAME_TASKID      ((tDWORD)(0x00000040L))
#define REQUEST_FLAG_FORCE_RUN        ((tDWORD)(0x00000080L))
#define REQUEST_FLAG_RESTART_TASK     ((tDWORD)(0x00000100L))
#define REQUEST_FLAG_NON_RECURSIVE    ((tDWORD)(0x00000200L))
#define REQUEST_FLAG_APPLY_POLICY     ((tDWORD)(0x00000400L))
#define REQUEST_FLAG_FROM_TASK        ((tDWORD)(0x00000800L))
#define REQUEST_FLAG_FROM_TASK        ((tDWORD)(0x00000800L))
#define REQUEST_FLAG_FLUSH_DATA       ((tDWORD)(0x00001000L))

#define ENABLE_FLAG_RECURSIVE         ((tDWORD)(0x00000001L))
#define ENABLE_FLAG_NOSYNC            ((tDWORD)(0x00000002L))
#define ENABLE_FLAG_BYCFG             ((tDWORD)(0x00000004L))

// ------------------------------------------------

#endif // __tm_profile_h
