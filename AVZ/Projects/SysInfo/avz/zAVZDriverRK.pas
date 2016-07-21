unit zAVZDriverRK;
// Драйвер расширенного мониторинга

interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, winsvc, StdCtrls, ntdll, AclAPI, AccCtrl, zSharedFunctions, zAntivirus,
  registry;
type

 TPROCESS_ITEM = packed record
	PID       : DWORD;  // Идентификатор процесса
	EPROCESS  : DWORD;  // Указатель на структуру EPROCESS процесса
  NAME      : packed array[0..15] of char;
  FULL_NAME : UNICODE_STRING;
 end;

 TPROCESS_LIST = packed array[0..$FF] of TPROCESS_ITEM;

 TDRIVER_ITEM = packed record
  Base, Size : DWORD;
 end;

 TDRIVER_LIST = packed array[0..1024] of TDRIVER_ITEM;

 TAVZDriverRK = class
  private
    FEnabled: boolean;
    FLoaded: boolean;
    FDriverPath, FDriverName: string;
    ServiceAPILoaded : boolean;
    procedure SetEnabled(const Value: boolean);
    procedure SetDriverPath(const Value: string);
    function GetLoaded: boolean;
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
   function IOCTL_Z_GET_PROCESSLIST(var PROCESS_LIST : TPROCESS_LIST): boolean;
   function IOCTL_Z_GET_EPROCESS_PID(Indx : dword; var PID : dword): boolean;
   function IOCTL_Z_GET_EPROCESS_NAME(Indx : dword; var AName : string): boolean;
   function IOCTL_Z_GET_EPROCESS_FULLNAME(Indx : dword; var AName : string): boolean;
   function IOCTL_Z_KILL_PROCESS(Indx : dword): boolean;
   function IOCTL_Z_GET_DRIVERSLIST(var DRIVER_LIST : TDRIVER_LIST): boolean;
   function IOCTL_Z_GET_DRIVER_FULLNAME(Base : dword; var AName : string): boolean;
   function CallDriver_IOCTL_Z_SETTRUSTEDPID : boolean;
   function CallDriver_IOCTL_Z_ENABLEULNOAD: boolean;
  published
   Property Enabled : boolean read FEnabled write SetEnabled;
   Property Loaded  : boolean read GetLoaded;
   Property DriverPath : string read FDriverPath write SetDriverPath;
 end;

function InstallPMDriver: integer;
function UninstallPMDriver(AUnload : boolean): integer;

var
 AVZDriverRK : TAVZDriverRK = nil;

implementation
uses zutil;
{ TAVZDriverN }


function TAVZDriverRK.CheckDriverEnabled: boolean;
begin
 Result := IsNT and FEnabled and ServiceAPILoaded;
end;

constructor TAVZDriverRK.Create;
begin
 FLoaded   := false;
 FEnabled  := true;
 ServiceAPILoaded := LoadServiceManagerAPI;
 // Имя файла
 FDriverName := 'avzrkm.sys';
 FDriverName := GenerateRandomName(8, 3) + '.sys';
 // Имя службы в реестре
 DriverSvcName := 'AVZRK';
 DriverSvcName := GenerateRandomName(8, 3);
 // Имя линка
 DriverLinkName := '\\.\\AvzRMDeviceLink';
end;

function TAVZDriverRK.InstallDriver: boolean;
var
 Reg     : TRegistry;
 Group : String;
 i       : integer;
