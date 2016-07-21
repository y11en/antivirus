unit NTDLL;

interface
uses windows, SysUtils, winsvc, ZUtil;

Const
  NTDLL_DLL_Name              = 'NTDLL.DLL';
  STATUS_SUCCESS              = $00000000;
  STATUS_INFO_LENGTH_MISMATCH = $C0000004;
  STATUS_BUFFER_OVERFLOW      = $80000005;
  STATUS_NO_MORE_FILES        = $80000006;
  STATUS_IMAGE_ALREADY_LOADED = $C000010E;
  OB_TYPE_TYPE                =    1;
  OB_TYPE_DIRECTORY           =    2;
  OB_TYPE_SYMBOLIC_LINK       =    3;
  OB_TYPE_TOKEN               =    4;
  OB_TYPE_PROCESS             =    5;
  OB_TYPE_THREAD              =    6;
  OB_TYPE_EVENT               =    7;
  OB_TYPE_EVENT_PAIR          =    8;
  OB_TYPE_MUTANT              =    9;
  OB_TYPE_SEMAPHORE           =    10;
  OB_TYPE_TIMER               =    11;
  OB_TYPE_PROFILE             =    12;
  OB_TYPE_WINDOW_STATION      =    13;
  OB_TYPE_DESKTOP             =    14;
  OB_TYPE_SECTION             =    15;
  OB_TYPE_KEY                 =    16;
  OB_TYPE_PORT                =    17;
  OB_TYPE_ADAPTER             =    18;
  OB_TYPE_CONTROLLER          =    19;
  OB_TYPE_DEVICE              =    20;
  OB_TYPE_DRIVER              =    21;
  OB_TYPE_IO_COMPLETION       =    22;
  OB_TYPE_FILE                =    23;


