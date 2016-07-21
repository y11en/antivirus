unit zLogSystem;
// Модуль протоколирования. Применяется всеми другими модулями
interface
 uses SysUtils, Classes;
const
 logNormal = 0;
 logInfo   = 1;
 logAlert  = 2;
 logVirus  = 3;
 logGood   = 4;
 logError  = 10;
 logDebug  = 11;
type
 // События протоколирования
 TAddToLogEvent      = procedure (AMsg : string; InfoType : integer) of object;
 TAddToDebugLogEvent = procedure (AMsg : string) of object;
 TGetLog             = procedure (ALog : TStrings) of object;

 // Класс, отвечающий за глобальное протоколирование
 TLogger = class
  private
    FOnAddToLog: TAddToLogEvent;
    FOnAddToDebugLog: TAddToDebugLogEvent;
    FOnGetLog: TGetLog;
    procedure SetOnAddToLog(const Value: TAddToLogEvent);
    procedure SetOnAddToDebugLog(const Value: TAddToDebugLogEvent);
    procedure SetOnGetLog(const Value: TGetLog);
  public
   constructor Create;
   // Добавление в протокол
   procedure AddToLog(AMsg : string; InfoType : integer);
   procedure GetLog(ALog : TStrings);
   // Добавление в отладочный протокол
   procedure AddToDebugLog(AMsg : string);
   // Обработчик события "добавление в протокол"
   property OnAddToLog      : TAddToLogEvent read FOnAddToLog write SetOnAddToLog;
   // Обработчик события "добавление в отладочный протокол"
   property OnAddToDebugLog : TAddToDebugLogEvent read FOnAddToDebugLog write SetOnAddToDebugLog;
   // Обработчик события "запрос всех событий протокола"
   property OnGetLog        : TGetLog read FOnGetLog write SetOnGetLog;
 end;

 TProgressEvent = procedure(AMax, ACurr : integer) of object;
 // Класс, отвечающий за глобальное протоколирование
 TzProgress = class
  private
    FMax, FCurr : integer;
    FOnProgress: TProgressEvent;
    procedure SetOnProgress(const Value: TProgressEvent);
    // Вызов обработчика прогресс-индикации
    procedure DoProgress(AMax, ACurr : integer);
    procedure SetMax(const Value: integer);
  public
   constructor Create;
   procedure AddProgressMax(AStepCount : integer);
   // Шаг прогресса
   procedure ProgressStep(AStep : integer = 1);
  published
   // Обработчик события "прогресс"
   property OnProgress      : TProgressEvent read FOnProgress write SetOnProgress;
   property Max : integer read FMax write SetMax;
 end;

 // Добавление в протокол
 procedure AddToLog(AMsg : string; InfoType : integer);
 procedure AddToFileLog(AMsg : string);
 // Добавление в отладочный протокол
 procedure AddToDebugLog(AMsg : string);

var
 DebugLogMode  : boolean = false; // Режим отладочного протоколирования
threadvar
 // Имя файла, в который дублируются строки протокола
 FSpoolLogFileName : string;
 ZLoggerClass   : TLogger;
 ZProgressClass : TzProgress;

// Инициализирует систему протоколирования
function InitLogger : boolean;

implementation
uses zTranslate;
{ TLogger }

procedure TLogger.AddToDebugLog(AMsg: string);
begin
 if Assigned(FOnAddToDebugLog) then
  FOnAddToDebugLog(AMsg);
end;

procedure TLogger.AddToLog(AMsg: string; InfoType: integer);
begin
 if Assigned(FOnAddToLog) then
  FOnAddToLog(AMsg, InfoType);
end;

constructor TLogger.Create;
begin
 FOnAddToLog      := nil;
 FOnAddToDebugLog := nil;
 FOnGetLog        := nil;
end;

procedure TLogger.GetLog(ALog: TStrings);
begin
 if Assigned(FOnGetLog) then
  FOnGetLog(ALog);
end;

procedure TLogger.SetOnAddToDebugLog(const Value: TAddToDebugLogEvent);
begin
  FOnAddToDebugLog := Value;
end;

procedure TLogger.SetOnAddToLog(const Value: TAddToLogEvent);
begin
  FOnAddToLog := Value;
end;

// Добавление в протокол
procedure AddToLog(AMsg : string; InfoType : integer);
begin
 ZLoggerClass.AddToLog(AMsg, InfoType);
 if FSpoolLogFileName <> '' then
  AddToFileLog(TranslateStr(AMsg));
end;

// Добавление в отладочный протокол
procedure AddToDebugLog(AMsg : string);
begin
 if DebugLogMode then
  ZLoggerClass.AddToDebugLog(AMsg);
end;

procedure AddToFileLog(AMsg : string);
var
 f     : TextFile;
begin
 if FSpoolLogFileName = '' then exit;
 AssignFile(f, FSpoolLogFileName);
 try
  // Если файл существует - дозапись, не существует - создание
  if FileExists(FSpoolLogFileName) then Append(f)
   else Rewrite(f);
 except
  exit; // При ошибках немедленный выход, дабы не усугублять положение программы
 end;
 try
  try Writeln(f, AMsg); except end;
 finally
  CloseFile(f); // в конце обязательно закроем файл
 end;
end;

procedure TLogger.SetOnGetLog(const Value: TGetLog);
begin
  FOnGetLog := Value;
end;

function InitLogger : boolean;
begin
 FSpoolLogFileName := '';
 ZLoggerClass   := TLogger.Create;
 ZProgressClass := TZProgress.Create;
end;


{ TzProgress }

{ TzProgress }
constructor TzProgress.Create;
begin
 FOnProgress := nil;
 FMax  := 0;
 FCurr := 0;
end;

procedure TzProgress.ProgressStep(AStep : integer = 1);
begin
 inc(FCurr, AStep);
 if FCurr > FMax then
  FMax := FCurr;
 DoProgress(FMax, FCurr);
end;

procedure TzProgress.DoProgress(AMax, ACurr : integer);
begin
 if Assigned(FOnProgress) then
  FOnProgress(AMax, ACurr);
end;

procedure TzProgress.SetOnProgress(const Value: TProgressEvent);
begin
  FOnProgress := Value;
end;

procedure TzProgress.AddProgressMax(AStepCount: integer);
begin
 if AStepCount < 0 then exit;
 FMax := FMax + AStepCount;
 if FCurr > FMax then
  FMax := FCurr;
 DoProgress(FMax, FCurr);
end;

procedure TzProgress.SetMax(const Value: integer);
begin
  FMax := Value;
end;

begin
end.
