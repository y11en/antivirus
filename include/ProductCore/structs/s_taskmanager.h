#ifndef __S_TASKMANAGER_H
#define __S_TASKMANAGER_H

#include <Prague/pr_vector.h>
#include <ProductCore/plugin/p_tm.h>
#include <ProductCore/structs/s_scheduler.h>
#include <ProductCore/structs/s_settings.h>

// ---
typedef enum tag_TMSerIds {
	tms_cTaskInfo       = 0,
	tms_cTaskSchedule,
	tms_cTaskStatistics,
	tms_cTaskStatisticsList,
	tms_cTaskStatisticsListItem,
	tms_cTaskInfoEx,
	tms_cTaskReportDsc,
	tms_cReportInfo,
	tms_cTaskInfoList,
	tms_cTaskInfoExList,
	tms_cCfg,
	tms_cCfgEx,
	tms_cProfileBase,
	tms_cProfile,
	tms_cProfileEx,
	tms_cProfileList,
	tms_cProfileExList,
	tms_cScheduleParams,
	tms_cTaskParams,
	tms_cProfileEvent,
	tms_cTaskRunAs,
	tms_cCloseRequest,
	tms_cTaskHeader,
	tms_cTaskSleepingMode,
	tms_cAKUnsupportedField,
	tms_cAKUnsupportedFields,
	tms_cTaskSleepingAppItem,
	tms_cTaskAskAction,

	tms_LAST
} tTMSerIds;


// ---
struct cProfile;
struct cProfileEx;

// --------------------------------------------------

#define SETTINGS_LEVEL_UNK     ((tDWORD)-1)
#define SETTINGS_LEVEL_RESET   ((tDWORD)-2)
#define SETTINGS_LEVEL_DEFAULT 0
#define SETTINGS_LEVEL_CUSTOM  1


typedef enum enProfileState {
	PROFILE_STATE_UNK                   = TASK_STATE_UNK,
	PROFILE_STATE_RUNNING               = TASK_STATE_RUNNING,
	PROFILE_STATE_RUNNING_MALFUNCTION   = TASK_STATE_RUNNING_MALFUNCTION,
	PROFILE_STATE_PAUSED                = TASK_STATE_PAUSED,
	PROFILE_STATE_STOPPED               = TASK_STATE_STOPPED,
	PROFILE_STATE_COMPLETED             = TASK_STATE_COMPLETED,
	PROFILE_STATE_FAILED                = TASK_STATE_FAILED,
	PROFILE_STATE_ENABLED               = TASK_STATE_CREATED,
	PROFILE_STATE_DISABLED              = 0x0000D0,
	PROFILE_STATE_STARTING              = TASK_STATE_STARTING,
	PROFILE_STATE_PAUSING               = TASK_STATE_PAUSING,
	PROFILE_STATE_RESUMING              = TASK_STATE_RESUMING,
	PROFILE_STATE_STOPPING              = TASK_STATE_STOPPING,
} tProfileState;

typedef enum enAdmFlags {
	PROFILE_ADMFLAG_NONE                = 0x0000,
	PROFILE_ADMFLAG_ADMINABLE           = 0x0001,
	PROFILE_ADMFLAG_INVISIBLE           = 0x0002,
	PROFILE_ADMFLAG_UNDELETABLE         = 0x0004,
	PROFILE_ADMFLAG_POLICY              = 0x0008,
	PROFILE_ADMFLAG_GROUPTASK           = 0x0010,
} tAdmFlags;

// --------------------------------------------------
// struct is used to get info from running task

struct cTaskInfo : public cSerializable {
	cTaskInfo() :
		cSerializable(),
		m_sType(),
		m_sProfile(),
		m_nTaskId(TASKID_UNK),
		m_nState(TASK_STATE_UNK),
		m_nParentId(TASKID_UNK),
		m_bService(cFALSE),
		m_iid(IID_ANY),
		m_pid(PID_ANY)
	{}
	~cTaskInfo() { clear(); }
	
	DECLARE_IID( cTaskInfo, cSerializable, PID_TM, tms_cTaskInfo );

	virtual tVOID  pr_call clear() { cSerializable::clear(); m_sType.clean(); m_sProfile.clean(); m_nTaskId = TASKID_UNK;	m_nState = TASK_STATE_UNK; }

