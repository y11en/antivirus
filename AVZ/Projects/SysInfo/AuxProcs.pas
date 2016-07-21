unit AuxProcs;

interface

uses mstask, windows, SysUtils;
function GetLocalComputerName: string;
function StrToWide(p_sSource: string): LPCWSTR;
function MessageFromValue(Value: Integer): string;


implementation

var
  wsBuffer: PWideChar;

function GetLocalComputerName: string;
var
  Count: Cardinal;
begin
  Count := MAX_COMPUTERNAME_LENGTH + 1;
  SetLength(Result, Count); { set the length }
  if GetComputerName(PCHar(Result), Count) then SetLength(Result, Count) { set the exact length if it succeeded }
  else
  begin
    RaiseLastWin32Error; { raise exception if it failed }
  end;
end;


function StrToWide(p_sSource: string): LPCWSTR;
begin
  result := StringToWideChar(p_sSource, wsBuffer, Length(p_sSource) + 1);
end;

function MessageFromValue(Value: Integer): string;
begin

  case Value of
    SCHED_S_TASK_READY:
      Result := 'The task is ready to run at its next scheduled time.';

    SCHED_S_TASK_RUNNING:
      Result := 'The task is currently running.';

    SCHED_S_TASK_DISABLED:
      Result := 'The task will not run at the scheduled times because it has been disabled.';

    SCHED_S_TASK_HAS_NOT_RUN:
      Result := 'The task has not yet run.';


    SCHED_S_TASK_NO_MORE_RUNS:
      Result := 'There are no more runs scheduled for this task.';


    SCHED_S_TASK_NOT_SCHEDULED:
      Result := 'One or more of the properties that are needed to run this task on a schedule have not been set.';


    SCHED_S_TASK_TERMINATED:
      Result := 'The last run of the task was terminated by the user.';

    SCHED_S_TASK_NO_VALID_TRIGGERS:
      Result := 'Either the task has no triggers or the existing triggers are disabled or not set.';

    SCHED_S_EVENT_TRIGGER:
      Result := 'Event triggers dont have set run times.';

    SCHED_E_TRIGGER_NOT_FOUND:
      Result := 'Trigger not found.';

    SCHED_E_TASK_NOT_READY:
      Result := 'One or more of the properties that are needed to run this task have not been set.';

    SCHED_E_TASK_NOT_RUNNING:
      Result := 'There is no running instance of the task to terminate.';

    SCHED_E_SERVICE_NOT_INSTALLED:
      Result := 'The Task Scheduler Service is not installed on this computer.';

    SCHED_E_CANNOT_OPEN_TASK:
      Result := 'The task object could not be opened.';

    SCHED_E_INVALID_TASK:
      Result := 'The object is either an invalid task object or is not a task object.';

    SCHED_E_ACCOUNT_INFORMATION_NOT_SET:
      Result := 'No account information could be found in the Task Scheduler security database for the task indicated.';


    SCHED_E_ACCOUNT_NAME_NOT_FOUND:
      Result := 'Unable to establish existence of the account specified.';


    SCHED_E_ACCOUNT_DBASE_CORRUPT:
      Result := 'Corruption was detected in the Task Scheduler security database; the database has been reset.';

    SCHED_E_NO_SECURITY_SERVICES:
      Result := 'Task Scheduler security services are available only on Windows NT.';


    SCHED_E_UNKNOWN_OBJECT_VERSION:
      Result := 'The task object version is either unsupported or invalid.';


    SCHED_E_UNSUPPORTED_ACCOUNT_OPTION:
      Result := 'The task has been configured with an unsupported combination of account settings and run time options.';

    SCHED_E_SERVICE_NOT_RUNNING:
      Result := 'The Task Scheduler Service is not running.';
    E_INVALIDARG:
      Result := 'The arguments are not valid.';

    E_OUTOFMEMORY:
      Result := 'Not enough memory is available.';
  end;
end;


initialization
  GetMem(wsBuffer, 1024);
finalization
  freeMem(wsBuffer);
end.
