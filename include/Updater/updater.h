#ifndef S_UPDATER_H_INCLUDED_D0D9AB52_1818_401d_8736_FB6EF52858D0
#define S_UPDATER_H_INCLUDED_D0D9AB52_1818_401d_8736_FB6EF52858D0

#include <Prague/prague.h>
#include <Prague/pr_serializable.h>
#include <Prague/pr_cpp.h>
#include <Prague/pr_vector.h>
#include <Prague/iface/i_mutex.h>

#include <ProductCore/structs/s_taskmanager.h>

// updater sends a message class
#define PID_UPDATER  ((tPID)(38016))

 // updater message class
#define pmc_UPDATER 0xeae914af
    // indicates that component is pre-installed and needed to be checked by component
    #define pm_COMPONENT_PREINSTALLED 0x00001100

    // indicates that component is updated successfully
    #define pm_COMPONENT_UPDATED 0x00001101

    // in case successfull update message with all files list is sent by updater
    #define pm_FILE_LIST_ON_SUCCESS_UPDATE 0x00001102

    // notifies recipients about database state changes
    #define pm_DATABASE_STATE_CHANGED 0x00001103

// Updater report message class
#define pmc_UPDATER_REPORT 0x43273c66 //

    #define pm_REP_UPDATE_END 0x00002001 // (8193) --

    #define pm_CORE_EVENT 0x00002003 // (8195) --

// Updater critical report message class
#define pmc_UPDATER_CRITICAL_REPORT 0x87cc73ae //


// TODO: last success update source property (not implemented)
#define REG_LAST_SUCC_URL   "LastSuccUrl"

// TODO: updater already running status code
 // TODO: change to updater KLUPD::CoreError code
#define errTASK_ALREADY_RUNNING     PR_MAKE_IMP_ERR(PID_UPDATER, 0x008) // 0x99480008,-1723334648 (8) --

enum
{
    updater_UpdatedFile = 1,
    // Note: in case updater_UpdateEvent identifier changes, then reports must be adjusted
    updater_UpdateEvent = 2,
    updater_UpdatedFileList = 3,
    updater_UpdatedComponentName = 4,
	updater_FilesToRemove = 5,
    updater_UpdateSource = 6,
    updater_DatabaseState = 7,
};


//////////////////////////////////////////////////////////////////////////
////// hardcoded component list //////

// Updater Component: updater configuration files
#define cUPDATE_CATEGORY_UPDATER        "Updater"
// the signature 6 registry elements
#define cUPDATE_CATEGORY_CORE           "CORE"
// black list
#define cUPDATE_CATEGORY_BLST           "BLST"
// component names localization
#define cUPDATE_CATEGORY_LOCALIZATION   "RT"
// the Anti-Virus component
#define cUPDATE_CATEGORY_AVS            "AVS"
// the News component
#define cUPDATE_CATEGORY_NEWS           "INFO"


#define cUPDATE_CATEGORY_INDEX50        "INDEX50"
#define cUPDATE_CATEGORY_INDEX60        "INDEX60"

// Anti-Hacker component name
#define cUPDATE_CATEGORY_ANTIHACKER_I386 "AHI386"
#define cUPDATE_CATEGORY_ANTIHACKER_X64 "AHX64"
// Anti-Hacker for builds before MP2 (with merged I386 and X64 platforms)
#define cUPDATE_CATEGORY_ANTIHACKER_FOR_OLDER_BUILDS "AH"

// Anti-Phishing component name
#define cUPDATE_CATEGORY_ANTIPHISHING   "AP"
// Anti-Banner component name
#define cUPDATE_CATEGORY_ANTIBANNER     "ADB"
// Anti-Spam component name
#define cUPDATE_CATEGORY_AS             "AS"
// Executable modules
#define cUPDATE_CATEGORY_EXEC           "KAV2006Exec"
// Proactive Defence Module (Registry Monitor)
#define cUPDATE_CATEGORY_PDM_RM         "RM"
// Proactive Defence Module (Behaviour Blocker)
#define cUPDATE_CATEGORY_PDM_BB         "BB"
// Proactive Defence Module (Behaviour Blocker for Vista OS)
#define cUPDATE_CATEGORY_PDM_BB_Vista   "BB2"
// Web Monitoring component
#define cUPDATE_CATEGORY_WebMonitoring  "WAI386"
// Emilator
#define cUPDATE_CATEGORY_Emulator		"EMUI386"
// Parental control
#define cUPDATE_CATEGORY_ParentalControl "PCDB"
// Emilator
#define cUPDATE_CATEGORY_KLAVA		    "KDBI386"

