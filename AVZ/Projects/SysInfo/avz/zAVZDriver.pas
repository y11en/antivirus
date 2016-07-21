unit zAVZDriver;

interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, winsvc, StdCtrls, ntdll, zSharedFunctions, zAntivirus, zLogSystem;
type
 TIDT = packed record
  Size : word;
  Base : DWORD;
 end;
 TIDTGATE = packed record
  off1 : word;
  sel  : word;
  none, flags : byte;
  off2 : word;
 end;

 // Класс для обмена с драйвером общего назначения
 TAVZDriver = class
  private
    FEnabled: boolean;
    FLoaded: boolean;
    FDriverPath, FDriverName: string;
    ServiceAPILoaded : boolean;
    procedure SetEnabled(const Value: boolean);
    procedure SetDriverPath(const Value: string);
  public
   DriverSvcName, DriverLinkName : string;
   LastStatus : DWORD;
   KMBase : TKMBase;
   // Конструктор
   constructor Create; virtual;
   // Проверка, разрешен ли драйвер
   function CheckDriverEnabled : boolean; virtual;
   // Инсталляция драйвера
   function InstallDriver   : boolean;  virtual;
   // Загрузка драйвера
   function LoadDriver      : boolean;  virtual;
   // Выгрузка драйвера
   function UnLoadDriver    : boolean;  virtual;
   // Деинсталляция драйвера
   function UnInstallDriver : boolean;  virtual;
   // Подготовка кода управления
   function IOC(ACode : dword) : dword; virtual;
   // Запрос указателя на KIST
   function CallDriver_GET_KESDT_PTR: dword; virtual;
   // Запрос указателя на SDT
   function CallDriver_GET_SDT_PTR : dword; virtual;
   // Запрос кол-ва вхождений SDT
   function CallDriver_GET_SDT_COUNT: dword; virtual;
   // Запрос адреса из ячейки SDT с номером N
   function CallDriver_GET_SDT_ENTRY(N: dword): dword; virtual;
   // Запись нового адреса в ячейку SDT с номером N
   function CallDriver_SET_SDT_ENTRY(N, NewPtr: dword): boolean; virtual;
   // Удаление файла из Kernel Mode (устарело в связи с появлением BC)
   function CallDriver_DELETE_FILE(AFileName : string): boolean; virtual;
   // Чтение блока памяти
   function CallDriver_READ_MEMORY(Buf : pointer; MemAdd : DWord; MemSize : word): boolean; virtual;
   // Запись блока памяти
   function CallDriver_WRITE_MEMORY(Buf : pointer; MemAdd : DWord; MemSize : word): boolean; virtual;
   // Чтение регистра ЦП 176
   function CallDriver_GET_CPU_176REG : dword; virtual;
   // Установка регистра ЦП 176
   function CallDriver_SET_CPU_176REG(NewPtr: dword): boolean; virtual;
   // Запрос IDT
   function CallDriver_GET_IDT(var IDT : TIDT) : boolean; virtual;
   // Установка PID процесса, который имеет право работать с драйвером
   function CallDriver_IOCTL_Z_SETTRUSTEDPID : boolean; virtual;
   // Дампирование памяти
   function CallDriver_DUMP_MEMORY(Buf: pointer; MemAdd: DWord;
      MemSize: word): boolean; virtual;
  published
   // Признак того, что драйвер разрешен
   Property Enabled : boolean read FEnabled write SetEnabled;
   // Признак того, что драйвер загружен
   Property Loaded  : boolean read FLoaded;
   // Путь к файлу драйвера
   Property DriverPath : string read FDriverPath write SetDriverPath;
 end;

 // Драйвер KIS
 TAVZDriverKIS = class(TAVZDriver)
  // Конструктор
  constructor Create; override;
  // Инсталляция драйвера
  function InstallDriver   : boolean;  override;
  // Загрузка драйвера
  function LoadDriver      : boolean;  override;
  // Выгрузка драйвера
  function UnLoadDriver    : boolean;  override;
  // Деинсталляция драйвера
  function UnInstallDriver : boolean;  override;
  // Подготовка кода управления
  function IOC(ACode : dword) : dword; override;
 end;

