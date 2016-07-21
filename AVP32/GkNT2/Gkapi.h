#ifndef __GKAPI_H__
#define __GKAPI_H__

#ifdef NT_DRIVER
#define	GKSTATUS	NTSTATUS
#endif

#ifdef VTOOLSD  
#define	GKSTATUS	ULONG
#endif

#if !defined ( __FILTER_DRIVER_SOURCE__) && !defined(VTOOLSD)
#define FILTER_API _declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"  {
#endif

#ifndef POLICY_H
#include "Policy.h"
#endif __POLICY_H

// what type of message a compund message contains
#define	GK_MT_ALERT			0x1
#define	GK_MT_PROGRESS		0x2

// what should be done with the message in user-mode
#define FS_MSG_DEFAULT  0x0		// default behaviour (content is normal GKAPI message)
#define	FS_MSG_DISPLAY	0x1		// display the message (content is normal GKAPI message)
#define FS_MSG_LOG		0x2		// log the message (content is normal GKAPI message)
#define FS_MSG_OID		0x4		// OID registration
#define FS_MSG_PRIVATE	0x100	// pass message to plug-in dll (content may be anything)

// plug-in message type
#define FS_MSG_TYPE_STARTUP		 0x1		// startup messages
#define FS_MSG_TYPE_SHUTDOWN	 0x2		// shutdown messages
#define FS_MSG_TYPE_ERROR		 0x3		// error messages (database missingm etc.)
#define FS_MSG_TYPE_WARNING		 0x4		// warning messages 
#define FS_MSG_TYPE_ALERT		 0x5		// normal messages (access denied, etc.)

// internal message types
#define FS_INTERNAL_MSG_DBVALIDATION	0x1	// check integrity of database files
#define FS_INTERNAL_MSG_CANCEL			0x2	// cancel specific task in plug-in
#define FS_INTERNAL_MSG_DBRELOAD		0x3	// reload database files
#define FS_INTERNAL_MSG_AUTORELOAD_ON	0x4	// reload database files automatically if detected
#define FS_INTERNAL_MSG_AUTORELOAD_OFF	0x5	// do *not* reload database files automatically if detected
#define FS_INTERNAL_MSG_SCANNINGSTATUS	0x6	// report if able toi detect viruses
//turbpa010412-[14323]{
#define FS_INTERNAL_MSG_DELETE_OLD_DB	0x7	// enable/disable deletion of old databases 
//turbpa010412-[14323]}

// status codes
#define FS_STATUS_OK					0x0
#define FS_STATUS_FAILURE				0x1
#define FS_STATUS_NOSUCHPLUGIN			0x2
#define FS_STATUS_INVALIDPARAMETER		0x3
#define FS_STATUS_INVALIDINPUTBUFFER	0x4
#define FS_STATUS_INVALIDOUTPUTBUFFER	0x5
#define FS_STATUS_OUTPUTBUFFERTOOSMALL	0x6

// database validation status codes (values with 0x100 cause update to be invalid)
#define FSAV_DB_VALID			0x10		// file is valid for plug-in
#define FSAV_DB_NOTCHECKED		0x11		// not checked
#define FSAV_DB_DELETE			0x12		// delete from runtime directory
#define FSAV_DB_MISSING			0x114	// file required, not available
#define FSAV_DB_INVALID			0x115	// database invalid
#define FSAV_DB_NOTDB			0x116	// does not seem to be a database file
#define FSAV_DB_CORRUPT			0x117	// integrity check failed (/eg. crc)
#define FSAV_DB_VERSION			0x118	// file version incompatible with plug-in version


// task actions
#define FS_TASK_ACTION_NONE			0x0
#define FS_TASK_ACTION_DISINFECT	0x2
#define FS_TASK_ACTION_DELETE		0x4
#define FS_TASK_ACTION_RENAME		0x6
#define FS_TASK_ACTION_ASK			0x8

// actions taken
#define ACTION_NONE							0x0
#define ACTION_ACCESS_DENIED                0x1
#define ACTION_ACCESS_DENIED_DISINFECTED    0x2
#define ACTION_ACCESS_DENIED_RENAMED        0x3
#define ACTION_ACCESS_DENIED_REMOVED        0x4
#define ACTION_ACCESS_DENIED_PROMPT_USER    0x5

