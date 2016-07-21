unit zTaskScheduler;

interface
uses
  Windows, Messages, SysUtils, Classes, ActiveX, MsTask, MsTaskUtils,
  RxVerInf, AuxProcs, zSharedFunctions, zAVKernel, zLOgSystem;
type
 // Информация о BHO
 TTaskSchedulerJob = record
  Name            : string;  // Имя
  BinFile         : string;  // Бинарный файл
  Status          : HRESULT; // Статус задания
  Descr           : string;  // Описание файла
  LegalCopyright  : string;  // Копирайты файла
  CheckResult     : integer; // Результат проверки по базе безопасных программ
  NextRecentTime,
  MostRecentRunTime : TDateTime;
 end;
 TTaskSchedulerJobInfoArray = array of TTaskSchedulerJob;
 // Менеджер заданий планировщика
 TTaskSchedulerJobs = class
  private
   SchedulingAgent: ITaskScheduler;
   ScheduledWorkItem: IScheduledWorkItem;
   pIPersistFile: IPersistFile;
   Task: ITask;
   function AddToDebugLog(S : string) : boolean;
    function GetITask(TaskName: string): ITask;
  public
   TaskSchedulerJobInfoArray : TTaskSchedulerJobInfoArray;
   constructor Create;
   destructor Destroy; override;
   function Init : HRESULT;
   // Обновлени списка
   function RefresTaskSchedulerJobsList : boolean;
   // Получение статуса элемента
   function GetItemEnabledStatus(TaskSchedulerJob : TTaskSchedulerJob) : integer;
   // Удаление элемента
   function DeleteItem(TaskSchedulerJob : TTaskSchedulerJob) : boolean;
   // Настройка включен/выключен
   function SetItemEnabledStatus(var TaskSchedulerJob : TTaskSchedulerJob; NewStatus : boolean) : boolean;
   function AddTask(TaskSchedulerJob : TTaskSchedulerJob) : boolean;
 end;

implementation
{ TTaskSchedulerJobs }

function TTaskSchedulerJobs.AddToDebugLog(S: string): boolean;
begin
 ;
end;

constructor TTaskSchedulerJobs.Create;
begin
 TaskSchedulerJobInfoArray := nil;
end;

function TTaskSchedulerJobs.DeleteItem(
  TaskSchedulerJob: TTaskSchedulerJob): boolean;
begin
  Result := SchedulingAgent.Delete(StrToWide(TaskSchedulerJob.Name)) = S_OK;
end;

destructor TTaskSchedulerJobs.Destroy;
begin
  TaskSchedulerJobInfoArray := nil;
  inherited;
end;

function TTaskSchedulerJobs.GetItemEnabledStatus(
  TaskSchedulerJob: TTaskSchedulerJob): integer;
begin

end;

function TTaskSchedulerJobs.Init: HRESULT;
begin
  Result := S_OK;
  if not assigned(SchedulingAgent) then
  begin
    Result := ActiveX.CoInitialize(nil);
{    if Result <> S_OK then
    begin
      ActiveX.CoUninitialize;
      AddToDebugLog('$AVZ0004 = ' + IntToHex(Result, 8));
      exit;
    end;   }
    AddToDebugLog('$AVZ0182 ....');
    Result := ActiveX.CoCreateInstance(CLSID_CSchedulingAgent,
      nil,
      CLSCTX_INPROC_SERVER,
      IID_ITaskScheduler,
      SchedulingAgent);
    AddToDebugLog('$AVZ0181');
    if Result <> S_OK then
    begin
      AddToDebugLog('$AVZ0003 ' + IntToHex(Result, 8));
    end;
  end;
end;

function TTaskSchedulerJobs.GetITask(TaskName: string): ITask;
var
  PITask: IUnknown;
  hr: HRESULT;
  C1 : TGUID;
