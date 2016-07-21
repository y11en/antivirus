unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, SystemMonitor, zSharedFunctions, zAntivirus, Registry,
  ComCtrls, zAutoruns;

type
  TMain = class(TForm)
    Memo1: TMemo;
    Button1: TButton;
    StatusBar: TStatusBar;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   Mem : packed array of byte;
   DeletedFilesList : TStringList;
   SysPath : string;
   procedure AddToLog(S : string);
   procedure AddToDebugLog(S : string);
   function ScanMem(ASearchPattern : String) : boolean;
   function ScanProcesss(APID : integer) : boolean;
   function DelayDeleteFile(AFileName: string): boolean;
   procedure DelWinlogonNotifyByFileName(AFileName: string);
   procedure DelShellExt(ACLSID: string);
   function ExtSystemClean(ADeletedVirusList: TStrings): boolean;
   procedure DelCLSID(ACLSID: string);
   function AVZDeleteFile(AFileName: string): boolean;
  end;

var
  Main: TMain;

implementation
uses zutil;
{$R *.dfm}

function InjectDLLtoProcess(hProcess: THandle; ADllName: string): boolean;
var
 hRemoteThread  : THandle; // Handle удаленного потока
 NameBufPtr     : Pointer; // Адрес буфера с именем DLL
 LoadLibraryPtr : Pointer; // Адрес функции LoadLibrary
 NumberOfBytesWritten, ThreadId : dword;
begin
 Result := false;
 hRemoteThread := 0; NameBufPtr := nil;
 try
  if hProcess = 0 then exit;
  // 2. Создаем в памяти процесса буфер для имени DLL
  NameBufPtr := VirtualAllocEx(hProcess, nil, Length(ADllName)+1, MEM_COMMIT, PAGE_READWRITE);
  if NameBufPtr = nil then begin
   exit;
  end;
  // 3. Копируем имя в буфер
  if not(WriteProcessMemory(hProcess, NameBufPtr, PChar(ADllName), Length(ADllName)+1, NumberOfBytesWritten)) then begin
   exit;
  end;
  // 4. Выполняем определение адреса kernel32.dll!LoadLibraryA
  LoadLibraryPtr := GetProcAddress(GetModuleHandle('kernel32.dll'), 'LoadLibraryA');
  if LoadLibraryPtr = nil then begin
   exit;
  end;
  // 5. Выполняем создание удаленного потока
  hRemoteThread := CreateRemoteThread(hProcess, 0, 0,
                               LoadLibraryPtr, NameBufPtr, 0, ThreadId);
  if hRemoteThread <> 0 then begin
   // 6. Дожидаемся завершения потока (ждем 5 секунд)
   WaitForSingleObject(hRemoteThread, 500);
   Result := true;
  end;
 finally
  // Освобождение памяти и закрытие Handle потока и процесса
  if NameBufPtr <> nil then
   VirtualFreeEx(hProcess, NameBufPtr, 0, MEM_RELEASE);
  if hRemoteThread <> 0 then
   CloseHandle(hRemoteThread);
 end;
end;

procedure TMain.DelShellExt(ACLSID: string);
var
 Reg   : TRegistry;
 S : string;
begin
 if Trim(ACLSID) = '' then exit;
 Reg   := TRegistry.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey('SOFTWARE\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved', false) then
  Reg.DeleteValue(S);
 AddToLog('Удален элемент Shell Extensions '+S);
end;

procedure TMain.DelCLSID(ACLSID: string);
var
 Reg   : TRegistry;
