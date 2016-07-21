unit zAVZArcKernel;

interface
uses
  Windows, SysUtils, Classes, math,
  dllunit,
  zAntivirus,
  zSharedFunctions,
  activex,
  AbUnZper, AbUtils,
  AbCabTyp,
  AbGzTyp,
  AbTarTyp,
  zEmailUnpacker,
  zCHMUnpacker,
  UnRAR,
  zUnRar,
  zUtil,
  zUUE,
  zLogSystem;

const
 MailBombSize : integer = 10000000;
type
  // Описание архивного файла
  TAVZArcInfo = record
   MainArcName  : string;  // Имя главного архива
   ArcType      : string;  // Тип текущего архива
   AArcFileName : string;  // Имя файла в архиве
   TmpFileName  : string;  // Полное имя временного файла, ассоциированного с данным
  end;

  // Массив описаний
  TAVZArcInfoArray = array of TAVZArcInfo;

  // Событие "из архива извлечен файл"
  TOnAVZFileExtracted = function (AVZArcInfo : TAVZArcInfoArray) : boolean of object;
  // Событие "требуется проверка имени по фильтру"
  TOnAVZFileCheckName = function (AFileName : string) : boolean of object;
  // Событие "сообщение архиватора для внесения в протокол"
  TOnAVZExtractLog    = function (AMsg : string; InfoType : integer) : boolean of object;

  // Машинный код распаковки (библиотека)
  TUnpackCodeLib = record
   Name    : string;
   BinFile : array of byte;
   LibInfo : TLibInfo;
  end;
  // Коллекция единиц машинного кода для распаковки
  TUnpackCodeArray = array of TUnpackCodeLib;

  TAVZDecompress = class
  private
    FUnpackCode : TUnpackCodeArray;
    FTempPath : string;
    FDatabasePath : string;
    FPrefix   : string;
    FOnAVZFileExtracted: TOnAVZFileExtracted;
    FIterationLevel : integer;
    FOnAVZFileCheckName: TOnAVZFileCheckName;
    FOnAVZExtractLog: TOnAVZExtractLog;
    procedure SetTempPath(const Value: string);
    procedure OnPasswdNeed(Sender : TObject;  var NewPassword : string);
    function ZipDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray; IsSFX : boolean = false) : boolean;
    function ARJDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function CHMDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function ZipSFXDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function CabDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function GZDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function TARDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function RarDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray; IsSFX : boolean = false) : boolean;
    function PEJoinerDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray; Rel : dword) : boolean;
    function EmailDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function Base64Decompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function OLEDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray; IsSFX : boolean = false) : boolean;
    procedure SetOnAVZFileExtracted(const Value: TOnAVZFileExtracted);
    function  DoOnAVZFileExtracted(AArcName, AArcType, AArcFileName, ATmpFileName : string; AVZArcInfo : TAVZArcInfoArray) : boolean;
    function  DoCheckFilename(AFileName : string) : boolean;
    function  DoAddToLog(AMSG : string; InfoType : integer) : boolean;
    procedure SetOnAVZFileCheckName(const Value: TOnAVZFileCheckName);
    procedure SetOnAVZExtractLog(const Value: TOnAVZExtractLog);
    procedure DeleteTempArcFile(AFileName : string);
  public
   StopF,                                   // Признак остановки
   DebugLogMode,                            // Признак "отладочный режим"
   DetectMailBomb : boolean;                // Признак "детектировать почтовые бомбы"
   DefaultPass : string;
   ArchiveError : boolean;
   constructor Create(ABasePath : string);
   destructor  Destroy; override;
   // Проверка файла
   function ScanFile(AFileName : string; FileScanner : TFileScanner; AVZArcInfo : TAVZArcInfoArray) : boolean;
   // Удаление временных файлов
   function DelTempFiles : boolean;
   // Очистить базу
   Function ClearDatabase : boolean;
   Function LoadBinDatabase : boolean;
   // Загрузить бинарную базу данных
   Function LoadBinDatabaseFile(AFileName : string; AMainBase : boolean) : boolean;
   // Добавление ошибки к протоколу
   procedure AddError(s : string);
  published
   // Путь к временной папке
   property TempPath : string read FTempPath write SetTempPath;
   // **** События ****
   // Извлечен файл
   property OnAVZFileExtracted : TOnAVZFileExtracted read FOnAVZFileExtracted write SetOnAVZFileExtracted;
   // Проверка имени (для фильтров)
   property OnAVZFileCheckName : TOnAVZFileCheckName read FOnAVZFileCheckName write SetOnAVZFileCheckName;
   // Событие "запись в протокол извления"
   property OnAVZExtractLog    : TOnAVZExtractLog read FOnAVZExtractLog write SetOnAVZExtractLog;
  end;

  // Форматирование имени файла (с полной цепочкой имен архивов)
  function FormatArcFileName(AVZArcInfo : TAVZArcInfoArray) : string;
