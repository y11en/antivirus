#ifndef OBJ_KERNEL_HANDLE
#define OBJ_KERNEL_HANDLE       OBJ_CASE_INSENSITIVE
#endif

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (HANDLE)-1
#endif

#define KL_MEM_ALLOC(_size_)        ExAllocatePoolWithTag( NonPagedPool, (_size_), COMMON_TAG )
#define KL_MEM_FREE( _mem_ )        ExFreePool ( (_mem_) )

#define ONE_SECOND_TIME				(KL_TIME)0x989680
#define HALF_SECOND_TIME			(KL_TIME)0x4C4B40

#define KL_TIME						ULONGLONG
#define PKL_TIME					PULONGLONG
#define KlGetSystemTime(_Time_)		KeQuerySystemTime((PLARGE_INTEGER)(_Time_))
#define KL_EVENT					KEVENT
#define PKL_EVENT					PKEVENT
#define KlWaitEvent(_Event_)		KeWaitForSingleObject((PVOID)(_Event_), Executive, KernelMode, FALSE, NULL)

#define KlSleep(Time)											\
{																\
LARGE_INTEGER	tt;	\
tt.QuadPart	= -1 * 10000000 * Time;							\
KeDelayExecutionThread(KernelMode, FALSE, (PLARGE_INTEGER)&tt);	\
}

NTKERNELAPI
VOID
ExSystemTimeToLocalTime (
    PLARGE_INTEGER SystemTime,
    PLARGE_INTEGER LocalTime
    );

#define KlGetTimeFields( _Year, _Month, _Day, _Hour, _Minute, _Second, _Millisecond, _Weekday ) \
{ \
    TIME_FIELDS  tt; \
    KL_TIME      t_time; \
    KlGetSystemTime( &t_time ); \
    ExSystemTimeToLocalTime( (PLARGE_INTEGER)&t_time, (PLARGE_INTEGER)&t_time ); \
    RtlTimeToTimeFields( (PLARGE_INTEGER)&t_time, &tt ); \
    _Year = tt.Year; _Month = tt.Month; _Day = tt.Day; _Hour = tt.Hour; \
    _Minute = tt.Minute; _Second = tt.Second; _Millisecond = tt.Milliseconds; _Weekday = tt.Weekday; \
}

#define KlInitializeEvent(_Event_, _Type_, _State_)		KeInitializeEvent((_Event_), (_Type_), (_State_) )
#define KlSetEvent(_Event_)								KeSetEvent((_Event_), IO_NO_INCREMENT, FALSE)
#define KlClearEvent(_Event_)							KeClearEvent((_Event_))

#define SPIN_LOCK					KSPIN_LOCK
#define IRQL                        KIRQL
#define AllocateSpinLock(_Lock_)	KeInitializeSpinLock((_Lock_))
#define AcquireSpinLock(_Lock_, _OldIrql)		KeAcquireSpinLock( (_Lock_), (_OldIrql)  )
#define ReleaseSpinLock(_Lock_, _OldIrql)		KeReleaseSpinLock( (_Lock_), (_OldIrql)  )

#define KlCompareMemory(_Dst_, _Src_, _Length_) ((_Length_) == RtlCompareMemory( (_Dst_), (_Src_), (_Length_) ))

typedef unsigned long	DWORD,	*PDWORD;
typedef unsigned long	ULONG,	*PULONG;
typedef unsigned char	BYTE,	*PBYTE;
typedef unsigned char	UCHAR,	*PUCHAR;
typedef unsigned short	WORD,	*PWORD, WCHAR;
typedef unsigned short	USHORT,	*PUSHORT;
typedef unsigned int	UINT,	*PUINT;

typedef void    *PVOID;

#ifndef _WIN64
typedef unsigned long DWORD_PTR, *PDWORD_PTR;
#endif // _WIN64

#define TCHAR   unsigned short

#define _T(_x_)   L ## _x_

