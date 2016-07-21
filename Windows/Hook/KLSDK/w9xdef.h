#define IN
#define OUT

#define DbgPrint    nprintf
#define VXDINLINE static __inline

#define PAGED_CODE()

typedef void    *PVOID;
typedef unsigned long	DWORD,	*PDWORD, DWORD_PTR;
typedef unsigned long	ULONG,	*PULONG;
typedef unsigned char	BYTE,	*PBYTE;
typedef unsigned char	UCHAR,	*PUCHAR;
typedef unsigned short	WORD,	*PWORD, WCHAR, wchar_t;
typedef unsigned short	USHORT,	*PUSHORT;
typedef unsigned int	UINT,	*PUINT;

#define TCHAR   unsigned char

#define _T(_x_) (TCHAR*)(_x_)

#ifndef RemoveTailList
#define RemoveTailList(ListHead) \
    (ListHead)->Blink;\
    {RemoveEntryList((ListHead)->Blink)}
#endif


#define USE_NDIS

#ifdef __CPP
#include <vtoolscp.h>
#else
#include <vtoolsc.h>
#endif

#include "tdistat.h"
#include "TDI.h"
#include "tdivxd.h"

//----------- VWIN32 stuff -----------------------------------------------------
#define  vwin32_DEVICE_ID 0x2a
#define  VWIN32_StdCall_Service Declare_SCService

Begin_Service_Table(vwin32)
VWIN32_Service  (vwin32_Get_Version )
VWIN32_Service  (vwin32_DIOCCompletionRoutine )
VWIN32_Service  (_vwin32_QueueUserApc)
VWIN32_Service  (_vwin32_Get_Thread_Context)
VWIN32_Service  (_vwin32_Set_Thread_Context)
VWIN32_Service  (_vwin32_CopyMem )
VWIN32_Service  (_vwin32_Npx_Exception)
VWIN32_Service  (_vwin32_Emulate_Npx)
VWIN32_Service  (_vwin32_CheckDelayedNpxTrap)
VWIN32_Service  (vwin32_EnterCrstR0)
VWIN32_Service  (vwin32_LeaveCrstR0)
VWIN32_Service  (_vwin32_FaultPopup)
VWIN32_Service  (vwin32_GetContextHandle)
VWIN32_Service  (vwin32_GetCurrentProcessHandle )
VWIN32_Service  (_vwin32_SetWin32Event)
VWIN32_Service  (_vwin32_PulseWin32Event)
VWIN32_Service  (_vwin32_ResetWin32Event)
VWIN32_Service  (_vwin32_WaitSingleObject)
VWIN32_Service  (_vwin32_WaitMultipleObjects)
VWIN32_Service  (_vwin32_CreateRing0Thread)
VWIN32_Service  (_vwin32_CloseVxDHandle)
VWIN32_Service  (vwin32_ActiveTimeBiasSet )
VWIN32_Service  (vwin32_GetCurrentDirectory )
VWIN32_Service  (vwin32_BlueScreenPopup)
VWIN32_Service  (vwin32_TerminateApp)
VWIN32_Service  (_vwin32_QueueKernelAPC)
VWIN32_Service  (vwin32_SysErrorBox)
VWIN32_Service  (_vwin32_IsClientWin32)
VWIN32_Service  (vwin32_IFSRIPWhenLev2Taken )
VWIN32_Service  (_vwin32_InitWin32Event)
VWIN32_Service  (_vwin32_InitWin32Mutex)
VWIN32_Service  (_vwin32_ReleaseWin32Mutex)
VWIN32_Service  (_vwin32_BlockThreadEx)
VWIN32_Service  (vwin32_GetProcessHandle )
VWIN32_Service  (_vwin32_InitWin32Semaphore)
VWIN32_Service  (_vwin32_SignalWin32Sem)
VWIN32_Service  (_vwin32_QueueUserApcEx)
VWIN32_Service	(_vwin32_OpenVxDHandle)
VWIN32_Service	(_vwin32_CloseWin32Handle)
VWIN32_Service	(_vwin32_AllocExternalHandle)
VWIN32_Service	(_vwin32_UseExternalHandle)
VWIN32_Service	(_vwin32_UnuseExternalHandle)
VWIN32_StdCall_Service	(KeInitializeTimer, 1)
VWIN32_StdCall_Service	(KeSetTimer, 4)
VWIN32_StdCall_Service	(KeCancelTimer, 1)
VWIN32_StdCall_Service	(KeReadStateTimer, 1)
VWIN32_Service	(_vwin32_ReferenceObject)
VWIN32_Service	(_vwin32_GetExternalHandle)
VWIN32_StdCall_Service	(vwin32_ConvertNtTimeout, 1)
VWIN32_Service	(_vwin32_SetWin32EventBoostPriority)
VWIN32_Service	(_vwin32_GetRing3Flat32Selectors)
VWIN32_Service	(_vwin32_GetCurThreadCondition)
VWIN32_Service  (vwin32_Init_FP)
VWIN32_StdCall_Service  (R0SetWaitableTimer, 5)
End_Service_Table(vwin32)

