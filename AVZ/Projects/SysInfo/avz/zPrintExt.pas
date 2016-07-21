unit zPrintExt;

interface
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, zSharedFunctions, zAVKernel;
type
 // Информация о расширении системы печати
 TPrintExtInfo = record
  RegKey, ExtKey  : string;  // Ключ реестра
  Name            : string;  // Имя
  BinFile         : string;  // Бинарный файл
  Descr           : string;  // Описание файла
  LegalCopyright  : string;  // Копирайты файла
  CheckResult     : integer; // Результат проверки по базе безопасных программ
  PrintExtType    : integer; // Тип
 end;
 TPrintExtInfoArray = array of TPrintExtInfo;
 // Менеджер PrintExt
 TPrintExtList = class
   PrintExtInfoArray : TPrintExtInfoArray;
   constructor Create;
   destructor Destroy; override;
   // Обновлени списка
   function RefresPrintExtList : boolean;
   // Сканирование ключей
   function ScanPrintExtKeys(APrefix : string) : boolean;
   // Получение статууса элемента
   function GetItemEnabledStatus(PrintExtInfo : TPrintExtInfo) : integer;
   // Удаление элемента
   function DeleteItem(PrintExtInfo : TPrintExtInfo) : boolean;
   // Настройка включен/выключен
   function SetItemEnabledStatus(var PrintExtInfo : TPrintExtInfo; NewStatus : boolean) : boolean;
   function AddPrintExt(PrintExtInfo : TPrintExtInfo; AType : integer) : boolean;
 end;
implementation
uses zutil;

{ TPrintExtManager }
function TPrintExtList.AddPrintExt(PrintExtInfo: TPrintExtInfo; AType: integer): boolean;
var
 VersionInfo : TVersionInfo;
begin
 PrintExtInfo.BinFile     := NTFileNameToNormalName(PrintExtInfo.BinFile);
 if not(FileExists(PrintExtInfo.BinFile)) then
  if FileExists(GetSystemDirectoryPath + PrintExtInfo.BinFile) then
   PrintExtInfo.BinFile := GetSystemDirectoryPath + PrintExtInfo.BinFile
    else if FileExists(GetWindowsDirectoryPath + PrintExtInfo.BinFile) then
     PrintExtInfo.BinFile := GetWindowsDirectoryPath + PrintExtInfo.BinFile;
 PrintExtInfo.CheckResult := FileSignCheck.CheckFile(PrintExtInfo.BinFile);
 PrintExtInfo.PrintExtType     := AType;
 try
  VersionInfo := TVersionInfo.Create(PrintExtInfo.BinFile);
  PrintExtInfo.Descr := VersionInfo.FileDescription;
  PrintExtInfo.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 SetLength(PrintExtInfoArray, Length(PrintExtInfoArray)+1);
 PrintExtInfoArray[Length(PrintExtInfoArray)-1] := PrintExtInfo;
end;

constructor TPrintExtList.Create;
begin
 PrintExtInfoArray := nil;
end;

function TPrintExtList.DeleteItem(PrintExtInfo: TPrintExtInfo): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 Reg := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 Reg.DeleteKey(PrintExtInfo.RegKey+'\'+PrintExtInfo.ExtKey);
 Reg.Free;
 Result := true;
end;

destructor TPrintExtList.Destroy;
begin
 PrintExtInfoArray := nil;
 inherited;
end;

function TPrintExtList.GetItemEnabledStatus(PrintExtInfo: TPrintExtInfo): integer;
begin
 Result := 1;
 if copy(PrintExtInfo.RegKey, length(PrintExtInfo.RegKey), 1) = '-' then
  Result := 0;
end;

function TPrintExtList.RefresPrintExtList: boolean;
begin
 PrintExtInfoArray := nil;
 // Поиск обычных ключей
 ScanPrintExtKeys('');
 // Поиск заблокированных ключей
 ScanPrintExtKeys('-');
end;

function TPrintExtList.ScanPrintExtKeys(APrefix: string): boolean;
var
 Reg  : TRegistry;
 List : TStrings;
 Tmp  : TPrintExtInfo;
 i    : integer;
 BaseKey    : string;
begin
 Reg  := TRegistry.Create;
 List := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 // Поиск PrintExt типа Monitor
 BaseKey := 'SYSTEM\CurrentControlSet\Control\Print\Monitors'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  for i := 0 to List.Count - 1 do begin
   Tmp.RegKey  := BaseKey;
   Tmp.ExtKey  := List[i];
   Tmp.Name    := List[i];
   if Reg.OpenKey(BaseKey+'\'+List[i], false) then begin
    if Reg.ValueExists('Driver') then begin
     Tmp.BinFile := Reg.ReadString('Driver');
     AddPrintExt(Tmp, 1);
    end;
    Reg.CloseKey;
   end;
  end;
 end;
 // Поиск PrintExt типа Providers
 BaseKey := 'SYSTEM\CurrentControlSet\Control\Print\Providers'+APrefix;
 if Reg.OpenKey(BaseKey, false) then begin
  Reg.GetKeyNames(List);
  Reg.CloseKey;
  for i := 0 to List.Count - 1 do begin
   Tmp.RegKey  := BaseKey;
   Tmp.ExtKey  := List[i];
   if Reg.OpenKey(BaseKey+'\'+List[i], false) then begin
    if Reg.ValueExists('Name') then begin
     Tmp.BinFile := Reg.ReadString('Name');
     try Tmp.Name    := Reg.ReadString('DisplayName');except end;
     AddPrintExt(Tmp, 2);
    end;
    Reg.CloseKey;
   end;
  end;
 end;
 List.Free;
 Reg.Free;
end;

function TPrintExtList.SetItemEnabledStatus(var PrintExtInfo: TPrintExtInfo;
  NewStatus: boolean): boolean;
var
 Reg : TRegistry;
begin
 Result := false;
 // Лог. защита - блокирования активации активного и выключения неактивного
 if NewStatus      and  (GetItemEnabledStatus(PrintExtInfo) <> 0) then exit;
 if not(NewStatus) and  (GetItemEnabledStatus(PrintExtInfo) <> 1) then exit;
 // Блокировка
 if not(NewStatus) then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.MoveKey(PrintExtInfo.RegKey+'\'+PrintExtInfo.ExtKey, PrintExtInfo.RegKey+'-\'+PrintExtInfo.ExtKey, true);
  PrintExtInfo.RegKey := PrintExtInfo.RegKey + '-';
  Reg.Free;
  Result := true;
 end;
 // Разблокировка
 if NewStatus then begin
  Reg := TRegistry.Create;
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.MoveKey(PrintExtInfo.RegKey+'\'+PrintExtInfo.ExtKey, copy(PrintExtInfo.RegKey, 1, length(PrintExtInfo.RegKey)-1)+'\'+PrintExtInfo.ExtKey, true);
  PrintExtInfo.RegKey := copy(PrintExtInfo.RegKey, 1, length(PrintExtInfo.RegKey)-1);
  Reg.Free;
  Result := true;
 end;
end;

end.
