program ftp_tool;

{$APPTYPE CONSOLE}

uses
  Windows, SysUtils, IdBaseComponent, IdComponent, IdTCPConnection,
  IdTCPClient, IdFTP, INIFiles, Classes, zutil;

procedure AddToLog(S : string);
begin
 Writeln(S);
end;

function ExecuteProcess(AFileName, AParams, ADefDir: string;
  Mode, WaitTime: integer; Terminate : boolean; Var ExitCode : cardinal): boolean;
var
 StartupInfo         : TStartupInfo;
 ProcessInformation  : TProcessInformation;
 T : dword;
begin
 ExitCode := $FFFFFFFF;
 AddToLog('Run process '+AFileName+' '+AParams);
 // Запуск приложения согласно заданным настройкам
 FillChar( StartupInfo, SizeOf(StartupInfo), 0);
 with StartupInfo do begin
  cb := SizeOf(StartupInfo);
  dwFlags := startf_UseShowWindow;
  wShowWindow := Mode;
 end;
 // Создание процесса
 Result := CreateProcess(nil, PChar(AFileName+' '+AParams),nil,nil,false,Create_default_error_mode,nil,PChar(ADefDir),StartupInfo,ProcessInformation);
 if WaitTime > 0 then begin
  T := GetTickCount;
  while (WaitForSingleObject(ProcessInformation.hProcess, 500) = WAIT_TIMEOUT) and (GetTickCount - T < WaitTime) do begin
  end;
   // Остановка процесса
   if (WaitForSingleObject(ProcessInformation.hProcess, 0) = WAIT_TIMEOUT) and Terminate then begin
    AddToLog('Terminate Process !! Timeout = '+IntToStr(WaitTime));
    TerminateProcess(ProcessInformation.hProcess, 0);
    CloseHandle(ProcessInformation.hProcess);
   end else begin
    GetExitCodeProcess(ProcessInformation.hProcess, ExitCode);
    CloseHandle(ProcessInformation.hProcess);
   end;
 end;
end;

var
 FTP    : TIdFTP;
 INI    : TINIFile;
 FTPDir,  SavePath, ProfileFile : string;
 FTPFiles : tStrings;
 i : integer;
 ExitCode : cardinal;
 FileExt : string;
begin
 AddToLog('FTP file load tool. Zaiysev Oleg, 2007');
 AddToLog('ftp_tool.exe <profile_name>');
 FTP      := TIdFTP.Create(nil);
 FTPFiles := TStringList.Create;
 // Формирование имени профиля
 ProfileFile := ParamStr(1);
 if Pos(':', ProfileFile) = 0 then
  ProfileFile := NormalDir(ExtractFilePath(ParamStr(0))) + ProfileFile;
 if not(FileExists(ProfileFile)) then begin
  AddToLog('Profile not found !!');
  exit;
 end;
 INI := TIniFile.Create(ProfileFile);
 FTP.Host     := INI.ReadString('FTP', 'Host', '');
 FTP.Username := INI.ReadString('FTP', 'User', '');
 FTP.Password := INI.ReadString('FTP', 'Password', '');
 FTPDir       := INI.ReadString('FTP', 'Path', '');
 // Создание папки для принимаемых файлов
 SavePath := NormalDir(SavePath);
 try
   // Подключение к FTP серверу
   AddToLog('1. FTP.Connect, server = '+FTP.Host);
   FTP.Connect(true);
   AddToLog('FTP.Connect OK');
   // Переход в заданную папку
   AddToLog('2. FTP.ChangeDir, dir= '+FTPDir);
   FTP.ChangeDir(FTPDir);
   AddToLog(' >> FTP.ChangeDir OK');
   // Получение списка файлов и папок
   AddToLog('3. FTP.List');
   FTP.List(FTPFiles, '*.*');
   AddToLog(' >> FTP.List OK. Files count = '+IntToStr(FTP.DirectoryListing.Count));
   // Цикл загрузки файлов
   for i := 0 to FTP.DirectoryListing.Count - 1 do begin
    // Определение расширения
    FileExt := Uppercase(ExtractFileExt(FTP.DirectoryListing.Items[i].FileName));
    // Определение, как на него реагировать
    if INI.SectionExists(FileExt) then begin
     // Путь для сохранения
     SavePath        := INI.ReadString(FileExt, 'Path', '');
     if SavePath <> '' then begin
      if pos(':', SavePath) = 0 then
       SavePath :=  NormalDir(ExtractFilePath(ParamStr(0))) + SavePath;
      SavePath := NormalDir(SavePath);
      ForceDirectories(SavePath);
     end;
     // Загрузка
     if INI.ReadBool(FileExt, 'Download', false) then begin
      DeleteFile(SavePath + FTP.DirectoryListing.Items[i].FileName);
      AddToLog('Download file. FileName='+FTP.DirectoryListing.Items[i].FileName);
      FTP.Get(FTP.DirectoryListing.Items[i].FileName, SavePath + FTP.DirectoryListing.Items[i].FileName);
      AddToLog('File downloaded');
     end;
     // Удаление после обработки
     if INI.ReadBool(FileExt, 'Delete', false) then begin
      AddToLog('Delete file. FileName='+FTP.DirectoryListing.Items[i].FileName);
      FTP.Delete(FTP.DirectoryListing.Items[i].FileName);
      AddToLog('File downloaded. FileName='+FTP.DirectoryListing.Items[i].FileName);
     end;
     // Запуск доп. обработки принятого файла
     if INI.ReadString(FileExt, 'Run', '') <> '' then begin
      if FileExists(SavePath + FTP.DirectoryListing.Items[i].FileName) then begin
       AddToLog('Execute dop. tool. FileName='+FTP.DirectoryListing.Items[i].FileName +  ', Tool='+INI.ReadString(FileExt, 'Run', ''));
       ExecuteProcess(INI.ReadString(FileExt, 'Run', ''),
                      StringReplace(INI.ReadString(FileExt, 'RunParams', ''), '$FILENAME$', FTP.DirectoryListing.Items[i].FileName, [rfReplaceAll, rfIgnoreCase]),
                      INI.ReadString(FileExt, 'RunDefDir', ''),
                      4,
                      INI.ReadInteger(FileExt, 'RunTimeout', 60000),
                      INI.ReadBool(FileExt, 'RunTerminate', true), ExitCode);
      end;
      AddToLog('ExitCode = '+IntToStr(ExitCode));
      if (ExitCode = 0) and INI.ReadBool(FileExt, 'DeleteAfterRun', false) then begin
       DeleteFile(SavePath + FTP.DirectoryListing.Items[i].FileName);
       AddToLog('Delete file after tool execute. File Name = '+FTP.DirectoryListing.Items[i].FileName);
      end;
     end;
    end else
     AddToLog('File type not supported. File name='+FTP.DirectoryListing.Items[i].FileName);
   end;
 except
  on e : exception do
   AddToLog('ERROR: '+e.Message);
 end;
 INI.Free;
 FTP.Free;
 FTPFiles.Free;
end.