implementation

{ TAVZDriver }

function TAVZDriver.CallDriver_GET_KESDT_PTR: dword;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode, dw : DWord;
 Res        : boolean;
begin
 Result := 0;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 dw := 0;
 IOCode := CTL_CODE($022, IOC($800), 0, 0);
 Res := DeviceIoControl(hDriver, IOCode,
                        @dw, sizeof(dw),
                        @dw, sizeof(dw),
                        BytesReturned, nil);
 if not(Res) then Result := 0
  else Result := dw;
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_GET_SDT_PTR: dword;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode, dw : DWord;
 Res        : boolean;
begin
 Result := 0;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 dw := 0;
 IOCode := CTL_CODE($022, IOC($801), 0,0);
 Res := DeviceIoControl(hDriver, IOCode,
                        @dw, sizeof(dw),
                        @dw, sizeof(dw),
                        BytesReturned, nil);
 if not(Res) then Result := 0
  else Result := dw;
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_GET_SDT_COUNT: dword;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode, dw : DWord;
 Res        : boolean;
begin
 Result := 0;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 dw := 0;
 IOCode := CTL_CODE($022, IOC($802), 0,0);
 Res := DeviceIoControl(hDriver, IOCode,
                        @dw, sizeof(dw),
                        @dw, sizeof(dw),
                        BytesReturned, nil);
 if not(Res) then Result := 0
  else Result := dw;
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_GET_SDT_ENTRY(N : dword): dword;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode, dw : DWord;
 Res        : boolean;
begin
 Result := 0;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 dw := 0;
 IOCode := CTL_CODE($022, IOC($803), 0,0);
 Res := DeviceIoControl(hDriver, IOCode,
                        @N, sizeof(N),
                        @dw, sizeof(dw),
                        BytesReturned, nil);
 if not(Res) then Result := 0
  else Result := dw;
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_SET_SDT_ENTRY(N, NewPtr : dword): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 CopyMemory(@InpBuf[0], @N, 4);
 CopyMemory(@InpBuf[4], @NewPtr, 4);
 IOCode := CTL_CODE($022, IOC($804), 0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, sizeof(InpBuf),
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriver.CheckDriverEnabled: boolean;
begin
 Result := IsNT and FEnabled and ServiceAPILoaded;
end;

constructor TAVZDriver.Create;
var
 i : integer;
begin
 FLoaded   := false;
 FEnabled  := true;
 // Имя файла
 FDriverName := 'avz.sys'; //#DNL
 FDriverName := GenerateRandomName(8, 1) + '.sys';  //#DNL
 // Имя службы
 DriverSvcName := 'AVZ';
 DriverSvcName := GenerateRandomName(8, 1);
 ServiceAPILoaded := LoadServiceManagerAPI;
 // Имя линка
 DriverLinkName := '\\.\\AvzKernelDeviceLink'; //#DNL
end;

function TAVZDriver.InstallDriver: boolean;
begin
 if not(CheckDriverEnabled) then exit;
 if KMBase <> nil then
  KMBase.CreateDriverFile('AVZ', DriverPath+FDriverName, 0, DriverLinkName); //#DNL
 LastStatus := zSharedFunctions.InstallDriver(DriverPath+FDriverName,
                                          DriverSvcName, 'AVZ Kernel Driver'); //#DNL
 AddToDebugLog('InstallDriver '+DriverSvcName+',"'+DriverPath+FDriverName+'"');
 Result := LastStatus = STATUS_SUCCESS;
end;

function TAVZDriver.LoadDriver: boolean;
begin
 Result := false;
 if not(CheckDriverEnabled) then exit;
 AddToDebugLog('LoadDriver '+DriverSvcName);
 LastStatus := zSharedFunctions.LoadDriver(DriverSvcName); //#DNL
 Result := (LastStatus = STATUS_SUCCESS) or (LastStatus = STATUS_IMAGE_ALREADY_LOADED);
 FLoaded := Result;
end;

procedure TAVZDriver.SetDriverPath(const Value: string);
begin
  FDriverPath := Value;
end;

procedure TAVZDriver.SetEnabled(const Value: boolean);
begin
  FEnabled := Value;                                           
end;

function TAVZDriver.UnInstallDriver: boolean;
begin
 AddToDebugLog('UnInstallDriver '+DriverSvcName);
 LastStatus := zSharedFunctions.UnInstallDriver(DriverSvcName); //#DNL
 DeleteFile(FDriverPath+FDriverName);
 Result := LastStatus = STATUS_SUCCESS;
end;

function TAVZDriver.UnLoadDriver: boolean;
begin
 AddToDebugLog('UnLoadDriver '+DriverSvcName);
 Result := zSharedFunctions.UnLoadDriver(DriverSvcName) = STATUS_SUCCESS; //#DNL
 FLoaded := false;
end;

function TAVZDriver.CallDriver_DELETE_FILE(AFileName : string): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..500] of byte;
 IOCode, dw : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 LastStatus := GetLastError;
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 StringToWideChar(AFileName, @InpBuf, sizeof(InpBuf));
 IOCode := CTL_CODE($022, IOC($805), 0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, length(AFileName)*2,
                        @dw, 4,
                        BytesReturned, nil);
 CloseHandle(hDriver);
 MessageDlg(inttohex(dw, 6), mtInformation, [mbOk], 0);
