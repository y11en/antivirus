library avz_krnl;

uses
  SysUtils,
  Windows,
  Messages,
  Dialogs,
  ActiveX,
  Classes,
  zAntivirus,
  zAVZArcKernel,
  zutil,
  orautil;

{$R *.res}

const
  AVC_ScanError        =  3; // Ошибка распаковки ил анализа
  AVC_Suspicious       =  2; // Подозрительный
  AVC_VirusFound       =  1; // Вирус
  AVC_OK               =  0; // Успешное выполнение
  AVC_Error            = -1; // Ошибка
  cMaxMsgSize          = 1024; // Максимальный размер сообщения

type
 TAVZAScanKernel = class
 private
  Loaded     : Boolean;
  AVBasePath : string;
  // Антивирус
  MainAntivirus   : TAntivirus;
  // Распаковщик архивов
  AVZDecompress : TAVZDecompress;
  // Имена найденных вирусов
  VirusName       : string;
  // Результат сканирования
  FullResult      : integer;
  function OnAVZFileExtracted(AVZArcInfo : TAVZArcInfoArray): boolean;
 public
  constructor Create;
  destructor  Destroy; override;
  function LoadAVBase : boolean;
  // Сканирование файла на вирусы
  function VirusScan(AFileName: string; ArcInfo : TAVZArcInfoArray): integer;
 end;

var
 AVZAScanKernel : TAVZAScanKernel = nil; // Ядро сканера
 CopyUnpackedFiles : boolean = false;
 UnpackedPath : string;

function MakeStr(ABuf: PChar; ABufSize: Integer; const S: String): Integer;
begin
  if (ABuf = nil) or (ABufSize <= 0) then Result := Length(S) else
    if ABufSize < Length(S) then
    begin
      Result := ABufSize;
      StrLCopy(ABuf, PChar(S), ABufSize);
    end else
    begin
      Result := Length(S);
      StrCopy(ABuf, PChar(S));
    end;
end;

function GetModuleFileNameStr(Instance: THandle): String;
var
 buffer : array [0..MAX_PATH] of Char;
 len    : integer;
begin
 len := GetModuleFileName(Instance, buffer, MAX_PATH);
 Result := copy(buffer, 1, len);
end;

function AVZ_Initialize: Integer; stdcall;
begin
 AVZAScanKernel := TAVZAScanKernel.Create;
 // Загрузка базы
 if AVZAScanKernel.LoadAVBase then
  Result := AVC_OK
 else begin
  Result := AVC_Error;
 end;
end;

function AVZ_InitializeEx(ADbPath : PChar): Integer; stdcall;
begin
 AVZAScanKernel := TAVZAScanKernel.Create;
 AVZAScanKernel.AVBasePath := Trim(ADbPath);
 // Загрузка базы
 if AVZAScanKernel.LoadAVBase then
  Result := AVC_OK
 else begin
  Result := AVC_Error;
 end;
end;

function AVZ_EnableUnpack(AUnpackPath : PChar): Integer; stdcall;
begin
 CopyUnpackedFiles := true;
 UnpackedPath := Trim(NormalDir(AUnpackPath));
end;

function AVZ_DisableUnpack : Integer; stdcall;
begin
 CopyUnpackedFiles := false;
end;

function AVZ_ReloadBase: Integer; stdcall;
begin
 // Загрузка базы
 if AVZAScanKernel.LoadAVBase then
  Result := AVC_OK
 else begin
  Result := AVC_Error;
 end;
end;

function AVZ_CheckFile(AFileName: PChar; Msg: PChar): Integer; stdcall;
var
 FileName : string;
 CriticalSection: TRTLCriticalSection;
begin
 FileName := Trim(AFileName);
 Result := AVC_OK;
 AVZAScanKernel.VirusName := '';
 AVZAScanKernel.FullResult := 0;
 try
  AVZAScanKernel.VirusScan(FileName, nil);
 except
  on e: exception do
   AVZAScanKernel.FullResult := 3;
 end;
 // Проверка файла
 case AVZAScanKernel.FullResult of
  0 : Result := AVC_OK;
  1 : begin
       Result := AVC_VirusFound;
       MakeStr(Msg, cMaxMsgSize, AVZAScanKernel.VirusName);
      end;
  2 : begin
       Result := AVC_Suspicious;
       MakeStr(Msg, cMaxMsgSize, AVZAScanKernel.VirusName);
      end;
  3 : begin
       Result := AVC_ScanError;
      end;
 end;
end;

