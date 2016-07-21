//#define _AVZ_DEBUG_PRINT

#ifdef _AVZ_DEBUG_PRINT
#define zDbgPrint DbgPrint
#else
#define zDbgPrint
#endif //_AVZ_DEBUG_PRINT


// Контекст устройства
typedef struct _DEVICE_CONTEXT
    {
    PDRIVER_OBJECT pDriverObject;
    PDEVICE_OBJECT pDeviceObject;
    }
    DEVICE_CONTEXT, *PDEVICE_CONTEXT, **PPDEVICE_CONTEXT;

#define DEVICE_CONTEXT_ sizeof (DEVICE_CONTEXT)


// Структуры для работы с SDT
typedef UCHAR               BYTE,  *PBYTE,  **PPBYTE;
typedef USHORT              WORD,  *PWORD,  **PPWORD;
typedef ULONG               DWORD, *PDWORD, **PPDWORD;
typedef NTSTATUS (NTAPI *NTPROC) ();
typedef NTPROC *PNTPROC;

// Структуры для работы с SDT (с прагмой pack(1) !)
#pragma pack(1)
typedef struct _SYSTEM_SERVICE_TABLE
        {
		PNTPROC ServiceTable;           // array of entry points
		PDWORD  CounterTable;           // array of usage counters
		DWORD   ServiceLimit;           // number of table entries
		PBYTE   ArgumentTable;          // array of byte counts
		}
        SYSTEM_SERVICE_TABLE, * PSYSTEM_SERVICE_TABLE, **PPSYSTEM_SERVICE_TABLE;

#define SYSTEM_SERVICE_TABLE_ sizeof (SYSTEM_SERVICE_TABLE)

// -----------------------------------------------------------------


#define _CSTRING(_text) #_text
#define CSTRING(_text) _CSTRING (_text)

#define _USTRING(_text) L##_text
#define USTRING(_text) _USTRING (_text)

#define PRESET_UNICODE_STRING(_symbol,_buffer) \
        UNICODE_STRING _symbol = \
            { \
            sizeof (USTRING (_buffer)) - sizeof (WORD), \
            sizeof (USTRING (_buffer)), \
            USTRING (_buffer) \
            };


typedef struct _SERVICE_DESCRIPTOR_TABLE
        {
		SYSTEM_SERVICE_TABLE ntoskrnl;  // ntoskrnl.exe (native api)
		SYSTEM_SERVICE_TABLE win32k;    // win32k.sys   (gdi/user)
		SYSTEM_SERVICE_TABLE Table3;    // not used
		SYSTEM_SERVICE_TABLE Table4;    // not used
		} SERVICE_DESCRIPTOR_TABLE, *PSERVICE_DESCRIPTOR_TABLE, **PPSERVICE_DESCRIPTOR_TABLE;

#define SERVICE_DESCRIPTOR_TABLE_ sizeof (SERVICE_DESCRIPTOR_TABLE)
#pragma pack()

typedef NTSTATUS (NTAPI *NTPROC) ();
typedef NTPROC *PNTPROC;

#pragma pack(1)
// Элемент списка процессов
typedef struct _PROCESS_ITEM
        {
			DWORD  PID;       // Идентификатор процесса
			PVOID  EPROCESS;  // Указатель на структуру EPROCESS процесса
			UCHAR  NAME[16];  // Имя из PEB
			UNICODE_STRING FULL_NAME; // Полное имя EXE файла
		} PROCESS_ITEM;
#pragma pack()

// Элемент списка драйверов
typedef struct _DRIVER_LIST_ITEM
        {
			PVOID       ImageBase;		// Базовый адрес
			SIZE_T      ImageSize;		// Размер образа
			UNICODE_STRING FULL_NAME;	// Полное имя EXE файла
			PVOID		PrevItem, NextItem;		// Указатель на последующий элемент (NULL - элемент последний в списке)
		} DRIVER_LIST_ITEM, *PDRIVER_LIST_ITEM;
#pragma pack()