implementation
{ TAVZDecompress }

function StrStartFrom(S1, S : string) : boolean;
var
 i : integer;
begin
 Result := false;
 if Length(S1) > Length(S) then exit;
 for i := 1 to Length(S1) do
  if s[i] <> s1[i] then exit;
 Result := true;
end;

function TAVZDecompress.ScanFile(AFileName: string;
  FileScanner : TFileScanner; AVZArcInfo : TAVZArcInfoArray): boolean;
var
 s : string;
 i : integer;
begin
 ArchiveError := false;
 try
 if FileScanner.PEFileInfo.IsEXEFile then begin
  // RAR SFX DOS архив
  if (FileScanner.Buf1[28] =  52) and
     (FileScanner.Buf1[1] =  53)  and
     (FileScanner.Buf1[2] =  46)  and
     (FileScanner.Buf1[3] =  58)  then
      RARDecompress(AFileName, AVZArcInfo, true);
  end;
 if FileScanner.PEFileInfo.IsPEFile then begin
  // RAR SFX архив
  if (FileScanner.Buf6[0] =  byte('R')) and
     (FileScanner.Buf6[1] =  byte('a')) and
     (FileScanner.Buf6[2] =  byte('r')) and
     (FileScanner.Buf6[3] =  byte('!')) then
      RARDecompress(AFileName, AVZArcInfo, true);
  // ZIP SFX архив
  if (FileScanner.Buf6[0] =  byte('P')) and
     (FileScanner.Buf6[1] =  byte('K')) and
     (FileScanner.Buf6[2] =  3) and
     (FileScanner.Buf6[3] =  4) then begin
     ZipDecompress(AFileName, AVZArcInfo, true);
     end;
  // Joiner архив
  if (FileScanner.PEFileInfo.PE_HeaderSize < FileScanner.PEFileInfo.Size-200) and (FileScanner.PEFileInfo.PE_HeaderSize > 0) then begin
   for i := 0 to 128 do
    if (FileScanner.Buf6[i] =  byte('M')) and
       (FileScanner.Buf6[i+1] =  byte('Z')) then
         PEJoinerDecompress(AFileName, AVZArcInfo, FileScanner.PEFileInfo.PE_HeaderSize + i);
  end;
 end;
 if FileScanner.PEFileInfo.IsEXEFile then exit;
 // ZIP архив
 if (FileScanner.Buf1[0] =  byte('P')) and
    (FileScanner.Buf1[1] =  byte('K')) and
    (FileScanner.Buf1[2] =  3) and
    (FileScanner.Buf1[3] =  4) then begin
     ZipDecompress(AFileName, AVZArcInfo);
     end;
 // ARJ архив
 if (FileScanner.Buf1[0] =  $060) and
    (FileScanner.Buf1[1] =  $0EA) then
     ArjDecompress(AFileName, AVZArcInfo);
 // CAB архив
 if (FileScanner.Buf1[0] =  byte('M')) and
    (FileScanner.Buf1[1] =  byte('S')) and
    (FileScanner.Buf1[2] =  byte('C')) and
    (FileScanner.Buf1[3] =  byte('F')) then
    CabDecompress(AFileName, AVZArcInfo);
 // GZIP архив
 if (FileScanner.Buf1[0] =  $01F) and
    (FileScanner.Buf1[1] =  $08B) then
    GzDecompress(AFileName, AVZArcInfo);
 // TAR архив c сигнатурой ustar
 if (FileScanner.Buf1[$101] =  byte('u')) and
    (FileScanner.Buf1[$102] =  byte('s')) and
    (FileScanner.Buf1[$103] =  byte('t')) and
    (FileScanner.Buf1[$104] =  byte('a')) and
    (FileScanner.Buf1[$105] =  byte('r')) then
    TarDecompress(AFileName, AVZArcInfo);
 // TAR архив c сигнатурой GNUtar
 if (FileScanner.Buf1[$101] =  byte('G')) and
    (FileScanner.Buf1[$102] =  byte('N')) and
    (FileScanner.Buf1[$103] =  byte('U')) and
    (FileScanner.Buf1[$104] =  byte('t')) and
    (FileScanner.Buf1[$105] =  byte('a')) and
    (FileScanner.Buf1[$105] =  byte('r')) then
    TarDecompress(AFileName, AVZArcInfo);
 // CHM архив
 if (FileScanner.Buf1[0] =  byte('I')) and
    (FileScanner.Buf1[1] =  byte('T')) and
    (FileScanner.Buf1[2] =  byte('S')) and
    (FileScanner.Buf1[3] =  byte('F')) then
     CHMDecompress(AFileName, AVZArcInfo);
 // RAR архив
 if (FileScanner.Buf1[0] =  byte('R')) and
    (FileScanner.Buf1[1] =  byte('a')) and
    (FileScanner.Buf1[2] =  byte('r')) and
    (FileScanner.Buf1[3] =  byte('!')) then
     RARDecompress(AFileName, AVZArcInfo);
 // OLE хранилище
  if (FileScanner.Buf1[0] = $D0) and
     (FileScanner.Buf1[1] = $CF) and
     (FileScanner.Buf1[2] = $11) and
     (FileScanner.Buf1[3] = $E0) and
     (FileScanner.Buf1[4] = $A1) and
     (FileScanner.Buf1[5] = $B1) then
      OLEDecompress(AFileName, AVZArcInfo);

 // Email архив
 S := copy(PChar(@FileScanner.Buf1[0]), 0, 14);
 if StrStartFrom('From', S) or
    StrStartFrom('>From', S) or
    StrStartFrom('For:', S) or
    StrStartFrom('Received:', S) or
    StrStartFrom('Reply-To:', S) or
    StrStartFrom('Return-Path:', S) or
    StrStartFrom('Return-path:', S) or
    StrStartFrom('Delivered-To:', S) or
    StrStartFrom('X-Symantec-', S) or
    StrStartFrom('X-Apparently-To:', S) or
    StrStartFrom('X-EVS', S) or
    StrStartFrom('ID:', S) or
    StrStartFrom('Date:', S) or
    StrStartFrom('X-UIDL:', S) then
     EmailDecompress(AFileName, AVZArcInfo);
 if StrStartFrom('begin', S) then
     Base64Decompress(AFileName, AVZArcInfo);
 Result := true;
 except
  Result := false;
  ArchiveError := true;
 end;
