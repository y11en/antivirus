//+ vista transaction stuff --------------------------------------------------------------
//#if FLT_MGR_LONGHORN

typedef NTSTATUS
(FLTAPI *PFLT_TRANSACTION_NOTIFICATION_CALLBACK) (
	__in PCFLT_RELATED_OBJECTS FltObjects,
	__in PFLT_CONTEXT TransactionContext,
	__in ULONG NotificationMask
	);

//#endif // FLT_MGR_LONGHORN

typedef struct _FLT_REGISTRATION_VISTA {

	//
	//  The size, in bytes, of this registration structure.
	//

	USHORT Size;
	USHORT Version;

	//
	//  Flag values
	//

	FLT_REGISTRATION_FLAGS Flags;

	//
	//  Variable length array of routines that are used to manage contexts in
	//  the system.
	//

	CONST FLT_CONTEXT_REGISTRATION *ContextRegistration;

	//
	//  Variable length array of routines used for processing pre- and post-
	//  file system operations.
	//

	CONST FLT_OPERATION_REGISTRATION *OperationRegistration;

	//
	//  This is called before a filter is unloaded.  If an ERROR or WARNING
	//  status is returned then the filter is NOT unloaded.  A mandatory unload
	//  can not be failed.
	//
	//  If a NULL is specified for this routine, then the filter can never be
	//  unloaded.
	//

	PFLT_FILTER_UNLOAD_CALLBACK FilterUnloadCallback;

	//
	//  This is called to see if a filter would like to attach an instance
	//  to the given volume.  If an ERROR or WARNING status is returned, an
	//  attachment is not made.
	//
	//  If a NULL is specified for this routine, the attachment is always made.
	//

	PFLT_INSTANCE_SETUP_CALLBACK InstanceSetupCallback;

	//
	//  This is called to see if the filter wants to detach from the given
	//  volume.  This is only called for manual detach requests.  If an
	//  ERROR or WARNING status is returned, the filter is not detached.
	//
	//  If a NULL is specified for this routine, then instances can never be
	//  manually detached.
	//

	PFLT_INSTANCE_QUERY_TEARDOWN_CALLBACK InstanceQueryTeardownCallback;

	//
	//  This is called at the start of a filter detaching from a volume.
	//
	//  It is OK for this field to be NULL.
	//

	PFLT_INSTANCE_TEARDOWN_CALLBACK InstanceTeardownStartCallback;

	//
	//  This is called at the end of a filter detaching from a volume.  All
	//  outstanding operations have been completed by the time this routine
	//  is called.
	//
	//  It is OK for this field to be NULL.
	//

	PFLT_INSTANCE_TEARDOWN_CALLBACK InstanceTeardownCompleteCallback;

	//
	//  The following callbacks are provided by a filter only if it is
	//  interested in modifying the name space.
	//
	//  If NULL is specified for these callbacks, it is assumed that the
	//  filter would not affect the name being requested.
	//

	PFLT_GENERATE_FILE_NAME GenerateFileNameCallback;

	PFLT_NORMALIZE_NAME_COMPONENT NormalizeNameComponentCallback;

	PFLT_NORMALIZE_CONTEXT_CLEANUP NormalizeContextCleanupCallback;

	//
	//  The PFLT_NORMALIZE_NAME_COMPONENT_EX callback is also a name
	//  provider callback. It is not included here along with the
	//  other name provider callbacks to take care of the registration
	//  structure versioning issues.
	//

	//#if FLT_MGR_LONGHORN

	//
	//  This is called for transaction notifications received from the KTM
	//  when a filter has enlisted on that transaction.
	//

	PFLT_TRANSACTION_NOTIFICATION_CALLBACK TransactionNotificationCallback;

	//
	//  This is the extended normalize name component callback
	//  If a mini-filter provides this callback, then  this callback
	//  will be used as opposed to using PFLT_NORMALIZE_NAME_COMPONENT
	//
	//  The PFLT_NORMALIZE_NAME_COMPONENT_EX provides an extra parameter
	//  (PFILE_OBJECT) in addition to the parameters provided to
	//  PFLT_NORMALIZE_NAME_COMPONENT. A mini-filter may use this parameter
	//  to get to additional information like the TXN_PARAMETER_BLOCK.
	//
	//  A mini-filter that has no use for the additional parameter may
	//  only provide a PFLT_NORMALIZE_NAME_COMPONENT callback.
	//
	//  A mini-filter may provide both a PFLT_NORMALIZE_NAME_COMPONENT
	//  callback and a PFLT_NORMALIZE_NAME_COMPONENT_EX callback. The
	//  PFLT_NORMALIZE_NAME_COMPONENT_EX callback will be used by fltmgr
	//  versions that understand this callback (Vista RTM and beyond)
	//  and PFLT_NORMALIZE_NAME_COMPONENT callback will be used by fltmgr
	//  versions that do not understand the PFLT_NORMALIZE_NAME_COMPONENT_EX
	//  callback (prior to Vista RTM). This allows the same mini-filter
	//  binary to run with all versions of fltmgr.
	//

	PFLT_NORMALIZE_NAME_COMPONENT_EX NormalizeNameComponentExCallback;

	//#endif // FLT_MGR_LONGHORN

} FLT_REGISTRATION_VISTA, *PFLT_REGISTRATION_VISTA;

#define FLT_MAX_TRANSACTION_NOTIFICATIONS \
	(TRANSACTION_NOTIFY_PREPREPARE | \
	TRANSACTION_NOTIFY_PREPARE | \
	TRANSACTION_NOTIFY_COMMIT | \
	TRANSACTION_NOTIFY_ROLLBACK)
//- vista transaction stuff --------------------------------------------------------------

//
//  Globals
//