DWORD NTKERN_Get_Version(void);

#define ENEWHDR     0x003CL         /* offset of new EXE header */
#define EMAGIC      0x5A4D          /* old EXE magic id:  'MZ'  */
#define PEMAGIC     0x4550          /* NT portable executable */

#define GET_DIR(x)  (hdr->OptionalHeader.DataDirectory[x].VirtualAddress)

#define Touch_Register(Register) { __asm xor Register, Register }

typedef struct _MODREF
{
     struct _MODREF* pNextModRef;    //00h
     DWORD           un1;            //04h number of ?
     DWORD           un2;            //08h Ring0TCB ?
     DWORD           un3;            //0Ch
     WORD            mteIndex;       //10h
     WORD            un4;            //12h
     DWORD           un5;            //14h
     PVOID           ppdb;           //18h Pointer to process database
     DWORD           un6;            //1Ch
     DWORD           un7;            //20h
     DWORD           un8;            //24h
} MODREF, *PMODREF; 

typedef struct _IMTE
{
     DWORD           un1;            //00h
     DWORD           pNTHdr;     //04h
     DWORD           un2;            //08h
     PCHAR           pszFileName;    //0Ch
     PCHAR           pszModName;     //10h
     WORD            cbFileName;     //14h
     WORD            cbModName;      //16h
     DWORD           un3;            //18h
     DWORD           cSections;      //1Ch
     DWORD           un5;            //20h
     DWORD           baseAddress;    //24h
     WORD            hModule16;      //28h goo
     WORD            cUsage;         //2Ah
     DWORD           unknown;        //2Ch ??? ptr struc1,and struc1[18h]->PDB
                                     //    this is _MODREF ?
     PCHAR           pszFileName2;   //30h
     WORD            cbFileName2;    //34h
     DWORD           pszModName2;    //36h
     WORD            cbModName2;     //3Ah
} IMTE, *PIMTE; 

DECLARE_HANDLE ( HMODULE );
PIMTE * * GetPIMTEArrayHolder ( HMODULE hK32 );

struct _STARTUPINFOA;
typedef struct _STARTUPINFOA STARTUPINFOA, *LPSTARTUPINFOA; 

typedef struct _ENVIRONMENT_DATABASE	//why not name EDB
{
     PCHAR          pszEnvironment;     //00h Pointer to Environment
     DWORD          un1;                //04h
     PCHAR          pszCmdLine;         //08h Pointer to command line
     PCHAR          pszCurrDirectory;   //0Ch Pointer to current directory
     LPSTARTUPINFOA pStartupInfo;       //10h Pointer to STARTUPINFOA struct go
     HANDLE         hStdIn;             //14h Standard Input
     HANDLE         hStdOut;            //18h Standard Output
     HANDLE         hStdErr;            //1Ch Standard Error
     DWORD          un2;                //20h
     DWORD          InheritConsole;     //24h
     DWORD          BreakType;          //28h
     DWORD          BreakSem;           //2Ch
     DWORD          BreakEvent;         //30h
     DWORD          BreakThreadID;      //34h
     DWORD          BreakHandlers;      //38h
} ENVIRONMENT_DATABASE, *PENVIRONMENT_DATABASE, * PEDB; 

