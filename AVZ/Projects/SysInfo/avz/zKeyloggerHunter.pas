unit zKeyloggerHunter;
interface
uses messages, windows, forms, SystemMonitor, Classes, SysUtils, Graphics, Controls, ExtCtrls, orautil,
 LDasm,
 zSharedFunctions,
 zTranslate;
type
 TTraceDopInfo = record
  dwData      : DWord;     // Данные формата Dword
  stData      : String;    // Данные формата String
 end;
 TTraceInfo = record
  CallPrt     : DWORD;    // Адрес, с которого прошел вызов
  Mode        : integer;  // Код режима
  Breackpoint : integer;  // Код "брекпоинта" - кодирует функцию
  RptCount    : integer;  // Количество повторных вызовов точки
  DopInfo     : array of TTraceDopInfo;
 end;
var
 KeyloggerTraceInfo    : array of TTraceInfo    = nil;
 LockInterceptFunctions : boolean = false;
 KeyloggerHookMask : string = '111111111111111111111111111111111111111111111';
// Выполнение "охоты" - вся процедура, включая перехват функций, "дразнение" и снятие перехвата
function KeyloggerHunter : boolean;
// Получение текстового результата анализа
function GetModuleTraceResult(AModuleBase, AModuleSize : DWORD) : string;

implementation
const
 ModeKeyboard = 1;
 ModeMouse    = 2;
 ModeCBT      = 3;
 ModeAllMsg   = 4;
 ModeNames : array[1..4] of string = ('$AVZ0334', '$AVZ0446', '$AVZ0567', '$AVZ0156');
var
 TekMode : integer;
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
// Описание перехваченных функций
var
 CallNextHookExInterceptInfo,
 GetKeyboardStateInterceptInfo,
 GetForegroundWindowInterceptInfo,
 ToAsciiInterceptInfo,
 ToAsciiExInterceptInfo,
 GetKeyStateInterceptInfo,
 GetClassNameAInterceptInfo,
 GetClassNameWInterceptInfo,
 GetFocusInterceptInfo,
 GetKeyNameTextAInterceptInfo,
 GetKeyNameTextWInterceptInfo,
 SendMessageAInterceptInfo,  SendMessageWInterceptInfo,
 PostMessageAInterceptInfo,  PostMessageWInterceptInfo,
 CreateFileAInterceptInfo,   CreateFileWInterceptInfo,
 MapVirtualKeyAInterceptInfo, MapVirtualKeyWInterceptInfo,
 MapVirtualKeyExAInterceptInfo, MapVirtualKeyExWInterceptInfo,
 GetKeyboardLayoutInterceptInfo,
 GetActiveWindowInterceptInfo,
 WriteFileInterceptInfo,
 WriteFileExInterceptInfo,
 GetModuleFileNameAInterceptInfo, GetModuleFileNameWInterceptInfo,
 GetCurrentProcessIdInterceptInfo,
 WriteProcessMemoryInterceptInfo, ReadProcessMemoryInterceptInfo            : TInterceptInfo;