// task targets
#define FS_TARGET_FILE			0x1
#define FS_TARGET_DIRECTORY		0x2
#define FS_TARGET_BOOTSECTOR	0x3
#define FS_TARGET_MBR			0x4
#define FS_TARGET_MEMORY		0x5

// task status codes
#define FS_TASK_STATUS_DONE			0x1
#define FS_TASK_STATUS_INPROGRESS	0x2
#define FS_TASK_STATUS_FAILURE		0x3
#define FS_TASK_STATUS_CANCELLED	0x4
#define FS_TASK_STATUS_NOPLUGIN		0x5


// predefined sizes
#define MAX_FILENAME_SIZE			   1024    // filename size
#define MAX_PLUGIN_NAME                 256    // plug-in name size
#define MAX_SID_SIZE                    256    // SID string size 
#define MAX_MESSAGE_SIZE               1024    // message text size
#define MAX_DATA_SIZE                  1024    // data size
#define MAX_OPTIONAL_PARAMETER_SIZE    1024    // optional parameter size
#define MAX_CURRFILE_LENGTH				512    // length of filename for progress message
#define VIRUSNAME_SIZE			128

// scanning extensions to use
#define FS_EXTENSIONS_DEFAULT		0x0	// use the extensions provided by Gatekeeper
#define FS_EXTENSIONS_ALL			0x1	// scan all files

// object types
#define FS_OBJECT_FILE 	0x1		// default value
#define FS_OBJECT_BS   	0x2		// set for boot sector infections
#define FS_OBJECT_MBR  	0x4		// set for MBR infections
#define FS_OBJECT_MEMORY 	0x8 // set for memory infections
#define FS_OBJECT_INSIDE_ARCHIVE 0x10	// if the file is inside an archive
#define FS_OBJECT_CANNOT_DISINFECT 0x20 // set if the scanner knows for sure the object cannot be cured by disinfection (for example bs image files .BOO)
#define FS_OBJECT_SUSP_INFECTION 0x40 // if the virus was found using heuristic methods of some kind; "possibly infected with an unknown virus"
	
// versions
#define GATEKEEPER_API_VERSION		0x0510	// 5.10
#define PLUGIN_API_VERSION			0x0200	// 2.0
#define GATEKEEPER_MSG_VERSION		0x0200	// 2.0
#define GATEKEEPER_VERSION			0x0510	// 5.10
#define GKPROGRESSMESSAGE_VERSION	0x0100	// 1.0
#define GKTASK_VERSION				0x0100	// 1.0

#pragma pack(push, 1)

typedef ULONG	DWORD;
typedef BYTE *	PBYTE;

typedef struct tagPLUGINMESSAGE {
	// version & size - attribute of all the structures
	DWORD dwSize;       // sizeof(PLUGINMESSAGE)
	DWORD dwVersion;    // version (PLUGIN_API_VERSION)

	// data filled by plug-in driver
	DWORD dwTaskID;												// task id 
	DWORD dwAction;                                             // action taken (ACTION_*)
	DWORD dwType;                                               // message type (FS_MSG_TYPE_*)
	DWORD dwSequentialNumber;                                   // sequential message number
	DWORD dwLocalizationId;                                     // message ID for localization
	CHAR pszFileName[MAX_FILENAME_SIZE];						// filename - if present overrides the one in GKMESSAGE
	CHAR pszVirusName[VIRUSNAME_SIZE];							// virus name
	CHAR pszMessageText[MAX_MESSAGE_SIZE];						// message text
	DWORD dwOptionalParameterSize;                              // optional parameter size
	BYTE  pbOptionalParameter[MAX_OPTIONAL_PARAMETER_SIZE];     // optional parameter
	DWORD dwObjectType;											// FS_OBJECT_*
	DWORD dwReserved2;
	DWORD dwReserved3;
	DWORD dwReserved4;
	DWORD dwReserved5;

} PLUGINMESSAGE, *PPLUGINMESSAGE;


// GKMESSAGE structure is passed to FSMGR by GK
typedef struct tagGKMESSAGE {
	// version & size - attribute of all the structures
	DWORD dwSize;      // sizeof(GKMESSAGE)
	DWORD dwVersion;	 // version (GKMESSAGE_VERSION)

	// data filled by GK
	DWORD dwPluginId;                        // which plugin is sending information
	DWORD dwTaskID;							 // task id 
	DWORD dwFlags;                           // type of data (FS_MSG_*)
	CHAR pszPluginName[MAX_PLUGIN_NAME];     // plug-in name
	CHAR pszFileName[MAX_FILENAME_SIZE];             // filename
	CHAR pszSID[MAX_SID_SIZE];              // SID
	
	// data came from plug-in driver
	union 
	{
		PLUGINMESSAGE pim;                    // plug-in message

		struct 
		{
			DWORD dwSize;                   // sizeof data
			BYTE pbData[MAX_DATA_SIZE];    // data
		} PluginData;

	} PluginStruct;
	
} GKMESSAGE, *PGKMESSAGE;