typedef enum _SYSTEM_INFORMATION_CLASS 
{
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation, 
	SystemNotImplemented1,
	SystemProcessesAndThreadsInformation,
	SystemCallCounts, 
	SystemConfigurationInformation, 
	SystemProcessorTimes, 
	SystemGlobalFlag, 
	SystemNotImplemented2, 
	SystemModuleInformation, 
	SystemLockInformation,
	SystemNotImplemented3, 
	SystemNotImplemented4, 
	SystemNotImplemented5, 
	SystemHandleInformation, 
	SystemObjectInformation, 
	SystemPagefileInformation, 
	SystemInstructionEmulationCounts, 
	SystemInvalidInfoClass1, 
	SystemCacheInformation, 
	SystemPoolTagInformation, 
	SystemProcessorStatistics,
	SystemDpcInformation, 
	SystemNotImplemented6,
	SystemLoadImage, 
	SystemUnloadImage, 
	SystemTimeAdjustment, 
	SystemNotImplemented7, 
	SystemNotImplemented8, 
	SystemNotImplemented9,
	SystemCrashDumpInformation, 
	SystemExceptionInformation, 
	SystemCrashDumpStateInformation, 
	SystemKernelDebuggerInformation, 
	SystemContextSwitchInformation, 
	SystemRegistryQuotaInformation, 
	SystemLoadAndCallImage,
	SystemPrioritySeparation, 
	SystemNotImplemented10,
	SystemNotImplemented11, 
	SystemInvalidInfoClass2, 
	SystemInvalidInfoClass3, 
	SystemTimeZoneInformation, 
	SystemLookasideInformation, 
	SystemSetTimeSlipEvent,
	SystemCreateSession,
	SystemDeleteSession, 
	SystemInvalidInfoClass4, 
	SystemRangeStartInformation, 
	SystemVerifierInformation, 
	SystemAddVerifier, 
	SystemSessionProcessesInformation 
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	ULONG  Reserved[2];
	PVOID  Base;
	ULONG  Size;
	ULONG  Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR   ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _KAPC_STATE {
  LIST_ENTRY ApcListHead[2];
  PVOID Process;
  BOOLEAN KernelApcInProgress;
  BOOLEAN KernelApcPending;
  BOOLEAN UserApcPending;
} KAPC_STATE, *PKAPC_STATE;

// ZwQueryInformationProcess
typedef NTSTATUS 
(NTAPI *PZwQueryInformationProcess) (
									 IN HANDLE ProcessHandle,
									 IN PROCESSINFOCLASS ProcessInformationClass,
									 OUT PVOID ProcessInformation,
									 IN ULONG ProcessInformationLength,
									 OUT PULONG ReturnLength OPTIONAL
									 );

// ZwQuerySystemInformation
typedef NTSTATUS 
(NTAPI *PZwQuerySystemInformation)(
           IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
           IN OUT PVOID SystemInformation,
           IN ULONG SystemInformationLength,
           OUT PULONG ReturnLength OPTIONAL);

// ObOpenObjectByPointer
typedef NTSTATUS
(NTAPI *PObOpenObjectByPointer)(
    IN PVOID  Object,
    IN ULONG  HandleAttributes,
    IN PACCESS_STATE  PassedAccessState OPTIONAL,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_TYPE  ObjectType,
    IN KPROCESSOR_MODE  AccessMode,
    OUT PHANDLE  Handle
    );

// ZwOpenProcess
typedef NTSTATUS 
 (NTAPI *PZwOpenProcess)( 
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL 
); 

// ZwTerminateProcess
typedef NTSTATUS
 (NTAPI *PZwTerminateProcess) (
 IN HANDLE ProcessHandle OPTIONAL,
 IN NTSTATUS ExitStatus
 );

typedef NTSTATUS
 (NTAPI *PPsRemoveLoadImageNotifyRoutine) (
 IN PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine
 );

typedef NTSTATUS
(NTAPI *PPsSetLoadImageNotifyRoutine)(
    IN PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine
    );

typedef NTSTATUS
 (NTAPI *PRtlGetVersion)(
    IN OUT PRTL_OSVERSIONINFOW  lpVersionInformation
    );

// ************* Коды управления **************
#define IOCTL_Z_GET_PROCESSLIST			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_EPROCESS_PID		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_EPROCESS_NAME		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_EPROCESS_FULLNAME	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x804, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_KILL_PROCESS			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x805, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_Z_GET_DRIVER_LIST			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x806, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_GET_DRIVER_FULLNAME		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x807, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_PING					CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_KILL_PROCESS_BY_PID		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_Z_SETTRUSTEDPID			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Z_ENABLEULNOAD			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)