// hardcoded update element types
#define cUPDATE_TYPE_BLACK_LIST         "blst"


// hardcoded  initialization file name for Proactive Defence Module (Registry Monitor)
#define cUPDATE_PDM_INI_FILE            "regmonitor.ini"


// the substitution used to get path to registry files for signature 6 check
#define cUPDATE_LOCAL_PATH_DSKM         "DSKM"

// registry variable name to hold hardcoded common components list to update
#define CommonComponentsToUpdateRegistryKeyName "CommonComponents"

// registry variable name to hold hardcoded additional list of components to distribute
  // there is a difference between enumerated components and components are to be distributed by products,
 // because there is a requirement that KAV distribute for Kaspersky Internet Security and vise versa,
 // and the requirement that File Server product distribute for Workstation and vise versa
#define ComponentsToDistributeRegistryKeyName "ComponentsToDistribute"


struct cUpdaterDatabaseState : public cSerializable
{
    DECLARE_IID(cUpdaterDatabaseState, cSerializable, PID_UPDATER, updater_DatabaseState);

    tBOOL       m_bConsistent;
    tDATETIME   m_Date;
    cStrObj     m_BlackListPath;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUpdaterDatabaseState, "UpdaterDatabaseState")
	SER_VALUE(m_bConsistent,    tid_BOOL,       "Consistency")
    SER_VALUE(m_Date,           tid_DATETIME,   "Date")
    SER_VALUE(m_BlackListPath,  tid_STRING_OBJ, "BlackList")
IMPLEMENT_SERIALIZABLE_END();


// the cUpdaterUpdatedFile structure used for exchange about updated files with product
struct cUpdaterUpdatedFile : public cSerializable
{
    DECLARE_IID(cUpdaterUpdatedFile, cSerializable, PID_UPDATER, updater_UpdatedFile);

    enum ChangeStatus
    {
        unchanged,
        modified,
        added,
    };
    enum Type
    {
        base,
        patch,
        index,
        blackList,
        registry,
        key
    };
    enum ReplaceMode
    {
        ordinary,
        suffix,
    };

    cUpdaterUpdatedFile(const cStrObj &fileName = "", const cStrObj &localPath = "",
                            const ChangeStatus &status = unchanged, const Type &type = base,
                            const ReplaceMode &replaceMode = ordinary)
      : m_filename(fileName),
        m_localPath(localPath),
        m_status(status),
        m_type(type),
        m_replaceMode(replaceMode)
    {
    }

    cStrObj m_filename;
    cStrObj m_localPath;
    cVector<cStrObj> m_componentIdSet;
    ChangeStatus m_status;
    Type m_type;
    ReplaceMode m_replaceMode;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUpdaterUpdatedFile, "UpdaterUpdatedFile")
    SER_VALUE(m_filename,  tid_STRING_OBJ, "filename")
    SER_VALUE(m_localPath,  tid_STRING_OBJ, "localPath")
	SER_VECTOR(m_componentIdSet,   tid_STRING_OBJ, "componentIdSet")
	SER_VALUE(m_status, tid_DWORD, "status")
    SER_VALUE(m_type,  tid_DWORD, "type")
    SER_VALUE(m_replaceMode,  tid_DWORD, "replaceMode")
IMPLEMENT_SERIALIZABLE_END();

// the cUpdaterUpdatedFileList structure used for exchange about updated files with product
struct cUpdaterUpdatedFileList : public cSerializable
{
    DECLARE_IID(cUpdaterUpdatedFileList, cSerializable, PID_UPDATER, updater_UpdatedFileList);