	cStrObj    m_sType;
	cStrObj    m_sProfile;
	tTaskId    m_nTaskId;
	tTaskState m_nState;
	tTaskId    m_nParentId;
	tBOOL      m_bService;
	tIID       m_iid;
	tPID       m_pid;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskInfo, 0 )
	SER_VALUE( m_sType,    tid_STRING_OBJ, "type" )
	SER_VALUE( m_sProfile, tid_STRING_OBJ, "name" )
	SER_VALUE( m_nTaskId,  tid_DWORD,      "taskId" )
	SER_VALUE( m_nState,   tid_DWORD,      "state" )
	SER_VALUE( m_nParentId,tid_DWORD,      "parentId" )
	SER_VALUE( m_bService, tid_BOOL,       "isServive" )
	SER_VALUE( m_iid,      tid_DWORD,      "iid" )
	SER_VALUE( m_pid,      tid_DWORD,      "pid" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------
struct cTaskHeader : public cSerializable {
	cTaskHeader() :
		m_nPID(0),
		m_nSessionId(-1),
		m_nTaskId(TASKID_UNK),
		m_nBaseId(TASKID_UNK)
	{
	}

	DECLARE_IID( cTaskHeader, cSerializable, PID_TM, tms_cTaskHeader );

	tQWORD     m_nPID;
	tDWORD     m_nSessionId;
	tTaskId    m_nTaskId;
	tTaskId    m_nBaseId;
	cStringObj m_strTaskType;
	cStringObj m_strProfile;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskHeader, 0 )
	SER_VALUE( m_nSessionId,       tid_DWORD, 0 )
	SER_VALUE( m_nTaskId,          tid_DWORD,      "TaskId" )
	SER_VALUE( m_nBaseId,          tid_DWORD,      "BaseId" )
	SER_VALUE( m_strTaskType,      tid_STRING_OBJ, "TaskType" )
	SER_VALUE( m_strProfile,       tid_STRING_OBJ, "Profile" )
	SER_VALUE( m_nPID,             tid_QWORD,      "PID" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

struct cTaskParams : public cSerializable {
	cTaskParams() :
		cSerializable(),
		m_sType(),
		m_sProfile(),
		m_sDescription() {
	}

	DECLARE_IID( cTaskParams, cSerializable, PID_TM, tms_cTaskParams );

	cStrObj                m_sType;
	cStrObj                m_sProfile;
	cStrObj                m_sDescription;
	cSerObj<cSerializable> m_settings;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskParams, 0 )
	SER_VALUE( m_sType,          tid_STRING_OBJ,      "type" )
	SER_VALUE( m_sProfile,       tid_STRING_OBJ,      "name" )
	SER_VALUE( m_sDescription,   tid_STRING_OBJ,      "description" )
	SER_VALUE_PTR( m_settings,   cSerializable::eIID, "settings" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------
// base structure for ANY task statistic

struct cTaskStatistics : public cSerializable {
	cTaskStatistics() :
		cSerializable(),
		m_lastError( errOK ),
		m_nPercentCompleted(0),
		m_timeStart(-1), 
		m_timeFinish(-1) {
	}

	DECLARE_IID( cTaskStatistics, cSerializable, PID_TM, tms_cTaskStatistics );

	tERROR m_lastError;
	tDWORD m_nPercentCompleted;  // if you don't know how to fill it - just leave it unchanged!
	tDWORD m_timeStart;					 // if you don't know how to fill it - just leave it unchanged!
	tDWORD m_timeFinish;				 // if you don't know how to fill it - just leave it unchanged!
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskStatistics, 0 )
	SER_VALUE( m_lastError,         tid_ERROR, "LastError" )
	SER_VALUE( m_nPercentCompleted, tid_DWORD, "Completion" )
	SER_VALUE( m_timeStart,         tid_DWORD, "StartTime" )
	SER_VALUE( m_timeFinish,        tid_DWORD, "FinishTime" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cTaskStatisticsPerUser : public cTaskStatistics
{
	DECLARE_IID( cTaskStatisticsPerUser, cTaskStatistics, PID_ANY, setst_cTaskStatisticsPerUser );

	cStringObj             m_sUserId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskStatisticsPerUser, 0 )
	SER_BASE( cTaskStatistics, 0 )
	SER_KEY_VALUE( m_sUserId, "UserId" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cTaskStatisticsListItem : public cSerializable {

	DECLARE_IID( cTaskStatisticsListItem, cSerializable, PID_TM, tms_cTaskStatisticsListItem );

	cStrObj                      m_sProfile;
	cSerObj<cSerializable>       m_statistics;
};

// ---
struct cTaskStatisticsList : public cSerializable
{
public:
	DECLARE_IID( cTaskStatisticsList, cSerializable, PID_TM, tms_cTaskStatisticsList );

public:
	cVector<cTaskStatisticsListItem>  m_aItems;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskStatisticsListItem, 0 )
	SER_VALUE(     m_sProfile,   tid_STRING_OBJ,      0 )
	SER_VALUE_PTR( m_statistics, cSerializable::eIID, "statistics" )
IMPLEMENT_SERIALIZABLE_END();

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskStatisticsList, 0 )
	SER_VECTOR(m_aItems, cTaskStatisticsListItem::eIID, NULL)
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

struct cTaskRunAs : public cSerializable {
	cTaskRunAs() :
		m_enable( cFALSE ) {
	}

	DECLARE_IID( cTaskRunAs, cSerializable, PID_TM, tms_cTaskRunAs );

	enum Actions { CHECK_TASK_IMPERSONATED=eIID };

	tBOOL      m_enable;
	cStringObj m_sAccount;
	cStringObj m_sPassword;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskRunAs, 0 )
	SER_VALUE(m_enable,         tid_BOOL,            "enable" )
	SER_VALUE(m_sAccount,       tid_STRING_OBJ,      "account" )
	SER_PASSWORD(m_sPassword,  "password" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
// structure for task sleeping settings

struct cTaskSleepingAppItem : public cEnabledStrItem {

	cTaskSleepingAppItem() : m_bTriggered(cFALSE){}

	DECLARE_IID( cTaskSleepingAppItem, cEnabledStrItem, PID_TM, tms_cTaskSleepingAppItem );

	tBOOL       m_bTriggered;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskSleepingAppItem, 0 )
	SER_BASE( cEnabledStrItem, 0 )
IMPLEMENT_SERIALIZABLE_END();

struct cTaskSleepingMode : public cPolicySettings {
	cTaskSleepingMode() :
		m_bEnablePeriod(cFALSE),
		m_tmFrom(0),
		m_tmTo(0),
		m_bEnableAppList(cFALSE)
	{
	}

	DECLARE_IID( cTaskSleepingMode, cPolicySettings, PID_TM, tms_cTaskSleepingMode );
		
	tBOOL           m_bEnablePeriod;
	__time32_t      m_tmFrom;
	__time32_t      m_tmTo;

	tBOOL           m_bEnableAppList;
	cVector<cTaskSleepingAppItem> m_aAppList;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskSleepingMode, 0 )
	SER_BASE( cPolicySettings, 0 )
	SER_VALUE( m_bEnablePeriod,     tid_DWORD,   "EnablePeriod" )
	SER_VALUE( m_tmFrom,            tid_DWORD,   "From" )
	SER_VALUE( m_tmTo,              tid_DWORD,   "To" )
	SER_VALUE( m_bEnableAppList,    tid_DWORD,   "EnableAppList" )
	SER_VECTOR( m_aAppList,         cTaskSleepingAppItem::eIID,    "AppList" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

struct cAKUnsupportedField : public cSerializable {
	cAKUnsupportedField() {}

	DECLARE_IID( cAKUnsupportedField, cSerializable, PID_TM, tms_cAKUnsupportedField );

	tDWORD  m_serId;
	cStrObj m_sFieldName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAKUnsupportedField, 0 )
	SER_VALUE(     m_serId,     tid_DWORD,       "sid" )
	SER_VALUE(     m_sFieldName,tid_STRING_OBJ,  "FieldName" )
IMPLEMENT_SERIALIZABLE_END();

typedef cVector<cAKUnsupportedField> cAKUnsupportedFieldVector;

struct cAKUnsupportedFields : public cSerializable, cAKUnsupportedFieldVector {
	using cSerializable::operator==;
	DECLARE_IID( cAKUnsupportedFields, cSerializable, PID_TM, tms_cAKUnsupportedFields );
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAKUnsupportedFields, 0 )
	SER_BASE_EX( cAKUnsupportedFieldVector, SDT_FLAG_VECTOR, cAKUnsupportedField::eIID, "" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cCfg : public cPolicySettings {

public:
	typedef enum tag_Flags { fSettings=0x01, fSchedule=0x02, fRunAs=0x04,
		fPolicy=0x08, fSleepMode=0x10, fWithoutPolicy=0x20, fApplyPolicy=0x40,
		fAKUnsupportedFields=0x80, fGroupOfCfg=0x100000,
		allFlags=(fSettings|fSchedule|fRunAs|fSleepMode|fAKUnsupportedFields) } tFlags;
	
public:
	cCfg( tDWORD f = allFlags ) :
		m_bEnabled(cFALSE),
		m_bRemote(cFALSE),
		m_dwLevel(SETTINGS_LEVEL_DEFAULT),
		m_dwInstances(1),
		m_settings(),
		m_schedule(),
		m_runas()
	{ flags(f); }
	~cCfg() { clear(); }

	DECLARE_IID( cCfg, cPolicySettings, PID_TM, tms_cCfg );

public:
	virtual tVOID  pr_call clear() { 
		cPolicySettings::clear();
		m_bEnabled = cFALSE; 
		m_dwLevel = SETTINGS_LEVEL_DEFAULT;
		m_settings.clear(); 
		m_schedule.clear(); }

	tDWORD                 flags()           const { return m_nFlags; }
	tVOID                  flags( tDWORD f )       { m_nFlags = f; }
	tBOOL                  enabled()         const { return m_bEnabled; }
	tDWORD                 level()           const { return m_dwLevel; }
	tDWORD&                level()                 { return m_dwLevel; }

	cTaskSettings*&        settings()                       { return (cTaskSettings*&)m_settings.ptr_ref(); }
	const cTaskSettings*   settings()                 const { return (const cTaskSettings*)(const cSerializable*)m_settings; }
	tBOOL                  compatible( const cProfile& cmp ) const;

	bool                   isMySettings( const cSerializable& settings ) const { return m_settings ? m_settings->isBasedOn(settings.getIID()) : false; }

	cTaskSchedule&         schedule()                       { return m_schedule; }
	const cTaskSchedule&   schedule()                 const { return m_schedule; }
	
	cTaskRunAs&            run_as()                         { return m_runas; }
	const cTaskRunAs&      run_as()                   const { return m_runas; }

public:
	tDWORD                 m_nFlags;
	tBOOL                  m_bEnabled;
	tDWORD                 m_dwLevel;
	tBOOL                  m_bRemote;
	tDWORD                 m_dwInstances;
	cSerObj<cSerializable> m_settings;
	cTaskSchedule          m_schedule;
	cTaskRunAs             m_runas;
	cTaskSleepingMode      m_smode;
	cAKUnsupportedFields   m_aAKUnsupportedFields;
};


IMPLEMENT_SERIALIZABLE_BEGIN( cCfg, 0 )
	SER_BASE( cPolicySettings, 0 )
	SER_VALUE(     m_nFlags,     tid_DWORD,                "flags" )
	SER_VALUE(     m_bEnabled,   tid_BOOL,                 "enabled" )
	SER_VALUE(     m_dwLevel,    tid_DWORD,                "level" )
	SER_VALUE(     m_bRemote,    tid_BOOL,                 "remote" )
	SER_VALUE(     m_dwInstances,tid_DWORD,                "instances" )
	SER_VALUE(     m_schedule,   cTaskSchedule::eIID,      "schedule" )
	SER_VALUE(     m_runas,      cTaskRunAs::eIID,         "runas" )
	SER_VALUE(     m_smode,      cTaskSleepingMode::eIID,  "smode" )
	SER_VALUE_PTR( m_settings,   cSerializable::eIID,      "settings" )
	SER_VALUE(    m_aAKUnsupportedFields, cAKUnsupportedFields::eIID,   "AKUnsupportedFields" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

struct cCfgEx : public cCfg {

public:
	cCfgEx( tDWORD f = allFlags ) :
		cCfg(f),
		m_sType(),
		m_sName(),
		m_aChildren()
	{
	}
	cCfgEx( const cProfileEx& src, bool dirty_only );

	DECLARE_IID( cCfgEx, cCfg, PID_TM, tms_cCfgEx );

public:
	tVOID  pr_call clear() { cCfg::clear(); m_sType.clean(); m_sName.clean(); m_aChildren.clear(); }
	tBOOL  empty() const;
	cCfgEx* find(cStrObj& sType);
	tBOOL  compatible( const cProfileEx& cmp ) const;
	tERROR assignProfile( const cProfileEx& src, bool dirty_only );
	tERROR copyToProfile( cProfileEx& dst ) const;

	tUINT         count()                const { return m_aChildren.count(); }
	cCfgEx&       operator[] ( tSIZE_T i )       { return m_aChildren[i]; }
	const cCfgEx& operator[] ( tSIZE_T i ) const { return m_aChildren[i]; }

public:
	cStrObj         m_sType;
	cStrObj         m_sName;
	cVector<cCfgEx> m_aChildren;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cCfgEx, 0 )
	SER_BASE( cCfg, 0 )
	SER_VALUE(     m_sType,     tid_STRING_OBJ, "type" )
	SER_KEY_VALUE( m_sName,                     "name" )
	SER_VECTOR(    m_aChildren, cCfgEx::eIID,   "subItems" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

struct cTaskReportDsc : public cSerializable {
	cTaskReportDsc() :
		cSerializable(),
		m_id(),
		m_name(),
		m_baseSerId(cSerializable::eIID),
		m_aClasses() {
	}
	
	DECLARE_IID( cTaskReportDsc, cSerializable, PID_TM, tms_cTaskReportDsc );

	virtual tVOID  pr_call clear() { cSerializable::clear(); m_id.clean(); m_name.clean(); m_baseSerId = cSerializable::eIID; m_aClasses.clear(); }

	cStrObj         m_id;
	cStrObj         m_name;
	cStrObj         m_sTaskType;
	tDWORD          m_baseSerId;
	cVector<tDWORD> m_aClasses;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskReportDsc, 0 )
	SER_KEY_VALUE( m_id, "" )
	SER_VALUE( m_name,      tid_STRING_OBJ, "" )
	SER_VALUE( m_baseSerId, tid_DWORD,      "baseSerId" )
	SER_VALUE( m_sTaskType, tid_STRING_OBJ, "TaskType" )
	SER_VECTOR( m_aClasses, tid_DWORD,      "" )
IMPLEMENT_SERIALIZABLE_END();

// ---
struct cTaskReportDscList : public cVector<cTaskReportDsc> {
	tDWORD get_report_baseserid( const tCHAR* name ) {
		tUINT i=0, c = count();
		for( ; i<c; ++i ) {
			if ( at(i).m_id == (tSTRING)name )
				return at(i).m_baseSerId;
		}
		return cSerializable::eIID;
	}
};

// --------------------------------------------------

struct cProfileBase : public cSerializable {
	enum tProfileOrigin { 
		poUnk = -1, poSystem = 1, poUser, poLocalAdmin, poRemoteAdmin, poTemporary, poPrivate
	};

	cProfileBase() :
		cSerializable(),
		m_nRuntimeId( TASKID_UNK ),
		m_sType(), 
		m_sName(),
		m_sDescription(),
		m_eOrigin( poUnk ),
		m_bPersistent( cFALSE ),
		m_bService(cFALSE),
		m_nAdmFlags( PROFILE_ADMFLAG_NONE ),
		m_nState( PROFILE_STATE_UNK )
	{
	}
	~cProfileBase() { clear(); }

	DECLARE_IID( cProfileBase, cSerializable, PID_TM, tms_cProfileBase );

	tBOOL          isItYou( const tCHAR* name ) const { return !name || !*name || !m_sName.compare(name,fSTRING_COMPARE_CASE_INSENSITIVE); }
	tProfileState  state()                      const { return m_nState; }
	tTaskState     task_state()                 const { return ( m_nState == PROFILE_STATE_DISABLED ) ? TASK_STATE_COMPLETED : tTaskState(m_nState); }
	tUINT          id()                         const { return m_nRuntimeId; }
	tBOOL          persistent()                 const { return m_bPersistent; }
	bool           isService()                  const { return !!m_bService; }
	bool           isProtection()               const { return m_sName == AVP_PROFILE_PROTECTION; }
	bool           isTask()                     const { return !isService() && !isProtection(); }
	const cStrObj& serviceId()                  const { return m_sName; }
	tBOOL          running()                    const { tTaskState st = task_state(); return IS_TASK_RUNNING(st) || IS_IN_STATE(st,TASK_STATE_STARTING); }
	tERROR         fillTaskHeader( cSerializable& ser ) const;

	virtual tVOID  pr_call clear() { cSerializable::clear(); m_sType.clean(); m_sName.clean(); m_sDescription.clean(); m_nState = PROFILE_STATE_UNK; }

	tDWORD         m_nRuntimeId;
	cStrObj        m_sType;
	cStrObj        m_sName;
	cStrObj        m_sDescription;
	tProfileOrigin m_eOrigin;
	tBOOL          m_bPersistent;
	tBOOL          m_bService;
	tDWORD         m_nAdmFlags;
	cERROR         m_lastErr;
	tProfileState  m_nState;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProfileBase, 0 )
	SER_VALUE(     m_nRuntimeId,     tid_DWORD,       "id" )
	SER_VALUE(     m_sType,          tid_STRING_OBJ,  "type" )
	SER_KEY_VALUE( m_sName,                           "name" )
	SER_VALUE(     m_sDescription,   tid_STRING_OBJ,  "description" )
	SER_VALUE(     m_eOrigin,        tid_DWORD,       "origin" )
	SER_VALUE(     m_bPersistent,    tid_BOOL,        "persistent" )
	SER_VALUE(     m_bService,       tid_DWORD,       "ServiceId" )
	SER_VALUE(     m_nAdmFlags,      tid_DWORD,       "admflags" )
	SER_VALUE(     m_lastErr,        tid_BOOL,        "lastErr" )
	SER_VALUE(     m_nState,         tid_DWORD,       "state" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

struct cProfile : public cProfileBase {
public:
	typedef enum tag_Flags { fStatistics = 0x0100, fReports = 0x0200, myFlags = (fStatistics|fReports), allFlags=cCfg::allFlags|myFlags } tFlags;

public:
	cProfile( tDWORD f = allFlags ) :
		cProfileBase(),
		m_statistics(),
		m_nSerIdSettings(0),
		m_nSerIdStatistics(0),
		m_bSingleton(cTRUE),
		m_bEventsDeleted(cFALSE),
		m_tmCreationDate(0),
		m_tmLastModified(0),
		m_tmLastCompleted(0)
	{ flags(f); }
	~cProfile() { clear(); }

	virtual tVOID  pr_call clear() { 
		cProfileBase::clear(); 
		m_cfg.clear(); 
		m_statistics.clear(); 
		m_nSerIdSettings = m_nSerIdStatistics = 0; 
		m_bService = cFALSE;
		m_nAdmFlags = PROFILE_ADMFLAG_NONE; 
		m_bSingleton = cTRUE; 
		m_tmCreationDate = 0; 
		m_tmLastModified = 0; 
		m_tmLastCompleted = 0;
	}

	DECLARE_IID( cProfile, cProfileBase, PID_TM, tms_cProfile );

	enum Actions
	{
		PREPROCESS_SHEDULED_EVENT=eIID,
		PREPROCESS_INIT_TASK,
		PREPROCESS_START_TASK,
		PREPROCESS_RESTART_TASK,
		PREPROCESS_SLEEPING_MODE,
	};

public:
	operator       cCfg& ()       { return m_cfg; }
	operator const cCfg& () const { return m_cfg; }

	tDWORD                 flags()           const { return m_cfg.flags() | m_nFlags; }
	tVOID                  flags( tDWORD f )       { m_nFlags = (f & myFlags); m_cfg.flags(f); }

	tDWORD                 level()      const { return m_cfg.level(); }
	tDWORD&                level()            { return m_cfg.level(); }
	tBOOL                  enabled()    const { return m_cfg.enabled(); }
	tBOOL                  singleton()  const { return m_bSingleton || persistent(); }

	cTaskSettings*&	       settings()                       { return m_cfg.settings(); }
	const cTaskSettings*   settings()                 const { return m_cfg.settings(); }

	bool                   isMySettings( const cSerializable& settings ) const { return m_cfg.isMySettings(settings); }

	cTaskSchedule&         schedule()                       { return m_cfg.schedule(); }
	const cTaskSchedule&   schedule()                 const { return m_cfg.schedule(); }

	virtual bool           isSettingsDirty() const { return false; }

	cTaskStatistics*&      statistics()                       { return (cTaskStatistics*&)m_statistics.ptr_ref(); }
	const cTaskStatistics* statistics()                 const { return (const cTaskStatistics*)(const cSerializable*)m_statistics; }
	
public:
	tDWORD                 m_nFlags;
	cCfg                   m_cfg;
	cSerObj<cSerializable> m_statistics;
	
	cStrObj                m_sCategory;
	tSerializableId        m_nSerIdSettings;
	tSerializableId        m_nSerIdStatistics;
	tBOOL                  m_bSingleton;
	tBOOL                  m_bEventsDeleted;
	__time32_t             m_tmCreationDate;
	__time32_t             m_tmLastModified;
	__time32_t             m_tmLastCompleted;
};

#define PROFILE_CREATION_TIME_NAME "creationDate"
#define PROFILE_MODIFIED_TIME_NAME "lastModified"

IMPLEMENT_SERIALIZABLE_BEGIN( cProfile, 0 )
	SER_BASE(  cProfileBase, 0 )
	SER_VALUE( m_nFlags,           tid_DWORD,           "flags" )
	SER_VALUE( m_cfg,              cCfg::eIID,          "" )
	SER_VALUE_PTR( m_statistics,   cSerializable::eIID, "statistics" )
	SER_VALUE( m_sCategory,        tid_STRING_OBJ,      "category" )
	SER_VALUE( m_nSerIdSettings,   tid_DWORD,           "idSettings" )
	SER_VALUE( m_nSerIdStatistics, tid_DWORD,           "idStatistics" )
	SER_VALUE( m_bSingleton,       tid_BOOL,            "singleton" )
	SER_VALUE( m_bEventsDeleted,   tid_BOOL,            "eventsDeleted" )
	SER_VALUE( m_tmCreationDate,   tid_DWORD,           PROFILE_CREATION_TIME_NAME )
	SER_VALUE( m_tmLastModified,   tid_DWORD,           PROFILE_MODIFIED_TIME_NAME )
	SER_VALUE( m_tmLastCompleted,  tid_DWORD,           "lastCompleted" )
IMPLEMENT_SERIALIZABLE_END();


// --------------------------------------------------

// ---
typedef cSerObj<cProfileEx>                                   cProfileExPtr;
typedef cVector< cProfileExPtr, cSimpleMover<cProfileExPtr> > cProfileExPtrVector;

// ---
struct cProfileExList : public cSerializable, public cProfileExPtrVector {
	
	cProfileExList();
	~cProfileExList();
	
	DECLARE_IID( cProfileExList, cSerializable, PID_TM, tms_cProfileExList );
	
public:
	tVOID       pr_call  clear();
	cProfileEx*          find( const tCHAR* profile, bool go_down = false );
	const cProfileEx*    find( const tCHAR* profile, bool go_down = false ) const;
	tUINT                remove( tINT from, tINT to );
	tUINT                remove( const tCHAR* profile );
	tUINT                remove( const cProfile* profile );

	tUINT                      count()                const;
	cSerObj<cProfileEx>&       operator[] ( tUINT i );
	const cSerObj<cProfileEx>& operator[] ( tUINT i ) const;
};

// --------------------------------------------------

struct cProfileEx : public cProfile {
public:
	cProfileEx( tDWORD flags = allFlags ) :
		cProfile( flags ),
		m_aChildren(),
		m_aReports() {
	}
	~cProfileEx() { clear(); }

	DECLARE_IID( cProfileEx, cProfile, PID_TM, tms_cProfileEx );

	tVOID operator = ( const cProfileEx& o );

public:
	cProfileExList      m_aChildren;
	cTaskReportDscList  m_aReports;

public:
	tVOID             pr_call clear() { cProfile::clear(); m_aChildren.clear(); m_aReports.clear(); }

	cTaskReportDscList&       reports()                       { return m_aReports; }
	const cTaskReportDscList& reports()                 const { return m_aReports; }

	cProfileEx*               find_profile( const tCHAR* name, bool check_root = true, bool go_down = false );
	virtual bool              isSomeSettingsDirty() const;

	tUINT                     count()                            const { return m_aChildren.count(); }
	cProfileEx*               operator[] ( tUINT i )                   { return m_aChildren[i]; }
	const cProfileEx*         operator[] ( tUINT i )             const { return m_aChildren[i]; }

	cProfileEx*               operator[] ( const tCHAR* name )         { return m_aChildren.find( name ); }
	const cProfileEx*         operator[] ( const tCHAR* name )   const { return m_aChildren.find( name );	}
	
	cProfileEx*               operator[] ( const cStrObj& name )       { return m_aChildren.find(name.c_str(cCP_ANSI)); }
	const cProfileEx*         operator[] ( const cStrObj& name ) const { return m_aChildren.find(name.c_str(cCP_ANSI));	}
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProfileEx, 0 )
	SER_BASE( cProfile, 0 )
	SER_VALUE( m_aChildren, cProfileExList::eIID, "profiles" )
	SER_VECTOR( m_aReports, cTaskReportDsc::eIID, "reports" )
IMPLEMENT_SERIALIZABLE_END();

IMPLEMENT_SERIALIZABLE_BEGIN( cProfileExList, 0 )
	SER_BASE_EX( cProfileExPtrVector, SDT_FLAG_VECTOR|SDT_FLAG_POINTER, cProfileEx::eIID, "" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cReportInfo : public cProfileBase {
  cReportInfo() :
		m_tmStart(-1),
		m_tmFinish(-1),
		m_tmLastChange(0),
		m_nReportCount(0),
		m_nReportSize(0) {
	}

public:
	DECLARE_IID( cReportInfo, cProfileBase, PID_TM, tms_cReportInfo );

public:
	tDWORD   m_tmStart;
	tDWORD   m_tmFinish;
	tDWORD   m_tmLastChange;
	tUINT    m_nReportCount;
	tQWORD   m_nReportSize;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cReportInfo, 0 )
	SER_BASE( cProfileBase, 0 )
	SER_VALUE( m_tmStart,       tid_DWORD,  "StartTime" )
	SER_VALUE( m_tmFinish,      tid_DWORD,  "FinishTime" )
	SER_VALUE( m_tmLastChange,  tid_DWORD,  "LastChange" )
	SER_VALUE( m_nReportCount,  tid_UINT,   "ReportCount" )
	SER_VALUE( m_nReportSize,   tid_QWORD,  "ReportSize" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

struct cCloseRequest : public cSerializable {
	enum Cause
	{
		cNone = 0,
		cStopService = 1,
		cKillProcess = 2,
		cBlOnly = 3,
		cByUser = 4,
	};

	cCloseRequest(Cause cause = cNone) : m_nCause(cause){}

	DECLARE_IID( cCloseRequest, cSerializable, PID_TM, tms_cCloseRequest );

	Cause      m_nCause;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cCloseRequest, 0 )
	SER_VALUE( m_nCause,    tid_DWORD, "Cause" )
IMPLEMENT_SERIALIZABLE_END();

// --------------------------------------------------

enum enActions
{
	ACTION_UNKNOWN          = 0x0000,
	ACTION_DISINFECT        = 0x0001,
	ACTION_QUARANTINE       = 0x0002,
	ACTION_DELETE           = 0x0004,
	ACTION_TERMINATE        = 0x0008,
	ACTION_ALLOW            = 0x0010,
	ACTION_DENY             = 0x0020,
	ACTION_ROLLBACK         = 0x0040,
	ACTION_REPARE           = 0x0080,
	ACTION_OK               = 0x0100,
	ACTION_REPORTONLY       = 0x0200,
	ACTION_CANCEL           = 0x0400,
	ACTION_DELETE_ARCHIVE   = 0x0800,
	ACTION_TRYAGAIN         = 0x1000,
	ACTION_BACKUP           = 0x2000,
	ACTION_RENAME           = 0x4000,
	ACTION_ALL              = 0xFFFF,
};

enum enApplyToAll
{
	APPLYTOALL_FALSE       = 0x0,
	APPLYTOALL_TRUE        = 0x1,
	APPLYTOALL_NONE        = 0x2,
	APPLYTOALL_SESSION     = 0x4,
	APPLYTOALL_USE_SESSION = 0x8,
};

enum enDetectDanger
{
	DETDANGER_UNKNOWN       = 0,
	DETDANGER_HIGH          = 1,
	DETDANGER_MEDIUM        = 2,
	DETDANGER_LOW           = 4,
	DETDANGER_INFORMATIONAL = 8,
	DETDANGER_ALL_MASK      = 15,
};

struct cTaskAskAction : public cTaskHeader
{
	cTaskAskAction() :
		m_nActionId(0),
		m_nActionsAll(0),
		m_nActionsMask(0),
		m_nDefaultAction(ACTION_REPORTONLY),
		m_nResultAction(ACTION_REPORTONLY),
		m_nExcludeAction(ACTION_UNKNOWN),
		m_nApplyToAll(APPLYTOALL_FALSE),
		m_nDetectDanger(DETDANGER_UNKNOWN),
		m_bSystemCritical(cTRUE),
		m_bSound(cTRUE)
	{}

	DECLARE_IID( cTaskAskAction, cTaskHeader, PID_TM, tms_cTaskAskAction );

	tActionId         m_nActionId;
	tDWORD		      m_nActionsAll;
	tDWORD		      m_nActionsMask;
	enActions	      m_nDefaultAction;
	enActions         m_nResultAction;
	enActions         m_nExcludeAction;
	enApplyToAll      m_nApplyToAll;
	enDetectDanger    m_nDetectDanger;
	tBOOL             m_bSystemCritical;
	tBOOL             m_bSound;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cTaskAskAction, 0 )
	SER_BASE( cTaskHeader, 0 )
	SER_VALUE( m_nActionId,        tid_DWORD, NULL )
	SER_VALUE( m_nActionsAll,      tid_DWORD, "ActionsAll" )
	SER_VALUE( m_nActionsMask,     tid_DWORD, "ActionsMask" )
	SER_VALUE( m_nDefaultAction,   tid_DWORD, "DefaultAction" )
	SER_VALUE( m_nResultAction,    tid_DWORD, "ResultAction" )
	SER_VALUE( m_nExcludeAction,   tid_DWORD, "ExcludeAction" )
	SER_VALUE( m_nApplyToAll,      tid_DWORD, "ApplyToAll" )
	SER_VALUE( m_nDetectDanger,    tid_DWORD, "DetectDanger" )
	SER_VALUE( m_bSystemCritical,  tid_BOOL,  NULL )
	SER_VALUE( m_bSound,           tid_BOOL,  "Sound" )
IMPLEMENT_SERIALIZABLE_END()

// --------------------------------------------------
// inlines
// --------------------------------------------------


// ---
inline tBOOL cCfg::compatible( const cProfile& cmp ) const {
	const cTaskSettings* o = cmp.settings();
	if ( !m_settings || !o )
		return cTRUE;
	if ( o->isBasedOn(m_settings->getIID()) || m_settings->isBasedOn(o->getIID()) )
		return cTRUE;
	return cFALSE;
}


inline cCfgEx::cCfgEx( const cProfileEx& src, bool dirty_only ) :
	cCfg( src.flags() ),
	m_sType(),
	m_sName(),
	m_aChildren() {

	assignProfile( src, dirty_only );

}


inline tBOOL cCfgEx::empty() const {
	return m_sName.empty() && !m_settings && !m_aChildren.count();
}

inline cCfgEx* cCfgEx::find(cStrObj& sType) {
	if( sType == m_sType )
		return this;
	for( tUINT i=0,c=count(); i<c; ++i )
		if( cCfgEx* cfg = m_aChildren.at(i).find(sType) )
			return cfg;
	return NULL;
}

inline tBOOL cCfgEx::compatible( const cProfileEx& cmp ) const {
	if ( empty() )
		return cTRUE;
	if ( !cCfg::compatible(cmp) )
		return cFALSE;
	for( tUINT i=0,c=count(); i<c; ++i ) {
		const cCfgEx& cfg = m_aChildren[i];
		const cProfileEx* tp = cmp[cfg.m_sName];
		if ( !tp || !cfg.compatible(*tp) )
			return cFALSE;
	}
	return cTRUE;
}


inline tERROR cCfgEx::assignProfile( const cProfileEx& src, bool dirty_only ) {

	clear();

	cERROR e;

	tDWORD nFlags = flags();
	
//	if ( !dirty_only || src.isSettingsDirty() )
		*(cCfg*)this = src.m_cfg;
//	else
//		flags(0);
	
	e = m_sName.assign( src.m_sName );
	if ( PR_SUCC(e) )
		e = m_sType.assign( src.m_sType );

	tUINT  i, c;
	for( i=0,c=src.count(); PR_SUCC(e) && (i<c); ++i ) {
		const cProfileEx& prf = *src[i];
		if ( !dirty_only || prf.isSomeSettingsDirty() ) {
			cCfgEx& dst = m_aChildren.push_back( cCfgEx(nFlags) );
			e = dst.assignProfile( prf, dirty_only );
		}
	}
	return e;
}


inline tERROR cCfgEx::copyToProfile( cProfileEx& dst ) const {
	
	tUINT  i, c = dst.count();

	cERROR e = cCfg::copy( dst.m_cfg, false );
	for( i=0; PR_SUCC(e) && (i<c); ++i ) {
		const cCfgEx& cfg = m_aChildren[i];
		cProfileEx* tp = dst[cfg.m_sName];
		if ( !tp )
			return errPARAMETER_INVALID;
		e = cfg.copyToProfile( *tp );
	}
	return e;
}


inline tERROR cProfileBase::fillTaskHeader( cSerializable& ser ) const {
	if ( !ser.isBasedOn(cTaskHeader::eIID) )
		return errOBJECT_INCOMPATIBLE;
	cTaskHeader& th = (cTaskHeader&)ser;
	if ( th.m_nTaskId == TASKID_UNK )
		th.m_nTaskId = id();
	if ( th.m_strTaskType.empty() )
		th.m_strTaskType = m_sType;
	if ( th.m_strProfile.empty() )
		th.m_strProfile = m_sName;
	return errOK;
}

inline tVOID cProfileEx::operator = ( const cProfileEx& o ) {
	*(cProfile*)this = o;
	m_aReports = o.m_aReports;
	m_aChildren.clear();
	tDWORD f = flags();
	tUINT i = 0, c = o.count();
	for( ; i<c; ++i ) {
		cProfileExPtr& tp = m_aChildren.push_back();
		tp.init( getIID() );
		tp->flags( f );
		tp.assign( *o[i] );
	}
}

inline bool cProfileEx::isSomeSettingsDirty() const {
	if ( isSettingsDirty() )
		return true;
	for( tUINT i=0, c=m_aChildren.count(); i<c; ++i ) {
		if ( m_aChildren[i]->isSettingsDirty() )
			return true;
	}
	return false;
}


// ---
inline cProfileEx* cProfileEx::find_profile( const tCHAR* name, bool check_root, bool go_down ) {
	if ( check_root && isItYou(name) )
		return this;
	
	tUINT i=0, c = m_aChildren.count();
	for( ; i<c; ++i ) {
		cProfileEx& tp = *m_aChildren[i];
		if ( tp.isItYou(name) )
			return &tp;
	}
	
	if( !go_down )
		return 0;
	
	for( i=0; i<c; ++i ) {
		cProfileEx& tp = *m_aChildren[i];
		cProfileEx* found = tp.find_profile( name, false, true );
		if ( found )
			return found;
	}
	
	return 0;
}

inline cProfileExList::cProfileExList() 
	: cSerializable(), cProfileExPtrVector() {}

inline cProfileExList::~cProfileExList() { 
	clear(); 
}

inline tVOID pr_call cProfileExList::clear() { 
	cSerializable::clear(); cProfileExPtrVector::clear(); 
}

inline const cProfileEx* cProfileExList::find( const tCHAR* profile, bool go_down ) const { 
	cProfileEx* prf = const_cast<cProfileExList*>(this)->find(profile,go_down);
	 return const_cast<cProfileEx*>(prf); 
}

inline tUINT cProfileExList::remove( tINT from, tINT to ) { 
	return cProfileExPtrVector::remove( from, to ); 
}

inline tUINT cProfileExList::count() const { 
	return cProfileExPtrVector::count(); 
}

inline cSerObj<cProfileEx>& cProfileExList::operator[] ( tUINT i ) { 
	return cProfileExPtrVector::operator[](i); 
}

inline const cSerObj<cProfileEx>& cProfileExList::operator[] ( tUINT i ) const { 
	return cProfileExPtrVector::operator[](i); 
}

inline cProfileEx* cProfileExList::find( const tCHAR* profile, bool go_down ) {
	tUINT i, c;
	cProfileEx* tp;
	if ( !profile || !*profile )
		return 0;
	for( i=0,c=count(); i<c; ++i ) {
		if ( (tp=at(i))->isItYou(profile) )
			return tp;
	}
	if ( !go_down )
		return 0;
	for( i=0,c=count(); i<c; ++i ) {
		if ( (tp=(cProfileEx*)at(i)->m_aChildren.find(profile,true)) )
			return tp;
	}
	return 0;
}


inline tUINT cProfileExList::remove( const tCHAR* profile ) {
	tUINT i, c;
	if ( !profile || !*profile )
		return 0;
	for( i=0,c=count(); i<c; ++i ) {
		cProfile* tp = at(i);
		if ( tp->isItYou(profile) )
			return cProfileExPtrVector::remove( i );
	}
	return 0;
}


inline tUINT cProfileExList::remove( const cProfile* profile ) {
	tUINT i, c;
	for( i=0,c=count(); i<c; ++i ) {
		cProfileEx* tp = at(i);
		if ( tp == profile )
			return cProfileExPtrVector::remove( i );
	}
	return 0;
}

#endif