type
 TCallNextHookEx      = function (hhk: HHOOK; nCode: Integer; wParam: WPARAM; lParam: LPARAM): LRESULT; stdcall;
 TGetKeyboardState    = function (var KeyState: TKeyboardState): BOOL; stdcall;
 TGetForegroundWindow = function : HWND; stdcall;
 TToAscii             = function (uVirtKey, uScanCode: UINT; const KeyState: TKeyboardState;
                                  lpChar: PChar; uFlags: UINT): Integer; stdcall;
 TToAsciiEx           = function (uVirtKey: UINT; uScanCode: UINT; const KeyState: TKeyboardState;
                                  lpChar: PChar; uFlags: UINT; dwhkl: HKL): Integer; stdcall;
 TGetFocus            = function : HWND; stdcall;
 TSendMessageA        = function (hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): LRESULT; stdcall;
 TSendMessageW        = function (hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): LRESULT; stdcall;
 TPostMessageA        = function (hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): BOOL; stdcall;
 TPostMessageW        = function (hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): BOOL; stdcall;
 TGetActiveWindow     = function : HWND; stdcall;
 TGetKeyState         = function (nVirtKey: Integer): SHORT; stdcall;
 TGetKeyboardLayout   = function (dwLayout: DWORD): HKL; stdcall;
 TGetKeyNameTextA     = function (lParam: Longint; lpString: PAnsiChar; nSize: Integer): Integer; stdcall;
 TGetKeyNameTextW     = function (lParam: Longint; lpString: PWideChar; nSize: Integer): Integer; stdcall;
 TCreateFileA         = function (lpFileName: PAnsiChar; dwDesiredAccess, dwShareMode: DWORD;
                                 lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
                                 hTemplateFile: THandle): THandle; stdcall;
 TCreateFileW         = function (lpFileName: PWideChar; dwDesiredAccess, dwShareMode: DWORD;
                                 lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
                                 hTemplateFile: THandle): THandle; stdcall;
 TGetClassNameA       = function (hWnd: HWND; lpClassName: PAnsiChar; nMaxCount: Integer): Integer; stdcall;
 TGetClassNameW       = function (hWnd: HWND; lpClassName: PWideChar; nMaxCount: Integer): Integer; stdcall;
 TMapVirtualKeyA      = function (uCode, uMapType: UINT): UINT; stdcall;
 TMapVirtualKeyW      = function (uCode, uMapType: UINT): UINT; stdcall;

 TWriteFile           = function (hFile: THandle; const Buffer; nNumberOfBytesToWrite: DWORD;
                                  var lpNumberOfBytesWritten: DWORD; lpOverlapped: POverlapped): BOOL; stdcall;
 TWriteFileEx         = function (hFile: THandle; lpBuffer: Pointer; nNumberOfBytesToWrite: DWORD;
                                  const lpOverlapped: TOverlapped; lpCompletionRoutine: FARPROC): BOOL; stdcall;
 TGetModuleFileNameA  = function(hModule: HINST; lpFilename: PAnsiChar; nSize: DWORD): DWORD; stdcall;
 TGetModuleFileNameW  = function (hModule: HINST; lpFilename: PWideChar; nSize: DWORD): DWORD; stdcall;
 TGetCurrentProcessId = function : DWORD; stdcall;
 TWriteProcessMemory  = function (hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
                                  nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL; stdcall;
 TReadProcessMemory   = function (hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
                                  nSize: DWORD; var lpNumberOfBytesRead: DWORD): BOOL; stdcall;
{$O-}
threadvar
 LockLog : integer;

function IsHookEnabled(ID : integer) : boolean;
begin
 Result := copy(KeyloggerHookMask, ID, 1) = '1';
end;

// Модификация машинного кода функции
function SetHookCode(InterceptInfo : TInterceptInfo; ASetHook : boolean) : boolean;
var
 Tmp, OldProtect     : dword;
begin
 Result := false;
 if InterceptInfo.FunctionAddr  = nil then exit;
 if InterceptInfo.FunctCodeSize = 0   then exit;
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
 // Вызов функции из LDasm
 Result := SizeOfCode(APtr, nil);
end;

function InterceptFunctionEx(ALibName, AFunctName : string; var InterceptInfo : TInterceptInfo; HookFunct: Pointer) : boolean;
var
 Tmp, Tmp1 : dword;
 JMP_Rel   : dword;
begin
 Result := false;
 InterceptInfo.FunctionAddr := nil;
 InterceptInfo.LibraryName  := '';
 InterceptInfo.FunctionName := '';
 InterceptInfo.HookAddr     := nil;
 // 1. Поиск адреса фукции
 InterceptInfo.FunctionAddr := GetProcAddress(GetModuleHandle(PChar(ALibName)), PChar(AFunctName));
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
 JMP_Rel := (DWORD(HookFunct) - (DWORD(InterceptInfo.FunctionAddr) + 5));
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
 // Установка привилегий на буфер
 VirtualProtect(@InterceptInfo.HookBuf[0], InterceptInfo.FunctCodeSize+5, PAGE_EXECUTE_READWRITE, Tmp);
 // Установка перехватчика
 Result := SetHookCode(InterceptInfo, true);
end;

function AddInterceptInfo(RetAddr : dword; FunctCode : integer; dwData : DWord = 0; stData : string = '') : integer;
var
 S : string;
 i, FoundIndx : integer;
begin
 Result := -1;
 if TekMode = 0 then exit;
 if LockLog > 0 then exit;
 try
   // Поправка на длинну команды CALL
   RetAddr := RetAddr - 6;
   S := '?';
   case TekMode of
    ModeKeyboard : S := '$AVZ0333';
    ModeMouse    : S := '$AVZ0445';
    ModeCBT      : S := 'CBT';
    ModeAllMsg   : S := '$AVZ0157';
   end;
   // Поиск данных о указанной точке
   Result := -1;
   for i := 0 to Length(KeyloggerTraceInfo) - 1 do
    if ((KeyloggerTraceInfo[i].CallPrt = RetAddr) and
        (KeyloggerTraceInfo[i].Breackpoint = FunctCode) and
        (KeyloggerTraceInfo[i].Mode = TekMode)
       ) then begin
     Result := i;
     Break;
    end;
   // Элемента нет - добавить
   if Result = -1 then begin
    SetLength(KeyloggerTraceInfo, Length(KeyloggerTraceInfo) + 1);
    Result := Length(KeyloggerTraceInfo) - 1;
    KeyloggerTraceInfo[Result].CallPrt := RetAddr;
    KeyloggerTraceInfo[Result].Breackpoint := FunctCode;
    KeyloggerTraceInfo[Result].Mode := TekMode;
    KeyloggerTraceInfo[Result].RptCount := 1;
    KeyloggerTraceInfo[Result].DopInfo := nil;
   end else
    KeyloggerTraceInfo[Result].RptCount := KeyloggerTraceInfo[Result].RptCount + 1;
   // Есть доп. информация до данному вызову
   stData := Trim(AnsiLowerCase(stData));
   if (dwData <> 0) or (stData <> '') then begin
    FoundIndx := -1;
    for i := 0 to Length(KeyloggerTraceInfo[Result].DopInfo) - 1 do
     if (KeyloggerTraceInfo[Result].DopInfo[i].dwData = dwData) and
        (KeyloggerTraceInfo[Result].DopInfo[i].stData = stData) then begin
         FoundIndx := i;
         Break;
        end;
     // Такого события еще не регистрировалось - регистрируем
     if FoundIndx = - 1 then begin
      SetLength(KeyloggerTraceInfo[Result].DopInfo, Length(KeyloggerTraceInfo[Result].DopInfo) + 1);
      FoundIndx := length(KeyloggerTraceInfo[Result].DopInfo) - 1;
      KeyloggerTraceInfo[Result].DopInfo[FoundIndx].dwData := dwData;
      KeyloggerTraceInfo[Result].DopInfo[FoundIndx].stData := stData;
     end;
   end;
 except
 end;
end;

// *****************************************************************************************************
// ************************ Перехватчики для мониторинга вызовов функций *******************************
// *****************************************************************************************************

// Перехватчик - аналог точки останова на функции CallNextHookEx
function MyCallNextHookEx(hhk: HHOOK; nCode: Integer; wParam: WPARAM; lParam: LPARAM): LRESULT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 1);
  Result := TCallNextHookEx(@CallNextHookExInterceptInfo.HookBuf[0])(hhk, nCode, wParam, lParam);
 finally
 end;
