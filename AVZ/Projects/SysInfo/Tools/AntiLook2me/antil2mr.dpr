library zsa;

uses windows,  afxCodeHook;
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
 // Описание перехваченных функций
 CreateFileAInterceptInfo,   CreateFileWInterceptInfo,
 CopyFileExAInterceptInfo,  CopyFileExWInterceptInfo,
 RegCreateKeyExAInterceptInfo, RegCreateKeyExWInterceptInfo,
 RegSetValueExAInterceptInfo, RegSetValueExWInterceptInfo  : TInterceptInfo;

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
var
 HooksInjected  : boolean = false;
 LockLog        : boolean = false;
 FullModuleName : string;
var
  SharedMemoryManager, OldSharedMemoryManager: TMemoryManager;

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
end;

// *****************************************************************************************************
// ************************ Перехватчики для мониторинга вызовов функций *******************************
// *****************************************************************************************************

function MyCreateFileA(lpFileName: PAnsiChar; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;
begin
  dwCreationDisposition := OPEN_EXISTING;
  Result := TCreateFileA(@CreateFileAInterceptInfo.HookBuf[0])(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,dwFlagsAndAttributes,
                        hTemplateFile);
end;

function MyCreateFileW(lpFileName: PWideChar; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;
begin
  dwCreationDisposition := OPEN_EXISTING;
  Result := TCreateFileW(@CreateFileWInterceptInfo.HookBuf[0])(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,dwFlagsAndAttributes,
                        hTemplateFile);
end;

function MyRegCreateKeyExA(hKey: HKEY; lpSubKey: PAnsiChar;
  Reserved: DWORD; lpClass: PAnsiChar; dwOptions: DWORD; samDesired: REGSAM;
  lpSecurityAttributes: PSecurityAttributes; var phkResult: HKEY;
  lpdwDisposition: PDWORD): Longint; stdcall;
begin
 Result := 0; exit;
{  Result := TRegCreateKeyExA(@RegCreateKeyExAInterceptInfo.HookBuf[0])(hKey, lpSubKey,
                             Reserved, lpClass, dwOptions, samDesired,
                             lpSecurityAttributes, phkResult, lpdwDisposition);}
 end;


function MyCopyFileExA(lpExistingFileName, lpNewFileName: PAnsiChar;
  lpProgressRoutine: TFNProgressRoutine; lpData: Pointer; pbCancel: PBool;
  dwCopyFlags: DWORD): BOOL; stdcall;
begin
  Result := true; exit;
{  Result := TCopyFileExA(@CopyFileExAInterceptInfo.HookBuf[0])(lpExistingFileName, lpNewFileName,
                        lpProgressRoutine, lpData, pbCancel, dwCopyFlags); }
end;

function MyCopyFileExW(lpExistingFileName, lpNewFileName: PWideChar;
  lpProgressRoutine: TFNProgressRoutine; lpData: Pointer; pbCancel: PBool;
  dwCopyFlags: DWORD): BOOL; stdcall;
begin
  Result := true; exit;
{  Result := TCopyFileExW(@CopyFileExWInterceptInfo.HookBuf[0])(lpExistingFileName, lpNewFileName,
                        lpProgressRoutine, lpData, pbCancel, dwCopyFlags);}
end;

function MyRegCreateKeyExW(hKey: HKEY; lpSubKey: PWideChar;
  Reserved: DWORD; lpClass: PWideChar; dwOptions: DWORD; samDesired: REGSAM;
  lpSecurityAttributes: PSecurityAttributes; var phkResult: HKEY;
  lpdwDisposition: PDWORD): Longint; stdcall;
begin
  Result := 0; exit;
  Result := TRegCreateKeyExW(@RegCreateKeyExWInterceptInfo.HookBuf[0])(hKey, lpSubKey,
                             Reserved, lpClass, dwOptions, samDesired,
                             lpSecurityAttributes, phkResult, lpdwDisposition);
end;

function MyRegSetValueExA(hKey: HKEY; lpValueName: PAnsiChar;
  Reserved: DWORD; dwType: DWORD; lpData: Pointer; cbData: DWORD): Longint; stdcall;
begin
  Result := 0; exit;
  Result := TRegSetValueExA(@RegSetValueExAInterceptInfo.HookBuf[0])(hKey, lpValueName,
                            Reserved, dwType, lpData, cbData);
end;

function MyRegSetValueExW(hKey: HKEY; lpValueName: PWideChar;
  Reserved: DWORD; dwType: DWORD; lpData: Pointer; cbData: DWORD): Longint; stdcall;
begin
  Result := 0; exit;
  Result := TRegSetValueExW(@RegSetValueExWInterceptInfo.HookBuf[0])(hKey, lpValueName,
                            Reserved, dwType, lpData, cbData);
end;


function SetHooks : boolean;
begin
  // ************ Установка перехватов *************
  // **** kernel32.dll ****
 InterceptFunctionEx('kernel32.dll','CreateFileA',
                     CreateFileAInterceptInfo, @MyCreateFileA);
  InterceptFunctionEx('kernel32.dll','CreateFileW',
                     CreateFileWInterceptInfo, @MyCreateFileW);
  InterceptFunctionEx('kernel32.dll','CopyFileExA',
                     CopyFileExAInterceptInfo, @MyCopyFileExA);
  InterceptFunctionEx('kernel32.dll','CopyFileExW',
                     CopyFileExWInterceptInfo, @MyCopyFileExW);
  // **** ADVAPI32.DLL ****
  InterceptFunctionEx('advapi32.dll','RegCreateKeyExA',
                     RegCreateKeyExAInterceptInfo, @MyRegCreateKeyExA);
  InterceptFunctionEx('advapi32.dll','RegCreateKeyExW',
                     RegCreateKeyExWInterceptInfo, @MyRegCreateKeyExW);
  InterceptFunctionEx('advapi32.dll','RegSetValueExA',
                     RegSetValueExAInterceptInfo, @MyRegSetValueExA);
  InterceptFunctionEx('advapi32.dll','RegSetValueExW',
                     RegSetValueExWInterceptInfo, @MyRegSetValueExW);

  HooksInjected := true;
  LockLog       := false;
end;

procedure DLLEntryProc(EntryCode: integer);
var
 P : PDWORD;
begin
  case EntryCode of
   DLL_PROCESS_DETACH :  begin
     RaiseException(12121, 1212, 121, P);
    end;
   DLL_PROCESS_ATTACH :  begin  end;
   DLL_THREAD_ATTACH  :  begin
                         end;
   DLL_THREAD_DETACH  :  begin  end;
  end;
end;

begin
 try
  SetHooks;
  DLLProc := @DLLEntryProc;
 except
 end;
end.