end;

procedure TAVZDecompress.SetTempPath(const Value: string);
begin
  FTempPath := Value;
end;

function TAVZDecompress.ZipDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray; IsSFX : boolean = false): boolean;
var
 Archive : TAbUnZipper;
 i, FileSize : integer;
 TempName, FTypeName : string;
 FS : TFileStream;
begin
 Archive := nil;
 Archive := TAbUnZipper.Create(nil);
 inc(FIterationLevel);
 try
   try
    try
     if DefaultPass <> '' then begin
      Archive.OnNeedPassword  := OnPasswdNeed;
      Archive.PasswordRetries := 1;
     end;
     Archive.ForceType := true;
     Archive.ArchiveType := atZip;
     Archive.FileName := AFileName;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     for i := 0 to Archive.Count - 1 do begin
      if StopF then exit;
      // Не проверять файлы нулевой длинны
      if (Archive.Items[i].UncompressedSize = 0) and (Archive.Items[i].CompressedSize = 0) then continue;
      // Проверка имени файла по фильтру
      if not(DoCheckFilename(Archive.Items[i].FileName)) then continue;
      // Защита от почтовых бомб
      if DetectMailBomb then
       if ((Archive.Items[i].UnCompressedSize / (Archive.Items[i].CompressedSize + 2) > 100) and (Archive.Items[i].UncompressedSize > MailBombSize))
        or (Archive.Items[i].UncompressedSize < 0) then begin
        DoAddToLog(FormatArcFileName(AVZArcInfo) + ' ' +Archive.Items[i].FileName+ ' MailBomb detected !', logAlert);
        Continue;
      end;
      FS := nil;
      try
       FS := TFileStream.Create(TempName, fmCreate);
       FS.Size := 0;
       FS.Seek(0,0);
       Archive.ExtractToStream(Archive.Items[i].FileName, FS);
       FileSize := FS.Size;
       FS.Free; FS := nil;
       if IsSFX then FTypeName := 'ZIP-SFX' else
        FTypeName := 'ZIP';
       if FileSize = Archive.Items[i].UncompressedSize then
        DoOnAVZFileExtracted(AFileName,FTypeName,Archive.Items[i].FileName, TempName, AVZArcInfo);
      finally
       FS.Free;
       DeleteTempArcFile(TempName);
      end;
     end;
    finally
     Archive.Free;
     Archive := nil;
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
   end;
 finally
  dec(FIterationLevel);
 end;
end;


function TAVZDecompress.CabDecompress(AFileName : string; AVZArcInfo : TAVZArcInfoArray): boolean;
var
 Archive : TABCabArchive;
 i : integer;
 ErrMsg, TempName, TekArcFile : string;