end;

// Перехватчик - аналог точки останова на функции GetKeyboardState
function MyGetKeyboardState(var KeyState: TKeyboardState): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 2);
  Result :=  TGetKeyboardState(@GetKeyboardStateInterceptInfo.HookBuf[0])(KeyState);
 finally
 end;
end;

function MyGetForegroundWindow: HWND; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 3);
  Result := TGetForegroundWindow(@GetForegroundWindowInterceptInfo.HookBuf[0]);
 finally
 end;
end;

function MyToAscii(uVirtKey, uScanCode: UINT; const KeyState: TKeyboardState;
  lpChar: PChar; uFlags: UINT): Integer; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 4);
  Result := TToAscii(@ToAsciiInterceptInfo.HookBuf[0])(uVirtKey, uScanCode, KeyState, lpChar, uFlags);
 finally
 end;
end;

function MyToAsciiEx(uVirtKey: UINT; uScanCode: UINT; const KeyState: TKeyboardState;
  lpChar: PChar; uFlags: UINT; dwhkl: HKL): Integer; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 5);
  Result := TToAsciiEx(@ToAsciiExInterceptInfo.HookBuf[0])(uVirtKey, uScanCode, KeyState, lpChar, uFlags, dwhkl);
 finally
 end;
end;

function MyGetFocus: HWND; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 6);
  Result := TGetFocus(@GetFocusInterceptInfo.HookBuf[0]);
 finally
 end;
end;

function MyGetKeyState(nVirtKey: Integer): SHORT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 7);
  Result := TGetKeyState(@GetKeyStateInterceptInfo.HookBuf[0])(nVirtKey);
 finally
 end;
end;

function MyGetClassNameA(hWnd: HWND; lpClassName: PAnsiChar; nMaxCount: Integer): Integer; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 8, hWnd);
  Result := TGetClassNameA(@GetClassNameAInterceptInfo.HookBuf[0])(hWnd, lpClassName, nMaxCount);
 finally
 end;
end;

function MyGetClassNameW(hWnd: HWND; lpClassName: PWideChar; nMaxCount: Integer): Integer; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 8, hWnd);
  Result := TGetClassNameW(@GetClassNameWInterceptInfo.HookBuf[0])(hWnd, lpClassName, nMaxCount);
 finally
 end;
end;

function MyGetActiveWindow: HWND; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 9);
  Result := TGetActiveWindow(@GetActiveWindowInterceptInfo.HookBuf[0]);
 finally
 end;
end;

function MySendMessageA(hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): LRESULT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 10, hWnd);
  Result := TSendMessageA(@SendMessageAInterceptInfo.HookBuf[0])(hWnd, Msg, wParam, lParam);
 finally
 end;
end;

function MySendMessageW(hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): LRESULT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 10, hWnd);
  Result := TSendMessageW(@SendMessageWInterceptInfo.HookBuf[0])(hWnd, Msg, wParam, lParam);
 finally
 end;
end;

function MyPostMessageA(hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 11, hWnd);
  Result := TPostMessageA(@PostMessageAInterceptInfo.HookBuf[0])(hWnd, Msg, wParam, lParam);
 finally
 end;
end;

function MyPostMessageW(hWnd: HWND; Msg: UINT; wParam: WPARAM; lParam: LPARAM): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 11, hWnd);
  Result := TPostMessageW(@PostMessageWInterceptInfo.HookBuf[0])(hWnd, Msg, wParam, lParam);
 finally
 end;
