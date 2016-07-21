unit zScripting;

interface
uses Windows, Messages, SysUtils, Classes,
     fs_iinterpreter,
     fs_ipascal,
     fs_iclassesrtti,
     zLSP, zSharedFunctions,
     Registry, INIFiles,
     ZLogSystem;
type
 TAvzScript = class
  private
    FLogEnabled: boolean;
    procedure SetLogEnabled(const Value: boolean);
  protected
   fsScriptRuntime : TfsScript;
   fsPascal        : TfsPascal;
   fsClassesRTTI   : TfsClassesRTTI;
   Reg : TRegistry;
  public
   // Настройка RootKey пои имени
   function SetRootKeyByName(AName : string) : boolean;
   // Обработчик "моих" команд
   function OnScriptCallMetod(Instance: TObject; ClassType: TClass; const MethodName: String;
                             var Params: Variant): Variant;
   function OnScriptCallMetodEvGroup(Instance: TObject; ClassType: TClass; const MethodName: String;
                             var Params: Variant): Variant;
   // ***** Интерпретация команд ******
   // Удаление CLSID
   procedure DelCLSID(ACLSID : string);
   // Удаление Interface
   procedure DelInterface(ACLSID : string);
   // Удаление BHO
   procedure DelBHO(ACLSID : string);
   // Удаление элемента ActiveSetup
   procedure DelActiveSetup(ACLSID : string);
   // Удаление Shell Extensions
   procedure DelShellExt(ACLSID : string);
   // Удаление SharedTaskScheduler
   procedure DelSharedTaskSch(ACLSID : string);
   // Проверка, существует ли такой BHO
   function  BHOExists(ACLSID : string) : boolean;
   // Проверка, существует ли такой CLSID
   function  CLSIDExists(ACLSID : string) : boolean;
   // Поиск имени файла по CLSID
   function  CLSIDFileName(ACLSID : string; ARecurLevel : integer = 1) : string;
   // Удаление ключа реестра
   function  RegKeyDelete(ARoot, AKey : string) : boolean;
   // Создание ключа реестра
   function  RegKeyCreate(ARoot, AKey : string) : boolean;
   // Проверка существования ключа реестра
   function  RegKeyExists(ARoot, AKey : string) : boolean;
   // Проверка существования параметра реестра
   function  RegKeyParamExists(ARoot, AKey, AParam : string) : boolean;
   // Удаление параметра реестра
   function  RegKeyParamDelete(ARoot, AKey, AParam : string) : boolean;
   // Присвоение параметра реестра
   function  RegKeyStrParamSet(ARoot, AKey, AParam, AValue : string) : boolean;
   // Чтение параметра реестра
   function  RegKeyStrParamGet(ARoot, AKey, AParam : string) : string;
   function  RegKeyIntParamGet(ARoot, AKey, AParam : string) : integer;
   // "Эвристическая проверка" CLSID (проверка наличия CLSID + наличия файла + отсутствия файла в безопасных + отсуствия в базе вирусов)
   function  EvCheckCLSID(ACLSID, AVirusName, AFileMask : string) : boolean;
   // "Эвристическая проверка" файла (проверка наличия файла + отсутствия файла в безопасных + отсуствия в базе вирусов)
   function  EvCheckFileName(AFileName, AVirusName, AFileMask : string) : boolean;
   // Удаление записи WinlogonNotify по имени исполняемого файла
   procedure DelWinlogonNotifyByFileName(AFileName : string);
   // Удаление записи WinlogonNotify по имени ключа
   procedure DelWinlogonNotifyByKeyName(AKeyName : string);
  public
   ReadOnlyMode  : boolean;
   QurantinePath : string;
   constructor Create;
   procedure AddToLog(s : string);
   function  ExecuteScript(AFileName, AScript : string; AScriptName : string='') : boolean;
   destructor Destroy; override;
   property LogEnabled : boolean read FLogEnabled write SetLogEnabled;
 end;