Type
  // Специфические для C и NT DDK типы
  NTSTATUS = DWORD;
  USHORT   = Word;
  PVOID    = Pointer;

  // Строка в формате UNICODE
  TUNICODE_STRING = packed record
   Length    :  word;    // Длина строки
   MaxLength :  word;    // Максимальная длина (размер выделенного буфера)
   StrData   :  lpwstr;  // Указатель на область памяти, хранящую строку
  end;

  TIO_STATUS_BLOCK  = packed record
    Status: NTSTATUS;
    Information: ULONG
  End;
  PIO_STATUS_BLOCK = ^TIO_STATUS_BLOCK;

  // Классы информации о файле
  FILE_INFORMATION_CLASS = (
    FileDirectoryInformation = 1,
    FileFullDirectoryInformation,
    FileBothDirectoryInformation,
    FileBasicInformation,
    FileStandardInformation,
    FileInternalInformation,
    FileEaInformation,
    FileAccessInformation,
    FileNameInformation,
    FileRenameInformation,
    FileLinkInformation,
    FileNamesInformation,
    FileDispositionInformation,
    FilePositionInformation,
    FileFullEaInformation,
    FileModeInformation,
    FileAlignmentInformation,
    FileAllInformation,
    FileAllocationInformation,
    FileEndOfFileInformation,
    FileAlternateNameInformation,
    FileStreamInformation,
    FilePipeInformation,
    FilePipeLocalInformation,
    FilePipeRemoteInformation,
    FileMailslotQueryInformation,
    FileMailslotSetInformation,
    FileCompressionInformation,
    FileCopyOnWriteInformation,
    FileCompletionInformation,
    FileMoveClusterInformation,
    FileOleClassIdInformation,
    FileOleStateBitsInformation,
    FileNetworkOpenInformation,
    FileObjectIdInformation,
    FileOleAllInformation,
    FileOleDirectoryInformation,
    FileContentIndexInformation,
    FileInheritContentIndexInformation,
    FileOleInformation,
    FileMaximumInformation
  );
  PFILE_INFORMATION_CLASS = ^FILE_INFORMATION_CLASS;

  _PROCESSINFOCLASS = (
   ProcessBasicInformation = 0
  );

  // Строка в формате Unicode
  UNICODE_STRING = Record
    Length, MaximumLength: USHORT;
    Buffer: PWideChar;
  End;
  PUNICODE_STRING = ^UNICODE_STRING;

  PIO_APC_ROUTINE = Procedure(
    ApcContext: Pointer;
    IoStatusBlock: PIO_STATUS_BLOCK;
    Reserved: ULONG
  ); StdCall;

  FILE_DIRECTORY_INFORMATION = Record
    NextEntryOffset, FileIndex: ULONG;
    CreationTime, LastAccessTime, LastWriteTime,
    ChangeTime, EndOfFile, AllocationSize: LARGE_INTEGER;
    FileAttributes, FileNameLength: ULONG;
    FileName: WideChar
  End;
  PFILE_DIRECTORY_INFORMATION = ^FILE_DIRECTORY_INFORMATION;

 // Классы системной информаии
 _SYSTEM_INFORMATION_CLASS = (
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
          SystemSessionProcessesInformation);
    SYSTEM_INFORMATION_CLASS = _SYSTEM_INFORMATION_CLASS;

    _OBJECT_INFORMATION_CLASS = (
          ObjectBasicInformation,
          ObjectNameInformation,
          ObjectTypeInformation,
          ObjectAllTypesInformation,
          ObjectHandlesInformation
    );
    OBJECT_INFORMATION_CLASS = _OBJECT_INFORMATION_CLASS;

   // Информация о процессах и потоках
   TSystemProcessesAndThreadsInformation = packed record
    NextEntryDelta          : ULONG;
    ThreadCount             : ULONG;
    Reserved1               : array[1..6] of ULONG;
    CreateTime              : large_integer;
    UserTime                : large_integer;
    KernelTime              : large_integer;
    ProcessName             : TUNICODE_STRING; // Имя процесса
    BasePriority            : ULONG;
    ProcessID               : ULONG;
    InheritedFromProcessID  : ULONG;
    HandleCount             : ULONG;
    Reserved2               : array[1..2] of ULONG;
   end;

   // Информация о модулях
   TSystemModuleInformation = packed record
    Reserved                : array[0..1] of DWORD;
    Base                    : DWORD;
    Size                    : DWORD;
    Flags                   : DWORD;
    Indes                   : WORD;
    Unknown                 : WORD;
    LoadCount               : WORD;
    ModuleNameOffset        : WORD;
    ImageName               : array[0..$FF] of char;
   end;
   PSystemModuleInformation = ^TSystemModuleInformation;

   // Информация о файловых потоках
   TFileStreamInformation = packed record
    NextEntryOffset   : DWORD;
    StreamNameLength  : DWORD;
    EndOfStream       : LARGE_INTEGER;
    AllocationSize    : LARGE_INTEGER;
   end;

   // Информация о Handle
   PSystemHandleInformation = ^TSystemHandleInformation;
   TSystemHandleInformation = packed record
    ProcessId         : dword;
    ObjectTypeNumber  : byte;
    Flags             : byte;
    Handle            : word;
    pObject           : pointer;
    GrantedAccess     : dword;
   end;

   PObjectNameInformation = ^TObjectNameInformation;
   TObjectNameInformation = packed record
    Name : UNICODE_STRING;
   end;

   PSystemHandleInformationRec = ^TSystemHandleInformationRec;
   TSystemHandleInformationRec = packed record
    NumberOfHandles: dword;
    Information: array [0..0] of TSystemHandleInformation;
   end;

   // имя файла
   TFileNameInformation = packed record
     FileNameLength : DWORD;
     FileName       : array[0..$FF] of WCHAR;
   end;

   TProcessBasicInformation = packed record
    ExitStatus       : NTSTATUS;
    PebBaseAddress   : pointer;
    AffinityMask     : DWORD;
    BasePrioryty     : DWORD;
    UnicueProcessID  : DWORD;
    InheritedFromPID : DWORD;
   end;

  _RTL_USER_PROCESS_PARAMETERS = packed record
    MaximumLength : ULONG;
    Length : ULONG;
    Flags : ULONG;
    DebugFlags : ULONG;
    ConsoleHandle : pointer;
    ConsoleFlags : ULONG;
    StdInputHandle : DWORD;
    StdOutputHandle : DWORD;
    StdErrorHandle : DWORD;
    CurrentDirectoryPath : UNICODE_STRING;
    CurrentDirectoryHandle : DWORD;
    DllPath : UNICODE_STRING;
    ImagePathName : UNICODE_STRING;
    CommandLine : UNICODE_STRING;
    Environment : Pointer;
    StartingPositionLeft : ULONG;
    StartingPositionTop : ULONG;
    Width : ULONG;
    Height : ULONG;
    CharWidth : ULONG;
    CharHeight : ULONG;
    ConsoleTextAttributes : ULONG;
    WindowFlags : ULONG;
    ShowWindowFlags : ULONG;
    WindowTitle : UNICODE_STRING;
    DesktopName : UNICODE_STRING;
    ShellInfo : UNICODE_STRING;
    RuntimeData : UNICODE_STRING
   end;
   // Функция для запроса системной информации
   TNativeQuerySystemInformation = function(
          SystemInformationClass  : SYSTEM_INFORMATION_CLASS;
          SystemInformation       : PVOID;
          SystemInformationLength : ULONG;
          ReturnLength            : PULONG
          ): NTSTATUS; stdcall;
   // Запрос информации о файле
   TNtQueryInformationFile      = function(
          AFileHandle     : THandle;
          IoStatusBlock   : PIO_STATUS_BLOCK;
          FileInformation : PVOID;
          Length          : ULONG;
          FileInformationClass : FILE_INFORMATION_CLASS) : NTSTATUS; stdcall;
  // Запрос информации о процессе
  TNtQueryInformationProcess = function (
          ProcessHandle   : THandle;
          ProcessInformationClass : _PROCESSINFOCLASS;
          ProcessInformation  : pointer;
          ProcessInformationLength : dword;
          ReturnLength    : PDWORD) : NTSTATUS; stdcall;

   TZwQueryInformationFile = TNtQueryInformationFile;
   // Запрос данных об объекте
   TZwQueryObject = function (
      ObjectHandle           : THandle;
      ObjectInformationClass : OBJECT_INFORMATION_CLASS;
      ObjectInformation      : PVOID;
      Length                 : ULONG;
      ReturnLength           : PULONG
                              ) : NTSTATUS; stdcall;

   KEY_INFORMATION_CLASS = (
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation);

  KEY_BASIC_INFORMATION = packed record
    LastWriteTime : LARGE_INTEGER;
    TitleIndex    : ULONG;
    NameLength    : ULONG;
    Name          : packed array [0..500] of WideChar;
  end;

  KEY_NAME_INFORMATION = packed record
    NameLength    : ULONG;
    Name          : packed array [0..$1FF] of WideChar;
  end;

  TObjectAttributes = packed record
    Length                   : DWORD;
    RootDirectory            : THandle;           // Handle родительского объекта
    ObjectName               : PUNICODE_STRING;   // Имя обънкта относительно род.
    Attributes               : DWORD;             // Атрибуты
    SecurityDescriptor       : Pointer;
    SecurityQualityOfService : Pointer;
   end;
   PObjectAttributes = ^TObjectAttributes;

   TZwQueryKey = function (
    KeyHandle : THandle;
    KeyInformationClass : KEY_INFORMATION_CLASS;
    KeyInformation : PVOID;
    KeyInformationLength : ULONG;
    ResultLength : PULONG) : NTSTATUS; stdcall;

   // Загрузка и выгрузка драйвера
   TZwLoadDriver = function (DriverServiceName : PUNICODE_STRING) : NTSTATUS; stdcall;
   TZwUnloadDriver = function (DriverServiceName : PUNICODE_STRING) : NTSTATUS; stdcall;
