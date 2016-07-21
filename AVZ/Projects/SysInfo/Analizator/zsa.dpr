library zsa;

uses
  windows,  SysUtils,  orautil,  afxCodeHook,  oracle,  urlmon,  wininet,  winsock,
  inifiles,  Classes,  ShellAPI,  ZSharedFunctions,  SyncObjs,  md5,  winsvc, ntdll;

type
 // Информация о перехваченной функции
 TInterceptInfo = record
  LibraryName   : string;  // Имя DLL
  FunctionName  : string;  // Имя функции
  FunctionAddr  : Pointer; // Адрес функции
  HookAddr      : Pointer; // Адрес перехватчика
  FunctCode     : packed array [0..20] of byte; // Первые байты кода функции
  FunctCodeSize : byte;   // Размер кода функции, перемещенного руткитом
  HookBuf       : packed array [0..20] of byte; // Первые команды функции + JMP
  HookJMP       : packed array [0..4] of byte; // JMP на перехватчик
 end;
var
 InterceptedFunctions : array of TInterceptInfo;
 // Описание перехваченных функций
 CreateFileAInterceptInfo,   CreateFileWInterceptInfo,
 WriteFileInterceptInfo, WriteFileExInterceptInfo,
 WriteProcessMemoryInterceptInfo, ReadProcessMemoryInterceptInfo,
 URLDownloadToFileAInterceptInfo, URLDownloadToFileWInterceptInfo,
 InternetOpenUrlAInterceptInfo, InternetOpenUrlWInterceptInfo,
 gethostbynameInterceptInfo,
 CreateProcessAInterceptInfo,  CreateProcessWInterceptInfo,
 HttpOpenRequestAInterceptInfo, HttpOpenRequestWInterceptInfo,
 InternetConnectAInterceptInfo, InternetConnectWInterceptInfo,
 CreateRemoteThreadInterceptInfo,
 RegCreateKeyExAInterceptInfo, RegCreateKeyExWInterceptInfo,
 RegSetValueExAInterceptInfo, RegSetValueExWInterceptInfo,
 RegDeleteKeyAInterceptInfo, RegDeleteKeyWInterceptInfo,
 RegDeleteValueAInterceptInfo, RegDeleteValueWInterceptInfo,
 CopyFileExAInterceptInfo,  CopyFileExWInterceptInfo,
 WinExecInterceptInfo,
 DeleteFileAInterceptInfo, DeleteFileWInterceptInfo,
 ExitProcessInterceptInfo,
 SetWindowsHookExAInterceptInfo, SetWindowsHookExWInterceptInfo,
 ShellExecuteExAInterceptInfo, ShellExecuteExWInterceptInfo,
 CreateServiceAInterceptInfo,  CreateServiceWInterceptInfo,
 DeleteServiceInterceptInfo,
 OpenServiceAInterceptInfo, OpenServiceWInterceptInfo,
 EnumServicesStatusAInterceptInfo, EnumServicesStatusWInterceptInfo,
 LoadLibraryAInterceptInfo, LoadLibraryWInterceptInfo,
 LoadLibraryExAInterceptInfo, LoadLibraryExWInterceptInfo,
 TerminateProcessInterceptInfo,
 SendInterceptInfo,
 OpenProcessInterceptInfo,
 SetThreadContextInterceptInfo,
 ZwLoadDriverInterceptInfo, ZwUnLoadDriverInterceptInfo : TInterceptInfo;

