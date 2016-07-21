#ifndef __S_AVS_H
#define __S_AVS_H

#include <Prague/pr_oid.h>
#include <Prague/iface/i_io.h>

#include <AV/plugin/p_avs.h>

#include <ProductCore/common/UserBan.h>
#include <ProductCore/structs/s_taskmanager.h>

enum _eAVS_IID
{
	avsst_cScanProcessInfo          = 1,
	avsst_cObjectInfo               = 4,
	avsst_cInfectedObjectInfo       = 5,
	avsst_cDetectObjectInfo         = 6,
	avsst_cObjectPathMask           = 7,
	avsst_cAskObjectAction          = 8,
	avsst_cAskObjectPassword        = 9,
	avsst_cAskProcessTreats         = 10,
	avsst_cAskQBAction              = 12,
	avsst_cThreatsInfo              = 13,
	avsst_cScanTreatsInfo           = 14,
	avsst_cDetectExclude            = 15,
	avsst_cProcessCancelRequest     = 16,
	avsst_cScanObject               = 17,
	avsst_cScanObjectStatistics     = 18,
	avsst_cProtectionStatisticsEx   = 19,
	avsst_cDetectNotifyInfo         = 20,
	avsst_cAVSUpdateInfo            = 21,
};

#define pmc_PROTECTION_EVENTS_SPAM         pmc_EVENTS_NOTIFY
#define pmc_PROTECTION_EVENTS_NOTIFY       pmc_EVENTS_NOTIFY
#define pmc_PROTECTION_EVENTS_IMPORTANT    pmc_EVENTS_IMPORTANT
#define pmc_PROTECTION_EVENTS_FIX          pmc_EVENTS_IMPORTANT
#define pmc_PROTECTION_EVENTS_CRITICAL     pmc_EVENTS_CRITICAL

//-----------------------------------------------------------------------------

enum enEngineType
{
	ENGINE_UNKNOWN = 0x00000000,
	ENGINE_DEFAULT = 0x00000001,
	ENGINE_AVP3    = 0x00000002,
	ENGINE_KLAV    = 0x00000004,
	ENGINE_EMUL    = 0x00000008,
	ENGINE_STREAM  = 0x00000010,
};

enum enChunkType
{
	CHUNK_TYPE_FIRST      = 0x1,
	CHUNK_TYPE_MIDDLE     = 0x2,
	CHUNK_TYPE_LAST       = 0x3,
};

enum enStreamFormat
{
	STREAM_FORMAT_RAW     = 0x0,
	STREAM_FORMAT_GZIP    = 0x1,
};

enum enObjectOrigin
{
	OBJECT_ORIGIN_UNKNOWN = -1,
	OBJECT_ORIGIN_MAIL    = 0x0,
	OBJECT_ORIGIN_SCRIPT  = 0x1,
	OBJECT_ORIGIN_GENERIC = 0x2,
};

enum enObjectType
{
	OBJTYPE_GENERICIO            = OID_GENERIC_IO,
	OBJTYPE_LOGICALDRIVE         = OID_LOGICAL_DRIVE,
	OBJTYPE_PHYSICALDISK         = OID_PHYSICAL_DISK,
	OBJTYPE_AVP3DOSMEMORY        = OID_AVP3_DOS_MEMORY,
	OBJTYPE_AVP3BOOT             = OID_AVP3_BOOT,
	OBJTYPE_AVP3MBR              = OID_AVP3_MBR,
	OBJTYPE_MEMORYPROCESS        = OID_MEMORY_PROCESS,
	OBJTYPE_MEMORYPROCESSMODULE  = OID_MEMORY_PROCESS_MODULE,
	OBJTYPE_VOLNURABILITY        = 0x00010000,
	OBJTYPE_STARTUPOBJECT        = 0x00010001,
	OBJTYPE_MAILMSGBODY          = OID_MAIL_MSG_BODY,
	OBJTYPE_MAILMSGATTACH        = OID_MAIL_MSG_ATTACH,
	OBJTYPE_MAILMSGMIME          = OID_MAIL_MSG_MIME,
	OBJTYPE_MAILMSGREF           = OID_MAIL_MSG_REF,
	OBJTYPE_FOLDER_OBJECT        = 0x00010002,
	OBJTYPE_SCRIPT_TEXT          = OID_SCRIPT_TEXT,
	OBJTYPE_NETWORK			     = 0x00010003,
	OBJTYPE_QUARANTINED			 = OID_QUARANTINED_OBJECT,
	OBJTYPE_PHONE_NUMBER         = 0x00010004,
};

enum enObjectStatus
{
	OBJSTATUS_UNKNOWN                    = 0,

	OBJSTATUS_OK                         = 1,  // notify

	OBJSTATUS_INFECTED                   = 2,  // important
	OBJSTATUS_SUSPICION                  = 4,  // important

	OBJSTATUS_ARCHIVE                    = 5,  // spam
	OBJSTATUS_PACKED                     = 6,  // spam
	OBJSTATUS_ENCRYPTED                  = 7,  // spam
	OBJSTATUS_CORRUPTED                  = 8,  // spam
	OBJSTATUS_BACKUPPED                  = 13, // spam
	OBJSTATUS_PASSWORD_PROTECTED         = 19, // notify
	OBJSTATUS_CANTCHANGEATTR	         = 20, // spam
	OBJSTATUS_NOTPROCESSED               = 17, // spam or notify
	OBJSTATUS_ADDEDBYUSER                = 29, // spam

	OBJSTATUS_NOTDISINFECTED             = 10, // critical
	OBJSTATUS_NOTQUARANTINED             = 16, // critical
	OBJSTATUS_NOTBACKUPPED               = 14, // critical
	OBJSTATUS_CANTBEDELETED              = 12, // critical
	OBJSTATUS_PROCESSING_ERROR           = 18, // critical
	OBJSTATUS_DELETE_ON_REBOOT_FAILED	 = 24, // critical
	OBJSTATUS_DISINFECT_ON_REBOOT_FAILED = 26, // critical