begin
 if not(CheckDriverEnabled) then exit;
 if KMBase <> nil then
  KMBase.CreateDriverFile('AVZRK', DriverPath+FDriverName, 0, DriverLinkName);
 LastStatus := zSharedFunctions.InstallDriver(DriverPath+FDriverName,
                                          DriverSvcName, 'AVZ-RK Kernel Driver', 1);
 Result := LastStatus = STATUS_SUCCESS;
 if Result then begin
  Group := '';
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  // Определение группы
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Control\ServiceGroupOrder', false) then begin
   try
    SetLength(Group, 4096);
    i := Reg.ReadBinaryData('List', Group[1], Length(Group));
    SetLength(Group, i);
    Delete(Group, 1, pos(#0, Group));
    Group := Trim(copy(Group, 1, pos(#0, Group)-1));
   except
    Group := '';
   end;
  end;
  Reg.CloseKey;
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Services\'+DriverSvcName, false) then begin
   if Group <> '' then
    Reg.WriteString('Group', Group); //#DNL
   Reg.WriteInteger('Tag', 1);       //#DNL
  end;
  Reg.CloseKey;
  Reg.Free
 end;
end;


function TAVZDriverRK.IOCTL_Z_GET_PROCESSLIST(var PROCESS_LIST : TPROCESS_LIST): boolean;
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
 IOCode := CTL_CODE($022,$801,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 4,
                        @PROCESS_LIST, sizeof(PROCESS_LIST),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.IOCTL_Z_GET_EPROCESS_PID(Indx : dword; var PID: dword): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 PID := 0;
 IOCode := CTL_CODE($022,$802,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @Indx, 4,
                        @PID, sizeof(PID),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.LoadDriver: boolean;
begin
 Result := false;
 if not(CheckDriverEnabled) then exit;
 LastStatus := zSharedFunctions.LoadDriver(DriverSvcName);
 Result := (LastStatus = STATUS_SUCCESS) or (LastStatus = STATUS_IMAGE_ALREADY_LOADED);
 FLoaded := Result;
end;

procedure TAVZDriverRK.SetDriverPath(const Value: string);
begin
  FDriverPath := Value;
end;

procedure TAVZDriverRK.SetEnabled(const Value: boolean);
begin
  FEnabled := Value;
end;

function TAVZDriverRK.UnInstallDriver: boolean;
begin
 LastStatus := zSharedFunctions.UnInstallDriver(DriverSvcName);
 DeleteFile(FDriverPath+FDriverName);
 Result := LastStatus = STATUS_SUCCESS;
end;

function TAVZDriverRK.UnLoadDriver: boolean;
begin
 Result := zSharedFunctions.UnLoadDriver(DriverSvcName) = STATUS_SUCCESS;
 FLoaded := false;
end;



function TAVZDriverRK.IOCTL_Z_GET_EPROCESS_NAME(Indx: dword;
  var AName: string): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode     : DWord;
 FName  : packed array [0..16] of char;
begin
 Result := false;
 AName := '';
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 IOCode := CTL_CODE($022,$803,0,0);
 ZeroMemory(@FName[0], sizeof(FName));
 Result := DeviceIoControl(hDriver, IOCode,
                        @Indx, 4,
                        @FName[0], sizeof(FName),
                        BytesReturned, nil);
 if Result then
  AName := trim(FName);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.IOCTL_Z_GET_EPROCESS_FULLNAME(Indx: dword;
  var AName: string): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode     : DWord;
 FName  : packed array [0..500] of WideChar;
begin
 Result := false;
 AName := '';
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 IOCode := CTL_CODE($022,$804,0,0);
 ZeroMemory(@FName[0], sizeof(FName));
 Result := DeviceIoControl(hDriver, IOCode,
                        @Indx, 4,
                        @FName[0], sizeof(FName),
                        BytesReturned, nil);
 if Result then
  AName := trim(FName);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.IOCTL_Z_KILL_PROCESS(Indx: dword): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 IOCode := CTL_CODE($022,$805,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @Indx, 4,
                        nil, 0,
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.IOCTL_Z_GET_DRIVERSLIST(
  var DRIVER_LIST: TDRIVER_LIST): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@DRIVER_LIST, sizeof(DRIVER_LIST));
 IOCode := CTL_CODE($022,$806,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 4,
                        @DRIVER_LIST, sizeof(DRIVER_LIST),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.IOCTL_Z_GET_DRIVER_FULLNAME(Base: dword;
  var AName: string): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode     : DWord;
 FName  : packed array [0..500] of WideChar;
begin
 Result := false;
 AName := '';
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 IOCode := CTL_CODE($022,$807,0,0);
 ZeroMemory(@FName[0], sizeof(FName));
 Result := DeviceIoControl(hDriver, IOCode,
                        @Base, 4,
                        @FName[0], sizeof(FName),
                        BytesReturned, nil);
 if Result then
  AName := trim(FName);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.GetLoaded: boolean;
var
 hDriver : THandle;
begin
 if KMBase <> nil then
  KMBase.GetDriverLink('AVZRK', 0, DriverLinkName); //#DNL
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 Result := hDriver <> INVALID_HANDLE_VALUE;
 CloseHandle(hDriver);
end;

function TAVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID: boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..500] of byte;
 IOCode, dw, X, X1, X2 : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 dw := GetCurrentProcessId;  CopyMemory(@InpBuf[0], @dw, 4);
 X1 := random(1500) + 5;
 X2 := random(1500) + 5;
 X  := (X1*7+X2*3+761-dw)*(X1+7) + dw*2;
 dw := X;  CopyMemory(@InpBuf[4], @dw, 4);
 dw := X1; CopyMemory(@InpBuf[8], @dw, 4);
 dw := X2; CopyMemory(@InpBuf[12], @dw, 4);
 IOCode := CTL_CODE($022,$810,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 16,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriverRK.CallDriver_IOCTL_Z_ENABLEULNOAD: boolean;
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
 IOCode := CTL_CODE($022,$811,0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 4,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function UninstallPMDriver(AUnload : boolean): integer;
begin
 // Удаление без выгрузки
 if not(AUnload) then begin
   AVZDriverRK.UnInstallDriver;
   Result := 7;
   exit;
 end;
 try
  Result := 1;
  if not(AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID) then exit;
  Result := 2;
  if not(AVZDriverRK.CallDriver_IOCTL_Z_ENABLEULNOAD) then exit;
  Result := 3;
  if not(AVZDriverRK.UnLoadDriver) then begin
   if AVZDriverRK.UnInstallDriver then Result := 7;
   exit;
  end;
  Result := 4;
  if not(AVZDriverRK.UnInstallDriver) then exit;
  Result := 0;
 finally
 end;
end;

function InstallPMDriver: integer;
begin
 try
  Result := 1;
  if not(AVZDriverRK.InstallDriver) then exit;
  Result := 2;
  if not(AVZDriverRK.LoadDriver) then exit;
  Result := 3;
  if not(AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID) then exit;
  Result := 0;
 finally
 end;
end;

initialization
finalization
end.