begin
 Archive := nil;
 inc(FIterationLevel);
 TekArcFile := '';
 try
   try
    try
     Archive := TABCabArchive.Create(AFileName, fmOpenRead);
     Archive.Load;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     // Пропуск многотомных архивов
     if (Archive.HasPrev) or (Archive.HasNext) then exit;
     // Цикл по файлам в архиве
     for i := 0 to Archive.Count - 1 do begin
      if StopF then exit;
      TekArcFile := '/' + Archive.Items[i].FileName;
      // Не проверять файлы нулевой длинны
      if (Archive.Items[i].UncompressedSize = 0) and (Archive.Items[i].CompressedSize = 0) then continue;
      if not(DoCheckFilename(Archive.Items[i].FileName)) then continue;
      // Защита от почтовых бомб
      if DetectMailBomb and (Archive.Items[i].UncompressedSize / (Archive.Items[i].UncompressedSize + 2) > 100) and (Archive.Items[i].UncompressedSize > MailBombSize) then begin
       DoAddToLog(Archive.Items[i].FileName+ ' MailBomb detected !', logAlert);
       Continue;
      end;
      if not(Archive.Items[i].IsEncrypted) and not(Archive.Items[i].PartialFile) and not(Archive.Items[i].Tagged) then begin
       Archive.ExtractAt(i, TempName);
       DoOnAVZFileExtracted(AFileName, 'CAB', Archive.Items[i].FileName, TempName, AVZArcInfo);
       DeleteTempArcFile(TempName);
      end;
     end;
    finally
     DeleteTempArcFile(TempName);
     Archive.Free; Archive := nil;
    end;
   except
    on e : exception do begin
     if DebugLogMode then ErrMsg := E.Message else ErrMsg := '$AVZ0680';
     DoAddToLog(AFileName+ TekArcFile+' '+ErrMsg, logError);
    end;
   end;
 finally
  if not(FileExists(TempName)) then
   dec(FIterationLevel);
  Archive.Free; Archive := nil;
 end;
end;

constructor TAVZDecompress.Create(ABasePath : string);
begin
 FPrefix := 'avz_'+IntToStr(GetCurrentProcessId);
 DefaultPass := '';
 FDatabasePath := ABasePath;
 FIterationLevel := 0;
 FOnAVZFileExtracted := nil;
 FOnAVZFileCheckName := nil;
 FOnAVZExtractLog    := nil;
 ArchiveError := false;
 StopF := false;
end;

procedure TAVZDecompress.SetOnAVZFileExtracted(
  const Value: TOnAVZFileExtracted);
begin
  FOnAVZFileExtracted := Value;
end;

function TAVZDecompress.DoOnAVZFileExtracted(AArcName, AArcType,
  AArcFileName, ATmpFileName : string; AVZArcInfo : TAVZArcInfoArray): boolean;
begin
 if Assigned(FOnAVZFileExtracted) then begin
  try
   SetLength(AVZArcInfo, Length(AVZArcInfo)+1);
   AVZArcInfo[Length(AVZArcInfo)-1].MainArcName  := AArcName;
   AVZArcInfo[Length(AVZArcInfo)-1].ArcType      := AArcType;
   AVZArcInfo[Length(AVZArcInfo)-1].AArcFileName := AArcFileName;
   AVZArcInfo[Length(AVZArcInfo)-1].TmpFileName  := ATmpFileName;
   Result := FOnAVZFileExtracted(AVZArcInfo)
  except
  end;
  SetLength(AVZArcInfo, Length(AVZArcInfo)-1);
 end else Result := false;
end;

function FormatArcFileName(AVZArcInfo : TAVZArcInfoArray) : string;
var
 i : integer;
begin
 Result := '';
 for i := 0 to Length(AVZArcInfo) - 1 do
  if i = 0 then
   Result := AVZArcInfo[i].MainArcName + '/{' + AVZArcInfo[i].ArcType + '}/' + AVZArcInfo[i].AArcFileName
  else
   Result := Result + '/{' + AVZArcInfo[i].ArcType + '}/' + AVZArcInfo[i].AArcFileName;
end;

function TAVZDecompress.DoCheckFilename(AFileName: string): boolean;
begin
 if AFileName = '' then begin
  Result := true;
  exit;
 end;
 if Assigned(FOnAVZFileCheckName) then
  Result := FOnAVZFileCheckName(AFileName)
   else Result := true;
end;

procedure TAVZDecompress.SetOnAVZFileCheckName(
  const Value: TOnAVZFileCheckName);
begin
  FOnAVZFileCheckName := Value;
end;

// Распаковка RAR архивов
function TAVZDecompress.RarDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray; IsSFX : boolean = false): boolean;
var
 Archive : TUnrarArchive;
 CurrFileName, TempName : string;
 Res : integer;
 HeaderData: RARHeaderDataEx;
 FArcType : string;