end;

function MyCreateFileA(lpFileName: PAnsiChar; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 28]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 12, 0, lpFileName);
  Result := TCreateFileA(@CreateFileAInterceptInfo.HookBuf[0])(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,dwFlagsAndAttributes,
                        hTemplateFile);
 finally
 end;
end;

function MyCreateFileW(lpFileName: PWideChar; dwDesiredAccess, dwShareMode: DWORD;
  lpSecurityAttributes: PSecurityAttributes; dwCreationDisposition, dwFlagsAndAttributes: DWORD;
  hTemplateFile: THandle): THandle; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 28]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 12, 0, WideCharToString(lpFileName));
  Result := TCreateFileW(@CreateFileWInterceptInfo.HookBuf[0])(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,dwFlagsAndAttributes,
                        hTemplateFile);
 finally
 end;
end;

function MyMapVirtualKeyA(uCode, uMapType: UINT): UINT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 13);
  Result := TMapVirtualKeyA(@MapVirtualKeyAInterceptInfo.HookBuf[0])(uCode, uMapType);
 finally
 end;
end;

function MyMapVirtualKeyW(uCode, uMapType: UINT): UINT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 13);
  Result := TMapVirtualKeyW(@MapVirtualKeyWInterceptInfo.HookBuf[0])(uCode, uMapType);
 finally
 end;
end;

function MyMapVirtualKeyExA(uCode, uMapType: UINT; dwhkl: HKL): UINT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 13);
  SetHookCode(MapVirtualKeyExAInterceptInfo, false);
  Result := MapVirtualKeyExA(uCode, uMapType, dwhkl);
 finally
  SetHookCode(MapVirtualKeyExAInterceptInfo, true);
 end;
end;

function MyMapVirtualKeyExW(uCode, uMapType: UINT; dwhkl: HKL): UINT; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 13);
  SetHookCode(MapVirtualKeyExWInterceptInfo, false);
  Result := MapVirtualKeyExW(uCode, uMapType, dwhkl);
 finally
  SetHookCode(MapVirtualKeyExWInterceptInfo, true);
 end;
end;

function MyGetKeyboardLayout(dwLayout: DWORD): HKL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 14);
  Result := TGetKeyboardLayout(@GetKeyboardLayoutInterceptInfo.HookBuf[0])(dwLayout);
 finally
 end;
end;

function MyGetKeyNameTextA(lParam: Longint; lpString: PAnsiChar; nSize: Integer): Integer; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 15);
  Result := TGetKeyNameTextA(@GetKeyNameTextAInterceptInfo.HookBuf[0])(lParam, lpString, nSize);
 finally
 end;
end;

function MyGetKeyNameTextW(lParam: Longint; lpString: PWideChar; nSize: Integer): Integer; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 15);
  Result := TGetKeyNameTextW(@GetKeyNameTextWInterceptInfo.HookBuf[0])(lParam, lpString, nSize);
 finally
 end;
end;

function MyWriteFile(hFile: THandle; const Buffer; nNumberOfBytesToWrite: DWORD;
  var lpNumberOfBytesWritten: DWORD; lpOverlapped: POverlapped): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 40]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 16, 0, GetFileNameByHandle(hFile));
  Result := TWriteFile(@WriteFileInterceptInfo.HookBuf[0])(hFile, Buffer, nNumberOfBytesToWrite,
                       lpNumberOfBytesWritten, lpOverlapped);
 finally
 end;
end;

function MyWriteFileEx(hFile: THandle; lpBuffer: Pointer; nNumberOfBytesToWrite: DWORD;
  const lpOverlapped: TOverlapped; lpCompletionRoutine: FARPROC): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 40]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 16, 0, GetFileNameByHandle(hFile));
  Result := TWriteFileEx(@WriteFileExInterceptInfo.HookBuf[0])(hFile, lpBuffer, nNumberOfBytesToWrite,
                   lpOverlapped, lpCompletionRoutine);
 finally
 end;
end;


function MyGetModuleFileNameA(hModule: HINST; lpFilename: PAnsiChar; nSize: DWORD): DWORD; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 17, hModule);
  Result := TGetModuleFileNameA(@GetModuleFileNameAInterceptInfo.HookBuf[0])(hModule, lpFilename, nSize);
 finally
 end;
end;

function MyGetModuleFileNameW(hModule: HINST; lpFilename: PWideChar; nSize: DWORD): DWORD; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 17, hModule);
  Result := TGetModuleFileNameW(@GetModuleFileNameWInterceptInfo.HookBuf[0])(hModule, lpFilename, nSize);
 finally
 end;
end;

function MyGetCurrentProcessId: HWND; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 18);
  Result := TGetCurrentProcessId(@GetCurrentProcessIdInterceptInfo.HookBuf[0]);
 finally
 end;
end;

