library avz_thebat;

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
  orautil,
  Registry,
  uSetupDlg in 'uSetupDlg.pas' {SetupDlg},
  avz_options in 'avz_options.pas';

{$E BAV}

{$R *.res}

const
  AVC_Suspicious       =  2; // Подозрительный
  AVC_VirusFound       =  1; // Вирус
  AVC_OK               =  0; // Успешное выполнение
  AVC_NotSupported     = -1; // Функция не поддерживается
  AVC_CantBeCured      = -2; // Не может быть вылечен
  AVC_CantCure         = -2;
  AVC_Error            = -3; // Ошибка
  AVC_BufTooSmall      = -4; // Буфер слишком мал

  cMaxMsgSize          = 1024; // Максимальный размер сообщения

var
  KernelVersion   : string = 'AVZ Mail checker, 06.03.2007, (C) Zaitsev Oleg, http://www.z-oleg.com/secur';
  LogFileName     : string = '';
  EnableLog       : boolean = false;
  LogMalware      : boolean = false;

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

// Подготовка PChar строки
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

// Протоколирование
procedure AddToLog(S : string);
var
 f     : TextFile;
begin
 if not(EnableLog) then exit;
 AssignFile(f, LogFileName);
 try
  // Если файл существует - дозапись, не существует - создание
  if FileExists(LogFileName) then Append(f)
   else Rewrite(f);
 except
  exit; // При ошибках немедленный выход, дабы не усугублять положение программы
 end;
 try
  Writeln(f, DateTimetoStr(Now)+' : '+s);
 finally
  CloseFile(f); // в конце обязательно закроем файл
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

procedure SetPluginSetup;
begin
 AVZAScanKernel.AVBasePath := OptionsList.Values['BasePath'];
 EnableLog                 := OptionsList.Values['EnableLog'] = '1';
 LogMalware                := OptionsList.Values['LogMalware'] = '1';
end;

procedure LoadPluginSetup;
var
 Reg : TRegistry;
 S   : string;
begin
 AddToLog('Загрузка настроек плагина');
 Reg := TRegistry.Create;
 S := '';
 try
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  Reg.OpenKey('Software\AVZ\TheBatPlugin', false);
  S := Trim(Reg.ReadString('BasePath'));
 except
  S := '';
 end;
 try OptionsList.Values['BasePathMode'] := Reg.ReadString('BasePathMode'); except end;
 try OptionsList.Values['EnableLog']    := Trim(Reg.ReadString('EnableLog')); except OptionsList.Values['EnableLog'] := '0'; end;
 try OptionsList.Values['LogMalware']    := Trim(Reg.ReadString('LogMalware')); except OptionsList.Values['LogMalware'] := '0'; end;
 if (S = '') or (StrToIntDef(OptionsList.Values['BasePathMode'], 0) = 0) then
  S := NormalDir(ExtractFilePath(GetModuleFileNameStr(HInstance)))+'BASE\'
 else
  S := NormalDir(S);
 OptionsList.Values['BasePath'] := S;
 SetPluginSetup;
 AddToLog('Настройки загружены. Путь к AV базам: "'+AVZAScanKernel.AVBasePath+'"');
end;

procedure SavePluginSetup;
var
 Reg : TRegistry;
begin
 Reg := TRegistry.Create;
 try
  Reg.RootKey := HKEY_LOCAL_MACHINE;
  if Reg.OpenKey('Software\AVZ\TheBatPlugin', true) then begin
   Reg.WriteString('BasePath',     OptionsList.Values['BasePath']);
   Reg.WriteString('BasePathMode', OptionsList.Values['BasePathMode']);
   Reg.WriteString('EnableLog', OptionsList.Values['EnableLog']);
   Reg.WriteString('LogMalware', OptionsList.Values['LogMalware']);
  end;
  AddToLog('Настройки плагина успешно сохранены');
 except
 end;
end;

function BAV_Initialize: Integer; stdcall;
begin
 AddToLog('Инициализация плагина');
 AVZAScanKernel := TAVZAScanKernel.Create;
 LoadPluginSetup;
 AVZAScanKernel.VirusName := '';
 // Загрузка базы
 AddToLog('Загрузка AV базы. Путь к базе = "'+AVZAScanKernel.AVBasePath+'"');
 if AVZAScanKernel.LoadAVBase then begin
  AddToLog('База загружена успешно. Кол-во сигнатур = '+inttostr(AVZAScanKernel.MainAntivirus.SignCount));
  Result := AVC_OK
 end else begin
  Result := AVC_Error;
  AddToLog('Ошибка загрузки баз. Дополнительная информация: '+AVZAScanKernel.VirusName);
  MessageDlg('Ошибка загрузки антивирусных баз ! Не найдена папка Base (текущий путь - "'+AVZAScanKernel.AVBasePath+'")', mtError, [mbOk], 0);
 end;
end;