//  RtlNtStatusToDosError: Function(Status: NTSTATUS): ULONG; StdCall;
 // Прототипы функций
 TzOpenSCManager      = function (lpMachineName, lpDatabaseName: PChar; dwDesiredAccess: DWORD): SC_HANDLE; stdcall;
 TzEnumServicesStatus = function (hSCManager: SC_HANDLE; dwServiceType, dwServiceState: DWORD; var lpServices: TEnumServiceStatus; cbBufSize: DWORD; var pcbBytesNeeded, lpServicesReturned, lpResumeHandle: DWORD): BOOL stdcall;
 TzQueryServiceConfig = function (hService: SC_HANDLE; lpServiceConfig: PQueryServiceConfig; cbBufSize: DWORD; var pcbBytesNeeded: DWORD): BOOL stdcall;
 TzCloseServiceHandle = function (hSCObject: SC_HANDLE): BOOL stdcall;
 TzCreateService      = function (hSCManager: SC_HANDLE; lpServiceName, lpDisplayName: PChar;
                                  dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl: DWORD;
                                  lpBinaryPathName, lpLoadOrderGroup: PChar; lpdwTagId: LPDWORD; lpDependencies,
                                  lpServiceStartName, lpPassword: PChar): SC_HANDLE; stdcall;
 TzOpenService        = function (hSCManager: SC_HANDLE; lpServiceName: PChar; dwDesiredAccess: DWORD): SC_HANDLE stdcall;
 TzStartService       = function (hService: SC_HANDLE; dwNumServiceArgs: DWORD; var lpServiceArgVectors: PChar): BOOL; stdcall;
 TzDeleteService      = function (hService: SC_HANDLE): BOOL; stdcall;
 TzControlService     = function (hService: SC_HANDLE; dwControl: DWORD;
                                  var lpServiceStatus: TServiceStatus): BOOL; stdcall;
