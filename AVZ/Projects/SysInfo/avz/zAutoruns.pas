unit zAutoruns;
// Класс для работы с автозапуском
interface
uses Windows, Classes, SysUtils, registry, ShlObj, ComObj, ActiveX, INIFiles, zLogSystem;
const
 atRegKey       = 1;  // Ключ реестра
 atRegKeyMulty  = 11; // Ключ реестра, содержащий несколько значений
 atLnkFile      = 2;  // LNK/PIF файл
 atFile         = 3;  // Файл (например, физическое размещение программы в папаке автозапуска)
 atINIFile      = 4;  // Ключ INI файла
type

 TAutorunItemCfgParams = record
  EnableMode  : Shortint; // Режим перехода в статус "разрешен" (0-не поддерживается)
  DisableMode : Shortint; // Режим перехода в статус "запрещен" (0-не поддерживается)
  DeleteMode  : Shortint; // Режим удаления (0-не поддерживается)
  RestoreMode : Shortint; // Режим восстановления (0-не поддерживается)
  DefVal      : string;   // Параметры для восстановления
 end;

 // Описание элемента автозапуска
 TAutorunItem = record
  BinFile         : string;  // Полный путь к файлу
  CheckResult     : integer; // Результат проверки
  Enabled         : boolean; // Статус (true - это разрешенный элемент, false - заблокированный AVZ)
  AutorunType     : integer; // Код типа автозапуска
  X1, X2, X3, X4  : string;  // Параметры, которые описывают источик (их назначение зависит от кода)
  GroupCode   : integer;                // Код круппы
  CfgParams   : TAutorunItemCfgParams;  // Параметры конфигурации
 end;

 TAutorunEvent = function (AutorunItem : TAutorunItem) : boolean of object;

 // Массив элементов автозапуска
 TAutorunItems = array of TAutorunItem;

 // Менеджер автозапуска
 TAutorunManager = class
  private
   // Сканирование ключа реестра (формат параметра=исп_файл)
   function  ScanRegKey_(ARootKey: HKEY; AKeyName: String; ID : integer; AConfigScript : string; AValMode : integer = 0): boolean;
   // Сканирование параметра реестра
   function  ScanRegValue_(ARootKey: HKEY; AKeyName, AParamName : String; ID : integer; AConfigScript : string; AValMode : integer = 0): boolean;
   function  ScanRegMultyValue_(ARootKey: HKEY; AKeyName, AParamName : String; ID : integer; AConfigScript : string): boolean;
   // Сканирование ключа реестра (формат параметра=исп_файл)
   function  ScanRegKey(ARootKey: HKEY; AKeyName: String; ID : integer; AConfigScript : string; AValMode : integer = 0): boolean;
   // Сканирование параметра реестра
   function  ScanRegValue(ARootKey: HKEY; AKeyName, AParamName : String; ID : integer; AConfigScript : string; AValMode : integer = 0): boolean;
   function  ScanRegMultyValue(ARootKey: HKEY; AKeyName, AParamName : String; ID : integer; AConfigScript : string): boolean;
   // Сканирование параметра INI Файла
   function  ScanINIParam(AINIFileName, AKeyName, AParamName : String; ID : integer; AConfigScript : string): boolean;
   function  ScanDir(ADirName : string; ID : integer; AConfigScript : string)   : boolean;
   function  ScanFile(AFileName : string; ID : integer; AConfigScript : string) : boolean;
   Function  ScanAutorunRegKeys(ARootKey : HKEY; APrefix : string; ID : integer) : boolean;
   Function  ScanAutorunINIKeys(ID : integer) : boolean;
   // Сканирование системных ключей автозапуска
   Function  ScanAutorunSysRegKeys(ARootKey : HKEY; APrefix : string; ID : integer) : boolean;
   // Сканирование ключей Winlogon
   Function  ScanWinlogonSysRegKeys(ARootKey : HKEY; BaseKey : string; ID : integer) : boolean;
   // Добавление элемента
   procedure AddItem(Item : TAutorunItem; ID : integer; AConfigScript : string);
   function GetRecCount: integer;
    function ScanMPRRegKeys(ID: integer): boolean;
    function ScanWinlogonGPERegKeys(ARootKey: HKEY; BaseKey: string;
      ID: integer): boolean;
    function ScanUtilityManagerRegKeys(ARootKey: HKEY; BaseKey: string;
      ID: integer): boolean;
  public
   AutorunItems : TAutorunItems;
   constructor Create;
   destructor Destroy; override;
   // Обновление списка
   function Refresh : boolean;
   // Удаление из автозапуска
   function DeleteFromAutorun(AFileName : string) : boolean;
   // Проверка, существует ли элемент
   function AutorunItemExists(AFileName : string) : boolean;
   // Получение статуса элемента
   function GetItemEnabledStatus(AutorunItem : TAutorunItem) : integer;
   // **** Обработчики ****
   // Удаление
   function OnItemDelete(var AutorunItem : TAutorunItem) : boolean;
   // Запрет
   function OnItemDisable(var AutorunItem : TAutorunItem) : boolean;
   // Разрешение
   function OnItemEnable(var AutorunItem : TAutorunItem) : boolean;
   // Восстановление
   function OnItemRepair(var AutorunItem : TAutorunItem) : boolean;
   // Запрос кол-ва записей
   property RecCount : integer read GetRecCount;
 end;

implementation
uses zutil, zSharedFunctions, zAVZKernel;
{ TAutorunManager }

procedure TAutorunManager.AddItem(Item: TAutorunItem; ID : integer; AConfigScript : string);
var
 Lines : TStrings;
