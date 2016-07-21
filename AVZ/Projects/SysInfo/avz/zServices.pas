unit zServices;

interface
uses
  Windows, Messages, SysUtils, Variants, ntdll, winsvc, registry, Classes, ZLogSystem;
 type
  // Информация о службе
  TServiceInfo = record
   Name           : string;   // Имя службы
   Descr          : string;   // Описание службы
   Status         : string;   // Статус
   BinFile        : string;   // Путь к бинарному файлу
   CheckResult    : integer;  // Результат проверки по базе известных файлов
   Group          : string;   // Группа
   Dependencies   : string;   // Зависимости
   dwServiceType,             // Тип службы
   dwCurrentState : DWORD;    // Состояние службы
  end;

type
  // Массив описания служб
  TServiceArray = array of TServiceInfo;
  // Менеджер служб
  TServiceManager = class
  private
    function ExecuteControlService(ServiceName: String;
      ServiceControlCode: DWORD) : boolean;
  protected
   FAPILoaded          : boolean; // Признак того, что API загружено
   Function CompareItems(I1, I2 : TServiceInfo; AKeyField : integer) : boolean;
  public
   ServiceArray : TServiceArray;
   constructor Create;
   // Обновление списка сервисов по данным API
   function RefreshServicesByAPI(TypeFilter, StateFilter : DWord) : boolean;
   // Обновление списка сервисов по реестру
   function RefreshServicesByReg(TypeFilter, StateFilter : DWord) : boolean;
   // Остановка сервиса
   function StopService(AServiceName : string) : boolean;
   // Запуск сервиса
   function StartService(AServiceName : string) : boolean;
   // Пауза сервиса
   function PauseService(AServiceName : string) : boolean;
   // Запрос режима автозапуска
   function GetServiceStart(AServiceName : string) : integer;
   function SetServiceStart(AServiceName : string; AStart : integer) : boolean;
   // Запрос отображаемого имени службы
   function GetServiceName(AServiceName: string): string;
   // Удаление сервиса
   function DeleteService(AServiceName : string) : boolean;
   function DeleteServiceByFile(AFileName : string) : boolean;
   // Поиск сервиса в таблицы
   function ServiceExists(AServiceName : string; AServiceArray : TServiceArray) : boolean;
   // Создание текстовго отчета
   function CreateCSVRep(ALines : TStrings; ADelimStr : string) : boolean;
   // Сортировка данных
   Function SortData(AKeyField : integer) : boolean;
   function NormalServiceFile(AFileName, DefExt: string): string;
   // Разрушение класса
   destructor Destroy; override;
  end;

implementation
uses
 ZUtil,SystemMonitor, zSharedFunctions;

{ TServiceManager }

constructor TServiceManager.Create;
begin
 FAPILoaded := LoadServiceManagerAPI;
 ServiceArray := nil;
end;

destructor TServiceManager.Destroy;
begin
 ServiceArray := nil;
 inherited;
end;

function TServiceManager.RefreshServicesByAPI(TypeFilter, StateFilter : DWord): boolean;
var
 SCManagerHandle, SCHandle : THandle;
 lpServices : array of TEnumServiceStatus;
 lpQueryServiceConfig : PQueryServiceConfig;
 pcbBytesNeeded, lpServicesReturned, lpResumeHandle, ServiceConfigSize: DWORD;
 ServiceMode, ServiceStatus : integer;
 i  : integer;
 S  : string;