	OBJSTATUS_CURED	                     = 28, // fix
	OBJSTATUS_DISINFECTED                = 9,  // fix
	OBJSTATUS_DELETED                    = 11, // fix
	OBJSTATUS_QUARANTINED                = 15, // fix
	OBJSTATUS_OVERWRITED	             = 27, // fix
	OBJSTATUS_ADDEDTOEXCLUDE             = 30, // fix
	OBJSTATUS_DELETED_ON_REBOOT          = 23, // fix
	OBJSTATUS_DISINFECTED_ON_REBOOT      = 25, // fix
	OBJSTATUS_QUARANTINED_ON_REBOOT      = 31, // fix
	OBJSTATUS_RESTORED                   = 32, // fix
	OBJSTATUS_DENIED                     = 33, // fix
	OBJSTATUS_ALLOWED                    = 34, // notify
	OBJSTATUS_RENAMED                    = 35, // fix
    OBJSTATUS_FALSEALARM                 = 36,
    OBJSTATUS_RENAME_FAILED              = 37,
};								         

enum enDetectType
{
	DETYPE_UNKNOWN          = 0,
	DETYPE_VIRWARE          = 1,
	DETYPE_TROJWARE         = 2,
	DETYPE_MALWARE          = 3,
	DETYPE_ADWARE           = 4,
	DETYPE_PORNWARE         = 5,
	DETYPE_RISKWARE         = 6,
	DETYPE_XFILES           = 20,
	DETYPE_SOFTWARE         = 21,
	DETYPE_UNDETECT         = 30,
	DETYPE_ATTACK           = 50,
	DETYPE_REGISTRY         = 51,
	DETYPE_SUSPICACTION     = 52,
	DETYPE_VULNERABILITY    = 60,
	DETYPE_PHISHING         = 70,
};

enum enDetectStatus
{
	DSTATUS_UNKNOWN         = 0,
	DSTATUS_SURE            = 1,
	DSTATUS_PARTIAL         = 2,
	DSTATUS_HEURISTIC       = 3,
	DSTATUS_PROBABLY        = 4,
};

enum enPackerType
{
	DETPACKER_SUSPICIOUS    = 0x1,
	DETPACKER_MULTIPACKED   = 0x2,
	DETPACKER_UNKNOWN       = 0x4,
	DETPACKER_GREY          = 0x8,
	DETPACKER_ALL           = 0xF
};

enum enScanFilter
{
	SCAN_FILTER_ALL_INFECTABLE     = 0x0,
	SCAN_FILTER_ALL_OBJECTS        = 0x1,
	SCAN_FILTER_ALL_EXTENSION      = 0x2
};

enum enScanAction
{
	SCAN_ACTION_REPORT        = 0x0,
	SCAN_ACTION_ASKUSER       = 0x1,
	SCAN_ACTION_DETECT        = 0x2,
	SCAN_ACTION_POSTPONE      = 0x3,
	SCAN_ACTION_DISINFECT     = 0x4,
	SCAN_ACTION_ASKDETECT     = 0x5,
};

enum enOkReason
{
	OKREASON_NONE                = 0x1,
	OKREASON_ISFDB               = 0x2,
	OKREASON_ISTREAMS            = 0x3,
};

enum enDetectReason
{
	DTREASON_NONE                = 0x1,
	DTREASON_BYHASH              = 0x2,
	DTREASON_INFORMATION         = 0x3,
};

enum enNotCuredReason
{
	NCREASON_NONE                = 0x1,
	NCREASON_NONCURABLE          = 0x2,
	NCREASON_LOCKED              = 0x3,
	NCREASON_REPONLY             = 0x4,
	NCREASON_NORIGHTS            = 0x5,
	NCREASON_CANCELLED           = 0x6,
	NCREASON_WRITEPROTECT        = 0x7,
	NCREASON_TASKSTOPPED         = 0x8,
	NCREASON_POSTPONED           = 0x9,
	NCREASON_NONOVERWRITABLE     = 0xA,
	NCREASON_COPYFAILED          = 0xC,
	NCREASON_WRITEERROR          = 0xD,
	NCREASON_OUTOFSPACE          = 0xE,
	NCREASON_READERROR           = 0xF,
	NCREASON_DEVICENOTREADY      = 0x10,
	NCREASON_OBJECTNOTFOUND      = 0x11,
	NCREASON_WRITENOTSUPPORTED   = 0x12,
	NCREASON_CANNOTBACKUP        = 0x13,
};

enum enNotProcessedReason
{
	NPREASON_SIZE                = 0x1,
	NPREASON_TYPE                = 0x2,
	NPREASON_MASKS               = 0x3,
	NPREASON_TIME                = 0x4,
	NPREASON_NORIGHTS            = 0x5,
	NPREASON_NOTFOUND            = 0x6,
	NPREASON_SKIPPED             = 0x7,
	NPREASON_BUDDY               = 0x8,
	NPREASON_LOCKED              = 0x9,
};

//-----------------------------------------------------------------------------

struct cObjectPathMask : public cSerializable
{
	cObjectPathMask() : m_bRecurse(cTRUE){}
	cObjectPathMask(const cStringObj& pMask) : m_strMask(pMask), m_bRecurse(cTRUE){}
	
	DECLARE_IID( cObjectPathMask, cSerializable, PID_AVS, avsst_cObjectPathMask );