end;

function TAVZDriver.CallDriver_READ_MEMORY(Buf: pointer;
  MemAdd : DWord; MemSize: word): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..500000] of byte;
 IOCode, dw : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 if MemSize > SizeOf(OutBuf) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 LastStatus := GetLastError;
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 dw := MemAdd;  CopyMemory(@InpBuf[0], @dw, 4);
 dw := MemSize; CopyMemory(@InpBuf[4], @dw, 4);
 IOCode := CTL_CODE($022, IOC($806), 0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 10,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 if Result then
  CopyMemory(Buf, @OutBuf, BytesReturned);
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_DUMP_MEMORY(Buf: pointer;
  MemAdd : DWord; MemSize: word): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..5000] of byte;
 IOCode, dw : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 if MemSize > SizeOf(OutBuf) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 LastStatus := GetLastError;
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 dw := MemAdd;  CopyMemory(@InpBuf[0], @dw, 4);
 dw := MemSize; CopyMemory(@InpBuf[4], @dw, 4);
 IOCode := CTL_CODE($022, IOC($812), 0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 10,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 if Result then
  CopyMemory(Buf, @OutBuf, BytesReturned);
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_WRITE_MEMORY(Buf: pointer; MemAdd: DWord;
  MemSize: word): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..500] of byte;
 IOCode, dw : DWord;
