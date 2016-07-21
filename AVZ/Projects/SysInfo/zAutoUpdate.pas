unit zAutoUpdate;

interface
uses  Windows, Messages, SysUtils, Variants, Classes,
      FileLoader, zXML, md5, AbUnZper;
type
  // Описание файла
 TFileDescr = record
  Name    : string;      // Имя файла
  Size    : integer;     // Размер файла
  MD5     : TMD5Digest;  // MD5 сумма
  MD5Txt  : string;      // MD5 сумма в текстовом виде
 end;

 // Шаг процесса обновления
 TOnStep = procedure(AStep : integer) of object;
 // Прогресс операции с файлами
 TOnFileOperation = procedure(AFileName : string; ATek, AMax : integer) of object;
 OnAnalizComplete = procedure(DownloadSize : integer) of object;
 TAVZAutoUpdater = class
  private
   FOnStep: TOnStep;
   FileLoader : TCustomFileLoader;
   FOnFileOperation: TOnFileOperation;
    FOnAnalizComplete: OnAnalizComplete;
   procedure SetOnStep(const Value: TOnStep);
   function ZCopyFile(AFileName1, AFileName2 : string) : boolean;
   procedure AddError(ACode : integer; S : string);
   procedure AddAlarm(S : string);
   procedure DoOnStep(AStep : integer);
   procedure SetOnFileOperation(const Value: TOnFileOperation);
   procedure DoSetOnFileOperation(AFileName : string; ATek, AMax : integer);
   procedure DoOnAnalizComplete(DownloadSize : integer);
   procedure FileCopyProgress(Sender : TObject; ASize, AFullSize : integer);
   procedure SetOnAnalizComplete(const Value: OnAnalizComplete);
 public
  ServerPath,             // Путь к серверу
  DestPath,               // Размещение файлов
  TempPath : string;      // Временная папка
  HTTPDownloadMode : THTTPDownloadMode;
  ProxyServer, ProxyUser, ProxyPass : string;
  LastError   : string;     // Текст последней ошибки
  LastAlarm   : string;     // Текст последнего предупреждения
  TekFileName : string;   // Имя текущего файла
  StopF       : boolean;  // Признак остановки
  LocalVer    : Double;   // Локальная версия для контроля
  CurrentVer  : Double;   // Локальная версия для контроля
  ResStatus   : integer;  // Статус (0-ошибка, 1-обновления не требуются, 2-обновления загружены и установлены)
  Constructor Create;
  // Получение данных о файле на диске
  Function GetFileInfo(AFileName : string; var FileDescr : TFileDescr) : boolean;
  // Выполнение процесса обновления по настройкам
  Function Execute : boolean;
 published
  property OnStep : TOnStep read FOnStep write SetOnStep;
  property OnFileOperation : TOnFileOperation read FOnFileOperation write SetOnFileOperation;
  property OnAnalizComplete : OnAnalizComplete read FOnAnalizComplete write SetOnAnalizComplete;
 end;

function CryptPwd(S : string) : string;
function DeCryptPwd(S : string) : string;
implementation
uses zutil, orautil;

function CryptPwd(S : string) : string;
var
 i : integer;
 b : byte;
begin
 Result := '';
 for i := 1 to 64 do begin
  if i <= length(S) then b := byte(S[i])
   else b := 0;
  b := not(b xor byte(i*153+80));
  Result := Result + IntToHex(b, 2);
 end;
end;

function DeCryptPwd(S : string) : string;
var
 i : integer;
 b : byte;
 ST : string;
begin
 Result := '';
 S := Trim(S);
 if Length(S) <> 128 then begin
  Result := S;
  exit;
 end;
 for i := 1 to 64 do begin
  ST := copy(S, 1, 2); Delete(S, 1, 2);
  b := StrToIntDef('$'+ST, 0);
  b := not(b) xor byte(i*153+80);
  if B = 0 then exit;
  Result := Result + chr(b);
 end;
end;

{ TAVZAutoUpdater }

procedure TAVZAutoUpdater.AddAlarm(S: string);
begin
 AddToList(LastAlarm, S);
end;

procedure TAVZAutoUpdater.AddError(ACode: integer; S: string);
begin
 AddToList(LastError, S);
end;

constructor TAVZAutoUpdater.Create;
begin
 FOnStep := nil;
 FOnFileOperation := nil;
 inherited;
end;

procedure TAVZAutoUpdater.DoOnAnalizComplete(DownloadSize : integer);
begin
 if Assigned(FOnAnalizComplete) then
  FOnAnalizComplete(DownloadSize);
end;

procedure TAVZAutoUpdater.DoOnStep(AStep: integer);
begin
 if Assigned(FOnStep) then
  FOnStep(AStep);