	cStringObj          m_strMask;
	tBOOL               m_bRecurse;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cObjectPathMask, 0 )
	SER_VALUE( m_strMask,         tid_STRING_OBJ, "Mask" )
	SER_VALUE( m_bRecurse,        tid_BOOL,       "Recurse" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cDetectExclude : public cSerializable
{
	enum Triggers
	{
		fObjectMask      = 0x01,
		fVerdictMask     = 0x02,
		fTaskList        = 0x04,
		fVerdictPath = 0x08,
	};
	
	cDetectExclude() : m_bEnable(cTRUE), m_nTriggers(fObjectMask){}

	DECLARE_IID( cDetectExclude, cSerializable, PID_AVS, avsst_cDetectExclude );

	bool IsForTask(const cStringObj& sTask) { if( !(m_nTriggers & fTaskList) ) return true; return m_aTaskList.find(sTask) != (tUINT)-1; }
	bool IsObjectMask()  { return (m_nTriggers & fObjectMask) && !m_Object.m_strMask.empty(); }
	bool IsVerdictMask() { return (m_nTriggers & fVerdictMask) && !m_strVerdictMask.empty(); }
	bool IsVerdictPath() { return (m_nTriggers & fVerdictPath) && !m_VerdictPath.m_strMask.empty(); }

	tBOOL               m_bEnable;
	tDWORD              m_nTriggers;
	cStrObj             m_strDescr;
	cObjectPathMask     m_Object;
	cStrObj             m_strVerdictMask;
	cObjectPathMask     m_VerdictPath;
	cStringObjVector    m_aTaskList;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDetectExclude, 0 )
	SER_VALUE( m_bEnable,            tid_BOOL,              "Enable" )
	SER_VALUE( m_nTriggers,          tid_DWORD,             "Triggers" )
	SER_VALUE( m_strDescr,           tid_STRING_OBJ,        "Description" )
	SER_VALUE( m_Object,             cObjectPathMask::eIID, "Object" )
	SER_VALUE( m_strVerdictMask,     tid_STRING_OBJ,        "VerdictMask" )
	SER_VALUE( m_VerdictPath,        cObjectPathMask::eIID, "VerdictPath" )
	SER_VECTOR( m_aTaskList,         tid_STRING_OBJ,        "TaskList" )
IMPLEMENT_SERIALIZABLE_END();

typedef cVector<cEnabledStrItem> tDetectMasksList;

//-----------------------------------------------------------------------------
// cScanProcessInfo structure

struct cDetectObjectInfo;

struct cScanProcessInfo : public cSerializable
{
	cScanProcessInfo() :
		m_nProcessStatusMask(0),
		m_nActionSessionId(-1),
		m_nReportSessionId(0),
		m_nObjectScopeId(-1),
		m_nActionPID(0),
		m_hSessionObject(NULL),
		m_hICheckObject(NULL),
		m_pUserCtx(NULL),
		m_pOwnerCtx(NULL),
		m_pCtx(NULL),
		m_eStreamFormat(STREAM_FORMAT_RAW),
		m_fnAsyncResult(NULL),
		m_pAsyncCtx(NULL),
		m_nProcessId(0),
		m_nSecurityRating(-1) // undef
	{}

	DECLARE_IID( cScanProcessInfo, cSerializable, PID_AVS, avsst_cScanProcessInfo );

	long				m_nProcessStatusMask;
	long				m_nActionSessionId;
	long				m_nActionPID;
	long				m_nReportSessionId;
	long				m_nObjectScopeId;
	long				m_nProcessId; // for ProcessCancel
	long				m_nSecurityRating;

	hOBJECT				m_hSessionObject;
	hOBJECT				m_hICheckObject;
	tPTR                m_pCtx;  // internal (for streaming processesing)
	enStreamFormat      m_eStreamFormat;

	cUserInfo*          m_pUserCtx;
	cUserInfo*          m_pOwnerCtx;

	void ( *m_fnAsyncResult )(cObj* pObj, cScanProcessInfo* pPInfo, cDetectObjectInfo* pDInfo);
	tPTR                m_pAsyncCtx;

	enum StatusFlags
	{
		UNKNOWN			= 0x000,
		DETECTED		= 0x001,
		DETECTED_SURE	= 0x002,
		DELETED			= 0x004,
		DISINFECTED		= 0x008,
		SOME_SKIPPED	= 0x010,
		SOME_CHANGES	= 0x020,
		SFDB_KNOWN		= 0x040,
		NTFS_KNOWN		= 0x080,
		PRE_PROCESSED	= 0x100,
		CANCELED	    = 0x200,
		UNCHANGED	    = 0x400,
		EXTRENAL_DETECT	= 0x800,
		SOME_ALLOWED    = 0x1000,
	};
};

//-----------------------------------------------------------------------------

struct cScanObject : public cSerializable
{
	cScanObject(tDWORD nObjType = 0, tVOID * strObjName = NULL, tCODEPAGE cp = cCP_UNICODE) :
		m_nObjType(nObjType),
		m_bRecursive(true),
		m_bEnabled(true),
		m_bSystem(true)
	{
		if( strObjName )
			m_strObjName.assign(strObjName, cp);
	}

	bool operator == (const cScanObject& c) const {
		return m_nObjType == c.m_nObjType && m_strObjName == c.m_strObjName;
	}

	DECLARE_IID( cScanObject, cSerializable, PID_AVS, avsst_cScanObject );

	tDWORD          m_nObjType;  
	cStringObj      m_strObjName;
	tBOOL           m_bRecursive;  // recursive scanning
	tBOOL           m_bEnabled;    // scan or not this object
	tBOOL           m_bSystem;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScanObject, 0 )
	SER_VALUE( m_bEnabled,         tid_BOOL, "Enabled" )
	SER_VALUE( m_bRecursive,       tid_BOOL, "Recursive" )
	SER_VALUE( m_bSystem,          tid_BOOL, "System" )
	SER_VALUE( m_nObjType,         tid_DWORD, "ObjectType" )
	SER_VALUE( m_strObjName,       tid_STRING_OBJ, "ScanObjects" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cScanSettings structure

struct cScanSettings : public cTaskSettings
{
	cScanSettings() :
		m_nScanFilter(SCAN_FILTER_ALL_OBJECTS),
		m_bScanPacked(true),
		m_bScanArchived(true),
		m_bScanSFXArchived(true),
		m_bScanMailBases(true),
		m_bScanPlainMail(true),
		m_bScanOLE(true),
		m_bScanThreats(true),
		m_bScanVulnerability(false)
		{}

	DECLARE_IID( cScanSettings, cTaskSettings, PID_AVS, SER_SETTINGS_ID+2 );

	enScanFilter           m_nScanFilter;
	tBOOL                  m_bScanPacked;
	tBOOL                  m_bScanArchived;
	tBOOL                  m_bScanSFXArchived;
	tBOOL                  m_bScanMailBases;
	tBOOL                  m_bScanPlainMail;
	tBOOL                  m_bScanOLE;
	tBOOL                  m_bScanThreats;
	tBOOL                  m_bScanVulnerability;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScanSettings, 0 )
	SER_BASE( cTaskSettings,  0 )
	SER_VALUE( m_nScanFilter,      tid_DWORD,      "ScanFilter" )
	SER_VALUE( m_bScanPacked,      tid_BOOL,       "ScanPacked" )
	SER_VALUE( m_bScanArchived,    tid_BOOL,       "ScanArchived" )
	SER_VALUE( m_bScanSFXArchived, tid_BOOL,       "ScanSFXArchived" )
	SER_VALUE( m_bScanMailBases,   tid_BOOL,       "ScanMailBases" )
	SER_VALUE( m_bScanPlainMail,   tid_BOOL,       "ScanPlainMail" )
	SER_VALUE( m_bScanOLE,         tid_BOOL,       "ScanOLE" )
	SER_VALUE( m_bScanThreats,     tid_BOOL,       "ScanThreats" )
	SER_VALUE( m_bScanVulnerability,tid_BOOL,      "ScanVulnerability" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
// cProtectionSettings structure

struct cProtectionSettings : public cScanSettings
{
	cProtectionSettings() :
		m_nScanAction(SCAN_ACTION_DISINFECT),
		m_nDetectMask(DETDANGER_ALL_MASK),
		m_nAskActions(ACTION_ALL),
		m_bUseIChecker(true),
		m_bUseIStreams(true),
		m_bUseMSVerify(false),
		m_bUseExcludes(true),
		m_bTryDisinfect(true),
		m_bTryDelete(true),
		m_bTryDeleteContainer(false),
		m_bTryCureOnReboot(true),
		m_bAskPassword(false),
		m_bRegisterThreats(true),
		m_nSizeLimit(100, cFALSE),
		m_nTimeLimit(1, cFALSE),
		m_bExcludeByMask(false),
		m_bIncludeByMask(false),
		m_nDetectByMaskAction(ACTION_UNKNOWN),
		m_nTimeoutForCancelRequest(0),
		m_bActiveDisinfection(cFALSE),
		m_bScanUnchangedObjects(cTRUE),
		m_nMandatoryScanPeriod(-1),
		m_nABSNumber(0),
		m_bEmulEnable(0),
		m_nEmulValue(0),
		m_bPartlyDetect(false)
		{}

	DECLARE_IID( cProtectionSettings, cScanSettings, PID_AVS, SER_SETTINGS_ID+1 );

	enScanAction             m_nScanAction;
	tDWORD                   m_nDetectMask;
	tDWORD                   m_nAskActions;
					         
	tBOOL                    m_bUseIChecker;
	tBOOL                    m_bUseIStreams;
	tBOOL                    m_bUseMSVerify;
	tBOOL                    m_bUseExcludes;
	tDWORD                   m_nABSNumber;
					         
	tBOOL                    m_bTryDisinfect;
	tBOOL                    m_bTryDelete;
	tBOOL                    m_bTryDeleteContainer;
	tBOOL                    m_bTryCureOnReboot;
	tBOOL                    m_bAskPassword;
	tBOOL                    m_bRegisterThreats;
					         
	cSettingsCheckT<long>    m_nSizeLimit;
	cSettingsCheckT<long>    m_nTimeLimit;
					         
	tBOOL                    m_bExcludeByMask;
	cVector<cObjectPathMask> m_aExcludeList;
					         
	tBOOL                    m_bIncludeByMask;
	cVector<cObjectPathMask> m_aIncludeList;
						     
	enActions                m_nDetectByMaskAction;
	tDetectMasksList         m_aDetectMasksList;

	tDWORD                   m_nTimeoutForCancelRequest;
	tBOOL                    m_bActiveDisinfection;
						    
	tBOOL                    m_bScanUnchangedObjects;
	cSerObj<cScanSettings>   m_pForUnchangedObjects;
	tDWORD                   m_nMandatoryScanPeriod;

	tBOOL                    m_bEmulEnable;
	tDWORD                   m_nEmulValue;
	tBOOL                    m_bPartlyDetect;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProtectionSettings, 0 )
	SER_BASE( cScanSettings,  0 )
	SER_VALUE( m_nScanAction,              tid_DWORD,             "ScanAction" )
	SER_VALUE( m_nDetectMask,              tid_DWORD,             "DetectMask" )
	SER_VALUE( m_nAskActions,              tid_DWORD,             "AskActions" )
	SER_VALUE( m_bUseIChecker,             tid_BOOL,              "UseIChecker" )
	SER_VALUE( m_bUseMSVerify,             tid_BOOL,              "UseMSVerify" )
	SER_VALUE( m_bUseIStreams,             tid_BOOL,              "UseIStreams" )
	SER_VALUE( m_bUseExcludes,             tid_BOOL,              "UseExcludes" )
	SER_VALUE( m_nABSNumber,               tid_DWORD,             "ABSNumber" )
	SER_VALUE( m_bTryDisinfect,            tid_BOOL,              "TryDisinfect" )
	SER_VALUE( m_bTryDelete,               tid_BOOL,              "TryDelete" )
	SER_VALUE( m_bTryDeleteContainer,      tid_BOOL,              "TryDeleteContainer" )
	SER_VALUE( m_bTryCureOnReboot,         tid_BOOL,              "TryCureOnReboot" )
	SER_VALUE( m_bAskPassword,             tid_BOOL,              "AskPassword" )
	SER_VALUE( m_bRegisterThreats,         tid_BOOL,              "RegisterThreats" )
	SER_VALUE( m_nSizeLimit.m_on,          tid_BOOL,              "UseSizeLimit" )
	SER_VALUE( m_nSizeLimit.m_val,         tid_DWORD,             "SizeLimit" )
	SER_VALUE( m_nTimeLimit.m_on,          tid_BOOL,              "UseTimeLimit" )
	SER_VALUE( m_nTimeLimit.m_val,         tid_DWORD,             "TimeLimit" )
	SER_VALUE( m_bExcludeByMask,           tid_BOOL,              "ExcludeByMask" )
	SER_VECTOR( m_aExcludeList,            cObjectPathMask::eIID, "ExcludeList" )
	SER_VALUE( m_bIncludeByMask,           tid_BOOL,              "IncludeByMask" )
	SER_VECTOR( m_aIncludeList,            cObjectPathMask::eIID, "IncludeList" )
	SER_VALUE( m_nDetectByMaskAction,      tid_DWORD,             "DetectByMaskAction" )
	SER_VECTOR( m_aDetectMasksList,        cEnabledStrItem::eIID, "DetectMasksList" )
	SER_VALUE( m_nTimeoutForCancelRequest, tid_DWORD,             "TimeoutForCancelRequest" )
	SER_VALUE( m_bActiveDisinfection,      tid_BOOL,              "ActiveDisinfection" )
	SER_VALUE( m_bScanUnchangedObjects,    tid_BOOL,              "ScanUnchangedObjects" )
	SER_VALUE_PTR( m_pForUnchangedObjects, cScanSettings::eIID,   "ForUnchangedObjects" )
	SER_VALUE( m_nMandatoryScanPeriod,     tid_DWORD,             "MandatoryScanPeriod" )
	SER_VALUE( m_bEmulEnable,              tid_BOOL,              "EmulEnable" )
	SER_VALUE( m_nEmulValue,               tid_DWORD,             "EmulValue" )
	SER_VALUE( m_bPartlyDetect,            tid_BOOL,              "PartlyDetect" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
// cProtectionStatistics structure

struct cProtectionStatistics : public cTaskStatistics
{
	cProtectionStatistics(){ init(); }

	void init(){ memset(&m_nNumScaned, 0, (tBYTE*)&m_strCurObject - (tBYTE*)&m_nNumScaned); }

	void operator += (const cProtectionStatistics& c)
	{
		long* pStat1 = &m_nNumScaned; const long* pStat2 = &c.m_nNumScaned;
		for(; pStat1 != (long*)&m_nCurCancelId; pStat1++, pStat2++)
			(*pStat1) += (*pStat2);
		if( m_tmCurObject < c.m_tmCurObject )
		{
			m_strCurObject = c.m_strCurObject;
			m_tmCurObject = c.m_tmCurObject;
		}
	}

	DECLARE_IID_STATISTICS_EX( cProtectionStatistics, cTaskStatistics, PID_AVS, 2 );

	tLONG   m_nNumScaned;
	tLONG   m_nNumDetected;
	tLONG   m_nNumDetectedExact;
	tLONG   m_nNumDetectedSusp;
	tLONG   m_nNumThreats;
	tLONG   m_nNumUntreated;
	tLONG   m_nNumDisinfected;
	tLONG   m_nNumQuarantined;
	tLONG   m_nNumDeleted;
	tLONG   m_nNumAbandoned;
	tLONG   m_nNumNotProcessed;
	tLONG   m_nNumArchived;
	tLONG   m_nNumPacked;
	tLONG   m_nNumPswProtected;
	tLONG   m_nNumCorrupted;
	tLONG   m_nNumScanErrors;
	tLONG   m_nTotalSleepTime;

	tDWORD  m_nCurCancelId;
	tBOOL   m_bSomeUntreated;
	tDWORD  m_tmCurObject;
	cStringObj m_strCurObject;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProtectionStatistics, 0 )
	SER_BASE( cTaskStatistics, 0 )
	SER_VALUE( m_nNumScaned,       tid_DWORD, "NumScaned" )
	SER_VALUE( m_nNumDetected,     tid_DWORD, "NumDetected" )
	SER_VALUE( m_nNumDetectedExact,tid_DWORD, "NumDetectedExact" )
	SER_VALUE( m_nNumDetectedSusp, tid_DWORD, "NumDetectedSusp" )
	SER_VALUE( m_nNumThreats,      tid_DWORD, "NumThreats" )
	SER_VALUE( m_nNumUntreated,    tid_DWORD, "NumUntreated" )
	SER_VALUE( m_nNumDisinfected,  tid_DWORD, "NumDisinfected" )
	SER_VALUE( m_nNumQuarantined,  tid_DWORD, "NumQuarantined" )
	SER_VALUE( m_nNumDeleted,      tid_DWORD, "NumDeleted" )
	SER_VALUE( m_nNumAbandoned,    tid_DWORD, "NumAbandoned" )
	SER_VALUE( m_nNumArchived,     tid_DWORD, "NumArchived" )
	SER_VALUE( m_nNumPacked,       tid_DWORD, "NumPacked" )
	SER_VALUE( m_nNumPswProtected, tid_DWORD, "NumPswProtected" )
	SER_VALUE( m_nNumCorrupted,    tid_DWORD, "NumCorrupted" )
	SER_VALUE( m_nNumScanErrors,   tid_DWORD, "NumScanErrors" )
	SER_VALUE( m_nTotalSleepTime,  tid_DWORD, "TotalSleepTime" )
	SER_VALUE( m_nCurCancelId,     tid_DWORD, "CurCancelId" )
	SER_VALUE( m_tmCurObject,      tid_DWORD, NULL )
	SER_VALUE( m_strCurObject,     tid_STRING_OBJ,	"CurObject" )
	SER_VALUE( m_bSomeUntreated,   tid_BOOL,  "SomeUntreated" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cScanObjectStatistics : public cProtectionStatistics
{
	cScanObjectStatistics() :
		m_nScopeId(-1)
	{}

	DECLARE_IID( cScanObjectStatistics, cProtectionStatistics, PID_AVS, avsst_cScanObjectStatistics );

	cScanObject   m_pScanObj;
	tDWORD        m_nScopeId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScanObjectStatistics, 0 )
	SER_BASE( cProtectionStatistics, 0 )
	SER_VALUE( m_pScanObj,          cScanObject::eIID, 0 )
	SER_VALUE( m_nScopeId,          tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// cProtectionStatisticsEx structure

struct cProtectionStatisticsEx : public cProtectionStatistics
{
	DECLARE_IID( cProtectionStatisticsEx, cProtectionStatistics, PID_AVS, avsst_cProtectionStatisticsEx );

	cVector<cScanObjectStatistics> m_aObjStat;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProtectionStatisticsEx, 0 )
	SER_BASE( cProtectionStatistics, 0 )
	SER_VECTOR( m_aObjStat,  cScanObjectStatistics::eIID, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
// Object info

struct cObjectInfo : public cTaskHeader
{
    cObjectInfo() :
		m_strObjectName(),
		m_nObjectOrigin(OBJECT_ORIGIN_GENERIC),
		m_nObjectType(OBJTYPE_GENERICIO),
		m_nObjectStatus(OBJSTATUS_OK),
		m_nObjectScope(-1)
	{}

	DECLARE_IID( cObjectInfo, cTaskHeader, PID_AVS, avsst_cObjectInfo );

	enObjectOrigin    m_nObjectOrigin;
	enObjectType      m_nObjectType;
	enObjectStatus    m_nObjectStatus;
	tDWORD            m_nObjectScope;
	cStringObj        m_strObjectName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cObjectInfo, 0 )
	SER_BASE(  cTaskHeader, 0 )
	SER_VALUE( m_nObjectOrigin,   tid_DWORD, "ObjectOrigin" )
	SER_VALUE( m_nObjectType,     tid_DWORD, "ObjectType" )
	SER_VALUE( m_nObjectStatus,   tid_DWORD, "ObjectStatus" )
	SER_VALUE( m_strObjectName,   tid_STRING_OBJ, "ObjectName" )
	SER_VALUE( m_nObjectScope,    tid_DWORD, NULL )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
// Infected object info

struct cDetectObjectInfo : public cObjectInfo
{
    cDetectObjectInfo() :
		m_nDetectType(DETYPE_UNKNOWN),
		m_nDetectStatus(DSTATUS_UNKNOWN),
		m_nDetectDanger(DETDANGER_UNKNOWN),
		m_nDetectBehaviour(0),
		m_nDescription(0),
		m_tmTimeStamp(0),
		m_tmTimeDelta(0)
	{}

	DECLARE_IID( cDetectObjectInfo, cObjectInfo, PID_AVS, avsst_cDetectObjectInfo );

	cStringObj        m_strDetectName;
	enDetectType      m_nDetectType;
	enDetectStatus    m_nDetectStatus;
	enDetectDanger    m_nDetectDanger;
	long              m_nDetectBehaviour;
	long              m_nDescription;

	cStringObj        m_strUserName;
	cStringObj        m_strMachineName;
	__time32_t        m_tmTimeStamp;
	tDWORD            m_tmTimeDelta;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDetectObjectInfo, 0 )
	SER_BASE(  cObjectInfo, 0 )
	SER_VALUE( m_strDetectName,      tid_STRING_OBJ, "DetectName" )
	SER_VALUE( m_nDetectType,        tid_DWORD,      "DetectType" )
	SER_VALUE( m_nDetectStatus,      tid_DWORD,      "DetectStatus" )
	SER_VALUE( m_nDetectDanger,      tid_DWORD,      "DetectDanger" )
	SER_VALUE( m_nDescription,       tid_DWORD,      "Description" )
	SER_VALUE( m_tmTimeStamp,        tid_DWORD,      "ReportTime" )
	SER_VALUE( m_strUserName,        tid_STRING_OBJ, "UserName" )
	SER_VALUE( m_strMachineName,     tid_STRING_OBJ, "MachineName" )
	SER_VALUE( m_nDetectBehaviour,   tid_DWORD,      "DetectBehaviour" )
	SER_VALUE( m_tmTimeDelta,        tid_DWORD,      "TimeDelta" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cDetectNotifyInfo : public cDetectObjectInfo
{
	DECLARE_IID( cDetectNotifyInfo, cDetectObjectInfo, PID_AVS, avsst_cDetectNotifyInfo );

	cStringObj        m_strTemplate;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cDetectNotifyInfo, 0 )
	SER_BASE(  cDetectObjectInfo, 0 )
	SER_VALUE( m_strTemplate,    tid_STRING_OBJ, NULL )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cInfectedObjectInfo : public cDetectObjectInfo
{
    cInfectedObjectInfo() :
		m_nNonCuredReason(NCREASON_NONE),
		m_qwUniqueId(0),
		m_qwParentId(0),
		m_tmScanningBases(-1),
		m_qwQBObjectId(0)
	{}

	DECLARE_IID( cInfectedObjectInfo, cDetectObjectInfo, PID_AVS, avsst_cInfectedObjectInfo );

	enNotCuredReason  m_nNonCuredReason;
	cStringObj   	  m_strDetectObject;
	cStringObj   	  m_strArchiveObject;

	tQWORD            m_qwUniqueId;
	tQWORD            m_qwParentId;

	cCharVector       m_pReopenData;
	tDWORD            m_tmScanningBases;
	tQWORD            m_qwQBObjectId;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cInfectedObjectInfo, 0 )
	SER_BASE(  cDetectObjectInfo, 0 )
	SER_VALUE( m_nNonCuredReason,  tid_DWORD, "NonCuredReason" )
	SER_VALUE( m_strArchiveObject, tid_STRING_OBJ, "ArchiveObject" )
	SER_VALUE( m_strDetectObject,  tid_STRING_OBJ, "DetectObject" )
	SER_VALUE( m_qwUniqueId,       tid_QWORD, 0 )
	SER_VALUE( m_qwParentId,       tid_QWORD, 0 )
	SER_VECTOR( m_pReopenData,     tid_CHAR, 0 )
	SER_VALUE( m_tmScanningBases,  tid_DWORD, "ScanningBasesTime" )
	SER_VALUE( m_qwQBObjectId,     tid_QWORD, NULL )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cProcessCancelRequest : public cTaskHeader
{
	cProcessCancelRequest() :
		m_nProcessId(0)
	{}

	DECLARE_IID( cProcessCancelRequest, cTaskHeader, PID_AVS, avsst_cProcessCancelRequest );

	enum Actions { BEGIN = eIID, END };

	tDWORD				m_nProcessId;
	tDWORD              m_nTaskId;
	cStringObj          m_strObjectName;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cProcessCancelRequest, 0 )
	SER_BASE( cTaskHeader,  0 )
	SER_VALUE( m_nProcessId,       tid_DWORD,      0 )
	SER_VALUE( m_strObjectName,    tid_STRING_OBJ, "ObjectName" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cThreatsInfo : public cSerializable
{
	cThreatsInfo() :
		m_nThreats(0),
		m_nUntreated(0),
		m_tmStamp(0)
	{}

	DECLARE_IID( cThreatsInfo, cSerializable, PID_AVS, avsst_cThreatsInfo );

	tDWORD         m_nThreats;
	tDWORD         m_nUntreated;
	tDWORD         m_tmStamp;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cThreatsInfo, 0 )
	SER_VALUE( m_nThreats,      tid_DWORD, "NumThreats" )
	SER_VALUE( m_nUntreated,    tid_DWORD, "NumUntreated" )
	SER_VALUE( m_tmStamp,       tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cAskProcessTreats : public cTaskHeader
{
	cAskProcessTreats() :
		m_nAction(SCAN),
		m_bByIndex(cTRUE),
		m_bSyncScan(cFALSE),
		m_bAskPassword(cTRUE)
	{}

	enum Actions { SCAN, VERIFY, DISCARD, DELETE_, RESTORE };

	DECLARE_IID( cAskProcessTreats, cTaskHeader, PID_AVS, avsst_cAskProcessTreats );

	Actions        m_nAction;
	tBOOL          m_bByIndex;
	tBOOL          m_bSyncScan;
	tBOOL          m_bAskPassword;
	cDwordVector   m_aScanRecords;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskProcessTreats, 0 )
	SER_BASE( cTaskHeader, 0 )
	SER_VALUE( m_nAction,        tid_DWORD, 0 )
	SER_VALUE( m_bByIndex,       tid_BOOL, 0 )
	SER_VALUE( m_bAskPassword,   tid_BOOL, 0 )
	SER_VECTOR( m_aScanRecords,  tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cScanTreatsInfo : public cSerializable
{
	cScanTreatsInfo() :
		m_bCompleted(cTRUE),
		m_nProgress(0),
		m_nCurrentThreat(0)
	{}

	DECLARE_IID( cScanTreatsInfo, cSerializable, PID_AVS, avsst_cScanTreatsInfo );

	tBOOL          m_bCompleted;
	tDWORD         m_nProgress;
	tDWORD         m_nCurrentThreat;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cScanTreatsInfo, 0 )
	SER_VALUE( m_bCompleted,     tid_BOOL, 0 )
	SER_VALUE( m_nProgress,      tid_DWORD, 0 )
	SER_VALUE( m_nCurrentThreat, tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
// ask-actions

struct cAskObjectAction : public cInfectedObjectInfo
{
	cAskObjectAction() :
		m_nDefaultAction(ACTION_REPORTONLY),
		m_nExcludeAction(ACTION_UNKNOWN),
		m_nActionsAll(0),
		m_nActionsMask(0),
		m_nResultAction(ACTION_REPORTONLY),
		m_nApplyToAll(APPLYTOALL_FALSE),
		m_bSystemCritical(cTRUE),
		m_bSound(cTRUE),
		m_nActionId(0)
	{}

	DECLARE_IID( cAskObjectAction, cInfectedObjectInfo, PID_AVS, avsst_cAskObjectAction );

	enum Actions { DISINFECT = eIID, QUARANTINE, BACKUP, RESTORE, ACTIVE_DETECT, ACTIVE_DISINFECT };

	tActionId         m_nActionId;
	enActions	      m_nDefaultAction;
	enActions         m_nExcludeAction;
	int			      m_nActionsAll;
	int			      m_nActionsMask;
	enActions         m_nResultAction;
	enApplyToAll      m_nApplyToAll;
	tBOOL             m_bSystemCritical;
	tBOOL             m_bSound;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskObjectAction, 0 )
	SER_BASE( cInfectedObjectInfo, 0 )
	SER_VALUE( m_nDefaultAction,   tid_DWORD, "DefaultAction" )
	SER_VALUE( m_nExcludeAction,   tid_DWORD, "ExcludeAction" )
	SER_VALUE( m_nActionsMask,     tid_DWORD, "ActionsMask" )
	SER_VALUE( m_nActionsAll,      tid_DWORD, "ActionsAll" )
	SER_VALUE( m_nResultAction,    tid_DWORD, "ResultAction" )
	SER_VALUE( m_nApplyToAll,      tid_DWORD, "ApplyToAll" )
	SER_VALUE( m_bSystemCritical,  tid_BOOL,  NULL )
	SER_VALUE( m_bSound,           tid_BOOL,  NULL )
	SER_VALUE( m_nActionId,        tid_DWORD, NULL )
	SER_VALUE( m_qwQBObjectId,     tid_QWORD, NULL )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cAskObjectPassword : public cAskObjectAction
{
	cAskObjectPassword(){}

	DECLARE_IID( cAskObjectPassword, cAskObjectAction, PID_AVS, avsst_cAskObjectPassword );

// output
	cStringObj     m_strPassword;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskObjectPassword, 0 )
	SER_BASE( cAskObjectAction, 0 )
	SER_VALUE( m_strPassword,    tid_STRING_OBJ, "Password" )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------

struct cAskQBAction : public cAskObjectAction
{
	cAskQBAction() : 
		m_hSource(NULL)
	{}

	DECLARE_IID( cAskQBAction, cAskObjectAction, PID_AVS, avsst_cAskQBAction );

	enum Actions{ QUARANTINE = eIID, BACKUP };

	hIO                   m_hSource;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAskQBAction, 0 )
	SER_BASE( cAskObjectAction, 0 )
//	SER_VALUE( m_hSource, tid_OBJECT, 0 )
IMPLEMENT_SERIALIZABLE_END()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// cAVSSettings structure

struct cAVSSettings : public cTaskSettings
{
	cAVSSettings() :
		cTaskSettings(),
		m_nDetectMask(DETDANGER_ALL_MASK),
		m_bMultiThreaded(cTRUE),
		m_bDebugMode(cFALSE),
		m_bUseActiveDisinfection(cTRUE),
		m_bUseABS(cTRUE),
		m_nStackSizeToCheck(0),
		m_nPackerTypeMask(DETPACKER_ALL),
		m_bSafeScan(cTRUE)
	{
		memset(m_nDetectTypeMask, 0xff, sizeof(m_nDetectTypeMask));
	}

	DECLARE_IID_SETTINGS( cAVSSettings, cTaskSettings, PID_AVS );

	cStringObj          m_sBaseFolder;
	cStringObj          m_sDataFolder;
	cStringObj          m_sSetName;
	cStringObj          m_sKlavaBase;
	cStringObj          m_sEmulBase;
	cStringObj          m_sStreamBase;
	tBOOL               m_bMultiThreaded;
	tBOOL               m_bDebugMode;
	tBOOL               m_bUseABS;
	tBOOL               m_bUseActiveDisinfection;
	tBOOL               m_bSafeScan;
	tDWORD              m_nDetectMask;
	tDWORD              m_nDetectTypeMask[6];
	tDWORD              m_nPackerTypeMask;
	tDWORD              m_nStackSizeToCheck;
	cVector<cDetectExclude> m_aExcludes;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVSSettings, 0 )
	SER_BASE( cTaskSettings,  0 )
	SER_VALUE( m_sBaseFolder,        tid_STRING_OBJ, "BaseFolder" )
	SER_VALUE( m_sDataFolder,        tid_STRING_OBJ, "DataFolder" )
	SER_VALUE( m_sSetName,           tid_STRING_OBJ, "SetName" )
	SER_VALUE( m_sKlavaBase,         tid_STRING_OBJ, "KlavaBase" )
	SER_VALUE( m_sEmulBase,          tid_STRING_OBJ, "EmulBase" )
	SER_VALUE( m_sStreamBase,        tid_STRING_OBJ, "StreamBase" )
	SER_VALUE( m_bMultiThreaded,     tid_BOOL,       "MultiThreaded" )
	SER_VALUE( m_bUseABS,            tid_BOOL,       "UseABS" )
	SER_VALUE( m_bUseActiveDisinfection, tid_BOOL,   "UseActiveDisinfection" )
	SER_VECTOR( m_aExcludes,        cDetectExclude::eIID, "Excludes" )
	SER_VALUE( m_bDebugMode,         tid_BOOL,       "DebugMode" )
	SER_VALUE( m_nDetectMask,        tid_DWORD,      "DetectMask" )
	SER_VALUE( m_nDetectTypeMask[0], tid_DWORD,      "DetectTypeMask0" )
	SER_VALUE( m_nDetectTypeMask[1], tid_DWORD,      "DetectTypeMask1" )
	SER_VALUE( m_nDetectTypeMask[2], tid_DWORD,      "DetectTypeMask2" )
	SER_VALUE( m_nDetectTypeMask[3], tid_DWORD,      "DetectTypeMask3" )
	SER_VALUE( m_nDetectTypeMask[4], tid_DWORD,      "DetectTypeMask4" )
	SER_VALUE( m_nDetectTypeMask[5], tid_DWORD,      "DetectTypeMask5" )
	SER_VALUE( m_nPackerTypeMask,    tid_DWORD,      "PackerTypeMask" )
	SER_VALUE( m_bSafeScan,          tid_BOOL,       "SafeScan" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------
// cAVSPerformance structure

struct cAVSPerformance : public cTaskStatistics
{
	cAVSPerformance() :
		cTaskStatistics(),
		m_nCookie(0),
		m_nPerformance(0),
		m_nWorkingTickCount(0),
		m_nObjectOrigin(OBJECT_ORIGIN_GENERIC)
	{}

	DECLARE_IID_STATISTICS_EX( cAVSPerformance, cTaskStatistics, PID_AVS, 1 );

	long                m_nCookie;
	enObjectOrigin      m_nObjectOrigin;
	long                m_nPerformance;
	long                m_nWorkingTickCount;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVSPerformance, 0 )
	SER_BASE( cTaskStatistics, 0 )
	SER_VALUE( m_nCookie,           tid_DWORD, 0 )
	SER_VALUE( m_nObjectOrigin,     tid_DWORD, 0 )
	SER_VALUE( m_nPerformance,      tid_DWORD, 0 )
	SER_VALUE( m_nWorkingTickCount, tid_DWORD, 0 )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

enum enBasesState
{
	bsUnknown    =0x0,
	bsInvalid    =0x1,
	bsCorrupted  =0x2,
	bsLoaded     =0x4,
};

struct cAVSStatistics : public cAVSPerformance
{
	cAVSStatistics() :
		m_nNumRecords(0),
		m_nNumGenericScaned(0),
		m_nNumMailScaned(0),
		m_nNumScriptScaned(0),
		m_nNumThreats(0),
		m_nNumUntreated(0),
		m_tmBasesDate(-1),
		m_errLoadBases(errOK),
		m_nBasesDateDaysOld(-1),
		m_nBasesState(bsUnknown)
	{}

	void operator += (const cAVSStatistics& c)
	{
		m_nNumGenericScaned += c.m_nNumGenericScaned;
		m_nNumMailScaned += c.m_nNumMailScaned;
		m_nNumScriptScaned += c.m_nNumScriptScaned;
		m_nNumThreats += c.m_nNumThreats;
		m_nNumUntreated += c.m_nNumUntreated;
	}

	DECLARE_IID_STATISTICS( cAVSStatistics, cAVSPerformance, PID_AVS );

	tDWORD              m_nBasesState;
	tERROR              m_errLoadBases;
	tDWORD              m_tmBasesDate;
	tDWORD              m_nBasesDateDaysOld;
	tDWORD              m_nNumRecords;
	tDWORD              m_nNumGenericScaned;
	tDWORD              m_nNumMailScaned;
	tDWORD              m_nNumScriptScaned;
	tDWORD              m_nNumThreats;
	tDWORD              m_nNumUntreated;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVSStatistics, 0 )
	SER_BASE( cAVSPerformance,  0 )
	SER_VALUE( m_tmBasesDate,       tid_DWORD, "BasesDate" )
	SER_VALUE( m_nBasesDateDaysOld, tid_DWORD, "BasesDateDaysOld" )
	SER_VALUE( m_nNumRecords,       tid_DWORD, "NumRecords" )
	SER_VALUE( m_nNumGenericScaned, tid_DWORD, "NumGenericScaned" )
	SER_VALUE( m_nNumMailScaned,    tid_DWORD, "NumMailScaned" )
	SER_VALUE( m_nNumScriptScaned,  tid_DWORD, "NumScriptScaned" )
	SER_VALUE( m_nNumThreats,       tid_DWORD, "NumThreats" )
	SER_VALUE( m_nNumUntreated,     tid_DWORD, "NumUntreated" )
	SER_VALUE( m_errLoadBases,      tid_ERROR, "BasesLoadError" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

struct cAVSUpdateInfo : public cSerializable
{
	cAVSUpdateInfo() : m_eEngineMask(0){}

	DECLARE_IID( cAVSUpdateInfo, cSerializable, PID_AVS, avsst_cAVSUpdateInfo );

	tDWORD              m_eEngineMask;
	cStrObj             m_sSuffix;
	cVector<cStrObj>    m_aFiles;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cAVSUpdateInfo, 0 )
	SER_VALUE( m_eEngineMask,       tid_DWORD,      "EngineType" )
	SER_VALUE( m_sSuffix,           tid_STRING_OBJ, "Suffix" )
	SER_VECTOR( m_aFiles,           tid_STRING_OBJ, "Files" )
IMPLEMENT_SERIALIZABLE_END();

//-----------------------------------------------------------------------------

#endif//  __S_AVS_H