#ifndef IoCopyCurrentIrpStackLocationToNext
#define IoCopyCurrentIrpStackLocationToNext( Irp ) {										\
    PIO_STACK_LOCATION irpSp;																\
    PIO_STACK_LOCATION nextIrpSp;															\
    irpSp = IoGetCurrentIrpStackLocation( (Irp) );											\
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) ); \
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine));	\
    nextIrpSp->Control = 0;																	\
}
#endif

#ifndef IoSkipCurrentIrpStackLocation
#define IoSkipCurrentIrpStackLocation( Irp )												\
    (Irp)->CurrentLocation++;																\
    (Irp)->Tail.Overlay.CurrentStackLocation++;
#endif

// -----------------------------------------------------------------------
// --------------------- Filter Attach Specific declares -----------------
// -----------------------------------------------------------------------

#define TCP_TAG		'TCP '
#define MC_TAG		'MC  '
#define UDP_TAG		'UDP '
#define IP_TAG		'IP  '
#define RAW_IP_TAG	'RAW '
#define AFD_TAG		'AFD '

#define TCP6_TAG	'TCP6'
#define UDP6_TAG	'UDP6'
#define IP6_TAG		'IP6 '
#define RAW_IP6_TAG	'RAW6'


#define DEVICE_TCP_NAME      L"\\Device\\Tcp"
#define DEVICE_UDP_NAME      L"\\Device\\Udp"
#define DEVICE_IP_NAME       L"\\Device\\Ip"
#define DEVICE_RAW_IP_NAME   L"\\Device\\RawIp"
#define DEVICE_AFD_NAME      L"\\Device\\Afd"


#define DEVICE_TCP6_NAME      L"\\Device\\Tcp6"
#define DEVICE_UDP6_NAME      L"\\Device\\Udp6"
#define DEVICE_IP6_NAME       L"\\Device\\Ip6"
#define DEVICE_RAW_IP6_NAME   L"\\Device\\RawIp6"
#define DEVICE_AFD6_NAME      L"\\Device\\Afd6"

