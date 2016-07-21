unit zAVZDriverN;

interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, winsvc, StdCtrls, ntdll, AclAPI, AccCtrl, zSharedFunctions, zAntivirus,
  ZLogSystem;
type

 TAVZDriverSG = class
  private
    FEnabled: boolean;
    FLoaded: boolean;
    FDriverPath, FDriverName: string;
    ServiceAPILoaded : boolean;
    procedure SetEnabled(const Value: boolean);
    procedure SetDriverPath(const Value: string);
  public
   KMBase       : TKMBase;
   DriverSvcName, DriverLinkName : string;
   LastStatus : DWORD;
   constructor Create;
   function CheckDriverEnabled : boolean;
   function InstallDriver   : boolean;
   function LoadDriver      : boolean;
   function UnLoadDriver    : boolean;
   function UnInstallDriver : boolean;
   function IOCTL_Z_SET_TRUSTEDPROCESS(PID: dword): boolean;
   function IOCTL_Z_ADD_TRUSTEDPROCESS(PID: dword): boolean;
   function IOCTL_Z_ENABLEULNOAD: boolean;
   function IOCTL_Z_RESTRICTEDMODE(AMode : boolean): boolean;
  published
   Property Enabled : boolean read FEnabled write SetEnabled;
   Property Loaded  : boolean read FLoaded;
   Property DriverPath : string read FDriverPath write SetDriverPath;
 end;

var
 AVZDriverSG : TAVZDriverSG;
 OldExitProcessCode : packed array [0..5] of byte;
 ExitProcessHooked  : boolean = false;
 PeekMessageHooked  : boolean = false;

// Активировать защиту
Function ActivateAVZGuard : boolean;
// Деактивировать защиту
Function DeActivateAVZGuard : boolean;
// Настройка RestrictedMode
Function SetAVZGuardRestrictedMode(ANewMode : boolean) : boolean;
// Настройка TrustedProcess
Function SetAVZGuardTrustedProcess(APID : dword; ANewStatus : boolean) : boolean;
// Проверка, включе ли AVZ Guard
Function AVZGuardIsOn : boolean;

// Активировать защиту от вызова ExitProcess
Function ActivateInternalAntiExitProcess : boolean;
// Активировать защиту от передачи сообщений Application
Function ActivateInternalAntiApplicationMessage : boolean;

// Деактивировать внутреннюю защиту
procedure DeactivateInternalProcessProtection;
procedure ActivateInternalProcessProtection;


implementation
uses zutil;
{ TAVZDriverN }

