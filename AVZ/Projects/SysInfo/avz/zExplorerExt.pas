unit zExplorerExt;

interface
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, zSharedFunctions, zAVKernel, zAVZKernel, zLogSystem;
type
 // Информация о расширении проводника
 TExplorerExtInfo = record
  RegKey          : string;  // Ключ реестра
  CLSID           : string;  // CLSID
  BinFile         : string;  // Бинарный файл
  ExtName         : string;  // Название
  Descr           : string;  // Описание файла
  LegalCopyright  : string;  // Копирайты файла
  CheckResult     : integer; // Результат проверки по базе безопасных программ
 end;
 TExplorerExtInfoArray = array of TExplorerExtInfo;
 // Менеджер расширений проводника
 TExplorerExtList = class
   ExplorerExtInfoArray : TExplorerExtInfoArray;
   constructor Create;
   destructor Destroy; override;
   function RefresList : boolean;
   function ScanExplorerExtKeys(APrefix : string) : boolean;
   function GetItemEnabledStatus(ItemInfo : TExplorerExtInfo) : integer;
   function DeleteItem(ItemInfo : TExplorerExtInfo) : boolean;
   function SetItemEnabledStatus(var ItemInfo : TExplorerExtInfo; NewStatus : boolean) : boolean;
   function AddExplorerExt(ItemInfo : TExplorerExtInfo) : boolean;
  private
    function ExpandModuleFilename(AModuleName: string): string;
 end;

implementation
uses zutil, zScriptingKernel;
{ TExplorerExtList }

function TExplorerExtList.ExpandModuleFilename(AModuleName : string): string;
begin
 Result := NTFileNameToNormalName(Trim(AModuleName));
 if not(FileExists(Result)) then
  if FileExists(GetSystemDirectoryPath+Result) then
   Result := GetSystemDirectoryPath+Result else
    if FileExists(GetWindowsDirectoryPath+Result) then
     Result := GetWindowsDirectoryPath+Result;
end;

function TExplorerExtList.AddExplorerExt(
  ItemInfo: TExplorerExtInfo): boolean;
var
 VersionInfo : TVersionInfo;
begin
 ItemInfo.BinFile := ExpandModuleFilename(ItemInfo.BinFile);

 ItemInfo.CheckResult := FileSignCheck.CheckFile(ItemInfo.BinFile);
 try
  VersionInfo := TVersionInfo.Create(ItemInfo.BinFile);
  ItemInfo.Descr := VersionInfo.FileDescription;
  ItemInfo.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 SetLength(ExplorerExtInfoArray, Length(ExplorerExtInfoArray)+1);
 ExplorerExtInfoArray[Length(ExplorerExtInfoArray)-1] := ItemInfo;
end;

constructor TExplorerExtList.Create;
begin
 ExplorerExtInfoArray := nil;
end;

function TExplorerExtList.DeleteItem(
  ItemInfo: TExplorerExtInfo): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 Reg.DeleteKey(ItemInfo.RegKey+'\'+ItemInfo.CLSID);
 Reg.Free;
 Result := true;
end;

destructor TExplorerExtList.Destroy;
begin
  ExplorerExtInfoArray := nil;
  inherited;
end;

function TExplorerExtList.GetItemEnabledStatus(
  ItemInfo: TExplorerExtInfo): integer;
begin
 Result := 1;
 if copy(ItemInfo.RegKey, length(ItemInfo.RegKey), 1) = '-' then
  Result := 0;
end;

function TExplorerExtList.RefresList: boolean;
begin
 ExplorerExtInfoArray := nil;
 ScanExplorerExtKeys('');
 ScanExplorerExtKeys('-');
end;

function TExplorerExtList.ScanExplorerExtKeys(APrefix: string): boolean;
var
 Reg  : TRegistry;
 List : TStrings;
 Tmp  : TExplorerExtInfo;
 i    : integer;
 BaseKey, S    : string;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 // Поиск элементов расширения
 BaseKey := 'SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetValueNames(List);
  ZProgressClass.AddProgressMax(List.Count);
  for i := 0 to List.Count - 1 do begin
   ZProgressClass.ProgressStep;
   Tmp.RegKey  := BaseKey;
   Tmp.CLSID   := Trim(List[i]);
   S := copy(Tmp.CLSID, 2, length(Tmp.CLSID)-2);
   Tmp.BinFile := CLSIDFileName(S);
   Tmp.ExtName := '';
   if Reg.GetDataType(List[i]) in [rdString, rdExpandString] then
    Tmp.ExtName := Reg.ReadString(List[i]);
   AddExplorerExt(Tmp);
  end;
  Reg.CloseKey;
 end;
 List.Free;
 Reg.Free;
end;

function TExplorerExtList.SetItemEnabledStatus(
  var ItemInfo: TExplorerExtInfo; NewStatus: boolean): boolean;
var
 Reg : TRegistry;
 S   : string;
begin
 Result := false;
 // Лог. защита - блокирования активации активного и выключения неактивного
 if NewStatus      and  (GetItemEnabledStatus(ItemInfo) <> 0) then exit;
 if not(NewStatus) and  (GetItemEnabledStatus(ItemInfo) <> 1) then exit;
 // Блокировка
 if not(NewStatus) then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  if Reg.OpenKey(ItemInfo.RegKey, false) then begin
   S := Reg.ReadString(ItemInfo.CLSID);
   Reg.DeleteValue(ItemInfo.CLSID);
   Reg.CloseKey;
   // Создание на новом месте
   ItemInfo.RegKey := ItemInfo.RegKey + '-';
   if Reg.OpenKey(ItemInfo.RegKey, true) then begin
    Reg.WriteString(ItemInfo.CLSID, S);
    Reg.CloseKey;
    Result := true;
   end;
  end;
 end;
 // Разблокировка
 if NewStatus then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  if Reg.OpenKey(ItemInfo.RegKey, false) then begin
   S := Reg.ReadString(ItemInfo.CLSID);
   Reg.DeleteValue(ItemInfo.CLSID);
   Reg.CloseKey;
   ItemInfo.RegKey := copy(ItemInfo.RegKey, 1, length(ItemInfo.RegKey)-1);
   // Создание на новом месте
   if Reg.OpenKey(ItemInfo.RegKey, true) then begin
    Reg.WriteString(ItemInfo.CLSID, S);
    Reg.CloseKey;
   end;
   Result := true;
  end;
 end;
end;

end.
