unit zProcess;

interface
uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, ntdll,
  SystemMonitor;

type
  TProcessFileInfo = record
   ProcessName     : string;  // Имя
   PID             : DWord;   // PID или hModule
   Size            : DWord;   // Размер
   Descr           : string;  // Описание
   LegalCopyright  : string;  // Копирайты
   MD5             : string;  // Сумма MD5
   CheckResult     : integer; // Результаты проверки
   VirResult       : integer; // Результат проверки по базе антивируса
   VirName         : string;
   Visible         : integer; // Уровень видимости
   FullProcessName : string;  // Полное имя файла
   FileDateTime    : TDateTime; // Дата и время файла
   VisibleLevel    : integer;   // Уровень видимости
   Prim            : string;    // Примечания
   CmdLine         : string;    // Командная строка
   EPROCESS        : dword;     // EPROCESS
  end;

  TProcessInfoArray = array of TProcessFileInfo;

  TProcessManager = class
   FSysProcessInfo : TSysProcessInfo;
   public
    FProcessList, FModulesList    : TProcessInfoArray;
    constructor Create;
    // Создание списка процессов
    Function CreateProcessList : boolean;
    // Создание списка модулей
    Function CreateModuleList(APID : dword) : boolean;
    destructor Destroy; override;
  end;
implementation

{ TProcessManager }

constructor TProcessManager.Create;
begin

end;

function TProcessManager.CreateModuleList(APID: dword): boolean;
begin

end;

function TProcessManager.CreateProcessList: boolean;
begin
 FProcessList        := nil;
 FModulesList        := nil;
 if ISNT then
  FSysProcessInfo := TPSAPIProcessInfo.Create else
   FSysProcessInfo := TToolHelpProcessInfo.Create;
end;

destructor TProcessManager.Destroy;
begin
 FSysProcessInfo.Free;
 FSysProcessInfo := nil;
 FProcessList    := nil;
 FModulesList    := nil;
 inherited;
end;

end.