// Note that function names are case-sensitive!
exports
  AVZ_Initialize,
  AVZ_InitializeEx,
  AVZ_CheckFile,
  AVZ_ReloadBase,
  AVZ_EnableUnpack,
  AVZ_DisableUnpack;

{ TAVZAScanKernel }

constructor TAVZAScanKernel.Create;
begin
 Loaded := false;
 AVBasePath := NormalDir(ExtractFilePath(GetModuleFileNameStr(HInstance)))+'BASE\';
end;

destructor TAVZAScanKernel.Destroy;
begin
 MainAntivirus.Free;
 AVZDecompress.Free;
 inherited;
end;

function TAVZAScanKernel.VirusScan(AFileName: string; ArcInfo : TAVZArcInfoArray): integer;
var
 FileScanner     : TFileScanner;
 VirusDescr      : TVirusDescr;
 DeleteModeCode  : integer;
 S : string;
 i : integer;
 Function LogFileName : string;
 begin
  Result := AFileName;
  if ArcInfo <> nil then
   Result := FormatArcFileName(ArcInfo);
 end;
 Function GetBaseFileName : string;
 begin
  Result := AFileName;
  if ArcInfo <> nil then
   Result := ArcInfo[0].MainArcName;
 end;
begin
 Result := -1;
 FileScanner := TFileScanner.Create;
 try
  // Проверка файла
  FileScanner.CheckFile(AFileName);
  // Анализ результата
  Result := MainAntivirus.CheckFile(FileScanner.PEFileInfo, VirusDescr);
  case Result of
   0 : begin
       end;
   1 : begin
        AddToList(VirusName, MainAntivirus.VirusName);
        FullResult := 1;
       end;
   2 : begin
        AddToList(VirusName, 'Suspictios: '+MainAntivirus.VirusName);
        if FullResult = 0 then FullResult := 2;
       end;
  end;
  // Это архив ??
  try
   AVZDecompress.ScanFile(AFileName, FileScanner, ArcInfo);
   if AVZDecompress.ArchiveError then
    if FullResult = 0 then FullResult := 3;
  except
   on e : exception do
    if FullResult = 0 then FullResult := 3;
  end;
 finally
  FileScanner.Free;
 end;
end;

function TAVZAScanKernel.OnAVZFileExtracted(AVZArcInfo : TAVZArcInfoArray): boolean;
var
 S, ST : string;
 Tmp : TAVZArcInfo;
 i : integer;
begin
 if (AVZArcInfo = nil) or (Length(AVZArcInfo) = 0) then exit;
 Tmp := AVZArcInfo[Length(AVZArcInfo)-1];
 VirusScan(Tmp.TmpFileName, AVZArcInfo);

 if CopyUnpackedFiles then begin
   i := -1;
   repeat
    inc(i);
    ST := AVZArcInfo[Length(AVZArcInfo)-1].ArcType;
    if pos(':', ST) > 0 then        //#DNL
     Delete(ST, pos(':', ST), 100); //#DNL
    S := UnpackedPath+ST+'\'; //#DNL
    S := S + ExtractFileName(AVZArcInfo[Length(AVZArcInfo)-1].MainArcName);
    if i > 0 then
     S := S + IntToStr(i);
    ST := AVZArcInfo[Length(AVZArcInfo)-1].AArcFileName;
    ST := StringReplace(ST, '/','\', [rfReplaceAll]); //#DNL
    if copy(ST, 1, 1) = '\' then delete(ST, 1, 1);
    S := NormalDir(S) + ST;
   until not FileExists(S);
   ForceDirectories(ExtractFilePath(S));
   CopyFile(PChar(Tmp.TmpFileName), PChar(S), false);
 end;
end;

function TAVZAScanKernel.LoadAVBase : boolean;
begin
 Result := false;
 try
  // Создание класса "Антивирус"
  MainAntivirus   := TAntivirus.Create(AVBasePath);
  // Загрузка баз
  MainAntivirus.FDatabasePath := AVBasePath;
  Result := MainAntivirus.LoadBinDatabase;
  // Декомпрессор
  AVZDecompress := TAVZDecompress.Create(AVBasePath);
  AVZDecompress.TempPath := GetTempDirectoryPath;
  AVZDecompress.OnAVZFileExtracted := OnAVZFileExtracted;
  if not(AVZDecompress.LoadBinDatabase) then Result := false;
  Loaded := Result;
 except
  on e : Exception do
   VirusName := e.Message;
 end;
end;

begin
end.
