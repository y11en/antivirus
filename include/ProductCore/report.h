#ifndef _REPORT_H_0909123878
#define _REPORT_H_0909123878

////////////////////////////////////////////////////////////

typedef unsigned __int8  uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;
typedef uint64           tObjectId;

////////////////////////////////////////////////////////////

#define RF_UNICODE          0x0001 // The UTF16 object name will be stored converted to UTF8 and retrieved as UTF16 again
#define RF_CASE_INSENSETIVE 0x0100
#define RF_SPLIT_PATH       0x0200 // The name is treated as wchar_t* path, splitted at '\' and stored as series of objects (dirs)
#define RF_SPLIT_FULL_PATH  0x0400 // The name is treated as wchar_t* path, splitted at last '\' and stored as two objects - full path & filename
#define RF_MAP_SUBST        0x0800
#define RF_MAP_VOLUME       0x1000

#define RF_DONT(flag) (((uint32)(flag)) << 16) // Drops specified flags from default, for ex. RF_UNICODE | RF_DONT(RF_MAP_SUBST)

#define cSIZE_STR  ((size_t)-1)
#define cSIZE_WSTR ((size_t)-2)

////////////////////////////////////////////////////////////

enum enVerdict
{
	eUNKNOWN		       = 0,
	
	eCLEAN		           = 1,    // Object is clean
	eDETECTED              = 2,    // Object is detected (see enRDetectType)
	eSUSPICION             = 4,    // Object is suspicion (see enRDetectType)

	eARCHIVED	           = 5,    // Object is archive
	ePACKED	               = 6,    // Object is packed
	eENCRYPTED             = 7,    // Object is encrypted
	eCORRUPTED	           = 8,    // Object is corrupted
	ePASSWORD_PROTECTED    = 19,
	ePROCESSING_ERROR      = 18,
	eCANTCHANGEATTR        = 20,

	eALLOWED               = 34,   // Operation is allowed
	eDENIED                = 33,   // Operation is denied
	eREJECTED              = 40,   // Operation is rejected (? drop net packet?)
	eNOT_PROCESSED         = 17,   // Object was not processed (skipped)
	eADDEDTOEXCLUDE        = 30,   // Object is added to Trusted zone
	eTERMINATED            = 41, 
	eTERMINATE_FAILED      = 42, 
	eDISINFECTED           = 9,    // Object is disinfected
	eDISINFECTED_ON_REBOOT = 25,   // Object will be disinfected on computer reboot
	eDISINFECT_ON_REBOOT_FAILED = 26,
	eNOT_DISINFECTED       = 10,   // Disinfection failed
	eCURED	               = 28,
	eDELETED               = 11,   // Object is deleted
	eDELETED_ON_REBOOT     = 23,   // Object will be rejected on computer reboot
	eDELETE_ON_REBOOT_FAILED	 = 24,
	eNOT_DELETED           = 12,   // Deletion failed
	eOVERWRITED	             = 27,
	eQUARANTINED           = 15,   // Object is quarantined
	eQUARANTINED_ON_REBOOT = 31,   // Object will be quarantined on computer reboot
	eNOT_QUARANTINED       = 16,   // Object was not added to Quarantine
	eRESTORED              = 32,
	eRENAMED               = 35,   // Object is renamed
	eNOT_RENAMED           = 37,   // Object was not renamed
	eROLLBACKED            = 43,
	eROLLBACK_FAILED       = 44,
	eBACKUPED              = 13,
	eBACKUP_FAILED         = 14,
	eREPARED               = 45,
	eREPAIR_FAILED         = 46,
	eDISABLED              = 47,

	ePROTECTION            = 80,   // Base for protection verdicts (64 items)

	eTASK_STARTED          = 0x91,
	eTASK_STOPPED          = 0x92,
	eTASK_FAILED           = 0x93,

	eUPDATER_ERROR         = 0x101,
	eUPDATER               = 0x102,

	ePARCTL_SWITCHPROFILE   = 0x0200,
	ePARCTL_HEURISTICSTATUS = 0x0201,
};

////////////////////////////////////////////////////////////

