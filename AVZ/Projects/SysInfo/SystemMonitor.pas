unit SystemMonitor;

interface
uses
  Windows, SysUtils, Classes, extctrls,
  TlHelp32, psapi, Registry,
  zAntivirus,
  NetAPIDef, RxVerInf, zSharedFunctions;

type

 // Информация о процессе
 TProcessInfo = class
  PID             : DWORD;     // PID
  ParentProcessID : DWORD;     // PID родительского процесса
  cntThreads      : DWORD;     // Кол-во потоков
  PriorityClass   : Longint;   // Приоритет процесса
  ExeFileName     : String;    // Имя EXE файла
  VisibleLevel    : integer;   // УРовень видимости процесса
 end;

 // Информация о модуле
 TModuleInfo = class
  hModule       : DWORD;   // Handle модуля
  ModuleSize    : DWORD;   // Размер модуля
  ModuleName    : String;  // Полное имя файла модуля
 end;

 // Информация о драйвере
 TDriverInfo = class
  hDriver       : DWORD;   // Handle драйвера
  DriverName    : String;  // Полное имя файла модуля
 end;

 // Статистика процесса
 TProcessStat = class
  PID            : DWORD;        // PID
  ExeFileName    : String;       // Имя EXE файла
  Count          : integer;      // Кол-во экземпляров в памяти
  IsScaned       : boolean;      // Признак того, что файл отсканирован
  SummTime       : int64;        // Суммарное время работы
  FirstStart     : TDateTime;    // Время первого запуска
  PEFileInfo     : TAVZFileInfo; // Информация о файле
 end;

 // Информация о процессах - базовый класс
 TSysProcessInfo = class
  protected
   FileScanner : TFileScanner;
  public
   Constructor Create;
   // Очистка списка
   procedure ClearList(AList : TStrings);
   // Уничтожение процесса
   Function KillProcessByPID(APID : DWORD; AExtKill : boolean) : boolean;
   // Получение информации о файле
   Function GetFileInfo(AFileName : string; var FileInfo : TAVZFileInfo) : boolean;
   // Получение списка окон
   Function CreateWindowList(Handle : THandle; AList : TStrings) : boolean; virtual;
   // Создание списка процессов
   Function CreateProcessList(AList : TStrings) : boolean; virtual; abstract;
   // Создание списка модулей процесса
   Function CreateModuleList(AList: TStrings; PID : DWORD) : boolean; virtual; abstract;
   // Создание списка драйверов
   Function CreateDriversList(AList: TStrings) : boolean; virtual; abstract;
 end;

 // Информация о процессах - на основе ToolHelp (Win98, W2K, W-XP. Не работает в NT)
 TToolHelpProcessInfo = class(TSysProcessInfo)
 public
  Function CreateProcessList(AList : Tstrings) : boolean; override;
  Function CreateModuleList(AList: TStrings; PID : DWORD) : boolean; override;
  Function CreateDriversList(AList: TStrings) : boolean; override;
 end;

 // Информация о процессах - на основе ToolHelp (Win98, W2K, W-XP. Не работает в NT)
 TPSAPIProcessInfo = class(TSysProcessInfo)
 public
  Function CreateProcessList(AList : Tstrings) : boolean; override;
  Function CreateModuleList(AList: TStrings; PID : DWORD) : boolean; override;
  Function CreateDriversList(AList: TStrings) : boolean; override;
 end;

 TProcessEvent   = procedure (Sender: TObject; ProcessInfo : TProcessInfo) of object;
 TCheckFileEvent = procedure (Sender: TObject; ProcessStat : TProcessStat; SysProcessInfo : TSysProcessInfo; ProcessInfo : TProcessInfo) of object;
 // Базовый класс мониторинга процессов
 TProcessMonitor = class
 private
  FTimeOut : integer;
  FTimer   : TTimer;
  FOnProcessStart: TProcessEvent;
  FOnProcessEnd: TProcessEvent;
  FSysProcessInfo : TSysProcessInfo;
  FOnCheckProcess: TCheckFileEvent;
  procedure OnTimer(Sender: TObject);
  procedure SetTimeOut(const Value: integer);
  procedure SetOnProcessStart(const Value: TProcessEvent);
  procedure SetOnProcessEnd(const Value: TProcessEvent);
  procedure SetOnCheckProcess(const Value: TCheckFileEvent);
 public
  ProcessList, ProcessListStat : TStringList;
  constructor Create;
  property TimeOut : integer read FTimeOut write SetTimeOut;
  function RefreshProcessStat: boolean;
  function ClearStat : boolean;
  property OnProcessStart : TProcessEvent read FOnProcessStart write SetOnProcessStart;
  property OnProcessEnd   : TProcessEvent read FOnProcessEnd write SetOnProcessEnd;
  property OnCheckProcess : TCheckFileEvent read FOnCheckProcess write SetOnCheckProcess;
  property SysProcessInfo : TSysProcessInfo read FSysProcessInfo;
 end;

 // Информация о системе
 TSysInfo = class
  private
    function GetWindowsHomePath: string;
    function GetWindowsSystemPath: string;
    function GetMemoryStatus: TMemoryStatus;
    function GetOSVersionInfo: TOSVersionInfo;
    function GetActiveWindowCaption: string;
    function GetCurrentComputerName: string;
 public
  // Путь к папке Windows
  property WindowsHomePath   : string read GetWindowsHomePath;
  // Путь к папке System
  property WindowsSystemPath : string read GetWindowsSystemPath;
  // Информация о памяти
  property MemoryStatus      : TMemoryStatus read GetMemoryStatus;
  // Информация о версии
  property VersionInfo       : TOSVersionInfo read GetOSVersionInfo;
  // Имя компьютера
  property ComputerName      : string read GetCurrentComputerName;
  // Имя активного окна
  property ActiveWindowCaption : string read GetActiveWindowCaption;
 end;

 // Информация об расшаренном ресурсе
 TNetShareInfo = record
  NetName  : string;   // Сетевое имя
  Remark   : string;   // Примечания
  Path     : string;   // Путь
  CurUsers : integer;  // Кол-во пользователей, подкл. к ресурсу
 end;
 TNetShareInfoArray = array of TNetShareInfo;

 // Сетевая сессия
 TNetSessionInfo = record
  SessionName : string;   // Имя сессии
  UserName    : string;   // Имя пользователя
  NumOpens    : integer;  // Кол-во открытых ресурсов
  Time,                   // Время соединения
  IdleTime    : cardinal; // Время простоя
 end;
 TNetSessionInfoArray = array of TNetSessionInfo;

 // Открытый по сети файл
 TNetFileInfo = record
  ID            : DWORD;
  Permissions   : DWORD;
  Num_locks     : DWORD;
  PathName      : String;
  UserName      : String;
  ShareName     : String;
 end;
 TNetFileInfoArray = array of TNetFileInfo;

 // Информация о сети
 TCustomMSNetInfo = class
 protected
  Function CompareItems(I1, I2 : TNetShareInfo; AKey : integer) : integer; overload;
  Function CompareItems(I1, I2 : TNetSessionInfo; AKey : integer) : integer; overload;
  Function CompareItems(I1, I2 : TNetFileInfo; AKey : integer) : integer; overload;
 public
  // Построение списка общих ресурсов
  Function CreateShareList(var AList : TNetShareInfoArray) : boolean; virtual; abstract;
  // Построение списка сессий
  Function CreateSessionsList(var AList : TNetSessionInfoArray) : boolean; virtual; abstract;
  Function DelSession(NetSessionInfo : TNetSessionInfo) : boolean; virtual; abstract;
  // Построение списка открытых файлов и ресурсов
  Function CreateOpenResList(var AList : TNetFileInfoArray) : boolean; virtual; abstract;
  Function CloseRes(NetFileInfo : TNetFileInfo) : boolean; virtual; abstract;
  // Сортировка списка
  Function OrderList(var AList : TNetShareInfoArray; AKey : integer) : boolean; overload;
  Function OrderList(var AList : TNetSessionInfoArray; AKey : integer) : boolean; overload;
  Function OrderList(var AList : TNetFileInfoArray; AKey : integer) : boolean; overload;
 end;

 // Информация о сети - реализация для NT
 TNTMSNetInfo = class(TCustomMSNetInfo)
  Function CreateShareList(var AList : TNetShareInfoArray) : boolean;  override;
  Function CreateSessionsList(var AList : TNetSessionInfoArray) : boolean;  override;
  Function DelSession(NetSessionInfo : TNetSessionInfo) : boolean; override;
  Function CreateOpenResList(var AList : TNetFileInfoArray) : boolean; override;
  Function CloseRes(NetFileInfo : TNetFileInfo) : boolean; override;
 end;

 // Информация о сети - реализация для 9x
 T9xMSNetInfo = class(TCustomMSNetInfo)
  Function CreateShareList(var AList : TNetShareInfoArray) : boolean;  override;
  Function CreateSessionsList(var AList : TNetSessionInfoArray) : boolean;  override;
  Function DelSession(NetSessionInfo : TNetSessionInfo) : boolean; override;
  Function CreateOpenResList(var AList : TNetFileInfoArray) : boolean; override;
  Function CloseRes(NetFileInfo : TNetFileInfo) : boolean; override;
 end;