begin
  C1 := IID_ITask;
  hr := SchedulingAgent.Activate(StrToWide(TaskName), C1, PITask);
  if hr = S_OK then Result := ITask(PITask)
  else
  begin
    AddToDebugLog('$AVZ0653 =' + IntToHex(hr, 8));
    Result := nil;
  end;
end;

function TTaskSchedulerJobs.RefresTaskSchedulerJobsList: boolean;
var
  hr: HRESULT;
  pEnum: IEnumWorkItems;
  TaskNames: PLPWSTR;
  dwFetched: DWORD;
  szString: string;
  Appname: PWideChar;
  NextRunTime: _SYSTEMTIME;
  TaskSchedulerJob : TTaskSchedulerJob;
begin
 try
  TaskSchedulerJobInfoArray := nil;
{  hr := StartScheduler;
  if hr = ERROR_SUCCESS then AddToDebugLog('$AVZ0030')
   else AddToDebugLog('$AVZ0029 = ' + IntTohex(hr, 8));}
  // Инициализация интерфейса и прочей дребедени
  if Init <> S_OK then exit;

  hr := SchedulingAgent.Enum(pEnum);
  if hr <> S_OK then begin
   AddToDebugLog('$AVZ0025:' + IntTohex(hr, 8));
   exit;
  end;

  while (pEnum.Next(1, TaskNames, dwFetched) = 0) and (dwFetched > 0) do
  begin
    // Имя
    szString := WideCharToString(TaskNames^);
    TaskSchedulerJob.Name := szString;
    // Приложение
    Task := GetITask(szString);
    Task.GetApplicationName(Appname);
    TaskSchedulerJob.BinFile := Appname;
    // Статус
    Task.GetStatus(hr);
    //
    TaskSchedulerJob.Status := HR;
    TaskSchedulerJob.NextRecentTime    := 0;
    TaskSchedulerJob.MostRecentRunTime := 0;
    // GetNextRecentTime
    hr := Task.GetNextRunTime(NextRunTime);
    if hr = S_OK then TaskSchedulerJob.NextRecentTime := SystemTimeToDateTime(NextRunTime);
    // GetMostRecentTime
    hr := Task.GetMostRecentRunTime(NextRunTime);
    if hr = S_OK then TaskSchedulerJob.MostRecentRunTime :=  SystemTimeToDateTime(NextRunTime);
    AddTask(TaskSchedulerJob);
    ActiveX.CoTaskMemFree(TaskNames);
  end;
 except
  on E : Exception do
   AddToDebugLog('$AVZ0630: '+e.Message);
 end;
end;

function TTaskSchedulerJobs.SetItemEnabledStatus(
  var TaskSchedulerJob: TTaskSchedulerJob; NewStatus: boolean): boolean;
begin

end;

function TTaskSchedulerJobs.AddTask(
  TaskSchedulerJob: TTaskSchedulerJob): boolean;
var
 VersionInfo : TVersionInfo;
begin
 SetLength(TaskSchedulerJobInfoArray, Length(TaskSchedulerJobInfoArray)+1);
 TaskSchedulerJob.BinFile := NormalProgramFileName(TaskSchedulerJob.BinFile, '.exe');
 ZProgressClass.AddProgressMax(1);
 TaskSchedulerJob.CheckResult := FileSignCheck.CheckFile(TaskSchedulerJob.BinFile);
 ZProgressClass.ProgressStep;
 try
  VersionInfo := TVersionInfo.Create(TaskSchedulerJob.BinFile);
  TaskSchedulerJob.Descr := VersionInfo.FileDescription;
  TaskSchedulerJob.LegalCopyright := VersionInfo.LegalCopyright;
  VersionInfo.Free;
 except end;
 TaskSchedulerJobInfoArray[Length(TaskSchedulerJobInfoArray)-1] := TaskSchedulerJob;
end;

end.

{    // Отключение
   Task.SetParameters(StrToWide(''));
    Task.QueryInterface(IID_IPersistFile, pIPersistFile);
    if pIPersistFile <> nil then
      pIPersistFile.Save(nil, true);}

