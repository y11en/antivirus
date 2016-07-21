unit zCPLExt;

interface
uses Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, RxVerInf,
     Registry, zSharedFunctions, zAVKernel, zAVZKernel;
type
 // Информация о расширении системы печати
 TCPLInfo = record
  Name            : string;  // Имя
  BinFile         : string;  // Бинарный файл
  Descr           : string;  // Описание файла
  LegalCopyright  : string;  // Копирайты файла
  CheckResult     : integer; // Результат проверки по базе безопасных программ
 end;

 TCPLInfoArray = array of TCPLInfo;

 // Менеджер CPL
 TCPLList = class
   CPLInfoArray : TCPLInfoArray;
   constructor Create;
   destructor Destroy; override;
   // Обновлени списка
   function RefresCPLList : boolean;
   // Получение статуса элемента
   function GetItemEnabledStatus(CPLInfo : TCPLInfo) : integer;
   // Удаление элемента
   function DeleteItem(CPLInfo : TCPLInfo) : boolean;
   // Настройка включен/выключен
   function SetItemEnabledStatus(var CPLInfo : TCPLInfo; NewStatus : boolean) : boolean;
   function AddCPL(CPLInfo : TCPLInfo) : boolean;
   function SearchCPLFiles(APath : string; Status : boolean) : boolean;
 end;


implementation
uses zutil;

{ TCPLList }

function TCPLList.AddCPL(CPLInfo: TCPLInfo): boolean;
var
 VersionInfo : TVersionInfo;
begin
 CPLInfo.CheckResult   := FileSignCheck.CheckFile(CPLInfo.BinFile);
 try
  VersionInfo := TVersionInfo.Create(CPLInfo.BinFile);
  CPLInfo.Descr := VersionInfo.FileDescription;
  CPLInfo.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 SetLength(CPLInfoArray, Length(CPLInfoArray)+1);
 CPLInfoArray[Length(CPLInfoArray)-1] := CPLInfo;
end;


constructor TCPLList.Create;
begin
 CPLInfoArray := nil;
end;

destructor TCPLList.Destroy;
begin
 CPLInfoArray := nil;
 inherited;
end;

function TCPLList.DeleteItem(CPLInfo: TCPLInfo): boolean;
begin
 Result := AVZDeleteFile(CPLInfo.BinFile);
end;

function TCPLList.GetItemEnabledStatus(CPLInfo: TCPLInfo): integer;
begin
 Result := 1;
 if pos('disabledcpl', LowerCase(CPLInfo.BinFile)) > 0 then
  Result := 0;
end;

function TCPLList.RefresCPLList: boolean;
begin
 CPLInfoArray := nil;
 SearchCPLFiles(GetWindowsDirectoryPath, true);
 SearchCPLFiles(GetSystemDirectoryPath,  true);
 SearchCPLFiles(GetWindowsDirectoryPath+'DisabledCPL\', false);
end;

function TCPLList.SetItemEnabledStatus(var CPLInfo: TCPLInfo;
  NewStatus: boolean): boolean;
var
 S : string;
begin
 Result := false;
 // Лог. защита - блокирования активации активного и выключения неактивного
 if NewStatus      and  (GetItemEnabledStatus(CPLInfo) <> 0) then exit;
 if not(NewStatus) and  (GetItemEnabledStatus(CPLInfo) <> 1) then exit;
 // Блокировка
 if not(NewStatus) then begin
  S := ExtractFilePath(CPLInfo.BinFile) + 'DisabledCPL\' + ExtractFileName(CPLInfo.BinFile);
  zForceDirectories(ExtractFilePath(CPLInfo.BinFile) + 'DisabledCPL');
  MoveFile(PChar(CPLInfo.BinFile), PChar(S));
  CPLInfo.BinFile := S;
  Result := true;
 end;
 // Разблокировка
 if NewStatus then begin
  S := GetSystemDirectoryPath + ExtractFileName(CPLInfo.BinFile);
  MoveFile(PChar(CPLInfo.BinFile), PChar(S));
  CPLInfo.BinFile := S;
  Result := true;
 end;
end;

function TCPLList.SearchCPLFiles(APath: string; Status: boolean): boolean;
var
 SR : TSearchRec;
 Res : integer;
 Tmp : TCPLInfo;
begin
 APath := NormalDir(APath);
 Res := FindFirst(APath+'*.cpl', faAnyFile, SR);
 while Res = 0 do begin
  Tmp.BinFile := APath+SR.Name;
  Tmp.Name    := SR.Name;
  AddCPL(Tmp);
  Res := FindNext(SR);
 end;
 FindClose(SR);
end;

end.