implementation
uses Unit1, zutil, zAVZKernel;
{$I fs.inc}
{ TAvzScript }

procedure TAvzScript.AddToLog(s: string);
begin
 if FLogEnabled then
  ZLogSystem.AddToLog(s, logInfo);
end;

function TAvzScript.BHOExists(ACLSID: string): boolean;
var
 S : string;
begin
 Result := false;
 if Trim(ACLSID) = '' then exit;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 S := '{'+Trim(ACLSID)+'}';
 if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\'+S) then begin
  Result := true;
  exit;
 end;
 if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Extensions'+S) then begin
  Result := true;
  exit;
 end;
 if Reg.OpenKey('SOFTWARE\Microsoft\Internet Explorer\ToolBar', false) then begin
  if Reg.ValueExists(S) then
   Result := true;
  Reg.CloseKey;
 end;
 Reg.RootKey := HKEY_CURRENT_USER;
 S := '{'+Trim(ACLSID)+'}';
 if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\'+S) then begin
  Result := true;
  exit;
 end;
 if Reg.KeyExists('SOFTWARE\Microsoft\Internet Explorer\Extensions'+S) then begin
  Result := true;
  exit;
 end;
 if Reg.OpenKey('SOFTWARE\Microsoft\Internet Explorer\ToolBar', false) then begin
  if Reg.ValueExists(S) then
   Result := true;
  Reg.CloseKey;
 end;
end;


constructor TAvzScript.Create;
begin
 fsScriptRuntime := TfsScript.Create(nil);
 fsScriptRuntime.Parent := fsGlobalUnit;
 fsPascal        := TfsPascal.Create(nil);
 fsClassesRTTI   := TfsClassesRTTI.Create(nil);
 Reg := TRegistry.Create;
 ReadOnlyMode := false;
 FLogEnabled := true;
end;

