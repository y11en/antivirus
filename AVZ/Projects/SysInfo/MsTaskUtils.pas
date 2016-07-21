unit MsTaskUtils;

interface

uses
  Windows, WinSvc, SysUtils;

//+--------------------------------------------------------------------------
//
//  Function:   StartScheduler
//
//  Synopsis:   Start the task scheduler service if it isn't already running.
//
//  Returns:    ERROR_SUCCESS or a Win32 error code.
//
//  Notes:      This function works for either Win9x or Windows NT.
//              If the service is running but paused, does nothing.
//
//---------------------------------------------------------------------------

function StartScheduler(): DWORD;

implementation

function StartScheduler(): DWORD;
const
  SCHED_CLASS = 'SAGEWINDOWCLASS';
  SCHED_TITLE = 'SYSTEM AGENT COM WINDOW';
  SCHED_SERVICE_APP_NAME = 'mstask.exe';
  SCHED_SERVICE_NAME = 'Schedule';
  MAX_PATH: DWORD = 256;
var
  StartupInfo: TStartupInfo;
  ProcessInformation: TProcessInformation;
  szApp: PChar;
  pszPath: PChar;
  hsc: SC_HANDLE;
  hSchSvc: SC_HANDLE;
  SvcStatus: TServiceStatus;
  ServiceArgVectors: PChar;
begin
  // Determine what version of OS we are running on.
  if Win32Platform = VER_PLATFORM_WIN32_WINDOWS then
  begin
    // Start the Win95 version of TaskScheduler.
    if FindWindow(SCHED_CLASS, SCHED_TITLE) <> 0 then
      // It is already running.
      Result := ERROR_SUCCESS
    else
    begin
      //
      //  Execute the task scheduler process.
      //
      FillChar(StartupInfo, sizeof(StartupInfo), 0);
      StartupInfo.cb := sizeof(TStartupInfo);

      GetMem(szApp, MAX_PATH);
      try
        if SearchPath(nil, SCHED_SERVICE_APP_NAME, nil, MAX_PATH,
          szApp, pszPath) = 0 then
          Result := GetLastError
        else
        begin
          if not CreateProcess(szApp, nil, nil, nil, False,
            CREATE_NEW_CONSOLE or CREATE_NEW_PROCESS_GROUP, nil, nil,
            StartupInfo, ProcessInformation) then
            Result := GetLastError
          else
          begin
            CloseHandle(ProcessInformation.hProcess);
            CloseHandle(ProcessInformation.hThread);
            Result := ERROR_SUCCESS;
          end;
        end;
      finally
        FreeMem(szApp);
      end;
    end;
  end
  else
  begin
    // If not Win95 then start the NT version as a TaskScheduler service.

    hsc := OpenSCManager(nil, nil, SC_MANAGER_CONNECT);

    if hsc = 0 then
      Result := GetLastError
    else
    begin
      hSchSvc := OpenService(hsc, SCHED_SERVICE_NAME,
        SERVICE_START or SERVICE_QUERY_STATUS);

      CloseServiceHandle(hsc);

      if hSchSvc = 0 then
        Result := GetLastError
      else
      begin
        if not QueryServiceStatus(hSchSvc, SvcStatus) then
        begin
          CloseServiceHandle(hSchSvc);
          Result := GetLastError;
        end
        else
        begin
          if SvcStatus.dwCurrentState = SERVICE_RUNNING then
          begin
            // The service is already running.
            CloseServiceHandle(hSchSvc);
            Result := ERROR_SUCCESS;
          end
          else
          begin
            ServiceArgVectors := nil;
            if not StartService(hSchSvc, 0, ServiceArgVectors) then
            begin
              CloseServiceHandle(hSchSvc);
              Result := GetLastError();
            end
            else
            begin
              CloseServiceHandle(hSchSvc);
              Result := ERROR_SUCCESS;
            end;
          end;
        end;
      end;
    end;
  end;
end;

end.