type
 TCreateFileA         = function (lpFileName: PAnsiChar; dwDesiredAccess, dwShareMode: DWORD;
                                 lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
                                 hTemplateFile: THandle): THandle; stdcall;
 TCreateFileW         = function (lpFileName: PWideChar; dwDesiredAccess, dwShareMode: DWORD;
                                 lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
                                 hTemplateFile: THandle): THandle; stdcall;

 TWriteFile           = function (hFile: THandle; const Buffer; nNumberOfBytesToWrite: DWORD;
                                  var lpNumberOfBytesWritten: DWORD; lpOverlapped: POverlapped): BOOL; stdcall;
 TWriteFileEx         = function (hFile: THandle; lpBuffer: Pointer; nNumberOfBytesToWrite: DWORD;
                                  const lpOverlapped: TOverlapped; lpCompletionRoutine: FARPROC): BOOL; stdcall;
 TWriteProcessMemory  = function (hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
                                  nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL; stdcall;
 TReadProcessMemory   = function (hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
                                  nSize: DWORD; var lpNumberOfBytesRead: DWORD): BOOL; stdcall;
 TURLDownloadToFileA  = function (Caller: IUnknown; URL: PAnsiChar; FileName: PAnsiChar; Reserved: DWORD; StatusCB: IBindStatusCallback): HResult; stdcall;
 TURLDownloadToFileW  = function (Caller: IUnknown; URL: PWideChar; FileName: PWideChar; Reserved: DWORD; StatusCB: IBindStatusCallback): HResult; stdcall;

 TInternetOpenUrlA    = function (hInet: HINTERNET; lpszUrl: PAnsiChar;
                                 lpszHeaders: PAnsiChar; dwHeadersLength: DWORD; dwFlags: DWORD;
                                 dwContext: DWORD): HINTERNET; stdcall;
 TInternetOpenUrlW    = function (hInet: HINTERNET; lpszUrl: PWideChar;
                                 lpszHeaders: PWideChar; dwHeadersLength: DWORD; dwFlags: DWORD;
                                 dwContext: DWORD): HINTERNET; stdcall;
 THttpOpenRequestA    = function(hConnect: HINTERNET; lpszVerb: PAnsiChar;
  lpszObjectName: PAnsiChar; lpszVersion: PAnsiChar; lpszReferrer: PAnsiChar;
  lplpszAcceptTypes: PLPSTR; dwFlags: DWORD;
  dwContext: DWORD): HINTERNET; stdcall;
 THttpOpenRequestW    = function (hConnect: HINTERNET; lpszVerb: PWideChar;
  lpszObjectName: PWideChar; lpszVersion: PWideChar; lpszReferrer: PWideChar;
  lplpszAcceptTypes: PLPWSTR; dwFlags: DWORD;
  dwContext: DWORD): HINTERNET; stdcall;
 TInternetConnectA = function (hInet: HINTERNET; lpszServerName: PAnsiChar;
  nServerPort: INTERNET_PORT; lpszUsername: PAnsiChar; lpszPassword: PAnsiChar;
  dwService: DWORD; dwFlags: DWORD; dwContext: DWORD): HINTERNET; stdcall;
 TInternetConnectW = function (hInet: HINTERNET; lpszServerName: PWideChar;
  nServerPort: INTERNET_PORT; lpszUsername: PWideChar; lpszPassword: PWideChar;
  dwService: DWORD; dwFlags: DWORD; dwContext: DWORD): HINTERNET; stdcall;
 Tgethostbyname       = function (name: PChar): PHostEnt; stdcall;
 TCreateProcessA      = function (lpApplicationName: PAnsiChar; lpCommandLine: PAnsiChar;
  lpProcessAttributes, lpThreadAttributes: PSecurityAttributes;
  bInheritHandles: BOOL; dwCreationFlags: DWORD; lpEnvironment: Pointer;
  lpCurrentDirectory: PAnsiChar; const lpStartupInfo: TStartupInfo;
  var lpProcessInformation: TProcessInformation): BOOL; stdcall;
 TCreateProcessW = function (lpApplicationName: PWideChar; lpCommandLine: PWideChar;
  lpProcessAttributes, lpThreadAttributes: PSecurityAttributes;
  bInheritHandles: BOOL; dwCreationFlags: DWORD; lpEnvironment: Pointer;
  lpCurrentDirectory: PWideChar; const lpStartupInfo: TStartupInfo;
  var lpProcessInformation: TProcessInformation): BOOL; stdcall;
 TCreateRemoteThread = function (hProcess: THandle; lpThreadAttributes: Pointer;
  dwStackSize: DWORD; lpStartAddress: TFNThreadStartRoutine; lpParameter: Pointer;
  dwCreationFlags: DWORD; var lpThreadId: DWORD): THandle; stdcall;
 TRegCreateKeyExA = function (hKey: HKEY; lpSubKey: PAnsiChar;
  Reserved: DWORD; lpClass: PAnsiChar; dwOptions: DWORD; samDesired: REGSAM;
  lpSecurityAttributes: PSecurityAttributes; var phkResult: HKEY;
  lpdwDisposition: PDWORD): Longint; stdcall;
 TRegCreateKeyExW = function (hKey: HKEY; lpSubKey: PWideChar;
  Reserved: DWORD; lpClass: PWideChar; dwOptions: DWORD; samDesired: REGSAM;
  lpSecurityAttributes: PSecurityAttributes; var phkResult: HKEY;
  lpdwDisposition: PDWORD): Longint; stdcall;
 TRegSetValueExA = function (hKey: HKEY; lpValueName: PAnsiChar;
  Reserved: DWORD; dwType: DWORD; lpData: Pointer; cbData: DWORD): Longint; stdcall;
 TRegSetValueExW = function (hKey: HKEY; lpValueName: PWideChar;
  Reserved: DWORD; dwType: DWORD; lpData: Pointer; cbData: DWORD): Longint; stdcall;
 TRegDeleteKeyA = function (hKey: HKEY; lpSubKey: PAnsiChar): Longint; stdcall;
 TRegDeleteKeyW = function (hKey: HKEY; lpSubKey: PWideChar): Longint; stdcall;
 TRegDeleteValueA = function (hKey: HKEY; lpValueName: PAnsiChar): Longint; stdcall;
 TRegDeleteValueW = function (hKey: HKEY; lpValueName: PWideChar): Longint; stdcall;
 TCopyFileExA     = function (lpExistingFileName, lpNewFileName: PAnsiChar;
  lpProgressRoutine: TFNProgressRoutine; lpData: Pointer; pbCancel: PBool;
  dwCopyFlags: DWORD): BOOL; stdcall;
 TCopyFileExW     = function (lpExistingFileName, lpNewFileName: PWideChar;
  lpProgressRoutine: TFNProgressRoutine; lpData: Pointer; pbCancel: PBool;
  dwCopyFlags: DWORD): BOOL; stdcall;
 TWinExec = function (lpCmdLine: LPCSTR; uCmdShow: UINT): UINT; stdcall;
 TDeleteFileA = function (lpFileName: PAnsiChar): BOOL; stdcall;
 TDeleteFileW = function (lpFileName: PWideChar): BOOL; stdcall;
 TExitProcess = procedure (uExitCode: UINT); stdcall;
 TShellExecuteExA = function (lpExecInfo: PShellExecuteInfoA):BOOL; stdcall;
 TShellExecuteExW = function (lpExecInfo: PShellExecuteInfoW):BOOL; stdcall;
 TSetWindowsHookExA = function (idHook: Integer; lpfn: TFNHookProc; hmod: HINST; dwThreadId: DWORD): HHOOK; stdcall;
 TSetWindowsHookExW = function (idHook: Integer; lpfn: TFNHookProc; hmod: HINST; dwThreadId: DWORD): HHOOK; stdcall;
 TCreateServiceA  = function (hSCManager: SC_HANDLE; lpServiceName, lpDisplayName: PAnsiChar;
  dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl: DWORD;
  lpBinaryPathName, lpLoadOrderGroup: PAnsiChar; lpdwTagId: LPDWORD; lpDependencies,
  lpServiceStartName, lpPassword: PAnsiChar): SC_HANDLE; stdcall;
 TCreateServiceW = function (hSCManager: SC_HANDLE; lpServiceName, lpDisplayName: PWideChar;
  dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl: DWORD;
  lpBinaryPathName, lpLoadOrderGroup: PWideChar; lpdwTagId: LPDWORD; lpDependencies,
  lpServiceStartName, lpPassword: PWideChar): SC_HANDLE; stdcall;
 TDeleteService = function (hService: SC_HANDLE): BOOL; stdcall;
 TOpenServiceA = function (hSCManager: SC_HANDLE; lpServiceName: PAnsiChar;
  dwDesiredAccess: DWORD): SC_HANDLE; stdcall;
 TOpenServiceW = function (hSCManager: SC_HANDLE; lpServiceName: PWideChar;
  dwDesiredAccess: DWORD): SC_HANDLE; stdcall;
 TEnumServicesStatusA = function (hSCManager: SC_HANDLE; dwServiceType,
  dwServiceState: DWORD; var lpServices: TEnumServiceStatusA; cbBufSize: DWORD;
  var pcbBytesNeeded, lpServicesReturned, lpResumeHandle: DWORD): BOOL; stdcall;
 TEnumServicesStatusW = function (hSCManager: SC_HANDLE; dwServiceType,
  dwServiceState: DWORD; var lpServices: TEnumServiceStatusW; cbBufSize: DWORD;
  var pcbBytesNeeded, lpServicesReturned, lpResumeHandle: DWORD): BOOL; stdcall;
 TLoadLibraryA   = function (lpLibFileName: PAnsiChar): HMODULE; stdcall;
 TLoadLibraryW   = function (lpLibFileName: PWideChar): HMODULE; stdcall;
 TLoadLibraryExA = function (lpLibFileName: PAnsiChar; hFile: THandle; dwFlags: DWORD): HMODULE; stdcall;
 TLoadLibraryExW = function (lpLibFileName: PWideChar; hFile: THandle; dwFlags: DWORD): HMODULE; stdcall;
 TOpenProcess    = function (dwDesiredAccess: DWORD; bInheritHandle: BOOL; dwProcessId: DWORD): THandle; stdcall;
 TTerminateProcess = function (hProcess: THandle; uExitCode: UINT): BOOL; stdcall;
 Tsend = function (s: TSocket; var Buf; len, flags: Integer): Integer; stdcall;
 TSetThreadContext = function (hThread: THandle; const lpContext: TContext): BOOL; stdcall;
 TZwLoadDriver = function (DriverServiceName : PUNICODE_STRING) : NTSTATUS; stdcall;
 TZwUnloadDriver = function (DriverServiceName : PUNICODE_STRING) : NTSTATUS; stdcall;


var
 // Признак того, что перехваты мониторинга установлены
 HooksInjected  : boolean = false;
 // Полное имя и контрольная сумма файла
 FullModuleName, FileMD5 : string;
 // Сессия Oracle
 OracleSession  : TOracleSession;
 // SQL код протоколирования
 LogOracleQuery : TOracleQuery;
 // Критические секции
 OracleCS, QuarantineCS, GlobalThreadCounterCS : TCriticalSection;
 // Глобальный счетчик потоков
 GlobalThreadCounter : integer = 0;

threadvar
 LockLogCnt        : integer;
 ThreadInternalID  : integer;

procedure LockLog;
begin
 LockLogCnt := LockLogCnt + 1;
end;

procedure UnLockLog;
begin
 if LockLogCnt > 0 then
  LockLogCnt := LockLogCnt - 1;
end;

function GetThreadInternalID : integer;
begin
 try
  GlobalThreadCounterCS.Enter;
  GlobalThreadCounter := GlobalThreadCounter + 1;
  Result := GlobalThreadCounter;
 finally
  GlobalThreadCounterCS.Leave;
 end;
end;

// Модификация машинного кода функции
function SetHookCode(InterceptInfo : TInterceptInfo; ASetHook : boolean) : boolean;
var
 Tmp, OldProtect     : dword;
begin
 // 1. Настройка защиты
 VirtualProtect(InterceptInfo.FunctionAddr, InterceptInfo.FunctCodeSize, PAGE_EXECUTE_READWRITE, OldProtect);
 // 2. Запись в первые байты машинного кода функции
 if ASetHook then
  Result := WriteProcessMemory(GetCurrentProcess, InterceptInfo.FunctionAddr,
                     @InterceptInfo.HookJMP[0], 5, Tmp)
 else
  Result := WriteProcessMemory(GetCurrentProcess, InterceptInfo.FunctionAddr,
                     @InterceptInfo.FunctCode[0], InterceptInfo.FunctCodeSize, Tmp);
 // 3. Восстановление атрибутов защиты
 VirtualProtect(InterceptInfo.FunctionAddr, InterceptInfo.FunctCodeSize, OldProtect, Tmp);
end;

function GetCodeSize(APtr : pointer) : integer;
begin
 // Вызов функции из afxCodeHook.pas
 Result := SizeOfCode(APtr);
end;

function InterceptFunctionEx(ALibName, AFunctName : string; var InterceptInfo : TInterceptInfo; HookFunct: Pointer) : boolean;
var
 Tmp, Tmp1 : dword;
 JMP_Rel   : dword;
 HModule   : THandle;
begin
 Result := false;
 HModule := GetModuleHandle(PChar(ALibName));
 if (HModule = 0) or (HModule = INVALID_HANDLE_VALUE) then
  HModule := LoadLibrary(PChar(ALibName));
 // 1. Поиск адреса фукции
 InterceptInfo.FunctionAddr := GetProcAddress(HModule, PChar(AFunctName));
 if InterceptInfo.FunctionAddr = nil then exit;
 // 2. Сохранение параметров в структуре
 InterceptInfo.LibraryName  := ALibName;
 InterceptInfo.FunctionName := AFunctName;
 InterceptInfo.HookAddr     := HookFunct;
 // 3. Определение размера копируемых данных
 InterceptInfo.FunctCodeSize := 0;
 while InterceptInfo.FunctCodeSize < 5 do begin
  Tmp := GetCodeSize(pointer(dword(InterceptInfo.FunctionAddr) + InterceptInfo.FunctCodeSize));
  if Tmp <= 0 then exit;
  inc(InterceptInfo.FunctCodeSize, Tmp);
 end;
 // 4. Считывание машинного кода функции
 Result := ReadProcessMemory(GetCurrentProcess,
                    InterceptInfo.FunctionAddr,
                    @InterceptInfo.FunctCode[0], InterceptInfo.FunctCodeSize, Tmp);
 if not(Result) then exit;
 // Подготовка буфера с командой JMP на функцию - перехватчик
 JMP_Rel := DWORD(HookFunct) - (DWORD(InterceptInfo.FunctionAddr) + 5);
 InterceptInfo.HookJMP[0] := $0E9;
 CopyMemory(@InterceptInfo.HookJMP[1], @JMP_Rel, 4);
 // Копирование кода функции в буфер руткита
 CopyMemory(@InterceptInfo.HookBuf[0], @InterceptInfo.FunctCode[0], InterceptInfo.FunctCodeSize);
 Tmp := 0;
 while Tmp < InterceptInfo.FunctCodeSize do begin
  case InterceptInfo.HookBuf[Tmp] of
   $E8, $E9 : begin
               Tmp1 := dword(pointer(@InterceptInfo.HookBuf[Tmp+1])^);
               Tmp1 := Tmp1 + dword(InterceptInfo.FunctionAddr) - dword(pointer(@InterceptInfo.HookBuf));
               dword(pointer(@InterceptInfo.HookBuf[Tmp+1])^) := Tmp1;
              end;
  end;
  Tmp := Tmp + GetCodeSize(@InterceptInfo.HookBuf[Tmp]);
 end;
 // Подготовка буфера с командой JMP на продолжение перехватываемой функции
 JMP_Rel := (DWORD(InterceptInfo.FunctionAddr) + InterceptInfo.FunctCodeSize + 5) -
            (DWORD(@InterceptInfo.HookBuf[0])+InterceptInfo.FunctCodeSize+5+5);
 InterceptInfo.HookBuf[InterceptInfo.FunctCodeSize] := $0E9;
 CopyMemory(@InterceptInfo.HookBuf[InterceptInfo.FunctCodeSize+1], @JMP_Rel, 4);
 VirtualProtect(@InterceptInfo.HookBuf[0], InterceptInfo.FunctCodeSize+5, PAGE_EXECUTE_READWRITE, Tmp);
 // Установка перехватчика
 Result := SetHookCode(InterceptInfo, true);
 if Result then begin
  SetLength(InterceptedFunctions, Length(InterceptedFunctions) + 1);
  InterceptedFunctions[Length(InterceptedFunctions) - 1] := InterceptInfo;
 end;
end;

// Восстановление функции
function RestoreFunctionEx(InterceptInfo : TInterceptInfo) : boolean;
begin
 // Снятие перехватчика
 Result := SetHookCode(InterceptInfo, false);
end;

function InjectDLLtoProcess(hProcess: THandle; ADllName: string): boolean;
var
 hRemoteThread  : THandle; // Handle удаленного потока
 NameBufPtr     : Pointer; // Адрес буфера с именем DLL
 LoadLibraryPtr : Pointer; // Адрес функции LoadLibrary
 NumberOfBytesWritten, ThreadId : dword;
begin
 Result := false;
 hRemoteThread := 0; NameBufPtr := nil;
 try
  if hProcess = 0 then exit;
  // 2. Создаем в памяти процесса буфер для имени DLL
  NameBufPtr := VirtualAllocEx(hProcess, nil, Length(ADllName)+1, MEM_COMMIT, PAGE_READWRITE);
  if NameBufPtr = nil then begin
   exit;
  end;
  // 3. Копируем имя в буфер
  if not(WriteProcessMemory(hProcess, NameBufPtr, PChar(ADllName), Length(ADllName)+1, NumberOfBytesWritten)) then begin
   exit;
  end;
  // 4. Выполняем определение адреса kernel32.dll!LoadLibraryA
  LoadLibraryPtr := GetProcAddress(GetModuleHandle('kernel32.dll'), 'LoadLibraryA');
  if LoadLibraryPtr = nil then begin
   exit;
  end;
  // 5. Выполняем создание удаленного потока
  hRemoteThread := CreateRemoteThread(hProcess, nil, 0,
                               LoadLibraryPtr, NameBufPtr, 0, ThreadId);
  if hRemoteThread <> 0 then begin
   // 6. Дожидаемся завершения потока (ждем 5 секунд)
   WaitForSingleObject(hRemoteThread, 500);
   Result := true;
  end;
 finally
  // Освобождение памяти и закрытие Handle потока и процесса
  if NameBufPtr <> nil then
   VirtualFreeEx(hProcess, NameBufPtr, 0, MEM_RELEASE);
  if hRemoteThread <> 0 then
   CloseHandle(hRemoteThread);
 end;
end;

function InjectDllAlt(Process, hThread: dword; ModulePath: PChar): boolean;
var
  Context: _CONTEXT;
  InjData:  packed record
             OldEip: dword;
             OldEsi: dword;
             AdrLoadLibrary: pointer;
             AdrLibName: pointer;
            end;

  Procedure Injector();
  asm
    pushad
    db $E8              // опкод call short 0
    dd 0                //
    pop eax             // eax - адрес текущей инструкции
    add eax, $12
    mov [eax], esi      // модифицируем операнд dd $00000000
    push [esi + $0C]    // кладем в стек имя DLL
    call [esi + $08]    // call LoadLibraryA
    popad
    mov esi, [esi + $4] // восстанавливаем esi из старого контекста
    dw $25FF            // опкод Jmp dword ptr [00000000h]
    dd $00000000        // модифицируемый операнд
    ret
  end;

begin
  Result := false;
  if hThread = 0 then Exit;
  SuspendThread(hThread);
  //сохраняем старый контекст
  Context.ContextFlags := CONTEXT_FULL;
  GetThreadContext(hThread, Context);
  //подготавливаем данные для внедряемого кода
  InjData.OldEip := Context.Eip;
  InjData.OldEsi := Context.Esi;
  InjData.AdrLoadLibrary  := GetProcAddress(GetModuleHandle('kernel32.dll'),
                                            'LoadLibraryA');
  InjData.AdrLibName := InjectString(Process, ModulePath);
  if InjData.AdrLibName = nil then Exit;
  //внедряем данные и устанавливаем ebp контекста
  Context.Esi := dword(InjectMemory(Process, @InjData, SizeOf(InjData)));
  //внедряем код
  Context.Eip := dword(InjectMemory(Process, @Injector, SizeOfProc(@Injector)));
  //устанавливаем новый контекст
  SetThreadContext(hThread, Context);
  ResumeThread(hThread);
  Result := true;
end;


procedure DumpCurrentProcess(ADumpName : string);
var
 Buf      : packed array of byte;
 Base, Size, lpNumberOfBytesRead: DWORD;
 f        : file;
 ProcessBasicInfo : TProcessBasicInformation;
 ReturnLength    : DWORD;
begin
 try
   LockLog;
   try
     // Запрос информации о процессе
     ZeroMemory(@ProcessBasicInfo, sizeof(ProcessBasicInfo));
     zNtQueryInformationProcess(GetCurrentProcess, ProcessBasicInformation,
                                @ProcessBasicInfo, sizeof(ProcessBasicInfo),
                                @ReturnLength);
     ForceDirectories(ExtractFilePath(ADumpName));
     SetLength(Buf, Size);
     // Очитска буфера
     ZeroMemory(@Buf[0], Size);
     // Чтение памяти процесса
     ReadProcessMemory(GetCurrentProcess, pointer(Base), @Buf[0], Size, lpNumberOfBytesRead);
     // Сохранение полученных данных
     AssignFile(F, ADumpName); Rewrite(f, 1);
     BlockWrite(F, Buf[0], Size);
     CloseFile(F);
     Buf := nil;
   except
   end;
 finally
  UnLockLog;
 end;
end;

function AddInterceptInfo(FunctCode : integer; dwData : DWord = 0; stData : string = ''; stData1 : string = '') : integer;
begin
 try
   OracleCS.Enter;
   Result := -1;
   if not(HooksInjected) then exit;
   if LockLogCnt > 0 then exit;
   try
    LockLog;
    try
     LogOracleQuery.SetVariable('PID', GetCurrentProcessId);
     LogOracleQuery.SetVariable('OP_CODE', FunctCode);
     LogOracleQuery.SetVariable('DW_PARAM1', dwData);
     LogOracleQuery.SetVariable('ST_PARAM1', stData);
     LogOracleQuery.SetVariable('ST_PARAM2', stData1);
     LogOracleQuery.SetVariable('OP_TID', ThreadInternalID);
     LogOracleQuery.Execute;
     LogOracleQuery.Close;
     OracleSession.Commit;
    except end;
   finally
    UnLockLog;
   end;
 finally
  OracleCS.Leave;
 end;
end;

Function CopyToInfected_(AFileName, AVirus : string) : boolean;
var
 InfectedFolder   : string;
 InfectedFileName : string;
 AFolder : string;
 Year, Month, Day : word;
 BaseNum, FileSize   : integer;
 INI              : TIniFile;
 FS, FS1          : TFileStream;
 S                : string;
 StrSize          : dword;
 FileDate         : LongInt;
begin
 try
 Result := false;
 AFolder := 'Analizator';
 if Trim(AFileName) = '' then exit;
 try
  LockLog;
  if not(FileExists(AFileName)) then exit;
   FS := nil; FS1 := nil;
   try
     if Trim(AFileName) = '' then exit;
     // Формирование базового пути
     InfectedFolder := ExtractFilePath(FullModuleName) + AFolder + '\';
     // Формирование динамической части (год-месяц-день)
     DecodeDate(Now, Year, Month, Day);
     InfectedFolder := InfectedFolder + IntToStr(Year) + '-' +
                       FormatFloat('00', Month) + '-' + FormatFloat('00', Day)+'\';

     // Создание папки для инфицированных файлов
     ForceDirectories(InfectedFolder);
     // Создание имени инфицированного файла
     BaseNum := 1;
     // Фаза 1 - быстрый поиск
     while FileExists(InfectedFolder + 'avz' + FormatFloat('#00000', BaseNum)+'.dta') do begin   //#DNL
      INI   :=  TIniFile.Create(InfectedFolder + 'avz' + FormatFloat('#00000', BaseNum)+'.ini'); //#DNL
      if LowerCase(INI.ReadString('InfectedFile', 'Src', '')) = LowerCase(AFileName) then begin  //#DNL
       INI.Free;
       exit;
      end;
      INI.Free;
      inc(BaseNum, 1);
     end;
     InfectedFileName := 'avz' + FormatFloat('#00000', BaseNum)+'.dta'; //#DNL
     // Копирование файла
     try
      FS  := nil; FS1 := nil;
      FS  := TFileStream.Create(AFileName, fmOpenRead or fmShareDenyNone);
      FS1 := TFileStream.Create(InfectedFolder + InfectedFileName, fmCreate);
      FS.Seek(0,0);
      FS1.Seek(0,0);
      FS1.CopyFrom(FS, FS.Size);
      FS.Free;  FS  := nil;
      FileSize := FS1.Size;
      FS1.Free; FS1 := nil;
      // Копирование даты
      try
       FileDate := FileAge(AFileName);
       FileSetDate(InfectedFolder + InfectedFileName, FileDate);
      except
       CopyFile(PChar(AFileName), PChar(InfectedFolder + InfectedFileName), false);
      end;
      // Запись информации о файле
      INI   :=  TIniFile.Create(InfectedFolder + ChangeFileExt(InfectedFileName, '.ini')); //#DNL
      INI.WriteString('InfectedFile', 'Src', AFileName);             //#DNL
      INI.WriteString('InfectedFile', 'Infected', InfectedFileName); //#DNL
      INI.WriteString('InfectedFile', 'Virus', AVirus);              //#DNL
      INI.WriteDateTime('InfectedFile', 'QDate', Now);               //#DNL
      INI.WriteInteger('InfectedFile', 'Size', FileSize);            //#DNL
      INI.WriteDateTime('InfectedFile', 'FileDate', FileDateToDateTime(FileDate)); //#DNL
      INI.Free;
      Result := true;
     except
      FS.Free;
      FS1.Free;
     end;
   except
    Result := false;
   end;
 finally
  UnLockLog;
 end;
 except end;
end;

Function CopyToInfectedEx(AFileName, AVirus : string) : boolean;
var
 S : string;
 i : integer;
begin
 try
 try
  QuarantineCS.Enter;
  LockLog;
  S := Trim(AFileName);
  S := StringReplace(S, '"', '', [rfReplaceAll]);
  CopyToInfected_(S, AVirus);
  if pos('\??\', S) = 1 then
   CopyToInfected_(copy(S, 5, maxint), AVirus);
  for i := 1 to Length(S) do
   if S[i] in [' ', ',', ';'] then
    CopyToInfected_(copy(S, 1, i-1), AVirus);
 finally
  UnLockLog;
  QuarantineCS.Leave;
 end;
 except end;
end;

procedure DumpMem(lpBuffer : pointer; lpBaseAddress, nSize : dword);
var
 Path, Fname : string;
 x : integer;
 F    : file;
begin
 if LockLogCnt > 0 then exit;
 try
  try
   LockLog;
   Path := ExtractFilePath(FullModuleName) +'mem';
   ForceDirectories(Path);
   Fname := Path+'\'+FileMD5+'_'+IntToHex(dword(lpBaseAddress), 8);
   x := 0;
   while FileExists(Fname+'_'+inttostr(x)+'.mem') do
    inc(x);
   AssignFile(F, Fname+'_'+inttostr(x)+'.mem');
   Rewrite(F, 1);
   BlockWrite(F, lpBuffer^, nSize);
   CloseFile(F);
  finally
   UnLockLog;
  end;
 except
 end;
end;

// *****************************************************************************************************
// ************************ Перехватчики для мониторинга вызовов функций *******************************
// *****************************************************************************************************

function MyCreateFileA(lpFileName: PAnsiChar; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;
begin
  try AddInterceptInfo(1001, dwDesiredAccess, lpFileName); except end;
  Result := TCreateFileA(@CreateFileAInterceptInfo.HookBuf[0])(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,dwFlagsAndAttributes,
                        hTemplateFile);
end;

function MyCreateFileW(lpFileName: PWideChar; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;
begin
  try AddInterceptInfo(1001, dwDesiredAccess, WideCharToString(lpFileName)); except end;
  Result := TCreateFileW(@CreateFileWInterceptInfo.HookBuf[0])(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,dwFlagsAndAttributes,
                        hTemplateFile);
end;

function MyWriteFile(hFile: THandle; const Buffer; nNumberOfBytesToWrite: DWORD;
  var lpNumberOfBytesWritten: DWORD; lpOverlapped: POverlapped): BOOL; stdcall;
begin
  try AddInterceptInfo(1002, hFile, ''); except end;
  Result := TWriteFile(@WriteFileInterceptInfo.HookBuf[0])(hFile, Buffer, nNumberOfBytesToWrite,
                       lpNumberOfBytesWritten, lpOverlapped);
end;

function MyWriteFileEx(hFile: THandle; lpBuffer: Pointer; nNumberOfBytesToWrite: DWORD;
  const lpOverlapped: TOverlapped; lpCompletionRoutine: FARPROC): BOOL; stdcall;
begin
  try AddInterceptInfo(1002, hFile, ''); except end;
  Result := TWriteFileEx(@WriteFileExInterceptInfo.HookBuf[0])(hFile, lpBuffer, nNumberOfBytesToWrite,
                   lpOverlapped, lpCompletionRoutine);
end;

function MyCopyFileExA(lpExistingFileName, lpNewFileName: PAnsiChar;
  lpProgressRoutine: TFNProgressRoutine; lpData: Pointer; pbCancel: PBool;
  dwCopyFlags: DWORD): BOOL; stdcall;
begin
  try AddInterceptInfo(1003, 0, lpExistingFileName, lpNewFileName); except end;
  Result := TCopyFileExA(@CopyFileExAInterceptInfo.HookBuf[0])(lpExistingFileName, lpNewFileName,
                        lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
end;

function MyCopyFileExW(lpExistingFileName, lpNewFileName: PWideChar;
  lpProgressRoutine: TFNProgressRoutine; lpData: Pointer; pbCancel: PBool;
  dwCopyFlags: DWORD): BOOL; stdcall;
begin
  try AddInterceptInfo(1003, 0, lpExistingFileName, lpNewFileName); except end;
  try
   LockLog;
   Result := TCopyFileExW(@CopyFileExWInterceptInfo.HookBuf[0])(lpExistingFileName, lpNewFileName,
                        lpProgressRoutine, lpData, pbCancel, dwCopyFlags);
  finally
   UnLockLog;
  end;
end;

function MyDeleteFileA(lpFileName: PAnsiChar): BOOL; stdcall;
begin
  try AddInterceptInfo(1004, 0, lpFileName); except end;
  CopyToInfectedEx(lpFileName, 'Deleted file');
  Result := TDeleteFileA(@DeleteFileAInterceptInfo.HookBuf[0])(lpFileName);
end;

function MyDeleteFileW(lpFileName: PWideChar): BOOL; stdcall;
begin
  try AddInterceptInfo(1004, 0, lpFileName); except end;
  CopyToInfectedEx(lpFileName, 'Deleted file');
  Result := TDeleteFileW(@DeleteFileWInterceptInfo.HookBuf[0])(lpFileName);
end;

function MyWriteProcessMemory(hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
  nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL; stdcall;
begin
  try AddInterceptInfo(2001, hProcess); except end;
  // Дамп внедряемого кода
  if HooksInjected then
   DumpMem(lpBuffer, dword(lpBaseAddress), nSize);
  Result := TWriteProcessMemory(@WriteProcessMemoryInterceptInfo.HookBuf[0])(hProcess,lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
end;

function MyReadProcessMemory(hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
  nSize: DWORD; var lpNumberOfBytesRead: DWORD): BOOL; stdcall;
begin
  try AddInterceptInfo(2002, hProcess); except end;
  Result := TReadProcessMemory(@ReadProcessMemoryInterceptInfo.HookBuf[0])(hProcess,lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
end;

function MyCreateRemoteThread(hProcess: THandle; lpThreadAttributes: Pointer;
  dwStackSize: DWORD; lpStartAddress: TFNThreadStartRoutine; lpParameter: Pointer;
  dwCreationFlags: DWORD; var lpThreadId: DWORD): THandle; stdcall;
begin
  try AddInterceptInfo(2003, hProcess, Inttostr(integer(lpStartAddress)), GetProcessImageNameByHandle(hProcess)); except end;
  Result := TCreateRemoteThread(@CreateRemoteThreadInterceptInfo.HookBuf[0])(hProcess, lpThreadAttributes,
   dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
end;

function MySetWindowsHookExA(idHook: Integer; lpfn: TFNHookProc; hmod: HINST; dwThreadId: DWORD): HHOOK;
begin
  try AddInterceptInfo(2004, idHook, GetModuleName(hmod)); except end;
  Result := TSetWindowsHookExA(@SetWindowsHookExAInterceptInfo.HookBuf[0])(idHook, lpfn, hmod, dwThreadId);
end;

function MySetWindowsHookExW(idHook: Integer; lpfn: TFNHookProc; hmod: HINST; dwThreadId: DWORD): HHOOK;
begin
  try AddInterceptInfo(2004, idHook, GetModuleName(hmod)); except end;
  Result := TSetWindowsHookExW(@SetWindowsHookExWInterceptInfo.HookBuf[0])(idHook, lpfn, hmod, dwThreadId);
end;

function MyLoadLibraryA(lpLibFileName: PAnsiChar): HMODULE; stdcall;
begin
  try
   LockLog;
   try Result := TLoadLibraryA(@LoadLibraryAInterceptInfo.HookBuf[0])(lpLibFileName); except end;
  finally
   UnLockLog;
  end;
  if lpLibFileName <> nil then
   try AddInterceptInfo(2005, Result, lpLibFileName); except end;
  CopyToInfectedEx(lpLibFileName, '');
end;

function MyLoadLibraryW(lpLibFileName: PWideChar): HMODULE; stdcall;
begin
  try
   LockLog;
   try Result := TLoadLibraryW(@LoadLibraryWInterceptInfo.HookBuf[0])(lpLibFileName); except end;
  finally
   UnLockLog;
  end;
  if lpLibFileName <> nil then
   try AddInterceptInfo(2005, Result, lpLibFileName); except end;
end;

function MyLoadLibraryExA(lpLibFileName: PAnsiChar; hFile: THandle; dwFlags: DWORD): HMODULE; stdcall;
begin
  try
   LockLog;
   Result := TLoadLibraryExA(@LoadLibraryExAInterceptInfo.HookBuf[0])(lpLibFileName, hFile, dwFlags);
  finally
   UnLockLog;
  end;
  try AddInterceptInfo(2006, Result, lpLibFileName); except end;
end;

function MyLoadLibraryExW(lpLibFileName: PWideChar; hFile: THandle; dwFlags: DWORD): HMODULE; stdcall;
begin
  try
   LockLog;
   Result := TLoadLibraryExW(@LoadLibraryExWInterceptInfo.HookBuf[0])(lpLibFileName, hFile, dwFlags);
  finally
   UnLockLog;
  end;
  try AddInterceptInfo(2006, Result, lpLibFileName); except end;
end;

function MyTerminateProcess(hProcess: THandle; uExitCode: UINT): BOOL; stdcall;
begin
  try AddInterceptInfo(2007, hProcess, GetProcessImageNameByHandle(hProcess)); except end;
  try
   LockLog;
   Result := TTerminateProcess(@TerminateProcessInterceptInfo.HookBuf[0])(hProcess, uExitCode);
  finally
   UnLockLog;
  end;
end;

function MyOpenProcess(dwDesiredAccess: DWORD; bInheritHandle: BOOL; dwProcessId: DWORD): THandle; stdcall;
begin
  try AddInterceptInfo(2008, dwProcessId, ''); except end;
  Result := TOpenProcess(@OpenProcessInterceptInfo.HookBuf[0])(dwDesiredAccess, bInheritHandle, dwProcessId);
end;

function MyURLDownloadToFileA(Caller: IUnknown; URL: PAnsiChar; FileName: PAnsiChar; Reserved: DWORD; StatusCB: IBindStatusCallback): HResult; stdcall;
begin
  try AddInterceptInfo(3001, 0, URL, FileName); except end;
  try
   LockLog;
   Result := TURLDownloadToFileA(@URLDownloadToFileAInterceptInfo.HookBuf[0])(Caller, URL, FileName, Reserved, StatusCB);
  finally
   UnLockLog;
  end;
  CopyToInfectedEx(FileName, 'Downloaded file');
end;

function MyURLDownloadToFileW(Caller: IUnknown; URL: PWideChar; FileName: PWideChar; Reserved: DWORD; StatusCB: IBindStatusCallback): HResult; stdcall;
begin
  try AddInterceptInfo(3001, 0, URL, FileName); except end;
  try
   LockLog;
   Result := TURLDownloadToFileW(@URLDownloadToFileWInterceptInfo.HookBuf[0])(Caller, URL, FileName, Reserved, StatusCB);
  finally
   UnLockLog;
  end;
  CopyToInfectedEx(FileName, 'Downloaded file');
end;

function MyInternetOpenUrlA(hInet: HINTERNET; lpszUrl: PAnsiChar;
  lpszHeaders: PAnsiChar; dwHeadersLength: DWORD; dwFlags: DWORD;
  dwContext: DWORD): HINTERNET; stdcall;
begin
  try AddInterceptInfo(4001, 0, lpszUrl, lpszHeaders); except end;
  try
   LockLog;
   Result := TInternetOpenUrlA(@InternetOpenUrlAInterceptInfo.HookBuf[0])(hInet, lpszUrl,
                              lpszHeaders, dwHeadersLength,  dwFlags,  dwContext);
  finally
   UnLockLog;
  end;
end;

function MyInternetOpenUrlW(hInet: HINTERNET; lpszUrl: PWideChar;
  lpszHeaders: PWideChar; dwHeadersLength: DWORD; dwFlags: DWORD;
  dwContext: DWORD): HINTERNET; stdcall;
begin
  try AddInterceptInfo(4001, 0, lpszUrl, lpszHeaders); except end;
  try
   LockLog;
   Result := TInternetOpenUrlW(@InternetOpenUrlWInterceptInfo.HookBuf[0])(hInet, lpszUrl,
                              lpszHeaders, dwHeadersLength,  dwFlags,  dwContext);
  finally
   UnLockLog;
  end;
end;

function MyInternetConnectA(hInet: HINTERNET; lpszServerName: PAnsiChar;
  nServerPort: INTERNET_PORT; lpszUsername: PAnsiChar; lpszPassword: PAnsiChar;
  dwService: DWORD; dwFlags: DWORD; dwContext: DWORD): HINTERNET; stdcall;
begin
  try
   LockLog;
   Result := TInternetConnectA(@InternetConnectAInterceptInfo.HookBuf[0])(hInet, lpszServerName,
              nServerPort, lpszUsername, lpszPassword,
              dwService, dwFlags, dwContext);
  finally
   UnLockLog;
  end;
  try AddInterceptInfo(4002, dword(result), lpszServerName, inttostr(nServerPort)); except end;
end;

function MyInternetConnectW(hInet: HINTERNET; lpszServerName: PWideChar;
  nServerPort: INTERNET_PORT; lpszUsername: PWideChar; lpszPassword: PWideChar;
  dwService: DWORD; dwFlags: DWORD; dwContext: DWORD): HINTERNET; stdcall;
begin
  try
   LockLog;
   Result := TInternetConnectW(@InternetConnectWInterceptInfo.HookBuf[0])(hInet, lpszServerName,
              nServerPort, lpszUsername, lpszPassword,
              dwService, dwFlags, dwContext);
  finally
   UnLockLog;
  end;
  try AddInterceptInfo(4002, dword(result), lpszServerName, inttostr(nServerPort)); except end;
end;

function MyHttpOpenRequestA(hConnect: HINTERNET; lpszVerb: PAnsiChar;
  lpszObjectName: PAnsiChar; lpszVersion: PAnsiChar; lpszReferrer: PAnsiChar;
  lplpszAcceptTypes: PLPSTR; dwFlags: DWORD;
  dwContext: DWORD): HINTERNET; stdcall;
begin
  try AddInterceptInfo(4003, dword(hConnect), lpszVerb, lpszObjectName); except end;
  try
   LockLog;
   Result := THttpOpenRequestA(@HttpOpenRequestAInterceptInfo.HookBuf[0])(hConnect, lpszVerb,
                              lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
  finally
   UnLockLog;
  end;
end;

function MyHttpOpenRequestW(hConnect: HINTERNET; lpszVerb: PWideChar;
  lpszObjectName: PWideChar; lpszVersion: PWideChar; lpszReferrer: PWideChar;
  lplpszAcceptTypes: PLPWSTR; dwFlags: DWORD;
  dwContext: DWORD): HINTERNET; stdcall;
begin
  try AddInterceptInfo(4003, dword(hConnect), lpszVerb, lpszObjectName); except end;
  try
   LockLog;
   Result := THttpOpenRequestW(@HttpOpenRequestWInterceptInfo.HookBuf[0])(hConnect, lpszVerb,
                              lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
  finally
   UnLockLog;
  end;
end;


function Mygethostbyname(name: PChar): PHostEnt; stdcall;
begin
  try AddInterceptInfo(5001, 0, name); except end;
  try
   LockLog;
   Result := Tgethostbyname(@gethostbynameInterceptInfo.HookBuf[0])(name);
  finally
   UnLockLog;
  end;
end;

function Mysend(s: TSocket; var Buf; len, flags: Integer): Integer; stdcall;
var
 ST : string;
begin
  SetLength(ST, len);
  CopyMemory(@ST[1], @Buf, len);
  if (copy(UpperCase(ST), 1, 3) = 'GET') or (copy(UpperCase(ST), 1, 4) = 'POST') then begin
   if pos(#$0D, ST)>0 then
    ST := copy(ST, 1, pos(#$0D, ST)-1);
   try AddInterceptInfo(5002, 0, ST); except end;
  end;
  ST := '';
  try
   LockLog;
   Result := TSend(@SendInterceptInfo.HookBuf[0])(s, Buf, len, flags);
  finally
   UnLockLog;
  end;
end;

function MyCreateProcessA(lpApplicationName: PAnsiChar; lpCommandLine: PAnsiChar;
  lpProcessAttributes, lpThreadAttributes: PSecurityAttributes;
  bInheritHandles: BOOL; dwCreationFlags: DWORD; lpEnvironment: Pointer;
  lpCurrentDirectory: PAnsiChar; const lpStartupInfo: TStartupInfo;
  var lpProcessInformation: TProcessInformation): BOOL; stdcall;
var
 SuspendRun : boolean;
begin
  CopyToInfectedEx(lpApplicationName, '');
  CopyToInfectedEx(lpCommandLine,'');
  SuspendRun := (dwCreationFlags and CREATE_SUSPENDED) > 0;
  dwCreationFlags := dwCreationFlags or CREATE_SUSPENDED;
  Result := TCreateProcessA(@CreateProcessAInterceptInfo.HookBuf[0])(lpApplicationName, lpCommandLine,
      lpProcessAttributes, lpThreadAttributes,
      bInheritHandles, dwCreationFlags, lpEnvironment,
      lpCurrentDirectory, lpStartupInfo,
      lpProcessInformation);
  try AddInterceptInfo(6001, lpProcessInformation.dwProcessId, lpApplicationName, lpCommandLine); except end;
  if Result then begin
   try
    LockLog;
    InjectDllAlt(lpProcessInformation.hProcess, lpProcessInformation.hThread, PChar(FullModuleName));
   finally
    UnLockLog;
   end;
   // Запуск главного потока процесса
   if not(SuspendRun) then
    ResumeThread(lpProcessInformation.hThread);
  end;
end;

function MyCreateProcessW(lpApplicationName: PWideChar; lpCommandLine: PWideChar;
  lpProcessAttributes, lpThreadAttributes: PSecurityAttributes;
  bInheritHandles: BOOL; dwCreationFlags: DWORD; lpEnvironment: Pointer;
  lpCurrentDirectory: PWideChar; const lpStartupInfo: TStartupInfo;
  var lpProcessInformation: TProcessInformation): BOOL; stdcall;
var
 SuspendRun : boolean;
begin
  CopyToInfectedEx(lpApplicationName,'');
  CopyToInfectedEx(lpCommandLine,'');
  SuspendRun := (dwCreationFlags and CREATE_SUSPENDED) > 0;
  dwCreationFlags := dwCreationFlags or CREATE_SUSPENDED;
  Result := TCreateProcessW(@CreateProcessWInterceptInfo.HookBuf[0])(lpApplicationName, lpCommandLine,
      lpProcessAttributes, lpThreadAttributes,
      bInheritHandles, dwCreationFlags, lpEnvironment,
      lpCurrentDirectory, lpStartupInfo,
      lpProcessInformation);
  try AddInterceptInfo(6001, lpProcessInformation.dwProcessId, lpApplicationName, lpCommandLine); except end;
  if Result then begin
   try
    LockLog;
    InjectDllAlt(lpProcessInformation.hProcess, lpProcessInformation.hThread, PChar(FullModuleName));
   finally
    UnLockLog;
   end;
   // Запуск главного потока процесса
   if not(SuspendRun) then
    ResumeThread(lpProcessInformation.hThread);
  end;
end;

function MyWinExec(lpCmdLine: LPCSTR; uCmdShow: UINT): UINT; stdcall;
begin
  try
   AddInterceptInfo(6002, uCmdShow, lpCmdLine);
   CopyToInfectedEx(lpCmdLine,'');
  except end;
  Result := TWinExec(@WinExecInterceptInfo.HookBuf[0])(lpCmdLine, uCmdShow);
end;

function MyShellExecuteExA(lpExecInfo: PShellExecuteInfoA):BOOL; stdcall;
begin
  try AddInterceptInfo(6003, 0, lpExecInfo.lpFile); except end;
  CopyToInfectedEx(lpExecInfo.lpFile, '');
  Result := TShellExecuteExA(@ShellExecuteExAInterceptInfo.HookBuf[0])(lpExecInfo);
end;

function MyShellExecuteExW(lpExecInfo: PShellExecuteInfoW):BOOL; stdcall;
begin
  try AddInterceptInfo(6003, 0, lpExecInfo.lpFile); except end;
  CopyToInfectedEx(lpExecInfo.lpFile, '');
  Result := TShellExecuteExW(@ShellExecuteExWInterceptInfo.HookBuf[0])(lpExecInfo);
end;

procedure MyExitProcess(uExitCode: UINT); stdcall;
begin
  try AddInterceptInfo(-2, uExitCode, ParamStr(0)); except end;
  TExitProcess(@ExitProcessInterceptInfo.HookBuf[0])(uExitCode);
end;

function MyRegCreateKeyExA(hKey: HKEY; lpSubKey: PAnsiChar;
  Reserved: DWORD; lpClass: PAnsiChar; dwOptions: DWORD; samDesired: REGSAM;
  lpSecurityAttributes: PSecurityAttributes; var phkResult: HKEY;
  lpdwDisposition: PDWORD): Longint; stdcall;
begin
  Result := TRegCreateKeyExA(@RegCreateKeyExAInterceptInfo.HookBuf[0])(hKey, lpSubKey,
                             Reserved, lpClass, dwOptions, samDesired,
                             lpSecurityAttributes, phkResult, lpdwDisposition);
  try AddInterceptInfo(7001, hKey, lpSubKey, GetRegKeyNameByHandle(phkResult)); except end;
end;

function MyRegCreateKeyExW(hKey: HKEY; lpSubKey: PWideChar;
  Reserved: DWORD; lpClass: PWideChar; dwOptions: DWORD; samDesired: REGSAM;
  lpSecurityAttributes: PSecurityAttributes; var phkResult: HKEY;
  lpdwDisposition: PDWORD): Longint; stdcall;
begin
  Result := TRegCreateKeyExW(@RegCreateKeyExWInterceptInfo.HookBuf[0])(hKey, lpSubKey,
                             Reserved, lpClass, dwOptions, samDesired,
                             lpSecurityAttributes, phkResult, lpdwDisposition);
  try AddInterceptInfo(7001, hKey, lpSubKey, GetRegKeyNameByHandle(phkResult)); except end;
end;

function MyRegSetValueExA(hKey: HKEY; lpValueName: PAnsiChar;
  Reserved: DWORD; dwType: DWORD; lpData: Pointer; cbData: DWORD): Longint; stdcall;
var
 S : string;
begin
  try
   LockLog;
   Result := TRegSetValueExA(@RegSetValueExAInterceptInfo.HookBuf[0])(hKey, lpValueName,
                            Reserved, dwType, lpData, cbData);
  finally
   UnLockLog;
  end;
  S := '?';
  case dwType of
   REG_DWORD : S := IntToHex(dword(lpData^), 8);
   REG_EXPAND_SZ, REG_SZ, REG_MULTI_SZ : S := Pchar(lpData);
  end;
 try AddInterceptInfo(7002, hKey, GetRegKeyNameByHandle(hKey)+'\'+lpValueName, S); except end;
end;

function MyRegSetValueExW(hKey: HKEY; lpValueName: PWideChar;
  Reserved: DWORD; dwType: DWORD; lpData: Pointer; cbData: DWORD): Longint; stdcall;
var
 S : string;
begin
  try
   LockLog;
   Result := TRegSetValueExW(@RegSetValueExWInterceptInfo.HookBuf[0])(hKey, lpValueName,
                            Reserved, dwType, lpData, cbData);
  finally
   UnLockLog;
  end;
  S := '?';
  case dwType of
   REG_DWORD : S := IntToHex(dword(lpData^), 8);
   REG_EXPAND_SZ, REG_SZ, REG_MULTI_SZ : S := PWideChar(lpData);
  end;
 try AddInterceptInfo(7002, hKey, GetRegKeyNameByHandle(hKey)+'\'+lpValueName, S); except end;
end;

function MyRegDeleteKeyA(hKey: HKEY; lpSubKey: PAnsiChar): Longint; stdcall;
begin
 Result := TRegDeleteKeyA(@RegDeleteKeyAInterceptInfo.HookBuf[0])(hKey, lpSubKey);
 try AddInterceptInfo(7003, hKey, lpSubKey, GetRegKeyNameByHandle(hKey)); except end;
end;

function MyRegDeleteKeyW(hKey: HKEY; lpSubKey: PWideChar): Longint; stdcall;
begin
 Result := TRegDeleteKeyW(@RegDeleteKeyWInterceptInfo.HookBuf[0])(hKey, lpSubKey);
 try AddInterceptInfo(7003, hKey, lpSubKey, GetRegKeyNameByHandle(hKey)); except end;
end;

function MyRegDeleteValueA(hKey: HKEY; lpValueName: PAnsiChar): Longint; stdcall;
begin
 try AddInterceptInfo(7004, hKey, lpValueName, GetRegKeyNameByHandle(hKey)); except end;
 Result := TRegDeleteValueA(@RegDeleteValueAInterceptInfo.HookBuf[0])(hKey, lpValueName);
end;

function MyRegDeleteValueW(hKey: HKEY; lpValueName: PWideChar): Longint; stdcall;
begin
 try AddInterceptInfo(7004, hKey, lpValueName, GetRegKeyNameByHandle(hKey)); except end;
 Result := TRegDeleteValueW(@RegDeleteValueWInterceptInfo.HookBuf[0])(hKey, lpValueName);
end;

function MyCreateServiceA(hSCManager: SC_HANDLE; lpServiceName, lpDisplayName: PAnsiChar;
  dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl: DWORD;
  lpBinaryPathName, lpLoadOrderGroup: PAnsiChar; lpdwTagId: LPDWORD; lpDependencies,
  lpServiceStartName, lpPassword: PAnsiChar): SC_HANDLE; stdcall;
begin
 CopyToInfectedEx(lpBinaryPathName,'');
 try AddInterceptInfo(8001, hSCManager, lpServiceName, lpBinaryPathName); except end;
 Result := TCreateServiceA(@CreateServiceAInterceptInfo.HookBuf[0])(hSCManager, lpServiceName, lpDisplayName, dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl, lpBinaryPathName, lpLoadOrderGroup, lpdwTagId, lpDependencies, lpServiceStartName, lpPassword);
end;

function MyCreateServiceW(hSCManager: SC_HANDLE; lpServiceName, lpDisplayName: PWideChar;
  dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl: DWORD;
  lpBinaryPathName, lpLoadOrderGroup: PWideChar; lpdwTagId: LPDWORD; lpDependencies,
  lpServiceStartName, lpPassword: PWideChar): SC_HANDLE; stdcall;
begin
 CopyToInfectedEx(lpBinaryPathName,'');
 try AddInterceptInfo(8001, hSCManager, lpServiceName, lpBinaryPathName); except end;
 Result := TCreateServiceW(@CreateServiceWInterceptInfo.HookBuf[0])(hSCManager, lpServiceName, lpDisplayName, dwDesiredAccess, dwServiceType, dwStartType, dwErrorControl, lpBinaryPathName, lpLoadOrderGroup, lpdwTagId, lpDependencies, lpServiceStartName, lpPassword);
end;

function MyDeleteService(hService: SC_HANDLE): BOOL; stdcall;
begin
 try AddInterceptInfo(8002, hService); except end;
 Result := TDeleteService(@DeleteServiceInterceptInfo.HookBuf[0])(hService);
end;

function MyOpenServiceA(hSCManager: SC_HANDLE; lpServiceName: PAnsiChar;
  dwDesiredAccess: DWORD): SC_HANDLE; stdcall;
begin
 Result := TOpenServiceA(@OpenServiceAInterceptInfo.HookBuf[0])(hSCManager, lpServiceName, dwDesiredAccess);
 try AddInterceptInfo(8003, Result, lpServiceName); except end;
end;

function MyOpenServiceW(hSCManager: SC_HANDLE; lpServiceName: PWideChar;
  dwDesiredAccess: DWORD): SC_HANDLE; stdcall;
begin
 Result := TOpenServiceW(@OpenServiceWInterceptInfo.HookBuf[0])(hSCManager, lpServiceName, dwDesiredAccess);
 try AddInterceptInfo(8003, Result, lpServiceName); except end;
end;

function MyEnumServicesStatusA(hSCManager: SC_HANDLE; dwServiceType,
  dwServiceState: DWORD; var lpServices: TEnumServiceStatusA; cbBufSize: DWORD;
  var pcbBytesNeeded, lpServicesReturned, lpResumeHandle: DWORD): BOOL; stdcall;
begin
 try AddInterceptInfo(8004, hSCManager); except end;
 Result := TEnumServicesStatusA(@EnumServicesStatusAInterceptInfo.HookBuf[0])(hSCManager, dwServiceType,
  dwServiceState,lpServices, cbBufSize, pcbBytesNeeded, lpServicesReturned, lpResumeHandle);
end;

function MyEnumServicesStatusW(hSCManager: SC_HANDLE; dwServiceType,
  dwServiceState: DWORD; var lpServices: TEnumServiceStatusW; cbBufSize: DWORD;
  var pcbBytesNeeded, lpServicesReturned, lpResumeHandle: DWORD): BOOL; stdcall;
begin
 try AddInterceptInfo(8004, hSCManager); except end;
 Result := TEnumServicesStatusW(@EnumServicesStatusWInterceptInfo.HookBuf[0])(hSCManager, dwServiceType,
  dwServiceState,lpServices, cbBufSize, pcbBytesNeeded, lpServicesReturned, lpResumeHandle);
end;

function MyZwLoadDriver(DriverServiceName : PUNICODE_STRING) : NTSTATUS; stdcall;
begin
 try AddInterceptInfo(8005, 0, DriverServiceName^.Buffer); except end;
 Result :=  TZwLoadDriver(@ZwLoadDriverInterceptInfo.HookBuf[0])(DriverServiceName);
end;

function MyZwUnloadDriver(DriverServiceName : PUNICODE_STRING) : NTSTATUS; stdcall;
begin
 try AddInterceptInfo(8006, 0, DriverServiceName^.Buffer); except end;
 Result :=  TZwUnLoadDriver(@ZwUnLoadDriverInterceptInfo.HookBuf[0])(DriverServiceName);
end;

function MySetThreadContext(hThread: THandle; const lpContext: TContext): BOOL; stdcall;
begin
 try AddInterceptInfo(2009, hThread); except end;
 Result := TSetThreadContext(@SetThreadContextInterceptInfo.HookBuf[0])(hThread, lpContext);
end;


function SetHooks : boolean;
begin
  InterceptedFunctions := nil;
  // ************ Установка перехватов *************
  // **** kernel32.dll ****
  InterceptFunctionEx('kernel32.dll','CreateFileA',
                     CreateFileAInterceptInfo, @MyCreateFileA);
  InterceptFunctionEx('kernel32.dll','CreateFileW',
                     CreateFileWInterceptInfo, @MyCreateFileW);
{  InterceptFunctionEx('kernel32.dll','WriteFile',
                     WriteFileInterceptInfo, @MyWriteFile);
  InterceptFunctionEx('kernel32.dll','WriteFileEx',
                     WriteFileExInterceptInfo, @MyWriteFileEx);
  InterceptFunctionEx('kernel32.dll','ReadProcessMemory',
                     ReadProcessMemoryInterceptInfo, @MyReadProcessMemory);}
  InterceptFunctionEx('kernel32.dll','WriteProcessMemory',
                     WriteProcessMemoryInterceptInfo, @MyWriteProcessMemory);
  InterceptFunctionEx('kernel32.dll','CreateProcessA',
                     CreateProcessAInterceptInfo, @MyCreateProcessA);
  InterceptFunctionEx('kernel32.dll','CreateProcessW',
                     CreateProcessWInterceptInfo, @MyCreateProcessW);
  InterceptFunctionEx('kernel32.dll','CreateRemoteThread',
                     CreateRemoteThreadInterceptInfo, @MyCreateRemoteThread);
  InterceptFunctionEx('kernel32.dll','CopyFileExA',
                     CopyFileExAInterceptInfo, @MyCopyFileExA);
  InterceptFunctionEx('kernel32.dll','CopyFileExW',
                     CopyFileExWInterceptInfo, @MyCopyFileExW);
  InterceptFunctionEx('kernel32.dll','DeleteFileA',
                     DeleteFileAInterceptInfo, @MyDeleteFileA);
  InterceptFunctionEx('kernel32.dll','DeleteFileW',
                     DeleteFileWInterceptInfo, @MyDeleteFileW);
  InterceptFunctionEx('kernel32.dll','WinExec',
                     WinExecInterceptInfo, @MyWinExec);
  InterceptFunctionEx('kernel32.dll','ExitProcess',
                    ExitProcessInterceptInfo, @MyExitProcess);
  InterceptFunctionEx('kernel32.dll','TerminateProcess',
                    TerminateProcessInterceptInfo, @MyTerminateProcess);
  InterceptFunctionEx('kernel32.dll','OpenProcess',
                    OpenProcessInterceptInfo, @MyOpenProcess);
  InterceptFunctionEx('kernel32.dll','SetThreadContext',
                    SetThreadContextInterceptInfo, @MySetThreadContext);
  // **** ADVAPI32.DLL ****
  InterceptFunctionEx('advapi32.dll','RegCreateKeyExA',
                     RegCreateKeyExAInterceptInfo, @MyRegCreateKeyExA);
  InterceptFunctionEx('advapi32.dll','RegCreateKeyExW',
                     RegCreateKeyExWInterceptInfo, @MyRegCreateKeyExW);
  InterceptFunctionEx('advapi32.dll','RegSetValueExA',
                     RegSetValueExAInterceptInfo, @MyRegSetValueExA);
  InterceptFunctionEx('advapi32.dll','RegSetValueExW',
                     RegSetValueExWInterceptInfo, @MyRegSetValueExW);
  InterceptFunctionEx('advapi32.dll','RegDeleteKeyA',
                     RegDeleteKeyAInterceptInfo, @MyRegDeleteKeyA);
  InterceptFunctionEx('advapi32.dll','RegDeleteKeyW',
                     RegDeleteKeyWInterceptInfo, @MyRegDeleteKeyW);
  InterceptFunctionEx('advapi32.dll','RegDeleteValueA',
                     RegDeleteValueAInterceptInfo, @MyRegDeleteValueA);
  InterceptFunctionEx('advapi32.dll','RegDeleteValueW',
                     RegDeleteValueWInterceptInfo, @MyRegDeleteValueW);
  InterceptFunctionEx('advapi32.dll','CreateServiceA',
                     CreateServiceAInterceptInfo, @MyCreateServiceA);
  InterceptFunctionEx('advapi32.dll','CreateServiceW',
                     CreateServiceWInterceptInfo, @MyCreateServiceW);
  InterceptFunctionEx('advapi32.dll','DeleteService',
                     DeleteServiceInterceptInfo, @MyDeleteService);
  InterceptFunctionEx('advapi32.dll','OpenServiceA',
                     OpenServiceAInterceptInfo, @MyOpenServiceA);
  InterceptFunctionEx('advapi32.dll','OpenServiceW',
                     OpenServiceWInterceptInfo, @MyOpenServiceW);
  InterceptFunctionEx('advapi32.dll','EnumServicesStatusA',
                     EnumServicesStatusAInterceptInfo, @MyEnumServicesStatusA);
  InterceptFunctionEx('advapi32.dll','EnumServicesStatusW',
                     EnumServicesStatusWInterceptInfo, @MyEnumServicesStatusW);
  // **** URLMON.DLL ****
  InterceptFunctionEx('URLMON.DLL','URLDownloadToFileA',
                     URLDownloadToFileAInterceptInfo, @MyURLDownloadToFileA);
  InterceptFunctionEx('URLMON.DLL','URLDownloadToFileW',
                     URLDownloadToFileWInterceptInfo, @MyURLDownloadToFileW);
  // **** wininet.dll ****
  InterceptFunctionEx('wininet.dll','InternetOpenUrlA',
                     InternetOpenUrlAInterceptInfo, @MyInternetOpenUrlA);
  InterceptFunctionEx('wininet.dll','InternetOpenUrlW',
                     InternetOpenUrlWInterceptInfo, @MyInternetOpenUrlW);
  InterceptFunctionEx('wininet.dll','HttpOpenRequestA',
                     HttpOpenRequestAInterceptInfo, @MyHttpOpenRequestA);
  InterceptFunctionEx('wininet.dll','HttpOpenRequestW',
                     HttpOpenRequestWInterceptInfo, @MyHttpOpenRequestW);
  InterceptFunctionEx('wininet.dll','InternetConnectA',
                     InternetConnectAInterceptInfo, @MyInternetConnectA);
  InterceptFunctionEx('wininet.dll','InternetConnectW',
                     InternetConnectWInterceptInfo, @MyInternetConnectW);
  // **** ws2_32.dll ****
  InterceptFunctionEx('ws2_32.dll','gethostbyname',
                     gethostbynameInterceptInfo, @Mygethostbyname);
  InterceptFunctionEx('ws2_32.dll','send',
                     sendInterceptInfo, @MySend);
  // **** shell32.dll ****
  InterceptFunctionEx('shell32.dll','ShellExecuteExA',
                     ShellExecuteExAInterceptInfo, @MyShellExecuteExA);
  InterceptFunctionEx('shell32.dll','ShellExecuteExW',
                     ShellExecuteExWInterceptInfo, @MyShellExecuteExW);

  // ******** kernel32 ******
  InterceptFunctionEx('kernel32.dll','LoadLibraryA',
                    LoadLibraryAInterceptInfo, @MyLoadLibraryA);
  InterceptFunctionEx('kernel32.dll','LoadLibraryW',
                    LoadLibraryWInterceptInfo, @MyLoadLibraryW);
  InterceptFunctionEx('kernel32.dll','LoadLibraryExA',
                    LoadLibraryExAInterceptInfo, @MyLoadLibraryExA);
  InterceptFunctionEx('kernel32.dll','LoadLibraryExW',
                    LoadLibraryExWInterceptInfo, @MyLoadLibraryExW);

  // **** ntdll.dll ****
  InterceptFunctionEx('ntdll.dll','ZwLoadDriver',
                    ZwLoadDriverInterceptInfo, @MyZwLoadDriver);
  InterceptFunctionEx('ntdll.dll','ZwUnLoadDriver',
                    ZwUnLoadDriverInterceptInfo, @MyZwUnLoadDriver);
  // **** user32.dll ****
{  InterceptFunctionEx('user32.dll','SetWindowsHookExA',
                     SetWindowsHookExAInterceptInfo, @MySetWindowsHookExA);
  InterceptFunctionEx('user32.dll','SetWindowsHookExW',
                     SetWindowsHookExWInterceptInfo, @MySetWindowsHookExW);}
  HooksInjected := true;
  LockLogCnt    := 0;
end;

function RemoveHooks : boolean;
var
 i : integer;
begin
  for i := 0 to Length(InterceptedFunctions) - 1 do
   RestoreFunctionEx(InterceptedFunctions[i]);
  InterceptedFunctions := nil;
  HooksInjected := false;
  LockLogCnt    := 0;
end;

procedure DLLEntryProc(EntryCode: integer);
begin
  case EntryCode of
   DLL_PROCESS_DETACH :  begin
             try
              LockLogCnt := maxint;
              RemoveHooks;
              OracleCS.Free;
              QuarantineCS.Free;
              LogOracleQuery.Free;
              OracleSession.Free;
             except end;
    end;
   DLL_PROCESS_ATTACH :  begin
                         end;
   DLL_THREAD_ATTACH  :  begin
                          LockLogCnt := 0;
                          ThreadInternalID := GetThreadInternalID;
                         end;
   DLL_THREAD_DETACH  :  begin
                          LockLogCnt := MaxInt;
                         end;
  end;
end;

begin
 LockLogCnt := 1;
 ThreadInternalID := 0;
 HeapAllocFlags := GMEM_FIXED;
 IsMultiThread := true;
 // Создание критических секций
 OracleCS              := TCriticalSection.Create;
 QuarantineCS          := TCriticalSection.Create;
 GlobalThreadCounterCS := TCriticalSection.Create;
 // Импорт функций ntdll.dll
 LoadNTDllAPI;
 try
  OracleSession := TOracleSession.Create(nil);
  OracleSession.LogonUsername := 'avz';
  OracleSession.LogonPassword := 'aes';
  OracleSession.LogonPassword := OracleSession.LogonPassword + '254';
  OracleSession.LogonDatabase := 'virlab';
  OracleSession.ThreadSafe := true;
  OracleSession.Connected := true;
  LogOracleQuery := TOracleQuery.Create(nil);
  LogOracleQuery.Session := OracleSession;
  LogOracleQuery.SQL.Text := 'INSERT INTO TEST (PID, OP_CODE, DW_PARAM1, ST_PARAM1, ST_PARAM2, OP_TID) VALUES (:PID, :OP_CODE, :DW_PARAM1, :ST_PARAM1, :ST_PARAM2, :OP_TID)';
  LogOracleQuery.DeclareVariable('PID',     otInteger);
  LogOracleQuery.DeclareVariable('OP_CODE', otInteger);
  LogOracleQuery.DeclareVariable('DW_PARAM1', otString);
  LogOracleQuery.DeclareVariable('ST_PARAM1', otString);
  LogOracleQuery.DeclareVariable('ST_PARAM2', otString);
  LogOracleQuery.DeclareVariable('OP_TID', otInteger);
  FullModuleName := GetModuleName(hInstance);
  LockLogCnt := 0;
  FileMD5 := MD5DigestToStr(MD5File(ParamStr(0)));
  SetHooks;
  AddInterceptInfo(-1, GetCurrentProcessId, ParamStr(0), FileMD5);
  DLLProc := @DLLEntryProc;
 except
 end;
end.

