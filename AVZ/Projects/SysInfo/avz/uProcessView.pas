unit uProcessView;
// Диспетчер процессов
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, StdCtrls, winsvc, Grids, RXGrids, SystemMonitor, RxVerInf, md5,
  ValEdit, Buttons, ExtCtrls, registry, ntdll, zprocess, RXSplit, Menus,
  zPELoader, ShellAPI, zHTMLTemplate, uRegSearch, zProcessMemory, 
  WinTrust, zAVZDriverN, zAVZDriverRK, zLogSystem, zAVKernel, zAVZKernel;

type
  // Описание окна
  TWindowRec = record
   Handle   : HWND;           // Handle окна
   Caption  : string;         // Заголовок окна
   Visible, Child : boolean;  // Признаки видимости и дочерноего окна
   IsIconic : boolean;        // Признак того, что окно минимизировано
   PID      : DWORD;          // PID окна
   Rect     : TRect;          // Координаты
   WndClass : string;         // Класс окна
  end;

  // Таблица со списком найденных окон
  TWindowsList = array of TWindowRec;

  TProcessView = class(TForm)
    pcMain: TPageControl;
    tsProcess: TTabSheet;
    pbDetail: TPageControl;
    TabSheet3: TTabSheet;
    Panel1: TPanel;
    TabSheet1: TTabSheet;
    cbCalkMD5Summ: TCheckBox;
    cbCheckFile: TCheckBox;
    dgDLL: TRxDrawGrid;
    dgProcessList: TRxDrawGrid;
    Splitter1: TSplitter;
    tsWindowsList: TTabSheet;
    dgWindowsList: TRxDrawGrid;
    Panel2: TPanel;
    sbCopyDllToQurantine: TSpeedButton;
    sbCopyAllDllToQurantine: TSpeedButton;
    sbRefreshDLL: TSpeedButton;
    Panel3: TPanel;
    sbCopyProcToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    sbRefreshPr: TSpeedButton;
    sbTerminateProcess: TSpeedButton;
    sbSaveProcLog: TSpeedButton;
    sbSaveDLLLog: TSpeedButton;
    pmProcessPopUp: TPopupMenu;
    N1: TMenuItem;
    N2: TMenuItem;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    pmDLLPopUp: TPopupMenu;
    MenuItem3: TMenuItem;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
    MenuItem7: TMenuItem;
    N5: TMenuItem;
    N6: TMenuItem;
    N7: TMenuItem;
    N8: TMenuItem;
    N9: TMenuItem;
    N10: TMenuItem;
    sbCreateDump: TSpeedButton;
    cbCheckWinTrust: TCheckBox;
    N11: TMenuItem;
    DLL1: TMenuItem;
    N12: TMenuItem;
    pmAVZGuard1: TMenuItem;
    procedure FormCreate(Sender: TObject);
    procedure sbTerminateProcessClick(Sender: TObject);
    procedure lvProcessListClick(Sender: TObject);
    procedure dgProcessListDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgDLLDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgProcessListFixedCellClick(Sender: TObject; ACol,
      ARow: Integer);
    procedure dgProcessListClick(Sender: TObject);
    procedure dgWindowsListDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgDLLClick(Sender: TObject);
    procedure sbCopyDllToQurantineClick(Sender: TObject);
    procedure sbRefreshDLLClick(Sender: TObject);
    procedure sbCopyAllDllToQurantineClick(Sender: TObject);
    procedure sbRefreshPrClick(Sender: TObject);
    procedure sbCopyProcToQurantineClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure cbCalkMD5SummClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure sbSaveDLLLogClick(Sender: TObject);
    procedure pbDetailChange(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure pmiGoogleClick(Sender: TObject);
    procedure pmiYandexClick(Sender: TObject);
    procedure N1Click(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure pmiRamblerClick(Sender: TObject);
    procedure MenuItem3Click(Sender: TObject);
    procedure MenuItem5Click(Sender: TObject);
    procedure MenuItem7Click(Sender: TObject);
    procedure MenuItem6Click(Sender: TObject);
    procedure N6Click(Sender: TObject);
    procedure N7Click(Sender: TObject);
    procedure N9Click(Sender: TObject);
    procedure N10Click(Sender: TObject);
    procedure sbCreateDumpClick(Sender: TObject);
    procedure pcMainChange(Sender: TObject);
    procedure DLL1Click(Sender: TObject);
    procedure pmAVZGuard1Click(Sender: TObject);
    procedure dgDLLFixedCellClick(Sender: TObject; ACol, ARow: Integer);
  private
    FLastModuleProcessDescr : string;
    FLastProcessID : dword;
    function CreateWindowList(APID : dword): boolean;
  public
   FSysProcessInfo               : TSysProcessInfo;
   FProcessInfoCache             : TProcessInfoArray;
   FProcessList, FModulesList    : TProcessInfoArray;
   ProcessOrderKey, ModuleOrderKey : integer;
   WindowsList                   : TWindowsList;
   ProcessInfoCache              : TProcessInfoArray;
   ProcessInfoCacheSize          : integer;
   // Создание списка процессов
   Function CreateProcessList : boolean;
   // Создание списка модулей
   Function CreateModuleList(APID : dword) : boolean;
   // Выполнение диалога
   function Execute : boolean;
   // Сортировка данных
   Function SortData(AKeyField : integer; InfoArray : TProcessInfoArray) : boolean;
   Function CompareItems(I1, I2 : TProcessFileInfo; AKeyField : integer) : boolean;
   // Обновление данных закладки "детали"
   function RefreshProcessDetail : boolean;
   // Добавление файла в кеш
   function AddFileToCache(ProcessFileInfo : TProcessFileInfo) : boolean;
   // Поиск файла в кеше
   function SearchFileInfoInCache(ProcessFileInfo : TProcessFileInfo) : integer;
   // Поиск файла в кеше
   function FillFileInfoFromCache(var ProcessFileInfo : TProcessFileInfo; Indx : integer) : boolean;
   function UpdateCacheInfo(ProcessFileInfo : TProcessFileInfo) : boolean;
   // ***** Создание протоколов *****
   // Форматирование кода видимости процесса
   function FormatVisible(ALevel: integer): string;
   // Создание HTML протокола для процессов
   Function CreateHTMLProcessLog(ALines : TStrings; HideGoodFiles  : boolean; AddDllReport : integer) : boolean;
   // Создание XML лога
   Function CreateXMLProcessLog(ALines : TStrings; HideGoodFiles  : boolean) : boolean;
   // Создание HTML протокола для DLL
   Function CreateHTMLModulesLog(ALines : TStrings; HideGoodFiles : boolean; CreateTableTag : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  ProcessView: TProcessView;

Function ExecuteProcessView : boolean;
// Создание списка процессов через NativeAPI
function CreateNativeProcessList(ALines : TStrings) : boolean;
function CreateNativeProcessListAntiRootKit(ALines : TStrings) : boolean;
// Создание списка процессов
function CreateProcessListByPM(ALines : TStrings) : boolean;
function KillProcessListByPM(APID : dword) : boolean;

// Поиск маскируемых процессов по хендлам
function SearchFUHiddenProc(AList : TStrings) : boolean;

implementation
uses
 zutil, orautil, zSharedFunctions, uGUIShared, zTranslate;
{$R *.dfm}

function TProcessView.CreateWindowList(APID : dword): boolean;
var
 WinHandle   : HWnd;
 S : String;
 Tmp : TWindowRec;
begin
 WindowsList := nil;
 WinHandle := GetWindow(Handle, gw_HWndFirst);
 try
   while WinHandle <> 0 do begin
    // Подготовка буфера
    SetLength(S, 1024);
    ZeroMemory(@S[1], length(S));
    GetWindowText(WinHandle, @S[1], Length(S));
    Tmp.Handle  := WinHandle;
    Tmp.Caption := Trim(S);
    Tmp.Visible := IsWindowVisible(WinHandle);
    Tmp.Child   := (GetWindow(WinHandle, gw_Owner) = 0);
    GetWindowThreadProcessId(WinHandle, Tmp.PID);
    GetWindowRect(WinHandle, Tmp.Rect);
    Tmp.IsIconic := IsIconic(WinHandle);
    SetLength(S, 1024);
    ZeroMemory(@S[1], length(S));
    GetClassName(WinHandle, @S[1], Length(S));
    Tmp.WndClass := Trim(S);
    if (APID = 0) or (APID = Tmp.PID) then begin
     SetLength(WindowsList, length(WindowsList)+1);
     WindowsList[Length(WindowsList)-1] := Tmp;
    end;
    // Поиск следующего окна
    WinHandle := GetWindow(WinHandle, gw_hWndNext);
   end;
   if Length(WindowsList) > 0 then
    dgWindowsList.RowCount := Length(WindowsList) + 1
     else dgWindowsList.RowCount := 2;
 except
  on e : exception do
   AddToDebugLog('WindowsList Error: '+e.Message);
 end;
 dgWindowsList.Refresh;
end;

function TProcessView.CreateProcessList: boolean;
var
 i, poz : integer;
 VersionInfo        : TVersionInfo;
 S, FileName, MD5_Hash : string;
 ProcessList, NativeProcessList  : TStringList;
 FoundFL : boolean;
 ProcessInfo        : TProcessInfo;
 lpFilePart: PChar;
 procedure AddProcessFromNativeList(ALevel : integer);
 var
  i, j : integer;
 begin
  for i := 0 to NativeProcessList.Count - 1 do begin
   FoundFL := false;
   for j := 0 to ProcessList.Count - 1 do begin
    if (ProcessList.Objects[j] as TProcessInfo).PID = integer(NativeProcessList.Objects[i]) then begin
     FoundFL := true;
     Break;
    end;
   end;
    if not(FoundFl) then begin
     ProcessInfo  := TProcessInfo.Create;
     ProcessInfo.PID         := integer(NativeProcessList.Objects[i]);
     if ProcessInfo.PID <= 4 then S := NativeProcessList[i] else begin
      SetLength(S, 200);
      ZeroMemory(@S[1], 200);
      SearchPath(nil, PChar(NativeProcessList[i]), nil, 200, @S[1], lpFilePart);
     end;
     ProcessInfo.ExeFileName := Trim(S);
     if ProcessInfo.ExeFileName = '' then
      ProcessInfo.ExeFileName := NativeProcessList[i];
     ProcessInfo.VisibleLevel := ALevel;
     ProcessList.AddObject('', ProcessInfo);
    end;
   end;
 end;
begin
 Screen.Cursor := crHourGlass;
 ProcessList := TStringList.Create;
 FProcessList := nil;
 ZProgressClass.AddProgressMax(4);
 // Запрос списка процессов
 FSysProcessInfo.CreateProcessList(ProcessList);
 ZProgressClass.ProgressStep;
 // Запрос списка процессов через Native API
 if ISNT then begin
  NativeProcessList := TStringList.Create;
  // Создание списка процессов через NativeAPI
  NativeProcessList.Clear;
  CreateNativeProcessList(NativeProcessList);
  ZProgressClass.ProgressStep;
  // Добавление процессов, которые не видит стандартный API
  AddProcessFromNativeList(1);
  ZProgressClass.ProgressStep;

  // Создание списка процессов через NativeAPI с антируткитной защитой
  NativeProcessList.Clear;
  CreateNativeProcessListAntiRootKit(NativeProcessList);
  // Добавление процессов, которые не видит стандартный API
  AddProcessFromNativeList(2);
  ZProgressClass.ProgressStep;

  // Создание списка процессов через NativeAPI с антируткитной защитой + защитой от FU
  NativeProcessList.Clear;
  SearchFUHiddenProc(NativeProcessList);
  // Добавление процессов, которые не видит стандартный API
  AddProcessFromNativeList(3);

  // Добавление процессов, которые не видит стандартный API но видит драйвер
  CreateProcessListByPM(NativeProcessList);
  AddProcessFromNativeList(3);

  NativeProcessList.Free;
 end;

 SetLength(FProcessList, ProcessList.Count);
 // Цикл заполнения списка и проверки файлов
 ZProgressClass.AddProgressMax(ProcessList.Count);
 for i := 0 to ProcessList.Count - 1 do begin
  ZProgressClass.ProgressStep;
  FProcessList[i].ProcessName := ExtractFileName((ProcessList.Objects[i] as TProcessInfo).ExeFileName);
  FProcessList[i].PID         := (ProcessList.Objects[i] as TProcessInfo).PID;
  FileName := NTFileNameToNormalName(Trim((ProcessList.Objects[i] as TProcessInfo).ExeFileName));
  FProcessList[i].FullProcessName := FileName;
  FProcessList[i].Descr := '';
  FProcessList[i].CmdLine := GetProcessCmdLineByPID(FProcessList[i].PID);
  FProcessList[i].LegalCopyright := '';
  FProcessList[i].VisibleLevel   := (ProcessList.Objects[i] as TProcessInfo).VisibleLevel;
  Poz := SearchFileInfoInCache(FProcessList[i]);
  if Poz >= 0 then FillFileInfoFromCache(FProcessList[i], Poz) else begin
   // Определение версии файла
   try
    VersionInfo := TVersionInfo.Create(FileName);
    FProcessList[i].Descr := VersionInfo.FileDescription;
    FProcessList[i].LegalCopyright := VersionInfo.LegalCopyright;
    VersionInfo.Free;
   except end;
   // Хеш MD5
   MD5_Hash := '??';
   if cbCalkMD5Summ.Checked then
    MD5_Hash := CalkFileMD5(FileName);
   FProcessList[i].MD5 := MD5_Hash;
   FProcessList[i].CheckResult := -1;
   FProcessList[i].VirResult   := -1;
   FProcessList[i].VirName     := '';
   // Проверка файла по базе безопасных
   if cbCheckFile.Checked then
    FProcessList[i].CheckResult := FileSignCheck.CheckFile(FileName, false);
   // Проверка файла по каталогу безопасности MS
   if cbCheckWinTrust.Checked and (FProcessList[i].CheckResult <> 0) then
    if CheckFileTrust(FileName) then
     FProcessList[i].CheckResult := 0;
   // Процесс 4 считаем безопасным всегда
   if FProcessList[i].PID = 4 then
    FProcessList[i].CheckResult := 0;
   AddFileToCache(FProcessList[i]);
  end;
 end;
 dgProcessList.RowCount := Length(FProcessList) + 1;
 SortData(ProcessOrderKey, FProcessList);
 dgProcessList.OnClick(nil);
 Application.ProcessMessages;
 dgProcessList.Refresh;
 Application.ProcessMessages;
 dgProcessList.Repaint;
 Screen.Cursor := crDefault;
end;


function TProcessView.Execute: boolean;
begin
 ProcessOrderKey := 0;
 ModuleOrderKey := 0;
 ProcessInfoCache := nil;
 // Загрузка базы чистых объектов
 if FileSignCheck.SignCount = 0 then
  FileSignCheck.LoadBinDatabase;
 CreateProcessList;
 SortData(1, FProcessList);
 dgProcessListClick(nil);
 pmAVZGuard1.Visible := AVZGuardIsOn;
 ShowModal;
end;

procedure TProcessView.FormCreate(Sender: TObject);
begin
 FProcessList        := nil;
 FModulesList        := nil;
 FProcessInfoCache   := nil;
 ProcessOrderKey     := 0;
 ProcessInfoCacheSize := 2000;
 if ISNT then
  FSysProcessInfo := TPSAPIProcessInfo.Create else
   FSysProcessInfo := TToolHelpProcessInfo.Create;
 cbCheckWinTrust.Enabled := LoadWintrustDLL;
 TranslateForm(Self);
end;

function TProcessView.CreateModuleList(APID : dword): boolean;
var
 i, Poz : integer;
 VersionInfo : TVersionInfo;
 FileName, MD5_Hash       : string;
 ModulesList : TStringList;
begin
 Result := false;
 try
   FModulesList := nil;
   Screen.Cursor := crHourGlass;
   ModulesList := TStringList.Create;
   FSysProcessInfo.CreateModuleList(ModulesList, APID);
   FLastModuleProcessDescr := IntToStr(APID);
   FLastProcessID          := APID;
   SetLength(FModulesList, ModulesList.Count);
   ZProgressClass.AddProgressMax(ModulesList.Count);
   for i := 0 to ModulesList.Count - 1 do begin
    ZProgressClass.ProgressStep;
    if i = 0 then
     FLastModuleProcessDescr := IntToStr(APID) + ' ' + (ModulesList.Objects[i] as TModuleInfo).ModuleName;
    FModulesList[i].ProcessName := ExtractFileName((ModulesList.Objects[i] as TModuleInfo).ModuleName);
    FModulesList[i].PID := (ModulesList.Objects[i] as TModuleInfo).hModule;
    FModulesList[i].Size := (ModulesList.Objects[i] as TModuleInfo).ModuleSize;
    FileName := NTFileNameToNormalName((ModulesList.Objects[i] as TModuleInfo).ModuleName);
    FModulesList[i].FullProcessName := FileName;
    FModulesList[i].Descr := '';
    FModulesList[i].VisibleLevel := 0;
    FModulesList[i].LegalCopyright := '';
    Poz := SearchFileInfoInCache(FModulesList[i]);
    if Poz >= 0 then FillFileInfoFromCache(FModulesList[i], Poz) else begin
     // Определение версии файла
     try
      VersionInfo := TVersionInfo.Create(FileName);
      FModulesList[i].Descr := VersionInfo.FileDescription;
      FModulesList[i].LegalCopyright :=  VersionInfo.LegalCopyright;
      VersionInfo.Free;
     except end;
      // Хеш MD5
      MD5_Hash := '--';
      if cbCalkMD5Summ.Checked then
       MD5_Hash := CalkFileMD5(FileName);
      FModulesList[i].MD5 := MD5_Hash;
      FModulesList[i].CheckResult := -1;
      FModulesList[i].VirResult   := -1;
      FModulesList[i].VirName     := '';
      // Проверка по базе безопасных
      if cbCheckFile.Checked then
       FModulesList[i].CheckResult := FileSignCheck.CheckFile(FileName, false);
      // Проверка файла по каталогу безопасности MS
      if cbCheckWinTrust.Checked and (FModulesList[i].CheckResult <> 0) then
       if CheckFileTrust(FileName) then
        FModulesList[i].CheckResult := 0;
      AddFileToCache(FModulesList[i]);
     end;
    end;
   if ModulesList.Count > 0 then
    dgDLL.RowCount := ModulesList.Count + 1
     else dgDLL.RowCount := 2;
   SortData(ModuleOrderKey, FModulesList);
   dgDLL.Refresh;
   dgDLL.OnClick(nil);
   FSysProcessInfo.ClearList(ModulesList);
   Screen.Cursor := crDefault;
 except
  on e : exception do
   AddToDebugLog('DLL List Error: '+e.Message);
 end;
 Result := true;
end;

procedure TProcessView.sbTerminateProcessClick(Sender: TObject);
begin
 FSysProcessInfo.KillProcessByPID(FProcessList[dgProcessList.Row-1].PID, false);
 KillProcessListByPM(FProcessList[dgProcessList.Row-1].PID);
 CreateProcessList;
end;

procedure TProcessView.lvProcessListClick(Sender: TObject);
begin
 CreateModuleList(FProcessList[dgProcessList.Row].PID);
end;

Function ExecuteProcessView : boolean;
begin
 ProcessView := nil;
 try
  ProcessView := TProcessView.Create(nil);
  ProcessView.Execute;
 finally
  ProcessView.Free;
  ProcessView := nil;
 end;
end;

function CreateNativeProcessList(ALines: TStrings): boolean;
var
  NTDLL_DLL : THandle;
  BufSize, ResBufSize: DWORD;
  Buf : Pointer;
  rel, cnt, res : integer;
  EndOfList : boolean;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
  S : string;
begin
 Result := false;
 ALines.Clear;
 Result := false;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := GetModuleHandle(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 // Вывод в протокол адреса функции
 BufSize := 100000;
 // Получение списка процессов
 repeat
  GetMem(Buf, BufSize);
  ZeroMemory(Buf, BufSize);
  Res := ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, Buf, BufSize, @ResBufSize);
  if Res = STATUS_INFO_LENGTH_MISMATCH then begin
   FreeMem(Buf, BufSize);
   BufSize := BufSize * 2;
  end;
 until Res <> STATUS_INFO_LENGTH_MISMATCH;
 if Res <> STATUS_SUCCESS then exit;
 rel := 0; cnt := 0;
 // Перемещение по списку и анализ
 repeat
  EndOfList := TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).NextEntryDelta = 0;
  if TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessName.Length > 0 then begin
    {hProcess := OpenProcess(SYNCHRONIZE or PROCESS_QUERY_INFORMATION, true, TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessID);
    Res := GetModuleFileName(hProcess, @s[1], length(S));
    CloseHandle(hProcess);
    S := copy(S, 1, Res);
    if S = '' then}
     S := String(TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessName.StrData);
    ALines.AddObject(S,
                     pointer(TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessID));
   end else ;
  inc(cnt);
  // Переход на следующую структуру, описывающую процесс
  rel := rel + TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).NextEntryDelta;
 until  EndOfList;
 // Освобождение памяти
 FreeMem(Buf, BufSize);
 Result := true;
end;

procedure TProcessView.dgProcessListDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 ProcessFileInfo : TProcessFileInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0874';
   1 : S := 'PID';
   2 : S := '$AVZ0581';
   3 : S := '$AVZ0852';
   4 : S := 'MD5';
   5 : S := '$AVZ1159';
   6 : S := '$AVZ0382';
   7 : S := '$AVZ0351';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  ProcessFileInfo := FProcessList[ARow-1];
  case ACol of
   0 : S := ProcessFileInfo.ProcessName;
   1 : S := IntToStr(ProcessFileInfo.PID);
   2 : S := ProcessFileInfo.Descr;
   3 : S := ProcessFileInfo.LegalCopyright;
   4 : S := ProcessFileInfo.MD5;
   5 : S := ProcessFileInfo.FullProcessName;
   6 : begin
        case ProcessFileInfo.VisibleLevel of
         0 : S := '$AVZ0522';
         1 : S := '$AVZ0523';
         2 : S := '$AVZ0201';
         3 : S := '$AVZ0200';
        end;
        S := TranslateStr(S);
       end;
   7 : S := ProcessFileInfo.CmdLine;
  end;
  if not((gdFocused in State) or (gdSelected in State)) then begin
   if ProcessFileInfo.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
   if ProcessFileInfo.VisibleLevel > 1 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clRed;
  end else begin
   if ProcessFileInfo.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Brush.Color := clGreen;
   if ProcessFileInfo.VisibleLevel > 1 then
    (Sender as TRxDrawGrid).Canvas.Brush.Color := clRed;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TProcessView.dgDLLDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 ProcessFileInfo : TProcessFileInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0437';
   1 : S := 'Handle';
   2 : S := '$AVZ0581';
   3 : S := '$AVZ0852';
   4 : S := 'MD5';
   5 : S := '$AVZ1159';
   6 : S := '$AVZ0892';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow > length(FModulesList) then exit;
  ProcessFileInfo := FModulesList[ARow-1];
  case ACol of
   0 : S := ProcessFileInfo.ProcessName;
   1 : S := IntToHex(ProcessFileInfo.PID, 8);
   2 : S := ProcessFileInfo.Descr;
   3 : S := ProcessFileInfo.LegalCopyright;
   4 : S := ProcessFileInfo.MD5;
   5 : S := ProcessFileInfo.FullProcessName;
   6 : S := IntToStr(ProcessFileInfo.Size);
  end;
  if not((gdFocused in State) or (gdSelected in State)) then begin
   if ProcessFileInfo.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
  end else begin
   if ProcessFileInfo.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Brush.Color := clGreen;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

function TProcessView.SortData(AKeyField: integer; InfoArray : TProcessInfoArray): boolean;
var
 i, j : integer;
 Tmp  : TProcessFileInfo;
begin
 if ModuleOrderKey < 0 then exit;
 for i := 0 to Length(InfoArray) - 2 do
  for j := i+1 to Length(InfoArray) - 1 do
   if CompareItems(InfoArray[i], InfoArray[j], AKeyField) then begin
    Tmp :=  InfoArray[i];
    InfoArray[i] := InfoArray[j];
    InfoArray[j] := Tmp;
   end;
end;

function TProcessView.CompareItems(I1, I2: TProcessFileInfo;
  AKeyField: integer): boolean;
begin
 Result := false;
 case AKeyField of
  0 : Result := AnsiLowerCase(I1.ProcessName) > AnsiLowerCase(I2.ProcessName);
  1 : Result := I1.PID > I2.PID;
  2 : Result := AnsiLowerCase(I1.Descr) > AnsiLowerCase(I2.Descr);
  3 : Result := AnsiLowerCase(I1.LegalCopyright) > AnsiLowerCase(I2.LegalCopyright);
  4 : Result := I1.MD5 > I2.MD5;
  5 : Result := AnsiLowerCase(I1.FullProcessName) > AnsiLowerCase(I2.FullProcessName);
 end;
end;

procedure TProcessView.dgProcessListFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 ProcessOrderKey := ACol;
 SortData(ACol, FProcessList);
 dgProcessList.Refresh;
end;

procedure TProcessView.dgProcessListClick(Sender: TObject);
begin
 RefreshProcessDetail;
 if dgProcessList.Row > length(FProcessList) then begin
  sbCopyProcToQurantine.Enabled := false;
  exit;
 end;
 sbCopyProcToQurantine.Enabled := not(FProcessList[dgProcessList.Row-1].CheckResult = 0);
end;

procedure TProcessView.dgWindowsListDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TWindowRec;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := 'Handle';
   1 : S := '$AVZ0246';
   2 : S := '$AVZ0103';
   3 : S := '$AVZ0355';
   4 : S := '$AVZ0335';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  if ARow > length(WindowsList) then exit;
  S := '';
  Tmp := WindowsList[ARow-1];
  case ACol of
   0 : S := IntToHex(Tmp.Handle, 6);
   1 : begin
        S := Tmp.Caption;
        if S = '' then S := TranslateStr('$AVZ0524')
       end;
   2 : if Tmp.Visible then S := TranslateStr('$AVZ0102') else S := TranslateStr('$AVZ0492');
   3 : begin
        S := IntToStr(Tmp.Rect.Left)+','+IntToStr(Tmp.Rect.Top)+' '+IntToStr(Tmp.Rect.Right)+','+IntToStr(Tmp.Rect.Bottom);
        if Tmp.IsIconic then S := 'Minimized';
       end;
   4 : begin
        S := Tmp.WndClass;
        if S = '' then S := TranslateStr('$AVZ0525')
       end;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

function TProcessView.RefreshProcessDetail: boolean;
begin
 CreateModuleList(FProcessList[dgProcessList.Row - 1].PID);
 CreateWindowList(FProcessList[dgProcessList.Row - 1].PID);
end;

procedure TProcessView.dgDLLClick(Sender: TObject);
begin
 if dgDLL.Row > length(FModulesList) then begin
  sbCopyDllToQurantine.Enabled := false;
  exit;
 end;
 sbCopyDllToQurantine.Enabled := not(FModulesList[dgDLL.Row-1].CheckResult = 0);
end;

procedure TProcessView.sbCopyDllToQurantineClick(Sender: TObject);
var
 FileName : string;
begin
 if dgDLL.Row > length(FModulesList) then begin
  exit;
 end;
 if zMessageDlg('$AVZ0360'+' "' + FModulesList[dgDLL.Row-1].FullProcessName+'"',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 FileName := FModulesList[dgDLL.Row-1].FullProcessName;
 CopyToInfected(FileName, '$AVZ0439 ', 'Quarantine');
end;

procedure TProcessView.sbRefreshDLLClick(Sender: TObject);
begin
 RefreshProcessDetail;
end;

procedure TProcessView.sbCopyAllDllToQurantineClick(Sender: TObject);
var
 i : integer;
begin
 if zMessageDlg('$AVZ0362',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 for i := 0 to Length(FModulesList) - 1 do
  if FModulesList[i].CheckResult <> 0 then
   CopyToInfected(FModulesList[i].FullProcessName, '$AVZ0439 ', 'Quarantine');
end;

procedure TProcessView.sbRefreshPrClick(Sender: TObject);
begin
 CreateProcessList;
end;

procedure TProcessView.sbCopyProcToQurantineClick(Sender: TObject);
var
 FileName : string;
begin
 if zMessageDlg('$AVZ0360'+' "'+FProcessList[dgProcessList.Row-1].FullProcessName+'"',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 FileName := FProcessList[dgProcessList.Row-1].FullProcessName;
 CopyToInfected(FileName, '$AVZ0986 ', 'Quarantine');
end;

procedure TProcessView.SpeedButton5Click(Sender: TObject);
var
 i : integer;
begin
 if zMessageDlg('$AVZ0364',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 for i := 0 to Length(FProcessList) - 1 do
  if FProcessList[i].CheckResult <> 0 then
   CopyToInfected(FProcessList[i].FullProcessName, '$AVZ0986 ', 'Quarantine');
end;

function TProcessView.AddFileToCache(
  ProcessFileInfo: TProcessFileInfo): boolean;
begin
 Result := false;
 if Length(FProcessInfoCache) > ProcessInfoCacheSize then exit;
 SetLength(FProcessInfoCache, Length(FProcessInfoCache)+1);
 ProcessFileInfo.FullProcessName := LowerCase(ProcessFileInfo.FullProcessName);
 FProcessInfoCache[Length(FProcessInfoCache)-1] := ProcessFileInfo;
end;

function TProcessView.SearchFileInfoInCache(
  ProcessFileInfo: TProcessFileInfo): integer;
var
 S : string;
 i : integer;
begin
 Result := -1;
 S := LowerCase(ProcessFileInfo.FullProcessName);
 for i := 0 to Length(FProcessInfoCache)-1 do
  if LowerCase(FProcessInfoCache[i].FullProcessName) = S then begin
   Result := i;
   exit;
  end;
end;

function TProcessView.FillFileInfoFromCache(
  var ProcessFileInfo: TProcessFileInfo; Indx: integer): boolean;
begin
 ProcessFileInfo.LegalCopyright := FProcessInfoCache[Indx].LegalCopyright;
 ProcessFileInfo.Descr          := FProcessInfoCache[Indx].Descr;
 ProcessFileInfo.CheckResult    := FProcessInfoCache[Indx].CheckResult;
 ProcessFileInfo.MD5            := FProcessInfoCache[Indx].MD5;
 ProcessFileInfo.VirResult      := FProcessInfoCache[Indx].VirResult;
 ProcessFileInfo.VirName        := FProcessInfoCache[Indx].VirName;
end;

procedure TProcessView.FormDestroy(Sender: TObject);
begin
 FProcessList        := nil;
 FModulesList        := nil;
 FProcessInfoCache   := nil;
 FSysProcessInfo.Free;
end;

procedure TProcessView.cbCalkMD5SummClick(Sender: TObject);
begin
 FProcessInfoCache   := nil;
end;

function CreateNativeProcessListAntiRootKit(ALines : TStrings) : boolean;
var
 PEImageLoader   :  TPEImageLoader; // Класс - загрузчик PE
 ExportFunctions :  TStrings;
 ZwQuerySystemInformation: TNativeQuerySystemInformation;
 BufSize, ResBufSize, rel, cnt : DWORD;
 Buf : Pointer;
 EndOfList : boolean;
 S : string;
 Res : NTSTATUS;
 StepId : integer;
begin
 Result := false;
 if not(ISNT) then exit;
 StepId := 0;
 PEImageLoader   := nil; ExportFunctions := nil;
 // Инициализация классов
 PEImageLoader   :=  TPEImageLoader.Create;
 StepId := 1;
 ExportFunctions :=  TStringList.Create;
 try
  try
    StepId := 2;
    // Загрузка DLL. Если ошибка, то выход
    if not(PEImageLoader.LoadPEFile(GetSystemDirectoryPath + 'ntdll.dll')) then exit;
    StepId := 3;
    // Пересчет таблицы релокейшенов
    PEImageLoader.ExecuteReallocation(cardinal(PEImageLoader.FS.Memory));
    StepId := 4;
    // Анализ таблицы импорта
    PEImageLoader.GetExportTable(ExportFunctions);
    @ZwQuerySystemInformation := nil;
    StepId := 5;
    PEImageLoader.SetupPageProtect;
    // Поиск функции. Если функция не найдена, то выход
    if ExportFunctions.IndexOf('ZwQuerySystemInformation') < 0 then exit;
    @ZwQuerySystemInformation := pointer(cardinal(PEImageLoader.FS.Memory) + PEImageLoader.GetPhAddByRVA(cardinal(ExportFunctions.Objects[ExportFunctions.IndexOf('ZwQuerySystemInformation')])));
    // Вывод в протокол адреса функции
    AddToDebugLog('ZwQuerySystemInformation = '+IntToHex(dword(@ZwQuerySystemInformation), 8));
    BufSize := 100000;
    repeat
     GetMem(Buf, BufSize);
     StepId := 6;
     // Получение списка процессов
     ZeroMemory(Buf, BufSize);
     Res := ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, Buf, BufSize, @ResBufSize);
     if Res = STATUS_INFO_LENGTH_MISMATCH then begin
      FreeMem(Buf, BufSize);
      BufSize := BufSize * 2;
     end;
    until Res <> STATUS_INFO_LENGTH_MISMATCH;
    // Если функция выполнена неуспешно, то выход
    if Res <> STATUS_SUCCESS then exit;
    rel := 0; cnt := 0;
    StepId := 7;
    // Перемещение по списку и анализ
    repeat
     StepId := 8;
     EndOfList := TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).NextEntryDelta = 0;
     StepId := 9;
     if TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessName.Length > 0 then begin
       SetLength(S, 200);
       StepId := 10;
       {hProcess := OpenProcess(SYNCHRONIZE or PROCESS_QUERY_INFORMATION, true, TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessID);
       Res := GetModuleFileName(hProcess, @s[1], length(S));
       CloseHandle(hProcess);
       S := copy(S, 1, Res);
       if S = '' then}
       S := String(TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessName.StrData);
       ALines.AddObject(S,
                       pointer(TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).ProcessID));
      end else ;
     inc(cnt);
     // Переход на следующую структуру, описывающую процесс
     rel := rel + TSystemProcessesAndThreadsInformation(Ptr(integer(Buf) + rel)^).NextEntryDelta;
    until  EndOfList;
    StepId := 10;
    // Освобождение памяти
    FreeMem(Buf, BufSize);
    Result := true;
   except
    on e : exception do
     AddToDebugLog('$AVZ0630 "'+e.Message+'", $AVZ1147 = ' + inttostr(StepId));
   end;
 finally
  PEImageLoader.Free;
  ExportFunctions.Free;
 end;
end;

function CreateProcessListByPM(ALines : TStrings) : boolean;
var
 PROCESS_LIST : TPROCESS_LIST;
 i : integer;
 ProcName, S : string;
begin
 Result := false;
 ALines.Clear;
 AddToDebugLog('CPL-PM [1]');
 if (AVZDriverRK = nil) or not(AVZDriverRK.Loaded) then  exit;
 // Установка нашему процессу права на управление драйвером
 AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID;
 if not(AVZDriverRK.IOCTL_Z_GET_PROCESSLIST(PROCESS_LIST)) then exit;
 try
  for i := 0 to $FF-1 do
   if PROCESS_LIST[i].PID <> $FFFFFFFF then begin
    ProcName := Trim(PROCESS_LIST[i].NAME);
    if AVZDriverRK.IOCTL_Z_GET_EPROCESS_FULLNAME(i, S) then
     if Length(S) > Length(ProcName) then
      ProcName := S;
    ALines.AddObject(ProcName, pointer(PROCESS_LIST[i].PID));
   end;
 except
   on e : exception do
     AddToDebugLog('CreateProcessListByPM: '+e.Message);
 end;
end;

function KillProcessListByPM(APID : dword) : boolean;
var
 PROCESS_LIST : TPROCESS_LIST;
 i : integer;
begin
 Result := false;
 if (AVZDriverRK = nil) or not(AVZDriverRK.Loaded) then  exit;
 // Установка нашему процессу права на управление драйвером
 AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID;
 if not(AVZDriverRK.IOCTL_Z_GET_PROCESSLIST(PROCESS_LIST)) then exit;
 try
  for i := 0 to $FF-1 do
   if PROCESS_LIST[i].PID <> $FFFFFFFF then begin
    if PROCESS_LIST[i].PID = APID then begin
     result := AVZDriverRK.IOCTL_Z_KILL_PROCESS(i);
     exit;
    end;
   end;
 except
   on e : exception do
     AddToDebugLog('KillProcessByPM: '+e.Message);
 end;
end;

function SearchFUHiddenProc(AList : TStrings) : boolean;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  Buf : pointer;
  BufSize, ResBufSize, i: DWORD;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
  S : string;
  PL1, Lines : TStrings;
  FSysProcessInfo : TSysProcessInfo;
  WinHandle   : HWnd;
  PID : dword;
  procedure AddProcessToList(APID : DWORD);
  var
   FoundFl : boolean;
   j       : integer;
  begin
    FoundFl := false;
    // Поиск процесса в списке видимых
    for j := 0 to Lines.Count - 1 do
     if integer(Lines.Objects[j]) = APID then begin
      FoundFl := true;
      Break;
     end;
     // Не найден - добавим без повторов
     if not(FoundFl) and (AList.IndexOfObject(pointer(APID))<0) then begin
      // Первая попытка определения имени - через API
      FSysProcessInfo.CreateModuleList(PL1, APID);
      if PL1.Count > 0 then
       S := LowerCase((PL1.Objects[0] as TModuleInfo).ModuleName) else S := '';
      // Вторая попытка определения имени - чтением PEB
      if  S = '' then
       try S := GetProcessImageNameByPID(APID); except end;
      // Добавляем при условии, что нашли имя или похожий на правду PID
      if (APID > 10) then
       AList.AddObject(S, pointer(APID));
     end;
  end;
begin
 Result := false;
 AList.Clear;
 if not(ISNT) then exit;
 // Создание диспетчера процессов
 if ISNT then
  FSysProcessInfo := TPSAPIProcessInfo.Create else
   FSysProcessInfo := TToolHelpProcessInfo.Create;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := GetModuleHandle(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 AddToDebugLog('SHP: ZwQuerySystemInformation = '+IntToHex(dword(@ZwQuerySystemInformation), 8));
 if @ZwQuerySystemInformation = nil then exit;
 try
  try
   // Вывод в протокол адреса функции
   BufSize := 500000;
   repeat
    // Выделение буфера
    GetMem(Buf, BufSize);
    // Запрос информации
    AddToDebugLog('SHP: QSI[1]');
    R := ZwQuerySystemInformation(SystemHandleInformation, Buf, BufSize, @ResBufSize);
    AddToDebugLog('SHP: QSI[2] - '+IntToStr(ResBufSize));
    // Буфер недостаточного размера ?? Если да, то увеличим его в два раза и повторим
    if R = STATUS_INFO_LENGTH_MISMATCH then begin
     FreeMem(Buf, BufSize);
     BufSize := BufSize * 2;
    end;
   until R <> STATUS_INFO_LENGTH_MISMATCH;
   AddToDebugLog('SHP: QSI OK');
   // Функция выполнена неуспешно ?? Тогда выход
   if R <> STATUS_SUCCESS then exit;
   Lines       := TStringList.Create;
   PL1         := TStringList.Create;
   // Создание списка процессов с защитой от руткита UserMode
   AddToDebugLog('SHP: [1]');
   CreateNativeProcessListAntiRootKit(Lines);
   AddToDebugLog('SHP: [2]');
   // Процесс анализа HANDLE
   for i := 0 to TSystemHandleInformationRec(Buf^).NumberOfHandles - 1 do begin
    AddProcessToList(TSystemHandleInformationRec(Buf^).Information[i].ProcessId);
   end;
   AddToDebugLog('SHP: [3]');
   // 2. Поиск по окнам, работает естественно только для GUI (но помогает от FU версии 2+)
   WinHandle := GetWindow(Application.Handle, gw_HWndFirst);
   AddToDebugLog('SHP: [4]');
   while WinHandle <> 0 do begin
    GetWindowThreadProcessId(WinHandle, PID);
    AddProcessToList(PID);
    // Поиск следующего окна
    WinHandle := GetWindow(WinHandle, gw_hWndNext);
   end;
   Result := true;
   AddToDebugLog('SHP: OK');
  except
   on e : Exception do begin
    AddToDebugLog('SHP:  Error = '+e.Message);
    Result := false;
   end;
  end;
 finally
  // Освобождение памяти
  if Buf <> nil then
   try FreeMem(Buf, BufSize); except end;
  FSysProcessInfo.Free;
  Lines.Free;
  PL1.Free;
 end;
end;


function TProcessView.CreateHTMLModulesLog(ALines: TStrings; HideGoodFiles : boolean; CreateTableTag : boolean): boolean;
var
 i, RepProc, GoodProc : integer;
 S, S1, FileInfoStr : string;
begin
 Result := false;
 if CreateTableTag then begin
  ALines.Add('<TABLE '+HTML_TableFormat1+'>');
  ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0304'+HTML_TableHeaderTD+'Handle'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'Copyright'+HTML_TableHeaderTD+'MD5'+HTML_TableHeaderTD+'$AVZ0382');
 end;
 GoodProc := 0; RepProc := 0;
 for i := 0 to Length(FModulesList) - 1 do begin
  if not(HideGoodFiles) or (HideGoodFiles and (FModulesList[i].CheckResult <> 0)) then begin
   inc(RepProc);
   S :='';
   if FModulesList[i].CheckResult = 0 then
    S := 'bgColor='+HTMP_BgColorTrusted
     else S := 'bgColor='+HTMP_BgColor2;
   if FModulesList[i].VisibleLevel > 1 then S1 := '<b><font color=red>' else S1 := '';
   FileInfoStr := FormatFileInfo(FModulesList[i].FullProcessName, ', ');
   ALines.Add('<TR '+S+'>'+
              '<TD>'+'<a title="'+FileInfoStr+'" href="">'+FModulesList[i].FullProcessName+'</a>'+GenScriptTxt(FModulesList[i].FullProcessName)+
              '<TD>'+IntToStr(FModulesList[i].PID)+
              '<TD>'+HTMLNVL(DelCRLF(FModulesList[i].Descr))+
              '<TD>'+HTMLNVL(DelCRLF(FModulesList[i].LegalCopyright))+
              '<TD>'+HTMLNVL(FModulesList[i].MD5)+
              '<TD>'+'$AVZ0522'
             );
  end;
  if (FModulesList[i].CheckResult = 0) then
   Inc(GoodProc);
 end;
 Result := RepProc > 0;
 if CreateTableTag then begin
  ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=6>$AVZ0540:'+IntToStr(Length(FModulesList))+', $AVZ0293 '+IntToStr(GoodProc));
  ALines.Add('</TABLE>');
 end;
 // Перевод протокола
 TranslateLines(ALines);
end;

function TProcessView.FormatVisible(ALevel: integer): string;
begin
 Result := '?';
 case ALevel of
  0 : Result := '$AVZ0522';
  1 : Result := '$AVZ0523';
  2 : Result := '$AVZ0239';
 end;
end;

function TProcessView.CreateHTMLProcessLog(ALines: TStrings; HideGoodFiles : boolean; AddDllReport : integer): boolean;
var
 i, j, RepProc, GoodProc : integer;
 S, S1, RepLine, FileInfoStr, CmdLine : string;
 DLLRep : TStringList;
 ModulesDistinctList : TProcessInfoArray;
 Flag1 : boolean;
 // Добавление без повторов (контроль по имени)
 function AddModuleDistinct(PI : TProcessFileInfo; APID : DWORD) : boolean;
 var
  i : integer;
  S, PidStr : string;
 begin
  Result := not(HideGoodFiles) or (HideGoodFiles and (PI.CheckResult <> 0));
  S := AnsiLowerCase(Trim(PI.FullProcessName));
  PidStr := '<a href="#proc_'+IntToStr(APID)+'">'+IntToStr(APID)+'</a>';
  // Поиск по базе
  for i := 0 to Length(ModulesDistinctList) - 1 do
   if AnsiLowerCase(Trim(ModulesDistinctList[i].FullProcessName)) = S then begin
    AddToList(ModulesDistinctList[i].Prim, PIDStr, ', ');
    exit;
   end;
  // Добавление
  SetLength(ModulesDistinctList, Length(ModulesDistinctList)+1);
  ModulesDistinctList[Length(ModulesDistinctList)-1] := PI;
  ModulesDistinctList[Length(ModulesDistinctList)-1].Prim := PIDStr;
 end;
begin
 Result := false;
 ModulesDistinctList := nil;
 DLLRep := TStringList.Create;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'PID'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'Copyright'+HTML_TableHeaderTD+'MD5'+HTML_TableHeaderTD+'$AVZ0311');
 GoodProc := 0; RepProc := 0;
 for i := 0 to Length(FProcessList) - 1 do begin
   inc(RepProc);
   S1 :='';
   if FProcessList[i].CheckResult = 0 then
    S := 'bgColor='+HTMP_BgColorTrusted else
     S := 'bgColor='+HTMP_BgColor2;
   if FProcessList[i].VisibleLevel > 1 then
     S := 'bgColor='+HTMP_BgColor3;
    FileInfoStr := '';
    if FProcessList[i].VisibleLevel > 1 then FileInfoStr := FormatVisible(FProcessList[i].VisibleLevel);
    AddToList(FileInfoStr, FormatFileInfo(FProcessList[i].FullProcessName));
    CmdLine :=GetProcessCmdLineByPID(FProcessList[i].PID);
    if CmdLine <> '' then
     CmdLine := '<BR>' + CmdLine;
    RepLine := '<TR '+S+'>'+
               '<TD>'+'<a name="proc_'+inttostr(FProcessList[i].PID)+'"></a>'+S1+FProcessList[i].FullProcessName + GenScriptTxt(FProcessList[i].FullProcessName, true)+
               '<TD>'+S1+IntToStr(FProcessList[i].PID)+
               '<TD>'+S1+HTMLNVL(DelCRLF(FProcessList[i].Descr))+
               '<TD>'+S1+HTMLNVL(DelCRLF(FProcessList[i].LegalCopyright))+
               '<TD>'+S1+HTMLNVL(FProcessList[i].MD5)+
               '<TD>'+S1+FileInfoStr+'<br>$AVZ0351: '+CmdLine;
   if AddDllReport in [1,2] then begin
    DLLRep.Clear;
    CreateModuleList(FProcessList[i].PID);
    // Режим = 1 - со списком DLL внутри
    if AddDllReport = 1 then
     if CreateHTMLModulesLog(DLLRep, HideGoodFiles, false) then begin
      ALines.Add(RepLine);
      RepLine := '';
      ALines.AddStrings(DLLRep);
     end;
    // Режим = 2 - со списком DLL отдельно без повтором
    if AddDllReport = 2 then begin
     Flag1 := false;
     for j := 1 to length(FModulesList)-1 do
      Flag1 := AddModuleDistinct(FModulesList[j], FProcessList[i].PID) or Flag1;
     if Flag1 then begin
      ALines.Add(RepLine);
      RepLine := '';
     end;
    end;
   end;
   if not(HideGoodFiles) or (HideGoodFiles and (FProcessList[i].CheckResult <> 0)) then
    if RepLine <> '' then
     ALines.Add(RepLine);
  if (FProcessList[i].CheckResult = 0) then
   Inc(GoodProc);
 end;
 Result := RepProc > 0;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=6>$AVZ0538:'+IntToStr(Length(FProcessList))+', $AVZ0293 '+IntToStr(GoodProc));
 ALines.Add('</TABLE>');
 // Режим = 2 - со списком DLL отдельно без повтором
 if AddDllReport = 2 then begin
  // Сортировка таблицы
  SortData(5, ModulesDistinctList);
  ALines.Add('<TABLE '+HTML_TableFormat1+'>');
  ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0304'+HTML_TableHeaderTD+'Handle'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'Copyright'+HTML_TableHeaderTD+'MD5'+HTML_TableHeaderTD+'$AVZ0321');
  GoodProc := 0; RepProc := 0;
  for i := 0 to Length(ModulesDistinctList) - 1 do begin
   if not(HideGoodFiles) or (HideGoodFiles and (ModulesDistinctList[i].CheckResult <> 0)) then begin
    inc(RepProc);
    S :='';
    if ModulesDistinctList[i].CheckResult = 0 then
     S := 'bgColor='+HTMP_BgColorTrusted
      else S := 'bgColor='+HTMP_BgColor2;
    if ModulesDistinctList[i].VisibleLevel > 1 then S1 := '<b><font color=red>' else S1 := '';
   FileInfoStr := FormatFileInfo(ModulesDistinctList[i].FullProcessName, ', ');
   ALines.Add('<TR '+S+'>'+
              '<TD>'+'<a href="" title="'+FileInfoStr+'">'+ModulesDistinctList[i].FullProcessName+'</a>'+GenScriptTxt(ModulesDistinctList[i].FullProcessName)+
               '<TD>'+IntToStr(ModulesDistinctList[i].PID)+
               '<TD>'+HTMLNVL(DelCRLF(ModulesDistinctList[i].Descr))+
               '<TD>'+HTMLNVL(DelCRLF(ModulesDistinctList[i].LegalCopyright))+
               '<TD>'+HTMLNVL(ModulesDistinctList[i].MD5)+
               '<TD>'+HTMLNVL(ModulesDistinctList[i].Prim)
              );
   end;
   if (ModulesDistinctList[i].CheckResult = 0) then
    Inc(GoodProc);
  end;
  ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=6>$AVZ0540:'+IntToStr(Length(ModulesDistinctList))+', $AVZ0293 '+IntToStr(GoodProc));
  ALines.Add('</TABLE>');
 end;
 DLLRep.Free;
 ModulesDistinctList := nil;
 TranslateLines(ALines);
end;

procedure TProcessView.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ1038</H2>'));
 CreateHTMLProcessLog(RepLines, false, 0);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_process.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

procedure TProcessView.sbSaveDLLLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TRanslateStr('<H2 align=center>$AVZ1036 '+FLastModuleProcessDescr+'</H2>'));
 CreateHTMLModulesLog(RepLines, false, true);
 GUIShared.HTMLSaveDialog.FileName := 'avz_modules.htm';
 RepLines.Add('</BODY></HTML>');
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

procedure TProcessView.pbDetailChange(Sender: TObject);
begin
 if pbDetail.ActivePage = tsWindowsList then
  RefreshProcessDetail;
end;

procedure TProcessView.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
 case Key of
  VK_F5 : sbRefreshPr.Click;
  VK_F1 : Application.HelpJump('t_process');
 end;
end;

procedure TProcessView.pmiGoogleClick(Sender: TObject);
begin
 WebSearchInGoogle(FProcessList[dgProcessList.Row-1].ProcessName);
end;

procedure TProcessView.pmiYandexClick(Sender: TObject);
begin
 WebSearchInYandex(FProcessList[dgProcessList.Row-1].ProcessName);
end;

procedure TProcessView.N1Click(Sender: TObject);
begin
 sbTerminateProcess.Click;
end;

procedure TProcessView.N3Click(Sender: TObject);
begin
 sbCopyProcToQurantine.Click;
end;

procedure TProcessView.pmiRamblerClick(Sender: TObject);
begin
 WebSearchInRambler(FProcessList[dgProcessList.Row-1].ProcessName);
end;

procedure TProcessView.MenuItem3Click(Sender: TObject);
begin
 sbCopyDllToQurantine.Click;
end;

procedure TProcessView.MenuItem5Click(Sender: TObject);
begin
 WebSearchInGoogle(FModulesList[dgDLL.Row-1].ProcessName);
end;

procedure TProcessView.MenuItem7Click(Sender: TObject);
begin
 WebSearchInRambler(FModulesList[dgDLL.Row-1].ProcessName);
end;

procedure TProcessView.MenuItem6Click(Sender: TObject);
begin
 WebSearchInYandex(FModulesList[dgDLL.Row-1].ProcessName);
end;

function TProcessView.AddAllToQurantine: boolean;
var
 i, j : integer;
begin
 for i := 0 to Length(FProcessList) - 1 do begin
  // Добавление процесса
  if FProcessList[i].CheckResult <> 0 then
   CopyToInfected(FProcessList[i].FullProcessName, '$AVZ0975', 'Quarantine');
  CreateModuleList(FProcessList[i].PID);
  // Добавление библиотек
  for j := 0 to Length(FModulesList) - 1 do
   if FModulesList[j].CheckResult <> 0 then
    CopyToInfected(FModulesList[j].FullProcessName, '$AVZ0975', 'Quarantine');
 end;
end;

procedure TProcessView.N6Click(Sender: TObject);
begin
 ExecuteRegSearch(ExtractFileName(FProcessList[dgProcessList.Row-1].ProcessName));
end;

procedure TProcessView.N7Click(Sender: TObject);
begin
 ExecuteRegSearch(FProcessList[dgProcessList.Row-1].FullProcessName);
end;

procedure TProcessView.N9Click(Sender: TObject);
begin
 ExecuteRegSearch(FModulesList[dgDLL.Row-1].ProcessName);
end;

procedure TProcessView.N10Click(Sender: TObject);
begin
 ExecuteRegSearch(FModulesList[dgDLL.Row-1].FullProcessName);
end;

function TProcessView.UpdateCacheInfo(
  ProcessFileInfo: TProcessFileInfo): boolean;
var
 S : string;
 i : integer;
begin
 Result := false;
 S := LowerCase(ProcessFileInfo.FullProcessName);
 for i := 0 to Length(FProcessInfoCache)-1 do
  if LowerCase(FProcessInfoCache[i].FullProcessName) = S then begin
   FProcessInfoCache[i] := ProcessFileInfo;
   exit;
  end;
end;

procedure TProcessView.sbCreateDumpClick(Sender: TObject);
var
 FileName : string;
 Buf      : array of byte;
 hProcess : THandle;
 lpNumberOfBytesRead: DWORD;
 f        : file;
begin
 if dgDLL.Row > length(FModulesList) then begin
  exit;
 end;
 // Запрет дампа смоего процесса
 if UpperCase(FModulesList[dgDLL.Row-1].FullProcessName) = UpperCase(Application.ExeName) then exit;
 // Имя файла дампа
 FileName := ChangeFileExt(ExtractFileName(FModulesList[dgDLL.Row-1].FullProcessName), '.mem');
 FileName := ExtractFilePath(Application.ExeName) + 'DMP\' + FileName;
 zForceDirectories(ExtractFilePath(FileName));
 if zMessageDlg('$AVZ1006'+ ' '+ FModulesList[dgDLL.Row-1].FullProcessName,
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 SetLength(Buf, FModulesList[dgDLL.Row-1].Size);
 ZeroMemory(@Buf[0], FModulesList[dgDLL.Row-1].Size);
 hProcess := OpenProcess(PROCESS_ALL_ACCESS, true, FProcessList[dgProcessList.Row-1].PID);

 ReadProcessMemory(hProcess, pointer(FModulesList[dgDLL.Row-1].PID), @Buf[0], FModulesList[dgDLL.Row-1].Size, lpNumberOfBytesRead);
 CloseHandle(hProcess);
 AssignFile(F, FileName); Rewrite(f, 1);
 BlockWrite(F, Buf[0], FModulesList[dgDLL.Row-1].Size);
 CloseFile(F);
 Buf := nil;
end;

procedure TProcessView.pcMainChange(Sender: TObject);
begin
 FProcessInfoCache := nil;
end;

procedure TProcessView.DLL1Click(Sender: TObject);
begin
 if dgDLL.Row > length(FModulesList) then begin
  exit;
 end;
 if zMessageDlg('$AVZ0172',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 if RemoteFreeLibrary(FLastProcessID,  FModulesList[dgDLL.Row-1].PID) then begin
  RefreshProcessDetail;
 end;

end;

procedure TProcessView.pmAVZGuard1Click(Sender: TObject);
begin
 if SetAVZGuardTrustedProcess(FProcessList[dgProcessList.Row-1].PID, true) then begin
  zMessageDlg('$AVZ0874 '+FProcessList[dgProcessList.Row-1].ProcessName+' $AVZ0221', mtInformation, [mbOk], 0);
 end;
end;

procedure TProcessView.dgDLLFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 ModuleOrderKey := ACol;
 SortData(ACol, FModulesList);
 dgDLL.Refresh;
end;


// Создание XML лога
function TProcessView.CreateXMLProcessLog(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, j, RepProc, GoodProc : integer;
 S, RepLine, CmdLine : string;
 DLLRep : TStringList;
 ModulesDistinctList : TProcessInfoArray;
 Flag1 : boolean;
 // Добавление без повторов (контроль по имени)
 function AddModuleDistinct(PI : TProcessFileInfo; APID : DWORD) : boolean;
 var
  i : integer;
  S, PidStr : string;
 begin
  Result := not(HideGoodFiles) or (HideGoodFiles and (PI.CheckResult <> 0));
  S := AnsiLowerCase(Trim(PI.FullProcessName));
  PidStr := IntToStr(APID);
  // Поиск по базе
  for i := 0 to Length(ModulesDistinctList) - 1 do
   if AnsiLowerCase(Trim(ModulesDistinctList[i].FullProcessName)) = S then begin
    AddToList(ModulesDistinctList[i].Prim, PIDStr, ',');
    exit;
   end;
  // Добавление
  SetLength(ModulesDistinctList, Length(ModulesDistinctList)+1);
  ModulesDistinctList[Length(ModulesDistinctList)-1] := PI;
  ModulesDistinctList[Length(ModulesDistinctList)-1].Prim := PIDStr;
 end;
begin
 Result := false;
 ModulesDistinctList := nil;
 // Не сортировать список DLL
 ModuleOrderKey      := -1;
 DLLRep := TStringList.Create;
 ALines.Add(' <PROCESS>');
 GoodProc := 0; RepProc := 0;
 ZProgressClass.AddProgressMax(Length(FProcessList));
 for i := 0 to Length(FProcessList) - 1 do begin
   ZProgressClass.ProgressStep;
   inc(RepProc);
   S :='';
   if FProcessList[i].CheckResult = 0 then
    S := S + 'Hidden="'+IntToStr(FProcessList[i].VisibleLevel-1)+ '" ';
    CmdLine :=GetProcessCmdLineByPID(FProcessList[i].PID);
    RepLine := '  <ITEM PID="'+inttostr(FProcessList[i].PID)+'" '+
                   'File="'+XMLStr(FProcessList[i].FullProcessName)+'" '+
                   'CheckResult="'+IntToStr(FProcessList[i].CheckResult)+'" '+
                   'Descr="'+XMLStr(DelCRLF(FProcessList[i].Descr))+'" '+
                   'LegalCopyright="'+XMLStr(DelCRLF(FProcessList[i].LegalCopyright))+'" '+
                   S +
                   ' CmdLine="'+XMLStr(CmdLine)+'" '+
                   FormatXMLFileInfo(FProcessList[i].FullProcessName)+' />';
    // Режим = 2 - со списком DLL отдельно без повтором
   Flag1 := false;
   CreateModuleList(FProcessList[i].PID);
   // Цикл с 1, а не с 0 - пропускаем первый модуль - это сам EXE
   for j := 1 to length(FModulesList)-1 do
    Flag1 := AddModuleDistinct(FModulesList[j], FProcessList[i].PID) or Flag1;
   if not(HideGoodFiles) or (HideGoodFiles and (FProcessList[i].CheckResult <> 0)) or Flag1 then
    if RepLine <> '' then
     ALines.Add(RepLine);
 end;
 Result := RepProc > 0;
 ALines.Add(' </PROCESS>');
 // Режим = 2 - со списком DLL отдельно без повторов
  // Сортировка таблицы
  SortData(5, ModulesDistinctList);
  ALines.Add(' <DLL>');
  GoodProc := 0; RepProc := 0;
  for i := 0 to Length(ModulesDistinctList) - 1 do begin
   if not(HideGoodFiles) or (HideGoodFiles and (ModulesDistinctList[i].CheckResult <> 0)) then begin
    inc(RepProc);
   S :='Hidden="'+IntToStr(ModulesDistinctList[i].VisibleLevel - 1)+'"';
   ALines.Add('  <ITEM File="'+XMLStr(ModulesDistinctList[i].FullProcessName)+'" '+
                 'CheckResult="'+IntToStr(ModulesDistinctList[i].CheckResult)+'" '+
                 ' Descr="'+XMLStr(DelCRLF(ModulesDistinctList[i].Descr))+'" '+
                 ' LegalCopyright="'+XMLStr(DelCRLF(ModulesDistinctList[i].LegalCopyright))+'" '+
                 ' UsedBy="'+XMLStr(ModulesDistinctList[i].Prim)+'" '+
                 S + ' '+
                 FormatXMLFileInfo(ModulesDistinctList[i].FullProcessName) + ' />'
              );
   end;
  end;
 ALines.Add(' </DLL>');
 DLLRep.Free;
 ModulesDistinctList := nil;
end;

end.