begin
 try
   Lines := TStringList.Create;
   AConfigScript := AConfigScript + '|';
   while pos('|', AConfigScript) > 0 do begin
    Lines.Add(copy(AConfigScript, 1, pos('|', AConfigScript)-1));
    Delete(AConfigScript, 1, pos('|', AConfigScript));
   end;
   Item.GroupCode := ID;
   Item.CfgParams.EnableMode  := StrToIntDef(Lines.Values['Enable'], 0);
   Item.CfgParams.DisableMode := StrToIntDef(Lines.Values['Disable'], 0);
   Item.CfgParams.DeleteMode  := StrToIntDef(Lines.Values['Delete'], 0);
   Item.CfgParams.RestoreMode := StrToIntDef(Lines.Values['Restore'], 0);
   Item.CfgParams.DefVal      := Lines.Values['DefVal'];
   Lines.Free;
   SetLength(AutorunItems, Length(AutorunItems)+1);
   AutorunItems[Length(AutorunItems)-1] := Item;
 except
  on E : Exception do
   AddToDebugLog('$AVZ0648 ['+Item.BinFile+']: '+e.Message);
 end;
end;

constructor TAutorunManager.Create;
begin
 AutorunItems := nil;
end;

function TAutorunManager.GetRecCount: integer;
begin
 Result := length(AutorunItems);
end;

function TAutorunManager.OnItemDelete(
  var AutorunItem: TAutorunItem): boolean;
var
 Reg : TRegistry;
 INI : TINIFile;
 S   : string;
begin
 Result := false;
 // Режим 1 - параметр в реестре
 if AutorunItem.CfgParams.DeleteMode = 1 then begin
  Reg := TRegistry.Create;
  Reg.RootKey := StrToRootKey(AutorunItem.X1);
  if Reg.OpenKey(AutorunItem.X2, true) then begin
   try Reg.DeleteValue(AutorunItem.X3); except exit; end;
   Reg.CloseKey;
   Result := true;
  end;
  Reg.Free;
  exit;
 end;
 // Режим 2 - файл
 if AutorunItem.CfgParams.DeleteMode = 2 then begin
  Result := AVZDeleteFile(AutorunItem.X2);
 end;
 // Режим 3 - секция INI файла
 if AutorunItem.CfgParams.DeleteMode = 3 then begin
  INI := TIniFile.Create(AutorunItem.X1);
  INI.DeleteKey(AutorunItem.X2, AutorunItem.X3);
  Result := true;
  INI.Free;
  exit;
 end;
 // Режим 11 - параметр в реестре, но значение X4 удаляется из содержимого параметра X3
 if AutorunItem.CfgParams.DeleteMode = atRegKeyMulty then begin
  Reg := TRegistry.Create;
  Reg.RootKey := StrToRootKey(AutorunItem.X1);
  if Reg.OpenKey(AutorunItem.X2, true) then begin
   try
    S := Reg.ReadString(AutorunItem.X3);
    if Pos(AutorunItem.X4, S) > 0 then
     Delete(S, Pos(AutorunItem.X4, S), Length(AutorunItem.X4)+1);
    Reg.WriteString(AutorunItem.X3, Trim(S));
   except exit; end;
   Reg.CloseKey;
   Result := true;
  end;
  Reg.Free;
  exit;
 end;
end;

function TAutorunManager.OnItemDisable(
  var AutorunItem: TAutorunItem): boolean;
var
 TekStatus : integer;
 Reg : TRegistry;
 INI : TIniFile;
 S   : string;
begin
 Result := false;
 TekStatus := GetItemEnabledStatus(AutorunItem);
 if TekStatus <> 1 then exit;
 // Режим 1 - параметр в реестре
 if AutorunItem.CfgParams.EnableMode = 1 then begin
  Reg := TRegistry.Create;
  Reg.RootKey := StrToRootKey(AutorunItem.X1);
  if Reg.OpenKey(AutorunItem.X2+'-', true) then begin
   try Reg.WriteString(AutorunItem.X3, AutorunItem.X4); except exit; end;
   Reg.CloseKey;
   if Reg.OpenKey(AutorunItem.X2, false) then begin
    try Reg.DeleteValue(AutorunItem.X3); except end;
    Reg.CloseKey;
   end;
   AutorunItem.X2 := AutorunItem.X2+'-';
   Result := true;
  end;
  Reg.Free;
  exit;
 end;
 // Режим 2 - файл
 if AutorunItem.CfgParams.EnableMode = 2 then begin
  S := NormalNameDir(ExtractFilePath(AutorunItem.X2))+'-';
  S := S + '\' + ExtractFileName(AutorunItem.X2);
  zForceDirectories(ExtractFilePath(S));
  MoveFile(PChar(AutorunItem.X2), PChar(S));
  Result := FileExists(S);
  if Result then
   AutorunItem.X2 := S;
 end;
 // Режим 4 - секция INI файла
 if AutorunItem.CfgParams.EnableMode = 3 then begin
  INI := TIniFile.Create(AutorunItem.X1);
  INI.WriteString(AutorunItem.X2+'-',
                  AutorunItem.X3, AutorunItem.X4);
  INI.DeleteKey(AutorunItem.X2, AutorunItem.X3);
  AutorunItem.X2 := AutorunItem.X2+'-';
  Result := true;
  INI.Free;
  exit;
 end;
end;

function TAutorunManager.Refresh: boolean;
var
 Path : string;
 Tmp : TAutoRunItem;