// Проверка платформы (true - платформа NT)
function ISNT : boolean;
implementation
uses zutil;

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

{ TSysProcessInfo }

procedure TSysProcessInfo.ClearList(AList: TStrings);
var
 i : integer;
begin
 for i := 0 to AList.Count - 1 do begin
  if AList.Objects[i] is TProcessInfo then
   (AList.Objects[i] as TProcessInfo).Free
  else if AList.Objects[i] is TModuleInfo then
   (AList.Objects[i] as TModuleInfo).Free
  else if AList.Objects[i] is TDriverInfo then
   (AList.Objects[i] as TDriverInfo).Free;
 end;
 AList.Clear;
end;

constructor TSysProcessInfo.Create;
begin
 FileScanner := TFileScanner.Create;
end;

function TSysProcessInfo.CreateWindowList(Handle : THandle; AList: TStrings): boolean;
VAR
  Wnd   : hWnd;
  buff  : array [0..200] of Char;
begin
  Wnd := GetWindow(Handle, gw_HWndFirst);
  while Wnd <> 0 do begin  // Не показываем:
    IF IsWindowVisible(Wnd) AND {-Невидимые окна}
       (GetWindow(Wnd, gw_Owner) = 0) AND {-Дочернии окна}
       (GetWindowText(Wnd, buff, sizeof(buff)) <> 0){-Окна без заголовков}
    then begin
      GetWindowText(Wnd, buff, sizeof(buff));
      AList.Add(Trim(buff));
    end;
    Wnd := GetWindow(Wnd, gw_hWndNext);
  end;