    tDATETIME m_UpdateDate;
    cStrObj m_updateSuffix;
    cStrObj m_selectedComponentIdentidier;
    cVector<cUpdaterUpdatedFile> m_files;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUpdaterUpdatedFileList, "UpdaterUpdatedFileList")
    SER_VALUE(m_UpdateDate,  tid_DATETIME, "UpdateDate")
    SER_VALUE(m_updateSuffix,  tid_STRING_OBJ, "updateSuffix")
    SER_VALUE(m_selectedComponentIdentidier,  tid_STRING_OBJ, "selectedComponentIdentidier")
	SER_VECTOR(m_files, cUpdaterUpdatedFile::eIID, "files")
IMPLEMENT_SERIALIZABLE_END();


///////////////
struct cUpdatedComponentName : public cSerializable
{
    DECLARE_IID(cUpdatedComponentName, cSerializable, PID_UPDATER, updater_UpdatedComponentName);
    cStrObj m_updatedComponentName;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUpdatedComponentName, "UpdatedComponentName")
    SER_VALUE(m_updatedComponentName,  tid_STRING_OBJ, "updatedComponentName")
IMPLEMENT_SERIALIZABLE_END();
///////////////


///////////////
struct cUpdaterStatistics : public cTaskStatistics
{
	cUpdaterStatistics()
      : m_qwTotalDownloadBytesCount(0),
        m_qwDownloadedBytesCount(0),
        m_dbSpeed(0),
        m_errUpdateError(errOK)
	{
    }

	DECLARE_IID_STATISTICS(cUpdaterStatistics, cTaskStatistics, PID_UPDATER);

    // download path of file downloading
    cStringObj     m_strHostPath;
    // file name of file downloading
    cStringObj     m_strFileName;
    // total download size (bytes)
    tQWORD         m_qwTotalDownloadBytesCount;
    // already downloaded (bytes)
	tQWORD         m_qwDownloadedBytesCount;
    // update speed (Killobytes/sec per seconds)
	tDOUBLE        m_dbSpeed;
    // update result
    tERROR         m_errUpdateError;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUpdaterStatistics, "Updater2005Statistics" )
	SER_BASE( cTaskStatistics, 0 )
    SER_VALUE( m_strHostPath,                tid_STRING_OBJ, "HostPath"                )
    SER_VALUE( m_strFileName,                tid_STRING_OBJ, "FileName"                )
    SER_VALUE( m_qwTotalDownloadBytesCount,  tid_QWORD,      "TotalDownloadBytesCount" )
	SER_VALUE( m_qwDownloadedBytesCount,     tid_QWORD,      "DownloadedBytesCount"    )
	SER_VALUE( m_dbSpeed,                    tid_DOUBLE,     "Speed"                   )
    SER_VALUE( m_errUpdateError,             tid_ERROR,      "UpdateError"             )
IMPLEMENT_SERIALIZABLE_END();
///////////////


///////////////

// enumerates updater source types
// note: place default sources first
enum eUpdaterSourceType
{
	UPDSRC_AK,
	UPDSRC_KL,
	UPDSRC_CUSTOM,
	UPDSRC_NUMB,					// number of items
	UPDSRC_DEF_NUMB = UPDSRC_CUSTOM	// number of default items
};

// cUpdaterSource structure
struct cUpdaterSource : public cSerializable
{
	cUpdaterSource() :
		m_bEnable(false),
        m_bPlain(false),
		m_dwType(UPDSRC_CUSTOM)
	{
	}

	DECLARE_IID(cUpdaterSource, cSerializable, PID_UPDATER, updater_UpdateSource);

    tBOOL            m_bEnable;
    tBOOL            m_bPlain ;
    cStringObj       m_strPath;
    tDWORD           m_dwType;		// takes eUpdaterSourceType values
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUpdaterSource, 0 )
	SER_VALUE( m_bEnable,                tid_BOOL,       "Enable"    )
    SER_VALUE( m_bPlain,                 tid_BOOL,       "Plain"     )
	SER_VALUE( m_strPath,                tid_STRING_OBJ, "Path"      )
	SER_VALUE( m_dwType,                 tid_DWORD,      "Type"      )