begin
 // Очистка списка
 ServiceArray := nil;
 Result := false;
 // Если API не загружено, то операцию не выполняем
 if not(FAPILoaded) then exit;
 // 1. Подключениемся к менеджеру сервисов
 SCManagerHandle := zOpenSCManager(nil, nil, GENERIC_READ);
 case TypeFilter of
  0 : ServiceMode := SERVICE_WIN32;
  1 : ServiceMode := SERVICE_DRIVER;
  2 : ServiceMode := SERVICE_WIN32 or SERVICE_DRIVER;
 end;
 case StateFilter of
  0 : ServiceStatus := SERVICE_ACTIVE;
  1 : ServiceStatus := SERVICE_INACTIVE;
  2 : ServiceStatus := SERVICE_ACTIVE or SERVICE_INACTIVE;
 end;
 // ResumeHandle := 0 !! Это важно, т.к. это задает пречисление
 // сервисов с начала
 lpResumeHandle := 0;
 // 2. Перечисление сервисов (размер буфера = 0 - мы определяем, какой реально
 // размер необходим для списка сервисов
 SetLength(lpServices, 1);
 zEnumServicesStatus(SCManagerHandle,
                    ServiceMode,
                    ServiceStatus,
                    lpServices[0], // !! В описании функции ошибка - здесь должен быть указатель на массив элементов
                    0,
                    pcbBytesNeeded,
                    lpServicesReturned,
                    lpResumeHandle);
 // 3. Настройка размера массива
 SetLength(lpServices, pcbBytesNeeded div SizeOf(TEnumServiceStatus));
 // 4. Повторный запрос списка сервисов (размер массива соответствует определенному на шаге 2)
 lpResumeHandle := 0;
 zEnumServicesStatus(SCManagerHandle,
                    ServiceMode,
                    ServiceStatus,
                    lpServices[0],
                    Length(lpServices) * SizeOf(TEnumServiceStatus),
                    pcbBytesNeeded,
                    lpServicesReturned,
                    lpResumeHandle);
 // Вывод полученных данных в ListView1
 SetLength(ServiceArray, lpServicesReturned);
 for i := 0 to lpServicesReturned - 1 do begin
  ServiceArray[i].Name  := lpServices[i].lpServiceName;
  ServiceArray[i].Descr :=lpServices[i].lpDisplayName;
  ServiceArray[i].dwCurrentState := lpServices[i].ServiceStatus.dwCurrentState;
  ServiceArray[i].dwServiceType :=  lpServices[i].ServiceStatus.dwServiceType;
  S := '??';
  // Расшифровка кода состояния
  case lpServices[i].ServiceStatus.dwCurrentState of
   SERVICE_STOPPED          : S := '$AVZ0475';
   SERVICE_START_PENDING    : S := '$AVZ0096';
   SERVICE_STOP_PENDING     : S := '$AVZ0095';
   SERVICE_RUNNING          : S := '$AVZ0884';
   SERVICE_CONTINUE_PENDING : S := '$AVZ0097';
   SERVICE_PAUSE_PENDING    : S := '$AVZ0094';
   SERVICE_PAUSED           : S := '$AVZ0151';
  end;
  ServiceArray[i].Status := S;
  // 5.1 Открытие сервиса
  SCHandle := zOpenService(SCManagerHandle, lpServices[i].lpServiceName, SERVICE_QUERY_CONFIG or SERVICE_QUERY_STATUS);
  zQueryServiceConfig(SCHandle, nil, 0, pcbBytesNeeded);
  if pcbBytesNeeded > 0 then begin
   ServiceConfigSize := pcbBytesNeeded;
   GetMem(lpQueryServiceConfig, ServiceConfigSize);
   zQueryServiceConfig(SCHandle, lpQueryServiceConfig, ServiceConfigSize, pcbBytesNeeded);
   ServiceArray[i].BinFile      := lpQueryServiceConfig^.lpBinaryPathName;
   if Trim(ServiceArray[i].BinFile) = '' then begin
    ServiceArray[i].BinFile := Trim(lpServices[i].lpServiceName);
    if pos('.', ServiceArray[i].BinFile) = 0 then ServiceArray[i].BinFile := ServiceArray[i].BinFile + '.sys';
   end;
   case TypeFilter of
    0 : ServiceArray[i].BinFile := NormalProgramFileName(ServiceArray[i].BinFile, '.exe');
    1 : ServiceArray[i].BinFile := NormalProgramFileName(ServiceArray[i].BinFile, '.sys');
    else
     ServiceArray[i].BinFile := NormalProgramFileName(ServiceArray[i].BinFile, '.exe');
   end;
   ServiceArray[i].Group        := lpQueryServiceConfig^.lpLoadOrderGroup;
   ServiceArray[i].Dependencies := lpQueryServiceConfig^.lpDependencies;
   ServiceArray[i].CheckResult  := -1;
   FreeMem(lpQueryServiceConfig, ServiceConfigSize);
  end;
  // 5.3 Закрытие сервиса
  zCloseServiceHandle(SCHandle);
 end;
 // 6. Закрытие менеджера
 zCloseServiceHandle(SCManagerHandle);
end;

function TServiceManager.ExecuteControlService(ServiceName: String; ServiceControlCode : DWORD) : boolean;
var
 SCManagerHandle, SCHandle  : THandle;
 ServiceStatus : TServiceStatus;
begin
 Result := false;
 if not(FAPILoaded) then exit;
 // 1. Подключениемся к менеджеру сервисов
 SCManagerHandle := OpenSCManager(nil, nil, GENERIC_READ);
 // 2. Открытие сервиса
 SCHandle := OpenService(SCManagerHandle, PChar(ServiceName), SERVICE_ALL_ACCESS);
 // 3. Управление состоянием
 Result := ControlService(SCHandle, ServiceControlCode, ServiceStatus);
 // 4. Закрытие сервиса
 CloseServiceHandle(SCHandle);
 // 5. Закрытие менеджера
 CloseServiceHandle(SCManagerHandle);