end;

function TSysProcessInfo.GetFileInfo(AFileName: string; var FileInfo : TAVZFileInfo): boolean;
var
 VersionInfo : TVersionInfo;
begin
 Result := false;
 FileInfo.Size := -1;
 FileInfo.FileVersion := ''; FileInfo.LegalCopyright := '';
 FileInfo.FileName := AFileName;
 // Определение версии файла
 VersionInfo := TVersionInfo.Create(AFileName);
 FileInfo.FileVersion    := VersionInfo.FileVersion;
 FileInfo.LegalCopyright := VersionInfo.LegalCopyright;
 VersionInfo.Free;
 Result := FileScanner.CheckFile(AFileName);
 // Копирование информации о файле
 with FileScanner.PEFileInfo do begin
  FileInfo.FileName  := FileName;
  FileInfo.Size      := Size;
  FileInfo.IsEXEFile := IsEXEFile;
  FileInfo.IsPEFile  := IsPEFile;
  FileInfo.Sign[1]   := Sign[1];
  FileInfo.Sign[2]   := Sign[2];
  FileInfo.Sign[3]   := Sign[3];
  FileInfo.Sign[4]   := Sign[4];
 end;
 Result := true;
end;

function TSysProcessInfo.KillProcessByPID(APID: DWORD; AExtKill : boolean): boolean;
var
 hProcess : THandle;
begin
 hProcess := OpenProcess(PROCESS_TERMINATE, false, APID);
 Result := TerminateProcess(hProcess, 0);
 if not(Result) then begin
  // Активируем привилегию "Отладка"
  AdjustPrivileges('SeDebugPrivilege', true);
  hProcess := OpenProcess(PROCESS_TERMINATE, false, APID);
  Result := TerminateProcess(hProcess, 0);
 end;
end;


{ TProcessInfo }
function TToolHelpProcessInfo.CreateDriversList(AList: TStrings): boolean;
begin
 Result := false;
 ClearList(AList);
end;

function TToolHelpProcessInfo.CreateModuleList(AList: TStrings;
                                          PID: DWORD): boolean;
var
  hSnapshoot : THandle;
  me32              : TModuleEntry32;
  ModuleInfo        : TModuleInfo;
begin
  Result := false;
  ClearList(AList);
  // Создание снимка с данными о процессе
  hSnapshoot := CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, PID);
  if (hSnapshoot = -1) then
      Exit;
  me32.dwSize := SizeOf(TModuleEntry32);
  if (Module32First(hSnapshoot, me32)) then
  repeat
   { hFile := CreateFile(me32.szExePath,
                       GENERIC_READ, FILE_SHARE_READ, nil,
                       OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if hFile <> INVALID_HANDLE_VALUE then begin
     FileSize := GetFileSize(hFile, nil);
     CloseHandle(hFile);
    end;}
    ModuleInfo := TModuleInfo.Create;
    with ModuleInfo do begin
     hModule       := me32.hModule;
     ModuleSize    := me32.modBaseSize;
     ModuleName    := Trim(me32.szExePath);
    end;
    AList.AddObject('', ModuleInfo);
  until not Module32Next(hSnapshoot, me32);

  CloseHandle (hSnapshoot);
end;

function TToolHelpProcessInfo.CreateProcessList(AList: Tstrings): boolean;
var
  hSnapshoot  : THandle;
  pe32        : TProcessEntry32;
  ProcessInfo : TProcessInfo;
  hProcess    : THandle;
begin
  Result := false;
  ClearList(AList);
  // Создание снимка процессов
  hSnapshoot := CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  // Проверка результатов и выход в случае ошибки
  if (hSnapshoot = -1) then Exit;
  // Заполнение размера структуры и поиск первого процесса
  pe32.dwSize := SizeOf(TProcessEntry32);
  if (Process32First(hSnapshoot, pe32)) then
  repeat
   // Создание экземпляра класса "Информация о процессе"
   ProcessInfo := TProcessInfo.Create;

   // Копирование данных
   with ProcessInfo do begin
    PID                  := pe32.th32ProcessID;
    cntThreads           := pe32.cntThreads;
    ParentProcessID      := pe32.th32ParentProcessID;
    ExeFileName          := LowerCase(Trim(pe32.szExeFile));
    VisibleLevel         := 0;
   end;
   // Открытие процесса
   hProcess := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_VM_READ,
                           FALSE,
                           pe32.th32ProcessID);
   // Запрос приоритета
   ProcessInfo.PriorityClass := GetPriorityClass(hProcess);
   CloseHandle(hProcess);
   // Добавление информации о процессе в список
   AList.AddObject('', ProcessInfo);
  until not Process32Next(hSnapshoot, pe32);
  // Закрытие Snapshoot
  CloseHandle (hSnapshoot);
  Result := true;