IMPLEMENT_SERIALIZABLE_END();
///////////////



///////////////
// Updater Event structure
struct cUpdaterEvent : public cTaskHeader
{
	cUpdaterEvent()
      : m_qwEvent(0),
        m_errError(errOK),
        m_tmTimeStamp(-1),
        m_dwFileSize(0)
	{
    }

	DECLARE_IID(cUpdaterEvent, cTaskHeader, PID_UPDATER, updater_UpdateEvent);

	// event unique identifier to write to Journal
	tQWORD m_qwEvent;
    // event or error identifier, of KLUPD::CoreError type
	tERROR m_errError;
    // display host address in statistics
	cStringObj m_strHostPath;
    // display file name for statistics
	cStringObj m_strFileName;
    // timestamp of event
	time_t m_tmTimeStamp;
    // traffic caused by file download
	tDWORD m_dwFileSize;
    // default localization for message done by Updater Core
    cStringObj m_defaultLocalization;
    // additional parameter 1 for KLUPD::CoreError message
    cStringObj m_parameter1;
    // additional parameter 2 for KLUPD::CoreError message
    cStringObj m_parameter2;
    // rollback flag
    tBOOL m_blRollback ;
};

IMPLEMENT_SERIALIZABLE_BEGIN(cUpdaterEvent, "Updater2005Event")
	SER_BASE(cTaskHeader, 0)
	SER_VALUE( m_qwEvent,					tid_QWORD,       "Event"    )
	SER_VALUE( m_errError,					tid_ERROR,       "Error"    )
	SER_VALUE( m_strHostPath,				tid_STRING_OBJ,  "HostPath" )
	SER_VALUE( m_strFileName,				tid_STRING_OBJ,  "FileName" )
	SER_VALUE( m_tmTimeStamp,				tid_DWORD,       "ReportTime" )
	SER_VALUE( m_dwFileSize,				tid_DWORD,       "FileSize" )
    SER_VALUE( m_defaultLocalization,       tid_STRING_OBJ,  "defaultLocalization" )
	SER_VALUE( m_parameter1,				tid_STRING_OBJ,  "parameter1" )
	SER_VALUE( m_parameter2,				tid_STRING_OBJ,  "parameter2" )
    SER_VALUE( m_blRollback,				tid_BOOL,        "rollback" )
IMPLEMENT_SERIALIZABLE_END();
///////////////


// Update task settings
typedef cVector<cUpdaterSource> updatesources_t;

struct cUpdaterSettings : public cTaskSettings
{
	cUpdaterSettings()
      : m_bNotifyBeforeUpdate(cFALSE),
        m_bRescanQuarantine(cFALSE),
        m_bUseInternalSources(cTRUE),
        m_bRollback(cFALSE),
        m_bUseRegion(cFALSE),
        m_dwInstallId(0),
        m_blAKServerUse(cFALSE),
        m_bUseRetr(cFALSE)
	{
		m_dwVersion = 2;
	}

	DECLARE_IID_SETTINGS(cUpdaterSettings, cTaskSettings, PID_UPDATER);

	tBOOL               m_bNotifyBeforeUpdate;
	tBOOL               m_bRescanQuarantine;
	updatesources_t     m_aSources;
	tBOOL               m_bUseInternalSources;
	tBOOL               m_bRollback;
	tBOOL               m_bUseRegion;
    cStrObj             m_strRegion;
    cStrObj             m_strRetrPath;
    tDWORD              m_dwInstallId ;
    tBOOL               m_blAKServerUse ;
	tBOOL               m_bUseRetr;
};

