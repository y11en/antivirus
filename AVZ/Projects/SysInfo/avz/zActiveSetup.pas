unit zActiveSetup;
// Класс сбора данных о Active Setup элементах


interface
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, zSharedFunctions, zAVKernel;
type
 // Информация о элементе
 TActiveSetupInfo = record
  RegKey          : string;  // Ключ реестра
  CLSID           : string;  // CLSID
  BinFile         : string;  // Бинарный файл
  Descr           : string;  // Описание
  LegalCopyright  : string;  // Копирайты
  CheckResult     : integer; // Результат проверки по базе безопасных программ
 end;
 TASInfoArray = array of TActiveSetupInfo;

  // Менеджер AS
 TASList = class
   ActiveSetupInfoArray : TASInfoArray;
   constructor Create;
   destructor Destroy; override;
   function RefresASList : boolean;
   function ScanASKeys(APrefix : string) : boolean;
   function GetItemEnabledStatus(ASInfo : TActiveSetupInfo) : integer;
   function DeleteItem(ASInfo : TActiveSetupInfo) : boolean;
   function SetItemEnabledStatus(var ASInfo : TActiveSetupInfo; NewStatus : boolean) : boolean;
   function AddAS(ASInfo : TActiveSetupInfo) : boolean;
 end;


implementation
uses ZLogSystem;
{ TASList }

function TASList.AddAS(ASInfo: TActiveSetupInfo): boolean;
var
 VersionInfo : TVersionInfo;
begin
 Result := false;
 // Блокировка добавления пустых строк
 ASInfo.BinFile := Trim(ASInfo.BinFile);
 if ASInfo.BinFile = '' then
  exit;
 // Нормализация имени файла и его проверка по базе безопасных
 ASInfo.BinFile     := NormalProgramFileName(ASInfo.BinFile, '.exe');
 ASInfo.CheckResult := FileSignCheck.CheckFile(ASInfo.BinFile);
 try
  VersionInfo           := TVersionInfo.Create(ASInfo.BinFile);
  ASInfo.Descr          := VersionInfo.FileDescription;
  ASInfo.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 SetLength(ActiveSetupInfoArray, Length(ActiveSetupInfoArray)+1);
 ActiveSetupInfoArray[Length(ActiveSetupInfoArray)-1] := ASInfo;
 Result := true;
end;

constructor TASList.Create;
begin
 ActiveSetupInfoArray := nil;
end;

function TASList.DeleteItem(ASInfo: TActiveSetupInfo): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 Reg.DeleteKey(ASInfo.RegKey+'\'+ASInfo.CLSID);
 Reg.Free;
 Result := true;
end;

destructor TASList.Destroy;
begin
 ActiveSetupInfoArray := nil;
 inherited;
end;

function TASList.GetItemEnabledStatus(ASInfo: TActiveSetupInfo): integer;
begin
 Result := 1;
 if copy(ASInfo.RegKey, length(ASInfo.RegKey), 1) = '-' then
  Result := 0;
end;

function TASList.RefresASList: boolean;
begin
 ActiveSetupInfoArray := nil;
 // Сканирование ключей
 ScanASKeys('');
 ScanASKeys('-');
end;

function TASList.ScanASKeys(APrefix: string): boolean;
var
 Reg  : TRegistry;
 List : TStrings;
 Tmp  : TActiveSetupInfo;
 i    : integer;
 BaseKey    : string;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 // Поиск BHO
 BaseKey := 'Software\Microsoft\Active Setup\Installed Components'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  ZProgressClass.AddProgressMax(List.Count);
  for i := 0 to List.Count - 1 do begin
   ZProgressClass.ProgressStep;
   Tmp.RegKey  := BaseKey;
   Tmp.CLSID   := Trim(List[i]);
   Tmp.BinFile := '';
   try
    Reg.CloseKey;
    if Reg.OpenKey(Tmp.RegKey + '\' + Tmp.CLSID, false) then begin
     Tmp.BinFile := Reg.ReadString('StubPath');
     AddAS(Tmp);
     Reg.CloseKey;
    end;
   except end;
  end;
  Reg.CloseKey;
 end;
 Reg.Free;
end;

function TASList.SetItemEnabledStatus(var ASInfo: TActiveSetupInfo;
  NewStatus: boolean): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 // Лог. защита - блокирования активации активного и выключения неактивного
 if NewStatus      and  (GetItemEnabledStatus(ASInfo) <> 0) then exit;
 if not(NewStatus) and  (GetItemEnabledStatus(ASInfo) <> 1) then exit;
 // Блокировка
 if not(NewStatus) then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.MoveKey(ASInfo.RegKey+'\'+ASInfo.CLSID, ASInfo.RegKey+'-\'+ASInfo.CLSID, true);
  ASInfo.RegKey := ASInfo.RegKey + '-';
  Result := true;
 end;
 // Разблокировка
 if NewStatus then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.MoveKey(ASInfo.RegKey+'\'+ASInfo.CLSID, copy(ASInfo.RegKey, 1, length(ASInfo.RegKey)-1)+'\'+ASInfo.CLSID, true);
  ASInfo.RegKey := copy(ASInfo.RegKey, 1, length(ASInfo.RegKey)-1);
  Result := true;
 end;
end;

end.