begin
 Archive := nil;
 if FIterationLevel > 70 then exit;
 inc(FIterationLevel);
 try
   try
    try
     Archive := TUnrarArchive.Create(AFileName);
     if not(Archive.OpenArchive) then exit;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     repeat
      if StopF then exit;
      // Чтение очередного заголовка
      Res := Archive.ReadHeader(HeaderData);
      if Res <> 0 then break;
      // Не проверять файлы нулевой длинны
      if (HeaderData.UnpSize = 0) then begin
       Archive.SkipFile;
       continue;
      end;
      CurrFileName := Trim(HeaderData.FileNameW);
      if not(DoCheckFilename(CurrFileName)) then  begin
       Archive.SkipFile;
       continue;
      end;
      // Защита от почтовых бомб
      if DetectMailBomb and (HeaderData.UnpSize / (HeaderData.PackSize + 1) > 100) and (HeaderData.UnpSize > MailBombSize) then begin
       DoAddToLog(CurrFileName + ' MailBomb detected !', logAlert);
       Archive.SkipFile;
       Continue;
      end;
      if IsSFX then
       FArcType := 'RAR-SFX' else
        FArcType := 'RAR';
      if Archive.ExtractFile(TempName) = 0 then
       DoOnAVZFileExtracted(AFileName, FArcType, CurrFileName, TempName, AVZArcInfo)
        else ArchiveError := true;
      DeleteTempArcFile(TempName);
     until false;
    finally
     DeleteTempArcFile(TempName);
     Archive.Free;
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
   end;
 finally
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.GZDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
var
 Archive : TAbGzipArchive;
 i : integer;
 TempName : string;
begin
 Archive := nil;
 inc(FIterationLevel);
 try
   try
    try
     Archive := TAbGzipArchive.Create(AFileName, fmOpenRead);
     Archive.Load;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     for i := 0 to Archive.Count - 1 do begin
      if StopF then exit;
      // Не проверять файлы нулевой длинны
      if (Archive.Items[i].UncompressedSize = 0) and (Archive.Items[i].CompressedSize = 0) then continue;
      if not(DoCheckFilename(Archive.Items[i].FileName)) then continue;
      // Защита от почтовых бомб
      if DetectMailBomb and (Archive.Items[i].UncompressedSize / (Archive.Items[i].UncompressedSize + 2) > 100) and (Archive.Items[i].UncompressedSize > MailBombSize) then begin
       DoAddToLog(Archive.Items[i].FileName+ ' MailBomb detected !', logAlert);
       Continue;
      end;
      Archive.ExtractAt(i, TempName);
      DoOnAVZFileExtracted(AFileName, 'GZIP', Archive.Items[i].FileName, TempName, AVZArcInfo);
     end;
    finally
     DeleteTempArcFile(TempName);
     Archive.Free;
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
   end;
 finally
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.TARDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
var
 Archive : TAbTarArchive;
 FS : TFileStream;
 i : integer;
 TempName : string;
begin
 Archive := nil;
 inc(FIterationLevel);
 try
   try
    try
     Archive := TAbTarArchive.Create(AFileName, fmOpenRead);
     Archive.Load;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     for i := 0 to Archive.Count - 1 do begin
      if StopF then exit;
      // Не проверять файлы нулевой длинны
      if (Archive.Items[i].UncompressedSize = 0) and (Archive.Items[i].CompressedSize = 0) then continue;
      if not(DoCheckFilename(Archive.Items[i].FileName)) then continue;
      // Защита от почтовых бомб
      if DetectMailBomb and (Archive.Items[i].UncompressedSize / (Archive.Items[i].UncompressedSize + 2) > 100) and (Archive.Items[i].UncompressedSize > MailBombSize) then begin
       DoAddToLog(Archive.Items[i].FileName+ ' MailBomb detected !', logAlert);
       Continue;
      end;
      FS := nil;
      try
       FS := TFileStream.Create(TempName, fmCreate);
       Archive.ExtractToStream(Archive.Items[i].FileName, FS);
       FS.Free;
       FS := nil;
       DoOnAVZFileExtracted(AFileName, 'TAR', Archive.Items[i].FileName, TempName, AVZArcInfo);
      finally
       FS.Free;
       DeleteTempArcFile(TempName);
      end;
     end;
    finally
     DeleteTempArcFile(TempName);
     Archive.Free;
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
   end;
 finally
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.ZipSFXDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
var
 Archive : TAbUnZipper;
 i : integer;
 TempName : string;
 FS : TFileStream;