IMPLEMENT_SERIALIZABLE_BEGIN( cUpdaterSettings, "Updater2005Settings" )
	SER_BASE  ( cTaskSettings, 0 )
	SER_VALUE ( m_bNotifyBeforeUpdate,          tid_BOOL,           "NotifyBeforeUpdate" )
	SER_VALUE ( m_bRescanQuarantine,            tid_BOOL,           "RescanQuarantine" )
	SER_VECTOR( m_aSources,                     cUpdaterSource::eIID, "Sources" )
	SER_VALUE ( m_bUseInternalSources,          tid_BOOL,           "UseInternalSources" )
	SER_VALUE ( m_bRollback,			        tid_BOOL,	        "Rollback" )
	SER_VALUE ( m_bUseRegion,			        tid_BOOL,	        "UseRegion" )
	SER_VALUE ( m_strRegion,                    tid_STRING_OBJ,     "Region" )
	SER_VALUE ( m_strRetrPath,                  tid_STRING_OBJ,     "RetrPath" )
	SER_VALUE ( m_dwInstallId,   			    tid_DWORD,	        "InstallId" )
    SER_VALUE ( m_blAKServerUse,	            tid_BOOL,	        "AKServerUse" )
  	SER_VALUE ( m_bUseRetr,                     tid_BOOL,           "UseRetr2" )
IMPLEMENT_SERIALIZABLE_END();

struct FilesToRemove : public cSerializable
{
	DECLARE_IID(FilesToRemove, cSerializable, PID_UPDATER, updater_FilesToRemove);
    cVector<cStringObj> m_files;
};

IMPLEMENT_SERIALIZABLE_BEGIN(FilesToRemove, "FilesToRemove")
    SER_VECTOR(m_files, tid_STRING_OBJ, "Files")
IMPLEMENT_SERIALIZABLE_END();



///////////////
// generates unique mutex name for component to protect load bases while update.
//  mutex name is generated per component
inline cStrObj mutexNameToLockBasesLoadByComponent(const tCHAR *componentIdentifier)
{
	cStrObj result = "23DFE74B-7A8A-4ea6-8FA4-FF859F15F2C0:";
    result += componentIdentifier;
    return result;
}

inline tERROR LockBasesLoadByComponent(cObject *owner, const tCHAR *category, cMutex **pmtx)
{
	tERROR error = errOK;
	if(!*pmtx)
	{
		cStrObj sMutexName = mutexNameToLockBasesLoadByComponent(category);
		error = owner->sysCreateObject((hOBJECT *)pmtx, IID_MUTEX);
		if(PR_SUCC(error))
			error = (*pmtx)->set_pgOBJECT_NAME((tPTR)sMutexName.c_str(cCP_ANSI), 0);
		if(PR_SUCC(error))
			error = (*pmtx)->set_pgGLOBAL(cTRUE);
		if(PR_SUCC(error))
			error = (*pmtx)->sysCreateObjectDone();
	}
	PR_TRACE((0, prtIMPORTANT, "baselock\tLocking '%s' category", category));
	if(PR_SUCC(error))
		error = (*pmtx)->Lock(cSYNC_INFINITE);
	PR_TRACE((0, prtIMPORTANT, "baselock\tCategory '%s' locked (%terr)", category, error));
	return error;
}

inline tERROR UnlockBasesLoadByComponent(cMutex *mtx)
{
	if(!mtx)
		return errNOT_OK;
	tERROR error = mtx->Release();
	PR_TRACE((0, prtIMPORTANT, "baselock\tUnlocked (%terr)", error));
	return error;
}

class cBasesLoadLocker
{
public:
    enum LockType
    {
        withLock,
        noLock
    };

    // lockLoadBasesMutex [in] -  should not be locked if pm_COMPONENT_PREINSTALLED
     // message received, because updater locks the mutex itself.
     // Note: this parameter is for convenience of scoped lock idiom in case lock shold be performed under condition
	cBasesLoadLocker(cObject *owner, const tCHAR *category, const LockType &lockLoadBasesMutex)
        : m_lockLoadBasesMutex(lockLoadBasesMutex)
	{
        if(m_lockLoadBasesMutex == withLock)
		    LockBasesLoadByComponent(owner, category, m_mtx);
	}
	~cBasesLoadLocker()
	{
        if(m_lockLoadBasesMutex == withLock)
    		UnlockBasesLoadByComponent(m_mtx);
	}

private:
	cAutoObj<cMutex> m_mtx;
    const LockType m_lockLoadBasesMutex;
};

#endif  // S_UPDATER_H_INCLUDED_D0D9AB52_1818_401d_8736_FB6EF52858D0