typedef struct _GLOBAL_DATA {

    //  Handle to minifilter returned from FltRegisterFilter()
    PFLT_FILTER			m_Filter;

	PFLT_INSTANCE		m_SystemVolumeInstance;

    //  Driver object for this filter
    PDRIVER_OBJECT		m_FilterDriverObject;

    //  Control Device Object for this filter
    PDEVICE_OBJECT		m_FilterControlDeviceObject;

	// sync to global data
    ERESOURCE			m_Resource;
	ERESOURCE			m_ResourceFileNameInfo;
	ERESOURCE			m_FidBoxLock;
	
	ERESOURCE			m_TimingLock;
	LIST_ENTRY			m_Timing;

	ULONG				m_DrvFlags;
	ULONG				m_FilterEventEntersCount;
	ULONG				m_ReplyWaiterEnters;

    PFLT_PORT			m_ServerPort; // comm
	UNICODE_STRING		m_RegParams;
	UNICODE_STRING		m_RegInstances;

	// workitems
	LONG				m_WorkItemsInWork;

	UNICODE_STRING		m_ControlDeviceObjectSymLink;
} GLOBAL_DATA, *PGLOBAL_DATA;

extern GLOBAL_DATA Globals;

//  The name of the KLIF created by this filter

#define CONTROL_DEVICE_OBJECT_PREFIX	L"\\FileSystem\\Filters"
#define CONTROL_DEVICE_OBJECT_NAME		L"\\KLIF"
#define LINK_DEVICE_OBJECT_PREFIX		L"\\DosDevices"
#define LINK_DEVICE_OBJECT_NAME			L"\\KLIF"

//
//  Macro to test if this is my control device object
//

#define IS_MY_CONTROL_DEVICE_OBJECT(_devObj) \
    (((_devObj) == Globals.m_FilterControlDeviceObject) ? \
            (ASSERT(((_devObj)->DriverObject == Globals.m_FilterDriverObject) && \
                    ((_devObj)->DeviceExtension == NULL)), TRUE) : \
            FALSE)

//+ ----------------------------------------------------------------------------------------
#define FILEID_ASSIGN_NONE(_x)		{_x.HighPart = 0; _x.LowPart = 0;}


#define _STREAM_FLAGS_NONE				0x0000
#define _STREAM_FLAGS_DIRECTORY			0x0001
#define _STREAM_FLAGS_MODIFIED			0x0002
#define _STREAM_FLAGS_TRANSACTED		0x0004

#define _STREAM_HANDLE_FLAGS_FOR_BACKUP		0x0001
#define _STREAM_HANDLE_FLAGS_FOR_READ		0x0002
#define _STREAM_HANDLE_FLAGS_FOR_WRITE		0x0004
#define _STREAM_HANDLE_FLAGS_FOR_EXECUTE	0x0008
#define _STREAM_HANDLE_FLAGS_FOR_DELETE		0x0010
#define _STREAM_HANDLE_NEW_OBJECT			0x0020
#define _STREAM_HANDLE_FLAGS_DELETE_PENDING	0x0040
#define _STREAM_HANDLE_FLAGS_PREFETCH		0x0100

#include <pshpack1.h>

typedef struct _MKAV_OBJ_ID
{
	BYTE						m_ObjectId[16];
}MKAV_OBJ_ID, *PMKAV_OBJ_ID;

//------------------------------------------------------------------
typedef struct _MKAV_ENLISTED_FILE
{
	ULONG				m_ProcessId;
	USHORT				m_NameLen;
	WCHAR				m_FileName[0];
} MKAV_ENLISTED_FILE, *PMKAV_ENLISTED_FILE;

typedef struct _MKAV_ENLISTED_KEY
{
	ULONG				m_ProcessId;
	USHORT				m_NameLen;
	WCHAR				m_KeyPath[0];
} MKAV_ENLISTED_KEY, *PMKAV_ENLISTED_KEY;

typedef struct _MKAV_TRANSACTION_CONTEXT
{
	PVOID				m_pTransObj;
	ERESOURCE			m_FileLock;
	ERESOURCE			m_RegLock;
	RTL_GENERIC_TABLE	m_EnlistedFilesTable;
	RTL_GENERIC_TABLE	m_EnlistedKeysTable;
} MKAV_TRANSACTION_CONTEXT, *PMKAV_TRANSACTION_CONTEXT;
//------------------------------------------------------------------

typedef struct _MKAV_INSTANCE_CONTEXT
{
    DEVICE_TYPE					m_DeviceType;
	ULONG						m_ContextFlags;
	ULONG						m_SectorSize;
	UNICODE_STRING				m_VolumeName;
	MKAV_OBJ_ID					m_VolumeId;
} MKAV_INSTANCE_CONTEXT, *PMKAV_INSTANCE_CONTEXT;

typedef struct _MKAV_VOLUME_CONTEXT
{
	PFLT_INSTANCE				m_pFltInstance;
} MKAV_VOLUME_CONTEXT, *PMKAV_VOLUME_CONTEXT;

typedef struct _MKAV_STREAM_CONTEXT
{
	PFLT_FILE_NAME_INFORMATION	m_pFileNameInfo;
	PMKAV_OBJ_ID				m_pFileId;
	ULONG						m_Flags;
	LARGE_INTEGER				m_InternalId;
	ULONG						m_NumberOfLinks;
} MKAV_STREAM_CONTEXT, *PMKAV_STREAM_CONTEXT;

typedef struct _MKAV_STREAM_HANDLE_CONTEXT
{
	LUID						m_Luid;
	PSID						m_pSid;
	ULONG						m_Flags;
} MKAV_STREAM_HANDLE_CONTEXT, *PMKAV_STREAM_HANDLE_CONTEXT;

#include <poppack.h>

//+ ------------------------------------------------------------------------------------------