end;

procedure TAVZAutoUpdater.DoSetOnFileOperation(AFileName: string; ATek,
  AMax: integer);
begin
 if Assigned(FOnFileOperation) then
  FOnFileOperation(AFileName, ATek, AMax);
end;

function TAVZAutoUpdater.Execute: boolean;
var
 xml : TXMLParser;
 i, FFileSize, DownloadSize : integer;
 ZipArchive : TAbUnZipper;
 XMLTxt     : string;
 MS         : TMemoryStream;
 BaseTag    : TXMLToken;
 FileName, ServerFileName, MD5str : string;
 FileDescr  : TFileDescr;
 UpdateList : TList;
 MinVer     : Double;
begin
 Result := false;
 ResStatus := 0;
 StopF  := false;
 DoOnStep(1);
 LastError := '';
 ZipArchive := nil; xml        := nil; MS         := nil; UpdateList := nil;
 xml := TXMLParser.Create;
 FileLoader := THTTPFileLoader.Create;
 FileLoader.OnFileCopyProgress := FileCopyProgress;
 (FileLoader as THTTPFileLoader).DownloadMode  := HTTPDownloadMode;
 (FileLoader as THTTPFileLoader).ProxyServer   := ProxyServer;
 (FileLoader as THTTPFileLoader).ProxyServerLogin   := ProxyUser;
 (FileLoader as THTTPFileLoader).ProxyServerPasswd  := ProxyPass;
 UpdateList := TList.Create;
 TekFileName    := 'avzupd.zip';
 try
  // Загрузка описания процесса обновления - файл avzupd.zip
  zForceDirectories(TempPath);
  if not(FileLoader.LoadFile(ServerPath + 'avzupd.zip', TempPath+'avzupd.zip', 2000)) then begin
   AddError(1, '$AVZ0659 '+ServerPath+' ['+inttostr(FileLoader.LoadStep )+', '+IntToHex(FileLoader.GetLastErrorCode, 8)+']');
   exit;
  end;
  DoOnStep(2);
  // Описание загружено - извлечение XML файла
  ZipArchive := TAbUnZipper.Create(nil);
  MS := TMemoryStream.Create;
  try
   ZipArchive.FileName := TempPath+'avzupd.zip';
   ZipArchive.ExtractToStream('file.dta', MS);
  except
   on e : exception do begin
    AddError(2, '$AVZ0658');
    exit;
   end;
  end;
  MS.Seek(0,0);
  SetLength(XMLTxt, MS.Size);
  MS.Read(XMLTxt[1], MS.Size);
  MS.Free; MS := nil;
  ZipArchive.Free; ZipArchive := nil;
  // Парсинг извлеченного из архива XML
  xml.ParseXML(XMLTxt);
  // Анализ XML
  // Поиск тега со списком файлов
  BaseTag := XML.SearchToken('AVZ');
  if BaseTag <> nil then begin
   MinVer     := BaseTag.ReadFloat('MinVer', 0);
   CurrentVer := BaseTag.ReadFloat('CurVer', 0);
   if (MinVer <> 0) and (LocalVer <> 0 ) and (MinVer > LocalVer) then begin
    AddError(7, '$AVZ1166 $AVZ1107 '+FormatFloat('0.00', MinVer)+', $AVZ0543');
    exit;
   end;
   if (CurrentVer <> 0) and (LocalVer <> 0 ) and (CurrentVer > LocalVer) then begin
    AddAlarm('$AVZ1177 '+FormatFloat('0.00', CurrentVer)+' $AVZ1167 z-oleg.com/secur/');
   end;
  end;
  BaseTag := XML.SearchToken('AVZ\BASE');
  UpdateList.Clear;
  if BaseTag = nil then begin
   AddError(3, '$AVZ0250');
   exit;
  end;
  DoOnStep(3);
  // Анализ файлов и построение списка загружаемых файлов
  for i := 0 to BaseTag.SubNodes.Count - 1 do
   if BaseTag.SubNodes[i].UpperTokenName = 'FILE' then begin
    FileName := BaseTag.SubNodes[i].ReadString('Name','');
    MD5str   := Trim(UpperCase(BaseTag.SubNodes[i].ReadString('MD5', '')));
    TekFileName    := FileName;
    if FileName = '' then begin
     AddError(3, '$AVZ0250');
     exit;
    end;
    if not(GetFileInfo(DestPath + 'Base\' + FileName, FileDescr)) then UpdateList.Add(BaseTag.SubNodes[i])
     else if Trim(UpperCase(FileDescr.MD5Txt)) <> MD5str then
      UpdateList.Add(BaseTag.SubNodes[i]);
   end;
  // Список построен - если он пустой, то обновлять нечего
  if UpdateList.Count = 0 then begin
   Result := true;
   ResStatus := 1;
   DoOnStep(4);
   exit;
  end;
  // Вычисление размера загружаемых файлов
  DownloadSize := 0;
  for i := 0 to UpdateList.Count - 1 do begin
   DownloadSize := DownloadSize  + TXMLToken(UpdateList[i]).ReadInteger('Size', 0);
  end;
  DoOnAnalizComplete(DownloadSize);
  // Загрузка файлов
  for i := 0 to UpdateList.Count - 1 do begin
   FileName       := TXMLToken(UpdateList[i]).ReadString('Name', '');
   ServerFileName := TXMLToken(UpdateList[i]).ReadString('ServerName', '');
   MD5str         := TXMLToken(UpdateList[i]).ReadString('MD5', '');
   FFileSize      := TXMLToken(UpdateList[i]).ReadInteger('Size', 0);
   TekFileName    := FileName;
   if ServerFileName = '' then ServerFileName := FileName;
   // Загрузка не требуется, т.к. файл уже есть
   if GetFileInfo(TempPath + FileName, FileDescr) and (UpperCase(FileDescr.MD5Txt) = UpperCase(MD5str)) then begin
    Continue;
   end;
   // Загрузка требуется
   if not(FileLoader.LoadFile(ServerPath + ServerFileName, TempPath+FileName, FFileSize)) then begin
    AddError(4, '$AVZ0645 '+FileName);
    exit;
   end;
   // Проверка файла после загрузки - если ошибка, то выход
   if not(GetFileInfo(TempPath + FileName, FileDescr) and (UpperCase(FileDescr.MD5Txt) = UpperCase(MD5str))) then begin
    AddError(5, '$AVZ0252 - '+FileName);
    exit;
   end;
  end;
  // Загрузка завершена - инсталляция с контролем
  DoOnStep(4);
  // Загрузка файлов
  for i := 0 to UpdateList.Count - 1 do begin
   FileName       := TXMLToken(UpdateList[i]).ReadString('Name', '');
   MD5str         := TXMLToken(UpdateList[i]).ReadString('MD5', '');
   TekFileName    := FileName;
   // Проверка файла - если ошибка, то выход
   if not(GetFileInfo(TempPath + FileName, FileDescr) and (UpperCase(FileDescr.MD5Txt) = UpperCase(MD5str))) then begin
    AddError(6, '$AVZ0251 - '+FileName);
    exit;
   end;
   // Установка файла
   zForceDirectories(DestPath + 'Base\');
   ZCopyFile(TempPath + FileName, DestPath + 'Base\' + FileName);
   DeleteFile(TempPath + FileName);
  end;
 finally
  DeleteFile(TempPath+'avzupd.zip');
  RemoveDir(TempPath);
  FileLoader.Free;
 end;
 ResStatus := 2;
 Result := true;
end;

procedure TAVZAutoUpdater.FileCopyProgress(Sender: TObject; ASize,
  AFullSize: integer);
begin
 DoSetOnFileOperation(TekFileName, ASize, AFullSize);
end;

function TAVZAutoUpdater.GetFileInfo(AFileName: string;
  var FileDescr: TFileDescr): boolean;
var
 FS : TFileStream;
begin
 Result := false;
 FileDescr.Name := '';   FileDescr.Size := 0;  FileDescr.MD5Txt := '';
 // Проверка наличия файла
 if Not(FileExists(AFileName)) then exit;
 try
  // Открытие файла
  FS := TFileStream.Create(AFileName, fmOpenRead or fmShareDenyNone);
  // Имя
  FileDescr.Name := AFileName;
  // Размер
  FileDescr.Size := FS.Size;
  // MD5
  FileDescr.MD5  := MD5Stream(FS);
  FileDescr.MD5Txt  := MD5DigestToStr(FileDescr.MD5);
  FS.Free;
  Result := true;
 except end;
end;

procedure TAVZAutoUpdater.SetOnAnalizComplete(
  const Value: OnAnalizComplete);
begin
  FOnAnalizComplete := Value;
end;

procedure TAVZAutoUpdater.SetOnFileOperation(
  const Value: TOnFileOperation);
begin
  FOnFileOperation := Value;
end;

procedure TAVZAutoUpdater.SetOnStep(const Value: TOnStep);
begin
  FOnStep := Value;
end;

function TAVZAutoUpdater.ZCopyFile(AFileName1,
  AFileName2: string): boolean;
begin
 CopyFile(PChar(AFileName1), PChar(AFileName2), false);
end;

end.