end;

function TServiceManager.StopService(AServiceName: string): boolean;
begin
 Result := false;
 // Если API не загружено, то операцию не выполняем
 if not(FAPILoaded) then exit;
 ExecuteControlService(AServiceName,  SERVICE_CONTROL_STOP);
 Result := true;
end;

function TServiceManager.StartService(AServiceName: string): boolean;
var
 SCManagerHandle, SCHandle  : THandle;
 p : PChar;
begin
 Result := false;
 // Если API не загружено, то операцию не выполняем
 if not(FAPILoaded) then exit;
 // 1. Подключениемся к менеджеру сервисов
 SCManagerHandle := OpenSCManager(nil, nil, GENERIC_READ);
 // 2. Открытие сервиса
 SCHandle := OpenService(SCManagerHandle, PChar(AServiceName), SERVICE_ALL_ACCESS);
 // 3. Запуск сервиса
 p := nil;
 Result := zStartService(SCHandle, 0, p);
 // 4. Закрытие сервиса
 CloseServiceHandle(SCHandle);
 // 5. Закрытие менеджера
 CloseServiceHandle(SCManagerHandle);
end;

function TServiceManager.PauseService(AServiceName: string): boolean;
begin
 Result := false;
 // Если API не загружено, то операцию не выполняем
 if not(FAPILoaded) then exit;
 ExecuteControlService(AServiceName, SERVICE_CONTROL_PAUSE);
end;

function TServiceManager.ServiceExists(AServiceName: string;
  AServiceArray: TServiceArray): boolean;
var
 i : integer;
begin
 Result := false;
 AServiceName := LowerCase(AServiceName);
 for i := 0 to Length(AServiceArray) - 1 do
  if AServiceName = LowerCase(AServiceArray[i].Name) then begin
   Result := true;
   exit;
  end;
end;

function TServiceManager.RefreshServicesByReg(TypeFilter,
  StateFilter: DWord): boolean;
var
 Reg : TRegistry;
 KeyNames : TStringList;
 ServiceInfo : TServiceInfo;
 ServiceMode, ServiceStatus : integer;
 i : integer;
begin
 // Очистка списка
 ServiceArray := nil;
 Result := false;
 // Создание необходимых классов
 Reg      := TRegistry.Create;
 KeyNames := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 // Создание списка ключей в SYSTEM\CurrentControlSet\Services
 if Reg.OpenKey('SYSTEM\CurrentControlSet\Services', false) then begin
  Reg.GetKeyNames(KeyNames);
  Reg.CloseKey;
 end;
 case TypeFilter of
  0 : ServiceMode := SERVICE_WIN32;
  1 : ServiceMode := SERVICE_DRIVER;
  2 : ServiceMode := SERVICE_WIN32 or SERVICE_DRIVER;
 end;
 case StateFilter of
  0 : ServiceStatus := SERVICE_ACTIVE;
  1 : ServiceStatus := SERVICE_INACTIVE;
  2 : ServiceStatus := SERVICE_ACTIVE or SERVICE_INACTIVE;
 end;
 // Сканирование по списку
 for i := 0 to KeyNames.Count - 1 do
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Services\'+KeyNames[i], false) then begin
   if Reg.ValueExists('ImagePath') then begin
    ServiceInfo.Name := KeyNames[i];
    ServiceInfo.Status := '?';
    try ServiceInfo.Descr   := Reg.ReadString('DisplayName'); except end;
    try ServiceInfo.BinFile := Reg.ReadString('ImagePath'); except end;
    try ServiceInfo.Group   := Reg.ReadString('Group'); except ServiceInfo.Group := '' end;
    try ServiceInfo.dwServiceType   := Reg.ReadInteger('Type'); except ServiceInfo.dwServiceType  := $FFFFFFFF; end;
    if ((ServiceInfo.dwServiceType and ServiceMode) > 0) or (ServiceInfo.dwServiceType = $FFFFFFFF)  then begin
     SetLength(ServiceArray, Length(ServiceArray)+1);
     ServiceArray[Length(ServiceArray)-1] := ServiceInfo;
    end; 
   end;
   Reg.CloseKey;
  end;
 Reg.Free;
 KeyNames.Free;
end;

function TServiceManager.CreateCSVRep(ALines: TStrings; ADelimStr : string): boolean;
var
 i : integer;
begin
 ALines.Add('Name;Descr;Status;BinFile;Group;CheckResult');
 for i := 0 to Length(ServiceArray) - 1 do begin
  ALines.Add(ServiceArray[i].Name+ADelimStr+ServiceArray[i].Descr+ADelimStr+ServiceArray[i].Status+ADelimStr+ServiceArray[i].BinFile+ADelimStr+ServiceArray[i].Group+ADelimStr+IntToStr(ServiceArray[i].CheckResult));
 end;