end;

{ TPSAPIProcessInfo }

function TPSAPIProcessInfo.CreateDriversList(AList: TStrings): boolean;
var
 ArrSize, DriverPIDCount : DWORD;
 Res          : boolean;
 DriverInfo   : TDriverInfo;
 DriversPID   : packed array [0..1024] of DWORD;
 i            : integer;
 NameBuff     : array[0 .. 500] of char;
begin
  Result := false;
  ClearList(AList);
  // Получение списка PID драйверов
  Res := EnumDeviceDrivers(@DriversPID[0], sizeof(DriversPID), ArrSize);
  if not(Res) then exit;
  DriverPIDCount := ArrSize div sizeof(DWORD);
  for i := 0 to DriverPIDCount - 1 do  begin
   DriverInfo := TDriverInfo.Create;
   DriverInfo.hDriver := DriversPID[i];
   if GetDeviceDriverFileName(Pointer(DriversPID[i]),  NameBuff, sizeof(NameBuff)) > 0 then
    DriverInfo.DriverName := Trim(NameBuff)
      else DriverInfo.DriverName := '';
   AList.AddObject('', DriverInfo);
  end;
end;

function TPSAPIProcessInfo.CreateModuleList(AList: TStrings; PID: DWORD): boolean;
var
 ArrSize      : DWORD;
 Res          : boolean;
 ModuleInfo   : TModuleInfo;
 mi           : _MODULEINFO;
 hModule      : packed array [0..1024] of DWORD;
 hModuleCount : integer;
 i            : integer;
 hProcess     : THandle;
 NameBuff     : array[0 .. 500] of char;
begin
  Result := false;
  ClearList(AList);
  // Открытие процесса
  hProcess := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_VM_READ,
                          FALSE,PID);
   // Процесс открыт успешно - тогда его hProcess > 0
   if hProcess > 0 then begin
    // Создание списка модулей процесса
    // Построение списка модулей
    Res := EnumProcessModules(hProcess, @hModule, 1024, ArrSize);
    hModuleCount := Trunc(ArrSize / sizeof(DWORD));
    if Res then
     for i:= 0 to hModuleCount - 1 do
      if  hModule[i] > 0 then begin
       ModuleInfo := TModuleInfo.Create;
       ModuleInfo.hModule := hModule[i];
       GetModuleInformation(hProcess, hModule[i], @mi, sizeof(mi));
       ModuleInfo.ModuleSize := mi.SizeOfImage;
       // Запрос имени модуля
       if GetModuleFileNameEx(hProcess, hModule[i], NameBuff, sizeof(NameBuff)) > 0 then
        ModuleInfo.ModuleName := Trim(NameBuff)
         else ModuleInfo.ModuleName := '';
     AList.AddObject('', ModuleInfo);
    end;
   end;
   // Закрытие Handle Процесса
   CloseHandle(hProcess);
   Result := true;
end;

function TPSAPIProcessInfo.CreateProcessList(AList: Tstrings): boolean;
var
 ArrSize, ProcessesPIDCount : DWORD;
 Res          : boolean;
 ProcessInfo  : TProcessInfo;
 ProcessesPID : packed array [0..1024] of DWORD;
 hModule      : packed array [0..1024] of DWORD;
 i            : integer;
 hProcess     : THandle;
 NameBuff     : array[0 .. 500] of char;
 pmc          : PROCESS_MEMORY_COUNTERS;
