// 
//#define _Z_DEBUG_PRINT

#ifdef _Z_DEBUG_PRINT
#define zDbgPrint DbgPrint
#else
#define zDbgPrint
#endif //_AVZ_DEBUG_PRINT

#define PROCESS_TERMINATE         (0x0001)  
#define PROCESS_VM_WRITE          (0x0020)  

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
typedef int                 BOOL,  *PBOOL,  **PPBOOL;
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

// ********** Коды управления ************
// Указание главного безопасного процесса
#define IOCTL_Z_SET_TRUSTEDPROCESS     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Добавление процесса в список доверенных
#define IOCTL_Z_ADD_ALTTRUSTEDPROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x811, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Удаление процесса из списка доверенных
#define IOCTL_Z_DEL_ALTTRUSTEDPROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x812, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Очистка списка доверенных приложений
#define IOCTL_Z_CLR_ALTTRUSTEDPROCESS  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x814, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Разрешение/запрет выгрузки драйвера
#define IOCTL_Z_ENABLEULNOAD           CTL_CODE(FILE_DEVICE_UNKNOWN, 0x815, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Включение/выключение RestritedMode
#define IOCTL_Z_RESTRICTEDMODE         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x816, METHOD_BUFFERED, FILE_ANY_ACCESS)

// ZwCreateFile
typedef NTSTATUS 
(NTAPI *PZwCreateFile) (
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN PLARGE_INTEGER  AllocationSize  OPTIONAL,
    IN ULONG  FileAttributes,
    IN ULONG  ShareAccess,
    IN ULONG  CreateDisposition,
    IN ULONG  CreateOptions,
    IN PVOID  EaBuffer  OPTIONAL,
    IN ULONG  EaLength
    );

// ZwCreateKey
typedef NTSTATUS
(NTAPI  *PZwCreateKey) (
    OUT PHANDLE  KeyHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    IN ULONG  TitleIndex,
    IN PUNICODE_STRING  Class  OPTIONAL,
    IN ULONG  CreateOptions,
    OUT PULONG  Disposition  OPTIONAL
    );

// ZwOpenKey
typedef NTSTATUS
  (NTAPI  *PZwOpenKey) (
    OUT PHANDLE  KeyHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes
    );

// ZwSetValueKey
typedef NTSTATUS
  (NTAPI  *PZwSetValueKey) (
    IN HANDLE  KeyHandle,
    IN PUNICODE_STRING  ValueName,
    IN ULONG  TitleIndex  OPTIONAL,
    IN ULONG  Type,
    IN PVOID  Data,
    IN ULONG  DataSize
    );

// ZwDeleteValueKey
typedef NTSTATUS
  (NTAPI  *PZwDeleteValueKey) (
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName
    );


// ZwOpenProcess
typedef NTSTATUS 
 (NTAPI *PZwOpenProcess)( 
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN PCLIENT_ID ClientId OPTIONAL 
); 

// ZwCreateProcess
typedef NTSTATUS
(NTAPI *PZwCreateProcess) (
   OUT PHANDLE ProcessHandle,
   IN ACCESS_MASK DesiredAccess,
   IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
   IN HANDLE ParentProcess,
   IN BOOLEAN InheritObjectTable,
   IN HANDLE SectionHandle OPTIONAL,
   IN HANDLE DebugPort OPTIONAL,
   IN HANDLE ExceptionPort OPTIONAL
);

// ZwCreateProcessEx
typedef NTSTATUS 
(NTAPI *PZwCreateProcessEx)(
    OUT PHANDLE ProcessHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
    IN HANDLE ParentProcess,
    IN BOOLEAN InheritObjectTable,
    IN HANDLE SectionHandle OPTIONAL,
    IN HANDLE DebugPort OPTIONAL,
    IN HANDLE ExceptionPort OPTIONAL,
    IN HANDLE Unknown OPTIONAL); 

// ZwTerminateProcess 
typedef NTSTATUS
(NTAPI *PZwOpenSection)(
    OUT PHANDLE  SectionHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes
    );

// ZwOpenFile 
typedef NTSTATUS
(NTAPI *PZwOpenFile)(
    OUT PHANDLE  FileHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes,
    OUT PIO_STATUS_BLOCK  IoStatusBlock,
    IN ULONG  ShareAccess,
    IN ULONG  OpenOptions
    );

// ZwOpenThread
typedef NTSTATUS 
(NTAPI *PZwOpenThread)(
     OUT PHANDLE ThreadHandle,
	 IN ACCESS_MASK DesiredAccess,
	 OUT POBJECT_ATTRIBUTES ObjectAttributes,
	 IN PCLIENT_ID ClientId);

// ZwWriteVirtualMemory
typedef NTSTATUS 
(NTAPI *PZwWriteVirtualMemory)(
     IN HANDLE ProcessHandle,
	 IN PVOID BaseAddress,
	 IN PVOID Buffer,
	 IN ULONG BufferLength,
	 OUT PULONG ReturnLength OPTIONAL
);

// ZwTerminateJobObject
typedef NTSTATUS 
(NTAPI *PZwTerminateJobObject)(  
	IN HANDLE  JobHandle,
	IN NTSTATUS  ExitStatus 
 );

typedef NTSTATUS
(NTAPI *PZwCreateJobObject)(
   OUT PHANDLE JobHandle,
   IN ACCESS_MASK DesiredAccess,
   IN POBJECT_ATTRIBUTES ObjectAttributes
);

typedef NTSTATUS
(NTAPI *PZwAssignProcessToJobObject)(
  IN HANDLE JobHandle,
  IN HANDLE ProcessHandle
);

typedef enum _JOBOBJECTINFOCLASS {
    JobObjectBasicAccountingInformation = 1,
    JobObjectBasicLimitInformation,
    JobObjectBasicProcessIdList,
    JobObjectBasicUIRestrictions,
    JobObjectSecurityLimitInformation,
    JobObjectEndOfJobTimeInformation,
    JobObjectAssociateCompletionPortInformation,
    JobObjectBasicAndIoAccountingInformation,
    JobObjectExtendedLimitInformation,
    JobObjectJobSetInformation,
    MaxJobObjectInfoClass
    } JOBOBJECTINFOCLASS;


#define JOB_OBJECT_UILIMIT_NONE             0x00000000

#define JOB_OBJECT_UILIMIT_HANDLES          0x00000001
#define JOB_OBJECT_UILIMIT_READCLIPBOARD    0x00000002
#define JOB_OBJECT_UILIMIT_WRITECLIPBOARD   0x00000004
#define JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS 0x00000008
#define JOB_OBJECT_UILIMIT_DISPLAYSETTINGS  0x00000010
#define JOB_OBJECT_UILIMIT_GLOBALATOMS      0x00000020
#define JOB_OBJECT_UILIMIT_DESKTOP          0x00000040
#define JOB_OBJECT_UILIMIT_EXITWINDOWS      0x00000080

#define JOB_OBJECT_UILIMIT_ALL              0x000000FF


typedef NTSTATUS
(NTAPI *PZwSetInformationJobObject)(
   IN HANDLE JobHandle,
   IN JOBOBJECTINFOCLASS JobInformationClass,
   IN PVOID JobInformation,
   IN ULONG JobInformationLength
);