begin
 Archive := nil;
 Archive := TAbUnZipper.Create(nil);
 inc(FIterationLevel);
 try
   try
    try
     Archive.ForceType := true;
     Archive.ArchiveType := atSelfExtZip;
     Archive.FileName := AFileName;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     for i := 0 to Archive.Count - 1 do begin
      if StopF then exit;
      // Не проверять файлы нулевой длинны
      if (Archive.Items[i].UncompressedSize = 0) and (Archive.Items[i].CompressedSize = 0) then continue;
      // Проверка имени файла по маске
      if not(DoCheckFilename(Archive.Items[i].FileName)) then continue;
      // Защита от почтовых бомб
      if (Archive.Items[i].CompressionRatio > 100) and (Archive.Items[i].UncompressedSize > MailBombSize) then begin
       DoAddToLog(Archive.Items[i].FileName+ ' MailBomb detected !', logAlert);
       Continue;
      end;
      FS := nil;
      try
       FS := TFileStream.Create(TempName, fmCreate);
       FS.Size := 0;
       FS.Seek(0,0);
       Archive.ExtractToStream(Archive.Items[i].FileName, FS);
       FS.Free;
       FS := nil;
       DoOnAVZFileExtracted(AFileName,'ZIP-SFX',Archive.Items[i].FileName, TempName, AVZArcInfo);
      finally
       FS.Free;
       DeleteTempArcFile(TempName);
      end;
     end;
    finally
     Archive.Free;
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
   end;
 finally
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.ARJDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
Begin
end;

function TAVZDecompress.CHMDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
var
 CHMUnpacker : TCHMUnpacker;
 FS          : TFileStream;
 TempName    : string;
 i           : integer;
begin
 CHMUnpacker := nil;
 CHMUnpacker := TCHMUnpacker.Create;
 inc(FIterationLevel);
 try
  CHMUnpacker.FileName := AFileName;
  TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
  // Парсинг
  if not(CHMUnpacker.OpenCHM) then exit;
  for i := 0 to length(CHMUnpacker.DirItems)-1 do
   if (copy(CHMUnpacker.DirItems[i].Name, 1, 2) <> '::') and (CHMUnpacker.DirItems[i].Length > 0) then begin
      // Защита от почтовых бомб
      if DetectMailBomb and (CHMUnpacker.DirItems[i].Length > MailBombSize*10) then begin
       DoAddToLog(CHMUnpacker.DirItems[i].Name+ ' MailBomb detected !', logAlert);
       Continue;
      end;
   FS := nil;
   try
    FS := TFileStream.Create(TempName, fmCreate);
    CHMUnpacker.ExtractToStream(FS, CHMUnpacker.DirItems[i]);
    FS.Free;  FS := nil;
    DoOnAVZFileExtracted(AFileName, 'CHM', CHMUnpacker.DirItems[i].Name, TempName, AVZArcInfo);
   finally
    FS.Free;
    FS := nil;
    DeleteTempArcFile(TempName);
   end;
  end;
 finally
  CHMUnpacker.Free;
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.EmailDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
var
 EmailUnpacker : TEmailParser;
 i             : integer;
 TempName      : string;
 FS            : TMemoryStream;
begin
 EmailUnpacker := TEmailParser.Create;
 inc(FIterationLevel);
 try
   try
     EmailUnpacker.FileName := AFileName;
     if not(EmailUnpacker.ParseEmail) then exit;
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     for i := 0 to EmailUnpacker.PartsCount - 1 do begin
      if StopF then exit;
      FS := nil;
      try
       FS := TMemoryStream.Create;
       FS.Size := 0;
       FS.Seek(0,0);
       EmailUnpacker.ExtractToStream(EmailUnpacker.MailParts[i], FS);
       FS.SaveToFile(TempName);
       FS.Free;
       FS := nil;
       if EmailUnpacker.MailParts[i].FileName = '' then
        EmailUnpacker.MailParts[i].FileName := 'Part'+IntToStr(i+1);
       DoOnAVZFileExtracted(AFileName, 'E-MAIL:'+EmailUnpacker.MailParts[i].Encoding, EmailUnpacker.MailParts[i].FileName,TempName, AVZArcInfo);
      finally
       FS.Free;
       DeleteTempArcFile(TempName);
      end;
     end;
    finally
     EmailUnpacker.Free;
    end;
 finally
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.DelTempFiles: boolean;
var
 i : integer;
begin
 try
  for i := 0 to 30 do
   DeleteTempArcFile(TempPath + FPrefix + '_'+IntToStr(i)+'.tmp');
 except end;
end;

function TAVZDecompress.DoAddToLog(AMSG: string; InfoType : integer): boolean;
begin
 if Assigned(FOnAVZExtractLog) then
  if DebugLogMode or (InfoType <> logError) then
   FOnAVZExtractLog(AMSG, InfoType);
end;

procedure TAVZDecompress.SetOnAVZExtractLog(const Value: TOnAVZExtractLog);
begin
  FOnAVZExtractLog := Value;