function MyWriteProcessMemory(hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
  nSize: DWORD; var lpNumberOfBytesWritten: DWORD): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 19, hProcess);
  Result := TWriteProcessMemory(@WriteProcessMemoryInterceptInfo.HookBuf[0])(hProcess,lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
 finally
 end;
end;

function MyReadProcessMemory(hProcess: THandle; const lpBaseAddress: Pointer; lpBuffer: Pointer;
  nSize: DWORD; var lpNumberOfBytesRead: DWORD): BOOL; stdcall;
var
 RetAddr : DWORD;
begin
 try
  // Извлекаем точку возврата из стека +
  asm
   mov eax, dword ptr [esp + 24]
   mov RetAddr, eax
  end;
  AddInterceptInfo(RetAddr, 20, hProcess);
  Result := TReadProcessMemory(@ReadProcessMemoryInterceptInfo.HookBuf[0])(hProcess,lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
 finally
 end;
end;


function KeyloggerHunter : boolean;
var
 i : integer;
 // Очистка очереди сообщений
 procedure ClearMessageList;
 begin
  TekMode := 0;
  Application.ProcessMessages;
  Application.ProcessMessages;
 end;
 // Дразнение кейлоггера оконными событиями
 procedure WindowMsgTest;
 var
  TestForm  : TForm;
 begin
  TestForm  := TForm.Create(nil);
  TestForm.Visible := false;
  TestForm.left   := 10000;
  TestForm.Refresh;
  Application.ProcessMessages;
  TestForm.Repaint;
  Application.ProcessMessages;
  TestForm.Width  := 100;
  TestForm.Height := 100;
  Application.ProcessMessages;
  TestForm.Left := TestForm.Left + 1;
  Application.ProcessMessages;
  TestForm.Top  := TestForm.Top + 1;
  TestForm.Free;
 end;
begin
 Result := false;
 KeyloggerTraceInfo := nil;
 // Под Win98 это в данной реализации не сработает
 // поэтому "дразним"  перехвачик и завершаем анализ
 if not(ISNT) then begin
   keybd_event(byte('T'), 123, KEYEVENTF_KEYUP, 0);
   keybd_event(byte('E'), 123, KEYEVENTF_KEYUP, 0);
   keybd_event(byte('S'), 123, KEYEVENTF_KEYUP, 0);
   keybd_event(byte('T'), 123, KEYEVENTF_KEYUP, 0);
   mouse_event(MOUSEEVENTF_MOVE, 1, 1, 0, 0);
   mouse_event(MOUSEEVENTF_WHEEL, 1, 1, 0, 0);
   WindowMsgTest;
   exit;
 end;
 try
  ClearMessageList;
  LockLog := 1;
  // ************ Установка перехватов *************
{  InterceptFunctionEx('kernel32.dll','WriteProcessMemory',
                     WriteProcessMemoryInterceptInfo, @MyWriteProcessMemory);
  InterceptFunctionEx('kernel32.dll','ReadProcessMemory',
                     ReadProcessMemoryInterceptInfo, @MyReadProcessMemory);}
  if IsHookEnabled(1) then
    InterceptFunctionEx('user32.dll','CallNextHookEx',
                       CallNextHookExInterceptInfo, @MyCallNextHookEx);
  if IsHookEnabled(2) then
    InterceptFunctionEx('user32.dll','GetKeyboardState',
                       GetKeyboardStateInterceptInfo, @MyGetKeyboardState);
  if IsHookEnabled(3) then
    InterceptFunctionEx('user32.dll','GetForegroundWindow',
                       GetForegroundWindowInterceptInfo, @MyGetForegroundWindow);
  if IsHookEnabled(4) then
    InterceptFunctionEx('user32.dll','ToAscii',
                       ToAsciiInterceptInfo, @MyToAscii);
  if IsHookEnabled(5) then
    InterceptFunctionEx('user32.dll','ToAsciiEx',
                       ToAsciiExInterceptInfo, @MyToAsciiEx);
  if IsHookEnabled(6) then
    InterceptFunctionEx('user32.dll','GetFocus',
                       GetFocusInterceptInfo, @MyGetFocus);
  if IsHookEnabled(7) then
    InterceptFunctionEx('user32.dll','GetKeyState',
                       GetKeyStateInterceptInfo, @MyGetKeyState);
  if IsHookEnabled(8) then
    InterceptFunctionEx('user32.dll','GetClassNameA',
                       GetClassNameAInterceptInfo, @MyGetClassNameA);
  if IsHookEnabled(9) then
    InterceptFunctionEx('user32.dll','GetClassNameW',
                       GetClassNameWInterceptInfo, @MyGetClassNameW);
  if IsHookEnabled(10) then
    InterceptFunctionEx('user32.dll','GetActiveWindow',
                        GetActiveWindowInterceptInfo, @MyGetActiveWindow);
  if IsHookEnabled(11) then
    InterceptFunctionEx('user32.dll','SendMessageA',
                       SendMessageAInterceptInfo, @MySendMessageA);
  if IsHookEnabled(12) then
    InterceptFunctionEx('user32.dll','SendMessageW',
                       SendMessageWInterceptInfo, @MySendMessageW);
  if IsHookEnabled(13) then
    InterceptFunctionEx('user32.dll','PostMessageA',
                       PostMessageAInterceptInfo, @MyPostMessageA);
  if IsHookEnabled(14) then
    InterceptFunctionEx('user32.dll','PostMessageW',
                       PostMessageWInterceptInfo, @MyPostMessageW);
  if IsHookEnabled(15) then
    InterceptFunctionEx('kernel32.dll','CreateFileA',
                       CreateFileAInterceptInfo, @MyCreateFileA);
  if IsHookEnabled(16) then
    InterceptFunctionEx('kernel32.dll','CreateFileW',
                       CreateFileWInterceptInfo, @MyCreateFileW);
  if IsHookEnabled(17) then
    InterceptFunctionEx('kernel32.dll','WriteFile',
                       WriteFileInterceptInfo, @MyWriteFile);
  if IsHookEnabled(18) then
    InterceptFunctionEx('kernel32.dll','WriteFileEx',
                       WriteFileExInterceptInfo, @MyWriteFileEx);
  if IsHookEnabled(19) then
    InterceptFunctionEx('user32.dll','GetKeyboardLayout',
                       GetKeyboardLayoutInterceptInfo, @MyGetKeyboardLayout);
  if IsHookEnabled(20) then
    InterceptFunctionEx('user32.dll','GetKeyNameTextA',
                       GetKeyNameTextAInterceptInfo, @MyGetKeyNameTextA);
  if IsHookEnabled(21) then
    InterceptFunctionEx('user32.dll','GetKeyNameTextW',
                       GetKeyNameTextWInterceptInfo, @MyGetKeyNameTextW);
  if IsHookEnabled(22) then
    InterceptFunctionEx('kernel32.dll','GetModuleFileNameA',
                       GetModuleFileNameAInterceptInfo, @MyGetModuleFileNameA);
  if IsHookEnabled(23) then
    InterceptFunctionEx('kernel32.dll','GetModuleFileNameW',
                       GetModuleFileNameWInterceptInfo, @MyGetModuleFileNameW);
  if IsHookEnabled(24) then
    InterceptFunctionEx('kernel32.dll','GetCurrentProcessId',
                       GetCurrentProcessIdInterceptInfo, @MyGetCurrentProcessId);
  LockLog := 0;
  // ************* "Дразним" возможные перехватчики **************
  // Отработали накопившиеся сообщения в очереди
  ClearMessageList;
  // 1. Клавиатура
  TekMode := ModeKeyboard;
  for i := 1 to 50 do begin
   keybd_event(byte('T'), 123, KEY_NOTIFY, 0);  keybd_event(byte('T'), 123, KEYEVENTF_KEYUP, 0);
   keybd_event(byte('E'), 123, KEY_NOTIFY, 0);  keybd_event(byte('E'), 123, KEYEVENTF_KEYUP, 0);
   keybd_event(byte('S'), 123, KEY_NOTIFY, 0);  keybd_event(byte('S'), 123, KEYEVENTF_KEYUP, 0);
   keybd_event(byte('T'), 123, KEY_NOTIFY, 0);  keybd_event(byte('T'), 123, KEYEVENTF_KEYUP, 0);
   Application.ProcessMessages;
   if i < 5 then
    sleep(50);
   SendMessage(Application.handle, WM_KEYDOWN, 1, 2);
   PostMessage(Application.handle, WM_KEYDOWN, 1, 2);
   SendMessage(Application.handle, WM_KEYUP, 1, 2);
   Application.ProcessMessages;
  end;
  ClearMessageList;
  // 2. Мышь
  TekMode := ModeMouse;
  for i := 1 to 5 do begin
   mouse_event(MOUSEEVENTF_MOVE, 1, 1, 0, 0);
   mouse_event(MOUSEEVENTF_WHEEL, 1, 1, 0, 0);
   Application.ProcessMessages;
  end;
  ClearMessageList;
  // 3. Оконные события - создание/разрушение оконного элемента
  WindowMsgTest;
  ClearMessageList;
  // 4. Пользовательское сообщение
  TekMode := ModeAllMsg;
  for i := 1 to 5 do begin
   SendMessage(Application.handle, WM_USER+i, 1, 2);
  end;
  ClearMessageList;
  Result := true;
 finally
  // ************** Снятие перехватов ********************
  Application.ProcessMessages;
{  SetHookCode(WriteProcessMemoryInterceptInfo, false);
   SetHookCode(ReadProcessMemoryInterceptInfo, false);
}
  if IsHookEnabled(1) then
   SetHookCode(CallNextHookExInterceptInfo, false);
  if IsHookEnabled(2) then
   SetHookCode(GetKeyboardStateInterceptInfo, false);
  if IsHookEnabled(3) then
   SetHookCode(GetForegroundWindowInterceptInfo, false);
  if IsHookEnabled(4) then
   SetHookCode(ToAsciiInterceptInfo, false);
  if IsHookEnabled(5) then
   SetHookCode(ToAsciiExInterceptInfo, false);
  if IsHookEnabled(6) then
   SetHookCode(GetFocusInterceptInfo, false);
  if IsHookEnabled(7) then
   SetHookCode(GetKeyStateInterceptInfo, false);
  if IsHookEnabled(8) then
   SetHookCode(GetClassNameAInterceptInfo, false);
  if IsHookEnabled(9) then
   SetHookCode(GetClassNameWInterceptInfo, false);
  if IsHookEnabled(10) then
   SetHookCode(GetActiveWindowInterceptInfo, false);
  if IsHookEnabled(11) then
   SetHookCode(SendMessageAInterceptInfo, false);
  if IsHookEnabled(12) then
   SetHookCode(SendMessageWInterceptInfo, false);
  if IsHookEnabled(13) then
   SetHookCode(PostMessageAInterceptInfo, false);
  if IsHookEnabled(14) then
   SetHookCode(PostMessageWInterceptInfo, false);
  if IsHookEnabled(15) then
   SetHookCode(CreateFileAInterceptInfo, false);
  if IsHookEnabled(16) then
   SetHookCode(CreateFileWInterceptInfo, false);
  if IsHookEnabled(17) then
   SetHookCode(WriteFileInterceptInfo, false);
  if IsHookEnabled(18) then
   SetHookCode(WriteFileExInterceptInfo, false);
  if IsHookEnabled(19) then
   SetHookCode(GetKeyboardLayoutInterceptInfo, false);
  if IsHookEnabled(20) then
   SetHookCode(GetKeyNameTextAInterceptInfo, false);
  if IsHookEnabled(21) then
   SetHookCode(GetKeyNameTextWInterceptInfo, false);
  if IsHookEnabled(22) then
   SetHookCode(GetModuleFileNameAInterceptInfo, false);
  if IsHookEnabled(23) then
   SetHookCode(GetModuleFileNameWInterceptInfo, false);
  if IsHookEnabled(24) then
   SetHookCode(GetCurrentProcessIdInterceptInfo, false);
end;
end;

// **************** Запрос результатов по заданной DLL *************************
function GetModuleTraceResult(AModuleBase, AModuleSize : DWORD) : string;
var
 i, j : integer;
 ModeList, TargetHwndList : TList;
 FilesList, FilesSList, ModuleSList, ReadPMList, WritePmList : TStringList;
 FGetActiveWindowFlg, FGetKeyState,
 FGetKeyboard, FGetKeyboardLayout,
 FGetKeyNameText, FGetAscii,
 FMapVirtualKey, FGetCurrentProcessId : boolean;
 S : string;
 procedure AddToLog(ALogStr : string);
 begin
  AddToList(Result, ALogStr, #$0D#$0A);
 end;
begin
 Result := '';
 FGetActiveWindowFlg := false;
 FGetKeyState := false;FGetKeyboard := false;
 FGetKeyNameText := false;
 FGetKeyboardLayout := false;
 FGetAscii := false;
 FMapVirtualKey := false;
 FGetCurrentProcessId := false;
 ModeList        := TList.Create;
 TargetHwndList  := TList.Create;
 FilesList       := TStringList.Create;
 FilesSList      := TStringList.Create;
 ModuleSList     := TStringList.Create;
 ReadPMList      := TStringList.Create;
 WritePmList     := TStringList.Create;
 try
 // Просмотр таблицы с результатами регистрации событий. Условие - более 2х событий с точкой возврата
 // внутри указанного диапазона адресов
 for i := 0 to Length(KeyloggerTraceInfo) - 1 do
  if (((KeyloggerTraceInfo[i].CallPrt >= AModuleBase) and
      (KeyloggerTraceInfo[i].CallPrt <= AModuleBase + AModuleSize+1000)) and
      (KeyloggerTraceInfo[i].RptCount > 0)) then
   begin
    // Создание списка режимов без повторов
    if ModeList.IndexOf(pointer(KeyloggerTraceInfo[i].Mode)) < 0 then
     ModeList.Add(pointer(KeyloggerTraceInfo[i].Mode));
    // Отладочный вывод
//Main.AddToLog(inttostr(KeyloggerTraceInfo[i].Mode)+':'+inttostr(KeyloggerTraceInfo[i].Breackpoint)+' '+IntToHex(KeyloggerTraceInfo[i].CallPrt, 8));
    case KeyloggerTraceInfo[i].Breackpoint of
      6, 9, 3 :  FGetActiveWindowFlg := true;
      7  :  FGetKeyState := true;
      2  :  FGetKeyboard := true;
     14  :  FGetKeyboardLayout := true;
     4,5 :  FGetAscii := true;
     13  : FMapVirtualKey := true;
     15  : FGetKeyNameText := true;
     18  : FGetCurrentProcessId := true;
    end;
    // Анализ дополнительной информации
    for j := 0 to Length(KeyloggerTraceInfo[i].DopInfo) - 1 do begin
// Main.AddToLog(' '+inttostr(KeyloggerTraceInfo[i].Mode)+':'+inttostr(KeyloggerTraceInfo[i].Breackpoint)+' = '+inttostr(KeyloggerTraceInfo[i].DopInfo[j].dwData) + ' '+KeyloggerTraceInfo[i].DopInfo[j].stData);
     // Точки 10 (SendMessage) и 11 (PostMessage)
     if KeyloggerTraceInfo[i].Breackpoint in [10, 11] then
      if TargetHwndList.IndexOf(pointer(KeyloggerTraceInfo[i].DopInfo[j].dwData)) < 0 then
       TargetHwndList.Add(pointer(KeyloggerTraceInfo[i].DopInfo[j].dwData));
     // Точки 12 - работа с файлами
     if KeyloggerTraceInfo[i].Breackpoint in [12] then
      FilesList.Add(KeyloggerTraceInfo[i].DopInfo[j].stData);
     // Точки 16 - запись в файл
     if KeyloggerTraceInfo[i].Breackpoint in [16] then
      FilesSList.Add(KeyloggerTraceInfo[i].DopInfo[j].stData);
     // Точки 17 - определение имени модуля
     if KeyloggerTraceInfo[i].Breackpoint in [17] then
      ModuleSList.Add(GetModuleName(KeyloggerTraceInfo[i].DopInfo[j].dwData));
     // Точки 19 - запись в память процесса
     if KeyloggerTraceInfo[i].Breackpoint in [19] then
      WritePmList.Add(inttostr(GetProcessIDByHandle(KeyloggerTraceInfo[i].DopInfo[j].dwData))+' '+GetProcessImageNameByHandle(KeyloggerTraceInfo[i].DopInfo[j].dwData));
     // Точки 20 - чтение памяти процесса
     if KeyloggerTraceInfo[i].Breackpoint in [20] then
      ReadPmList.Add(inttostr(GetProcessIDByHandle(KeyloggerTraceInfo[i].DopInfo[j].dwData))+' '+GetProcessImageNameByHandle(KeyloggerTraceInfo[i].DopInfo[j].dwData));
    end;
  end;
  // Формирования текстового списка регистрируемых событий
  S := '';
  for i := 0 to ModeList.Count - 1 do
   AddToList(S, ModeNames[integer(ModeList[i])], ', ');
  if S <> '' then
   AddToLog('$AVZ0902: '+S);
  // Вывод данных о передаче инфомации сторонним окнам
  S := '';
  for i := 0 to TargetHwndList.Count - 1 do begin
   J := GetPIDByWindowHandle((integer(TargetHwndList[i])));
   AddToLog('$AVZ0702: '+
            inttostr(J)+ ' '+
            GetProcessByPID(J)+
            ' ($AVZ0566 = "'+GetWindowTextByHandle(integer(TargetHwndList[i]))+'")'
            );
  end;
  for i := 0 to FilesList.Count - 1 do
   AddToLog('$AVZ0885: ' + FilesList[i]);
  for i := 0 to FilesSList.Count - 1 do
   AddToLog('$AVZ0271: ' + FilesSList[i]);
  for i := 0 to ModuleSList.Count - 1 do
   if ModuleSList[i] <> '' then
    AddToLog('$AVZ0593: ' + ExtractFileName(ModuleSList[i]));
  if FGetActiveWindowFlg then
   AddToLog('$AVZ0198');

  if FGetKeyState then
   AddToLog('$AVZ0590');
  if FGetKeyboard then
   AddToLog('$AVZ0589');
  if FGetKeyboard then
   AddToLog('$AVZ0587');
  if FGetKeyNameText then
   AddToLog('$AVZ0588');
  if FGetAscii then
   AddToLog('$AVZ0591');
  if FMapVirtualKey then
   AddToLog('$AVZ1068');
  if FGetCurrentProcessId then
   AddToLog('$AVZ0592');
  for i := 0 to ReadPMList.Count - 1 do
   if ReadPMList[i] <> '' then
    AddToLog('$AVZ1145: ' + (ReadPMList[i]));
  for i := 0 to WritePMList.Count - 1 do
   if WritePMList[i] <> '' then
    AddToLog('$AVZ0719: ' + (WritePMList[i]));
  ModeList.Free;
  TargetHwndList.Free;
  FilesList.Free;
  FilesSList.Free;
  ModuleSList.Free;
  ReadPMList.Free;
  WritePmList.Free;
 finally
  Result := TranslateStr(Result);
 end; 
end;
{$O+}

begin

end.
{
Получение PID по Handle окна
var pProcID: ^DWORD;
begin
  GetMem(pProcID, SizeOf(DWORD));
  GetWindowThreadProcessId(WinHandle, pProcID);
end;

GetWindowText - имя окна
}