begin
  Result := false;
  ClearList(AList);
  // Получение списка PID процессов
  Res := EnumProcesses(@ProcessesPID[0], sizeof(ProcessesPID), ArrSize);
  if not(res) then exit;
  ProcessesPIDCount := Trunc(ArrSize / sizeof(DWORD));
  for i := 0 to ProcessesPIDCount - 1 do
   if ProcessesPID[i] > 0 then begin
     // Открытие процесса
     hProcess := OpenProcess(PROCESS_QUERY_INFORMATION or PROCESS_VM_READ,
                             FALSE,
                             ProcessesPID[i]);
     // Процесс открыт успешно - тогда его hProcess > 0
     if hProcess > 0 then begin
      // Создание списка модулей процесса
      Res := EnumProcessModules(hProcess, @hModule, 1000, ArrSize);
      if Res and (hModule[0] > 0) then begin
       ProcessInfo := TProcessInfo.Create;
       ProcessInfo.PID := ProcessesPID[i];
       ProcessInfo.VisibleLevel         := 0;
       if GetModuleFileNameEx(hProcess, hModule[0], NameBuff, sizeof(NameBuff)) > 0 then
        ProcessInfo.ExeFileName :=LowerCase(Trim(NameBuff))
         else ProcessInfo.ExeFileName := '';
       // Запрос приоритета
       ProcessInfo.PriorityClass := GetPriorityClass(hProcess);
       // Запрос данных о памяти
       GetProcessMemoryInfo(hProcess, @pmc, sizeof(pmc));
       AList.AddObject('', ProcessInfo);
      end;
      // Закрытие Handle Процесса
      CloseHandle(hProcess);
     end;
  end;
  Result := true;
end;

{ TSysInfo }

function TSysInfo.GetActiveWindowCaption: string;
var
 Buff : array[0..255] of char;
 Wnd  : hWnd;
begin
 Result := '';
 Wnd := GetActiveWindow;
 if Wnd <> 0 then begin
  GetWindowText(Wnd, buff, sizeof(buff));
  Result := Trim(buff);
 end;
end;

function TSysInfo.GetCurrentComputerName: string;
var
  Buf : array[0..255] of char;
  BufSize : dword;
begin
 BufSize := 255;
 GetComputerName(Buf, BufSize);
 Result := Trim(Buf);
end;

function TSysInfo.GetMemoryStatus: TMemoryStatus;
begin
 GlobalMemoryStatus(Result);
end;

function TSysInfo.GetOSVersionInfo: TOSVersionInfo;
begin
 // Очистка структуры
 ZeroMemory(@Result, SizeOf(Result));
 // Запонение поля "Размер"
 Result.dwOSVersionInfoSize := SizeOf(Result);
 // Запрос информации
 GetVersionEx(Result);
end;

function TSysInfo.GetWindowsHomePath: string;
var
  Buf : array[0..MAX_PATH] of char;
begin
  GetWindowsDirectory(Buf, MAX_PATH);
  Result := Trim(Buf);
end;

function TSysInfo.GetWindowsSystemPath: string;
var
  Buf : array[0..MAX_PATH] of char;
begin
  GetSystemDirectory(Buf, MAX_PATH);
  Result := Trim(Buf);
end;

{ TNTMSNetInfo }

function TNTMSNetInfo.CloseRes(NetFileInfo: TNetFileInfo): boolean;
begin
 Result := NetFileCloseNT(nil, NetFileInfo.ID) = 0;
end;

function TNTMSNetInfo.CreateOpenResList(var AList : TNetFileInfoArray): boolean;
var
 FileInfoNT: PFileInfo3Array;
 TotalEntries,EntriesReadNT: DWORD;
 Res, i : integer;
begin
 Result := false;
 AList := nil;
 FileInfoNT := nil;
 Res := NetFileEnumNT(nil,nil,nil,3,@FileInfoNT,DWORD(-1),@entriesreadNT, @totalentries, nil);
 if (Res = 0) and (entriesreadNT > 0) then begin
  SetLength(AList, entriesreadNT);
  for i := 0 to entriesreadNT - 1 do
   with AList[i] do begin
    ID := FileInfoNT^[i].fi3_id;
    Permissions := FileInfoNT^[i].fi3_permissions;
    Num_locks   := FileInfoNT^[i].fi3_num_locks;
    PathName    := FileInfoNT^[i].fi3_pathname;
    UserName    := FileInfoNT^[i].fi3_username;
    ShareName   := '';
   end;
 end;
end;

function TNTMSNetInfo.CreateSessionsList(var AList : TNetSessionInfoArray): boolean;
var
 SessionInfo502 : PSessionInfo502Array;
 TotalEntries, EntriesReadNT  : DWORD;
 Res, i : integer;
begin
 Result := false;
 AList := nil;
 SessionInfo502 := nil;
 Res := NetSessionEnumNT(nil, nil, nil, 502, @SessionInfo502, DWORD(-1), @EntriesReadNT, @TotalEntries, nil);
 if (Res = 0) and (EntriesReadNT > 0) then begin
  SetLength(AList, EntriesReadNT);
  for i := 0 to EntriesReadNT - 1 do
   with AList[i] do begin
    SessionName := Trim(SessionInfo502^[i].sesi502_cname);
    UserName    := Trim(SessionInfo502^[i].Sesi502_username);
    NumOpens    := SessionInfo502^[i].Sesi502_num_opens;
    Time        := SessionInfo502^[i].Sesi502_time;
    IdleTime    := SessionInfo502^[i].Sesi502_idle_time;
   end;
 end;