end;

function TAVZDecompress.ClearDatabase: boolean;
var
 i : integer;
begin
 for i := 0 to length(FUnpackCode) - 1 do begin
  FUnpackCode[i].BinFile := nil;
  if FUnpackCode[i].LibInfo.ImageBase <> nil then
   xFreeLibrary(FUnpackCode[i].LibInfo);
 end;
 FUnpackCode := nil;
 RAROpenArchiveEx := nil;
 RARCloseArchive  := nil;
 RARReadHeaderEx  := nil;
 RARProcessFile   := nil;
end;

// ******************** Загрузка базы с модулями распаковки ********************
function TAVZDecompress.LoadBinDatabaseFile(AFileName: string;
  AMainBase: boolean): boolean;
var
 MS    : TMemoryStream;               // Поток для работы файлом
 VirusBaseHeader : TVirusBaseHeader;  // Заголовок антивирусной базы
 i,  StartPosInBase : integer;
 Res : boolean;
 b  : byte;
 dw : dword;
begin
 Result := false;
 if not(FileExists(AFileName)) then exit;
 // Загрузка файла в память
 MS := TMemoryStream.Create;
 MS.LoadFromFile(AFileName);
 // Декодирование и проверка CRC
 Res := DecodeBinData(MS);
 if Not(Res) then begin
  AddError('AVZ-001 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0721');
  exit;
 end;
 MS.Seek(0,0);
 // Чтение заголовка
 MS.Read(VirusBaseHeader, sizeof(VirusBaseHeader));
 if VirusBaseHeader.Sign <> 'AVZ' then exit;
 // Проверка, поддерживается ли данная версия базы
 if VirusBaseHeader.MinAvzVersion > MaxFormatCode then begin
  AddError('AVZ-002 $AVZ1109 '+ExtractFileName(AFileName)+' $AVZ0477');
  exit;
 end;
 // Чтение данных
 StartPosInBase := length(FUnpackCode);
 // Добавление информации к таблице
 SetLength(FUnpackCode, length(FUnpackCode)+VirusBaseHeader.RecCount);
 for i := 1 to VirusBaseHeader.RecCount do begin
  // Загрузка имени
  MS.Read(b, 1);
  SetLength(FUnpackCode[StartPosInBase].Name, b);
  MS.Read(FUnpackCode[StartPosInBase].Name[1], b);
  FUnpackCode[StartPosInBase].Name := Trim(UpperCase(FUnpackCode[StartPosInBase].Name));
  // Загрузка библиотеки
  MS.Read(dw, 4);
  SetLength(FUnpackCode[StartPosInBase].BinFile, dw);
  MS.Read(FUnpackCode[StartPosInBase].BinFile[0], dw);
  // Перемещаемся на очередную запись
  inc(StartPosInBase);
 end;
 Result := true;
 MS.Free;
end;

procedure TAVZDecompress.AddError(s: string);
begin
 ;
end;

function TAVZDecompress.LoadBinDatabase: boolean;
var
 i : integer;
begin
 Result := false;
 ClearDatabase;
 Result := LoadBinDatabaseFile(NormalDir(FDatabasePath)+'extract.avz', true);
 for i := 0 to length(FUnpackCode) - 1 do begin
  if FUnpackCode[i].Name = 'UNRAR' then begin
   FUnpackCode[i].LibInfo := xLoadLibrary(@FUnpackCode[i].BinFile[0], RarImports);
   FUnpackCode[i].BinFile := nil;
  end;
 end;
end;

function TAVZDecompress.PEJoinerDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray; Rel : dword): boolean;
var
 TempName : string;
 FArcType : string;
 FS1, FS2 : TFileStream;
begin
 inc(FIterationLevel);
 FS1 := nil;
 FS2 := nil;
 try
  try
   try
    TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
    if StopF then exit;
    FArcType := 'EXE-Joiner';
    FS1 := TFileStream.Create(AFileName, fmOpenRead or fmShareDenyNone);
    FS2 := TFileStream.Create(TempName, fmCreate);
    FS1.Seek(Rel, 0);
    FS2.CopyFrom(FS1, FS1.Size - Rel);
    FS1.Free; FS1 := nil;
    FS2.Free; FS2 := nil;
    DoOnAVZFileExtracted(AFileName, FArcType, '.exe', TempName, AVZArcInfo);
    DeleteTempArcFile(TempName);
   finally
    FS1.Free; FS1 := nil;
    FS2.Free; FS2 := nil;
    DeleteTempArcFile(TempName);
   end;
 except
  on e : exception do
   DoAddToLog(AFileName+ ' '+E.Message, logError);
  end;
 finally
  dec(FIterationLevel);
 end;
