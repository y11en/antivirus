{******************************************************************}
{                                                                  }
{       Borland Delphi Runtime Library                             }
{       MS Task Scheduler interface unit                           }
{                                                                  }
{ Portions created by Microsoft are                                }
{ Copyright (C) 1995-1999 Microsoft Corporation.                   }
{ All Rights Reserved.                                             }
{                                                                  }
{ The original file is: mstask.h, released 12 May 1999.            }
{ The original Pascal code is: MsTask.pas, released 17 July 2000.  }
{ The initial developer of the Pascal code is Markus Pingel        }
{ (mpingel@vob.de).                                                }
{                                                                  }
{ Portions created by Markus Pingel are                            }
{ Copyright (C) 1999 Markus Pingel.                                }
{                                                                  }
{ Portions created by Marcel van Brakel are                        }
{ Copyright (C) 1999 Marcel van Brakel.                            }
{                                                                  }
{ Contributor(s): Sunish Issac  (sunish@nettaxi.com)               }
{                                                                  }
{ Obtained through:                                                }
{ Joint Endeavour of Delphi Innovators (Project JEDI)              }
{                                                                  }
{ You may retrieve the latest version of this file at the Project  }
{ JEDI home page, located at http://delphi-jedi.org                }
{                                                                  }
{ The contents of this file are used with permission, subject to   }
{ the Mozilla Public License Version 1.1 (the "License"); you may  }
{ not use this file except in compliance with the License. You may }
{ obtain a copy of the License at                                  }
{ http://www.mozilla.org/MPL/                                      }
{                                                                  }
{ Software distributed under the License is distributed on an      }
{ "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or   }
{ implied. See the License for the specific language governing     }
{ rights and limitations under the License.                        }
{                                                                  }
{******************************************************************}

unit MsTask;

interface

(*$HPPEMIT ''*)
(*$HPPEMIT '#include "mstask.h"'*)
(*$HPPEMIT ''*)

uses
  Windows, CommCtrl;

// This is a subset of WinError.pas where the Task Scheduler error codes
// currently reside. These declarations are missing from Windows.pas. The
// full WinError.pas file can be retrieved from www.delphi-jedi.org in
// the Windows Base package.

const

//
// Task Scheduler errors
//
//
// MessageId: SCHED_S_TASK_READY
//
// MessageText:
//
//  The task is ready to run at its next scheduled time.
//

  SCHED_S_TASK_READY = HRESULT($00041300);
  {$EXTERNALSYM SCHED_S_TASK_READY}

//
// MessageId: SCHED_S_TASK_RUNNING
//
// MessageText:
//
//  The task is currently running.
//

  SCHED_S_TASK_RUNNING = HRESULT($00041301);
  {$EXTERNALSYM SCHED_S_TASK_RUNNING}

//
// MessageId: SCHED_S_TASK_DISABLED
//
// MessageText:
//
//  The task will not run at the scheduled times because it has been disabled.
//

  SCHED_S_TASK_DISABLED = HRESULT($00041302);
  {$EXTERNALSYM SCHED_S_TASK_DISABLED}

//
// MessageId: SCHED_S_TASK_HAS_NOT_RUN
//
// MessageText:
//
//  The task has not yet run.
//

  SCHED_S_TASK_HAS_NOT_RUN = HRESULT($00041303);
  {$EXTERNALSYM SCHED_S_TASK_HAS_NOT_RUN}

//
// MessageId: SCHED_S_TASK_NO_MORE_RUNS
//
// MessageText:
//
//  There are no more runs scheduled for this task.
//

  SCHED_S_TASK_NO_MORE_RUNS = HRESULT($00041304);
  {$EXTERNALSYM SCHED_S_TASK_NO_MORE_RUNS}

//
// MessageId: SCHED_S_TASK_NOT_SCHEDULED
//
// MessageText:
//
//  One or more of the properties that are needed to run this task on a schedule have not been set.
//

  SCHED_S_TASK_NOT_SCHEDULED = HRESULT($00041305);
  {$EXTERNALSYM SCHED_S_TASK_NOT_SCHEDULED}

//
// MessageId: SCHED_S_TASK_TERMINATED
//
// MessageText:
//
//  The last run of the task was terminated by the user.
//

  SCHED_S_TASK_TERMINATED = HRESULT($00041306);
  {$EXTERNALSYM SCHED_S_TASK_TERMINATED}

//
// MessageId: SCHED_S_TASK_NO_VALID_TRIGGERS
//
// MessageText:
//
//  Either the task has no triggers or the existing triggers are disabled or not set.
//

  SCHED_S_TASK_NO_VALID_TRIGGERS = HRESULT($00041307);
  {$EXTERNALSYM SCHED_S_TASK_NO_VALID_TRIGGERS}

//
// MessageId: SCHED_S_EVENT_TRIGGER
//
// MessageText:
//
//  Event triggers don't have set run times.
//

  SCHED_S_EVENT_TRIGGER = HRESULT($00041308);
  {$EXTERNALSYM SCHED_S_EVENT_TRIGGER}

//
// MessageId: SCHED_E_TRIGGER_NOT_FOUND
//
// MessageText:
//
//  Trigger not found.
//

  SCHED_E_TRIGGER_NOT_FOUND = HRESULT($80041309);
  {$EXTERNALSYM SCHED_E_TRIGGER_NOT_FOUND}

//
// MessageId: SCHED_E_TASK_NOT_READY
//
// MessageText:
//
//  One or more of the properties that are needed to run this task have not been set.
//

  SCHED_E_TASK_NOT_READY = HRESULT($8004130A);
  {$EXTERNALSYM SCHED_E_TASK_NOT_READY}

//
// MessageId: SCHED_E_TASK_NOT_RUNNING
//
// MessageText:
//
//  There is no running instance of the task to terminate.
//

  SCHED_E_TASK_NOT_RUNNING = HRESULT($8004130B);
  {$EXTERNALSYM SCHED_E_TASK_NOT_RUNNING}

//
// MessageId: SCHED_E_SERVICE_NOT_INSTALLED
//
// MessageText:
//
//  The Task Scheduler Service is not installed on this computer.
//

  SCHED_E_SERVICE_NOT_INSTALLED = HRESULT($8004130C);
  {$EXTERNALSYM SCHED_E_SERVICE_NOT_INSTALLED}

//
// MessageId: SCHED_E_CANNOT_OPEN_TASK
//
// MessageText:
//
//  The task object could not be opened.
//

  SCHED_E_CANNOT_OPEN_TASK = HRESULT($8004130D);
  {$EXTERNALSYM SCHED_E_CANNOT_OPEN_TASK}

//
// MessageId: SCHED_E_INVALID_TASK
//
// MessageText:
//
//  The object is either an invalid task object or is not a task object.
//

  SCHED_E_INVALID_TASK = HRESULT($8004130E);
  {$EXTERNALSYM SCHED_E_INVALID_TASK}

//
// MessageId: SCHED_E_ACCOUNT_INFORMATION_NOT_SET
//
// MessageText:
//
//  No account information could be found in the Task Scheduler security database for the task indicated.
//

  SCHED_E_ACCOUNT_INFORMATION_NOT_SET = HRESULT($8004130F);
  {$EXTERNALSYM SCHED_E_ACCOUNT_INFORMATION_NOT_SET}

//
// MessageId: SCHED_E_ACCOUNT_NAME_NOT_FOUND
//
// MessageText:
//
//  Unable to establish existence of the account specified.
//

  SCHED_E_ACCOUNT_NAME_NOT_FOUND = HRESULT($80041310);
  {$EXTERNALSYM SCHED_E_ACCOUNT_NAME_NOT_FOUND}

//
// MessageId: SCHED_E_ACCOUNT_DBASE_CORRUPT
//
// MessageText:
//
//  Corruption was detected in the Task Scheduler security database; the database has been reset.
//

  SCHED_E_ACCOUNT_DBASE_CORRUPT = HRESULT($80041311);
  {$EXTERNALSYM SCHED_E_ACCOUNT_DBASE_CORRUPT}

//
// MessageId: SCHED_E_NO_SECURITY_SERVICES
//
// MessageText:
//
//  Task Scheduler security services are available only on Windows NT.
//

  SCHED_E_NO_SECURITY_SERVICES = HRESULT($80041312);
  {$EXTERNALSYM SCHED_E_NO_SECURITY_SERVICES}

//
// MessageId: SCHED_E_UNKNOWN_OBJECT_VERSION
//
// MessageText:
//
//  The task object version is either unsupported or invalid.
//

  SCHED_E_UNKNOWN_OBJECT_VERSION = HRESULT($80041313);
  {$EXTERNALSYM SCHED_E_UNKNOWN_OBJECT_VERSION}

//
// MessageId: SCHED_E_UNSUPPORTED_ACCOUNT_OPTION
//
// MessageText:
//
//  The task has been configured with an unsupported combination of account settings and run time options.
//

  SCHED_E_UNSUPPORTED_ACCOUNT_OPTION = HRESULT($80041314);
  {$EXTERNALSYM SCHED_E_UNSUPPORTED_ACCOUNT_OPTION}

//
// MessageId: SCHED_E_SERVICE_NOT_RUNNING
//
// MessageText:
//
//  The Task Scheduler Service is not running.
//

  SCHED_E_SERVICE_NOT_RUNNING = HRESULT($80041315);
  {$EXTERNALSYM SCHED_E_SERVICE_NOT_RUNNING}

// Start of MsTask.pas

const
  TASK_SUNDAY = ($1);
  {$EXTERNALSYM TASK_SUNDAY}
  TASK_MONDAY = ($2);
  {$EXTERNALSYM TASK_MONDAY}
  TASK_TUESDAY = ($4);
  {$EXTERNALSYM TASK_TUESDAY}
  TASK_WEDNESDAY = ($8);
  {$EXTERNALSYM TASK_WEDNESDAY}
  TASK_THURSDAY = ($10);
  {$EXTERNALSYM TASK_THURSDAY}
  TASK_FRIDAY = ($20);
  {$EXTERNALSYM TASK_FRIDAY}
  TASK_SATURDAY = ($40);
  {$EXTERNALSYM TASK_SATURDAY}
  TASK_FIRST_WEEK = (1);
  {$EXTERNALSYM TASK_FIRST_WEEK}
  TASK_SECOND_WEEK = (2);
  {$EXTERNALSYM TASK_SECOND_WEEK}
  TASK_THIRD_WEEK = (3);
  {$EXTERNALSYM TASK_THIRD_WEEK}
  TASK_FOURTH_WEEK = (4);
  {$EXTERNALSYM TASK_FOURTH_WEEK}
  TASK_LAST_WEEK = (5);
  {$EXTERNALSYM TASK_LAST_WEEK}
  TASK_JANUARY = ($1);
  {$EXTERNALSYM TASK_JANUARY}
  TASK_FEBRUARY = ($2);
  {$EXTERNALSYM TASK_FEBRUARY}
  TASK_MARCH = ($4);
  {$EXTERNALSYM TASK_MARCH}
  TASK_APRIL = ($8);
  {$EXTERNALSYM TASK_APRIL}
  TASK_MAY = ($10);
  {$EXTERNALSYM TASK_MAY}
  TASK_JUNE = ($20);
  {$EXTERNALSYM TASK_JUNE}
  TASK_JULY = ($40);
  {$EXTERNALSYM TASK_JULY}
  TASK_AUGUST = ($80);
  {$EXTERNALSYM TASK_AUGUST}
  TASK_SEPTEMBER = ($100);
  {$EXTERNALSYM TASK_SEPTEMBER}
  TASK_OCTOBER = ($200);
  {$EXTERNALSYM TASK_OCTOBER}
  TASK_NOVEMBER = ($400);
  {$EXTERNALSYM TASK_NOVEMBER}
  TASK_DECEMBER = ($800);
  {$EXTERNALSYM TASK_DECEMBER}
  TASK_FLAG_INTERACTIVE = ($1);
  {$EXTERNALSYM TASK_FLAG_INTERACTIVE}
  TASK_FLAG_DELETE_WHEN_DONE = ($2);
  {$EXTERNALSYM TASK_FLAG_DELETE_WHEN_DONE}
  TASK_FLAG_DISABLED = ($4);
  {$EXTERNALSYM TASK_FLAG_DISABLED}
  TASK_FLAG_START_ONLY_IF_IDLE = ($10);
  {$EXTERNALSYM TASK_FLAG_START_ONLY_IF_IDLE}
  TASK_FLAG_KILL_ON_IDLE_END = ($20);
  {$EXTERNALSYM TASK_FLAG_KILL_ON_IDLE_END}
  TASK_FLAG_DONT_START_IF_ON_BATTERIES = ($40);
  {$EXTERNALSYM TASK_FLAG_DONT_START_IF_ON_BATTERIES}
  TASK_FLAG_KILL_IF_GOING_ON_BATTERIES = ($80);
  {$EXTERNALSYM TASK_FLAG_KILL_IF_GOING_ON_BATTERIES}
  TASK_FLAG_RUN_ONLY_IF_DOCKED = ($100);
  {$EXTERNALSYM TASK_FLAG_RUN_ONLY_IF_DOCKED}
  TASK_FLAG_HIDDEN = ($200);
  {$EXTERNALSYM TASK_FLAG_HIDDEN}
  TASK_FLAG_RUN_IF_CONNECTED_TO_INTERNET = ($400);
  {$EXTERNALSYM TASK_FLAG_RUN_IF_CONNECTED_TO_INTERNET}
  TASK_FLAG_RESTART_ON_IDLE_RESUME = ($800);
  {$EXTERNALSYM TASK_FLAG_RESTART_ON_IDLE_RESUME}
  TASK_FLAG_SYSTEM_REQUIRED = ($1000);
  {$EXTERNALSYM TASK_FLAG_SYSTEM_REQUIRED}
  TASK_FLAG_RUN_ONLY_IF_LOGGED_ON = ($2000);
  {$EXTERNALSYM TASK_FLAG_RUN_ONLY_IF_LOGGED_ON}
  TASK_TRIGGER_FLAG_HAS_END_DATE = ($1);
  {$EXTERNALSYM TASK_TRIGGER_FLAG_HAS_END_DATE}
  TASK_TRIGGER_FLAG_KILL_AT_DURATION_END = ($2);
  {$EXTERNALSYM TASK_TRIGGER_FLAG_KILL_AT_DURATION_END}
  TASK_TRIGGER_FLAG_DISABLED = ($4);
  {$EXTERNALSYM TASK_TRIGGER_FLAG_DISABLED}
  TASK_MAX_RUN_TIMES = (1440);
  {$EXTERNALSYM TASK_MAX_RUN_TIMES}

type
  PTaskTriggerType = ^TTaskTriggerType;
  _TASK_TRIGGER_TYPE = (
    TASK_TIME_TRIGGER_ONCE,
    TASK_TIME_TRIGGER_DAILY,
    TASK_TIME_TRIGGER_WEEKLY,
    TASK_TIME_TRIGGER_MONTHLYDATE,
    TASK_TIME_TRIGGER_MONTHLYDOW,
    TASK_EVENT_TRIGGER_ON_IDLE,
    TASK_EVENT_TRIGGER_AT_SYSTEMSTART,
    TASK_EVENT_TRIGGER_AT_LOGON,
    TASK_TRIGGER_TYPE);
  {$EXTERNALSYM _TASK_TRIGGER_TYPE}
  PTASK_TRIGGER_TYPE = ^_TASK_TRIGGER_TYPE;
  {$EXTERNALSYM PTASK_TRIGGER_TYPE}
  TTaskTriggerType = _TASK_TRIGGER_TYPE;

  PDaily = ^TDaily;
  _DAILY = record
    DaysInterval: WORD;
  end;
  {$EXTERNALSYM _DAILY}
  TDaily = _DAILY;

  PWeekly = ^TWeekly;
  _WEEKLY = record
    WeeksInterval: WORD;
    rgfDaysOfTheWeek: WORD;
  end;
  {$EXTERNALSYM _WEEKLY}
  TWeekly = _WEEKLY;

  PMonthlydate = ^TMonthlydate;
  _MONTHLYDATE = record
    rgfDays: DWORD;
    rgfMonths: WORD;
  end;
  {$EXTERNALSYM _MONTHLYDATE}
  TMonthlydate = _MONTHLYDATE;

  PMonthlydow = ^TMonthlydow;
  _MONTHLYDOW = record
    wWhichWeek: WORD;
    rgfDaysOfTheWeek: WORD;
    rgfMonths: WORD;
  end;
  {$EXTERNALSYM _MONTHLYDOW}
  TMonthlydow = _MONTHLYDOW;

  PTriggerTypeUnion = ^TTriggerTypeUnion;
  _TRIGGER_TYPE_UNION = record
    case Integer of
      0: (Daily: TDaily);
      1: (Weekly: TWeekly);
      2: (MonthlyDate: TMonthlyDate);
      3: (MonthlyDOW: TMonthlyDow);
  end;
  {$EXTERNALSYM _TRIGGER_TYPE_UNION}
  TRIGGER_TYPE_UNION = _TRIGGER_TYPE_UNION;
  {$EXTERNALSYM TRIGGER_TYPE_UNION}
  TTriggerTypeUnion = _TRIGGER_TYPE_UNION;

  PTaskTrigger = ^TTaskTrigger;
  _TASK_TRIGGER = record
    cbTriggerSize: WORD;
    Reserved1: WORD;
    wBeginYear: WORD;
    wBeginMonth: WORD;
    wBeginDay: WORD;
    wEndYear: WORD;
    wEndMonth: WORD;
    wEndDay: WORD;
    wStartHour: WORD;
    wStartMinute: WORD;
    MinutesDuration: DWORD;
    MinutesInterval: DWORD;
    rgFlags: DWORD;
    TriggerType: TTaskTriggerType;
    Type_: TTriggerTypeUnion;
    Reserved2: WORD;
    wRandomMinutesInterval: WORD;
  end;
  {$EXTERNALSYM _TASK_TRIGGER}
  TASK_TRIGGER = _TASK_TRIGGER;
  {$EXTERNALSYM TASK_TRIGGER}
  PTASK_TRIGGER = ^TASK_TRIGGER;
  {$EXTERNALSYM PTASK_TRIGGER}
  TTaskTrigger = _TASK_TRIGGER;

const
  IID_ITaskTrigger: TGUID = '{148BD52B-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM IID_ITaskTrigger}

// interface ITaskTrigger

type
  ITaskTrigger = interface
    ['{148BD52B-A2AB-11CE-B11F-00AA00530503}']
    function SetTrigger(const pTrigger: PTaskTrigger): HRESULT; stdcall;
    function GetTrigger(pTrigger: PTaskTrigger): HRESULT; stdcall;
    function GetTriggerString(var ppwszTrigger: LPWSTR): HRESULT; stdcall;
  end;
  {$EXTERNALSYM ITaskTrigger}

const
  IID_IScheduledWorkItem: TGUID = '{A6B952F0-A4B1-11D0-997D-00AA006887EC}';
  {$EXTERNALSYM IID_IScheduledWorkItem}

// interface IScheduledWorkItem

type
  IScheduledWorkItem = interface
    ['{A6B952F0-A4B1-11D0-997D-00AA006887EC}']
    function CreateTrigger(out piNewTrigger: Word;
      out ppTrigger: ITaskTrigger): HRESULT; stdcall;
    function DeleteTrigger(iTrigger: Word): HRESULT; stdcall;
    function GetTriggerCount(out pwCount: Word): HRESULT; stdcall;
    function GetTrigger(iTrigger: Word; var ppTrigger: ITaskTrigger): HRESULT; stdcall;
    function GetTriggerString(iTrigger: Word;
      out ppwszTrigger: PWideChar): HRESULT; stdcall;
    function GetRunTimes(const pstBegin: PSystemTime; const pstEnd: PSystemTime;
      var pCount: Word; out rgstTaskTimes: PSystemTime): HRESULT; stdcall;
    function GetNextRunTime(var pstNextRun: TSystemTime): HRESULT; stdcall;
    function SetIdleWait(wIdleMinutes: Word; wDeadlineMinutes: Word): HRESULT; stdcall;
    function GetIdleWait(out pwIdleMinutes: Word; out pwDeadlineMinutes: Word): HRESULT; stdcall;
    function Run: HRESULT; stdcall;
    function Terminate: HRESULT; stdcall;
    function EditWorkItem(hParent: HWND; dwReserved: DWORD): HRESULT; stdcall;
    function GetMostRecentRunTime(out pstLastRun: TSystemTime): HRESULT; stdcall;
    function GetStatus(out phrStatus: HRESULT): HRESULT stdcall;
    function GetExitCode(out pdwExitCode: DWORD): HRESULT stdcall;
    function SetComment(pwszComment: LPCWSTR): HRESULT stdcall;
    function GetComment(out ppwszComment: LPWSTR): HRESULT stdcall;
    function SetCreator(pwszCreator: LPCWSTR): HRESULT; stdcall;
    function GetCreator(out ppwszCreator: LPWSTR): HRESULT; stdcall;
    function SetWorkItemData(cbData: Word; var rgbData: Byte): HRESULT; stdcall;
    function GetWorkItemData(out pcbData: Word; out prgbData: Byte): HRESULT; stdcall;
    function SetErrorRetryCount(wRetryCount: Word): HRESULT; stdcall;
    function GetErrorRetryCount(out pwRetryCount: Word): HRESULT; stdcall;
    function SetErrorRetryInterval(wRetryInterval: Word): HRESULT; stdcall;
    function GetErrorRetryInterval(out pwRetryInterval: Word): HRESULT; stdcall;
    function SetFlags(dwFlags: DWORD): HRESULT; stdcall;
    function GetFlags(out pdwFlags: DWORD): HRESULT; stdcall;
    function SetAccountInformation(pwszAccountName: LPCWSTR;
      pwszPassword: LPCWSTR): HRESULT; stdcall;
    function GetAccountInformation(out ppwszAccountName: LPWSTR): HRESULT; stdcall;
  end;
  {$EXTERNALSYM IScheduledWorkItem}

const
  IID_ITask: TGUID = '{148BD524-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM IID_ITask}

// interface ITask

type
  ITask = interface(IScheduledWorkItem)
    ['{148BD524-A2AB-11CE-B11F-00AA00530503}']
    function SetApplicationName(pwszApplicationName: LPCWSTR): HRESULT; stdcall;
    function GetApplicationName(out ppwszApplicationName: LPWSTR): HRESULT; stdcall;
    function SetParameters(pwszParameters: LPCWSTR): HRESULT; stdcall;
    function GetParameters(out ppwszParameters: LPWSTR): HRESULT; stdcall;
    function SetWorkingDirectory(pwszWorkingDirectory: LPCWSTR): HRESULT; stdcall;
    function GetWorkingDirectory(out ppwszWorkingDirectory: LPWSTR): HRESULT; stdcall;
    function SetPriority(dwPriority: DWORD): HRESULT; stdcall;
    function GetPriority(out pdwPriority: DWORD): HRESULT; stdcall;
    function SetTaskFlags(dwFlags: DWORD): HRESULT; stdcall;
    function GetTaskFlags(out pdwFlags: DWORD): HRESULT; stdcall;
    function SetMaxRunTime(dwMaxRunTimeMS: DWORD): HRESULT; stdcall;
    function GetMaxRunTime(out pdwMaxRunTimeMS: DWORD): HRESULT; stdcall;
  end;
  {$EXTERNALSYM ITask}

const
  IID_IEnumWorkItems: TGUID = '{148BD528-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM IID_IEnumWorkItems}

// interface IEnumWorkItems

type
  IEnumWorkItems = interface
    ['{148BD528-A2AB-11CE-B11F-00AA00530503}']
    function Next(celt: ULONG; out rgpwszNames: PLPWSTR;
      out pceltFetched: ULONG): HRESULT; stdcall;
    function Skip(celt: ULONG): HRESULT; stdcall;
    function Reset: HRESULT; stdcall;
    function Clone(out ppEnumWorkItems: IEnumWorkItems): HRESULT; stdcall;
  end;
  {$EXTERNALSYM IEnumWorkItems}

const
  IID_ITaskScheduler: TGUID = '{148BD527-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM IID_ITaskScheduler}

// interface ITaskScheduler

type
  ITaskScheduler = interface
    ['{148BD527-A2AB-11CE-B11F-00AA00530503}']
    function SetTargetComputer(pwszComputer: LPCWSTR): HRESULT; stdcall;
    function GetTargetComputer(out ppwszComputer: LPWSTR): HRESULT; stdcall;
    function Enum(out ppEnumWorkItems: IEnumWorkItems): HRESULT; stdcall;
    function Activate(pwszName: PWideChar; var riid: TGUID;
      out ppUnk: IUnknown): HRESULT; stdcall;
    function Delete(pwszName: LPCWSTR): HRESULT; stdcall;
    function NewWorkItem(pwszTaskName: PWideChar; var rclsid: TGUID;
      var riid: TGUID; out ppUnk: IUnknown): HRESULT; stdcall;
    function AddWorkItem(pwszTaskName: LPCWSTR;
      var pWorkItem: IScheduledWorkItem): HRESULT; stdcall;
    function IsOfType(pwszName: LPCWSTR; var riid: TGUID): HRESULT; stdcall;
  end;
  {$EXTERNALSYM ITaskScheduler}

const
  CLSID_CTask: TGuid = '{148BD520-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM CLSID_CTask}

  CLSID_CTaskScheduler: TGuid = '{148BD52A-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM CLSID_CTaskScheduler}

type
  PTaskPage = ^TTaskPage;
  _TASKPAGE = (
    TASKPAGE_TASK,
    TASKPAGE_SCHEDULE,
    TASKPAGE_SETTINGS
    );
  {$EXTERNALSYM _TASKPAGE}
  TASKPAGE = _TASKPAGE;
  {$EXTERNALSYM TASKPAGE}
  TTaskPage = _TASKPAGE;

const
  IID_IProvideTaskPage: TGuid = '{4086658a-cbbb-11cf-b604-00c04fd8d565}';
  {$EXTERNALSYM IID_IProvideTaskPage}

// interface IProvideTaskPage

type
  IProvideTaskPage = interface
    ['{4086658A-CBBB-11CF-B604-00C04FD8D565}']
    function GetPage(tpType: TTaskPage; fPersistChanges: BOOL;
      var phPage: HPropSheetPage): HRESULT; stdcall;
  end;
  {$EXTERNALSYM IProvideTaskPage}

const
  ISchedulingAgent = ITaskScheduler;
  {$EXTERNALSYM ISchedulingAgent}
  IEnumTasks = IEnumWorkItems;
  {$EXTERNALSYM IEnumTasks}
  //IID_ISchedulingAgent   = IID_ITaskScheduler;
  IID_ISchedulingAgent: TGUID = '{148BD527-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM IID_ISchedulingAgent}
  //CLSID_CSchedulingAgent = CLSID_CTaskScheduler;
  CLSID_CSchedulingAgent: TGuid = '{148BD52A-A2AB-11CE-B11F-00AA00530503}';
  {$EXTERNALSYM CLSID_CSchedulingAgent}


//added to use save option without ole2 - not there in original source of mstask.h

  {$EXTERNALSYM IID_IPersistFile}
  IID_IPersistFile: TGUID = (
    D1: $0000010B; D2: $0000; D3: $0000; D4: ($C0, $00, $00, $00, $00, $00, $00, $46));


implementation

end.