end;

function TNTMSNetInfo.CreateShareList(var AList : TNetShareInfoArray): boolean;
var
 entriesread,totalentries:DWORD;
 ShareNT : PShareInfo2Array;
 i : integer;
 Res : boolean;
begin
 Result := false;
 AList := nil;
 //Очищаем указатель на массив структур
 ShareNT := nil;
 //Вызов функции
 res := NetShareEnumNT(nil,2,@ShareNT, DWORD(-1), @entriesread,@totalentries,nil)  = 0;
 if not Res then exit;
 //Обработка результатов
 if entriesread > 0 then begin
  SetLength(AList, entriesread);
  for i := 0 to entriesread - 1 do
   with AList[i] do begin
    NetName  := ShareNT^[i].shi2_netname;
    Remark   := ShareNT^[i].shi2_remark;
    Path     := ShareNT^[i].shi2_path;
    CurUsers := ShareNT^[i].shi2_current_uses;
   end;
 end;
end;

function TNTMSNetInfo.DelSession(NetSessionInfo: TNetSessionInfo): boolean;
var
 S1, S2 : packed array[0..128] of widechar;
 Res : integer;
begin
 Result := false;
 ZeroMemory(@S1[0], sizeof(S1));
 ZeroMemory(@S2[0], sizeof(S2));
 StringToWideChar('\\'+NetSessionInfo.SessionName, @S1[0], sizeof(S1));
 StringToWideChar(NetSessionInfo.UserName, @S2[0], sizeof(S2));
 Res := NetSessionDelNT(nil, @S1[0], @S2[0]);
 Result := Res = 0;
end;

{ T9xMSNetInfo }

function T9xMSNetInfo.CloseRes(NetFileInfo: TNetFileInfo): boolean;
begin
 Result := NetFileClose9x(nil, NetFileInfo.ID) = 0;
end;

function T9xMSNetInfo.CreateOpenResList(var AList : TNetFileInfoArray): boolean;
var
 FileInfo9x: array [0..512] of TFileInfo50;
 EntriesRead,TotalAvial: Word;
 Res, i : integer;
begin
 Result := false;
 AList := nil;
 Res := NetFileEnum9x(nil, nil,50,@FileInfo9x,SizeOf(FileInfo9x),@EntriesRead,@TotalAvial);
 if (Res = 0) and (entriesread > 0) then begin
  SetLength(AList, entriesread);
  for i := 0 to entriesread - 1 do
   with AList[i] do begin
    ID := FileInfo9x[i].fi50_id;
    Permissions := FileInfo9x[i].fi50_permissions;
    Num_locks   := FileInfo9x[i].fi50_num_locks;
    PathName    := FileInfo9x[i].fi50_pathname;
    UserName    := FileInfo9x[i].fi50_username;
    ShareName   := FileInfo9x[i].fi50_sharename;
   end;
  end;
end;

function T9xMSNetInfo.CreateSessionsList(var AList : TNetSessionInfoArray): boolean;
var
 SessionInfo50: array [0..512] of TSessionInfo50;
 EntriesRead,TotalAvial: Word;
 Res, i : integer;
begin
 Result := false;
 AList := nil;
 Res := NetSessionEnum9x(nil,50,@SessionInfo50,SizeOf(SessionInfo50),@EntriesRead,@TotalAvial);
 if (Res = 0) and (EntriesRead > 0) then begin
  SetLength(Alist, EntriesRead);
  for i := 0 to EntriesRead - 1 do
   with AList[i] do begin
    SessionName := Trim(SessionInfo50[i].sesi50_cname);
    UserName    := Trim(SessionInfo50[i].sesi50_username);
    NumOpens    := SessionInfo50[i].sesi50_num_opens;
    Time        := SessionInfo50[i].sesi50_time;
    IdleTime    := SessionInfo50[i].sesi50_idle_time;
   end;
 end;
end;

function T9xMSNetInfo.CreateShareList(var AList : TNetShareInfoArray): boolean;
var
 Share : array [0..512] of TShareInfo50;
 pcEntriesRead,pcTotalAvail:Word;
 Res, i : integer;
begin
 Result := false;
 AList  := nil;
 //Вызов функции
 res := NetShareEnum9x(nil,50,@Share,SizeOf(Share), @pcEntriesRead,@pcTotalAvail);
 if Res <> 0 then exit;
 //Обработка результатов
 if pcEntriesRead > 0 then begin
  SetLength(AList, pcEntriesRead);
  for i := 0 to pcEntriesRead - 1 do
   with AList[i] do begin
    NetName  := Share[i].shi50_netname;
    Remark   := Share[i].shi50_remark;
    Path     := Share[i].shi50_path;
    CurUsers := -1;
   end;
 end;
end;

