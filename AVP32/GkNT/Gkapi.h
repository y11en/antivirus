#ifndef __GKAPI_H__
#define __GKAPI_H__

#ifndef __FILTER_DRIVER_SOURCE__
#define FILTER_API _declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"  {
#endif


// what should be done with the message in user-mode
#define FS_MSG_DEFAULT  0x0		// default behaviour (content is normal GKAPI message)
#define	FS_MSG_DISPLAY	0x1		// display the message (content is normal GKAPI message)
#define FS_MSG_LOG		0x2		// log the message (content is normal GKAPI message)
#define FS_MSG_PRIVATE	0x100	// pass message to plug-in dll (content may be anything)

// plug-in message type
#define FS_MSG_TYPE_STARTUP		0x1		// startup messages
#define FS_MSG_TYPE_SHUTDOWN	0x2		// shutdown messages
#define FS_MSG_TYPE_ERROR		0x3		// error messages (database missingm etc.)
#define FS_MSG_TYPE_WARNING		0x4		// warning messages 
#define FS_MSG_TYPE_ALERT		0x5		// normal messages (access denied, etc.)
#define FS_MSG_TYPE_OID			0x6		// OID registration

// actions
#define ACTION_NONE							0x0
#define ACTION_ACCESS_DENIED                0x1
#define ACTION_ACCESS_DENIED_DISINFECTED    0x2
#define ACTION_ACCESS_DENIED_RENAMED        0x3
#define ACTION_ACCESS_DENIED_REMOVED        0x4
#define ACTION_ACCESS_DENIED_PROMPT_USER    0x5

// predefined sizes
#define MAX_FILENAME_SIZE				512    // plug-in name size
#define MAX_PLUGIN_NAME                 256    // plug-in name size
#define MAX_SID_SIZE                    256    // SID string size 
#define MAX_MESSAGE_SIZE               1024    // message text size
#define MAX_DATA_SIZE                  1024    // data size
#define MAX_OPTIONAL_PARAMETER_SIZE     256    // optional parameter size


// versions
#define GATEKEEPER_API_VERSION  0x0100	// 1.0
#define PLUGIN_API_VERSION      0x0100	// 1.0
#define GATEKEEPER_MSG_VERSION  0x0100	// 1.0
#define GATEKEEPER_VERSION		0x0401	// 4.01

#pragma pack(push, 1)

typedef ULONG	DWORD;
typedef BYTE *	PBYTE;

typedef struct tagPLUGINMESSAGE {
	// version & size - attribute of all the structures
	DWORD dwSize;       // sizeof(PLUGINMESSAGE)
	DWORD dwVersion;    // version (PLUGIN_API_VERSION)

	// data filled by plug-in driver
	DWORD dwAction;                                             // action taken (ACTION_*)
	DWORD dwType;                                               // message type (FS_MSG_TYPE_*)
	DWORD dwSequentialNumber;                                   // sequential message number
	DWORD dwLocalizationId;                                     // message ID for localization
	CHAR pszMessageText[MAX_MESSAGE_SIZE];                     // message text
	DWORD dwOptionalParameterSize;                              // optional parameter size
	BYTE  pbOptionalParameter[MAX_OPTIONAL_PARAMETER_SIZE];     // optional parameter
} PLUGINMESSAGE, *PPLUGINMESSAGE;


// GKMESSAGE structure is passed to FSMGR by GK
typedef struct tagGKMESSAGE {
	// version & size - attribute of all the structures
	DWORD dwSize;      // sizeof(GKMESSAGE)
	DWORD dwVersion;	 // version (GKMESSAGE_VERSION)

	// data filled by GK
	DWORD dwPluginId;                        // which plugin is sending information
	DWORD dwFlags;                           // type of data (FS_MSG_*)
	CHAR pszPluginName[MAX_PLUGIN_NAME];    // plug-in name
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

#pragma pack(pop)

#if defined __GKNT_MES__
	#define __FP_ID__	10001
	#define __AVP_ID__	10002
#endif

#if !defined (FSMGR) && !defined (__GKNT_MES__)

typedef NTSTATUS (*FILTER_FILE_PROC)(PCHAR, PCHAR, PPLUGINMESSAGE *, BOOLEAN, BOOLEAN, PBOOLEAN, ULONG, ULONG, PETHREAD);
typedef NTSTATUS (*NOTIFICATION_PROC)();
typedef NTSTATUS (*FILTER_MOUNT_PROC)(PDEVICE_OBJECT, WCHAR, PPLUGINMESSAGE *);

#pragma pack(push, 1)

typedef struct {
	FILTER_FILE_PROC			FileOpen;
	FILTER_FILE_PROC			FileClose;
	FILTER_FILE_PROC			FileModified;
	FILTER_FILE_PROC			FileRename;
	FILTER_MOUNT_PROC			MountVolume;
	NOTIFICATION_PROC			SettingsModified;
} FILTER_CALLBACK, *PFILTER_CALLBACK;

typedef struct {
	ULONG					PlugInId;
	PCHAR					PlugInName;
	PCHAR					FilterExtensions;
	PCHAR					ExcludeExtensions;
	ULONG					Version; // MAKELONG(0, (VERSION_MAJOR << 8) | VERSION_MINOR);
	ULONG					Reserved2;
	ULONG					Reserved3;
	ULONG					Reserved4;
} PLUGIN_INFO, *PPLUGIN_INFO;

#pragma pack(pop)


NTSTATUS FILTER_API FilterRegister(PDRIVER_OBJECT, PFILTER_CALLBACK, PPLUGIN_INFO, PULONG, ULONG, ULONG, PVOID *);
NTSTATUS FILTER_API FilterDeregister(ULONG pluginId_, PVOID callbackHandle_);

BOOLEAN FILTER_API FilterRegisterThread(PETHREAD thread_);
BOOLEAN FILTER_API FilterDeregisterThread(PETHREAD thread_);

NTSTATUS FILTER_API ChangeExtensions(PPLUGIN_INFO info_);

NTSTATUS FILTER_API SendMessage(ULONG pluginId_, ULONG flags_, PPLUGINMESSAGE message_);
NTSTATUS FILTER_API SendData(ULONG pluginId_, LONG length_, VOID *data_);
NTSTATUS FILTER_API LogEvent(ULONG pluginId_, NTSTATUS eventCode, PCHAR eventMessage_);
NTSTATUS FILTER_API LogError(PDRIVER_OBJECT driverObject_, NTSTATUS eventCode, PCHAR eventMessage_);


ULONG FILTER_API GetAPIVersion();
ULONG FILTER_API GetGatekeeperVersion();
BOOLEAN FILTER_API IsAPICompatibleWithVersion(ULONG version_);

// ask Gatekeeper to rename a file
NTSTATUS FILTER_API FSRenameFile(PCCHAR oldname_, PCCHAR newname_);

// tell Gatekeeper to bypass all access to a file until told otherwise
void FILTER_API FSAllowInternalAccess(PUCHAR file_);
void FILTER_API FSRemoveInternalAccess(PUCHAR file_);

VOID     FILTER_API FlushCache();

#define STATUS_REPORT	              				STATUS_ACCESS_DENIED
#define STATUS_SUCCESS_ABORT_FURTHER_SCANS          ((NTSTATUS)0x00000123L)
#define STATUS_ERROR_PROCESSING_FILE				((NTSTATUS)0x00000124L)


#endif // FSMGR

#ifdef __cplusplus
}
#endif

#endif __GKAPI_H__
