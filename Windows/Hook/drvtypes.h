#ifndef __KLDRV_TYPES_
#define __KLDRV_TYPES_

#ifndef DWORD
	#define DWORD	ULONG
#endif

//+ ----------------------------------------------------------------------------------------

#ifndef BYTE
	#define BYTE	UCHAR
#endif

//+ ----------------------------------------------------------------------------------------

#ifndef WORD
	#define WORD	USHORT
#endif

//+ ----------------------------------------------------------------------------------------

#ifndef SIZE_T
	typedef unsigned __int64 SIZE_T;
#endif

#ifdef _HOOK_NT_
//+ ----------------------------------------------------------------------------------------

#ifndef _IMAGE_INFO
	typedef struct _IMAGE_INFO {
		union {
			ULONG Properties;
			struct {
				ULONG ImageAddressingMode  : 8;  // code addressing mode
				ULONG SystemModeImage      : 1;  // system mode image
				ULONG ImageMappedToAllPids : 1;  // image mapped into all processes
				ULONG Reserved             : 22;
			};
		};
		PVOID       ImageBase;
		ULONG       ImageSelector;
		SIZE_T      ImageSize;
		ULONG       ImageSectionNumber;
	} IMAGE_INFO, *PIMAGE_INFO;
#endif //_IMAGE_INFO

//+ ----------------------------------------------------------------------------------------

#ifndef PLOAD_IMAGE_NOTIFY_ROUTINE
	typedef VOID (*PLOAD_IMAGE_NOTIFY_ROUTINE)(
    IN PUNICODE_STRING FullImageName,
    IN HANDLE ProcessId,                // pid into which image is being mapped
    IN PIMAGE_INFO ImageInfo
    );
#endif //PLOAD_IMAGE_NOTIFY_ROUTINE

//+ ----------------------------------------------------------------------------------------

#ifndef _SYSTEM_MODULE_INFORMATION_DEFINED
#define _SYSTEM_MODULE_INFORMATION_DEFINED

#include <pshpack1.h>
typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG		Reserved[2];
	PVOID		Base;
	ULONG		Size;
	ULONG		Flags;
	USHORT		Index;
	USHORT		Unknown;
	USHORT		LoadCount;
	USHORT		ModNameOffset;
	CHAR		ImageName[256];
}SYSTEM_MODULE_INFORMATION,*PSYSTEM_MODULE_INFORMATION;

typedef struct _SYS_MOD_INFO {
	ULONG	NumberOfModules;
	SYSTEM_MODULE_INFORMATION Module[1];
}SYS_MOD_INFO,*PSYS_MOD_INFO;
#include <poppack.h>

#endif //_SYSTEM_MODULE_INFORMATION_DEFINED

#include <pshpack1.h>
typedef struct _MY_QUERY_DIRECTORY_STRUCT {
	ULONG	Index;
	ULONG	Retlen;
	CHAR	Buffer[0x200];
}MY_QUERY_DIRECTORY_STRUCT,*PMY_QUERY_DIRECTORY_STRUCT;

typedef struct _OBJECT_NAMETYPE_INFO {
	UNICODE_STRING ObjectName;
	UNICODE_STRING ObjectType;
}OBJECT_NAMETYPE_INFO, *POBJECT_NAMETYPE_INFO;
#include <poppack.h>

#ifndef PROCESS_BASIC_INFORMATION
typedef struct _PROCESS_BASIC_INFORMATION {
    NTSTATUS ExitStatus;
    PPEB PebBaseAddress;
    ULONG AffinityMask;						// ULONG_PTR
    KPRIORITY BasePriority;
    ULONG UniqueProcessId;					// ULONG_PTR
    ULONG InheritedFromUniqueProcessId;		// ULONG_PTR
} PROCESS_BASIC_INFORMATION;	

typedef PROCESS_BASIC_INFORMATION *PPROCESS_BASIC_INFORMATION;

#endif	//_PROCESS_BASIC_INFORMATION

#ifndef _PROCESSINFOCLASS
typedef enum _PROCESSINFOCLASS {
	ProcessBasicInformation,
    ProcessQuotaLimits,
    ProcessIoCounters,
    ProcessVmCounters,
    ProcessTimes,
    ProcessBasePriority,
    ProcessRaisePriority,
    ProcessDebugPort,
    ProcessExceptionPort,
    ProcessAccessToken,
    ProcessLdtInformation,
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    ProcessIoPortHandlers,          // Note: this is kernel mode only
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    ProcessUserModeIOPL,
    ProcessEnableAlignmentFaultFixup,
    ProcessPriorityClass,
    ProcessWx86Information,
    ProcessHandleCount,
    ProcessAffinityMask,
    ProcessPriorityBoost,
    ProcessDeviceMap,
    ProcessSessionInformation,
    ProcessForegroundInformation,
    ProcessWow64Information,
    MaxProcessInfoClass
};
#endif //_PROCESSINFOCLASS


#define LPC_CLIENT_ID CLIENT_ID
#define LPC_SIZE_T SIZE_T

#define SEC_IMAGE         0x1000000     


typedef struct _PORT_MESSAGE {
    union {
        struct {
            CSHORT DataLength;
            CSHORT TotalLength;
        } s1;
        ULONG Length;
    } u1;
    union {
        struct {
            CSHORT Type;
            CSHORT DataInfoOffset;
        } s2;
        ULONG ZeroInit;
    } u2;
    union {
        LPC_CLIENT_ID ClientId;
        double DoNotUseThisField;       // Force quadword alignment
    };
    ULONG MessageId;
    union {
        LPC_SIZE_T ClientViewSize;          // Only valid on LPC_CONNECTION_REQUEST message
        ULONG CallbackId;                   // Only valid on LPC_REQUEST message
    };
	//  UCHAR Data[];
} PORT_MESSAGE, *PPORT_MESSAGE;


typedef struct _CSRSS_MESSAGE {
    ULONG Unknown1;
    ULONG Opcode;
    ULONG Status;
    ULONG Unknown2;
}CSRSS_MESSAGE;


typedef struct _PROCESS_INFORMATION {
    HANDLE hProcess;
    HANDLE hThread;
    DWORD dwProcessId;
    DWORD dwThreadId;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef struct _impPROCESS_PARAMETERS
{
	ULONG AllocationSize;
	ULONG Size;
	ULONG Flags;
	ULONG reserved1;
	LONG Console;
	ULONG ProcessGroup;
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
	UNICODE_STRING CurrentDirectoryName;
	HANDLE CurrentDirectoryHandle;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImageFile;
	UNICODE_STRING CommandLine;
	PWSTR Environment;
	ULONG dwX;
	ULONG dwY;
	ULONG dwXSize;
	ULONG dwYSize;
	ULONG dwXCountChars;
	ULONG dwYCountChars;
	ULONG dwFillAttribite;
	ULONG dwFlags;
	ULONG wShowWindow;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING Desktop;
	UNICODE_STRING reserved2;
	UNICODE_STRING reserved3;
}impPROCESS_PARAMETERS;

typedef struct _SECTION_IMAGE_INFORMATION {
    PVOID   EntryPoint;
    ULONG   Unknown1;
    ULONG   StackReserve;
    ULONG   StackCommit;
    ULONG   Subsystem;
    USHORT  MinorSubsystemVersion;
    USHORT  MajorSubsystemVersion;
    ULONG   Unknown2;
    ULONG   Characteristics;
    USHORT  ImageNumber;
    BOOLEAN Executable;
    UCHAR   Unknown3;
    ULONG   Unknown4[3];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;

#endif //_HOOK_NT_

#endif //__KLDRV_TYPES_