function T9xMSNetInfo.DelSession(NetSessionInfo: TNetSessionInfo): boolean;
var
 S1, S2 : String;
 Res : integer;
begin
 Result := false;
 S1 := NetSessionInfo.SessionName;
 S2 := NetSessionInfo.UserName;
 Res := NetSessionDel9x(nil, @S2, 0);
 Result := Res = 0;
end;

{ TProcessMonitor }
function TProcessMonitor.ClearStat: boolean;
var
 i : integer;
begin
 for i := 0 to ProcessListStat.Count - 1 do
  (ProcessListStat.Objects[i] as TProcessStat).Free;
 ProcessListStat.Clear;
end;

constructor TProcessMonitor.Create;
begin
 FTimeOut := 30000;
 // Создание текущего списка
 ProcessList     := TStringList.Create;
 // Создание статистики о процессах
 ProcessListStat := TStringList.Create;
 // Создание и настройка таймера
 FTimer := TTimer.Create(nil);
 FTimer.OnTimer  := OnTimer;
 FTimer.Interval := FTimeOut;
 FTimer.Enabled  := true;
 // Создание класса "информация о процессах" в зависимости от типа ОС
 if ISNT then
  FSysProcessInfo := TPSAPIProcessInfo.Create
   else FSysProcessInfo := TToolHelpProcessInfo.Create;
end;

procedure TProcessMonitor.OnTimer(Sender: TObject);
begin
 RefreshProcessStat;
end;

function TProcessMonitor.RefreshProcessStat : boolean;
var
 Res : boolean;
 i,j : integer;
 ProcessInfo : TProcessInfo;
 ProcessStat : TProcessStat;
begin
 ProcessInfo := nil;
 ProcessStat := nil;
 // Очистка списка процессов
 for i := 0 to ProcessList.Count - 1 do
  (ProcessList.Objects[i] as TProcessInfo).Free;
 ProcessList.Clear;
 // Запрос списка процессов
 Res := FSysProcessInfo.CreateProcessList(ProcessList);
 if not(Res) then Exit;
 // 1. Добавления данных к статистике
 // 1.1 Сброс счетчика текущего кол-ва процессов в памяти
 for i := 0 to ProcessListStat.Count - 1 do
  (ProcessListStat.Objects[i] as TProcessStat).Count := 0;
 // 1.2. Добавления данных о процессах
 for i := 0 to ProcessList.Count - 1 do begin
  ProcessInfo := ProcessList.Objects[i] as TProcessInfo;
  ProcessStat := nil;
  // Цикл поиска процесса в статистике
  for j := 0 to ProcessListStat.Count - 1 do begin
   if (ProcessListStat.Objects[j] as TProcessStat).ExeFileName = ProcessInfo.ExeFileName then begin
    ProcessStat := ProcessListStat.Objects[j] as TProcessStat;
    Break;
   end;
  end;
  // Если ProcessStat != nil, то запись найдена
  if ProcessStat <> nil then begin
   // Инкремент количества копий в памяти
   inc(ProcessStat.Count);
   // Инкремент таймера
   if ProcessStat.Count = 1 then
    inc(ProcessStat.SummTime, FTimeOut);
   if not(ProcessStat.IsScaned) then
    if Assigned(FOnCheckProcess) then
     FOnCheckProcess(Self, ProcessStat, FSysProcessInfo, ProcessInfo);
  end else begin
   ProcessStat := TProcessStat.Create;
   ProcessStat.PID         := ProcessInfo.PID;
   ProcessStat.ExeFileName := LowerCase(ProcessInfo.ExeFileName);
   FSysProcessInfo.GetFileInfo(ProcessStat.ExeFileName, ProcessStat.PEFileInfo);
   ProcessStat.Count       := 1;
   ProcessStat.SummTime    := 0;
   ProcessStat.IsScaned    := false;
   if Assigned(FOnCheckProcess) then
    FOnCheckProcess(Self, ProcessStat, FSysProcessInfo, ProcessInfo);
   ProcessStat.FirstStart  := now;
   ProcessListStat.AddObject('', ProcessStat);
  end;
 end;

 ProcessInfo := nil;
 ProcessStat := nil;
end;

procedure TProcessMonitor.SetOnCheckProcess(const Value: TCheckFileEvent);
begin
  FOnCheckProcess := Value;
end;

procedure TProcessMonitor.SetOnProcessEnd(const Value: TProcessEvent);
begin
  FOnProcessEnd := Value;
end;

procedure TProcessMonitor.SetOnProcessStart(const Value: TProcessEvent);
begin
 FOnProcessStart := Value;
end;

procedure TProcessMonitor.SetTimeOut(const Value: integer);
begin
  FTimeOut := Value;
  // Чаще чем раз в 5 сек опрашивать смысла нет
  if FTimeOut < 100 then
   FTimeOut := 100;
  FTimer.Enabled  := false;
  FTimer.Interval := FTimeOut;
  FTimer.Enabled  := true;