var
 zOpenSCManager          : TzOpenSCManager      = nil;
 zEnumServicesStatus     : TzEnumServicesStatus = nil;
 zQueryServiceConfig     : TzQueryServiceConfig = nil;
 zCloseServiceHandle     : TzCloseServiceHandle = nil;
 zCreateService          : TzCreateService      = nil;
 zOpenService            : TzOpenService        = nil;
 zStartService           : TzStartService       = nil;
 zDeleteService          : TzDeleteService      = nil;
 zControlService         : TzControlService     = nil;
 zNtQueryInformationFile    : TNtQueryInformationFile = nil;
 zNtQueryInformationProcess : TNtQueryInformationProcess = nil;
 zZwQueryKey : TZwQueryKey = nil;

 zZwLoadDriver    : TZwLoadDriver   = nil;
 zZwUnloadDriver  : TZwUnloadDriver = nil;

 // Загрузка API для работы с сервисами
 function LoadServiceManagerAPI : boolean;
 function LoadNTDllAPI : boolean;
 // Проверка платформы (true - платформа NT)
 function ISNT : boolean;
 // Формирование IRP кода (аналог макроса в C)
 function CTL_CODE(ADeviceType, AFunction, AMethod, AAccess : DWORD) : dword;
 Function zSetThreadLocale(Locale: LCID) : boolean;

 function CallZwLoadDriver(DriverServiceName : PUNICODE_STRING) : NTSTATUS;
 function CallZwUnLoadDriver(DriverServiceName : PUNICODE_STRING) : NTSTATUS;

 function zSetProcessAffinityMask(hProcess: THandle;
  dwProcessAffinityMask: DWORD): BOOL;

implementation

function CTL_CODE(ADeviceType, AFunction, AMethod, AAccess : DWORD) : dword;
begin
 Result := ((ADeviceType) shl 16) or ((AAccess) shl 14) or ((AFunction) shl 2) or (AMethod);
end;

// Проверка платформы (true - платформа NT)
function ISNT : boolean;
var
 Ver  : TOSVersionInfo;
begin
 Result := False;
 Ver.dwOSVersionInfoSize := SizeOf(TOSVersionInfo);
 GetVersionEx(Ver);
 case Ver.dwPlatformId of
  VER_PLATFORM_WIN32_NT      : Result := True;   //Windows NT - подходит
  VER_PLATFORM_WIN32_WINDOWS : Result := False;  //Windows 9x-Me - подходит
  VER_PLATFORM_WIN32s        : Result := False;  //Windows 3.x - не подходит
 end;
end;

function LoadServiceManagerAPI : boolean;
var
 LibHandle    : THandle;
begin
 Result := false;
 if @zOpenSCManager = nil then begin
  LibHandle := 0;
  zOpenSCManager      := nil;
  zEnumServicesStatus := nil;
  zQueryServiceConfig := nil;
  zCloseServiceHandle := nil;
  zOpenService        := nil;
  zStartService       := nil;
  zDeleteService      := nil;
  zControlService     := nil;
  // Загрузка таблицы сервисов производится только для NT
  if ISNT then begin
   LibHandle := LoadLibrary('advapi32.dll');
   if LibHandle = 0 then exit;
   @zOpenSCManager      := GetProcAddress(LibHandle, 'OpenSCManagerA');
   @zOpenService        := GetProcAddress(LibHandle, 'OpenServiceA');
   @zEnumServicesStatus := GetProcAddress(LibHandle, 'EnumServicesStatusA');
   @zQueryServiceConfig := GetProcAddress(LibHandle, 'QueryServiceConfigA');
   @zCloseServiceHandle := GetProcAddress(LibHandle, 'CloseServiceHandle');
   @zStartService       := GetProcAddress(LibHandle, 'StartServiceA');
   @zCreateService      := GetProcAddress(LibHandle, 'CreateServiceA');
   @zDeleteService      := GetProcAddress(LibHandle, 'DeleteService');
   @zControlService     := GetProcAddress(LibHandle, 'ControlService');
  end;
 end;
 // Проверка результата загрузки
 Result := (@zOpenSCManager <> nil) and (@zOpenService <> nil) and
               (@zEnumServicesStatus <> nil) and (@zQueryServiceConfig <> nil) and
               (@zCloseServiceHandle <> nil) and (@zStartService <> nil) and
               (@zCreateService <> nil);