end;

function TServiceManager.NormalServiceFile(AFileName: string; DefExt : string): string;
begin
 Result := NormalProgramFileName(AFileName, '.exe');
 if not(FileExists(Result)) then
  Result := NormalProgramFileName(AFileName, '.sys');
end;

function TServiceManager.DeleteService(AServiceName: string): boolean;
var
 SCManagerHandle, SCHandle : THandle;
begin
 Result := false;
 // Если API не загружено, то операцию не выполняем
 if not(FAPILoaded) then exit;
 // 1. Подключениемся к менеджеру сервисов
 SCManagerHandle := OpenSCManager(nil, nil, GENERIC_READ);
 // 2. Открытие сервиса
 SCHandle := OpenService(SCManagerHandle, PChar(AServiceName), SERVICE_ALL_ACCESS);
 // 3. Удаление
 Result  := zDeleteService(SCHandle);
 // 4. Закрытие сервиса
 CloseServiceHandle(SCHandle);
 // 5. Закрытие менеджера
 CloseServiceHandle(SCManagerHandle);
end;


function TServiceManager.CompareItems(I1, I2: TServiceInfo;
  AKeyField: integer): boolean;
begin
 Result := false;
 case AKeyField of
  -1 : Result := (AnsiLowerCase(I1.Name) > AnsiLowerCase(I2.Name)) or (I1.dwCurrentState < I2.dwCurrentState);
   0 : Result := AnsiLowerCase(I1.Name) > AnsiLowerCase(I2.Name);
   1 : Result := AnsiLowerCase(I1.Descr) > AnsiLowerCase(I2.Descr);
   2 : Result := AnsiLowerCase(I1.Status) > AnsiLowerCase(I2.Status);
   3 : Result := AnsiLowerCase(I1.BinFile) > AnsiLowerCase(I2.BinFile);
   4 : Result := AnsiLowerCase(I1.Group) > AnsiLowerCase(I2.Group);
   5 : Result := AnsiLowerCase(I1.Dependencies) > AnsiLowerCase(I2.Dependencies);
 end;
end;

function TServiceManager.SortData(AKeyField: integer): boolean;
var
 i, j : integer;
 Tmp  : TServiceInfo;
begin
 for i := 0 to Length(ServiceArray) - 2 do
  for j := i+1 to Length(ServiceArray) - 1 do
   if CompareItems(ServiceArray[i], ServiceArray[j], AKeyField) then begin
    Tmp :=  ServiceArray[i];
    ServiceArray[i] := ServiceArray[j];
    ServiceArray[j] := Tmp;
   end;
end;

function TServiceManager.DeleteServiceByFile(AFileName: string): boolean;
var
 Reg : TRegistry;
 i   : integer;
begin
 Result := false;
 try
  AFileName := LowerCase(AFileName);
  for i := 0 to length(ServiceArray)-1 do
   if LowerCase(NTFileNameToNormalName(ServiceArray[i].BinFile)) = (AFileName) then begin
    Reg := TRegistry.Create;
    Reg.RootKey := HKEY_LOCAL_MACHINE;
    if Reg.DeleteKey('SYSTEM\CurrentControlSet\Services\'+ServiceArray[i].Name) then
     AddToLog('$AVZ0424 '+ServiceArray[i].Name, logInfo);
    Reg.Free;
   end;
 except
 end;
end;

function TServiceManager.GetServiceStart(AServiceName: string): integer;
var
 Reg : TRegistry;
begin
 Result := -1;
 // Создание необходимых классов
 Reg      := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 try
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Services\'+AServiceName, false) then
   if Reg.ValueExists('Start') then
    Result := Reg.ReadInteger('Start');
 except
 end;
 Reg.Free;
end;

function TServiceManager.GetServiceName(AServiceName: string): string;
var
 Reg : TRegistry;
begin
 Result := '';
 // Создание необходимых классов
 Reg      := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 try
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Services\'+AServiceName, false) then
   if Reg.ValueExists('DisplayName') then
    Result := Reg.ReadString('DisplayName');
 except
 end;
 Reg.Free;
end;

function TServiceManager.SetServiceStart(AServiceName: string;
  AStart: integer): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 // Создание необходимых классов
 Reg      := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 try
  if Reg.OpenKey('SYSTEM\CurrentControlSet\Services\'+AServiceName, false) then begin
   Reg.WriteInteger('Start', AStart);
   Result := true;
  end;
 except
 end;
 Reg.Free;
end;

end.