function TAVZDriverSG.IOCTL_Z_ADD_TRUSTEDPROCESS(
  PID: dword): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 CopyMemory(@InpBuf[0], @PID, 4);
 IOCode := CTL_CODE($022,$811,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, sizeof(InpBuf),
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverSG.IOCTL_Z_SET_TRUSTEDPROCESS(PID: dword): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 CopyMemory(@InpBuf[0], @PID, 4);
 IOCode := CTL_CODE($022,$810,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, sizeof(InpBuf),
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverSG.CheckDriverEnabled: boolean;
begin
 Result := IsNT and FEnabled and ServiceAPILoaded;
end;

constructor TAVZDriverSG.Create;
begin
 FLoaded   := false;
 FEnabled  := true;
 ServiceAPILoaded := LoadServiceManagerAPI;
 // Имя файла
 FDriverName := 'avzsg.sys';
 FDriverName := GenerateRandomName(8, 2) + '.sys';
 // Имя службы в реестре
 DriverSvcName := 'AVZSG';
 DriverSvcName := GenerateRandomName(8, 2);
 // Имя линка
 DriverLinkName := '\\.\\AvzSGKernelDeviceLink';
end;

function TAVZDriverSG.InstallDriver: boolean;
begin
 if not(CheckDriverEnabled) then exit;
 if KMBase <> nil then
  KMBase.CreateDriverFile('AVZSG', DriverPath+FDriverName, 0, DriverLinkName);
 LastStatus := zSharedFunctions.InstallDriver(DriverPath+FDriverName,
                                          DriverSvcName, 'AVZ-SG Kernel Driver');
 Result := LastStatus = STATUS_SUCCESS;
end;

function TAVZDriverSG.IOCTL_Z_ENABLEULNOAD: boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 InpBuf[0] := 1;
 IOCode := CTL_CODE($022,$815,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 4,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverSG.IOCTL_Z_RESTRICTEDMODE(AMode: boolean): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 if AMode then
  InpBuf[0] := 1;
 IOCode := CTL_CODE($022,$816,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 4,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverSG.LoadDriver: boolean;
begin
 Result := false;
 if not(CheckDriverEnabled) then exit;
 LastStatus := zSharedFunctions.LoadDriver(DriverSvcName);
 Result := (LastStatus = STATUS_SUCCESS) or (LastStatus = STATUS_IMAGE_ALREADY_LOADED);
 FLoaded := Result;
end;

procedure TAVZDriverSG.SetDriverPath(const Value: string);
begin
  FDriverPath := Value;
end;

procedure TAVZDriverSG.SetEnabled(const Value: boolean);
begin
  FEnabled := Value;
end;

function TAVZDriverSG.UnInstallDriver: boolean;
begin
 LastStatus := zSharedFunctions.UnInstallDriver(DriverSvcName);
 DeleteFile(FDriverPath+FDriverName);
 Result := LastStatus = STATUS_SUCCESS;
end;

function TAVZDriverSG.UnLoadDriver: boolean;
begin
 Result := zSharedFunctions.UnLoadDriver(DriverSvcName) = STATUS_SUCCESS;
 FLoaded := false;
end;

// Активировать защиту
Function ActivateAVZGuard : boolean;
begin
 Result := false;
 AVZDriverSG.UnInstallDriver;
 // Установка и загрузка драйвера
 Result := AVZDriverSG.InstallDriver;
 Result := AVZDriverSG.LoadDriver;
 // Ошибка - выгрузка и удаление
 if not(Result) then begin
  AddToLog('$AVZ0636: '+IntToHex(AVZDriverSG.LastStatus, 8), logError);
  AVZDriverSG.UnLoadDriver;
  AVZDriverSG.UnInstallDriver;
  exit;
 end;
 // Установка "доверенного безопасного" процесса - это сам AVZ 
 Result := AVZDriverSG.IOCTL_Z_SET_TRUSTEDPROCESS(GetCurrentProcessId);
 if not(Result) then begin
  AVZDriverSG.UnLoadDriver;
  AVZDriverSG.UnInstallDriver;
 end;
 ActivateInternalProcessProtection;
end;

Function DeActivateAVZGuard : boolean;
begin
 AVZDriverSG.IOCTL_Z_ENABLEULNOAD;
 AVZDriverSG.UnLoadDriver;
 AVZDriverSG.UnLoadDriver;
 AVZDriverSG.UnInstallDriver;
 Result := not AVZDriverSG.Loaded;
 // Отключение внутренней защиты процесса
 DeactivateInternalProcessProtection;
end;

// Настройка RestrictedMode
Function SetAVZGuardRestrictedMode(ANewMode : boolean) : boolean;
begin
 Result := AVZDriverSG.IOCTL_Z_RESTRICTEDMODE(ANewMode);
end;

// Проверка, включей AVZ Guard
Function AVZGuardIsOn : boolean;
begin
 Result := AVZDriverSG.Loaded;
end;

// Настройка TrustedProcess
Function SetAVZGuardTrustedProcess(APID : dword; ANewStatus : boolean) : boolean;
begin
 AVZDriverSG.IOCTL_Z_ADD_TRUSTEDPROCESS(APID);
end;

procedure DeactivateInternalProcessProtection;
var
 pExitProcess : pointer;
 lpNumberOfBytesWritten: DWORD;
begin
 // Восстановление PeekMessage
 if PeekMessageHooked then begin
  ModifyProgrammAPICall(@PeekMessage, @PeekMessageA);
  PeekMessageHooked := false;
 end;
 if not(ISNT) then exit;
 // Восстановление ExitProcess
 PExitProcess := GetProcAddress(GetModuleHandle('kernel32.dll'), 'ExitProcess');
 if (pExitProcess <> nil) and (ExitProcessHooked) then begin
  WriteProcessMemory(GetCurrentProcess, pExitProcess, @OldExitProcessCode[0], 5, lpNumberOfBytesWritten);
  ExitProcessHooked := false;
 end;
end;

procedure ActivateInternalProcessProtection;
begin
 ActivateInternalAntiExitProcess;
 ActivateInternalAntiApplicationMessage;
end;

procedure MyExitProc;
begin
 DeactivateInternalProcessProtection;
end;

// Активировать защиту
Function ActivateInternalAntiExitProcess : boolean;
var
 pExitProcess : pointer;
 lpNumberOfBytesWritten: DWORD;
 b : packed array[0..2] of byte;
begin
 Result := false;
 if not(ISNT) then exit;
 if ExitProcessHooked then exit;
 PExitProcess := GetProcAddress(GetModuleHandle('kernel32.dll'), 'ExitProcess');
 if pExitProcess = nil then exit;
 b[0] := $C2;
 b[1] := $04;
 b[2] := $00;
 ReadProcessMemory(GetCurrentProcess, pExitProcess, @OldExitProcessCode[0], 5, lpNumberOfBytesWritten);
 WriteProcessMemory(GetCurrentProcess, pExitProcess, @b[0], 3, lpNumberOfBytesWritten);
 ExitProcessProc := MyExitProc;
 ExitProcessHooked := true;
 Result := true;
end;


function MyPeekMessage(var lpMsg: TMsg; hWnd: HWND;
  wMsgFilterMin, wMsgFilterMax, wRemoveMsg: UINT): BOOL; stdcall;
begin
 Result := PeekMessageA(lpMsg, hWnd,
  wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
 //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 if not(AVZGuardIsOn) then exit;
 if lpMsg.message = WM_QUIT then begin
  Result := false;
 end;
 if lpMsg.message = 16 then begin
  Result := false;
 end;
 if lpMsg.message = SC_CLOSE then begin
  Result := false;
 end;
 if lpMsg.message = WM_CLOSE then begin
  Result := false;
 end;
end;

// Активировать защиту от передачи сообщений Application
Function ActivateInternalAntiApplicationMessage : boolean;
begin
 if ModifyProgrammAPICall(@PeekMessage, @MyPeekMessage) then
  PeekMessageHooked := true;
end;

initialization
 AVZDriverSG := TAVZDriverSG.Create;
 AVZDriverSG.DriverPath := GetSystemDirectoryPath+'Drivers\';
finalization
 AVZDriverSG.UnInstallDriver;
 AVZDriverSG.Free;
end.