begin
 if Trim(ACLSID) = '' then exit;
 AddToLog(' Удаление класса с CLSID = '+ACLSID);
 Reg   := TRegistry.Create;
 Reg.RootKey := HKEY_CLASSES_ROOT;
 if Reg.DeleteKey('CLSID\'+ACLSID) then
  AddToLog(' Удален класс с CLSID '+ACLSID);
 DelShellExt(ACLSID);
end;


function TMain.ExtSystemClean(ADeletedVirusList : TStrings): boolean;
var
 Reg : TRegistry;
 CLSIDList, VirusCLSIDList : TStringList;
 AutorunManager : TAutorunManager;
 i : integer;
 // Добавление CLSID списку, если он ссылается на файла
 procedure CheckCLSID(ACLSID, AFileName : string);
 var
  i : integer;
 begin
  AFileName := StringReplace(Trim(AFileName), '"', '', [rfReplaceAll]);
  if (AFileName = '') then exit;
  AFileName    := AnsiLowerCase(NTFileNameToNormalName(AFileName));
  if (pos(':', AFileName) = 0) then
   AFileName := SysPath + AFileName;
  // Поиск и добавление
  for i := 0 to ADeletedVirusList.Count - 1 do
   if (AFileName = ADeletedVirusList[i]) then begin
    VirusCLSIDList.Add(ACLSID);
   end;
 end;
begin
 if ADeletedVirusList.Count = 0 then exit;
 Screen.Cursor := crHourGlass;
 Reg := nil;
 CLSIDList := nil;
 VirusCLSIDList := nil;
 try
  Reg := TRegistry.Create;
  CLSIDList := TStringList.Create;
  VirusCLSIDList := TStringList.Create;
  Reg.RootKey := HKEY_CLASSES_ROOT;
  Reg.OpenKey('CLSID', false);
  Reg.GetKeyNames(CLSIDList);
  Reg.CloseKey;
  // Цикл анализа CLSID
  for i := 0 to CLSIDList.Count - 1 do begin
   if i mod 300 = 0 then begin
    Application.ProcessMessages;
   end;
   if Reg.OpenKey('CLSID\'+CLSIDList[i]+'\InprocServer32', false) then begin //#DNL
    if Reg.ValueExists('') then
     CheckCLSID(CLSIDList[i], Reg.ReadString(''));
    Reg.CloseKey;
   end;
   if Reg.OpenKey('CLSID\'+CLSIDList[i]+'\LocalServer32', false) then begin //#DNL
    if Reg.ValueExists('') then
     CheckCLSID(CLSIDList[i], Reg.ReadString(''));
    Reg.CloseKey;
   end;
   if Reg.OpenKey('CLSID\'+CLSIDList[i]+'\LocalServer', false) then begin  //#DNL
    if Reg.ValueExists('') then
     CheckCLSID(CLSIDList[i], Reg.ReadString(''));
    Reg.CloseKey;
   end;
  end;
  // Цикл удаления CLSID
  for i := 0 to VirusCLSIDList.Count - 1 do
   DelCLSID(VirusCLSIDList[i]);
  // Чистка автозапуска
  AutorunManager := nil;
  try
   AutorunManager := TAutorunManager.Create;
   AutorunManager.Refresh;
   for i := 0 to ADeletedVirusList.Count - 1 do begin
    // Удаление элементов автозапуска
    AutorunManager.DeleteFromAutorun(ADeletedVirusList[i]);
    // Чистка расширений WinLogon
    DelWinlogonNotifyByFileName(ADeletedVirusList[i]);
   end;
  finally
   AutorunManager.Free;
  end; 
 finally
  Reg.Free;
  CLSIDList.Free;
  VirusCLSIDList.Free;
  Screen.Cursor := crDefault;
 end;
end;


procedure TMain.DelWinlogonNotifyByFileName(AFileName: string);
var
 Reg   : TRegistry;
 Lines : TStringList;
 i     : integer;
 S     : string;
begin
 Reg   := TRegistry.Create;
 Lines := TStringList.Create;
 Reg.RootKey := HKEY_LOCAL_MACHINE;
 if Reg.OpenKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify', false) then begin
  Reg.GetKeyNames(Lines);
  Reg.CloseKey;
  // Цикл поиска ключа
  for i := 0 to Lines.Count - 1 do
   if Reg.OpenKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+Lines[i], false) then begin
    if Reg.ValueExists('DllName') then begin
     S :=  LowerCase(Trim(NTFileNameToNormalName(LowerCase(Trim(Reg.ReadString('DllName'))))));
     if ExtractFilePath(S) = '' then begin
      if FileExists(GetSystemDirectoryPath+S) then S := GetSystemDirectoryPath + S;
     end;
     if pos(LowerCase(AFileName), S) > 0  then begin
      Reg.DeleteValue('DllName');
      Reg.CloseKey;
      Reg.DeleteKey('SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon\Notify\'+Lines[i]);
      AddToLog('Удален элемент Winlogon, ссылающийся на '+AFileName);
     end;
    end;
    try Reg.CloseKey; except end;
   end;
 end;
 Reg.Free;
 Lines.Free;
end;

function TMain.DelayDeleteFile(AFileName: string): boolean;
var
 Lines : TStrings;
 i, poz     : integer;
begin
 Result := false;
 // Метод для Windows 95/98/ME
 if not(ISNT) then begin
  Lines := TStringList.Create;
  if FileExists(GetWindowsDirectoryPath + 'wininit.ini') then    //#DNL
   Lines.LoadFromFile(GetWindowsDirectoryPath + 'wininit.ini');  //#DNL
  poz := -1;
  for i := 0 to Lines.Count - 1 do
  if Trim(LowerCase(Lines[i])) = '[rename]' then begin          //#DNL
    poz := i;
    Break;
   end;
  if poz >= 0 then
   Lines.Insert(poz+1, 'NUL='+AFileName)                        //#DNL
    else begin
     Lines.Add('[rename]');                                     //#DNL
     Lines.Add('NUL='+AFileName);                               //#DNL
    end;
  Lines.SaveToFile(GetWindowsDirectoryPath + 'wininit.ini');    //#DNL
  Lines.Free;
 end;
 // Метод для NT/W2K/XP
 if ISNT then begin
  Result := MoveFileEx(PChar(AFileName), nil, MOVEFILE_DELAY_UNTIL_REBOOT);
 end;
end;

procedure TMain.Button1Click(Sender: TObject);
var
 SI : TSysProcessInfo;
 PL : TStrings;
 i : integer;
 S : string;
 Res : boolean;
begin
 DeletedFilesList.Clear;
 if ISNT then
  SI := TPSAPIProcessInfo.Create
   else SI := TToolHelpProcessInfo.Create;
 PL := TStringList.Create;
 SI.CreateProcessList(PL);
 for i := 0 to PL.Count - 1 do begin
  S := AnsiLowerCase(TProcessInfo(Pl.Objects[i]).ExeFileName);
  AddToLog('Проверка процесса '+TProcessInfo(Pl.Objects[i]).ExeFileName);
  Res := ScanProcesss(TProcessInfo(Pl.Objects[i]).PID);
  if Res and (pos('rundll', S) > 0) then begin
   SI.KillProcessByPID(TProcessInfo(Pl.Objects[i]).PID, false);
   AddToLog('>> Остановлен процесс '+S);
  end;
 end;
 for i := 0 to DeletedFilesList.Count - 1 do begin
   S := DeletedFilesList[i];
   DeletedFilesList[i] := AnsiLowerCase(DeletedFilesList[i]);
   DelWinlogonNotifyByFileName(S);
   DelWinlogonNotifyByFileName(ExtractFileName(S));
   DeleteFile(S);
   DelayDeleteFile(S);
   DelWinlogonNotifyByFileName(S);
   DelWinlogonNotifyByFileName(ExtractFileName(S));
   AddToLog('Удален файл '+S);
 end;
 ExtSystemClean(DeletedFilesList);
 SI.ClearList(PL);
 StatusBar.SimpleText := '';
end;


function TMain.ScanProcesss(APID: integer): boolean;
var
 SI : TSysProcessInfo;
 ML : TStrings;
 i : integer;
 hProcess : THandle;
 S : string;
 Tmp : TModuleInfo;
 RS : cardinal;
begin
 Result := false;
 if ISNT then
  SI := TPSAPIProcessInfo.Create
   else SI := TToolHelpProcessInfo.Create;
 ML := TStringList.Create;
 SI.CreateModuleList(ML, APID);
 hProcess := OpenProcess(PROCESS_ALL_ACCESS, true, APID);
 for i := 1 to ML.Count - 1 do begin
  Tmp := TModuleInfo(ML.Objects[i]);
  S := AnsiLowerCase(Tmp.ModuleName);
  SetLength(Mem, Tmp.ModuleSize);
  ZeroMemory(@Mem[0], Tmp.ModuleSize);
  ReadProcessMemory(hProcess, pointer(Tmp.hModule), @Mem[0], Tmp.ModuleSize, RS);
  if ScanMem('www.ad-w-a-r-e.com') and ScanMem('WinLogon') then begin
   Memo1.Lines.Add('>>>>>>> '+S);
   if InjectDLLtoProcess(hProcess, ExtractFilePath(Application.ExeName)+'antil2mr.dll') then
    Memo1.Lines.Add(' 1. Injected');
   DeletedFilesList.Add(S);
   Result := true;
  end;
  StatusBar.SimpleText := S;
 end;
end;

procedure TMain.FormCreate(Sender: TObject);
begin
 DeletedFilesList := TStringList.Create;
 // Активируем привилегию "Отладка"
 AdjustPrivileges('SeDebugPrivilege', true);       //#DNL
 AdjustPrivileges('SeLoadDriverPrivilege', true);  //#DNL
 AdjustPrivileges('SeBackupPrivilege', true);      //#DNL
 AdjustPrivileges('SeRestorePrivilege', true);     //#DNL
 SysPath := LowerCase(GetSystemDirectoryPath);
end;

function TMain.ScanMem(ASearchPattern: String): boolean;
var
 i, j : integer;
 Found : boolean;
begin
 Result := false;
 ASearchPattern := LowerCase(ASearchPattern);
 for i := 0 to Length(Mem) - 2 - Length(ASearchPattern) do
  if (LowerCaseArray[mem[i]] = byte(ASearchPattern[1])) and
     (LowerCaseArray[mem[i+1]] = byte(ASearchPattern[2])) then begin
     Found := true;
      for j := 0 to Length(ASearchPattern) do
       if LowerCaseArray[mem[i+j]] <> byte(ASearchPattern[j+1]) then begin
        Found := false;
        Break;
       end;
       if Found then begin
        Result := true;
        Exit;
       end;
     end;
end;

procedure TMain.AddToLog(S: string);
begin
 Memo1.Lines.Add(S);
end;

procedure TMain.AddToDebugLog(S: string);
begin

end;

function TMain.AVZDeleteFile(AFileName: string): boolean;
begin
 DeleteFile(AFileName);
 DelayDeleteFile(AFileName);
end;

end.