enum enObjType
{
	// object types
	eFile        = 0x01, // File
	eDirectory   = 0x02,
	eRegKey      = 0x03, // Registry key
	eRegValue    = 0x04, // Registry value
	eProcess     = 0x05, // Process
	eThread      = 0x06, // Thread
	eModule      = 0x07, // Module
	eLogSector   = 0x08, // Logical sector
	ePhysSector  = 0x09, // Physical sector
	eMemory	     = 0x0A, // Memory
	eMailMessage = 0x0B, // Mail message
	eMailAttach  = 0x0C, // Mail attachment
	eURL         = 0x0D, // URL
	eScript      = 0x0E, // VB or Java script
	ePort        = 0x0F, // Network port
	eConnection  = 0x10, // Network connection
	ePacket      = 0x11, // Network packet
	eDialStr     = 0x12, // Dialing string
	eDetectName  = 0x13, // Detection name
	eLogicalDisk = 0x14, // C:, D:, etc.
	eDevice      = 0x15, // \Device\HardDiskVolume1\...
	eVolume      = 0x16, // \Volume{GUID}\...
	eString      = 0x17, // any string, for ex. command line
	eRegRoot     = 0x18, // dword value HKEY_LOCAL_MACHINE, HKEY_USERS, etc.
	eTask        = 0x19, // Task
};	

typedef uint32 RDBI_TAG;
#define MAKE_RDBI_TAG(A,B,C,D) \
	((((((((RDBI_TAG)(A)) << 8) | (B)) << 8) | (C)) << 8) | (D))

enum eCommonDataTypes
{
	eObjectMapping = MAKE_RDBI_TAG(0xCD, 0, 0, 1),
};

////////////////////////////////////////////////////////////

enum enEventAction
{
	evtUnk            = 0x00,
	
	// action with object
	evtOpen           = 0x01,
	evtCreate         = 0x02,
	evtRead           = 0x03, 
	evtWrite          = 0x04, 
	evtModify         = evtWrite,
	evtDelete         = 0x05,
	evtRename         = 0x06,
	evtProcessStart   = 0x07,
	evtProcessStop    = 0x08,
	evtImageLoad      = 0x09,
	evtImageUnload    = 0x0A,
	evtSend           = 0x0B,
	evtReceive        = 0x0C,
	evtTerminate      = 0x0D,
	evtProcStart          = 0x0E,
	evtProcStop           = 0x0F,
	evtSetHook            = 0x10,
	evtCodeInject         = 0x11,
	evtWindowsShutDown    = 0x12,
	evtHiddenRegistry     = 0x12,
	evtKeyLogger          = 0x13,
	evtSetHardLink        = 0x14,
	evtSespend            = 0x15,
	evtSchedulerStart     = 0x16,
	evtWMSend             = 0x17,
	evtDrvStart           = 0x18,
	evtServiceStart       = 0x19,
	evtScreenShots        = 0x1A,
	evtDiskFormat         = 0x1B,
	evtLLDiskAccess       = 0x1C,
	evtLLFSAccess         = 0x1D,
	evtClipBoardAcceess   = 0x1E,
	evtSysEnterChange     = 0x1F,
	evtADSAccess          = 0x20,
	evtDirectMemAccess    = 0x21,
	evtReadProcMem        = 0x22,
	evtSetDbgPrivilege    = 0x23,
	evtChangeObjPrivilege = 0x24,
	evtUseBrowserCL       = 0x25,
	evtUseBrowserAPI      = 0x26,
	evtUseSystemAPI       = 0x27,
	evtAddAppToGr         = 0x28,
};

////////////////////////////////////////////////////////////

enum enRDetectType
{
	// AV
	eVirware        = 1,
	eTrojware       = 2,
	eMalware        = 3,
	eAdware         = 4,
	ePornware       = 5,
	eRiskware       = 6,
	eXFiles         = 20,
	eSoftware       = 21,
	eVulnerability  = 22,
				   
	// OS		   
	ePhishing       = 30,   // Phishing is detected
	eDialing        = 31,   // Hidden dialing attempt is detected
	eAttack         = 32,   // Network attack is detected
				   
	// UC		   
	eBanner         = 40,   // Banner url was blocked
	eUnwantedWebContent = 41,   // Attempt to open unwanted web-site
	eSpam           = 42,
	eProbableSpam   = 43,
	eHam            = 44,