begin
 Result := false;
 LastStatus := $F00001;
 if not(Loaded) then exit;
 LastStatus := GetLastError;
 if MemSize > SizeOf(OutBuf) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 dw := MemAdd;  CopyMemory(@InpBuf[0], @dw, 4);
 dw := MemSize; CopyMemory(@InpBuf[4], @dw, 4);
 CopyMemory(@InpBuf[8], Buf, MemSize);
 IOCode := CTL_CODE($022, IOC($807), 0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 8+MemSize,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_GET_CPU_176REG: dword;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode, dw : DWord;
 Res        : boolean;
begin
 Result := 0;
 LastStatus := $F00001;
 if not(Loaded) then exit;
 LastStatus := GetLastError;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 dw := 0;
 IOCode := CTL_CODE($022, IOC($808), 0,0);
 Res := DeviceIoControl(hDriver, IOCode,
                        @dw, sizeof(dw),
                        @dw, sizeof(dw),
                        BytesReturned, nil);
 if not(Res) then Result := 0
  else Result := dw;
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_IOCTL_Z_SETTRUSTEDPID: boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 InpBuf, OutBuf : array[0..500] of byte;
 IOCode, dw, X, X1, X2 : DWord;
begin
 Result := false;
 LastStatus := $F00001;
 if not(Loaded) then exit;
 // Открываем драйвер
 AddToDebugLog('Open driver link');
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 LastStatus := GetLastError;
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@InpBuf, sizeof(InpBuf));
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 dw := GetCurrentProcessId;  CopyMemory(@InpBuf[0], @dw, 4);
 X1 := random(1500) + 5;
 X2 := random(1500) + 5;
 X  := (X1*3+X2*2+155)*(X2-3) + dw;
 dw := X;  CopyMemory(@InpBuf[4], @dw, 4);
 dw := X1; CopyMemory(@InpBuf[8], @dw, 4);
 dw := X2; CopyMemory(@InpBuf[12], @dw, 4);
 IOCode := CTL_CODE($022, IOC($809), 0,0);
 AddToDebugLog('IOCTL Query');
 Result := DeviceIoControl(hDriver, IOCode,
                        @InpBuf, 16,
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_GET_IDT(var IDT : TIDT): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 IOCode, dw : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 dw := 0;
 IOCode := CTL_CODE($022, IOC($811), 0,0);
 ZeroMemory(@IDT, sizeof(IDT));
 Result := DeviceIoControl(hDriver, IOCode,
                        @dw, sizeof(dw),
                        @IDT, sizeof(IDT),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriver.CallDriver_SET_CPU_176REG(NewPtr: dword): boolean;
var
 hDriver : THandle;
 BytesReturned : Cardinal;
 OutBuf : array[0..100] of byte;
 IOCode     : DWord;
begin
 Result := false;
 if not(Loaded) then exit;
 // Открываем драйвер
 hDriver := CreateFile(PChar(DriverLinkName),GENERIC_READ, 0, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,0);
 if hDriver = INVALID_HANDLE_VALUE then exit;
 ZeroMemory(@OutBuf, sizeof(OutBuf));
 IOCode := CTL_CODE($022, IOC($810), 0,0);
 Result := DeviceIoControl(hDriver, IOCode,
                        @NewPtr, sizeof(NewPtr),
                        @OutBuf, sizeof(OutBuf),
                        BytesReturned, nil);
 CloseHandle(hDriver);
end;

function TAVZDriver.IOC(ACode: dword): dword;
begin
 Result := ACode;
end;

{ TAVZDriverKIS }

constructor TAVZDriverKIS.Create;
begin
 inherited;
 FLoaded   := true; // Драйвер уже загружен - он BOOT драйвер
 FEnabled  := true;
 // Имя файла (заглушка, мы не загружаем драйвер и не выгружаем его)
 FDriverName := ''; //#DNL
 // Имя службы (заглушка, мы не регистрируем этот драйвер)
 DriverSvcName := '';
 ServiceAPILoaded := LoadServiceManagerAPI;
 // Имя линка (берется из исходника CTLCODES.H)
 DriverLinkName := '\\.\\klbg007'; //#DNL
end;

function TAVZDriverKIS.InstallDriver: boolean;
begin
 // заглушка
 Result := true;
end;

function TAVZDriverKIS.IOC(ACode: dword): dword;
begin
 Result := ACode + $10;
end;

function TAVZDriverKIS.LoadDriver: boolean;
begin
 FLoaded := true;
 // заглушка
 Result := true;
end;

function TAVZDriverKIS.UnInstallDriver: boolean;
begin
 // заглушка
 Result := true;
end;

function TAVZDriverKIS.UnLoadDriver: boolean;
begin
 // заглушка
 Result := true;
end;

end.