procedure TAvzScript.DelBHO(ACLSID: string);
var
 S : string;
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelCLSID заблокирована');
  exit;
 end;
 if Trim(ACLSID) = '' then exit;
 S := '{'+Trim(ACLSID)+'}';
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.DeleteKey('SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\'+S) then
  AddToLog('[микропрограмма лечения]> Удалена панель IE'+S);
 if Reg.DeleteKey('SOFTWARE\Microsoft\Internet Explorer\Extensions'+S) then
  AddToLog('[микропрограмма лечения]> Удалено расширение IE'+S);
 if Reg.DeleteKey('SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects\'+S) then
  AddToLog('[микропрограмма лечения]> Удалено расширение IE'+S);
 if Reg.OpenKey('SOFTWARE\Microsoft\Internet Explorer\ToolBar', false) then begin
  if Reg.DeleteValue(S) then
   AddToLog('[микропрограмма лечения]> Удалена панель IE'+S);
  Reg.CloseKey;
 end;
 Reg.RootKey := HKEY_CURRENT_USER;
 if Reg.DeleteKey('SOFTWARE\Microsoft\Internet Explorer\Explorer Bars\'+S) then
  AddToLog('[микропрограмма лечения]> Удалена панель IE'+S);
 if Reg.DeleteKey('SOFTWARE\Microsoft\Internet Explorer\Extensions'+S) then
  AddToLog('[микропрограмма лечения]> Удалено расширение IE'+S);
 if Reg.OpenKey('SOFTWARE\Microsoft\Internet Explorer\ToolBar', false) then begin
  if Reg.DeleteValue(S) then
   AddToLog('[микропрограмма лечения]> Удалена панель IE'+S);
  Reg.CloseKey;
 end;
end;

procedure TAvzScript.DelCLSID(ACLSID: string);
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelCLSID заблокирована');
  exit;
 end;
 if Trim(ACLSID) = '' then exit;
 Reg.RootKey := HKEY_CLASSES_ROOT;
 // Удаление ключа реестра
 if Reg.DeleteKey('CLSID\{'+Trim(ACLSID)+'}') then
  AddToLog('[микропрограмма лечения]> Удален CLSID '+ACLSID);
 // Удаление ссылки в Shell Extensions\Approved
 RegKeyParamDelete('HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved', '{'+Trim(ACLSID)+'}');
 // Удаление интерфейса
 DelInterface(ACLSID);
 // Удаление BHO
 DelBHO(ACLSID);
 // Удаление ActiveSetup
 DelActiveSetup(ACLSID);
 // Удаление Shell Extention
 DelShellExt(ACLSID);
 // Удаление SharedTaskScheduler
 DelSharedTaskSch(ACLSID);
end;

destructor TAvzScript.Destroy;
begin
 fsScriptRuntime.Free;
 fsScriptRuntime := nil;
 Reg.Free;
 Reg := nil;
 fsPascal.Free;
 fsPascal := nil;
 inherited;
end;

function TAvzScript.ExecuteScript(AFileName, AScript: string; AScriptName : string=''): boolean;
begin
 Result := false;
 QurantinePath := NormalDir(QurantinePath);
 // Выполнение скрипта лечения
 if Trim(AScript) = '' then exit;
 // Подготовка текста скрипта
 fsScriptRuntime.Clear;
 fsScriptRuntime.Lines.Text := AScript;

 fsScriptRuntime.AddConst('AFileName', 'string', AFileName);
 fsScriptRuntime.AddConst('AScriptName', 'string', AScriptName);
 fsScriptRuntime.AddMethod('function AddEvAlarmByCLSID(AName, AVirus, AFileList : string) : boolean', OnScriptCallMetodEvGroup);
 fsScriptRuntime.AddMethod('function AddEvAlarmByName(AName, AVirus, AFileList : string) : boolean', OnScriptCallMetodEvGroup);
 fsScriptRuntime.AddMethod('procedure AddToLog(S : String)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure DelSPIByFileName(ALibrary : string; NameCompare : boolean)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure AutoFixSPI', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure KillProcess(APID : integer)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure KillProcessByName(AName : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure DelCLSID(CLSID : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function  CLSIDExists(CLSID : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function  CLSIDFileExists(CLSID : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function  CLSIDAndFileExists(CLSID : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function  CLSIDFileName(CLSID : string) : string', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure DelUninstall(AName : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function  BHOExists(BHO : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure DelBHO(BHO : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure RegKeyDel(ARoot, AName : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure RegKeyCreate(ARoot, AName : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure RegKeyParamDel(ARoot, AName, AParam : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure RegKeyStrParamWrite(ARoot, AName, AParam, AValue : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function RegKeyStrParamRead(ARoot, AName, AParam : string) : string', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function RegKeyIntParamRead(ARoot, AName, AParam : string) : integer', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function RegKeyExists(ARoot, AName : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function RegKeyParamExists(ARoot, AName, AParam : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function FileExists(AName : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function DirectoryExists(AName : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function DeleteFile(AName : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function ExtractFileName(AName : string) : string', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function ExtractFilePath(AName : string) : string', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function ExtractFileExt(AName : string) : string', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function Trim(S : string) : string', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function DeleteVirus(AName, AVirus : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function AddToQurantine(AName, AVirus : string) : boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function AntiRootKit(Deactivate : boolean): boolean', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('function StringReplace(S, OldPattern, NewPattern: string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure DelWinlogonNotifyByFileName(AFileName : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure DelWinlogonNotifyByKeyName(AKeyName : string)', OnScriptCallMetod);
 fsScriptRuntime.AddMethod('procedure Sleep(AInterval : integer)', OnScriptCallMetod);
 fsScriptRuntime.AddClass(TRegistry, 'TObject');
 fsScriptRuntime.AddClass(TIniFile, 'TObject');

 // Компиляция и выполнение
 if fsScriptRuntime.Compile then begin
  fsScriptRuntime.Execute;
  Result := true;
 end else AddToLog('Ошибка микропрограммы: '+fsScriptRuntime.ErrorMsg);
end;

function TAvzScript.SetRootKeyByName(AName: string): boolean;
begin
 Result := false;                                    
 if StrToRootKey(AName) > 0 then begin
  Reg.RootKey := StrToRootKey(AName);
  Result := true;
 end;
end;

function TAvzScript.OnScriptCallMetod(Instance: TObject; ClassType: TClass;
  const MethodName: String; var Params: Variant): Variant;
var
 LSPManager : TLSPManager;
begin
 // Добавление в протокол
 if MethodName = 'ADDTOLOG' then begin
  AddToLog(Params[0]);
  exit;
 end;
 // Удаление LSP
 if MethodName = 'DELSPIBYFILENAME' then begin
  LSPManager := TLSPManager.Create;
  LSPManager.Refresh;
  LSPManager.DeleteNameSpaceByBinaryName(Params[0], Params[1]);
  LSPManager.DeleteProtocolByBinaryName(Params[0], Params[1]);
  LSPManager.DeleteNameSpaceByBinaryName(Params[0], Params[1]);
  LSPManager.DeleteProtocolByBinaryName(Params[0], Params[1]);
  LSPManager.Free;
  exit;
 end;
 // Лечение SPI
 if MethodName = 'AUTOFIXSPI' then begin
  LSPManager := TLSPManager.Create;
  LSPManager.Refresh;
  LSPManager.AutoRepairNameSpace;
  LSPManager.AutoRepairProtocol;
  LSPManager.Free;
  exit;
 end;
 // Удаление CLSID
 if MethodName = 'DELCLSID' then begin
  DelCLSID(Params[0]);
  exit;
 end;
 // Проверка наличия CLSID
 if MethodName = 'CLSIDEXISTS' then begin
  Result := CLSIDExists(Params[0]);
  exit;
 end;
 // Проверка наличия CLSID
 if MethodName = 'CLSIDFILENAME' then begin
  Result := CLSIDFileName(Params[0]);
  exit;
 end;
 // Удаление BHO
 if MethodName = 'DELBHO' then begin
  DelBHO(Params[0]);
  exit;
 end;
 // Поиск BHO
 if MethodName = 'BHOEXISTS' then begin
  Result := BHOExists(Params[0]);
  exit;
 end;
 if MethodName = 'FILEEXISTS' then begin
  Result := FileExists(Params[0]);
  exit;
 end;
 if MethodName = 'DIRECTORYEXISTS' then begin
  Result := DirectoryExists(Params[0]);
  exit;
 end;
 if MethodName = 'EXTRACTFILENAME' then begin
  Result := ExtractFileName(Params[0]);
  exit;
 end;
 if MethodName = 'EXTRACTFILEPATH' then begin
  Result := ExtractFilePath(Params[0]);
  exit;
 end;
 if MethodName = 'EXTRACTFILEEXT' then begin
  Result := ExtractFileExt(Params[0]);
  exit;
 end;
 if MethodName = 'TRIM' then begin
  Result := Trim(Params[0]);
  exit;
 end;
 if MethodName = 'STRINGREPLACE' then begin
  Result := StringReplace(Params[0], Params[1], Params[2], [rfReplaceAll, rfIgnoreCase]);
  exit;
 end;
 if MethodName = 'REGKEYDEL' then begin
  Result := RegKeyDelete(Params[0], Params[1]);
  exit;
 end;
 if MethodName = 'REGKEYCREATE' then begin
  Result := RegKeyCreate(Params[0], Params[1]);
  exit;
 end;
 if MethodName = 'REGKEYPARAMDEL' then begin
  Result := RegKeyParamDelete(Params[0], Params[1], Params[2]);
  exit;
 end;
 if MethodName = 'REGKEYEXISTS' then begin
  Result := RegKeyExists(Params[0], Params[1]);
  exit;
 end;
 if MethodName = 'REGKEYPARAMEXISTS' then begin
  Result := RegKeyParamExists(Params[0], Params[1], Params[2]);
  exit;
 end;
 if MethodName = 'REGKEYSTRPARAMWRITE' then begin
  Result := RegKeyStrParamSet(Params[0], Params[1], Params[2], Params[3]);
  exit;
 end;
 if MethodName = 'REGKEYSTRPARAMREAD' then begin
  Result := RegKeyStrParamGet(Params[0], Params[1], Params[2]);
  exit;
 end;
 if MethodName = 'REGKEYINTPARAMREAD' then begin
  Result := RegKeyIntParamGet(Params[0], Params[1], Params[2]);
  exit;
 end;
 if MethodName = 'ADDTOQURANTINE' then begin
  Result :=  CopyToInfected(Params[0], Params[1], 'Quarantine');
  exit;
 end;
 if MethodName = 'ANTIROOTKIT' then begin
  Result := ScanRootKit(Params[0], Params[0]);
  exit;
 end;
 if MethodName = 'DELETEVIRUS' then begin
  if not(ReadOnlyMode) then
   Result := DeleteVirus(Params[0], Params[1], 0)
    else AddToLog('Режим "только чтение" - команда DeleteVirus заблокирована');
  exit;
 end;

 if MethodName = 'DELWINLOGONNOTIFYBYFILENAME'  then begin
  DelWinlogonNotifyByFileName(Params[0]);
  exit;
 end;
 if MethodName = 'DELWINLOGONNOTIFYBYKEYNAME'  then begin
  DelWinlogonNotifyByKeyName(Params[0]);
  exit;
 end;
 if MethodName = 'SLEEP'  then begin
  Sleep(Params[0]);
  exit;
 end;
 AddToLog('Ошибка [1, '+MethodName+']');
end;

function TAvzScript.RegKeyDelete(ARoot, AKey: string): boolean;
begin
 Result := false;
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда RegKeyDelete заблокирована');
  exit;
 end;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 Result := Reg.DeleteKey(AKey);
 if Result then
  AddToLog('[микропрограмма лечения]> удален ключ реестра '+ARoot+'\'+AKey);
end;

function TAvzScript.RegKeyExists(ARoot, AKey: string): boolean;
begin
 Result := false;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 Result := Reg.KeyExists(AKey);
end;

function TAvzScript.RegKeyParamExists(ARoot, AKey,
  AParam: string): boolean;
begin
 Result := false;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 if Reg.OpenKey(AKey, false) then begin
  Result := Reg.ValueExists(AParam);
  Reg.CloseKey;
 end;
end;

function TAvzScript.RegKeyParamDelete(ARoot, AKey,
  AParam: string): boolean;
begin
 Result := false;
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда RegKeyParamDelete заблокирована');
  exit;
 end;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 if Reg.OpenKey(AKey, false) then begin
  Result := Reg.DeleteValue(AParam);
  if Result then
   AddToLog('[микропрограмма лечения]> удален параметр '+AParam+' ключа '+ARoot+'\'+AKey);
  Reg.CloseKey;
 end;
end;

function TAvzScript.RegKeyStrParamSet(ARoot, AKey, AParam,
  AValue: string): boolean;
begin
 Result := false;
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда RegKeyStrParamSet заблокирована');
  exit;
 end;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 if Reg.OpenKey(AKey, false) then begin
  Result := true;
  Reg.WriteString(AParam, AValue);
  AddToLog('[микропрограмма лечения]> изменен параметр '+AParam+' ключа '+ARoot+'\'+AKey);
  Reg.CloseKey;
 end;
end;

function TAvzScript.RegKeyStrParamGet(ARoot, AKey, AParam: string): string;
begin
 Result := '';
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 if Reg.OpenKey(AKey, false) then begin
  try
   Result := Reg.ReadString(AParam);
  except
  end;
  Reg.CloseKey;
 end;
end;

function TAvzScript.CLSIDExists(ACLSID: string): boolean;
begin
 ACLSID := Trim(ACLSID);
 if ACLSID = '' then exit;
 Reg.RootKey := HKEY_CLASSES_ROOT;
 Result := Reg.KeyExists('CLSID\{'+ACLSID+'}');
end;

function TAvzScript.CLSIDFileName(ACLSID: string; ARecurLevel : integer = 1): string;
begin
 Result := '';
 // Интизацикливание при рекурсивном поиске
 if ARecurLevel > 10 then exit;
 if Not(CLSIDExists(ACLSID)) then exit;
 ACLSID := 'CLSID\{'+Trim(ACLSID)+'}';
 Reg.RootKey := HKEY_CLASSES_ROOT;
 // Поиск в ключе InprocServer32
 if Reg.OpenKey(ACLSID+'\InprocServer32', false) then begin
  if Reg.ValueExists('') then
   Result := Reg.ReadString('');
  Reg.CloseKey;
  if Result <> '' then exit;
 end;
 // Поиск в ключе LocalServer32
 if Reg.OpenKey(ACLSID+'\LocalServer32', false) then begin
  if Reg.ValueExists('') then
   Result := Reg.ReadString('');
  Reg.CloseKey;
  if Result <> '' then exit;
 end;
 // Поиск в ключе LocalServer
 if Reg.OpenKey(ACLSID+'\LocalServer', false) then begin
  if Reg.ValueExists('') then
   Result := Reg.ReadString('');
  Reg.CloseKey;
  if Result <> '' then exit;
 end;
 // Итак, не нашли ничего - может, это ссылка ??
 if Reg.OpenKey(ACLSID+'\TreatAs', false) then begin
  if Reg.ValueExists('') then
   Result := Reg.ReadString('');
  Reg.CloseKey;
  if Result <> '' then begin
   Result := StringReplace(Result, '{', '', []);
   Result := Trim(StringReplace(Result, '}', '', []));
   Result := CLSIDFileName(Result, ARecurLevel+1);
  end;
 end;
end;

function TAvzScript.EvCheckCLSID(ACLSID, AVirusName,
  AFileMask: string): boolean;
begin
 Main.EvSysFileCheckByCLSID(ACLSID, AVirusName, AFileMask);
end;

function TAvzScript.EvCheckFileName(AFileName, AVirusName,
  AFileMask: string): boolean;
begin
 Main.EvSysFileCheckByName(AFileName, AVirusName, AFileMask);
end;

procedure TAvzScript.DelWinlogonNotifyByFileName(AFileName: string);
var
 Reg   : TRegistry;
 Lines : TStringList;
 i     : integer;
 S     : string;
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelWinlogonNotifyByFileName заблокирована');
  exit;
 end;
 Reg   := TRegistry.Create;
 Lines := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify', false) then begin
  Reg.GetKeyNames(Lines);
  Reg.CloseKey;
  // Цикл поиска ключа
  for i := 0 to Lines.Count - 1 do
   if Reg.OpenKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+Lines[i], false) then begin
    if Reg.ValueExists('DllName') then begin
     S :=  LowerCase(Trim(NTFileNameToNormalName(LowerCase(Trim(Reg.ReadString('DllName'))))));
     if ExtractFilePath(S) = '' then begin
      if FileExists(GetSystemDirectoryPath+S) then S := GetSystemDirectoryPath + S;
     end;
     if S = LowerCase(AFileName) then begin
      Reg.DeleteKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+Lines[i]);
      AddToLog('Удален элемент Winlogon, ссылающийся на '+AFileName);
     end;
    end;
    try Reg.CloseKey; except end;
   end;
 end;
 Reg.Free;
 Lines.Free;
end;

procedure TAvzScript.DelWinlogonNotifyByKeyName(AKeyName: string);
var
 Reg   : TRegistry;
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelWinlogonNotifyByKeyName заблокирована');
  exit;
 end;
 Reg   := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.DeleteKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+AKeyName) then begin
  AddToLog('Удален элемент Winlogon с именем '+AKeyName);
 end;
 Reg.Free;
end;

procedure TAvzScript.DelActiveSetup(ACLSID: string);
var
 S : string;
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelActiveSetup заблокирована');
  exit;
 end;
 if Trim(ACLSID) = '' then exit;
 S := '{'+Trim(ACLSID)+'}';
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.DeleteKey('SOFTWARE\Microsoft\Active Setup\Installed Components\'+S) then
  AddToLog('[микропрограмма лечения]> Удален элемент Active Setup '+S);
end;

procedure TAvzScript.DelShellExt(ACLSID: string);
var
 S : string;
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelShellExt заблокирована');
  exit;
 end;
 if Trim(ACLSID) = '' then exit;
 S := '{'+Trim(ACLSID)+'}';
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.DeleteKey('SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved'+S) then
  AddToLog('[микропрограмма лечения]> Удален элемент Shell Extensions '+S);
end;

procedure TAvzScript.DelSharedTaskSch(ACLSID: string);
var
 S : string;
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelSharedTaskSch заблокирована');
  exit;
 end;
 if Trim(ACLSID) = '' then exit;
 S := '{'+Trim(ACLSID)+'}';
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey('SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\SharedTaskScheduler\', false) then begin
  if Reg.ValueExists(S) then
   if Reg.DeleteValue(S) then
    AddToLog('[микропрограмма лечения]> Удален элемент Shared Task Scheduler '+S);
  Reg.CloseKey;  
 end;
end;

procedure TAvzScript.DelInterface(ACLSID: string);
begin
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда DelInterface заблокирована');
  exit;
 end;
 if Trim(ACLSID) = '' then exit;
 Reg.RootKey := HKEY_CLASSES_ROOT;
 // Удаление ключа реестра
 try
  if Reg.DeleteKey('CLSID\Interface\{'+Trim(ACLSID)+'}') then
   AddToLog('[микропрограмма лечения]> Удален Interface '+ACLSID);
 except end;
end;

function TAvzScript.RegKeyCreate(ARoot, AKey: string): boolean;
begin
 Result := false;
 if ReadOnlyMode then begin
  AddToLog('Режим "только чтение" - команда RegKeyCreate заблокирована');
  exit;
 end;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 Result := Reg.CreateKey(AKey);
 if Result then
  AddToLog('[микропрограмма лечения]> создан ключ реестра '+ARoot+'\'+AKey);
end;

function TAvzScript.OnScriptCallMetodEvGroup(Instance: TObject;
  ClassType: TClass; const MethodName: String;
  var Params: Variant): Variant;
begin
 if MethodName = 'ADDEVALARMBYCLSID' then begin
  Result := EvCheckCLSID(Params[0], Params[1], Params[2]);
  exit;
 end;
 if MethodName = 'ADDEVALARMBYNAME' then begin
  Result := EvCheckFileName(Params[0], Params[1], Params[2]);
  exit;
 end;
 AddToLog('Ошибка [2, '+MethodName+']');
end;

procedure TAvzScript.SetLogEnabled(const Value: boolean);
begin
  FLogEnabled := Value;
end;

function TAvzScript.RegKeyIntParamGet(ARoot, AKey,
  AParam: string): integer;
begin
 Result := -1;
 if not(SetRootKeyByName(ARoot)) then exit;
 if trim(AKey) = '' then exit;
 if Reg.OpenKey(AKey, false) then begin
  try
   Result := Reg.ReadInteger(AParam);
  except
  end;
  Reg.CloseKey;
 end;
end;

end.