	// HIPS
	eFileAccess     = 50,   // File access
	eRegistryAccess	= 51,   // Registry access
	eNetworkAccess	= 52,   // Network access
	eHardwareAccess	= 53,   // Hardware access
	eSecurityAccess	= 54,   // Security access

	eSuspicionActivity = 60, // Suspicion activity
	// ... 
};


////////////////////////////////////////////////////////////

enum enRDetectDanger
{
	eDDHigh		     = 1, // Critical detect
	eDDMedium		 = 2, // Warning detect
	eDDLow		     = 4, // Low detect
	eDDInformational = 8, // Informational detect
};

////////////////////////////////////////////////////////////

enum enIsExactDetect
{
	//	eUnknown    = 0x00,
	eExact		     = 0x01, // Exact detection
	ePartial         = 0x02,
	eHeuristic       = 0x03,
	eProbably	     = 0x04, // Probably detection
};

////////////////////////////////////////////////////////////

enum enDecisionReason
{
	//	eUnknown       = 0x00,
	eUSER                = 0x0001, // User-decided, also Object was skipped by user in GUI(eNOT_PROCESSED+eUser)
	eREPORTONLY          = 0x0002,
	ePOSTPONED           = 0x0003,
	eTASKSTOPPED         = 0x0004,
	eERROR               = 0x0005,

	eDATABASE            = 0x0021, // (AKA eBlackList) AV bases, Phishing bases, IDS bases, etc.
	eUSER_BLACK_LIST     = 0x0022, // User-defined blacklist (for ex. banner sites)
	eWHITE_LIST          = 0x0023, // KL whitelist
	eUSER_WHITE_LIST     = 0x0024, // user whitelist
	eEMULATOR            = 0x0025, // AV emulator
	eBB                  = 0x0026, // Behavior analysis
	eHEURISTIC           = 0x0027, // heuristic analisys (like anti-banner heur) 
	eBAYES               = 0x0028, // iBayes filter
	eGSG                 = 0x0029, // GSG technology (images recognition)
	ePDB                 = 0x0030, // PDB technology (headers recognition)
	eSFDB                = 0x0031, // i-Checker
	eISWIFT              = 0x0032, // i-Swift
	eALLOWED_SENDER      = 0x0033, // Sender from white list
	eBLOCKED_SENDER      = 0x0034, // Sender from black list
	eALLOWED_PHRASE      = 0x0035, // Phrase from white list
	eBLOCKED_PHRASE      = 0x0036, // Phrase from black list

	// -> Detect
	eDETECT_BYHASH       = 0x0041,
	eDETECT_INFORMATION  = 0x0042,

	// -> skipped
	eSIZE                = 0x0081, // Skipped by size
	eTYPE                = 0x0082, // Skipped by type
	eEXCLUDE             = 0x0083, // Skipped by exclude list (MASKS - ?!)
	eTIME                = 0x0084, // Time limit
	eNORIGHTS            = 0x0085, // Skipped by right (no enough rights to read the object)
	eNOTFOUND            = 0x0086, // Object not found
	eLOCKED              = 0x0089, // Object is locked and cannot be read

	// -> not Cured
	eNONCURABLE          = 0x0102,
	eWRITEPROTECT        = 0x0103,
	eNONOVERWRITABLE     = 0x0106,
	eCOPYFAILED          = 0x0107,
	eWRITEERROR          = 0x0108,
	eOUTOFSPACE          = 0x0109,
	eREADERROR           = 0x0110,
	eDEVICENOTREADY      = 0x0111,
	eWRITENOTSUPPORTED   = 0x0112,
	eCANNOTBACKUP        = 0x0113,

	eANTISPAM            = 0x0200,
	eANTISPAM_BASE       = 0x0210,
	eANTISPAM_END        = 0x0300,
};

////////////////////////////////////////////////////////////


// databases IDs in range 0x00-0xFE
// value 0xFF reserved for all databases, i.e. GetDBInfo(ALL_DATABASES, ...)
#define dbAllDatabases 0xFF
enum eReportDatabases
{
	dbGlobalObjects,
	dbRTP,
	dbScan,
	dbUpdate,
	dbNetworkPackets,
	dbProcesses,
	dbMaxReportDatabaseId
};