end;

procedure TAVZDecompress.DeleteTempArcFile(AFileName: string);
begin
 if DeleteFile(AFileName) then exit;
 SetFileAttributes(PChar(AFileName), 0);
 if DeleteFile(AFileName) then exit;
end;

procedure TAVZDecompress.OnPasswdNeed(Sender: TObject;
  var NewPassword: string);
begin
 if DefaultPass <> '' then
  NewPassword := DefaultPass;
end;


function TAVZDecompress.OLEDecompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray; IsSFX: boolean): boolean;
var
 OLEStorage : IStorage;
 FileIndex : integer;
 TempName, FTypeName : string;
 WFileName : WideString;
 buf : array[0..16000] of byte;
 function ScanNode(Level : integer; OLEStorage: IStorage; APath : string): boolean;
  var
   Enm         : IEnumStatStg;
   statstg     : TStatStg;
   BytesReaded, Count : Longint;
   OLEStorage2 : IStorage;
   Stream      : IStream;
   FS          : TFileStream;
   S : string;
  begin
   if OLEStorage = nil then exit;
   OLEStorage.EnumElements(0, nil, 0, Enm);
   if Enm = nil then exit;
   while Enm.Next(1, statstg, @Count) = S_OK do begin
 //   Memo1.Lines.Add(APath+'\'+ statstg.pwcsName);
    if statstg.dwType = STGTY_STORAGE then begin
     OLEStorage.OpenStorage(statstg.pwcsName, nil,
                            STGM_READ or STGM_SHARE_EXCLUSIVE,
                            nil, 0, OLEStorage2);
     ScanNode(Level + 1, OLEStorage2, APath+'\'+statstg.pwcsName);
     OLEStorage2 := nil;
    end else begin
     if statstg.dwType = STGTY_STREAM then begin
      inc(FileIndex);
      OLEStorage.OpenStream(statstg.pwcsName, nil, STGM_READ or STGM_SHARE_EXCLUSIVE,
                            0, Stream);
      try
       FS := TFileStream.Create(TempName, fmCreate);
       FS.Size := 0;
       FS.Seek(0,0);
       repeat
        Stream.Read(@buf[0], sizeof(buf), @BytesReaded);
        if BytesReaded > 0 then
         FS.Write(buf[0], BytesReaded);
       until BytesReaded < sizeof(buf);
      finally
       FS.Free;
      end;
      Stream := nil;
      S := Trim(StringReplace(statstg.pwcsName, '?', '', [rfReplaceAll]));
      if S = '' then
       S := inttostr(FileIndex);
      DoOnAVZFileExtracted(AFileName, FTypeName, APath+'\'+S, TempName, AVZArcInfo);
     end;
    end;
   end;
  end;

begin
 inc(FIterationLevel);
 FileIndex := 0;
 try
   try
    try
     TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
     FTypeName := 'MS-OLE';
     WFileName := AFileName;
     StgOpenStorage(PWideChar(WFileName), nil,
                    STGM_READ or STGM_SHARE_EXCLUSIVE,
                    nil, 0, OLEStorage);
     ScanNode(0, OLEStorage, '');
     OLEStorage := nil;
    finally
     DeleteTempArcFile(TempName);
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
   end;
 finally
  dec(FIterationLevel);
 end;
end;

function TAVZDecompress.Base64Decompress(AFileName: string;
  AVZArcInfo: TAVZArcInfoArray): boolean;
var
 TempName, FFileName, S : string;
 MS     : TMemoryStream;
begin
 inc(FIterationLevel);
 MS   := TMemoryStream.Create;
 try
   try
    TempName := TempPath + FPrefix + '_'+IntToStr(FIterationLevel)+'.tmp';
    try
     MS.LoadFromFile(AFileName);
     MS.Seek(0,0);
     SetLength(S, MS.Size);
     MS.Read(S[1], length(S));
     FFileName := copy(S, 11, min(255, pos(#$0D, S)-11));
     S := DeCodeStringUUEEx(S);
     MS.Clear;
     MS.Write(S[1], Length(S));
     MS.SaveToFile(TempName);
     DoOnAVZFileExtracted(AFileName, 'MIME', FFileName,TempName, AVZArcInfo);
    finally
     DeleteTempArcFile(TempName);
    end;
   except
    on e : exception do
     DoAddToLog(AFileName+ ' '+E.Message, logError);
    end;
 finally
  dec(FIterationLevel);
 end;
 MS.Free;
end;

destructor TAVZDecompress.Destroy;
var
 i : integer;
begin
 for i := 0 to Length(FUnpackCode)-1 do
  FUnpackCode[i].BinFile := nil; 
 FUnpackCode := nil;
 inherited;
end;

end.
