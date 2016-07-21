unit zBHO;
// Класс для оперирования BHO
interface
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, zSharedFunctions, zAVKernel, zAVZKernel;
type
 // Информация о BHO
 TBHOInfo = record
  RegKey          : string;  // Ключ реестра
  CLSID           : string;  // CLSID
  BinFile         : string;  // Бинарный файл
  Descr           : string;  // Описание
  LegalCopyright  : string;  // Копирайты
  CheckResult     : integer; // Результат проверки по базе безопасных программ
  BHOType         : integer; // Код типа BHO (1-BHO, 2-Toolbar, 3- IE Extention)
 end;
 TBHOInfoArray = array of TBHOInfo;
 // Менеджер BHO
 TBHOList = class
   BHOInfoArray : TBHOInfoArray;
   constructor Create;
   destructor Destroy; override;
   function RefresBHOList : boolean;
   function ScanBHOKeys(APrefix : string) : boolean;
   function GetItemEnabledStatus(BHOInfo : TBHOInfo) : integer;
   function DeleteItem(BHOInfo : TBHOInfo) : boolean;
   function SetItemEnabledStatus(var BHOInfo : TBHOInfo; NewStatus : boolean) : boolean;
   function AddBHO(BHOInfo : TBHOInfo; AType : integer) : boolean;
 end;
implementation
uses zutil, zScriptingKernel, zLogSystem;

{ TBHOManager }
function TBHOList.AddBHO(BHOInfo: TBHOInfo; AType: integer): boolean;
var
 VersionInfo : TVersionInfo;
begin
 BHOInfo.BinFile := NTFileNameToNormalName(BHOInfo.BinFile);
 BHOInfo.CheckResult := FileSignCheck.CheckFile(BHOInfo.BinFile);
 BHOInfo.BHOType     := AType;
 try
  VersionInfo := TVersionInfo.Create(BHOInfo.BinFile);
  BHOInfo.Descr := VersionInfo.FileDescription;
  BHOInfo.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 SetLength(BHOInfoArray, Length(BHOInfoArray)+1);
 BHOInfoArray[Length(BHOInfoArray)-1] := BHOInfo;
end;

constructor TBHOList.Create;
begin
 BHOInfoArray := nil;
end;