enum enTaskGroup
{
	eTASK_GROUP_PROTECTION = 0x00, // Protection
	eTASK_GROUP_AV         = 0x01, // Anti-Virus
	eTASK_GROUP_OS         = 0x02, // Online Security
	eTASK_GROUP_UC         = 0x03, // Unwanted content
	eTASK_GROUP_HIPS       = 0x04, // HIPS
	eTASK_GROUP_SCAN       = 0x05, // SCAN
	eTASK_GROUP_UPDATER    = 0x06, // Updater
};

enum enTaskID
{
	eTASK_PROTECTION = 0x00, // Protection
	eTASK_FAV	     = 0x01, // File Anti-Virus
	eTASK_MAV	     = 0x02, // Web Anti-Virus
//	eTASK_UNUSED	 = 0x03,
	eTASK_HTTP	     = 0x04, // Main Anti-Virus
	eTASK_AP	     = 0x05, // Anti-Phishing
	eTASK_AD	     = 0x06, // Anti-Dialer
	eTASK_IDS	     = 0x07, // Intrusion detection system
	eTASK_AB	     = 0x08, // Anti-Banner
	eTASK_AS	     = 0x09, // Anti-Spam
	eTASK_PC	     = 0x0A, // Parental control
	eTASK_HIPS	     = 0x0B, // HIPS
	eTASK_SW         = 0x0C, // System Watcher
	eTASK_SELFPROT   = 0x0D, // Self-Protection
	eTASK_PDM        = 0x0E, // Proactive Defence Monitor
	eTASK_LIC	     = 0x0F, // Licensing Subsytem

	eTASK_F_RUNTIME	 = 0x80, // задача не пермaментная

	eTASK_SCAN       = eTASK_F_RUNTIME|0x00, // Scan
	eTASK_UPDATER    = eTASK_F_RUNTIME|0x01, // Updater
	eTASK_ROLLBACK   = eTASK_F_RUNTIME|0x02, // Rollback
	eTASK_SNIFFER    = eTASK_F_RUNTIME|0x03, // Sniffer
};

////////////////////////////////////////////////////////////

#include <pshpack1.h>

typedef struct tag_Event
{
	union
	{
		uint64       m_ExtraInfo;        // -> ObjectDB?
		struct
		{
			uint32       m_ExtraInfoSub1;
			uint32       m_ExtraInfoSub2;
		};
		double       m_dExtraInfo;
	};

	tObjectId    m_ObjectID;         // object id -> ObjectDB
	tObjectId    m_OldObjectID;      // old object id -> ObjectDB
	tObjectId    m_DetectName;       // verdict name
	uint64       m_Timestamp;        // date-time (?) in FILETIME format or Prague DATETIME format?
	uint64       m_PID;              // uniq process id -> ProcessDB
	uint32       m_AppID;            // uniq application id -> AppDB
	uint32       m_Error;            // error
	//	uint16   UserID;           // uniq user id -> UserDB => moved into process info
	uint16       m_EventID;          // (?) AdminKit
	uint8        m_Action;           // enum enEventAction
	uint16       m_DecisionReason;   // enum enDecisionReason (includes technologies)
	uint8        m_ObjectType;       // enum enObjType
	uint8        m_TaskGroup;        // enum TaskGroup
	uint8        m_TaskID;           // enum TaskID
	uint16       m_Verdict;          // enum enVerdict
	uint8        m_DetectType;       // enum enRDetectType
	uint8        m_DetectDanger;     // enum enRDetectDanger (?) isn't the same as Severity?
	uint8        m_IsExact;          // enum enIsExactDetect
	uint8        m_Severity;         // enum EventSeverity
} tReportEvent;

typedef struct tag_ProcessInfo
{
	tObjectId    m_ImageFileID;
	tObjectId    m_CmdLineID;      
	uint32       m_NativePid;
	uint32       m_ParentNativePid;
	uint64       m_UniqPid;
	uint64       m_ParentUniqPid;
	uint64       m_ftStartTime;
	uint64       m_StartEventID;
	uint64       m_ftExitTime;
	uint64       m_ExitEventID;
	tObjectId    m_UserID;
	uint32       m_AppId;
} tProcessInfo;

#define IS_UNIQ_PID(pid) (((pid) >> 32) != 0)

#include <poppack.h>

////////////////////////////////////////////////////////////

#endif  // _REPORT_H_0909123878