function BAV_Uninitialize: Integer; stdcall;
begin
 AddToLog('Деинициализация плагина, освобождение памяти');
 AVZAScanKernel.Free;
 AVZAScanKernel := nil;
 Result := AVC_OK;
end;

function BAV_GetName(ABuf: PChar; ABufSize: Integer): Integer; stdcall;
begin
 Result := MakeStr(ABuf, ABufSize, KernelVersion);
end;

function BAV_GetVersion(ABuf: PChar; ABufSize: Integer): Integer; stdcall;
begin
 Result := MakeStr(ABuf, ABufSize, '4.23');
end;

function BAV_SetCfgData(ABuf : ppointer; ADataSize: Integer): Integer; stdcall;
begin
 Result := AVC_OK;
end;

function BAV_GetCfgData(var ABuf : pointer; var ADataSize: Integer): Integer; stdcall;
begin
 Result := AVC_OK;
end;

function BAV_GetStatus: Integer; stdcall;
begin
  Result := AVC_OK;
end;

function BAV_Setup: Integer; stdcall;
begin
  AddToLog('Вызов диалога настройки плагина');
  if ExecuteSetupDlg then begin
   SavePluginSetup;
   SetPluginSetup;
   AVZAScanKernel.LoadAVBase;
  end;
  Result := AVC_OK;
end;

function BAV_ConfigNeeded : Integer; stdcall;
begin
  Result := AVC_OK;
end;

function BAV_MemoryChecking: Integer; stdcall;
begin
  Result := AVC_NotSupported;
end;

function BAV_FileChecking: Integer; stdcall;
begin
  Result := AVC_OK;
//  Result := AVC_NotSupported;
end;

function BAV_StreamChecking: Integer; stdcall;
begin
  Result := AVC_NotSupported;
 // Result := AVC_OK;
end;

function BAV_CheckFile(AFileName: PChar; Msg: PChar): Integer; stdcall;
var
 FileName : string;
 CriticalSection: TRTLCriticalSection;
begin
 FileName := Trim(AFileName);
 Result := AVC_OK;
 AVZAScanKernel.VirusName := '';
 AVZAScanKernel.FullResult := 0;
 // Проверка файла
 AVZAScanKernel.VirusScan(FileName, nil);
 // Формирование результата
 case AVZAScanKernel.FullResult of
  0 : Result := AVC_OK;
  1 : begin
       Result := AVC_VirusFound;
       MakeStr(Msg, cMaxMsgSize, AVZAScanKernel.VirusName);
       if LogMalware then
        AddToLog('Вредоносный объект: '+AFileName+' -> '+AVZAScanKernel.VirusName);
      end;
  2 : begin
       Result := AVC_Suspicious;
       AddToLog('Вредоносный объект (подозрение): '+AFileName+' -> '+AVZAScanKernel.VirusName);
       MakeStr(Msg, cMaxMsgSize, AVZAScanKernel.VirusName);
      end;
 end;
end;

function BAV_CheckStreamEx(InStream: IStream; Msg: PWideChar; const FileName: PWideChar): Integer; stdcall;
var
 f : file;
 Buf : array[0..8000] of byte;
 libNewPosition: Largeint;
 BytesReaded, StreamSize : Longint;
 FMsg : String;
begin
 InStream.Seek(0, STREAM_SEEK_END, libNewPosition);
 StreamSize := libNewPosition;
 InStream.Seek(0, STREAM_SEEK_SET, libNewPosition);
 InStream.Seek(0, 0, libNewPosition);
 ShowMessage(string(FileName)+' '+inttostr(StreamSize));
 AssignFile(f, GetTempDirectoryPath + 'avz_bat.tmp');
 Rewrite(f, 1);
 repeat
  InStream.Read(@Buf[0], 8000, @BytesReaded);
  BlockWrite(f, Buf[0], BytesReaded);
 until BytesReaded < 8000;
 CloseFile(f);
 SetLength(FMsg, cMaxMsgSize);
 Result := BAV_CheckFile(PChar(GetTempDirectoryPath + 'avz_bat.tmp'), @Msg[1]);
end;

function BAV_CureFile(AFileName: PChar; Msg: PChar): Integer; stdcall;
begin
  Result := AVC_NotSupported;
end;

// Note that function names are case-sensitive!
exports
  BAV_Initialize,
  BAV_Uninitialize,
  BAV_GetName,
  BAV_GetVersion,
  BAV_SetCfgData,
  BAV_GetCfgData,
  BAV_GetStatus,
  BAV_Setup,
  BAV_FileChecking,
  BAV_CheckFile,
  BAV_CureFile,
  BAV_ConfigNeeded;

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
  AVZDecompress.ScanFile(AFileName, FileScanner, ArcInfo);
 finally
  FileScanner.Free;
 end;
 except end;
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
 OptionsList  := TStringList.Create;
 LogFileName  := ChangeFileExt(GetModuleFileNameStr(HInstance), '.log');
end.