typedef struct tagGKPROGRESSMESSAGE {
	// version & size - attribute of all the structures
	DWORD dwSize;      // sizeof(GKPROGRESSMESSAGE)
	DWORD dwVersion;	 // version (GKPROGRESSMESSAGE_VERSION)

	// data filled by GK
	DWORD dwTaskID;	// task id 
	DWORD dwTaskStatus;	// FS_TASK_STATUS_*
	DWORD dwTaskTarget;	// FS_TARGET_*
	DWORD dwObjectsProcessed;
	DWORD dwObjectsInfected;
	DWORD dwObjectsDisinfected;
	DWORD dwObjectsDeleted;
	DWORD dwObjectsRenamed;
	DWORD dwReserved1;
	DWORD dwReserved2;
	DWORD dwReserved3;
	DWORD dwReserved4;
	DWORD dwReserved5;
	CHAR  currentFile[MAX_CURRFILE_LENGTH];
	} GKPROGRESSMESSAGE, *PGKPROGRESSMESSAGE;

typedef struct tagGKCOMPOUNDMESSAGE {
	ULONG	type;
	union
	{
		GKMESSAGE			alert;
		GKPROGRESSMESSAGE	progress;
	} Compound;

} GKCOMPOUNDMESSAGE, *PGKCOMPOUNDMESSAGE;


typedef struct tagGKTASK {
	// version & size - attribute of all the structures
	DWORD dwSize;      // sizeof(GKPROGRESSMESSAGE)
	DWORD dwVersion;	 // version (GKPROGRESSMESSAGE_VERSION)

	// data filled by GK
	DWORD dwTaskID;		// task id 
	DWORD dwScanningEngine;	// id of engine to use, = == all engines
	DWORD dwTaskTarget;	// FS_TARGET_*
	DWORD dwAction;		// FS_TASK_ACTION_*
	DWORD dwProgressInterval;	// in milliseconds

	DWORD dwScanInsideArchives;
	DWORD dwScanAllFiles;


	CHAR Target[1024];		// file name, directory name, drive 
	CHAR includeExtensions[128];
	CHAR excludeExtensions[128];
	GKPROGRESSMESSAGE Progress;

} GKTASK,*PGKTASK;

/*
 *  This pragma disables the warning issued by the Microsoft C compiler
 *  when using a zero size array as place holder when compiling for
 *  C++ or with -W4.
 *
 */
#ifdef _MSC_VER
#pragma warning(disable:4200)
#endif /* _MSC_VER */

typedef struct tagINTERNALMESSAGE {
	DWORD	 dwType;
	DWORD	 dwInputLength;
    BYTE	 *InputData;
	DWORD	 dwStatus;
	DWORD	 dwOutputLength;
    BYTE	 *OutputData;
} INTERNALMESSAGE, *PINTERNALMESSAGE;

#ifdef _MSC_VER
#pragma warning(default:4200)
#endif /* _MSC_VER */



typedef struct tagUSERMODEMESSAGE
{
	DWORD			plugin;	// set zero for all plugins
	INTERNALMESSAGE	msg;
} USERMODEMESSAGE, *PUSERMODEMESSAGE;
#pragma pack(pop)

#if defined __GKNT_MES__
	#define __FP_ID__		10001
	#define __AVP_ID__		10002
	#define __ORION_ID__	10003
#endif

#if !defined (FSMGR) && !defined (__GKNT_MES__) 

#ifdef NT_DRIVER
typedef GKSTATUS (*FILTER_FILE_PROC)(PCCHAR, PCCHAR, PPLUGINMESSAGE *, BOOLEAN, BOOLEAN, PBOOLEAN, ULONG, ULONG, PETHREAD);
typedef GKSTATUS (*NOTIFICATION_PROC)();
typedef GKSTATUS (*FILTER_MOUNT_PROC)(PDEVICE_OBJECT, WCHAR, PPLUGINMESSAGE *);