#ifdef __CPP
extern "C"
{
#include <ndis.h>    
#include "tdi.h"
#include <tdikrnl.h>

//#if _MSC_VER < 1300 // We need it for VC6
    
    typedef char *  va_list;
    _CRTIMP int __cdecl _vsnprintf(char *, size_t, const char *, va_list);
    _CRTIMP int __cdecl sprintf(char *, const char *, ...);    
    _CRTIMP int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
    

//#endif

#define _INTSIZEOF(n)   ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )

#ifndef va_start
#define va_start(ap,v)  ( ap = (va_list)&v + _INTSIZEOF(v) )
#endif

//#define va_arg(ap,t)    ( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)      ( ap = (va_list)0 )
/*
    typedef struct _PROCESS_PARAMETERS {
        ULONG AllocationSize;
        ULONG ActualSize;
        ULONG Flags;//PPFLAG_xxx
        ULONG Unknown1;
        ULONG Unknown2;
        ULONG Unknown3;
        HANDLE InputHandle;
        HANDLE OutputHandle;
        HANDLE ErrorHandle;
        UNICODE_STRING CurrentDirectory;
        HANDLE CurrentDir;
        UNICODE_STRING SearchPaths;
        UNICODE_STRING ApplicationName;
        UNICODE_STRING CommandLine;
        PVOID EnvironmentBlock;
        ULONG Unknown[9];
        UNICODE_STRING Unknown4;
        UNICODE_STRING Unknown5;
        UNICODE_STRING Unknown6;
        UNICODE_STRING Unknown7;
    } PROCESS_PARAMETERS, *PPROCESS_PARAMETERS;
    
    struct _PEB {				// Size: 0x1D8
        UCHAR InheritedAddressSpace;
        UCHAR ReadImageFileExecOptions;
        UCHAR BeingDebugged;
        UCHAR SpareBool;                	// Allocation size
        HANDLE Mutant;
        VOID * ImageBaseAddress;		// Instance
        VOID *DllList;
        PROCESS_PARAMETERS *ProcessParameters;
        //   <...>
    };
*/
#define DOS_MAX_COMPONENT_LENGTH 255
#define DOS_MAX_PATH_LENGTH (DOS_MAX_COMPONENT_LENGTH + 5 )

    typedef struct _CURDIR {
        UNICODE_STRING DosPath;
        HANDLE Handle;
    } CURDIR, *PCURDIR;

    //
    // Low order 2 bits of handle value used as flag bits.
    //

#define RTL_USER_PROC_CURDIR_CLOSE      0x00000002
#define RTL_USER_PROC_CURDIR_INHERIT    0x00000003

    typedef struct _RTL_DRIVE_LETTER_CURDIR {
        USHORT Flags;
        USHORT Length;
        ULONG TimeStamp;
        STRING DosPath;
    } RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;

#define RTL_MAX_DRIVE_LETTERS 32
#define RTL_DRIVE_LETTER_VALID (USHORT)0x0001

    typedef struct _RTL_USER_PROCESS_PARAMETERS {
        ULONG MaximumLength;
        ULONG Length;

        ULONG Flags;
        ULONG DebugFlags;

        HANDLE ConsoleHandle;
        ULONG  ConsoleFlags;
        HANDLE StandardInput;
        HANDLE StandardOutput;
        HANDLE StandardError;

        CURDIR CurrentDirectory;        // ProcessParameters
        UNICODE_STRING DllPath;         // ProcessParameters
        UNICODE_STRING ImagePathName;   // ProcessParameters
        UNICODE_STRING CommandLine;     // ProcessParameters
        PVOID Environment;              // NtAllocateVirtualMemory

        ULONG StartingX;
        ULONG StartingY;
        ULONG CountX;
        ULONG CountY;
        ULONG CountCharsX;
        ULONG CountCharsY;
        ULONG FillAttribute;

        ULONG WindowFlags;
        ULONG ShowWindowFlags;
        UNICODE_STRING WindowTitle;     // ProcessParameters
        UNICODE_STRING DesktopInfo;     // ProcessParameters
        UNICODE_STRING ShellInfo;       // ProcessParameters
        UNICODE_STRING RuntimeData;     // ProcessParameters
        RTL_DRIVE_LETTER_CURDIR CurrentDirectores[ RTL_MAX_DRIVE_LETTERS ];
    } RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;


    struct _PEB {				// Size: 0x1D8
        UCHAR InheritedAddressSpace;
        UCHAR ReadImageFileExecOptions;
        UCHAR BeingDebugged;
        UCHAR SpareBool;                	// Allocation size
        HANDLE Mutant;
        VOID * ImageBaseAddress;		// Instance
        VOID *DllList;
        RTL_USER_PROCESS_PARAMETERS *ProcessParameters;
        //   <...>
    };

    struct _EPROCESS {
        CHAR    fill1[0x1b0];
        _PEB*   pPeb;
        CHAR    fill2[0xa1];
    };

    NTKERNELAPI
        PEPROCESS
        IoGetRequestorProcess(
        IN PIRP Irp
        );
    
    NTSTATUS 
        ZwQuerySystemInformation(IN  ULONG		InfoClass,
        OUT PVOID		SysInfo,
        IN  ULONG		SysInfoLen,
        OUT PULONG		LenReturned OPTIONAL);
    
    NTSYSAPI
        NTSTATUS
        NTAPI
        ZwOpenSymbolicLinkObject(
        OUT PHANDLE     LinkHandle,
        IN  ACCESS_MASK DesiredAccess,
        IN  POBJECT_ATTRIBUTES ObjectAttributes
        );

    NTSYSAPI
        NTSTATUS
        NTAPI
        ZwQuerySymbolicLinkObject(
        IN  HANDLE      LinkHandle,
        IN OUT PUNICODE_STRING LinkTarget,
        OUT PULONG      ReturnedLength OPTIONAL
        );

    NTSTATUS
        PsLookupProcessByProcessId(HANDLE   PID, PEPROCESS* Process);

    NTSYSAPI
        NTSTATUS
        NTAPI
        ObQueryNameString (
        PVOID Object,
        PUNICODE_STRING Result OPTIONAL,
        DWORD Len,
        DWORD * RetLen OPTIONAL );

    NTSTATUS
        ObReferenceObjectByName(
        IN PUNICODE_STRING ObjectName,
        IN ULONG Attributes,
        IN PACCESS_STATE PassedAccessState OPTIONAL,
        IN ACCESS_MASK DesiredAccess OPTIONAL,
        IN POBJECT_TYPE ObjectType,
        IN KPROCESSOR_MODE AccessMode,
        IN OUT PVOID ParseContext OPTIONAL,
        OUT PVOID *Object
        );
    
    NTSYSAPI
        WCHAR
        NTAPI
        RtlUpcaseUnicodeChar(
        WCHAR SourceCharacter
        );
    
    extern POBJECT_TYPE IoDriverObjectType;

}
#endif // __CPP