begin
 AutorunItems := nil;
 // Ключи автозапуска в реестре
 ScanAutorunRegKeys(HKEY_LOCAL_MACHINE, '', 010201);
 ScanAutorunRegKeys(HKEY_CURRENT_USER, '', 010201);
 ScanAutorunRegKeys(HKEY_USERS, '.DEFAULT\', 010201);

 ScanAutorunSysRegKeys(HKEY_LOCAL_MACHINE, '', 010202);

 ScanRegMultyValue(HKEY_LOCAL_MACHINE, 'SYSTEM\CurrentControlSet\Control\SecurityProviders', 'SecurityProviders', 050000,  'Delete=11');

 // Сканирование Winlogon
 ScanWinlogonSysRegKeys(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify', 020101);
 ScanWinlogonGPERegKeys(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\GPExtensions', 020201);

 ScanUtilityManagerRegKeys(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Accessibility\Utility Manager', 040001);

 ScanMPRRegKeys(030101);
 ScanAutorunSysRegKeys(HKEY_CURRENT_USER, '', 010202);
 ScanAutorunSysRegKeys(HKEY_USERS, '.DEFAULT\', 010202);
 Path := NormalDir(RegKeyReadString(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders', 'Common Startup'));
 if Length(Path) > 3 then begin
  ScanDir(Path, 010101,'GetStatus=2|Enable=2|Disable=2|Delete=2');
  ScanDir(NormalDir(NormalNameDir(Path)+'-'), 010101,'GetStatus=2|Enable=2|Disable=2|Delete=2');
 end;
 // SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders
 Path := NormalDir(RegKeyReadString(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders', 'Startup'));
 if Length(Path) > 3 then begin
  ScanDir(Path, 010102,'GetStatus=2|Enable=2|Disable=2|Delete=2');
  ScanDir(NormalDir(NormalNameDir(Path)+'-'), 010101,'GetStatus=2|Enable=2|Disable=2|Delete=2');
 end;
 // Сканирование папки Startup
 Path := NormalDir(RegKeyReadString(HKEY_CURRENT_USER, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders', 'Startup'));
 if Length(Path) > 3 then begin
  ScanDir(Path, 010103,'GetStatus=2|Enable=2|Disable=2|Delete=2');
  ScanDir(NormalDir(NormalNameDir(Path)+'-'), 010101,'GetStatus=2|Enable=2|Disable=2|Delete=2');
 end;
 // Сканирование папки Startup Win98
 Path := NormalDir(GetWindowsDirectoryPath + 'start menu\programs\startup');
 if Length(Path) > 3 then begin
  ScanDir(Path, 010103,'GetStatus=2|Enable=2|Disable=2|Delete=2');
  ScanDir(NormalDir(NormalNameDir(Path)+'-'), 010101,'GetStatus=2|Enable=2|Disable=2|Delete=2');
 end;
 // Сканирование папки Startup Win98
 Path := NormalDir(GetWindowsDirectoryPath + 'All Users\Start Menu\Programs\StartUp');
 if Length(Path) > 3 then begin
  ScanDir(Path, 010103,'GetStatus=2|Enable=2|Disable=2|Delete=2');
  ScanDir(NormalDir(NormalNameDir(Path)+'-'), 010101,'GetStatus=2|Enable=2|Disable=2|Delete=2');
 end;
 if FileExists(GetWindowsDirectoryPath + 'winstart.bat') then begin
  Tmp.BinFile     := GetWindowsDirectoryPath + 'winstart.bat';
  Tmp.Enabled     := true;
  Tmp.AutorunType := atFile;
  Tmp.X1          := GetWindowsDirectoryPath;
  Tmp.X2          := GetWindowsDirectoryPath + 'winstart.bat';
  AddItem(Tmp, 010103, 'GetStatus=2|Delete=2');
 end;
 // Сканирование INI файлов
 ScanAutorunINIKeys(010209);
end;

function TAutorunManager.ScanAutorunRegKeys(ARootKey: HKEY; APrefix : string; ID : integer): boolean;
var
 BaseScript1 : string;
begin
 BaseScript1 := 'GetStatus=1|Enable=1|Disable=1|Delete=1';
 // Стандартные ключи автозапуска
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\Run', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\RunOnce', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\RunOnce\Setup', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\RunOnceEx', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\RunServices', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce', ID, BaseScript1);
 // Ключи автозапуска NT
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Run', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows NT\CurrentVersion\RunOnce', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows NT\CurrentVersion\RunOnce\Setup', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows NT\CurrentVersion\RunOnceEx', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows NT\CurrentVersion\RunServices', ID, BaseScript1);
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows NT\CurrentVersion\RunServicesOnce', ID, BaseScript1);
 Result := true;
end;

function TAutorunManager.ScanAutorunSysRegKeys(ARootKey: HKEY;
  APrefix: string; ID: integer): boolean;
var
 S : string;
begin
 S := 'GetStatus=1|Enable=1|Disable=1|Delete=1';
 // Ключи Winlogon
 ScanRegMultyValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Winlogon', 'Userinit', ID, 'Restore=1|DefVal='+GetSystemDirectoryPath+'userinit.exe,');
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Winlogon', 'Taskman', ID, 'Restore=1|DefVal=TASKMAN.EXE');
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Winlogon', 'UIHost', ID, 'Restore=1|DefVal=logonui.exe');
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Winlogon', 'GinaDLL', ID, 'Restore=1|DefVal=MSGina.dll');
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Winlogon', 'Shell', ID, 'Restore=1|DefVal=Explorer.exe');
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Winlogon', 'VmApplet', ID, 'Restore=1|DefVal=rundll32 shell32,Control_RunDLL "sysdm.cpl"');

 ScanRegValue(ARootKey,   APrefix+'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon', 'AppSetup', 010202, S);
 ScanRegValue(ARootKey,   APrefix+'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon', 'TaskMan', 010202, S);
 ScanRegValue(ARootKey,   APrefix+'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon', 'System', 010202, S);

 ScanRegValue(ARootKey,   APrefix+'Control Panel\Desktop', 'scrnsave.exe', ID, S);
 ScanRegValue(ARootKey,   APrefix+'System\CurrentControlSet\Control\BootVerificationProgram','ImageName', ID, S);
 ScanRegValue(ARootKey,   APrefix+'System\CurrentControlSet\Control\Lsa\AccessProviders\Windows NT Access Provider','ProviderPath', ID, 'Restore=1|DefVal=%SystemRoot%\system32\ntmarta.dll');

 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Command Processor','AutoRun', ID, S);

 // Ключи SharedTaskScheduler
 ScanRegKey(ARootKey, APrefix+'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\SharedTaskScheduler',  010205, S, 2);
 // Ключи ShellExecuteHooks
 ScanRegKey(ARootKey, APrefix+'Software\Microsoft\Windows\CurrentVersion\Explorer\ShellExecuteHooks',  010206, S, 2);

 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows\CurrentVersion\Policies\System','Shell', ID, '');

 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Command Processor\','Autorun', ID, S);


 ScanRegKey(ARootKey,     APrefix+'Software\Microsoft\Windows\CurrentVersion\ShellServiceObjectDelayLoad', 010203, S);
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Windows','Run', ID, '');
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows NT\CurrentVersion\Windows','Load', ID, '');
 ScanRegValue(ARootKey,   APrefix+'Software\Policies\Microsoft\Windows\System','Scripts', ID, S);
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows\CurrentVersion\Policies\Explorer','Run', ID, S);
 ScanRegValue(ARootKey,   APrefix+'Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\Run','system', ID, S);
 ScanRegKey(ARootKey,     APrefix+'Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\Run',ID, S);
 ScanRegValue(ARootKey,   APrefix+'System\CurrentControlSet\Control\Session Manager','BootExecute', ID, 'Restore=1|DefVal=autocheck autochk *');
 // Terminal Server
 ScanRegKey(ARootKey,     APrefix+'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Terminal Server\Install\Software\Microsoft\Windows\CurrentVersion\Runonce', 010204, S);
 ScanRegKey(ARootKey,     APrefix+'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Terminal Server\Install\Software\Microsoft\Windows\CurrentVersion\RunonceEx', 010204, S);
 ScanRegKey(ARootKey,     APrefix+'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Terminal Server\Install\Software\Microsoft\Windows\CurrentVersion\Run', 010204, S);
 ScanRegValue(ARootKey,   APrefix+'System\CurrentControlSet\Control\Terminal Server\Wds\rdpwd','StartupPrograms', 010204, S);

 // AppInit_DLLs
 ScanRegMultyValue(ARootKey, 'SOFTWARE\Microsoft\Windows NT\CurrentVersion\Windows', 'AppInit_DLLs', 010207,  'Delete=11');

 ScanRegValue(ARootKey, 'Control Panel\IOProcs', 'MVB', ID,  'Restore=1|DefVal=mvfs32.dll');

 // Ключи для кнопок "Мой компьютер/Сервис"
 ScanRegValue(ARootKey, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\MyComputer\BackupPath', '', ID,  'Restore=1|DefVal=%SystemRoot%\system32\ntbackup.exe');
 ScanRegValue(ARootKey, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\MyComputer\cleanuppath', '', ID,  'Restore=1|DefVal=%SystemRoot%\system32\cleanmgr.exe /D %c');
 ScanRegValue(ARootKey, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\MyComputer\DefragPath', '', ID,  'Restore=1|DefVal=%systemroot%\system32\dfrg.msc %c:');
 // Отладчик
 ScanRegValue(ARootKey, 'SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug', 'Debugger', ID,  'Restore=1|DefVal=drwtsn32 -p %ld -e %ld -g');

 // Windows Scripting Host
 ScanRegValue(ARootKey, 'SOFTWARE\Microsoft\Windows Scripting Host\Locations', 'CScript', 010208,  'Restore=1|DefVal=%SystemRoot%\System32\cscript.exe');
 ScanRegValue(ARootKey, 'SOFTWARE\Microsoft\Windows Scripting Host\Locations', 'WScript', 010208,  'Restore=1|DefVal=%SystemRoot%\System32\wscript.exe');
end;

function TAutorunManager.ScanDir(ADirName: string; ID : integer; AConfigScript : string): boolean;
var
 SR  : TSearchRec;
 Res : integer;
 Tmp : TAutoRunItem;
begin
 ADirName := NormalDir(ADirName);
 try
   Res := FindFirst(ADirName+'*.*', faAnyFile, SR);
   while Res = 0 do begin
    if (SR.Attr and faDirectory) = 0 then begin
     if not(ScanFile(ADirName + SR.Name, ID, AConfigScript)) then begin
      Tmp.BinFile     := ADirName + SR.Name;
      Tmp.Enabled     := true;
      Tmp.AutorunType := atFile;
      Tmp.X1          := ADirName;
      Tmp.X2          := ADirName + SR.Name;
      if LowerCase(SR.Name) <> 'desktop.ini' then
       AddItem(Tmp, ID, AConfigScript);
     end;
    end;
    Res := FindNext(SR);
   end;
 except
  on E : Exception do
   AddToDebugLog('$AVZ0651+'+ADirName+': '+e.Message);
 end;
end;


function TAutorunManager.ScanFile(AFileName: string; ID : integer; AConfigScript : string): boolean;
var
 IObject : IUnknown;
 SLink   : IShellLink;
 PFile   : IPersistFile;
 pfd     : TWin32FindData;
 S       : string;
 Tmp     : TAutoRunItem;
begin
 Result := false;
 try
  if Not(FileExists(AFileName)) then exit;
  try
   IObject := CreateComObject(CLSID_ShellLink);
   if (IObject = nil) then exit;
   SLink   := IObject as IShellLink;
   PFile   := IObject as IPersistFile;
   if (IObject = nil) or (SLink = nil) or (PFile = nil) then exit;
   if PFile.Load(PWChar(WideString(AFileName)), STGM_READ) <> S_OK then exit;
   SetLength(S, 255);
   ZeroMemory(@S[1], length(S));
   SLink.GetPath(@S[1], 255, pfd, SLGP_UNCPRIORITY);
   S := Trim(S);
   if S = '' then exit;
   Tmp.BinFile     := S;
   Tmp.Enabled     := true;
   Tmp.AutorunType := atLnkFile;
   Tmp.X1          := ExtractFilePath(AFileName);
   Tmp.X2          := AFileName;
   AddItem(Tmp, ID, AConfigScript);
   Result := true;
  except
   on E : Exception do begin
    Result := false;
    AddToDebugLog('$AVZ0647 ['+AFileName+']: '+e.Message);
   end;
  end;
 finally
  SLink   := nil;
  PFile   := nil;
  IObject := nil;
 end;
end;

function TAutorunManager.ScanRegKey_(ARootKey: HKEY; AKeyName: String; ID : integer; AConfigScript : string; AValMode : integer = 0): boolean;
var
 Tmp      : TAutoRunItem;
 Reg      : TRegistry;
 ValNames : TStrings;
 i        : integer;
begin
 Result := false;
 try
   // Создание экземпляров классов и очиска списка результатов
   Reg := TRegistry.Create;
   ValNames := TStringList.Create;
   // Анализ заданного раздела
   Reg.RootKey := ARootKey;
   // Открытие указанного ключа
   if Reg.OpenKey(AKeyName, false) then begin
    try Reg.GetValueNames(ValNames); except end;
    for i := 0 to ValNames.Count - 1 do
     if Reg.GetDataType(ValNames[i]) in [rdString, rdExpandString] then begin
      Tmp.AutorunType := atRegKey;
      Tmp.X1      := RootKeyToStr(ARootKey);
      Tmp.X2      := Reg.CurrentPath;
      Tmp.X3      := ValNames[i];
      Tmp.X4      := '';
      try Tmp.X4      := Reg.ReadString(ValNames[i]); except end;
      Tmp.BinFile := '';
      // Добавление сообразно режиму (0-значение=exe, 1-значение=CLSID, 2- имя_параметра=CLSID, 3-имя_параметра=EXE)
      case AValMode of
       0 : begin
            Tmp.BinFile := Trim(Tmp.X4);
           end;
       1 : begin
            Tmp.BinFile := GetBinFileByGUID(Tmp.X4);
           end;
       2 : begin
            Tmp.BinFile := GetBinFileByGUID(Tmp.X3);
           end;
       4 : begin
            Tmp.BinFile := Trim(Tmp.X3);
           end;
      end;
      if (Tmp.BinFile <> '') and (pos('{', Tmp.BinFile) > 0) and (pos('}', Tmp.BinFile) > 0) then
       Tmp.BinFile := GetBinFileByGUID(Tmp.BinFile);
      Tmp.BinFile := NormalProgramFileName(Tmp.BinFile, '.exe');
      if (Tmp.BinFile <> '') then
       AddItem(Tmp, ID, AConfigScript);
     end;
     Reg.CloseKey;
   end;
   Reg.Free;
   ValNames.Free;
   Result := true;
  except
   on E : Exception do
    AddToDebugLog('$AVZ0650+'+AKeyName+': '+e.Message);
  end;
end;

function TAutorunManager.ScanRegValue_(ARootKey: HKEY; AKeyName,
  AParamName: String; ID : integer; AConfigScript : string; AValMode : integer = 0): boolean;
var
 Tmp      : TAutoRunItem;
 Reg      : TRegistry;
 i        : integer;
begin
 Result := false;
 try
   // Создание экземпляров классов и очиска списка результатов
   Reg := TRegistry.Create;
   // Анализ заданного раздела
   Reg.RootKey := ARootKey;
   // Открытие указанного ключа в режиме "только чтение"
   Reg.Access := KEY_READ;
   if Reg.OpenKey(AKeyName, false) then begin
    if Reg.ValueExists(AParamName) then begin
      Tmp.AutorunType := atRegKey;
      Tmp.X1      := RootKeyToStr(ARootKey);
      Tmp.X2      := Reg.CurrentPath;
      Tmp.X3      := AParamName;
      Tmp.X4      := '';
      Tmp.BinFile := '';
      try
       case Reg.GetDataType(AParamName) of
        rdString, rdExpandString :   Tmp.X4      := Reg.ReadString(AParamName);
        rdBinary, rdUnknown :  begin
              SetLength(Tmp.X4, 2048);
              i := Reg.ReadBinaryData(AParamName, Tmp.X4[1], length(Tmp.X4));
              SetLength(Tmp.X4, i);
             end;
       end;
      except end;
      // Добавление сообразно режиму (0-значение=exe, 1-значение=CLSID, 2- имя_параметра=CLSID, 3-имя_параметра=EXE)
      case AValMode of
       0 : begin
            Tmp.BinFile := Trim(Tmp.X4);
           end;
       1 : begin
            Tmp.BinFile := GetBinFileByGUID(Tmp.X4);
           end;
       2 : begin
            Tmp.BinFile := GetBinFileByGUID(Tmp.X3);
           end;
       4 : begin
            Tmp.BinFile := Trim(Tmp.X3);
           end;
      end;
      Tmp.BinFile := NTFileNameToNormalName(Tmp.BinFile);
      if Tmp.BinFile  <> '' then
       AddItem(Tmp, ID, AConfigScript);
    end;
    Reg.CloseKey;
   end;
   Reg.Free;
 except
  on E : Exception do
   AddToDebugLog('$AVZ0652+'+AKeyName+' '+AParamName+': '+e.Message);
 end;
 Result := true;
end;

function TAutorunManager.OnItemEnable(var AutorunItem: TAutorunItem): boolean;
var
 TekStatus : integer;
 Reg : TRegistry;
 INI : TIniFile;
 S   : string;
begin
 Result := false;
 TekStatus := GetItemEnabledStatus(AutorunItem);
 if TekStatus <> 0 then exit;
 // Режим 1 - параметр ключа реестра
 if AutorunItem.CfgParams.EnableMode =  1 then begin
  Reg := TRegistry.Create;
  Reg.RootKey := StrToRootKey(AutorunItem.X1);
  if Reg.OpenKey(copy(AutorunItem.X2, 1, length(AutorunItem.X2)-1), true) then begin
   try Reg.WriteString(AutorunItem.X3, AutorunItem.X4); except exit; end;
   Reg.CloseKey;
   if Reg.OpenKey(AutorunItem.X2, false) then begin
    try Reg.DeleteValue(AutorunItem.X3); except end;
    Reg.CloseKey;
   end;
   AutorunItem.X2 := copy(AutorunItem.X2, 1, length(AutorunItem.X2)-1);
   Result := true;
  end;
  Reg.Free;
  exit;
 end;
 // Режим 2 - файл
 if AutorunItem.CfgParams.EnableMode = 2 then begin
  S := NormalNameDir(ExtractFilePath(AutorunItem.X2));
  S := copy(S, 1, length(S)-1);
  S := S + '\' + ExtractFileName(AutorunItem.X2);
  zForceDirectories(ExtractFilePath(S));
  MoveFile(PChar(AutorunItem.X2), PChar(S));
  Result := FileExists(S);
  if Result then
   AutorunItem.X2 := S;
 end;
 // Режим 3 - секция INI файла
 if AutorunItem.CfgParams.EnableMode = 3 then begin
  INI := TIniFile.Create(AutorunItem.X1);
  INI.WriteString(copy(AutorunItem.X2, 1, length(AutorunItem.X2)-1),
                  AutorunItem.X3, AutorunItem.X4);
  INI.DeleteKey(AutorunItem.X2, AutorunItem.X3);
  AutorunItem.X2 := copy(AutorunItem.X2, 1, length(AutorunItem.X2)-1);
  Result := true;
  INI.Free;
  exit;
 end;
end;

function TAutorunManager.GetItemEnabledStatus(
  AutorunItem: TAutorunItem): integer;
var
 S : string;
begin
 Result := -1;
 case AutorunItem.CfgParams.EnableMode of
  // Ключ реестра
  1 : if copy(AutorunItem.X2, length(AutorunItem.X2), 1) = '-' then
       Result := 0 else Result := 1;
  // Ссылка на файл или файл
  2 : begin
      S :=  NormalNameDir(ExtractFilePath(AutorunItem.X2));
      if copy(S, length(S), 1) = '-' then
       Result := 0 else Result := 1;
      end;
  // INI файл
  3 : if copy(AutorunItem.X2, length(AutorunItem.X2), 1) = '-' then
       Result := 0 else Result := 1;
 end;
end;

function TAutorunManager.OnItemRepair(
  var AutorunItem: TAutorunItem): boolean;
var
 Reg : TRegistry;
 INI : TINIFile;
begin
 Result := false;
 Reg := TRegistry.Create;
 case AutorunItem.CfgParams.RestoreMode of
  1 : begin
       Reg.RootKey := StrToRootKey(AutorunItem.X1);
       if Reg.OpenKey(AutorunItem.X2, true) then begin
        try Reg.WriteString(AutorunItem.X3, AutorunItem.CfgParams.DefVal); except exit; end;
        Reg.CloseKey;
        Result := true;
      end;
     end;
   3 :  begin
       INI := TIniFile.Create(AutorunItem.X1);
       INI.WriteString(AutorunItem.X2,
                  AutorunItem.X3, AutorunItem.CfgParams.DefVal);
       AutorunItem.X4 := AutorunItem.CfgParams.DefVal;
       Result := true;
       INI.Free;
      end;
 end;
 Reg.Free;
end;

function TAutorunManager.ScanWinlogonSysRegKeys(ARootKey: HKEY;
  BaseKey: string; ID: integer): boolean;
var
 Tmp      : TAutoRunItem;
 Reg  : TRegistry;
 List : TStrings;
 i    : integer;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  for i := 0 to List.Count - 1 do begin
   if Reg.OpenKey(BaseKey + '\' + List[i], false) then begin
    if Reg.ValueExists('DLLName') then begin
     Tmp.BinFile := Reg.ReadString('DLLName');
     Tmp.CheckResult := -1;
     Tmp.Enabled := false;
     Tmp.AutorunType := atRegKey;
     Tmp.X1      := RootKeyToStr(ARootKey);
     Tmp.X2      := Reg.CurrentPath;
     Tmp.X3      := 'DLLName';
     Tmp.X4      := Tmp.BinFile;
     if Tmp.BinFile  <> '' then
      AddItem(Tmp, ID,  'GetStatus=1|Enable=1|Disable=1|Delete=1');
    end;
    Reg.CloseKey;
   end;
  end;
 end;
 Reg.Free;
 List.Free;
end;

function TAutorunManager.ScanWinlogonGPERegKeys(ARootKey: HKEY;
  BaseKey: string; ID: integer): boolean;
var
 Tmp      : TAutoRunItem;
 Reg  : TRegistry;
 List : TStrings;
 i    : integer;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  for i := 0 to List.Count - 1 do begin
   if Reg.OpenKey(BaseKey + '\' + List[i], false) then begin
    if Reg.ValueExists('DLLName') then begin
     Tmp.BinFile :=  NormalProgramFileName(Reg.ReadString('DLLName'), '.dll');
     Tmp.CheckResult := -1;
     Tmp.Enabled := false;
     Tmp.AutorunType := atRegKey;
     Tmp.X1      := RootKeyToStr(ARootKey);
     Tmp.X2      := Reg.CurrentPath;
     Tmp.X3      := 'DLLName';
     Tmp.X4      := Tmp.BinFile;
     if Tmp.BinFile  <> '' then
      AddItem(Tmp, ID,  'GetStatus=1|Enable=1|Disable=1|Delete=1');
    end;
    Reg.CloseKey;
   end;
  end;
 end;
 Reg.Free;
 List.Free;
end;

function TAutorunManager.ScanUtilityManagerRegKeys(ARootKey: HKEY;
  BaseKey: string; ID: integer): boolean;
var
 Tmp      : TAutoRunItem;
 Reg  : TRegistry;
 List : TStrings;
 i    : integer;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  for i := 0 to List.Count - 1 do begin
   if Reg.OpenKey(BaseKey + '\' + List[i], false) then begin
    if Reg.ValueExists('Application path') then begin
     Tmp.BinFile := Reg.ReadString('Application path');
     Tmp.CheckResult := -1;
     Tmp.Enabled := false;
     Tmp.AutorunType := atRegKey;
     Tmp.X1      := RootKeyToStr(ARootKey);
     Tmp.X2      := Reg.CurrentPath;
     Tmp.X3      := 'Application path';
     Tmp.X4      := Tmp.BinFile;
     if Tmp.BinFile  <> '' then
      AddItem(Tmp, ID,  'GetStatus=1|Enable=1|Disable=1|Delete=1');
    end;
    Reg.CloseKey;
   end;
  end;
 end;
 Reg.Free;
 List.Free;
end;

function TAutorunManager.ScanMPRRegKeys(ID: integer): boolean;
var
 Tmp      : TAutoRunItem;
 Reg  : TRegistry;
 List : TStrings;
 i    : integer;
 BaseKey : string;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 BaseKey := 'System\CurrentControlSet\Control\MPRServices';
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  for i := 0 to List.Count - 1 do begin
   if Reg.OpenKey(BaseKey + '\' + List[i], false) then begin
    if Reg.ValueExists('DLLName') then begin
     Tmp.BinFile := Reg.ReadString('DLLName');
     Tmp.CheckResult := -1;
     Tmp.Enabled := false;
     Tmp.AutorunType := atRegKey;
     Tmp.X1      := RootKeyToStr(HKEY_LOCAL_MACHINE);
     Tmp.X2      := Reg.CurrentPath;
     Tmp.X3      := 'DLLName';
     Tmp.X4      := Tmp.BinFile;
     if Tmp.BinFile  <> '' then
      AddItem(Tmp, ID,  'GetStatus=1|Enable=1|Disable=1|Delete=1');
    end;
    Reg.CloseKey;
   end;
  end;
 end;
 Reg.Free;
 List.Free;
end;

function TAutorunManager.ScanINIParam(AINIFileName, AKeyName,
  AParamName: String; ID: integer; AConfigScript: string): boolean;
var
 INI : TINIFile;
 Tmp      : TAutoRunItem;
begin
 Result := false;
 try
  INI := TIniFile.Create(AINIFileName);
  try
   if not(INI.SectionExists(AKeyName)) then exit;
   if not(INI.ValueExists(AKeyName, AParamName)) then exit;
   Tmp.AutorunType := atINIFile;
   Tmp.X1      := AINIFileName;
   Tmp.X2      := AKeyName;
   Tmp.X3      := AParamName;
   Tmp.X4      := INI.ReadString(AKeyName, AParamName, '');
   Tmp.BinFile := Trim(Tmp.X4);
   if (Tmp.BinFile <> '') then
    AddItem(Tmp, ID, AConfigScript);
  finally
   INI.Free;
  end;
 except
  on e:Exception do begin
   AddToDebugLog('$AVZ0649+'+AINIFileName+': '+e.Message);
  end;
 end;
end;

function TAutorunManager.ScanAutorunINIKeys(ID: integer): boolean;
var
 BaseScript1 : string;
begin
 BaseScript1 := 'GetStatus=3|Enable=3|Disable=3|Delete=3';
 // Сканирование INI файлов
 ScanINIParam(GetWindowsDirectoryPath+'win.ini',    'windows', 'load', ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'win.ini',    'windows', 'run',  ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'windows', 'load', ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'windows', 'run',  ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'boot', 'shell',  ID, 'Restore=1|DefVal=Explorer.exe');
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'boot', 'SCRNSAVE.EXE',  ID, BaseScript1);
 // Сканирование INI файлов
 ScanINIParam(GetWindowsDirectoryPath+'win.ini',    'windows-', 'load', ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'win.ini',    'windows-', 'run',  ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'windows-', 'load', ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'windows-', 'run',  ID, BaseScript1);
 ScanINIParam(GetWindowsDirectoryPath+'system.ini', 'boot-', 'shell',  ID, 'Restore=1|DefVal=Explorer.exe');
end;

function TAutorunManager.DeleteFromAutorun(AFileName: string): boolean;
var
 i : integer;
 S : string;
begin
 Result := false;
 AFileName := AnsiLowerCase(AFileName);
 for i := 0 to Length(AutorunItems) - 1 do begin
  S := Trim(AnsiLowerCase(AutorunItems[i].BinFile));
  if pos(AFileName, S) > 0 then begin
   OnItemDelete(AutorunItems[i]);
   OnItemRepair(AutorunItems[i]);
   AddToLog('$AVZ0422 '+AutorunItems[i].X1+','+AutorunItems[i].X2+','+AutorunItems[i].X3+','+AutorunItems[i].X4, logInfo)
  end;
 end;
end;

function TAutorunManager.AutorunItemExists(AFileName: string): boolean;
var
 i : integer;
 S : string;
begin
 Result := false;
 AFileName := AnsiLowerCase(AFileName);
 for i := 0 to Length(AutorunItems) - 1 do begin
  S := Trim(AnsiLowerCase(AutorunItems[i].BinFile));
  if pos(AFileName, S) > 0 then begin
   Result := true;
   exit;
  end;
 end;
end;

destructor TAutorunManager.Destroy;
begin
 AutorunItems := nil;
 inherited;
end;

function TAutorunManager.ScanRegKey(ARootKey: HKEY; AKeyName: String;
  ID: integer; AConfigScript: string; AValMode: integer): boolean;
begin
 Result := ScanRegKey_(ARootKey, AKeyName, ID, AConfigScript, AValMode);
 Result := ScanRegKey_(ARootKey, AKeyName+'-', ID, AConfigScript, AValMode);
end;

function TAutorunManager.ScanRegValue(ARootKey: HKEY; AKeyName,
  AParamName: String; ID: integer; AConfigScript: string;
  AValMode: integer): boolean;
begin
 Result := ScanRegValue_(ARootKey, AKeyName, AParamName, ID, AConfigScript, AValMode);
 Result := ScanRegValue_(ARootKey, AKeyName+'-', AParamName, ID, AConfigScript, AValMode);
end;

function TAutorunManager.ScanRegMultyValue_(ARootKey: HKEY; AKeyName,
  AParamName: String; ID: integer; AConfigScript: string): boolean;
var
 Tmp      : TAutoRunItem;
 Reg      : TRegistry;
 S        : string;
 i        : integer;
begin
 Result := false;
 try
   // Создание экземпляров классов и очиска списка результатов
   Reg := TRegistry.Create;
   // Анализ заданного раздела
   Reg.RootKey := ARootKey;
   // Открытие указанного ключа в режиме "только чтение"
   Reg.Access := KEY_READ;
   if Reg.OpenKey(AKeyName, false) then begin
    if Reg.ValueExists(AParamName) then begin
      Tmp.AutorunType := atRegKeyMulty;
      Tmp.X1      := RootKeyToStr(ARootKey);
      Tmp.X2      := Reg.CurrentPath;
      Tmp.X3      := AParamName;
      Tmp.X4      := '';
      Tmp.BinFile := '';
      try
       case Reg.GetDataType(AParamName) of
        rdString, rdExpandString :   Tmp.X4      := Reg.ReadString(AParamName);
        rdBinary, rdUnknown :  begin
              SetLength(Tmp.X4, 2048);
              i := Reg.ReadBinaryData(AParamName, Tmp.X4[1], length(Tmp.X4));
              SetLength(Tmp.X4, i);
             end;
       end;
      except end;
      // Добавление сообразно режиму (0-значение=exe, 1-значение=CLSID, 2- имя_параметра=CLSID, 3-имя_параметра=EXE)
      S := Trim(Tmp.X4)+' ';
      i := 1;
      while i <= length(S) do begin
       if S[i] in [' ', ','] then begin
        Tmp.X4 := Trim(copy(S, 1, i-1));
        Tmp.BinFile := NormalProgramFileName(Tmp.X4, '.exe');
        if Tmp.BinFile  <> '' then
         AddItem(Tmp, ID, AConfigScript);
        delete(S, 1, i);
        i := 1;
       end else inc(i);
      end;
    end;
    Reg.CloseKey;
   end;
   Reg.Free;
 except
  on E : Exception do
   AddToDebugLog('$AVZ0652+'+AKeyName+' '+AParamName+': '+e.Message);
 end;
 Result := true;
end;

function TAutorunManager.ScanRegMultyValue(ARootKey: HKEY; AKeyName,
  AParamName: String; ID: integer; AConfigScript: string): boolean;
begin
 Result := ScanRegMultyValue_(ARootKey, AKeyName, AParamName, ID, AConfigScript);
 Result := ScanRegMultyValue_(ARootKey, AKeyName+'-', AParamName, ID, AConfigScript);
end;

end.