typedef GKSTATUS (*ODS_FILE_PROC)(PCCHAR, PCCHAR, PPLUGINMESSAGE *, ULONG, ULONG, DWORD, DWORD, BOOLEAN);
typedef GKSTATUS (*ODS_BOOTSECTOR_PROC)(PCCHAR, PPLUGINMESSAGE *, DWORD, DWORD);
typedef GKSTATUS (*ODS_MBR_PROC)(ULONG, PPLUGINMESSAGE *, DWORD, DWORD);
typedef GKSTATUS (*ODS_MEMORY_PROC)(PPLUGINMESSAGE *, DWORD, DWORD);

typedef GKSTATUS (*MESSAGE_PROC)(PINTERNALMESSAGE);

#pragma pack(push, 1)

typedef struct {
	FILTER_FILE_PROC			FileOpen;
	FILTER_FILE_PROC			FileClose;
	FILTER_FILE_PROC			FileModified;
	FILTER_FILE_PROC			FileRename;
	FILTER_MOUNT_PROC			MountVolume;
	NOTIFICATION_PROC			SettingsModified;
	MESSAGE_PROC				InternalMessage;

	ODS_FILE_PROC				ODScanFile;
	ODS_BOOTSECTOR_PROC			ODScanBootSector;
	ODS_MBR_PROC				ODScanMBR;
	ODS_MEMORY_PROC				ODScanMemory;
} FILTER_CALLBACK, *PFILTER_CALLBACK;

typedef struct {
	ULONG					PlugInId;
	PCHAR					PlugInName;
	DWORD					Extensions;	// FS_EXTENSIONS_*
	ULONG					Version; // MAKELONG(0, (VERSION_MAJOR << 8) | VERSION_MINOR);
	ULONG					Reserved2;
	ULONG					Reserved3;
	ULONG					Reserved4;
} PLUGIN_INFO, *PPLUGIN_INFO;

#pragma pack(pop)

GKSTATUS FILTER_API FilterRegister(PDRIVER_OBJECT, PFILTER_CALLBACK, PPLUGIN_INFO, PULONG, ULONG, PVOID *);
GKSTATUS FILTER_API FilterDeregister(ULONG pluginId_, PVOID callbackHandle_);

BOOLEAN FILTER_API FilterRegisterThread(PETHREAD thread_);
BOOLEAN FILTER_API FilterDeregisterThread(PETHREAD thread_);

GKSTATUS FILTER_API GetCustomSettings(ULONG pluginId_, PBYTE *buffer_, ULONG *size_);


GKSTATUS FILTER_API SendMessage(ULONG pluginId_, ULONG flags_, PPLUGINMESSAGE message_);
GKSTATUS FILTER_API SendData(ULONG pluginId_, LONG length_, const VOID *data_);
GKSTATUS FILTER_API LogEvent(ULONG pluginId_, GKSTATUS eventCode, PCCHAR eventMessage_);
GKSTATUS FILTER_API LogError(PDRIVER_OBJECT driverObject_, GKSTATUS eventCode, PCCHAR eventMessage_);

ULONG FILTER_API GetAPIVersion();
ULONG FILTER_API GetGatekeeperVersion();
BOOLEAN FILTER_API IsAPICompatibleWithVersion(ULONG version_);

// ask Gatekeeper to rename a file
GKSTATUS FILTER_API FSRenameFile(PCCHAR oldname_, PCCHAR newname_);

// tell Gatekeeper to bypass all access to a file until told otherwise
void FILTER_API FSAllowInternalAccess(PCCHAR file_);
void FILTER_API FSRemoveInternalAccess(PCCHAR file_);

VOID     FILTER_API FlushCache();

#endif //NT_DRIVER

#ifdef VTOOLSD
#define STATUS_REPORT                           ERROR_ACCESS_DENIED
#else
#define STATUS_REPORT                           STATUS_ACCESS_DENIED
#endif
#define STATUS_SUCCESS_ABORT_FURTHER_SCANS          ((GKSTATUS)0x00000123L)
#define STATUS_ERROR_PROCESSING_FILE				((GKSTATUS)0x00000124L)

// policy reading functions
DfPolicyStatus FSGetPolicyInteger(PCCHAR oid_, DWORD *value_);
DfPolicyStatus FSGetPolicyString(PCCHAR oid_, PCHAR *value_);

#endif // FSMGR

#ifdef __cplusplus
}
#endif

#endif __GKAPI_H__