function TBHOList.DeleteItem(BHOInfo: TBHOInfo): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if BHOInfo.BHOType in [1,3] then begin
  Reg.DeleteKey(BHOInfo.RegKey+'\'+BHOInfo.CLSID);
  Result := true;
 end else begin
  if Reg.OpenKey(BHOInfo.RegKey, false) then begin
   Reg.DeleteValue(BHOInfo.CLSID);
   Reg.CloseKey;
   Result := true;
  end;
 end;
 Reg.Free;
 Result := true;
end;

destructor TBHOList.Destroy;
begin
 BHOInfoArray := nil;
 inherited;
end;

function TBHOList.GetItemEnabledStatus(BHOInfo: TBHOInfo): integer;
begin
 Result := 1;
 if copy(BHOInfo.RegKey, length(BHOInfo.RegKey), 1) = '-' then
  Result := 0;
end;

function TBHOList.RefresBHOList: boolean;
begin
 BHOInfoArray := nil;
 ScanBHOKeys('');
 ScanBHOKeys('-');
end;

function TBHOList.ScanBHOKeys(APrefix: string): boolean;
var
 Reg  : TRegistry;
 List : TStrings;
 Tmp  : TBHOInfo;
 i    : integer;
 BaseKey, S    : string;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 // Поиск BHO
 BaseKey := 'Software\Microsoft\Windows\CurrentVersion\Explorer\Browser Helper Objects'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  ZProgressClass.AddProgressMax(List.Count);
  for i := 0 to List.Count - 1 do begin
   ZProgressClass.ProgressStep;
   Tmp.RegKey  := BaseKey;
   Tmp.CLSID   := Trim(List[i]);
   S := copy(Tmp.CLSID, 2, length(Tmp.CLSID)-2);
   Tmp.BinFile := CLSIDFileName(S);
   AddBHO(Tmp, 1);
  end;
  Reg.CloseKey;
 end;
 // Поиск ToolBar
 BaseKey := 'SOFTWARE\Microsoft\Internet Explorer\Toolbar'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetValueNames(List);
  ZProgressClass.AddProgressMax(List.Count);
  for i := 0 to List.Count - 1 do begin
   ZProgressClass.ProgressStep;
   Tmp.RegKey  := BaseKey;
   Tmp.CLSID   := Trim(List[i]);
   S := copy(Tmp.CLSID, 2, length(Tmp.CLSID)-2);
   Tmp.BinFile := CLSIDFileName(S);
   AddBHO(Tmp, 2);
  end;
  Reg.CloseKey;
 end;
 // Поиск Extention
 BaseKey := 'Software\Microsoft\Internet Explorer\Extensions'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  ZProgressClass.AddProgressMax(List.Count);
  for i := 0 to List.Count - 1 do begin
   ZProgressClass.ProgressStep;
   Tmp.RegKey  := BaseKey;
   Tmp.CLSID   := Trim(List[i]);
   if Reg.OpenKey(BaseKey +'\'+ List[i], false) then begin
    try
     if Reg.ValueExists('ClsidExtension') then begin
      S := copy(Tmp.CLSID, 2, length(Tmp.CLSID)-2);
      Tmp.BinFile := CLSIDFileName(S);
     end else if Reg.ValueExists('Exec') then begin
      Tmp.BinFile := Reg.ReadString('Exec');
     end else if Reg.ValueExists('Script') then begin
      Tmp.BinFile := Reg.ReadString('Script');
     end;
     AddBHO(Tmp, 3);
    except end;
   end;
   Reg.CloseKey;
  end;
 end;
 List.Free;
 Reg.Free;
end;

function TBHOList.SetItemEnabledStatus(var BHOInfo: TBHOInfo;
  NewStatus: boolean): boolean;
var
 Reg : TRegistry;
 S   : string;
begin
 Result := false;
 // Лог. защита - блокирования активации активного и выключения неактивного
 if NewStatus      and  (GetItemEnabledStatus(BHOInfo) <> 0) then exit;
 if not(NewStatus) and  (GetItemEnabledStatus(BHOInfo) <> 1) then exit;
 // Блокировка
 if not(NewStatus) then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  if BHOInfo.BHOType in [1,3] then begin
   Reg.MoveKey(BHOInfo.RegKey+'\'+BHOInfo.CLSID, BHOInfo.RegKey+'-\'+BHOInfo.CLSID, true);
   BHOInfo.RegKey := BHOInfo.RegKey + '-';
   Result := true;
  end else begin
   if Reg.OpenKey(BHOInfo.RegKey, false) then begin
    S := Reg.ReadString(BHOInfo.CLSID);
    Reg.DeleteValue(BHOInfo.CLSID);
    Reg.CloseKey;
    // Создание на новом месте
    BHOInfo.RegKey := BHOInfo.RegKey + '-';
    if Reg.OpenKey(BHOInfo.RegKey, true) then begin
     Reg.WriteString(BHOInfo.CLSID, S);
     Reg.CloseKey;
     Result := true;
    end;
   end;
  end;
 end;
 // Разблокировка
 if NewStatus then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  if BHOInfo.BHOType in [1,3] then begin
   Reg.MoveKey(BHOInfo.RegKey+'\'+BHOInfo.CLSID, copy(BHOInfo.RegKey, 1, length(BHOInfo.RegKey)-1)+'\'+BHOInfo.CLSID, true);
   BHOInfo.RegKey := copy(BHOInfo.RegKey, 1, length(BHOInfo.RegKey)-1);
   Result := true;
  end else begin
   if Reg.OpenKey(BHOInfo.RegKey, false) then begin
    S := Reg.ReadString(BHOInfo.CLSID);
    Reg.DeleteValue(BHOInfo.CLSID);
    Reg.CloseKey;
    BHOInfo.RegKey := copy(BHOInfo.RegKey, 1, length(BHOInfo.RegKey)-1);
    // Создание на новом месте
    if Reg.OpenKey(BHOInfo.RegKey, true) then begin
     Reg.WriteString(BHOInfo.CLSID, S);
     Reg.CloseKey;
    end;
    Result := true;
   end;
  end;
 end;
end;

end.
