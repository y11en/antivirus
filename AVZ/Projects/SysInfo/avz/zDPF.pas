unit zDPF;

interface
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, zSharedFunctions, zAVKernel, zAVZKernel, zLogSystem;
type
 // Информация о DPF
 TDPFInfo = record
  RegKey          : string;  // Ключ реестра
  CLSID           : string;  // CLSID
  BinFile         : string;  // Бинарный файл
  Descr           : string;  // Описание
  LegalCopyright  : string;  // Копирайты
  CheckResult     : integer; // Результат проверки по базе безопасных программ
  CodeBase        : string;  // Источник
 end;
 TDPFInfoArray = array of TDPFInfo;

 // Менеджер DPF
 TDPFList = class
   DPFInfoArray : TDPFInfoArray;
   constructor Create;
   destructor Destroy; override;
   function RefresDPFList : boolean;
   function ScanDPFKeys(APrefix : string) : boolean;
   function GetItemEnabledStatus(DPFInfo : TDPFInfo) : integer;
   function DeleteItem(DPFInfo : TDPFInfo) : boolean;
   function SetItemEnabledStatus(var DPFInfo : TDPFInfo; NewStatus : boolean) : boolean;
   function AddDPF(DPFInfo : TDPFInfo) : boolean;
 end;

implementation
uses zScriptingKernel;
{ TDPFList }

function TDPFList.AddDPF(DPFInfo: TDPFInfo): boolean;
var
 VersionInfo : TVersionInfo;
begin
 DPFInfo.BinFile := NTFileNameToNormalName(DPFInfo.BinFile);
 DPFInfo.CheckResult := FileSignCheck.CheckFile(DPFInfo.BinFile);
 try
  VersionInfo := TVersionInfo.Create(DPFInfo.BinFile);
  DPFInfo.Descr := VersionInfo.FileDescription;
  DPFInfo.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 SetLength(DPFInfoArray, Length(DPFInfoArray)+1);
 DPFInfoArray[Length(DPFInfoArray)-1] := DPFInfo;
end;

constructor TDPFList.Create;
begin
 DPFInfoArray := nil;
end;

destructor TDPFList.Destroy;
begin
 DPFInfoArray := nil;
 inherited;
end;

function TDPFList.DeleteItem(DPFInfo: TDPFInfo): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 Reg := TRegistry.Create;
 try
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.DeleteKey(DPFInfo.RegKey+'\'+DPFInfo.CLSID);
 except
 end; 
 Result := true;
 Reg.Free;
end;

function TDPFList.GetItemEnabledStatus(DPFInfo: TDPFInfo): integer;
begin
 Result := 1;
 if copy(DPFInfo.RegKey, length(DPFInfo.RegKey), 1) = '-' then
  Result := 0;
end;

function TDPFList.RefresDPFList: boolean;
begin
 DPFInfoArray := nil;
 ScanDPFKeys('');
 ScanDPFKeys('-');
end;

function TDPFList.ScanDPFKeys(APrefix: string): boolean;
var
 Reg  : TRegistry;
 List : TStrings;
 Tmp  : TDPFInfo;
 i    : integer;
 BaseKey, S    : string;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 // Поиск DPF
 BaseKey := 'SOFTWARE\Microsoft\Code Store Database\Distribution Units'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  ZProgressClass.AddProgressMax(List.Count);
  for i := 0 to List.Count - 1 do begin
   ZProgressClass.ProgressStep;
   Tmp.RegKey  := BaseKey;
   Tmp.CLSID   := Trim(List[i]);
   Tmp.CodeBase := '';
   if Reg.OpenKey(BaseKey + '\'+List[i]+'\DownloadInformation', false) then begin
    try Tmp.CodeBase := Reg.ReadString('CODEBASE'); except end;
    Reg.CloseKey;
   end;
   S := copy(Tmp.CLSID, 2, length(Tmp.CLSID)-2);
   Tmp.BinFile := CLSIDFileName(S);
   AddDPF(Tmp);
  end;
 end;
 List.Free;
 Reg.Free;
end;

function TDPFList.SetItemEnabledStatus(var DPFInfo: TDPFInfo;
  NewStatus: boolean): boolean;
var
 Reg : TRegistry;
 S   : string;
begin
 Result := false;
 // Лог. защита - блокирования активации активного и выключения неактивного
 if NewStatus      and  (GetItemEnabledStatus(DPFInfo) <> 0) then exit;
 if not(NewStatus) and  (GetItemEnabledStatus(DPFInfo) <> 1) then exit;
 // Блокировка
 if not(NewStatus) then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.MoveKey(DPFInfo.RegKey+'\'+DPFInfo.CLSID, DPFInfo.RegKey+'-\'+DPFInfo.CLSID, true);
  DPFInfo.RegKey := DPFInfo.RegKey + '-';
  Result := true;
 end;
 // Разблокировка
 if NewStatus then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.MoveKey(DPFInfo.RegKey+'\'+DPFInfo.CLSID, copy(DPFInfo.RegKey, 1, length(DPFInfo.RegKey)-1)+'\'+DPFInfo.CLSID, true);
  DPFInfo.RegKey := copy(DPFInfo.RegKey, 1, length(DPFInfo.RegKey)-1);
  Result := true;
 end;
end;

end.