#pragma pack( push, 1)
typedef struct _IRP_COMPLETE_CONTEXT {
	LIST_ENTRY			    m_ListEntry;  // связь в списки
	PIRP				    Irp;	
    PFILE_OBJECT            FileObject;
	PDEVICE_OBJECT		    DeviceObject;    
	KIRQL				    Irql;	
    WORK_QUEUE_ITEM		    WorkQueueItem;    
} IRP_COMPLETE_CONTEXT, *PIRP_COMPLETE_CONTEXT;

typedef struct _PEND_IRP {
	LIST_ENTRY	            m_ListEntry;
	PIRP		            pIrp;
    PVOID                   OldCancel;
    PVOID                   OldCancelContext;	
    IRQL                    m_Irql;
	PIRP*					pOriginalIrp;
} PEND_IRP, *PPEND_IRP;
#pragma pack( pop )

//Threads Creating
typedef
VOID (__cdecl *THREADINITPROC)( VOID );

typedef
VOID (__stdcall *PSTART_ROUTINE)( PVOID StartContext );

#pragma pack( push, 1)
typedef struct _START_PARAMS {
		PSTART_ROUTINE StartRoutine;   // Function to call
		PVOID StartContext;				// Paramerer for this function
		PVOID  ThreadHandle;			// Handle of the thread
		void*	pObject;
} START_PARAMS,*PSTART_PARAMS;
#pragma pack( pop )

#define DEFINE_DELAYED_ROUTINE(RoutineName)                             \
extern KL_EVENT RoutineName##Event;                                     \
extern CKl_List<IRP_COMPLETE_CONTEXT>* RoutineName##List;               \
VOID                                                                    \
RoutineName##(PVOID Context)

#define INIT_DELAYED_ROUTINE(RoutineName)                               \
KeInitializeEvent ( &(RoutineName##Event), NotificationEvent, FALSE);   \
RoutineName##List = new CKl_List<IRP_COMPLETE_CONTEXT>();               \
StartThread(RoutineName, NULL)


#define IMPLEMENT_DELAYED_ROUTINE(RoutineName, DelayedItemRoutine)      \
KL_EVENT RoutineName##Event;                                            \
CKl_List<IRP_COMPLETE_CONTEXT>* RoutineName##List;                      \
VOID                                                                    \
RoutineName##(PVOID Context)                                            \
{                                                                       \
    PIRP_COMPLETE_CONTEXT  CompleteContext = NULL;                      \
    do {                                                                \
        KeWaitForSingleObject(&(RoutineName##Event, Executive, KernelMode, FALSE, NULL);   \
        RoutineName##List->Lock();                                      \
        CompleteContext = RoutineName##List->InterlockedRemoveHead();              \
        RoutineName##List->Unlock();                                    \
        if ( CompleteContext )  {                                       \
        DelayedItemRoutine(CompleteContext);                            \
        ExFreePool(CompleteContext);                                    \
        }                                                               \
    } while (TRUE);                                                     \
}


static
bool
KlCompareWStr(wchar_t* src, wchar_t* dst, bool CaseInSensitive)
{
    UNICODE_STRING  str1, str2;
    RtlInitUnicodeString(&str1, src);
    RtlInitUnicodeString(&str2, dst);
    
    return ( TRUE  == RtlEqualUnicodeString( &str1, &str2, CaseInSensitive ) );
}