typedef struct _HANDLE_TABLE_ENTRY
{
    DWORD   flags;      // Valid flags depend on what type of object this is
    PVOID   pObject;    // Pointer to the object that the handle refers to
} HANDLE_TABLE_ENTRY, *PHANDLE_TABLE_ENTRY;

typedef struct _HANDLE_TABLE
{
    DWORD   cEntries;               // Max number of handles in table
    HANDLE_TABLE_ENTRY array[1];    // An array (number is given by cEntries)
} HANDLE_TABLE, *PHANDLE_TABLE;


typedef BYTE * LPBYTE;
struct _STARTUPINFOA
{
	DWORD     cb;
	PCHAR     lpReserved;
	PCHAR     lpDesktop;
	PCHAR     lpTitle;
	DWORD     dwX;
	DWORD     dwY;
	DWORD     dwXSize;
	DWORD     dwYSize;
	DWORD     dwXCountChars;
	DWORD     dwYCountChars;
	DWORD     dwFillAttribute;
	DWORD     dwFlags;
	WORD      wShowWindow;
	WORD      cbReserved2;
	LPBYTE    lpReserved2;
	HANDLE    hStdInput;
	HANDLE    hStdOutput;
	HANDLE    hStdError;
};

DECLARE_HANDLE( HTASK );

struct _PROCESS_DATABASE;
typedef struct _PROCESS_DATABASE *PPDB, PROCESS_DATABASE, *PPROCESS_DATABASE; 
typedef BYTE CRIT_SEC_24h[0x24];
struct _PROCESS_DATABASE	//PDB 
{
     DWORD          Type;               //00h KERNEL32 object type (5)
     DWORD          cReference;         //04h Number of references to process
     DWORD          un1;                //08h
     DWORD          someEvent;          //0Ch An event object (What's it used for???)
     DWORD          TerminationStatus;  //10h Returned by GetExitCodeProcess
     DWORD          un2;                //14h
     DWORD          DefaultHeap;        //18h Address of the process heap
     DWORD          MemoryContext;      //1Ch pointer to the process's context goo
     DWORD          flags;              //20h go
     DWORD          pPSP;               //24h Linear address of PSP?
     WORD           PSPSelector;        //28h
     WORD           MTEIndex;           //2Ah *4+ModuleList=IMTE goo
     WORD           cThreads;           //2Ch
     WORD           cNotTermThreads;    //2Eh
     WORD           un3;                //30h
     WORD           cRing0Threads;      //32h number of ring 0 threads
     HANDLE         HeapHandle;         //34h Heap to allocate handle tables out of
                                        //    This seems to always be the KERNEL32 heap
     HTASK          W16TDB;             //38h Win16 Task Database selector goo
     DWORD          MemMapFiles;        //3Ch memory mapped file list (?)
     PEDB           pEDB;               //40h Pointer to Environment Database go
     PHANDLE_TABLE  pHandleTable;       //44h Pointer to process handle table
     PPDB           ParentPDB;          //48h Parent process database
     PMODREF        MODREFlist;         //4Ch Module reference list go
     DWORD          ThreadList;         //50h Threads in this process
     DWORD          DebuggeeCB;         //54h Debuggee Context block?
     DWORD          LocalHeapFreeHead;  //58h Head of free list in process heap
     DWORD          InitialRing0ID;     //5Ch
     CRIT_SEC_24h   crst;               //60h defined in winnt.h goo 24h len
			                  //    yes, this structure is 24h len
     DWORD          pConsole;           //84h Pointer to console for process
     DWORD          tlsInUseBits1;      //88h  // Represents TLS indices 0 - 31
     DWORD          tlsInUseBits2;      //8Ch  // Represents TLS indices 32 - 63
     DWORD          ProcessDWORD;       //90h
     PPDB           ProcessGroup;       //94h
     DWORD          pExeMODREF;         //98h pointer to EXE's MODREF
     DWORD          TopExcFilter;       //9Ch Top Exception Filter?
     DWORD          BasePriority;       //A0h Base scheduling priority for process
     DWORD          HeapOwnList;        //A4h Head of the list of process heaps
     DWORD          HeapHandleBlockList;//A8h Pointer to head of heap handle block list
     DWORD          pSomeHeapPtr;       //ACh normally zero, but can a pointer to a
                                        //moveable handle block in the heap
     DWORD          pConsoleProvider;   //B0h Process that owns the console we're using?
     WORD           EnvironSelector;    //B4h Selector containing process environment
     WORD           ErrorMode;          //B6H SetErrorMode value (also thunks to Win16)
     DWORD          pevtLoadFinished;   //B8h Pointer to event LoadFinished?
     WORD           UTState;            //BCh
};