end;



{ TCustomMSNetInfo }

{ TCustomMSNetInfo }

function TCustomMSNetInfo.OrderList(var AList: TNetShareInfoArray;
  AKey: integer): boolean;
var
 i, j : integer;
 Tmp  : TNetShareInfo;
begin
 Result := false;
 for i := 0 to Length(Alist) - 2 do
  for j := i+1 to Length(Alist) - 1 do
   if CompareItems(AList[i], AList[j], AKey) > 0 then begin
    Tmp := AList[i];
    AList[i] := AList[j];
    AList[j] := Tmp;
   end;
 Result := true;
end;

function TCustomMSNetInfo.OrderList(var AList: TNetSessionInfoArray;
  AKey: integer): boolean;
var
 i, j : integer;
 Tmp  : TNetSessionInfo;
begin
 Result := false;
 for i := 0 to Length(Alist) - 2 do
  for j := i+1 to Length(Alist) - 1 do
   if CompareItems(AList[i], AList[j], AKey) > 0 then begin
    Tmp := AList[i];
    AList[i] := AList[j];
    AList[j] := Tmp;
   end;
 Result := true;
end;

function TCustomMSNetInfo.CompareItems(I1, I2: TNetShareInfo;
  AKey: integer): integer;
begin
 Result := 0;
 case AKey of
  0 : if LowerCase(I1.NetName) > LowerCase(I2.NetName) then Result := 1
       else if LowerCase(I1.NetName) < LowerCase(I2.NetName) then Result := -1;
  1 : if LowerCase(I1.Remark) > LowerCase(I2.Remark) then Result := 1
       else if LowerCase(I1.Remark) < LowerCase(I2.Remark) then Result := -1;
  2 : if LowerCase(I1.Path) > LowerCase(I2.Path) then Result := 1
       else if LowerCase(I1.Path) < LowerCase(I2.Path) then Result := -1;
  3 : if I1.CurUsers > I2.CurUsers then Result := 1
       else if I1.CurUsers < I2.CurUsers then Result := -1;
 end;
end;

function TCustomMSNetInfo.CompareItems(I1, I2: TNetSessionInfo;
  AKey: integer): integer;
begin
 Result := 0;
 case AKey of
  0 : if I1.SessionName > I2.SessionName then Result := 1
       else if I1.SessionName < I2.SessionName then Result := -1;
  1 : if I1.UserName > I2.UserName then Result := 1
       else if I1.UserName < I2.UserName then Result := -1;
  2 : if I1.NumOpens > I2.NumOpens then Result := 1
       else if I1.NumOpens < I2.NumOpens then Result := -1;
  3 : if I1.Time > I2.Time then Result := 1
       else if I1.Time < I2.Time then Result := -1;
  4 : if I1.IdleTime > I2.IdleTime then Result := 1
       else if I1.IdleTime < I2.IdleTime then Result := -1;
 end;
end;

function TCustomMSNetInfo.CompareItems(I1, I2: TNetFileInfo;
  AKey: integer): integer;
begin
 Result := 0;
 case AKey of
  0 : if I1.ID > I2.ID then Result := 1
       else if I1.ID < I2.ID then Result := -1;
  1 : if I1.Permissions > I2.Permissions then Result := 1
       else if I1.Permissions < I2.Permissions then Result := -1;
  2 : if I1.Num_locks > I2.Num_locks then Result := 1
       else if I1.Num_locks < I2.Num_locks then Result := -1;
  3 : if I1.PathName > I2.PathName then Result := 1
       else if I1.PathName < I2.PathName then Result := -1;
  4 : if I1.UserName > I2.UserName then Result := 1
       else if I1.UserName < I2.UserName then Result := -1;
  5 : if I1.ShareName > I2.ShareName then Result := 1
       else if I1.ShareName < I2.ShareName then Result := -1;
 end;
end;

function TCustomMSNetInfo.OrderList(var AList: TNetFileInfoArray;
  AKey: integer): boolean;
var
 i, j : integer;
 Tmp  : TNetFileInfo;
begin
 Result := false;
 for i := 0 to Length(Alist) - 2 do
  for j := i+1 to Length(Alist) - 1 do
   if CompareItems(AList[i], AList[j], AKey) > 0 then begin
    Tmp := AList[i];
    AList[i] := AList[j];
    AList[j] := Tmp;
   end;
 Result := true;
end;

end.

{
C:\Documents and Settings\All Users\Главное меню\Программы\Автозагрузка
C:\Documents and Settings\Олег\Главное меню\Программы\Автозагрузка
HKCU\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\Run\
HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Policies\Explorer\Run\
HKCU\Software\Microsoft\Windows\CurrentVersion\RunOnce\
HKCU\Software\Microsoft\Windows\CurrentVersion\RunOnceEx\
C:\WINDOWS\win.ini
+ cmd.exe c:\msg.jpg
Task Scheduler

}