end;

function LoadNTDllAPI : boolean;
var
 LibHandle    : THandle;
begin
 Result := false;
 @zNtQueryInformationFile := nil;
 if ISNT then begin
  LibHandle := LoadLibrary('ntdll.dll');
  @zNtQueryInformationFile    := GetProcAddress(LibHandle, 'NtQueryInformationFile');
  @zNtQueryInformationProcess := GetProcAddress(LibHandle, 'NtQueryInformationProcess');
  @zZwLoadDriver              := GetProcAddress(LibHandle, 'ZwLoadDriver');
  @zZwUnloadDriver            := GetProcAddress(LibHandle, 'ZwUnloadDriver');
  @zZwQueryKey                := GetProcAddress(LibHandle, 'ZwQueryKey');
 end;
 Result := @zNtQueryInformationFile <> nil;
end;

function CallZwLoadDriver(DriverServiceName : PUNICODE_STRING) : NTSTATUS;
begin
 Result := 1;
 if @zZwLoadDriver = nil then LoadNTDllAPI;
 if @zZwLoadDriver = nil then exit;
 Result := zZwLoadDriver(DriverServiceName);
end;

function CallZwUnloadDriver(DriverServiceName : PUNICODE_STRING) : NTSTATUS;
begin
 Result := 1;
 if @zZwUnLoadDriver = nil then LoadNTDllAPI;
 if @zZwUnLoadDriver = nil then exit;
 Result := zZwUnLoadDriver(DriverServiceName);
end;

Function zSetThreadLocale(Locale: LCID) : boolean;
type
 TSetThreadLocale = function (Locale: LCID): BOOL; stdcall;
var
 h : THandle;
 zSetThreadLocale : TSetThreadLocale;
begin
 if ISNT then begin
  h := GetModuleHandle('Kernel32.dll');
  @zSetThreadLocale := GetProcAddress(h, 'SetThreadLocale');
  if @zSetThreadLocale <> nil then
   zSetThreadLocale(Locale);
 end;
end;

function zSetProcessAffinityMask(hProcess: THandle;
  dwProcessAffinityMask: DWORD): BOOL;
type
 TSetProcessAffinityMask = function (hProcess: THandle;
  dwProcessAffinityMask: DWORD): BOOL; stdcall;
var
 h : THandle;
 Tmp : TSetProcessAffinityMask;
begin
 Result := false;
 if ISNT then begin
  h := GetModuleHandle('Kernel32.dll');
  @Tmp := GetProcAddress(h, 'SetProcessAffinityMask');
  if @Tmp <> nil then
   Result := Tmp(hProcess, dwProcessAffinityMask);
 end;
end;

begin
 LoadNTDllAPI;
end.

.text:0044065F                      arg_0           = dword ptr  4
.text:0044065F                      arg_4           = dword ptr  8
.text:0044065F                      arg_8           = dword ptr  0Ch
.text:0044065F
.text:0044065F 8B 4C 24 04                          mov     ecx, [esp+arg_0]
.text:00440663 8B 44 24 08                          mov     eax, [esp+arg_4]
.text:00440667 8B 54 24 0C                          mov     edx, [esp+arg_8]
.text:0044066B 0F 30                                wrmsr
.text:0044066D C2 0C 00                             retn    0Ch
.text:0044066D                      _WRMSR          endp

.text:0044061A 74 31                                jz      short loc_44064D
.text:0044061C 6A 00                                push    0
.text:0044061E 6A 08                                push    8
.text:00440620 68 74 01 00 00                       push    174h
.text:00440625 E8 35 00 00 00                       call    _WRMSR
.text:0044062A 6A 00                                push    0
.text:0044062C 68 0F 6E 40 00                       push    (offset dword_406DAD+62h)
.text:00440631 68 76 01 00 00                       push    176h
.text:00440636 E8 24 00 00 00                       call    _WRMSR
.text:0044063B 6A 00                                push    0
.text:0044063D FF B6 68 08 00 00                    push    dword ptr [esi+868h]
.text:00440643 68 75 01 00 00                       push    175h
.text:00440648 E8 12 00 00 00                       call    _WRMSR


