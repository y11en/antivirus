unit Unit1;
// Контроль версий - version.txt
interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, INIFiles,
  StdCtrls, ComCtrls, Dialogs,
  ExtCtrls, Buttons, ToolEdit, Menus, math,
  RxMenus, XPMan, RXShell, RXSpin,
  ImgList,
  SystemMonitor,
  zStringCompressor,
  zTreeViews, zPELoader, zLSP,
  zUserScripting,
  zIpPorts,
  zServices,
  zAutoruns,
  zVirFileList,
  zDirectFileCopy,
  zAntivirus,
  zAVZDriver, zAVZDriverN, zAVZDriverRK,
  zAVZArcKernel,
  zProcessMemory,
  uAutoQurantine,
  wintrust,
  ntdll,  uCookies,
  zKeyloggerHunter,
  uSelectDeletedFile,
  zSharedFunctions,
  zLogSystem,
  zAVZKernel,
  zAVKernel,
  zScriptingKernel,
  zAVZVersion,
  zHTMLTemplate,
  zTranslate,
  zCodepageFix,
  zBackupEngine;

type
  TMain = class(TForm)
    GroupBox1: TGroupBox;
    StatusBar: TStatusBar;
    Timer1: TTimer;
    SaveDialog: TSaveDialog;
    MainMenu1: TMainMenu;
    mmFile: TMenuItem;
    mmExit: TMenuItem;
    mmHelp: TMenuItem;
    N4: TMenuItem;
    mmService: TMenuItem;
    mmShowInfected: TMenuItem;
    N6: TMenuItem;
    mmStart: TMenuItem;
    mmStop: TMenuItem;
    N9: TMenuItem;
    N10: TMenuItem;
    SpeedButton2: TSpeedButton;
    sbDetailLogView: TSpeedButton;
    N11: TMenuItem;
    N12: TMenuItem;
    Panel1: TPanel;
    psScanZone: TPageControl;
    tsSelectDisk: TTabSheet;
    ZShellTreeView: TZShellTreeView;
    cbCheckProcess: TCheckBox;
    cbEvSysCheck: TCheckBox;
    TabSheet2: TTabSheet;
    GroupBox2: TGroupBox;
    Label7: TLabel;
    lbEvLevel: TLabel;
    tbEvLevel: TTrackBar;
    GroupBox3: TGroupBox;
    cbCheckRootKit: TCheckBox;
    cbAntiRootKitSystemUser: TCheckBox;
    gbSetup: TGroupBox;
    lbClassVir: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label8: TLabel;
    cbDeleteVirus: TCheckBox;
    cbModeVirus: TComboBox;
    cbModeAdvWare: TComboBox;
    cbModeSpy: TComboBox;
    cbModeHackTools: TComboBox;
    cbModeRiskWare: TComboBox;
    cbInfected: TCheckBox;
    cbQuarantine: TCheckBox;
    cbModePornWare: TComboBox;
    mbStart: TBitBtn;
    mbStop: TBitBtn;
    TabSheet1: TTabSheet;
    rgFileTypes: TRadioGroup;
    cbIncludeFiles: TCheckBox;
    edIncludeFiles: TEdit;
    edExcludeFiles: TEdit;
    cbExcludeFiles: TCheckBox;
    pmDirTreeMenu: TPopupMenu;
    mmSelectHDD: TMenuItem;
    mmSelectCDROM: TMenuItem;
    mmSelectFDD: TMenuItem;
    N20: TMenuItem;
    mmRefresh: TMenuItem;
    N22: TMenuItem;
    mmServiceView: TMenuItem;
    mmLSPView: TMenuItem;
    Label1: TLabel;
    Label2: TLabel;
    N14: TMenuItem;
    GroupBox4: TGroupBox;
    Label9: TLabel;
    cbCheckLSP: TCheckBox;
    cbAutoRepairLSP: TCheckBox;
    cbKeyloggerSearch: TCheckBox;
    N13: TMenuItem;
    CPUDP1: TMenuItem;
    N15: TMenuItem;
    N16: TMenuItem;
    N23: TMenuItem;
    N24: TMenuItem;
    N25: TMenuItem;
    tRunScan: TTimer;
    N17: TMenuItem;
    N18: TMenuItem;
    N19: TMenuItem;
    N21: TMenuItem;
    MsConfig1: TMenuItem;
    cbRepGood: TCheckBox;
    cbSearchTrojanPorts: TCheckBox;
    pbScanProgress: TProgressBar;
    cbExtEv: TCheckBox;
    N26: TMenuItem;
    N27: TMenuItem;
    IEBHO1: TMenuItem;
    OpenDialog1: TOpenDialog;
    N28: TMenuItem;
    N29: TMenuItem;
    N30: TMenuItem;
    N31: TMenuItem;
    N32: TMenuItem;
    mmSysCheck: TMenuItem;
    cbAntiRootKitSystemKernel: TCheckBox;
    mmKernelModules: TMenuItem;
    mmRepairSystem: TMenuItem;
    RxTrayIcon: TRxTrayIcon;
    cbCheckArchives: TCheckBox;
    XPManifest1: TXPManifest;
    cbMaxArcSize: TCheckBox;
    seMaxArcSize: TRxSpinEdit;
    Label10: TLabel;
    cbExtFileDelete: TCheckBox;
    cbRepGoodCheck: TCheckBox;
    cbCheckNTFSStream: TCheckBox;
    N34: TMenuItem;
    pmAVZTrayMenu: TRxPopupMenu;
    pmStart: TMenuItem;
    pmStop: TMenuItem;
    N37: TMenuItem;
    pmExit: TMenuItem;
    N2: TMenuItem;
    tRunScript: TTimer;
    tWatchDog: TTimer;
    DLL1: TMenuItem;
    N35: TMenuItem;
    mmCheckWintrust: TMenuItem;
    N38: TMenuItem;
    N36: TMenuItem;
    N39: TMenuItem;
    mmAutoUpdate: TMenuItem;
    cbHosts: TMenuItem;
    SpeedButton1: TSpeedButton;
    N41: TMenuItem;
    N42: TMenuItem;
    mbStopScript: TBitBtn;
    mmRunScript: TMenuItem;
    mmSG_TurnOn: TMenuItem;
    mmSG: TMenuItem;
    mmSG_TrustedRun: TMenuItem;
    mmSG_TurnOff: TMenuItem;
    N44: TMenuItem;
    mmStdScripts: TMenuItem;
    N46: TMenuItem;
    mmRevizor: TMenuItem;
    DownloadedProgramFiles1: TMenuItem;
    CPL1: TMenuItem;
    N48: TMenuItem;
    N49: TMenuItem;
    sbAutoUpdate: TSpeedButton;
    N50: TMenuItem;
    mbPause: TSpeedButton;
    mmPause: TMenuItem;
    pmPause: TMenuItem;
    N51: TMenuItem;
    pmShow: TMenuItem;
    mmPMControl: TMenuItem;
    mmPMDriverInstall: TMenuItem;
    mmPMDriverRemove: TMenuItem;
    N7: TMenuItem;
    mmSaveSetupProfile: TMenuItem;
    N52: TMenuItem;
    ProfileSaveDialog: TSaveDialog;
    ProfileOpenDialog: TOpenDialog;
    mmPMDriverUnloadRemove: TMenuItem;
    ActiveSetup1: TMenuItem;
    LogMemo: TRichEdit;
    cbEvSysIPU: TCheckBox;
    mmExtLogView: TMenuItem;
    mmBackup: TMenuItem;
    FontDialog1: TFontDialog;
    mmWizards: TMenuItem;
    mmCalkMD5: TMenuItem;
    procedure FormCreate(Sender: TObject);
    procedure mbStartClick(Sender: TObject);
    procedure mbStopClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure mmExitClick(Sender: TObject);
    procedure N4Click(Sender: TObject);
    procedure mmShowInfectedClick(Sender: TObject);
    procedure ZShellTreeViewTreeNodeStateIconClick(Sender: TObject;
      Node: TTreeNode);
    procedure tbEvLevelChange(Sender: TObject);
    procedure N11Click(Sender: TObject);
    procedure mmStartClick(Sender: TObject);
    procedure mmStopClick(Sender: TObject);
    procedure cbDeleteVirusClick(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure N10Click(Sender: TObject);
    procedure mmRefreshClick(Sender: TObject);
    procedure N22Click(Sender: TObject);
    procedure mmSelectHDDClick(Sender: TObject);
    procedure lbHelpLabelClick(Sender: TObject);
    procedure mmServiceViewClick(Sender: TObject);
    procedure mmLSPViewClick(Sender: TObject);
    procedure N14Click(Sender: TObject);
    procedure cbCheckRootKitClick(Sender: TObject);
    procedure CPUDP1Click(Sender: TObject);
    procedure N15Click(Sender: TObject);
    procedure N23Click(Sender: TObject);
    procedure N24Click(Sender: TObject);
    procedure sbDetailLogViewClick(Sender: TObject);
    procedure tRunScanTimer(Sender: TObject);
    procedure N17Click(Sender: TObject);
    procedure N21Click(Sender: TObject);
    procedure MsConfig1Click(Sender: TObject);
    procedure cbCheckLSPClick(Sender: TObject);
    procedure N27Click(Sender: TObject);
    procedure N29Click(Sender: TObject);
    procedure N31Click(Sender: TObject);
    procedure mmSysCheckClick(Sender: TObject);
    procedure IEBHO1Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure mmKernelModulesClick(Sender: TObject);
    procedure mmRepairSystemClick(Sender: TObject);
    procedure RxTrayIconClick(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure OnFileCountThreadProgress(Sender: TObject; FileCount, FullFilesCount : int64; ScanComplete : boolean);
    procedure cbCheckArchivesClick(Sender: TObject);
    procedure cbRepGoodClick(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure FormResize(Sender: TObject);
    procedure N34Click(Sender: TObject);
    procedure pmExitClick(Sender: TObject);
    procedure pmStartClick(Sender: TObject);
    procedure pmStopClick(Sender: TObject);
    procedure N2Click(Sender: TObject);
    procedure tRunScriptTimer(Sender: TObject);
    procedure tWatchDogTimer(Sender: TObject);
    procedure DLL1Click(Sender: TObject);
    procedure N35Click(Sender: TObject);
    procedure ZShellTreeViewKeyPress(Sender: TObject; var Key: Char);
    procedure mmCheckWintrustClick(Sender: TObject);
    procedure N38Click(Sender: TObject);
    procedure N36Click(Sender: TObject);
    procedure mmAutoUpdateClick(Sender: TObject);
    procedure cbHostsClick(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure cbIncludeFilesClick(Sender: TObject);
    procedure cbExcludeFilesClick(Sender: TObject);
    procedure rgFileTypesClick(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
    procedure N41Click(Sender: TObject);
    procedure N42Click(Sender: TObject);
    procedure mbStopScriptClick(Sender: TObject);
    procedure mmRunScriptClick(Sender: TObject);
    procedure mmSG_TurnOnClick(Sender: TObject);
    procedure mmSG_TrustedRunClick(Sender: TObject);
    procedure mmSG_TurnOffClick(Sender: TObject);
    function CheckCanClose : boolean;
    procedure N44Click(Sender: TObject);
    procedure mmStdScriptsClick(Sender: TObject);
    procedure mmRevizorClick(Sender: TObject);
    procedure DownloadedProgramFiles1Click(Sender: TObject);
    procedure CPL1Click(Sender: TObject);
    procedure N49Click(Sender: TObject);
    procedure sbAutoUpdateClick(Sender: TObject);
    procedure N50Click(Sender: TObject);
    procedure ZShellTreeViewNewTreeNodeSetState(Sender: TObject; Node,
      NewNode: TTreeNode; var State: Integer);
    procedure mbPauseClick(Sender: TObject);
    procedure pmDirTreeMenuPopup(Sender: TObject);
    procedure pmShowClick(Sender: TObject);
    procedure mmPauseClick(Sender: TObject);
    procedure pmPauseClick(Sender: TObject);
    procedure mmPMDriverInstallClick(Sender: TObject);
    procedure mmPMDriverRemoveClick(Sender: TObject);
    procedure mmSaveSetupProfileClick(Sender: TObject);
    procedure N52Click(Sender: TObject);
    procedure mmPMDriverUnloadRemoveClick(Sender: TObject);
    procedure ActiveSetup1Click(Sender: TObject);
    procedure mmExtLogViewClick(Sender: TObject);
    procedure mmBackupClick(Sender: TObject);
    procedure mmWizardsClick(Sender: TObject);
    procedure mmCalkMD5Click(Sender: TObject);
  private
    FDefaultWindowProc: TWndMethod; // WindowProc of the associated control
    FVirCount: integer;                    // Кол-во найденных зловредов
    FScanCount, FScanArcCount : integer;   // Просканировано файлов и архивов
    FPodVirCount: integer;                 // Кол-во подозрений
    FFullScanFileCount, FDopFileCount : integer;
    // Настройка счетчиков
    procedure SetScanCount(const Value: integer);
    procedure SetVirCount(const Value: integer);
    procedure SetPodVirCount(const Value: integer);
    // Блокировка/разблокировка интерфейса
    procedure LockInterface(ALockState : boolean);
    // Запрос уровня эвристики
    function GetEvLevel: integer;
    // Добавление в протокол
    procedure OnAddToLog(AMsg : string; InfoType : integer);
    // Добавление в протокол
    procedure OnAddToDebugLog(AMsg : string);
    // Запрос протокола
    procedure OnGetLog(ALog : TStrings);
    // Обработчик связи с GUI
    function GUILink(Sender : TObject; OpCode : integer; AMessage, AParam : string; var Res : variant) : boolean;
    // Обработчик связи с подсистемой настройки
    function SetupLink(Sender : TObject; ASetting : string) : boolean;
    Procedure WMGetSysCommand(var Message :TMessage); message WM_SYSCOMMAND;
    Procedure WMCloseHandler(var Message :TMessage); message WM_CLOSE;
    function OnAVZFileExtracted(AVZArcInfo : TAVZArcInfoArray) : boolean;
    function OnAVZCheckFilename(AFileName : string) : boolean;
    function OnAVZDecompressLog(AMSG : string; InfoType : integer) : boolean;
    procedure MemSuspFound(AVirName : string);
    procedure SubClassWndProc(var Message: TMessage);
    function CreateScanPathStr: string;
  public
   // Флаги
   ScanAllFiles, ScanDangerFiles, ScanInProgress : boolean;
   // Время начала сканирования (мс)
   StartScanTime : integer;
   NeuroTokenList    : TStrings;
   // Признак "создавать копию раскакованных файлов"
   CopyUnpackedFiles : boolean;
   UserScriptText : string;
   // Маски на включение и на исключение
   IncludeMackList, ExcludeMaskList,
   // Список файлов, сканируемых помимо найденных в ходе обхода отмеченных папок
   ScanFileList      : TStringList;
   // Признак режима "урезанного" протоколирования - в лог выводятся только основные данные
   MiniLogMode : boolean;
   // Подготовка списка включения/исключения файлов
   procedure RefrechFilterList;
   // Сканирование папки
   function ScanDir(ADir: string; ADeleteF, AScanSubdir: boolean): boolean;
   // Сканирование каталогов в соответствии с деревом
   function ScanTreeDirs(ARootNode : TTreeNode; ADeleteF: boolean): boolean;
   // Поиск троянских портов
   function SearchPorts : boolean;
   // Сканирование памяти
   function ScanMemory(ADeleteF: boolean): boolean;
   function AVScanProcessMemory(AFileName  : string; APID : dword; AHidden : boolean) : boolean;
   // Сканирование файла на вирусы
   function VirusScan(AFileName: string; DeleteF: boolean; ProcessInfo : TProcessInfo; ArcInfo : TAVZArcInfoArray; IsFileSteam : boolean=false): integer;
   // Сканирование NTFS потока
   function VirusScanNTFSStreams(AFileName: string; DeleteF: boolean): integer;
   // Проверка, разрешено ли удаление вируса указанной группы
   function DeleteGroupEnabled(AVirusName : string) : integer;
   // Интеллектуальная проверка
   function IQ_Check(AFileName: string; ProcessInfo : TProcessInfo; PEFileInfo : TAVZFileInfo) : boolean;
   // Проверка скрипта
   function Script_Check(AFileName: string) : boolean;
   // Обновление данных интерфейса
   Function RefreshInterface : boolean;
   // Обновление списка дисков и директорий в древоводном списке проверяемых объектов
   function RefreshDirInfo : boolean;
   // Получение адреса процесса в памяти
   function GetPEImageAddress : dword;
   // Настройка статуса родительской ветви дерева папок
   procedure SetParentStatus(ANode : TTreeNode);
   // Запрос текстового описания текущего режима леничения
   function GetCureModeStr : string;
   // Управление драйвером AVZ PM
   Function InstallPMDriver : integer;
   Function UninstallPMDriver(AUnload : boolean) : integer;
   procedure OnMemSignFound(MemSign : TMemSign; Rel : DWORD);
   // ******** Свойства ********
   // Кол-во отсканированных файлов
   property ScanCount : integer read FScanCount write SetScanCount;
   // Кол-во вирусов
   property VirCount  : integer read FVirCount write SetVirCount;
   // Кол-во подозрительных объектов
   property PodVirCount : integer read FPodVirCount write SetPodVirCount;
   // Уровень эвритстики
   property EvLevel : integer read GetEvLevel;
  end;

var
  Main: TMain;

implementation
uses zutil, orautil, uAboutDLG, uInfectedView, uProcessView,
  uSPIView, uRegSearch,uServiceView, uOpenPorts, uFileSearch, uExtLogView,
  uNetShares, uFileCountThread, uAutoruns, uSysCheck, uIESettings,
  uKernelObjectsView, uSystemRepair, uBHOManager, uDeleteModeDLG,
  uExplorerExtManager, uQurantineByList, uKeyloggerManager,
  uPrintExtManager, uAutoUpdate, uTaskScheduler, uHostsManager,
  uExecProcessDLG, uProtocolManager, uStdScripts, uRevizor,
  uFileDeleteModeDLG, uDPFManager, uCPLManager, uRunScript, uLast,
  uActiveSetup, uGUIShared, uBackupScripts, uRepairWizard;
{$R *.DFM}


// Сканирование файла на вирусы
function TMain.VirusScan(AFileName: string; DeleteF : boolean; ProcessInfo : TProcessInfo; ArcInfo : TAVZArcInfoArray; IsFileSteam : boolean=false): integer;
var
 FileScanner     : TFileScanner;
 VirusDescr      : TVirusDescr;
 SysProcessInfo  : TSysProcessInfo;
 DeleteModeCode  : integer;
 S, TempFile : string;
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
 // Прерывание операции
 if StopF then
  exit;
 // Задержка (применяется на очень медленных ПК для снижения нагрузки)
 if SleepScanTime > 0 then Sleep(SleepScanTime);
 ScanCount := ScanCount + 1;
 FileScanner := TFileScanner.Create;
 try
  // Проверка файла
  if not(FileScanner.CheckFile(AFileName)) then begin
   try
    TempFile := AVZTempDirectoryPath + 'avz_'+IntToStr(GetCurrentProcessId)+'_raw.tmp';
    if not(DirectCopyFile(AFileName, TempFile)) then begin
     Result := -1;
     exit;
    end;
    AddToLog('$AVZ0881 '+AFileName, logInfo);
    FileScanner.CheckFile(TempFile);
    DeleteFile(TempFile);
   except
    Result := -1;
    exit;
   end;
  end;
  // Анализ результата
  Result := MainAntivirus.CheckFile(FileScanner.PEFileInfo, VirusDescr);
  case Result of
   0 : begin
        if cbExtEv.Checked then
         IQ_Check(AFileName, ProcessInfo, FileScanner.PEFileInfo);
        if pos(UpperCase(ExtractFileExt(AFileName)), ScanFilesEXT_Script) > 0 then
         Script_Check((AFileName));
        if cbRepGood.Checked then begin
          if cbRepGoodCheck.Checked then
           if FileSignCheck.CheckFile(AFileName) = 0 then
            S := ', $AVZ0454' else
             S := ',$AVZ0087';
          AddToLog(LogFileName + ' - $AVZ1143'+S, logInfo);
        end;
       end;
   1 : begin
        VirCount := VirCount + 1;
        S := '';
        if DeleteF then begin
         DeleteModeCode := DeleteGroupEnabled(MainAntivirus.VirusName);
         if DeleteModeCode = 2 then begin
          DeleteModeCode := ExecuteDeleteModeDLG(AFileName, MainAntivirus.VirusName);
          if DeleteModeCode = 10 then begin
           mbStop.Click;
           DeleteModeCode := 0;
           exit;
          end;
         end;
         // Удаление процесса из памяти
         if ProcessInfo <> nil then begin
          SysProcessInfo  := TSysProcessInfo.Create;
          SysProcessInfo.KillProcessByPID(ProcessInfo.PID, true);
          S := ' $AVZ0877 ';
         end;
         // Удаление файла с диска
         if DeleteModeCode = 1  then begin
          // Удаление файла
          if DeleteVirus(AFileName, MainAntivirus.VirusName, VirusDescr.Script, MainAntivirus.VirusUnregisterDLL) then
           S := ' $AVZ1104' else S := ' $AVZ0683 ';
         end else begin
          S  := ' $AVZ1075';
          VirFileList.AddVir(AFileName, ' '+MainAntivirus.VirusName, vrtVirus, VirusDescr.Script);
         end;
        end else VirFileList.AddVir(GetBaseFileName, ' '+MainAntivirus.VirusName, vrtVirus, VirusDescr.Script);
        AddToLog(LogFileName+' >>>>> '+MainAntivirus.VirusName+' ' + S, logVirus);
       end;
   2 : begin
        if FileSignCheck.CheckFile(AFileName) = 0 then exit;
        PodVirCount := PodVirCount + 1;
        AddToLog(LogFileName+' >>> $AVZ0723 '+MainAntivirus.VirusName, logAlert);
        VirFileList.AddVir(GetBaseFileName, TranslateStr('$AVZ0722 ')+MainAntivirus.VirusName, vrtVirusPartSign, VirusDescr.Script);
        if cbQuarantine.Checked then
         CopyToInfected(GetBaseFileName, '$AVZ0722 '+MainAntivirus.VirusName,
                       'Quarantine'); //#DNL
       end;
  end;
  // Это OLE объект ??
{  if FileScanner.PEFileInfo.StorageType = 1 then begin
   try OfficeMacroBase.CheckFile(AFileName); except end;
  end;}
  // Это архив ??
  if cbCheckArchives.Checked and (not(cbMaxArcSize.Checked) or (FileScanner.PEFileInfo.Size < seMaxArcSize.AsInteger*1024*1024)) then
   AVZDecompress.ScanFile(AFileName, FileScanner, ArcInfo);
  // Это файл с вложенными потоками ?
  if cbCheckNTFSStream.Checked and (ArcInfo = nil) and not(IsFileSteam) and(@zNtQueryInformationFile <> nil)  then begin
   VirusScanNTFSStreams(AFileName, DeleteF);
  end;
  // ******* Эвристик **********
  // Это вложенный поток типа PE ?
  if cbCheckNTFSStream.Checked and (IsFileSteam) and (EvLevel >= 2) and (FileScanner.PEFileInfo.IsPEFile) then
   if FileSignCheck.CheckFile(AFileName) <> 0 then begin
    AddToLog(LogFileName+' >>> $AVZ0569 ', logAlert);
    VirFileList.AddVir(GetBaseFileName, TranslateStr(' >>> $AVZ0569 '), vrtVirusEv);
   end;
  // Это PE внутри CHM файла ?
  if (ArcInfo <> nil) and (Result = 0) and (ArcInfo[length(ArcInfo)-1].ArcType = 'CHM') and (EvLevel >= 2) and (FileScanner.PEFileInfo.IsPEFile) and (Result <> 2) then //#DNL
   if FileSignCheck.CheckFile(AFileName) <> 0 then begin
    AddToLog(LogFileName+' >>> $AVZ0568 ', logAlert);
    VirFileList.AddVir(GetBaseFileName, TranslateStr(' >>> $AVZ0568 '), vrtVirusEv);
   end;
 finally
  FileScanner.Free;
 end;
end;

// Сканирование NTFS потока
function TMain.VirusScanNTFSStreams(AFileName: string;
  DeleteF: boolean): integer;
var
  Buf, Addr  : Pointer;
  BufSize    : Integer;
  FileHandle : THandle;
  Status     : NTSTATUS;
  IO_STATUS_BLOCK : TIO_STATUS_BLOCK;
  StreamNameBuf : array[0..255] of char;
  StreamName : string;
  EndOfList : boolean;
begin
 Result := 0;
 try
 if (@zNtQueryInformationFile = nil) then exit;
 // Открытие файла
 FileHandle := CreateFile(PChar(AFileName), GENERIC_READ,
                          FILE_SHARE_READ or FILE_SHARE_WRITE,
                          nil,
                          OPEN_EXISTING,
                          FILE_FLAG_BACKUP_SEMANTICS, 0 );
  if( FileHandle = INVALID_HANDLE_VALUE ) then exit;
  try
   // Выделение памяти
   BufSize := 16384;
   GetMem(Buf, BufSize);
   try
    // Запрос данных
    Status := zNtQueryInformationFile(fileHandle, @IO_STATUS_BLOCK,
                                     Buf, BufSize,
                                     FileStreamInformation);
    // Буфер слишком мал ??
    if (Status = STATUS_BUFFER_OVERFLOW) then begin
     FreeMem(Buf, BufSize);
     BufSize := 163840;
     GetMem(Buf, BufSize);
     Status := zNtQueryInformationFile(fileHandle, @IO_STATUS_BLOCK,
                                     Buf, BufSize,
                                     FileStreamInformation);
    end;
    if Status <> STATUS_SUCCESS then exit;
    if IO_STATUS_BLOCK.Information = 0 then exit;
    Addr := Pointer(DWord(Buf));
    repeat
     // Поиск имени потока
     ZeroMemory(@StreamNameBuf[0], SizeOf(StreamNameBuf));
     if TFileStreamInformation(Addr^).StreamNameLength > 0 then begin
      CopyMemory(@StreamNameBuf[0], Pointer(Dword(Addr) + sizeof(TFileStreamInformation)), TFileStreamInformation(Addr^).StreamNameLength);
      StreamName := WideCharToString(@StreamNameBuf[0]);
      // Не проверяем стандартный поток файла - он уже проверен
      if StreamName <> '::$DATA' then //#DNL
       Result := VirusScan(AFileName+StreamName, DeleteF, nil, nil, true);
     end;
     // Переход на новый адрес
     EndOfList := TFileStreamInformation(Addr^).NextEntryOffset = 0;
     Addr := Pointer(dword(Addr) + TFileStreamInformation(Addr^).NextEntryOffset);
    until EndOfList;
   finally
    // Освобождение памяти
    FreeMem(Buf, BufSize);
   end;
  finally
   CloseHandle(FileHandle);
  end;
 except end;
end;

// Сканирование папки
function TMain.ScanDir(ADir : string; ADeleteF, AScanSubdir : boolean) : boolean;
var
 SR  : TSearchRec;
 FileExt, FileName : string;
 Res, ErrPoz : integer;
 Cnt : integer;
begin
 SR.Name := '';
 Result := false;
 ADir := NormalDir(ADir);
 ErrPoz := 1;
 // Не сканировать свою папку !!
 if ScanAVZFolders or (pos(AvzPath, LowerCase(ADir)) = 1) then exit;
 try
   try
    ErrPoz := 2;
    Res := FindFirst(ADir+'*.*', faAnyFile, SR);//#DNL
    ErrPoz := 3;
    Application.ProcessMessages;
    StatusBar.Panels[0].Text := ADir;
    ErrPoz := 4;
    while Res = 0 do begin
     inc(Cnt);
     // Прерывание операции
     if StopF then exit;
     if ScanPauseF then begin
      StatusBar.Panels[0].Text := TranslateStr('$AVZ0701');
      while ScanPauseF do begin
       Application.HandleMessage;
       if StopF then exit;
      end;
      StatusBar.Panels[0].Text := TranslateStr('$AVZ0965');
     end;
     if Cnt mod 32 = 0 then Application.ProcessMessages;
     if ((SR.Attr and faDirectory) = 0) then begin
      FileName := SR.Name;
      FileName := UpperCase(FileName);
      FileExt := ExtractFileExt(FileName);
      ErrPoz := 5;
      // Фильтр
      if (ScanAllFiles or
         (ScanDangerFiles and (pos(FileExt, ExtToScan) > 0)) or
         (cbIncludeFiles.Checked and MatchesMask(FileName, IncludeMackList))) and
         not(cbExcludeFiles.Checked and MatchesMask(FileName, ExcludeMaskList)) then
       begin
        ErrPoz := 6;
        try
         VirusScan(ADir + SR.Name, ADeleteF, nil, nil);
        except
        end;
        ErrPoz := 7;
       end;
     end;
     // Сканирование подкаталогов
     if AScanSubdir then
      if (SR.Attr and faDirectory) > 0 then
       if (SR.Name <> '.') and (SR.Name <> '..') then begin //#DNL
        ErrPoz := 8;
        ScanDir(ADir + SR.Name, ADeleteF, AScanSubdir);
        ErrPoz := 9;
        // Это каталог с вложенными потоками ?
        if cbCheckNTFSStream.Checked and (@zNtQueryInformationFile <> nil)  then begin
         VirusScanNTFSStreams(ADir+ SR.Name, ADeleteF);
        end;
       end;
     ErrPoz := 10;
     Res := FindNext(SR);
     ErrPoz := 11;
    end;
   Result := true;
   finally
    try FindClose(SR); except end;
   end;
 except
  on e : Exception do
   AddToLog('$AVZ0679'+' ('+ ADir+', '+ e.Message + ', '+ inttostr(ErrPoz)  + ','+ SR.Name,logError);
 end;
end;

procedure TMain.SubClassWndProc(var Message: TMessage);
begin
  case Message.Msg of
    WM_NCDESTROY, WM_CLOSE, SC_CLOSE, 2 : Message.Msg := 0;
    else
     FDefaultWindowProc(Message);
  end;
end;

procedure TMain.FormCreate(Sender: TObject);
var
 i   : integer;
 INI : TINIFile;
 S, LocaleST : string;
begin
{  PDefaultCodePage := GetDefaultUserCodePageAddr;
  if PDefaultCodePage <> nil then
   PDefaultCodePage^ := 1251;
 }
 StopF := false;
 // Протоколирование
 InitLogger;
 ZShellTreeView.StateImages := GUIShared.StateImageList;
 // Отключение фильтра сообщений протокола
 MiniLogMode := false;
 // Отключение спуллера протокола в файл
 FSpoolLogFileName := '';
 // Подключение обработчика протоколирования
 ZLoggerClass.OnAddToDebugLog := OnAddToDebugLog;
 ZLoggerClass.OnAddToLog      := OnAddToLog;
 ZLoggerClass.OnGetLog        := OnGetLog;
 // Подключение обработчика кнопки "Пуск"
 mbStart.OnClick := mbStartClick;
 // Путь к базе - по умолчанию равен пути к EXE + \BASE\
 AvzPath := LowerCase(ExtractFilePath(Application.ExeName));
 AVPath  := ExtractFilePath(Application.ExeName)+'BASE\';
 // Создание папок
 try
  zForceDirectories(AVPath);  //#DNL
  QuarantineBaseFolder := ExtractFilePath(Application.ExeName);
  // Папка для временных файлов
  AVZTempDirectoryPath := GetTempDirectoryPath;
  zForceDirectories(AVZTempDirectoryPath);
 except end;
 // **************** Инициализируем ядро ***************
 // Загружаем опции
 INI := TINIFile.Create(ChangeFileExt(Application.ExeName, '.loc'));
 // Шаг 1. Если в профиле ее нет, то загружаем из параметров (у нее приоритет над профилем)
 for i := 1 to ParamCount do
  if pos('LANG=', UpperCase(ParamStr(i))) = 1 then begin
   LocaleST := Trim(UpperCase(copy(ParamStr(i), 6, maxint)));
   Break;
  end;
 // Шаг 2. Загружаем локализацию из профиля
 if LocaleST = '' then
  LocaleST       := Trim(UpperCase(INI.ReadString('Main', 'Lang', '')));
 // Шаг 3. В профиле и параметре данных нет - берем автоматом
 if LocaleST = '' then begin
  SetLength(S, 20);
  ZeroMemory(@S[1], length(S));
  if GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_ILANGUAGE, @S[1], length(S)) > 0 then
   S := Trim(S);
  case StrToIntDef(S, 0) of
   419 : LocaleST := 'RU';
   409 : LocaleST := 'EN';
   else
    LocaleST := 'EN';
  end;
 end;
 LangLocaleFont        := Trim(INI.ReadString(LocaleST, 'Font', ''));
 LangLocaleFontCharset := INI.ReadInteger(LocaleST, 'Charset', RUSSIAN_CHARSET);
 // Установка ThreadLocale по профилю
 if INI.ReadInteger(LocaleST, 'ThreadLocale', -1) >= 0 then
  SetThreadLocale(INI.ReadInteger(LocaleST, 'ThreadLocale', LOCALE_SYSTEM_DEFAULT));

 // AV ядро
 InitAVKernel(LocaleST);
 INI.Free;
 // Скриптовой движок - инициализация
 InitScriptingKernel;
 AvzUserScript.GUILink   := GUILink;
 AvzUserScript.SetupLink := SetupLink;
 // Загрузка NTAPI
 LoadNTDllAPI;
 // Загрузка Wintrust
 mmCheckWintrust.Enabled := LoadWintrustDLL;
 // настройка времени отображения хинтов
 Application.HintHidePause := 10000;

 // Настройка декомпрессора
 AVZDecompress.TempPath := AVZTempDirectoryPath;
 AVZDecompress.OnAVZFileExtracted := OnAVZFileExtracted;
 AVZDecompress.OnAVZFileCheckName := OnAVZCheckFilename;
 AVZDecompress.OnAVZExtractLog    := OnAVZDecompressLog;
 AVZDecompress.DetectMailBomb := true;

 // Настройка окна для режима 640*480
 if Screen.Width < 650 then begin
  Constraints.MinWidth  := Screen.Width  - 10;
  Constraints.MinHeight := Screen.Height - 10;
 end;
 // Субклассирование окна
 FDefaultWindowProc := WindowProc;
 WindowProc := SubClassWndProc;

 CopyUnpackedFiles := false;
 NetWorkMode := false;

 StartScanTime := 0;
 ScanInProgress := false;
 AvzUserScript.QurantinePath := GetQuarantineDirName('Quarantine');//#DNL

 // Обновление списка каталогов
 RefreshDirInfo;

 // Настройка значений по умолчанию
 cbModeVirus.ItemIndex := 1;
 cbModeAdvWare.ItemIndex := 1;
 cbModeSpy.ItemIndex := 1;
 cbModePornWare.ItemIndex := 1;
 cbModeHackTools.ItemIndex := 0;
 cbModeRiskWare.ItemIndex := 0;
 rgFileTypes.ItemIndex := 0;
 tbEvLevel.Position    := 1;

 // Активация Guard непосрадственно при старте AVZ
 for i := 1 to ParamCount do
  if (UpperCase(ParamStr(i)) = 'AG=1') or (UpperCase(ParamStr(i)) = 'AG=Y') then begin
   ActivateAVZGuard;
   Break;
  end;
 // Обновление интерфейса
 RefreshInterface;
 // Подключение справки
 if FileExists(ChangeFileExt(Application.ExeName, '.hlp')) then//#DNL
  Application.HelpFile := ChangeFileExt(Application.ExeName, '.hlp');//#DNL
 // Доступность специфичных для NT опций
 mmServiceView.Enabled   := IsNT;
 mmKernelModules.Enabled := IsNT;
 cbCheckNTFSStream.Enabled := IsNT;
 if not(FileSignCheck.LoadBinDatabase) then begin
  AddToLog('$AVZ0677 ' + MainAntivirus.LastError,logError);
  exit;
 end;
 // Активируем привилегию "Отладка"
 AdjustPrivileges('SeDebugPrivilege', true);       //#DNL
 AdjustPrivileges('SeLoadDriverPrivilege', true);  //#DNL
 AdjustPrivileges('SeBackupPrivilege', true);      //#DNL
 AdjustPrivileges('SeRestorePrivilege', true);     //#DNL
 AdjustPrivileges('SeShutdownPrivilege', true);     //#DNL
 // Создаем список удаленных файлов
 DeletedVirusList := TStringList.Create;
 IncludeMackList  := TStringList.Create;
 ExcludeMaskList  := TStringList.Create;
 ScanFileList     := TStringList.Create;
 TranslateForm(Self);
 mmFile.Caption := TranslateStr('$AVZ1109');
 mmService.Caption := TranslateStr('$AVZ0945');
 mmHelp.Caption := TranslateStr('$AVZ1041');
end;

procedure TMain.mbStartClick(Sender: TObject);
var
 i : integer;
 LSPManager : TLSPManager;
 S  : string;
 FileCountThread : TFileCountThread;
 OS : TOSVersionInfo;
begin
 try
   // Настройка ядра
   AutoQuarantine := cbQuarantine.Checked;
   AutoInfected   := cbInfected.Checked;
   // Сброс доп-меню
   ExtMenuScripts := nil;
   StopF          := false;
   ScanPauseF     := false;
   FDopFileCount := 0;
   mbPause.enabled := false;
   ScanPauseF := false;
   mbStart.enabled := false;
   mbStop.Enabled := true;
   pmStart.Enabled := false;
   pmStop.Enabled  := true;
   DeletedVirusList.Clear;
   // Обновление списка масок для фильтра
   RefrechFilterList;
   // Предупреждение о последствиях в KernelMode ...
   if cbCheckRootKit.Checked and cbAntiRootKitSystemKernel.Checked and not(AvzUserScript.Running) then
    if zMessageDlg('$AVZ0115 '+
               '$AVZ0500'+
               '$AVZ0100'+#$0D#$0A+
               '$AVZ0849 ?', mtWarning, [mbYes, mbNo], 0) <> mrYes then exit;
   KernelRootKitFunctionsCount := 0;
   // Сброс индикатора и его режима
   pbScanProgress.Position := 0;
   pbScanProgress.Tag      := 0;  // Режим  0 - отображение количества файлов
   ScanInProgress := true;
   FileCountThread := nil;
   VirFileList.Clear;
   // Очистка протокола
   LogMemo.lines.clear;
   LogMemo.Refresh;
  AddToDebugLog('Locale: DefaultLCID='+IntToStr(SysLocale.DefaultLCID)+','+
               'PriLangID='+IntToStr(SysLocale.PriLangID)+','+
               'SubLangID='+IntToStr(SysLocale.SubLangID)+','+
               'DefaultIME="'+Screen.DefaultIme+'",');
   // Создание потока, который будет подсчитывать файлы
   FileCountThread := TFileCountThread.Create(true);
   FileCountThread.OnProgress   := OnFileCountThreadProgress;
   // Даем время на прорисовку, чтобы устранить мерцание
   Application.ProcessMessages;
   Sleep(25);
   Application.ProcessMessages;
   // Сброс счетчиков
   ScanCount := 0; VirCount := 0; PodVirCount := 0; FScanArcCount := 0;
   StartScanTime := GetTickCount;
   ScanDangerFiles := rgFileTypes.ItemIndex = 0;
   ScanAllFiles    := rgFileTypes.ItemIndex = 1;
   // Загрузка базы антивируса
   if not(MainAntivirus.LoadBinDatabase) then begin
    AddToLog('$AVZ0677 '+MainAntivirus.LastError,logError);
    exit;
   end;
   // Загрузка базы макровирусов
   OfficeMacroBase.LoadBinDatabase;
   // Настройка режима протоколирования декомпрессора
   AVZDecompress.DebugLogMode := DebugLogMode;

   if MainAntivirus.MainDBDate = 0 then begin
    AddToLog('$AVZ0633', logError);
    exit;
   end;

   // Настройка и запуск потока подсчета файлов
   FileCountThread.ScanDangerFiles := ScanDangerFiles;
   FileCountThread.ScanAllFiles    := ScanAllFiles;
   FileCountThread.cbIncludeFiles  := cbIncludeFiles.Checked;
   FileCountThread.cbExcludeFiles  := cbExcludeFiles.Checked;
   FileCountThread.Resume;
   // Проверка, не устарела ли база
   if Abs(Now - MainAntivirus.MainDBDate) > 14 then begin
    AddToLog('$AVZ0114 ' + DateToStr(MainAntivirus.MainDBDate)+' $AVZ0515', logInfo);
   end;
   // Загрузка системы проверки файлов
   if not(FileSignCheck.LoadBinDatabase) then begin
    AddToLog('$AVZ0677 ' + FileSignCheck.LastError,logError);
    exit;
   end;
    // Загрузка класса проверки системы
   if not(SystemCheck.LoadBinDatabase) then begin
    AddToLog('$AVZ0677 ' + SystemCheck.LastError,logError);
    exit;
   end;
    // Загрузка класса проверки системы - ИПУ
   if not(SystemIPU.LoadBinDatabase) then begin
    AddToLog('$AVZ0677 ' + SystemIPU.LastError,logError);
    exit;
   end;
   // Загрузка базы восстановления системы
   SystemRepair.LoadBinDatabase;
   // Загрузка базы стандартных скриптов
   StdScripts.LoadBinDatabase;
   // Загрузка базы сканирования памяти
   MemScanBase.LoadBinDatabase;
    // Загрузка класса экспертной системы
   ESSystem.LoadBinDatabase;
   // Проверка исполняемого файла avz.exe
   if FileSignCheck.CheckFile(Application.ExeName, false) <> 0 then
    AddToLog('>>>> $AVZ0576', logAlert);
   MainAntivirus.EvLevel := EvLevel;
   psScanZone.ActivePageIndex := 0;
   try
    StopF := false;
    AVZDecompress.StopF := false;
    // Блокирование интерфейса
    LockInterface(True);
    RebootNeed := false;

    AddToLog('$AVZ0869 '+VER_NUM, logInfo);
    AddToLog('$AVZ0963 ' + Datetimetostr(now), logInfo);
    // Загружена баз ...
    AddToLog('$AVZ0248: '+MainAntivirus.VirBaseDescription, logInfo);
    // Скрипты эвристической проверки
    AddToLog('$AVZ0253: ' + IntToStr(Length(SystemCheck.CheckScripts)), logInfo);
    // Скрипты ИПУ
    AddToLog('$AVZ1180: ' + IntToStr(Length(SystemIPU.CheckScripts)), logInfo);
    // Чистые файлы
    AddToLog('$AVZ0254: ' + IntToStr(FileSignCheck.SignCount), logInfo);
    case EvLevel of
     0 : S := '$AVZ1152';
     1 : S := '$AVZ0427';
     2 : S := '$AVZ1047';
     3 : S := '$AVZ0381';
    end;
    AddToLog('$AVZ0927: '+S, logInfo);
    AddToLog('$AVZ0910: '+GetCureModeStr, logInfo);
    // Определение версии ОС
    OS.dwOSVersionInfoSize := sizeof(TOSVERSIONINFO);
    GetVersionEx(OS);
    if UserIsNTAdmin then
     S := '; $AVZ0005'
    else
     S := '';
    if ISNT then begin
     if GetEnvironmentVariable('SESSIONNAME') <> '' then
      if UpperCase(Trim(GetEnvironmentVariable('SESSIONNAME'))) <> 'CONSOLE' then
       AddToList(S, '$AVZ1172 ('+GetEnvironmentVariable('SESSIONNAME')+')', ',');
    end;
    AddToLog('$AVZ0101: '+Trim(format('%d.%d.%d', [OS.dwMajorVersion, OS.dwMinorVersion, LOWORD(OS.dwBuildNumber)]))+', '+Trim(OS.szCSDVersion)+' '+S, logInfo);
    // Состояния восстановления системы
    if ISNT then begin
     if RegKeyReadInt(HKEY_LOCAL_MACHINE, 'SOFTWARE\Microsoft\Windows NT\CurrentVersion\SystemRestore', 'DisableSR') = 1 then
      S := '$AVZ1185' else S := '$AVZ0108';
     AddToLog('$AVZ0145: '+S, logInfo);
    end;
    // Отображение в логе режима загрузки (обычный/SafeMode)
    case GetSystemMetrics(SM_CLEANBOOT) of
     1 : AddToLog('$AVZ0956', logInfo);
     2 : AddToLog('$AVZ0957', logInfo);
    end;
    // Поиск и нейтрализация перехватов
    AddToLog('1. $AVZ0750', logInfo);
    if cbCheckRootKit.Checked then
     ScanRootKit(cbAntiRootKitSystemUser.Checked, cbAntiRootKitSystemKernel.Checked)
      else AddToLog('$AVZ0746', logInfo);
    AddToLog('2. $AVZ0829', logInfo);
    // Проверка процессов
    if cbCheckProcess.Checked then
     ScanMemory(cbDeleteVirus.Checked)
      else AddToLog('$AVZ0831', logInfo);
    AddToLog('$AVZ0830', logInfo);
    // Рекурсивное сканирование указанных директорий
    AddToLog('3. $AVZ0960', logInfo);
    try
     for i := 0 to ScanFileList.Count - 1 do begin
      VirusScan(ScanFileList[i], cbDeleteVirus.Checked, nil, nil);
     end;
    except end;
    try
     mbPause.Down    := false;
     mbPause.enabled := true;
     for i := 0 to ZShellTreeView.Items.Count - 1 do begin
      if (ZShellTreeView.Items[i].Parent = nil) then
       ScanTreeDirs(ZShellTreeView.Items[i], cbDeleteVirus.Checked);
     end;
    finally
     mbPause.enabled := false;
     mbPause.Down    := false;
     FileCountThread.StopF := true;
     // Расширенная чистка следов в реестре
     if cbExtFileDelete.Checked then
      ExtSystemClean(DeletedVirusList);
    end;
    // Переключение прогресс-индикатора в режим "независимое управление"
    pbScanProgress.Tag := 1;
    // Проверка LSP
    AddToLog('4. $AVZ0813', logInfo);
    if cbCheckLSP.Checked then begin
     if FileExists(GetSystemDirectoryPath+'kernel32.dll') then begin // защита - на случай, если AVZ получае неверный путь к SYSTEM32
       LSPManager := TLSPManager.Create;
       LSPManager.Refresh;
       i := LSPManager.SearchErrors(LogMemo.Lines);
       if i > 0 then begin
        AddToLog(' $AVZ0118 - '+IntToStr(i), logAlert);
       end else
        AddToLog(' $AVZ0463', logInfo);
       if (i > 0) and cbAutoRepairLSP.Checked then begin
        LSPManager.FullAutoRepair(GetQuarantineDirName('BackUp'));
        AddToLog(' $AVZ0806', logAlert);
       end;
       LSPManager.Free;
     end else
       AddToLog(' $AVZ0667', logAlert);
    end else
     AddToLog(' $AVZ0828', logInfo);
    // Поиск кейлоггеров и троянских DLL
    AddToLog('5. $AVZ0771', logInfo);
    if cbKeyloggerSearch.Checked then begin
     SearchKeyloggers;
    end else
     AddToLog(' $AVZ0828', logInfo);
    AddToLog('6. $AVZ0767', logInfo);
    // Поиск троянских портов
    if cbSearchTrojanPorts.Checked then begin
     SearchPorts;
    end else
     AddToLog(' $AVZ0828', logInfo);
    // Эвристическое исследование системы
    AddToLog('7. $AVZ1148', logInfo);
    if cbEvSysCheck.Checked then begin
     try
      mbPause.Down    := false;
      mbPause.enabled := true;
      ZProgressClass.Max := 10000;
      EvSysCheck(GUILink);
     finally
      mbPause.Down    := false;
      mbPause.enabled := false;
     end;
     AddToLog('$AVZ0816', logInfo);
    end else
     AddToLog(' $AVZ0828', logInfo);
    // Эвристическое исследование - ИПУ
    AddToLog('8. $AVZ0773', logInfo);
    if cbEvSysIPU.Checked then begin
     try
      mbPause.Down    := false;
      mbPause.enabled := true;
      EvSysCheckIPU(GUILink);
     finally
      mbPause.Down    := false;
      mbPause.enabled := false;
     end;
     AddToLog('$AVZ0816', logInfo);
    end else
     AddToLog(' $AVZ0828', logInfo);
    if cbEvSysIPU.Checked then begin
     // Эвристическое исследование - визарды
     AddToLog('9. $AVZ1245', logInfo);
     RunWizardScripts(GUILink, 'tsw', 2, -1, false);
     AddToLog('$AVZ0816', logInfo);
    end else
     AddToLog(' $AVZ0828', logInfo);
    // Вывод итоговой статистики
    AddToLog('$AVZ0854: '+IntToStr(ScanCount)+', $AVZ0294: '+IntToStr(FScanArcCount)+', $AVZ0460 '+IntToStr(VirCount)+'$AVZ1175 '+IntToStr(PodVirCount), logInfo);
    AddToLog('$AVZ0962 '+Datetimetostr(now), logInfo);
    // Вывод запроса не перезагрузку в случае необходимости
    if RebootNeed then
     AddToLog('$AVZ0116 ', logAlert);
    if KernelRootKitFunctionsCount > 0 then begin
     AddToLog('$AVZ0001 '+IntToStr(KernelRootKitFunctionsCount)+' $AVZ1134', logAlert);
     AddToLog('$AVZ1156', logAlert);
    end;
   finally
    // Остановка потока подсчета файлов
    if FileCountThread <> nil then begin
     FileCountThread.StopF := true;
     FileCountThread.Terminate;
     FileCountThread.Free;
    end;
    // Вычисление затраченного времени
    AddToLog('$AVZ0961 '+MilliSecToTimeStr(GetTickCount - StartScanTime), logInfo);
    AddToLog('$AVZ0238,', logInfo);
    AddToLog('$AVZ1065', logInfo);
    ScanInProgress := false;
    pbScanProgress.Tag := 0;
    pbScanProgress.Position := 0;
   end;
 finally
  if StopF then
   AddToLog('$AVZ0964', logInfo);
  mbStart.visible := true;
  mbStart.Enabled := true;
  mbStop.Enabled  := false;
  pmStart.Enabled := true;
  pmStop.Enabled  := false;
  StatusBar.Panels[0].Text := '';
  // Резблокировка интрефейса
  LockInterface(false);
  // Удаление временных файлов, созданных декомпрессором
  AVZDecompress.DelTempFiles;
 end;
end;

procedure TMain.SetScanCount(const Value: integer);
begin
 FScanCount    := Value;
end;

procedure TMain.SetVirCount(const Value: integer);
begin
 FVirCount := Value;
end;

procedure TMain.mbStopClick(Sender: TObject);
begin
 ScanPauseF := false;
 StopF := true;
end;

procedure TMain.SetPodVirCount(const Value: integer);
begin
 FPodVirCount := Value;
end;

procedure TMain.Timer1Timer(Sender: TObject);
var
 MSec, WaitMSec: integer;
 ScanSpeed, Procent : double;
begin
 //Caption := IntToStr(pbScanProgress.Position) + ' / ' + IntToStr(pbScanProgress.Max);
 mmPause.Enabled := mbPause.Enabled;
 mmStop.Enabled  := mbStop.Enabled;
 mmStart.Enabled := mbStart.Enabled;
 StatusBar.Panels[1].Text := IntToStr(ScanCount)+'/'+IntToStr(VirCount)+'/'+IntToStr(PodVirCount); //#DNL
 if ScanInProgress then begin
  // Tag = 0 - отображение прогресса сканирования файлов
  if pbScanProgress.Tag = 0 then begin
   pbScanProgress.Position := ScanCount;
   pbScanProgress.Max      := FFullScanFileCount + FDopFileCount;
  end;
  Procent := pbScanProgress.Position / (pbScanProgress.Max + 1) * 100;
  MSec := GetTickCount - StartScanTime;
  if (StartScanTime > 0) and (ScanCount > 0) and (MSec > 0) and (FFullScanFileCount > 0) then begin
   ScanSpeed := MSec / ScanCount;
   WaitMSec := trunc((FFullScanFileCount + FDopFileCount - ScanCount) * ScanSpeed);
   if WaitMSec > 100 then
    StatusBar.Panels[2].Text := TranslateStr('$AVZ0596 ') + MilliSecToTimeStr(WaitMSec)
     else StatusBar.Panels[2].Text := '';
  end else
   StatusBar.Panels[2].Text := '';
  RxTrayIcon.Hint := TranslateStr('$AVZ0063.'+
                     '$AVZ0292 ' +
                     FormatFloat('#0', Procent)+'% '+ //#DNL
                     StatusBar.Panels[2].Text);
  Application.Title := RxTrayIcon.Hint;
 end else begin
  RxTrayIcon.Hint := TranslateStr('$AVZ0063');
  Application.Title := RxTrayIcon.Hint;
  pbScanProgress.Max       := 100;
  pbScanProgress.Position  := 0;
  StatusBar.Panels[2].Text := '';
 end;
end;

procedure TMain.OnAddToLog(AMsg: string; InfoType : integer);
begin
 // Фильтр
 if MiniLogMode and (InfoType = logInfo) then exit;
 case InfoType of
  logNormal : LogMemo.SelAttributes.Color := clBlack;
  //logInfo   : LogMemo.SelAttributes.Color := clGray;
  logAlert  : begin
               LogMemo.SelAttributes.Color := clRed;
               //LogMemo.SelAttributes.Style := [fsBold];
              end;
  logVirus  : begin
               LogMemo.SelAttributes.Color := clRed;
               //LogMemo.SelAttributes.Style := [fsBold];
              end;
  logError  : begin
               LogMemo.SelAttributes.Color := clRed;
              end;
  logGood  : begin
               LogMemo.SelAttributes.Color := clGreen;
              end;
  logDebug  : LogMemo.SelAttributes.Color := clDkGray;
  else
   LogMemo.SelAttributes.Color := clBlack;
 end;
 // Вывод в протокол
 LogMemo.Lines.AddObject(TranslateStr(AMsg), Pointer(InfoType));
end;

function TMain.ScanMemory(ADeleteF: boolean): boolean;
var
 SysProcessInfo : TSysProcessInfo;
 ProcessInfo    : TProcessInfo;
 ModuleInfo     : TModuleInfo;
 ProcessList, ModulesList  : TStrings;
 AllModulesList : TStringList;
 i,j : integer;
 S : string;
begin
 Result := false;
 ProcessList    := TStringList.Create;
 ModulesList    := TStringList.Create;
 AllModulesList := TStringList.Create;
 // Создание класса "информация о процессах" в зависимости от типа ОС
 if ISNT then
  SysProcessInfo := TPSAPIProcessInfo.Create
   else SysProcessInfo := TToolHelpProcessInfo.Create;
 // Построение списка процессов
 SysProcessInfo.CreateProcessList(ProcessList);
 // Добавление в протокол
 AddToLog(' $AVZ0348: '+inttostr(ProcessList.Count), logInfo);
 // Сканирование процессов
 FFullScanFileCount := ProcessList.Count;
 for i := 0 to ProcessList.Count - 1 do begin
  inc(FDopFileCount);
  ProcessInfo := ProcessList.Objects[i] as TProcessInfo;
  StatusBar.Panels[0].Text := ProcessInfo.ExeFileName;
  Application.ProcessMessages;
  // Проверка процесса
  AddToDebugLog('Scan proc '+NormalProgramFileName(ProcessInfo.ExeFileName, '.exe')); //#DNL
  if VirusScan(NormalProgramFileName(ProcessInfo.ExeFileName, '.exe'), ADeleteF, ProcessInfo, nil) <= 0 then
   try AVScanProcessMemory(ProcessInfo.ExeFileName, ProcessInfo.PID, false);except end;
  S := '';
  // Проверка модулей процесса
  SysProcessInfo.CreateModuleList(ModulesList, ProcessInfo.PID);
  FFullScanFileCount := FFullScanFileCount + ModulesList.Count - 1;
  for j := 1 to ModulesList.Count - 1 do begin
   ModuleInfo := ModulesList.Objects[j] as TModuleInfo;
   ModuleInfo.ModuleName := LowerCase(ModuleInfo.ModuleName);
   if EvLevel > 2 then
    if pos(LowerCase(ExtractFileName(ModuleInfo.ModuleName)),
           'inetcomm.dll, inetres.dll, wininet.dll, rasapi32.dll, ws2_32.dll, ws2help.dll, netapi32.dll, tapi32.dll, urlmon.dll') > 0 then //#DNL
            if FileSignCheck.CheckFile(ModuleInfo.ModuleName) <> 0 then
             AddToList(S, ExtractFileName(ModuleInfo.ModuleName));
   if AllModulesList.IndexOf(ModuleInfo.ModuleName) < 0 then
    AllModulesList.Add(ModuleInfo.ModuleName);
  end;
  if S <> '' then
   if FileSignCheck.CheckFile(NormalProgramFileName(ProcessInfo.ExeFileName, '.exe')) <> 0 then
    AddToLog('$AVZ0874 '+NormalProgramFileName(ProcessInfo.ExeFileName, '.exe')+' $AVZ0442 ('+s+')', logAlert);
 end;
 AddToDebugLog('Scan hidden process');
 SysProcessInfo.ClearList(ProcessList);
 ProcessList.Clear;
 // Проверка процесса
 if SearchFUHiddenProc(ProcessList) then
  for i := 0 to ProcessList.Count - 1 do
   try AVScanProcessMemory(ProcessList[i], integer(ProcessList.Objects[i]), true); except end;
 AddToLog(' $AVZ0346: '+inttostr(AllModulesList.Count), logInfo);
 for i := 0 to AllModulesList.Count - 1 do begin
  StatusBar.Panels[0].Text := AllModulesList[i];
  if i mod 32 = 0 then  Application.ProcessMessages;
  // Проверка процесса
  if VirusScan(NormalProgramFileName(AllModulesList[i],'.exe'), ADeleteF, nil, nil) < 0 then
   if not(StopF) then
    if FileExists(NormalProgramFileName(AllModulesList[i],'.exe')) then
     AddToLog('$AVZ1181 '+AllModulesList[i]+' - $AVZ1176', logAlert);
 end;
 SysProcessInfo.ClearList(ModulesList);
 SysProcessInfo.ClearList(AllModulesList);
 SysProcessInfo.Free;
 ProcessList.Free;
 ModulesList.Free;
 AllModulesList.Free;
 Result := true;
end;

function TMain.IQ_Check(AFileName: string; ProcessInfo: TProcessInfo;
  PEFileInfo: TAVZFileInfo): boolean;
var
 FileExt, FileName, TrimedFileName, S : string;
 TestRes    : string;
 TestRang   : integer;
begin
 Result := true;
 TestRes := '';
 TestRang := 0;
 FileExt        := UpperCase(ExtractFileExt(AFileName));
 FileName       := Copy(AFileName, 1, length(AFileName) - length(FileExt));
 TrimedFileName := Trim(FileName);
 if FileExt = '.TMP' then exit; //#DNL
 if PEFileInfo.Size < 20 then exit;
 // Проверка нестандартного расширения
 if  (pos(FileExt, ScanFilesEXT_Data) > 0) then
  if PEFileInfo.IsEXEFile then begin
   AddToList(TestRes, '$AVZ0023', ';');
   inc(TestRang, 5);
   Result := false;
  end;
 // Проверка точки входа в заголовке
 if PEFileInfo.IsPEFile then
  if (PEFileInfo.PE_EntryPoint > 0) and (PEFileInfo.PE_EntryPoint < $100) then begin
   AddToList(TestRes, '$AVZ0730', ';');
   inc(TestRang, 75);
   Result := false;
  end;
 // Проверка нестандартного расширения
 if  (pos(FileExt, '.LNK.COM.PIF') > 0) then //#DNL
  if PEFileInfo.IsEXEFile then begin
   AddToList(TestRes, '$AVZ1183', ';');
   inc(TestRang, 35);
   Result := false;
  end;
 // Пробелы в имени
 S := ExtractFileName(AFileName);
 if PEFileInfo.IsEXEFile and (pos('     ', S) > 0) then begin
   AddToList(TestRes, '$AVZ0089', ';');
   inc(TestRang, 5);
   Result := false;
  end;
 if Length(FileName) - Length(TrimedFileName) > 15 then begin
   AddToList(TestRes, '$AVZ0535', ';');
   inc(TestRang, 5);
   // Это еще и EXE файл !? Тогда существенное увеличение ранга
   if PEFileInfo.IsEXEFile then
    inc(TestRang, 25);
   Result := false;
  end;
 // Нет имени
 if PEFileInfo.IsEXEFile and (Length(TrimedFileName) = Length(ExtractFilePath(TrimedFileName))) then begin
   AddToList(TestRes, '$AVZ1116', ';');
   inc(TestRang, 15);
   Result := false;
  end;
 // Добавление в протокол
 if TestRes <> '' then
  if FileSignCheck.CheckFile(AFileName) <> 0 then begin
   AddToLog(AFileName + ' - ' + TestRes + '($AVZ1052 '+IntToStr(min(TestRang, 100))+'%)', logAlert);
   VirFileList.AddVir(AFileName, TestRes + TranslateStr('($AVZ1052 ')+IntToStr(min(TestRang, 100))+'%)', vrtVirusEv, -1);
   if cbQuarantine.Checked then
    CopyToInfected(AFileName, TestRes, 'Quarantine'); //#DNL
  end;
end;

procedure TMain.mmExitClick(Sender: TObject);
begin
 if not(CheckCanClose) then exit;
 // Деактивация защиты процесса
 DeactivateInternalProcessProtection;
 // Выставляем флаг остановки сканирования
 StopF := true;
 // Выдерживаем таймаут - чтобы процесс сканирования успел остановиться
 Application.ProcessMessages;
 Application.ProcessMessages;
 Sleep(100);
 Application.ProcessMessages;
 Application.ProcessMessages;
 Application.Terminate;
end;

procedure TMain.N4Click(Sender: TObject);
begin
 ExecuteAboutDLG;
end;

function TMain.Script_Check(AFileName: string): boolean;
var
 FS : TFileStream;
 S, ST  : String;
 i  : integer;
begin
 exit;
 FS := nil;
 try
  FS := TFileStream.Create(AFileName, fmOpenRead);
  FS.Seek(0,0);
  if FS.Size > 0 then begin
   SetLength(S, FS.Size);
   FS.Read(S[1], FS.Size);
   S  := Trim(LowerCase(S));
   ST := '';
   for i := 1 to Length(S) do
    if Byte(S[i]) >= 32 then
     ST := ST+S[i];
   if pos(LowerCase('Scripting.FileSystemObject'), ST) > 0 then //#DNL
    AddToLog(AFileName + ' >>>> $AVZ0724', logAlert);
 end;
 FS.Free;
 except
  try FS.Free; except end;
 end;
end;

procedure TMain.mmShowInfectedClick(Sender: TObject);
begin
 ExecuteInfectedView('Infected', '$AVZ0016');
end;

procedure TMain.ZShellTreeViewTreeNodeStateIconClick(Sender: TObject;
  Node: TTreeNode);
begin
 // Меняем статус
 if Node.StateIndex in [1,3] then
  Node.StateIndex := 2
   else Node.StateIndex := 1;
 // Меняем стутус подчиненных элементов
 (Sender as TZShellTreeView).SetChildNodesState(Node, Node.StateIndex);
 // Меняем стутус родительских элементов
 if Node.Parent <> nil then
  SetParentStatus(Node.Parent);
end;

procedure TMain.SetParentStatus(ANode: TTreeNode);
var
 i, CheckedCount, PartCheckedCount, FileCheckedCount : integer;
begin
 // Случайный вызов ?? тогда выход
 if (ANode = nil) or (ANode.Count <= 0) then exit;
 CheckedCount := 0; PartCheckedCount := 0; FileCheckedCount := 0;
 for i := 0 to ANode.Count - 1 do begin
  if ANode.Item[i].StateIndex = 2 then
   inc(CheckedCount);
  if ANode.Item[i].StateIndex = 3 then
   inc(PartCheckedCount);
  if ANode.Item[i].StateIndex = 4 then
   inc(FileCheckedCount);
 end;
 // Анализ количества отмеченных узлов
 if PartCheckedCount > 0 then
  ANode.StateIndex := 3 else
   if CheckedCount = ANode.Count then
    ANode.StateIndex := 2 else
     if (CheckedCount = 0) and (FileCheckedCount = 0) then
      ANode.StateIndex := 1
       else ANode.StateIndex := 3;
 // Установка статуса на уровне N-1
 if ANode.Parent <> nil then
  SetParentStatus(ANode.Parent);
end;


procedure TMain.LockInterface(ALockState: boolean);
begin
 mbStart.Enabled := not(ALockState);
 mbStop.Enabled  := ALockState;
 psScanZone.Enabled := not(ALockState);
 gbSetup.Enabled        := not(ALockState);
 cbDeleteVirus.Enabled  := not(ALockState);
 cbInfected.Enabled     := not(ALockState);
 cbQuarantine.Enabled   := not(ALockState);
 sbDetailLogView.Enabled   := not(ALockState);
end;


function TMain.ScanTreeDirs(ARootNode: TTreeNode;
  ADeleteF: boolean): boolean;
var
 i : integer;
 res : boolean;
begin
 Result := true;
 // Дальнейшие действия
 case ARootNode.StateIndex of
  0, 1 : ; // Не выделен - никаких действий
  2    : Result := ScanDir(NormalDir(ZShellTreeView.GetPath(ARootNode)), ADeleteF, true);
  3    : begin
          //Result := ScanDir(NormalDir(ZShellTreeView.GetPath(ARootNode)), ADeleteF, false);
          for i := 0 to ARootNode.Count - 1 do
           if ARootNode.Item[i].StateIndex > 1 then begin
            res := ScanTreeDirs(ARootNode.Item[i], ADeleteF);
            Result := Result and res;
           end;
         end;
  4    : ScanDir(NormalDir(ZShellTreeView.GetPath(ARootNode)), ADeleteF, false);
 end;
end;

procedure TMain.tbEvLevelChange(Sender: TObject);
begin
 RefreshInterface;
end;

function TMain.RefreshInterface: boolean;
var
 S : String;
begin
 lbEvLevel.Caption := '';
 case EvLevel of
  0 : S := '$AVZ1152';
  1 : S := '$AVZ0427';
  2 : S := '$AVZ1047';
  3 : S := '$AVZ0381';
 end;
 lbEvLevel.Caption := TranslateStr(S);
 // Переключатель расширенной эврситики
 cbExtEv.Enabled := EvLevel = 3;
 if not(cbExtEv.Enabled) then cbExtEv.Checked := false;
 // Настройка доступности режимов удаления
 cbModeVirus.Enabled     := cbDeleteVirus.Checked;
 cbModeAdvWare.Enabled   := cbDeleteVirus.Checked;
 cbModeSpy.Enabled       := cbDeleteVirus.Checked;
 cbModeHackTools.Enabled := cbDeleteVirus.Checked;
 cbModeRiskWare.Enabled  := cbDeleteVirus.Checked;
 cbModePornWare.Enabled  := cbDeleteVirus.Checked;
 cbInfected.Enabled      := cbDeleteVirus.Checked;
 cbExtFileDelete.Enabled := cbDeleteVirus.Checked;
 //cbQuarantine.Enabled    := cbDeleteVirus.Checked;
 // В режиме 1 (все файлы) включение по маске теряет свой смысл
 if rgFileTypes.ItemIndex = 1 then begin
  cbIncludeFiles.Checked := false; cbIncludeFiles.Enabled := false;
 end else begin
  cbIncludeFiles.Enabled := true;
 end;
 // Фильтры
 edIncludeFiles.Enabled := cbIncludeFiles.Checked;
 edExcludeFiles.Enabled := cbExcludeFiles.Checked;
 // Переключатель антируткита KernelMode
 if not(IsNT) then begin
  cbAntiRootKitSystemKernel.Enabled := false;
  cbAntiRootKitSystemKernel.Checked := false;
  mmPMControl.Enabled := false;
  mmSG.Enabled := false;
 end else begin
  cbAntiRootKitSystemUser.Enabled   := cbCheckRootKit.Checked;
  cbAntiRootKitSystemKernel.Enabled := cbCheckRootKit.Checked;
 end;
 // Настройка меню
 if ISNT then begin
  mmSG_TurnOff.Enabled    := AVZGuardIsOn;
  mmSG_TrustedRun.Enabled := AVZGuardIsOn;
  mmSG_TurnOn.Enabled  := not(AVZGuardIsOn);
  mmPMDriverInstall.Enabled := (AVZDriverRK <> nil) and not(AVZDriverRK.Loaded);
  mmPMDriverRemove.Enabled  := not(mmPMDriverInstall.Enabled);
  mmPMDriverUnloadRemove.Enabled  := not(mmPMDriverInstall.Enabled);
  if AVZGuardIsOn then begin
   cbAntiRootKitSystemKernel.Checked := false;
   cbAntiRootKitSystemKernel.Enabled := false;
  end else
   cbAntiRootKitSystemKernel.Enabled := cbCheckRootKit.Checked;
 end;
end;

function TMain.GetEvLevel: integer;
begin
 Result := 3 - tbEvLevel.Position;
end;

procedure TMain.N11Click(Sender: TObject);
begin
 ExecuteInfectedView('Quarantine', '$AVZ0325');
end;

procedure TMain.mmStartClick(Sender: TObject);
begin
 if mbStart.Enabled then
  mbStart.Click;
end;

procedure TMain.mmStopClick(Sender: TObject);
begin
 if mbStop.Enabled then
  mbStop.Click;
end;

procedure TMain.cbDeleteVirusClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TMain.SpeedButton2Click(Sender: TObject);
begin
 if SaveDialog.Execute and (SaveDialog.FileName <> '') then
  LogMemo.Lines.SaveToFile(SaveDialog.FileName);
end;

procedure TMain.N10Click(Sender: TObject);
begin
 LogMemo.PlainText:=True;
 if SaveDialog.Execute and (SaveDialog.FileName <> '') then
  LogMemo.Lines.SaveToFile(SaveDialog.FileName);
end;

function TMain.DeleteGroupEnabled(AVirusName: string): integer;
var
 S : string;
begin
 Result := 0;
 // Выделение строки
 S := Trim(LowerCase(copy(AVirusName, 1, pos('.', AVirusName)-1)));
 // Вирус без категории
 if S = '' then exit;
 // AdvWare
 if (S = 'adware') or (S = 'advware') then begin //#DNL
  Result := cbModeAdvWare.ItemIndex;
  exit;
 end;
 // SpyWare
 if (S = 'spy') or (S = 'spyware') then begin   //#DNL
  Result := cbModeSpy.ItemIndex;
  exit;
 end;
 // PornWare
 if (S = 'porno') or (S = 'pornware') or (S = 'dialer') then begin //#DNL
  Result := cbModePornWare.ItemIndex;
  exit;
 end;
 // HackTool
 if (S = 'hacktool') or (S = 'exploit') or (S = 'ddos') or (S = 'constructor') or (S = 'hacktools') then begin //#DNL
  Result := cbModeHackTools.ItemIndex;
  exit;
 end;
 // RiskWare
 if (S = 'riskware') or (S = 'risktool') or (S = 'downloader') then begin //#DNL
  Result := cbModeRiskWare.ItemIndex;
  exit;
 end;
 Result := cbModeVirus.ItemIndex;
end;

procedure TMain.mmRefreshClick(Sender: TObject);
begin
 RefreshDirInfo;
end;

function TMain.RefreshDirInfo: boolean;
var
 i : integer;
 Bufer      : array[0..1024] of char; // Буфер
 RealLen    : integer;                // Результирующая длина
 S          : string;
begin
 ZShellTreeView.Items.Clear;
 // Составление списка устройств
 RealLen := GetLogicalDriveStrings(SizeOf(Bufer),Bufer);
 i := 0; S := '';
 // Цикл анализа буфера (последний символ не обрабатывается, т.к. он всегда #0)
 while i < RealLen do begin
  if Bufer[i] <> #0 then begin
   S := S + Bufer[i];
   inc(i);
  end else begin
   inc(i); // Пропуск разделяющего #0
   ZShellTreeView.AddRootFolder(S);
   S := '';
  end;
 end;
end;

procedure TMain.N22Click(Sender: TObject);
begin
 ExecuteProcessView;
end;

procedure TMain.mmSelectHDDClick(Sender: TObject);
var
 i : integer;
begin
 // Выделение дисков определенных типов
 for i := 0 to ZShellTreeView.Items.Count - 1 do
  if (ZShellTreeView.Items[i].Parent = nil) then
   case GetDriveType(PChar(ZShellTreeView.GetPath(ZShellTreeView.Items[i]))) of
    DRIVE_FIXED     : if (Sender as TMenuItem).Tag = 1 then ZShellTreeView.Items[i].StateIndex := 2;
    DRIVE_CDROM     : if (Sender as TMenuItem).Tag = 2 then ZShellTreeView.Items[i].StateIndex := 2;
    DRIVE_REMOVABLE : if (Sender as TMenuItem).Tag = 3 then ZShellTreeView.Items[i].StateIndex := 2;
   end;
end;

procedure TMain.lbHelpLabelClick(Sender: TObject);
begin
 case (Sender as TLabel).Tag of
  1 : Application.HelpJump('t_klass_vir');             //#DNL
  2 : Application.HelpJump('t_klass_adware');          //#DNL
  3 : Application.HelpJump('t_klass_spy');             //#DNL
  4 : Application.HelpJump('t_klass_pornware');        //#DNL
  5 : Application.HelpJump('t_klass_hijacker');        //#DNL
  6 : Application.HelpJump('t_klass_riskware');        //#DNL
  7 : Application.HelpJump('term_ha');                 //#DNL
  8 : Application.HelpJump('term_rootkit');            //#DNL
  9 : Application.HelpJump('term_lsp');                //#DNL
 end;
end;

procedure TMain.mmServiceViewClick(Sender: TObject);
begin
 ExecuteServiceView;
end;

procedure TMain.mmLSPViewClick(Sender: TObject);
begin
 ExecuteLSPView;
end;

function TMain.GetPEImageAddress: dword;
var
 list : TStrings;
 SysProcessInfo : TSysProcessInfo;
begin
 Result := 0;
 List := TStringList.Create;
 // Создание класса "информация о процессах" в зависимости от типа ОС
 if ISNT then
  SysProcessInfo := TPSAPIProcessInfo.Create
   else SysProcessInfo := TToolHelpProcessInfo.Create;
 // Получение списка модулей
 SysProcessInfo.CreateModuleList(List, GetCurrentProcessId);
 // Первый модуль - это наш EXE
 if list.Count > 0 then
  Result := (List.Objects[0] as TModuleInfo).hModule;
 list.Free;
 SysProcessInfo.Free;
end;

procedure TMain.N14Click(Sender: TObject);
begin
 ExecuteRegSearch('');
end;

procedure TMain.cbCheckRootKitClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TMain.CPUDP1Click(Sender: TObject);
begin
 ExecuteNetOpenPorts;
end;

procedure TMain.N15Click(Sender: TObject);
begin
 ExecuteFileSearchDLG;
end;

function TMain.SearchPorts: boolean;
var
 IPPorts   : TIPPorts;
 i, VirCnt : integer;
 S         : string;
begin
 try
   IPPorts := TIPPorts.Create;
   IPPorts.LoadPortDescr(ExtractFilePath(Application.ExeName)+'Base\ports.avz'); //#DNL
   AddToLog(' $AVZ0086 '+Inttostr(length(IPPorts.PortDescrList))+' $AVZ0583', logInfo);
   // Попытка построить базу портов
   if not(IPPorts.Refresh) then begin
    IPPorts.Free;
    AddToLog(' $AVZ0634 ', logInfo);
    exit;
   end;
   VirCnt := 0;
   AddToLog(' $AVZ0447 '+Inttostr(length(IPPorts.TCPConnections))+' $AVZ0031 '+Inttostr(length(IPPorts.UDPConnections))+' $AVZ0032', logInfo);
   for i := 0 to length(IPPorts.TCPConnections) - 1 do
    if (IPPorts.TCPConnections[i].State = 'LISTENING') and ((IPPorts.TCPConnections[i].Flags and not(7)) > 0) then begin
     S := '';
     if (IPPorts.TCPConnections[i].ProcessBin <> '') and (FileSignCheck.CheckFile(IPPorts.TCPConnections[i].ProcessBin) = 0) then
      S := ' - $AVZ0584';
     AddToLog(' >>> $AVZ0561 '+inttostr(IPPorts.TCPConnections[i].LocalPort)+' TCP - '+IPPorts.TCPConnections[i].Prim+' ('+IPPorts.TCPConnections[i].ProcessBin+S+')', logAlert);
     if IPPorts.TCPConnections[i].ProcessBin <> '' then
       VirFileList.AddVir(IPPorts.TCPConnections[i].ProcessBin, TranslateStr('$AVZ0848 ')+IntToStr(IPPorts.TCPConnections[i].LocalPort)+' TCP - '+IPPorts.TCPConnections[i].Prim, vrtVirusEv);
     inc(VirCnt);
    end;
   for i := 0 to length(IPPorts.UDPConnections) - 1 do
    if (IPPorts.UDPConnections[i].Flags and not(7)) > 0 then begin
     S := '';
     if (IPPorts.UDPConnections[i].ProcessBin <> '') and (FileSignCheck.CheckFile(IPPorts.UDPConnections[i].ProcessBin) = 0) then
      S := ' - $AVZ0584';
     AddToLog(' >> $AVZ0561 '+inttostr(IPPorts.UDPConnections[i].LocalPort)+' UDP - '+IPPorts.UDPConnections[i].Prim+' ('+IPPorts.UDPConnections[i].ProcessBin+S+')', logAlert);
     if IPPorts.UDPConnections[i].ProcessBin <> '' then
       VirFileList.AddVir(IPPorts.UDPConnections[i].ProcessBin, TranslateStr('$AVZ0848 ')+IntToStr(IPPorts.UDPConnections[i].LocalPort)+' UDP - '+IPPorts.UDPConnections[i].Prim, vrtVirusEv);
     inc(VirCnt);
    end;
   if VirCnt = 0 then
    AddToLog(' $AVZ0817', logInfo)
     else AddToLog('$AVZ0448', logInfo);
  except
   on e : Exception do begin
    AddToLog(' $AVZ0099', logError);
    AddToDebugLog(' $AVZ0646: '+ e.Message);
   end;
  end;
end;

procedure TMain.N23Click(Sender: TObject);
begin
 Application.HelpJump('t_main_1');
end;

procedure TMain.N24Click(Sender: TObject);
begin
 Application.HelpCommand(HELP_FINDER,0);
end;

procedure TMain.sbDetailLogViewClick(Sender: TObject);
begin
 ExtLogView.Execute;
end;

procedure TMain.tRunScanTimer(Sender: TObject);
begin
 tRunScan.Enabled := false;
 Application.ProcessMessages;
 Refresh;
 Repaint;
 Application.ProcessMessages;
 mbStart.Click;
end;

procedure TMain.N17Click(Sender: TObject);
begin
 ExecuteNetSharesDLG;
end;

procedure TMain.N21Click(Sender: TObject);
begin
 ExecuteFile('regedit.exe', '', '', SW_NORMAL);
end;

procedure TMain.MsConfig1Click(Sender: TObject);
begin
 ExecuteFile('msconfig.exe', '', '', SW_NORMAL);
end;

procedure TMain.cbCheckLSPClick(Sender: TObject);
begin
 cbAutoRepairLSP.Enabled := cbCheckLSP.Checked;
end;

procedure TMain.OnFileCountThreadProgress(Sender: TObject; FileCount, FullFilesCount : int64; ScanComplete : boolean);
begin
 if ScanComplete then FFullScanFileCount := FileCount else
  if FileCount - ScanCount < 5000 then
   FFullScanFileCount := ScanCount  + 5000
    else FFullScanFileCount := FileCount;
end;

procedure TMain.N27Click(Sender: TObject);
begin
 ExecuteAutorunsDLG;
end;

procedure TMain.N29Click(Sender: TObject);
var
 res : integer;
 S : string;
 InfoType : integer;
begin
 InfoType := logNormal;
 if OpenDialog1.Execute then begin
  Res := FileSignCheck.CheckFile(OpenDialog1.FileName, false);
  if Res = 0 then begin
   S := '$AVZ1120';
   InfoType := logGood;
  end else S :='$AVZ1118';
  zMessageDlg(S, mtInformation, [mbOk], 0);
  AddToLog('$AVZ1109: '+OpenDialog1.FileName+'. $AVZ0928: '+S, InfoType);
 end;
end;

procedure TMain.N31Click(Sender: TObject);
var
 Lines       : TStringList;
 FileName    : string;
 DelMode     : integer;
 FQuarantine : boolean;
begin
 FileName := Trim(ExecuteSelectDeletedFile);
 if FileName = '' then exit;
 AddToLog('$AVZ0611 '+FileName, logAlert);
 ExecuteFileDeleteModeDLG(DelMode, FQuarantine);
 if DelMode = 0 then exit;
 FileName := StringReplace(FileName, '"', '', [rfReplaceAll]);
 // Копия файла перед удалением
 if FQuarantine then
    CopyToInfected(FileName,
                        '$AVZ0376',
                        'Quarantine'); //#DNL
   // Удаление файла
   if not(AVZDeleteFile(FileName)) then
    AddToLog('>>>$AVZ0214 '+FileName+' $AVZ0514', logAlert);
   // Звритическая чистка
   if DelMode = 2 then begin
    Lines := TStringList.Create;
    Lines.Add(FileName);
    ExtSystemClean(Lines);
    Lines.Free;
   end;
end;

procedure TMain.mmSysCheckClick(Sender: TObject);
begin
 ExecuteSysCheck;
end;

procedure TMain.IEBHO1Click(Sender: TObject);
begin
 ExecuteBHOView;
end;

procedure TMain.FormDestroy(Sender: TObject);
begin
 RxTrayIcon.Active := FALSE; // Убираем иконку в момент завершения
 if AVZDriver <> nil then begin
  AVZDriver.UnLoadDriver;
  AVZDriver.UnInstallDriver;
 end;
 DeletedVirusList.Free;
 MainAntivirus.Free;
 IncludeMackList.Free;
 ExcludeMaskList.Free;
 // Формирование кода возврата
 ExitCode := 0;
 if VirCount > 0 then ExitCode := 1
  else if PodVirCount > 0 then ExitCode := 2;
end;

procedure TMain.OnAddToDebugLog(AMsg: string);
begin
 AddToLog('[DEBUG]>'+AMsg, logDebug); //#DNL
end;

procedure TMain.mmKernelModulesClick(Sender: TObject);
begin
 ExecuteKernelObjectsView;
end;

procedure TMain.mmRepairSystemClick(Sender: TObject);
begin
 ExecuteSysRepairDLG;
end;

procedure TMain.WMGetSysCommand(var Message: TMessage);
begin
 // Сворачивание окна
 if (Message.WParam = SC_MINIMIZE) then begin
  Visible := false;
  Message.Msg := 0;
  RxTrayIcon.Active := true;
  exit;
 end else
 // Закрытие окна
 if (Message.WParam = SC_CLOSE) then begin
  // При включенном AVZ Guard блокировка закрытия окна
  if AVZGuardIsOn then begin
   Message.Msg := 0;
   exit;
  end;
  if CheckCanClose then
   Application.Terminate
  else exit;
  inherited;
 end else
  inherited;
end;

procedure TMain.RxTrayIconClick(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
 Visible := true;
 RxTrayIcon.Active := false;
end;

function TMain.OnAVZFileExtracted(AVZArcInfo : TAVZArcInfoArray): boolean;
var
 S, ST : string;
 Tmp : TAVZArcInfo;
 i : integer;
begin
 if (AVZArcInfo = nil) or (Length(AVZArcInfo) = 0) then exit;
 inc(FScanArcCount);
 Tmp := AVZArcInfo[Length(AVZArcInfo)-1];
 StatusBar.Panels[0].Text := FormatArcFileName(AVZArcInfo);
 StatusBar.Repaint;
 // Создание копий распакованных объектов
 if CopyUnpackedFiles then begin
   i := -1;
   repeat
    inc(i);
    ST := AVZArcInfo[Length(AVZArcInfo)-1].ArcType;
    if pos(':', ST) > 0 then        //#DNL
     Delete(ST, pos(':', ST), 100); //#DNL
    S := ExtractFilePath(Application.ExeName)+'UNPACKED\'+ST+'\'; //#DNL
    S := S + ExtractFileName(AVZArcInfo[Length(AVZArcInfo)-1].MainArcName);
    if i > 0 then
     S := S + IntToStr(i);
    ST := AVZArcInfo[Length(AVZArcInfo)-1].AArcFileName;
    ST := StringReplace(ST, '/','\', [rfReplaceAll]); //#DNL
    if copy(ST, 1, 1) = '\' then delete(ST, 1, 1);
    S := NormalDir(S) + ST;
   until not FileExists(S);
   zForceDirectories(ExtractFilePath(S));
   CopyFile(PChar(Tmp.TmpFileName), PChar(S), false);
 end;
 // Сканирование объекта
 VirusScan(Tmp.TmpFileName, false, nil, AVZArcInfo);
 Application.ProcessMessages;
 inc(FDopFileCount);
 // Прерывание цикла по текущему архиву
 if StopF then begin
  AVZDecompress.StopF := true;
  Abort;
 end;
end;

function TMain.OnAVZCheckFilename(AFileName: string): boolean;
begin
 // Фильтр
 Result := ( ScanAllFiles or (pos(UpperCase(ExtractFileExt(AFileName)), ExtToScan) > 0));
end;

procedure TMain.cbCheckArchivesClick(Sender: TObject);
begin
 cbMaxArcSize.Enabled := cbCheckArchives.Checked;
 seMaxArcSize.Enabled := cbCheckArchives.Checked;
end;

procedure TMain.cbRepGoodClick(Sender: TObject);
begin
 cbRepGoodCheck.Enabled := cbRepGood.Checked;
end;

procedure TMain.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
 if Key = VK_F1 then  begin
  Application.HelpJump('t_main_1');
  Key := 0;
 end; 
end;

procedure TMain.FormResize(Sender: TObject);
begin
 if Width <= 650 then
  StatusBar.Panels[0].Width := 400 else
   StatusBar.Panels[0].Width := 400 + (width - 650);
end;

function TMain.GetCureModeStr: string;
begin
 Result := '';
 // Режим
 if cbDeleteVirus.Checked then Result := '$AVZ0108'
  else begin Result := '$AVZ0178'; exit; end;
end;

procedure TMain.N34Click(Sender: TObject);
begin
 ExecuteExplorerExtView;
end;

procedure TMain.pmExitClick(Sender: TObject);
begin
 if zMessageDlg('$AVZ0244', mtConfirmation, [mbYes, mbNo], 0) = mrYes then
  mmExit.Click;
end;

procedure TMain.pmStartClick(Sender: TObject);
begin
 mbStart.Click;
end;

procedure TMain.pmStopClick(Sender: TObject);
begin
 if mbStop.Enabled then
  mbStop.Click;
end;

procedure TMain.N2Click(Sender: TObject);
begin
 ExecuteQurantineByList;
end;

procedure TMain.tRunScriptTimer(Sender: TObject);
var
 i : integer;
begin
 tRunScript.Enabled := false;
 for i := 1 to 20 do
  Application.ProcessMessages;
 try
  mbStart.Visible := false;
  mbStop.Visible := false;
  mbStopScript.Visible := true;
  AvzUserScript.ExecuteScript('', UserScriptText);
  AddToLog('$AVZ0184 ', logInfo);
 finally
  mbStopScript.Visible := false;
  mbStart.Visible := true;
  mbStop.Visible := true;
 end;
 tRunScript.Enabled := false;
end;

procedure TMain.tWatchDogTimer(Sender: TObject);
begin
 RxTrayIcon.Active := false;
 Application.Terminate;
 Application.Terminate;
 Application.Terminate;
 Sleep(100);
 ExitProcess(0);
end;

procedure TMain.DLL1Click(Sender: TObject);
begin
 ExecuteKeyloggerManager;
end;

procedure TMain.N35Click(Sender: TObject);
begin
 ExecutePrintExtManager;
end;

procedure TMain.ZShellTreeViewKeyPress(Sender: TObject; var Key: Char);
begin
 if Key = ' ' then
  if ZShellTreeView.Selected <> nil then
   ZShellTreeViewTreeNodeStateIconClick(Sender, ZShellTreeView.Selected);
end;

function TMain.OnAVZDecompressLog(AMSG: string; InfoType : integer): boolean;
begin
 AddToLog(AMSG, InfoType);
end;

procedure TMain.mmCheckWintrustClick(Sender: TObject);
var
 res : boolean;
 S : string;
 InfoType : integer;
begin
 InfoType := logNormal;
 if OpenDialog1.Execute then begin
  Res := CheckFileTrust(OpenDialog1.FileName);
  if Res then begin
   S := '$AVZ1122';
   InfoType := logGood;
  end
   else S :='$AVZ1119';
  zMessageDlg(S, mtInformation, [mbOk], 0);
  AddToLog('$AVZ1109: '+OpenDialog1.FileName+'. $AVZ0928: '+S, InfoType);
 end;
end;

procedure TMain.N38Click(Sender: TObject);
begin
 ExecuteAutoQurantine;
end;

procedure TMain.N36Click(Sender: TObject);
begin
 ExecuteTaskScheduler;
end;

procedure TMain.mmAutoUpdateClick(Sender: TObject);
begin
 ExecuteAutoUpdate;
end;

procedure TMain.cbHostsClick(Sender: TObject);
begin
 ExecuteHostsManager;
end;

procedure TMain.FormClose(Sender: TObject; var Action: TCloseAction);
begin
 RxTrayIcon.Active := FALSE; // Убираем иконку в момент завершения
end;

procedure TMain.RefrechFilterList;
var
 S, ST : string;
begin
 IncludeMackList.Clear;
 ExcludeMaskList.Clear;
 S := Trim(edIncludeFiles.Text);
 if cbIncludeFiles.Checked and (S <> '') then begin
  S := StringReplace(S, ';', ',', [rfReplaceAll]);
  S := StringReplace(S, ' ', ',', [rfReplaceAll]);
  S := S + ',';
  while pos(',', S) > 0 do begin
   st := LowerCase(Trim(copy(S, 1, pos(',', S)-1)));
   delete(S, 1, pos(',', S));
   if st <> '' then
    IncludeMackList.Add(st);
  end;
 end;
 S := Trim(edExcludeFiles.Text);
 if cbExcludeFiles.Checked and (S <> '') then begin
  S := StringReplace(S, ';', ',', [rfReplaceAll]);
  S := StringReplace(S, ' ', ',', [rfReplaceAll]);
  S := S + ',';
  while pos(',', S) > 0 do begin
   st := LowerCase(Trim(copy(S, 1, pos(',', S)-1)));
   delete(S, 1, pos(',', S));
   if st <> '' then
    ExcludeMaskList.Add(st);
  end;
 end;
end;

procedure TMain.cbIncludeFilesClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TMain.cbExcludeFilesClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TMain.rgFileTypesClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TMain.SpeedButton1Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0626 ?', mtConfirmation, [mbYes, mbNo], 0) = mrYes then begin
  LogMemo.Clear;
  VirFileList.Clear;
 end;
end;

procedure TMain.N41Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0626 ?', mtConfirmation, [mbYes, mbNo], 0) = mrYes then begin
  LogMemo.Clear;
  VirFileList.Clear;
 end;
end;

function TMain.AVScanProcessMemory(AFileName: string;
  APID: dword; AHidden : boolean): boolean;
var
 Buf      : array of byte;
 hProcess : THandle;
 lpNumberOfBytesRead        : DWORD;
 ModulesList                : TStrings;
 SysProcessInfo             : TSysProcessInfo;
 ScriptName, S              : string;
 i, j                       : integer;
 PeImageLoader, PeMemLoader : TPeImageLoader;
 SR                         : TSearchRec;
 IPPorts                    : TIPPorts;
 TCPPortListen, TCPPortConnected, TCPPortMail, TCPPortHttp, TCPPortIRC   : integer;
 AutorunManager  : TAutorunManager;
 WndAll, WndVisible, WndVisibleC, ProcessMemSize : dword;
begin
 if not(cbExtEv.Checked) then exit;
 AddToDebugLog('AVS-1 '+AFileName);
 ModulesList    := nil;
 NeuroTokenList := nil;
 IPPorts        := nil;
 SysProcessInfo := nil;
 AutorunManager := nil;
 hProcess       := INVALID_HANDLE_VALUE;
 PeImageLoader  := nil; PeMemLoader := nil;
 // Блокировка сканирования своего процесса
 if APID = GetCurrentProcessId then exit;
 try
   AddToDebugLog('AVS-2');
   NeuroTokenList  := TStringList.Create;
   ModulesList     := TStringList.Create;
   // Создание класса "информация о процессах" в зависимости от типа ОС
   if ISNT then
    SysProcessInfo := TPSAPIProcessInfo.Create
     else SysProcessInfo := TToolHelpProcessInfo.Create;
   AddToDebugLog('AVS-3');
   // Создание списка модулей
   SysProcessInfo.CreateModuleList(ModulesList, APID);
   ProcessMemSize := 0;
   if ModulesList.Count > 0 then begin
    AFileName := NTFileNameToNormalName((ModulesList.Objects[0] as TModuleInfo).ModuleName);
    ProcessMemSize := (ModulesList.Objects[0] as TModuleInfo).ModuleSize;
   end;
   AddToDebugLog('AVS-4');
   // Блокировка проверки безопасных процессов
   if FileSignCheck.CheckFile(AFileName) = 0 then exit;
   // Ограничение сканируемого размера
   if ProcessMemSize > 2000000 then
    ProcessMemSize := 2000000;
   AddToDebugLog('AVS-5');
   // Построение списка модулей
   for i := 1 to ModulesList.Count - 1 do
    NeuroTokenList.AddObject('DLL.' + UpperCase(ChangeFileExt(ExtractFileName((ModulesList.Objects[i] as TModuleInfo).ModuleName), '')), pointer(1));
   AddToDebugLog('AVS-6');
   // Информация о видимости окон
   WndAll     := GetWindowsCountByPid(APID, false);
   WndVisible := GetWindowsCountByPid(APID, true);
   WndVisibleC := 0;
   NeuroTokenList.AddObject('WND.All',     pointer(WndAll));
   NeuroTokenList.AddObject('WND.Visible', pointer(WndVisible));
   if WndVisible = 0 then begin
    WndVisibleC := 2;
    if WndAll > 0 then WndVisibleC := 1;
   end;
   NeuroTokenList.AddObject('WND.VisibleC', pointer(WndVisibleC));
   NeuroTokenList.AddObject('PROC.pID', pointer(APID));
   if AHidden then
    NeuroTokenList.AddObject('PROC.Hidden', pointer(1));
   AddToDebugLog('AVS-7');
   // Первый модуль - это исполняемый файл
   SetLength(Buf, ProcessMemSize);
   ZeroMemory(@Buf[0], length(Buf));
   // Открытие процесса - если не откроется, дальнейшая логика бесполезна
   hProcess := OpenProcess(PROCESS_ALL_ACCESS, true, APID);
   AddToDebugLog('AVS-8');
   if hProcess = 0 then exit;
   ReadProcessMemory(hProcess, pointer((ModulesList.Objects[0] as TModuleInfo).hModule), @Buf[0], length(Buf), lpNumberOfBytesRead);
   AddToDebugLog('AVS-9');
   // ************* Атрибуты и размер *************************
   if GetFileInfo(NTFileNameToNormalName(AFileName), SR) then begin
    if (SR.Attr and faHidden)   > 0 then  NeuroTokenList.AddObject('FILE.Attr.Hidden', pointer(1));
    if (SR.Attr and faSysFile)  > 0 then  NeuroTokenList.AddObject('FILE.Attr.System', pointer(1));
    if (SR.Attr and faReadOnly) > 0 then  NeuroTokenList.AddObject('FILE.Attr.Readonly', pointer(1));
    if ((SR.Attr and faHidden)   > 0) or ((SR.Attr and faSysFile)   > 0) or ((SR.Attr and faReadOnly)   > 0) then
     NeuroTokenList.AddObject('FILE.Hidden', pointer(1));
    NeuroTokenList.AddObject('FILE.Size', pointer(SR.Size));
    if not(AHidden) and (SR.Size > 1000000) then exit;
   end;
   // ******* Местоположение - детект системной папки *************
   AddToDebugLog('AVS-10');
   S := LowerCase(NTFileNameToNormalName(AFileName));
   if (pos(LowerCase(GetWindowsDirectoryPath), S) = 1) then
    NeuroTokenList.AddObject('FILE.SysDir', pointer(1));
   AddToDebugLog('AVS-11');
   // ************** Поиск упаковщика *****************************
   PeImageLoader := TPeImageLoader.Create;
   PeMemLoader   := TPeImageLoader.Create;
   if PeImageLoader.LoadPEFile(NTFileNameToNormalName(AFileName)) then begin
    j := 0;
    AddToDebugLog('AVS-12');
    for i := 0 to Length(PeImageLoader.ImageSections) - 1 do begin
     if (PeImageLoader.ImageSections[i].Characteristics and IMAGE_SCN_MEM_EXECUTE) > 0 then begin
      inc(j);
      if PeImageLoader.ImageSections[i].VirtSize > PeImageLoader.ImageSections[i].PhSize * 1.2 then begin
       NeuroTokenList.AddObject('FILE.Packed', pointer(1));
       break;
      end;
     end;
    end;
    if Length(PeImageLoader.ImageSections) = 0 then
     NeuroTokenList.AddObject('FILE.Packed', pointer(1));
    if j = 0 then
     NeuroTokenList.AddObject('FILE.Packed', pointer(1));
    //PeImageLoader.ExecuteReallocation((ModulesList.Objects[0] as TModuleInfo).hModule);
    AddToDebugLog('AVS-13');
    if (Buf <> nil) and (ModulesList.Count > 0) then begin
     AddToDebugLog('AVS-14');
     PeMemLoader.LoadMemImage(@Buf[0], ProcessMemSize);
     if length(PeImageLoader.ImageSections) <> length(PeMemLoader.ImageSections) then begin
      NeuroTokenList.AddObject('FILE.PackedS', pointer(1));
    end;
    end;
   end;
   AddToDebugLog('AVS-15');
   if PeMemLoader.PELoaded then
    if PeMemLoader.ImageOptionalHeader.SizeOfImage > ProcessMemSize+2000 then begin
     ProcessMemSize := PeMemLoader.ImageOptionalHeader.SizeOfImage;
     AddToLog('>>> $AVZ0905 = '+IntToStr(ProcessMemSize), logAlert);
     NeuroTokenList.AddObject('PROC.ImgSize.Hide', pointer(1));
     NeuroTokenList.AddObject('PROC.RealImgSize', pointer(PeMemLoader.ImageOptionalHeader.SizeOfImage));
     SetLength(Buf, ProcessMemSize);
     AddToDebugLog('AVS-16 '+IntToStr(ProcessMemSize));
     try
      ReadProcessMemory(hProcess, pointer((ModulesList.Objects[0] as TModuleInfo).hModule), @Buf[0], length(Buf), lpNumberOfBytesRead);
     except
     end;
    end;
   // ********************** Порты и соединения ***************************
   AddToDebugLog('AVS-17');
   IPPorts  := TIPPorts.Create;
   try
     IPPorts.Refresh;
     TCPPortMail := 0; TCPPortHttp := 0; TCPPortListen := 0; TCPPortConnected := 0;
     TCPPortIRC := 0;
     for i := 0 to length(IPPorts.TCPConnections) - 1 do
      if IPPorts.TCPConnections[i].ProcessID = APID then begin
       if IPPorts.TCPConnections[i].State   = 'LISTENING' then inc(TCPPortListen);
       if IPPorts.TCPConnections[i].StateID in [3..9]     then inc(TCPPortConnected);
       if (IPPorts.TCPConnections[i].StateID in [3..9]) then begin
        if IPPorts.TCPConnections[i].RemotePort in [25, 110] then inc(TCPPortMail);
        if IPPorts.TCPConnections[i].RemotePort = 80 then inc(TCPPortHttp);
        if (IPPorts.TCPConnections[i].RemotePort = 6666) or
           (IPPorts.TCPConnections[i].RemotePort = 6667) or
           (IPPorts.TCPConnections[i].RemotePort = 6668) then inc(TCPPortIRC);
       end;
      end;
      if TCPPortListen > 0 then
       NeuroTokenList.AddObject('TCP.TCPPortListen', pointer(TCPPortListen));
      if TCPPortConnected > 0 then
       NeuroTokenList.AddObject('TCP.Connected', pointer(TCPPortConnected));
      if TCPPortMail > 0 then
       NeuroTokenList.AddObject('TCP.Listen.Mail', pointer(TCPPortMail));
      if TCPPortHttp > 0 then
       NeuroTokenList.AddObject('TCP.Listen.HTTP', pointer(TCPPortHttp));
      if TCPPortIRC > 0 then
       NeuroTokenList.AddObject('TCP.Listen.IRC', pointer(TCPPortIRC));
   except end;
   AddToDebugLog('AVS-18');
   // ********************** Автозапуск ***************************
   AutorunManager  := TAutorunManager.Create;
   AutorunManager.Refresh;
   if AutorunManager.AutorunItemExists(AFileName) then
    NeuroTokenList.AddObject('File.Autorun', pointer(1));

   AddToDebugLog('AVS-19');
   // Сканирование буфера
   MemScanBase.OnMemSignFound := OnMemSignFound;
   if lpNumberOfBytesRead > 0 then
    MemScanBase.ScanBuf(Buf);

   AddToDebugLog('AVS-20');
   ESUserScript.NeuroTokenList := NeuroTokenList;
   AddToLog('$AVZ0062 '+inttostr(APID)+' '+AFileName, logNormal);

   AddToDebugLog('AVS-21');
   for i := 0 to length(ESSystem.ESScripts)-1 do begin
     if StopF then exit;
     AddToDebugLog('AVS-21.'+IntToStr(i));
     // Распаковка имени
     ScriptName := DeCompressString(ESSystem.ESScripts[i].CompressedName);
     // Распаковка строки
     S := DeCompressString(ESSystem.ESScripts[i].CompressedText);
     // Выполнение скрипта
     if not(ESUserScript.ExecuteScript(S, ScriptName)) then
      AddToLog('$AVZ0630 '+IntToStr(i), logError);
     ZeroMemory(@S[1], length(s)); S := '';
     // Отдаем квант времени
     if i mod 5 = 0 then Application.ProcessMessages;
    end;
   try SysProcessInfo.ClearList(ModulesList); except end;
   MemScanBase.NeuroTokenList := NeuroTokenList;
   MemScanBase.OnMemSuspFound := MemSuspFound;
   MemScanBase.Detect;
   // Вывод списка токенов
 //  for i := 1 to NeuroTokenList.Count - 1 do
 //   AddToLog(NeuroTokenList[i]+' = '+inttostr(integer(NeuroTokenList.Objects[i])));

 finally
  AddToDebugLog('AVS-22'); //#DNL
  CloseHandle(hProcess);
  Buf := nil;
  IPPorts.Free;
  ModulesList.Free;
  NeuroTokenList.Free;
  SysProcessInfo.Free;
  AutorunManager.Free;
  PeImageLoader.Free;
  PeMemLoader.Free;
 end;
 //#translate=on Включили перевод кода
end;

procedure TMain.OnMemSignFound(MemSign: TMemSign; Rel: DWORD);
var
 poz : integer;
 S   : string;
begin
 S := Trim(MemSign.Name+'.'+inttostr(MemSign.ID));
 poz := NeuroTokenList.IndexOf(S);
 if  poz >= 0 then
  NeuroTokenList.Objects[poz] := pointer(integer(NeuroTokenList.Objects[poz])+1)
   else NeuroTokenList.AddObject(S, pointer(1));
end;

procedure TMain.MemSuspFound(AVirName: string);
begin
 AddToLog('$AVZ0722 "' + AVirName+'"', logAlert);
end;

procedure TMain.N42Click(Sender: TObject);
begin
 ExecuteCookiesSearch;
end;

procedure TMain.mbStopScriptClick(Sender: TObject);
begin
 AvzUserScript.StopF := true;
 StopF := true;
end;

procedure TMain.mmRunScriptClick(Sender: TObject);
begin
 ExecuteRunScriptDLG;
end;

procedure TMain.mmSG_TurnOnClick(Sender: TObject);
begin
 if ActivateAVZGuard then
  zMessageDlg('$AVZ0007', mtInformation, [mbOk], 0)
 else begin
  zMessageDlg('$AVZ0641', mtError, [mbOk], 0);
 end;
 RefreshInterface;
end;

procedure TMain.mmSG_TrustedRunClick(Sender: TObject);
var
  Prg, CmdLine  : string;
  si : Tstartupinfo;
  p  : Tprocessinformation;
begin
 if ExecProcessDLGExecute(Prg, CmdLine) then begin
  try
   SetAVZGuardRestrictedMode(true);
   FillChar( Si, SizeOf( Si ) , 0 );
   with Si do begin
    cb := SizeOf( Si);
    dwFlags := startf_UseShowWindow;
    wShowWindow := 4;
   end;
   Createprocess(nil,PChar(Prg + ' '+ CmdLine),nil,nil,false,Create_default_error_mode,nil,nil,si,p);
  finally
   SetAVZGuardRestrictedMode(false);
  end;
 end;
end;


procedure TMain.mmSG_TurnOffClick(Sender: TObject);
begin
 if DeActivateAVZGuard then
  zMessageDlg('$AVZ0008', mtInformation, [mbOk], 0)
 else begin
  zMessageDlg('$AVZ0669', mtError, [mbOk], 0)
 end;
 RefreshInterface;
end;

function TMain.CheckCanClose: boolean;
begin
 Result := true;
 if AVZGuardIsOn then
  Result := zMessageDlg('$AVZ0091', mtConfirmation, [mbYes, mbNo], 0) = mrYes;
end;

procedure TMain.WMCloseHandler(var Message: TMessage);
begin
 if AVZGuardIsOn then begin
  Message.Msg := 0;
  exit;
 end; 
end;

procedure TMain.N44Click(Sender: TObject);
begin
 ExecuteProtocolView;
end;

procedure TMain.mmStdScriptsClick(Sender: TObject);
begin
 ExecuteStdScriptsDLG;
end;

procedure TMain.mmRevizorClick(Sender: TObject);
begin
 ExecuteRevizorDLG;
end;

procedure TMain.DownloadedProgramFiles1Click(Sender: TObject);
begin
 ExecuteDPFManager;
end;

procedure TMain.CPL1Click(Sender: TObject);
begin
 ExecuteCPLManager;
end;

procedure TMain.N49Click(Sender: TObject);
var
 S    : string;
 Tmp  : TTreeNode;
begin
 // Диалог запроса имени скапируемой папки
 S := InputBox(TranslateStr('$AVZ0226'), TranslateStr('$AVZ0883:'), '');
 if Trim(S) = '' then exit;
 // Попытка найти и отметить на сканирование папку с указанным именем
 Tmp := main.ZShellTreeView.SearchDirNode(S);
 if Tmp = nil then exit;
 Tmp.StateIndex := 2;
 Main.ZShellTreeView.SetChildNodesState(Tmp, Tmp.StateIndex);
 // Меняем статус родительских элементов
 if Tmp.Parent <> nil then
  Main.SetParentStatus(Tmp.Parent);
end;

procedure TMain.sbAutoUpdateClick(Sender: TObject);
begin
 // Вызов автообновления
 ExecuteAutoUpdate;
end;

procedure TMain.N50Click(Sender: TObject);
var
 Node: TTreeNode;
begin
 Node := ZShellTreeView.Selected;
 if Node = nil then exit;
 // Меняем статус
 Node.StateIndex := 4;
 // Меняем стутус родительских элементов
 if Node.Parent <> nil then
  SetParentStatus(Node.Parent);
end;

procedure TMain.ZShellTreeViewNewTreeNodeSetState(Sender: TObject; Node,
  NewNode: TTreeNode; var State: Integer);
begin
 if State = 4 then
  State := 1;
end;

procedure TMain.mbPauseClick(Sender: TObject);
begin
 ScanPauseF := not(ScanPauseF);
 mbPause.Down := ScanPauseF;
end;

procedure TMain.pmDirTreeMenuPopup(Sender: TObject);
begin
 pmPause.Enabled := mbPause.Enabled;
 pmStop.Enabled  := mbStop.Enabled;
 pmStart.Enabled := mbStart.Enabled;
end;

procedure TMain.pmShowClick(Sender: TObject);
begin
 Visible := true;
 RxTrayIcon.Active := false;
end;

procedure TMain.mmPauseClick(Sender: TObject);
begin
 if mbPause.Enabled then
  mbPause.Click;
end;

procedure TMain.pmPauseClick(Sender: TObject);
begin
 if mbPause.Enabled then
  mbPause.Click;
end;

procedure TMain.mmPMDriverInstallClick(Sender: TObject);
begin
 if InstallPMDriver = 0 then
  zMessageDlg('$AVZ0236', mtInformation, [mbOk], 0);
 RefreshInterface;
end;

procedure TMain.mmPMDriverRemoveClick(Sender: TObject);
var
 Res : integer;
begin
 try
  Res := UninstallPMDriver(false);
  case Res of
   0 : zMessageDlg('$AVZ0233', mtInformation, [mbOk], 0);
   7 : zMessageDlg('$AVZ0235', mtInformation, [mbOk], 0)
   else zMessageDlg('$AVZ0684 '+inttostr(Res), mtInformation, [mbOk], 0);
  end;
 finally
  Screen.Cursor := crDefault;
  RefreshInterface;
 end;
end;

function TMain.UninstallPMDriver(AUnload : boolean): integer;
begin
 try
  if AVZDriverRK = nil then exit;
  Screen.Cursor := crHourGlass;
  Result := zAVZDriverRK.UnInstallPMDriver(AUnload);
 finally
  Screen.Cursor := crDefault;
 end;
end;

function TMain.InstallPMDriver: integer;
begin
 try
  Screen.Cursor := crHourGlass;
  Result := zAVZDriverRK.InstallPMDriver;
 finally
  Screen.Cursor := crDefault;
 end;
end;

procedure TMain.mmSaveSetupProfileClick(Sender: TObject);
var
 Params : TStringList;
begin
 // Сохранение профиля
 if ProfileSaveDialog.Execute and (ProfileSaveDialog.FileName <> '') then begin
  Params := TStringList.Create;
  Last.CreateSetupProfile(Params);
  try Params.SaveToFile(ProfileSaveDialog.FileName); except end;
  Params.Free;
 end;
end;

procedure TMain.N52Click(Sender: TObject);
var
 Params : TStringList;
begin
 if ProfileOpenDialog.Execute and (ProfileOpenDialog.FileName<>'') and (FileExists(ProfileOpenDialog.FileName)) then begin
  Params := TStringList.Create;
  Params.LoadFromFile(ProfileOpenDialog.FileName);
  Last.ProcessSetup(Params);
  Params.Free;
 end;
end;

procedure TMain.mmPMDriverUnloadRemoveClick(Sender: TObject);
var
 Res : integer;
begin
 try
  Res := UninstallPMDriver(true);
  case Res of
   0 : zMessageDlg('$AVZ0233', mtInformation, [mbOk], 0);
   7 : zMessageDlg('$AVZ0235', mtInformation, [mbOk], 0)
   else zMessageDlg('$AVZ0684 '+inttostr(Res), mtInformation, [mbOk], 0);
  end;
 finally
  Screen.Cursor := crDefault;
  RefreshInterface;
 end;
end;

procedure TMain.ActiveSetup1Click(Sender: TObject);
begin
 ExecuteASView;
end;

function TMain.GUILink(Sender: TObject; OpCode: integer; AMessage,
  AParam: string; var Res: variant): boolean;
begin
 Result := false;
 AMessage := Trim(UpperCase(AMessage));
 // Команды GUI класса GET - запрос данных
 if OpCode = guiGet then begin
  // Запрос списка сканируемых папок
  if AMessage = 'SCANPATH' then begin
   Res := CreateScanPathStr;
   Result := true;
   exit;
  end;
  // Запрос количества подозрений на зловред
  if AMessage = 'MAIN-PODVIRCOUNT' then begin
   Res := PodVirCount;
   Result := true;
   exit;
  end;
  // Запрос количества детектированных зловредов
  if AMessage = 'MAIN-VIRCOUNT' then begin
   Res := VirCount;
   Result := true;
   exit;
  end;

 end;
 // Команды GUI класса SET - настройка и управление
 if OpCode = guiSet then begin
  // Разблокировка главного окна
  if AMessage = 'MAIN-ENABLE' then begin
    Main.Enabled := true;
    Result := true;
    exit;
  end;
  // Запуск сканирования
  if AMessage = 'MAIN-START' then begin
    mbStart.Click;
    Result := true;
    exit;
  end;
  // Сохранение протокола
  if AMessage = 'MAIN-SAVELOG' then begin
    Main.LogMemo.Lines.SaveToFile(AParam);
    Result := true;
    exit;
  end;
  // Включение WatchDog
  if AMessage = 'MAIN-SETWATCHDOG' then begin
    Main.tWatchDog.Interval := StrToIntDef(AParam, 100000) * 1000;
    Main.tWatchDog.Enabled  := true;
    Result := true;
    exit;
  end;
  // Отключение WatchDog
  if AMessage = 'MAIN-DISABLEWATCHDOG' then begin
    Main.tWatchDog.Enabled  := false;
    Result := true;
    exit;
  end;
  // Блокировка интерфейса
  if AMessage = 'MAIN-LOCKINTERFACE' then begin
    Main.Enabled := false;
    Result := true;
    exit;
  end;
  // Разблокировка интерфейса
  if AMessage = 'MAIN-UNLOCKINTERFACE' then begin
    Main.Enabled := true;
    Result := true;
    exit;
  end;
  // Отображение данных в троке статуса
  if AMessage = 'MAIN-STATUSBAR' then begin
    Main.StatusBar.Panels[0].Text := TranslateStr(AParam);
    Result := true;
    exit;
  end;
  // Обновление интерфейса
  if AMessage = 'MAIN-REFRESHINTERFACE' then begin
    RefreshInterface;
    Result := true;
    exit;
  end;
  // Завершение программы
  if AMessage = 'MAIN-TERMINATE' then begin
    Application.Terminate;
    Application.Terminate;
    sleep(50);
    Application.Terminate;
    Application.Terminate;
    sleep(50);
    ExitProcess(0);
    Result := true;
    exit;
  end;
  // Позиция прогресс-индикатора
  if AMessage = 'MAIN-PROGRESS-POS' then begin
   pbScanProgress.Position := StrToIntDef(AParam, 0);
   exit;
  end;
  // Max прогресс-индикатора
  if AMessage = 'MAIN-PROGRESS-MAX' then begin
   pbScanProgress.Max := StrToIntDef(AParam, 0);
   exit;
  end;
 end;
end;

// Построение списка папок, отмеченных на сканирование
function TMain.CreateScanPathStr: string;
var
 i : integer;
 function ScanTreeDirs(ARootNode: TTreeNode; var S : string): boolean;
  var
   i : integer;
   res : boolean;
  begin
   Result := true;
   // Дальнейшие действия
   case ARootNode.StateIndex of
    0, 1 : ; // Не выделен - никаких действий
    2    :  AddToList(S, NormalDir(ZShellTreeView.GetPath(ARootNode)), ';');
    3    : begin
            //Result := ScanDir(NormalDir(ZShellTreeView.GetPath(ARootNode)), ADeleteF, false);
            for i := 0 to ARootNode.Count - 1 do
             if ARootNode.Item[i].StateIndex > 1 then begin
              res := ScanTreeDirs(ARootNode.Item[i], S);
              Result := Result and res;
             end;
           end;
   end;
  end;
begin
 Result := '';
 for i := 0 to ZShellTreeView.Items.Count - 1 do
  if (ZShellTreeView.Items[i].Parent = nil) then
   ScanTreeDirs(ZShellTreeView.Items[i], Result);
end;

procedure TMain.OnGetLog(ALog: TStrings);
begin
 // Возвращаем текущий протокол в виде plain-текста
 ALog.AddStrings(LogMemo.Lines);
end;

function TMain.SetupLink(Sender: TObject; ASetting: string): boolean;
begin
 Last.ProcessSetupStr(ASetting);
 Application.ProcessMessages;
end;

procedure TMain.mmExtLogViewClick(Sender: TObject);
begin
 ExtLogView.Execute;
end;

procedure TMain.mmBackupClick(Sender: TObject);
begin
 ExecuteBackupScriptsDLG;
end;

procedure TMain.mmWizardsClick(Sender: TObject);
begin
 ExecuteRepairWizardDLG;
end;

procedure TMain.mmCalkMD5Click(Sender: TObject);
var
 S : string; 
begin
 // Вычисление MD5 суммы файла
 if OpenDialog1.Execute then begin
  S := CalkFileMD5(OpenDialog1.FileName);
  zMessageDlg('$AVZ1109: '+OpenDialog1.FileName+'. $AVZ1609 = '+S, mtInformation, [mbOk], 0);
  AddToLog('$AVZ1109: '+OpenDialog1.FileName+'. $AVZ1609: '+S, logNormal);
 end;
end;

end.

{
        xor     edx,edx
        mov     dr0,edx
        mov     dr1,edx
        mov     dr2,edx
        mov     dr3,edx


function ConvertSidToStringSid (Sid : PSID; var StringSid : PChar) : BOOL; stdcall; external 'advapi32.dll' name 'ConvertSidToStringSidA';

// Получить SID текущего юзера
function GetUserSID : string;
type
  SID_AND_ATTRIBUTES = record
    Sid : PSID;
    Attributes : DWORD;
  end;

var
  hToken : THandle;
  cbBuffer : Cardinal;
  Buffer : Pointer;
  UserSID : PChar;
begin
  OpenProcessToken (GetCurrentProcess, TOKEN_QUERY, hToken);
  GetTokenInformation (hToken, TokenUser, nil, 0, cbBuffer);
  GetMem (Buffer, cbBuffer);
  GetTokenInformation (hToken, TokenUser, Buffer, cbBuffer, cbBuffer);
  ConvertSidToStringSid (SID_AND_ATTRIBUTES(Buffer^).Sid, UserSID);
  Result := UserSID;
  FreeMem (Buffer);
  LocalFree (Cardinal(UserSID));
end;

// дать полные права себе на ключ реестра
procedure RegAllowAllAccess (const KeyPath : string);
var
  hk : HKEY;
  sd : PSECURITY_DESCRIPTOR;
  Count, MaxLen : DWORD;
  SubKey : PChar;
  i : integer;

begin
  RegOpenKeyEx (HKEY_CURRENT_USER, PChar(KeyPath), 0, WRITE_DAC, hk);
  GetMem (sd, 1024*1024);
  InitializeSecurityDescriptor (sd, SECURITY_DESCRIPTOR_REVISION);
  RegSetKeySecurity (hk, DACL_SECURITY_INFORMATION, sd);
  RegCloseKey (hk);
  FreeMem (sd);

  RegOpenKeyEx (HKEY_CURRENT_USER, PChar(KeyPath), 0, KEY_READ, hk);
  RegQueryInfoKey (hk, nil, nil, nil, @Count, @MaxLen, nil, nil, nil, nil, nil, nil);
  Inc (MaxLen);
  GetMem (Subkey, MaxLen);
  if Count > 0 then
    for i := 0 to Count-1 do
    begin
      RegEnumKey (hk, i, Subkey, MaxLen);
      RegAllowAllAccess (KeyPath + '\' + Subkey);
    end;
  FreeMem (Subkey);
  RegCloseKey (hk);
end;

}