typedef struct _IMAGE_DATA_DIRECTORY {
	DWORD   VirtualAddress;
	DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0

typedef struct _IMAGE_OPTIONAL_HEADER {
	//
	// Standard fields.
	//
	
	WORD    Magic;
	BYTE    MajorLinkerVersion;
	BYTE    MinorLinkerVersion;
	DWORD   SizeOfCode;
	DWORD   SizeOfInitializedData;
	DWORD   SizeOfUninitializedData;
	DWORD   AddressOfEntryPoint;
	DWORD   BaseOfCode;
	DWORD   BaseOfData;
	
	//
	// NT additional fields.
	//
	
	DWORD   ImageBase;
	DWORD   SectionAlignment;
	DWORD   FileAlignment;
	WORD    MajorOperatingSystemVersion;
	WORD    MinorOperatingSystemVersion;
	WORD    MajorImageVersion;
	WORD    MinorImageVersion;
	WORD    MajorSubsystemVersion;
	WORD    MinorSubsystemVersion;
	DWORD   Win32VersionValue;
	DWORD   SizeOfImage;
	DWORD   SizeOfHeaders;
	DWORD   CheckSum;
	WORD    Subsystem;
	WORD    DllCharacteristics;
	DWORD   SizeOfStackReserve;
	DWORD   SizeOfStackCommit;
	DWORD   SizeOfHeapReserve;
	DWORD   SizeOfHeapCommit;
	DWORD   LoaderFlags;
	DWORD   NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD   Characteristics;
	DWORD   TimeDateStamp;
	WORD    MajorVersion;
	WORD    MinorVersion;
	DWORD   Name;
	DWORD   Base;
	DWORD   NumberOfFunctions;
	DWORD   NumberOfNames;
	DWORD   AddressOfFunctions;     // RVA from base of image
	DWORD   AddressOfNames;         // RVA from base of image
	DWORD   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_FILE_HEADER {
	WORD    Machine;
	WORD    NumberOfSections;
	DWORD   TimeDateStamp;
	DWORD   PointerToSymbolTable;
	DWORD   NumberOfSymbols;
	WORD    SizeOfOptionalHeader;
	WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_NT_HEADERS {
	DWORD Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;


//Threads Creating
typedef
VOID (__cdecl *THREADINITPROC)( VOID );

typedef
VOID (__stdcall *PKSTART_ROUTINE)( PVOID StartContext );

#pragma pack(1)
typedef struct _START_PARAMS {
		PKSTART_ROUTINE StartRoutine;   // Function to call
		PVOID StartContext;				// Paramerer for this function
		PTCB  ThreadHandle;				// Handle of the thread
        PVOID   _ThisThread;            // Pointer to CKl_Thread
} START_PARAMS,*PSTART_PARAMS;
#pragma pack()

PTCB VXDINLINE
VMMCreateThreadEx(DWORD ClientSS,
				   DWORD ClientESP,
				   DWORD ClientCS,
				   DWORD ClientEIP,
				   DWORD ClientDS,
				   DWORD ClientES,
				   DWORD dwThreadType,
				   THREADINITPROC InitCallback,
				   DWORD dwRefData)
{
    __asm push [dwRefData]
		__asm push [InitCallback]
		__asm push [dwThreadType]
		__asm push [ClientES]
		__asm push [ClientDS]
		__asm push [ClientEIP]
		__asm push [ClientCS]
		__asm push [ClientESP]
		__asm push [ClientSS]
		VMMcall(VMMCreateThreadEx)
		__asm add esp,9*4
}

VOID VXDINLINE
InitWin32Event(PKEVENT Event,BOOL fManualReset,BOOL fInitialState)
{
    __asm push [fInitialState]    ;
	__asm push [fManualReset];
	__asm push [Event];
	VMMcall (_vwin32_InitWin32Event);	
	__asm add esp,12;
}

#define DEFINE_DELAYED_ROUTINE(RoutineName, ContextType)                \
extern KL_EVENT RoutineName##Event;                                     \
extern CKl_List<ContextType>* RoutineName##List;                        \
VOID                                                                    \
RoutineName##(PVOID Context)

#define INIT_DELAYED_ROUTINE(RoutineName, ContextType)                               \
KeInitializeEvent ( &(RoutineName##Event), NotificationEvent, FALSE);   \
RoutineName##List = new CKl_List<ContextType>();                   \
StartThread(RoutineName, NULL)


#define IMPLEMENT_DELAYED_ROUTINE(RoutineName, DelayedItemRoutine, ContextType )      \
KL_EVENT RoutineName##Event;                                            \
CKl_List<ContextType>* RoutineName##List;                          \
VOID                                                                    \
RoutineName##(PVOID Context)                                            \
{                                                                       \
    ContextType * CompleteContext = NULL;                          \
    do {                                                                \
        Wait_Semaphore (RoutineName##Event, BLOCK_SVC_INTS);            \
        RoutineName##List->Lock();                                      \
        CompleteContext = RoutineName##List->InterlockedRemoveHead();              \
        RoutineName##List->Unlock();                                    \
        if ( CompleteContext )  {                                       \
        DelayedItemRoutine(CompleteContext);                            \
        HeapFree(CompleteContext, 0);                                   \
        }                                                               \
    } while (TRUE);                                                     \
}

#define NTAPI	__stdcall

typedef __int64 KL_TIME, *PKL_TIME;

//#define KlGetSystemTime(_Time_)		*(_Time_) = Get_System_Time();
/*
#define KlGetSystemTime(_Time_)	{							\
ULONG		DosTime;										\
ULONG		AdditionalmSec;									\
DosTime = IFSMgr_Get_DOSTime(&AdditionalmSec);	\
IFSMgr_DosToWin32Time(*(dos_time*)&DosTime,(PFILETIME)(_Time_));			\
}
*/

#define KlGetSystemTime(_Time_)	{							\
dos_time	DosTime;										\
ULONG		AdditionalmSec;									\
*(ULONG*)&DosTime = IFSMgr_Get_DOSTime(&AdditionalmSec);	\
IFSMgr_DosToWin32Time(DosTime,(PFILETIME)(_Time_));			\
}

#define ONE_SECOND_TIME				(KL_TIME)0x989680
#define HALF_SECOND_TIME			(KL_TIME)0x4C4B40

#define TICKSPERMIN        600000000
#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12

#define DOS_YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define DOS_MONTH(t)  ((t & 0x01E0) >> 5)
#define DOS_DAY(t)    (t & 0x001F)
/*
#define DOS_HOUR(t)   ((t & 0xF800) >> 11)
#define DOS_MINUTE(t) ((t & 0x07E0) >> 5)
#define DOS_SECOND(t) ((t & 0x001F) << 1)
*/
#define DOS_HOUR(t)   (t >> 11)
#define DOS_MINUTE(t) ((t >> 5) & 0x3f)
#define DOS_SECOND(t) ((t & 0x1f) << 1)

#define DOS_WEEKDAY(t) (((EPOCHWEEKDAY + DOS_DAY(t)) % DAYSPERWEEK)


#define KlGetTimeFields( _Year, _Month, _Day, _Hour, _Minute, _Second, _Milliseconds, _Weekday ) \
{ \
     dos_time  DosTime; \
     ULONG     AdditionalmSec; \
     *(ULONG*)&DosTime=IFSMgr_Get_DOSTime(&AdditionalmSec); \
     _Year   = DOS_YEAR  (DosTime.dt_date);  \
    _Month  = DOS_MONTH (DosTime.dt_date ); \
    _Day    = DOS_DAY   (DosTime.dt_date ); \
    _Hour   = DOS_HOUR  (DosTime.dt_time ); \
    _Minute = DOS_MINUTE(DosTime.dt_time ); \
    _Second = DOS_SECOND(DosTime.dt_time ); \
    _Milliseconds = 0;                      \
    _Weekday = 0;                           \
}    

#define PsGetCurrentProcessId		(HANDLE)VWIN32_GetCurrentProcessHandle
#define PsGetCurrentThreadId		(HANDLE)Get_Cur_Thread_Handle

#define InterlockedIncrement(_x_)	++((*_x_))
#define InterlockedDecrement(_x_)	--((*_x_))

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE (HANDLE)-1
#endif




#define SPIN_LOCK					MUTEXHANDLE
#define IRQL                        int
#define AllocateSpinLock(_Lock_)	*(_Lock_) = CreateMutex(0,MUTEX_MUST_COMPLETE)
#define AcquireSpinLock(_Lock_, _OldIrql)		EnterMutex(*(_Lock_), BLOCK_THREAD_IDLE) 
#define ReleaseSpinLock(_Lock_, _OldIrql)		LeaveMutex(*(_Lock_))

#define KeRaiseIrql(x, y)
#define KeLowerIrql(x)

#define KEVENT									SEMHANDLE
#define PKEVENT									PVOID
#define KeSetEvent(pEvent,PriorInc,Wait)		Signal_Semaphore_No_Switch(*pEvent)
#define KeInitializeEvent(pEvent,Type,State)	*pEvent=Create_Semaphore(State)
#define KeDestroyEvent(pEvent)					Destroy_Semaphore(*pEvent)
#define KeWaitForSingleObject(Object,WaitReason,WaitMode,Alertable,Timeout) Wait_Semaphore(*Object,BLOCK_SVC_INTS)

#define KlSleep(_Time_)   Time_Slice_Sleep(_Time_ * 1000)

/*
#define KL_EVENT					ULONG
#define PKL_EVENT					PULONG

VXDINLINE void KlWaitEvent(PKL_EVENT  Event)
{
    Begin_Nest_Exec();
    while ( *(Event) == 0 )
    {
        Resume_Exec();
        Time_Slice_Sleep(0);
    }
    End_Nest_Exec();   
}

#define KlInitializeEvent(_Event_, _Type_, _State_)		*(_Event_) = (_State_)
#define KlSetEvent(_Event_)								*(_Event_) += 1
#define KlClearEvent(_Event_)							*(_Event_) = 0
*/

#define  KL_EVENT   KEVENT
#define  PKL_EVENT  *KL_EVENT
#define  KlWaitEvent(_Event_) KeWaitForSingleObject( (_Event_), Executive, KernelMode, FALSE, NULL)
#define  KlInitializeEvent(_Event_, _Type_, _State_) KeInitializeEvent(_Event_,_Type_,_State_)
#define  KlSetEvent(_Event_)	KeSetEvent(_Event_,0,0)
#define  KlClearEvent(_Event_)
    

typedef enum _POOL_TYPE {
	NonPagedPool,
	PagedPool,
	NonPagedPoolMustSucceed,
	DontUseThisType,
	NonPagedPoolCacheAligned,
	PagedPoolCacheAligned,
	NonPagedPoolCacheAlignedMustS,
	MaxPoolType
} POOL_TYPE;

#define KL_MEM_ALLOC(_size_)    HeapAllocate((_size_), HEAPZEROINIT )
#define KL_MEM_FREE(_memory_)   HeapFree( (_memory_), 0 )

#define ExAllocatePoolWithTag(PoolType, NumberOfBytes, Tag) HeapAllocate ( NumberOfBytes, HEAPZEROINIT )
#define ExFreePool( P ) HeapFree ( P, 0 )

#define KlCompareMemory(_Dst_, _Src_, _Length_)         !memcmp( (_Dst_), (_Src_), (_Length_) )

#define RtlCopyMemory(_Destination, _Source,_Length)	memcpy((_Destination),(_Source),(_Length))
#define RtlFillMemory(_Destination, _Length,_Fill)		memset((_Destination),(_Fill),(_Length))
#define RtlZeroMemory(_Destination,_Length)				RtlFillMemory(_Destination, _Length, 0)

VOID	NdisInitializeString(PUNICODE_STRING	DestStr, PUCHAR		SrcStr);

#define NdisFreeString(_String_)	 HeapFree((_String_).Buffer, 0 );

#define NdisAdjustBufferLength(_buffer_, _length_)  (_buffer_)->Length = (_length_)


#define PASSIVE_LEVEL   0
#define DISPATCH_LEVEL  2

#define KeRaiseIrqlToDpcLevel()
#define KeLowerIrql(_Irql_)
#define KeGetCurrentIrql()    PASSIVE_LEVEL

extern bool K32Initialized;

static inline size_t wcslen( wchar_t* src )
{
    size_t size = 0;
    while ( src[size] != 0 )
        ++size;

    return size;
}

static inline void wcscpy( wchar_t *dst, wchar_t *src )
{
    int i = 0;
    while ( src[i] != 0 )
    { 
        dst[i] = src[i]; 
        ++i; 
    } 
}

static inline VOID
Ansi2Unicode(wchar_t*   dst, char*  src)
{
    RtlZeroMemory(dst, ( strlen( src ) + 1 ) << 1);

    for ( size_t i = 0; i < strlen( src ); ++i )    
        ((char*)dst)[2*i]   = src[i];
}

static inline VOID
Unicode2Ansi(char*  dst, wchar_t*   src)
{
    size_t i = 0;

    RtlZeroMemory( dst, wcslen( src ) + 1 );

    for ( i = 0; i < wcslen( src ); ++i )    
        dst[i]   = ((char*)src)[2*i];
    
    dst[i] = 0;
}

static
bool
KlCompareWStr(wchar_t* src, wchar_t* dst, bool CaseInSensitive)
{    
    char* src1 = (char*)KL_MEM_ALLOC ( (wcslen(src) + 1 )); 
    char* dst1 = (char*)KL_MEM_ALLOC ( (wcslen(dst) + 1 )); ;

    Unicode2Ansi( src1, src );
    Unicode2Ansi( dst1, dst );

    bool retval = ( 0 == stricmp ( src1, dst1 ) );

    KL_MEM_FREE( src1 );
    KL_MEM_FREE( dst1 );
    
    return retval;
}

static BOOLEAN bUpperTableInited = FALSE;

static
BYTE _UpperCharA(BYTE ch)	//!! nehorosho
{
    static CHAR UpperTable[257];
    DWORD cou;
    if (bUpperTableInited == FALSE) 
    {
        for (cou = 0; cou < sizeof(UpperTable); cou++) 
        {
            UpperTable[cou] = (CHAR)cou;
        }
        UpperTable[sizeof(UpperTable) ] = 0;
        
        __asm mov eax, offset UpperTable
        __asm inc eax
        __asm push eax
        VxDCall (_strupr);
        __asm add esp,4
            
            bUpperTableInited = TRUE;
    }
    return UpperTable[ch];
}