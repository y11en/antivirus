unit uLast;
// Последний модуль проекта - настройка прилождения, обработка параметров
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, zAntivirus, wintrust, zSharedFunctions, zKeyloggerHunter,
  zLogSystem, zAVKernel;

type
  TLast = class(TForm)
    procedure FormCreate(Sender: TObject);
  private
    procedure AddFileToScan(S: string);
  public
   // Отметка каталога в списке проверяемых
   Function CheckDirectory(AName : string; AState : integer) : boolean;
   // Обработка списка параметров настройки
   Function ProcessSetup(Params : TStringList) : boolean;
   // Обработка списка параметров настройки
   Function CreateSetupProfile(Params : TStringList) : boolean;
   // Обработка отдельного параметра
   Function ProcessSetupStr(Params : String) : boolean;
   // Преобразование строки в Boolean
   Function zStrToBool(S : string; ADefVal : boolean) : boolean;
   Function zBoolToStr(AVal : boolean) : string;
  end;

var
  Last: TLast;

implementation

uses Unit1, zutil;

{$R *.dfm}

function TLast.CheckDirectory(AName: string; AState: integer): boolean;
var
 Tmp     : TTreeNode;
begin
 Result := false;
 Tmp := main.ZShellTreeView.SearchDirNode(AName);
 if Tmp = nil then exit;
 Tmp.StateIndex := AState;
 Main.ZShellTreeView.SetChildNodesState(Tmp, Tmp.StateIndex);
 // Меняем статус родительских элементов
 if Tmp.Parent <> nil then
  Main.SetParentStatus(Tmp.Parent);
 Result := true;
end;

procedure TLast.FormCreate(Sender: TObject);
var
 Params         : TStringList;
 LoadedProfile  : TStringList;
 Tmp            : TStringList;
 i       : integer;
 S              : string;
begin
 Params         := TStringList.Create;
 LoadedProfile  := TStringList.Create;
 Tmp            := TStringList.Create;
 try
  // Загрузка профиля по умолчанию
  S := ChangeFileExt(Application.ExeName, '.prf');
  if FileExists(S) then begin
    Tmp.LoadFromFile(S);
    // Добавление данных из профиля
    Params.AddStrings(Tmp);
  end;
 except end;   
 // Загрузка списка параметров
 for i := 1 to ParamCount do
  Params.Add(Trim(ParamStr(i)));
 // Поиск и дозагрузка профилей
 i := 0;
 while i <= Params.Count - 1 do begin
  if pos('PROFILE',  UpperCase(Params[i])) = 1 then begin
   // Выделение имени профиля
   S := Trim(copy(Params[i], pos('=', Params[i])+1, 200));
   // Удаление ссылки на профиль
   Params.Delete(i);
   // Такой профиль уже загружался (антизацикливание)
   if LoadedProfile.IndexOf(S) < 0 then begin
    Tmp.Clear;
    try
     // Загрузка профиля
     Tmp.LoadFromFile(S);
     // Добавление данных из профиля
     Params.AddStrings(Tmp);
     // Добавление имени профиля в список загруженных
     LoadedProfile.Add(S);
    except
    end;
   end;
  end else
   inc(i); // Переход на следующую строку
 end;
 // Поиск строк без знака = внутри
 i := 0;
 while i <= Params.Count - 1 do begin
  if pos('=', Params[i]) = 0 then begin
   if DirectoryExists(Params[i]) then
    CheckDirectory(Params[i], 2) else
   if FileExists(Params[i]) then
    AddFileToScan(Params[i]);
   // Удаление параметра без знака равенства
   Params.Delete(i);
  end else
   inc(i); // Переход на следующую строку
 end;
 ProcessSetup(Params);
 // Запуск сканирования
 if zStrToBool(Params.Values['RUN'], false) then
  Main.tRunScan.Enabled := true;
 if Params.Values['SCRIPT']<>'' then begin
  Tmp.Clear;
  try
   S := Params.Values['SCRIPT'];
   Tmp.LoadFromFile(S);
   Main.UserScriptText := Tmp.Text;
   AddToLog('$AVZ0185 '+S, logInfo);
   Main.tRunScript.Enabled := true;
  except end;
 end;
 LoadedProfile.Free;
 Params.Free;
 Tmp.Free;
end;

procedure TLast.AddFileToScan(S : string);
begin
  S := StringReplace(S, '"', '', [rfReplaceAll]);
  S := Trim(S);
  if S = '' then exit;
  if Main.ScanFileList.IndexOf(S) >= 0 then exit;
  Main.ScanFileList.Add(S);
end;

function TLast.ProcessSetup(Params: TStringList): boolean;
var
 S              : string;
 i              : integer;
begin
 if Params.Values['DEBUG'] <> '' then
  DebugLogMode := zStrToBool(Params.Values['DEBUG'], false);
 // Пометка дисков на сканирование
 if (Params.Values['SCANDRIVE'] <> '') then begin
  S := UpperCase(Trim(Params.Values['SCANDRIVE']));
  if pos('HDD', S)>0 then
   Main.mmSelectHDD.Click;
  if pos('CDROM', S)>0 then
   Main.mmSelectCDROM.Click;
  if pos('FDD', S)>0 then
   Main.mmSelectFDD.Click;
 end;
 // Отметка папок на сканирование
 for i := 0 to Params.Count - 1 do
  if pos('SCAN=',  UpperCase(Params[i])) = 1 then
   CheckDirectory(copy(Params[i], pos('=', Params[i])+1, 200), 2);
 // Отмена сканирования для заданных папок
 for i := 0 to Params.Count - 1 do
  if pos('NOSCAN=',  UpperCase(Params[i])) = 1 then
   CheckDirectory(copy(Params[i], pos('=', Params[i])+1, 200), 1);
 // Сканирование отдельных файлов
 for i := 0 to Params.Count - 1 do
  if pos('SCANFILE=',  UpperCase(Params[i])) = 1 then
   AddFileToScan(copy(Params[i], pos('=', Params[i])+1, 200));
 // Сканирование процессов
 if Params.Values['ScanProcess'] <> '' then
  Main.cbCheckProcess.Checked := zStrToBool(Params.Values['ScanProcess'], true);
 // Сканирование системы микропрограммами эвристики
 if Params.Values['ScanSystem'] <> '' then
  Main.cbEvSysCheck.Checked := zStrToBool(Params.Values['ScanSystem'], true);
 // Сканирование системы микропрограммами IPU
 if Params.Values['ScanSystemIPU'] <> '' then
  Main.cbEvSysIPU.Checked := zStrToBool(Params.Values['ScanSystemIPU'], true);
 // Проверка архивов
 if Params.Values['CheckArchives'] <> '' then
  Main.cbCheckArchives.Checked := zStrToBool(Params.Values['CheckArchives'], true);
 if Params.Values['MaxArcSize'] <> '' then
  Main.seMaxArcSize.Value := StrToIntDef(Params.Values['MaxArcSize'], 10);

 // Сканирование всех файлов
 if Params.Values['ScanAllFiles'] <> '' then
  if zStrToBool(Params.Values['ScanAllFiles'], false) then
   Main.rgFileTypes.ItemIndex := 1;
 // Сканирование всех файлов
 if Params.Values['ScanFilesMode'] <> '' then
   Main.rgFileTypes.ItemIndex := StrToIntDef(Params.Values['ScanFilesMode'], 0);
 // Маска на включение файлов
 if Params.Values['IncludeFiles'] <> '' then begin
  Main.cbIncludeFiles.Checked := true;
  Main.edIncludeFiles.Text := Params.Values['IncludeFiles'];
 end;
 // Маска на исключение файлов
 if Params.Values['ExcludeFiles'] <> '' then begin
  Main.cbExcludeFiles.Checked := true;
  Main.edExcludeFiles.Text := Params.Values['ExcludeFiles'];
 end;
 // Копирование удаляемых файлов в Infected
 if Params.Values['UseInfected'] <> '' then
  Main.cbInfected.Checked := zStrToBool(Params.Values['UseInfected'], false);
 // Копирование подозрительных файлов в карантин
 if Params.Values['UseQuarantine'] <> '' then
  Main.cbQuarantine.Checked := zStrToBool(Params.Values['UseQuarantine'], false);
 // Уровень эвристики
 if (Params.Values['EvLevel'] <> '') and (StrToIntDef(Params.Values['EvLevel'], -1) in [0..3]) then
  Main.tbEvLevel.Position := 3 - StrToIntDef(Params.Values['EvLevel'], 0);
 if (Params.Values['ExtEvCheck'] <> '') then
  Main.cbExtEv.Checked := zStrToBool(Params.Values['ExtEvCheck'], false);
 // Противодействие руткитам
 if (Params.Values['RootKitDetect'] <> '') then
  Main.cbCheckRootKit.Checked := zStrToBool(Params.Values['RootKitDetect'], true);
 if (Params.Values['AntiRootKitSystem'] <> '') then begin
  Main.cbAntiRootKitSystemUser.Checked := zStrToBool(Params.Values['AntiRootKitSystem'], false);
  Main.cbAntiRootKitSystemKernel.Checked := zStrToBool(Params.Values['AntiRootKitSystem'], false);
 end;
 if (Params.Values['AntiRootKitSystemUser'] <> '') then begin
  Main.cbAntiRootKitSystemUser.Checked := zStrToBool(Params.Values['AntiRootKitSystemUser'], false);
 end;
 if (Params.Values['AntiRootKitSystemKernel'] <> '') then begin
  Main.cbAntiRootKitSystemKernel.Checked := zStrToBool(Params.Values['AntiRootKitSystemKernel'], false);
 end;
 if (Params.Values['AntiRootKitSystem'] <> '') then begin
  Main.cbAntiRootKitSystemUser.Checked := zStrToBool(Params.Values['AntiRootKitSystem'], false);
 end;
 if not(Main.cbCheckRootKit.Checked) and (Main.cbAntiRootKitSystemUser.Checked or Main.cbAntiRootKitSystemKernel.Checked) then
  Main.cbCheckRootKit.Checked := true;

 // Расширенное удаление файлов
 if (Params.Values['WebServerMode'] <> '') and zStrToBool(Params.Values['WebServerMode'], false) then begin
  Main.cbExtFileDelete.Checked           := false;
  Main.cbSearchTrojanPorts.Checked       := false;
  Main.cbKeyloggerSearch.Checked         := false;
  Main.cbCheckLSP.Checked                := false;
  Main.cbCheckRootKit.Checked            := false;
  Main.cbAntiRootKitSystemUser.Checked   := false;
  Main.cbCheckNTFSStream.Checked         := false;
  Main.cbAntiRootKitSystemKernel.Checked := false;
  Main.cbCheckProcess.Checked            := false;
  Main.cbEvSysCheck.Checked              := false;
  Main.cbEvSysIPU.Checked                := false;
 end;

 // LSP
 if (Params.Values['CheckLSP'] <> '') then begin
  Main.cbCheckLSP.Checked := zStrToBool(Params.Values['CheckLSP'], true);
 end;
 if Main.cbCheckLSP.Checked and (Params.Values['AutoRepairLSP'] <> '') then begin
  Main.cbAutoRepairLSP.Checked := zStrToBool(Params.Values['AutoRepairLSP'], false);
 end;
 // Поиск кейлоггеров
 if (Params.Values['KeyloggerSearch'] <> '') then begin
  Main.cbKeyloggerSearch.Checked := zStrToBool(Params.Values['KeyloggerSearch'], true);
 end;
 // Порты троянов
 if (Params.Values['SearchTrojanPorts'] <> '') then begin
  Main.cbSearchTrojanPorts.Checked := zStrToBool(Params.Values['SearchTrojanPorts'], true);
 end;

 // Расширенное удаление файлов
 if (Params.Values['ExtFileDelete'] <> '') then begin
  Main.cbExtFileDelete.Checked := zStrToBool(Params.Values['ExtFileDelete'], true);
 end;

 // Распаковка архивов
 if (Params.Values['Unpack_Archives'] <> '') then begin
  Main.CopyUnpackedFiles := zStrToBool(Params.Values['Unpack_Archives'], true);
 end;
 if (Params.Values['Extract_Archives'] <> '') then begin
  Main.CopyUnpackedFiles := zStrToBool(Params.Values['Extract_Archives'], true);
 end;

 if (Params.Values['ArcDefPWD'] <> '') then begin
  AVZDecompress.DefaultPass := Params.Values['ArcDefPWD'];
 end;

 // Уровень контроля WinTrust
 if (Params.Values['WinTrustLevel'] <> '') then begin
  WinTrustLevel := StrToIntDef(Params.Values['WinTrustLevel'], 1);
 end;

  // Режим видимости
 if (Params.Values['HiddenMode'] <> '') then begin
  case StrToIntDef(Params.Values['HiddenMode'], 0) of
   0 : Application.ShowMainForm := true;
   1 : begin
        Application.ShowMainForm := false;
        Main.RxTrayIcon.Active   := true;
        Main.RxTrayIcon.Enabled  := true;
       end;
   2 : begin
        Application.ShowMainForm := false;
        Main.RxTrayIcon.Active   := true;
        Main.RxTrayIcon.Enabled  := false;
       end;
   3 : Application.ShowMainForm := false;
   else Application.ShowMainForm := true;
  end;
 end;

 // Настройка приоритета
 if (Params.Values['Priority'] <> '') then
  case StrToIntDef(Params.Values['Priority'], 10) of
   -1 : SetPriorityClass(GetCurrentProcess, IDLE_PRIORITY_CLASS);
    1 : SetPriorityClass(GetCurrentProcess, HIGH_PRIORITY_CLASS);
  end;
 if (Params.Values['Priority'] <> '') then begin
  if UpperCase(Params.Values['Priority']) = 'LOW' then SetPriorityClass(GetCurrentProcess, IDLE_PRIORITY_CLASS);
  if UpperCase(Params.Values['Priority']) = 'HIGH' then SetPriorityClass(GetCurrentProcess, HIGH_PRIORITY_CLASS);
 end;

 // Настройка задержек в сканировании
 if (Params.Values['SleepScanTime'] <> '') then begin
  SleepScanTime := StrToIntDef(Params.Values['SleepScanTime'], 0)
 end;

 // Настройка задержек в сканировании
 if (Params.Values['ScanAVZFolders'] <> '') then begin
  ScanAVZFolders := zStrToBool(Params.Values['ScanAVZFolders'], false)
 end;

 // режим работы в сети
 if zStrToBool(Params.Values['NQ'], false) then
  NetWorkMode := true;

 // отчет о чистых файлах
 if Params.Values['RepGoodFiles'] <> '' then
  Main.cbRepGood.Checked := zStrToBool(Params.Values['RepGoodFiles'], false);
 if (Params.Values['RepGoodCheck'] <> '') and Main.cbRepGood.Checked then
  Main.cbRepGoodCheck.Checked := zStrToBool(Params.Values['RepGoodCheck'], false);

 // Режим реакции на вирусы
 if (Params.Values['ModeVirus'] <> '') and (StrToIntDef(Params.Values['ModeVirus'], -1) in [0..2]) then
  Main.cbModeVirus.ItemIndex := StrToIntDef(Params.Values['ModeVirus'], 0);
 if (Params.Values['ModeAdvWare'] <> '') and (StrToIntDef(Params.Values['ModeAdvWare'], -1) in [0..2]) then
  Main.cbModeAdvWare.ItemIndex := StrToIntDef(Params.Values['ModeAdvWare'], 0);
 if (Params.Values['ModeSpy'] <> '') and (StrToIntDef(Params.Values['ModeSpy'], -1) in [0..2]) then
  Main.cbModeSpy.ItemIndex := StrToIntDef(Params.Values['ModeSpy'], 0);
 if (Params.Values['ModePornWare'] <> '') and (StrToIntDef(Params.Values['ModePornWare'], -1) in [0..2]) then
  Main.cbModePornWare.ItemIndex := StrToIntDef(Params.Values['ModePornWare'], 0);
 if (Params.Values['ModeRiskWare'] <> '') and (StrToIntDef(Params.Values['ModeRiskWare'], -1) in [0..2]) then
  Main.cbModeRiskWare.ItemIndex := StrToIntDef(Params.Values['ModeRiskWare'], 0);
 if (Params.Values['ModeHackTools'] <> '') and (StrToIntDef(Params.Values['ModeHackTools'], -1) in [0..2]) then
  Main.cbModeHackTools.ItemIndex := StrToIntDef(Params.Values['ModeHackTools'], 0);

 if zStrToBool(Params.Values['DelVir'], false) then
  Main.cbDeleteVirus.Checked := true;
 Main.MiniLogMode := zStrToBool(Params.Values['MiniLog'], false);
 FSpoolLogFileName := Trim(Params.Values['SpoolLog']);
 // Папка карантина
 if (Params.Values['QuarantineBaseFolder'] <> '') then
  QuarantineBaseFolder := NormalDir(Params.Values['QuarantineBaseFolder']);
 // Папка TEMP
 if (Params.Values['TempFolder'] <> '') then begin
  AVZTempDirectoryPath := NormalDir(Params.Values['TempFolder']);
  zForceDirectories(AVZTempDirectoryPath);
  AVZDecompress.TempPath := AVZTempDirectoryPath;
 end;

 // Кейлоггер
 if (Params.Values['KHM'] <> '') then
  KeyloggerHookMask := Params.Values['KHM'];

 if zStrToBool(Params.Values['AM'], false) then begin
  Main.Caption      := '';
  Application.Name  := '';
  Application.Title := '';
 end;
end;

function TLast.ProcessSetupStr(Params: String): boolean;
var
 Lines : TStringList;
begin
  Params := Trim(Params)+' ';
  Lines := TStringList.Create;
  Lines.Add(Trim(Params));
  ProcessSetup(Lines);
  Lines.Free;
  Main.RefreshInterface;
end;

function TLast.zStrToBool(S: string; ADefVal : boolean): boolean;
begin
 Result := ADefVal;
 S := AnsiUpperCase(Trim(S));
 if S = '' then exit;
 // Анализ состояния TRUE
 if (S = 'Y') or (S = '1') or (S = 'YES') or (S = 'ДА') or (StrToIntDef(S, 0) > 0) then begin
  Result := true;
  exit;
 end;
 // Анализ состояния FALSE
 if (S = 'N') or (StrToIntDef(S, -1) = 0) or (S = 'NO') or (S = 'НЕТ') then begin
  Result := false;
  exit;
 end;
end;

function TLast.CreateSetupProfile(Params: TStringList): boolean;
var
 i : integer;
 function ScanTreeDirs(ARootNode: TTreeNode): boolean;
 var
   i : integer;
   res : boolean;
 begin
   Result := true;
   // Дальнейшие действия
   case ARootNode.StateIndex of
    0, 1 : ; // Не выделен - никаких действий
    2    : Params.Add('Scan='+ NormalDir(Main.ZShellTreeView.GetPath(ARootNode)));
    3    : begin
            for i := 0 to ARootNode.Count - 1 do
             if ARootNode.Item[i].StateIndex > 1 then begin
              res := ScanTreeDirs(ARootNode.Item[i]);
              Result := Result and res;
             end;
           end;
  end;
end;
begin
 for i := 0 to Main.ZShellTreeView.Items.Count - 1 do begin
  if (Main.ZShellTreeView.Items[i].Parent = nil) then
   ScanTreeDirs(Main.ZShellTreeView.Items[i]);
 end;
 // Сканирование процессов
 Params.Add('ScanProcess='+zBoolToStr(Main.cbCheckProcess.Checked));
 // Сканирование системы микропрограммами эвристики
 Params.Add('ScanSystem='+zBoolToStr(Main.cbEvSysCheck.Checked));
 // Сканирование системы микропрограммами IPU
 Params.Add('ScanSystemIPU='+zBoolToStr(Main.cbEvSysIPU.Checked));
 // Проверка архивов
 Params.Add('CheckArchives='+zBoolToStr(Main.cbCheckArchives.Checked));
 Params.Add('MaxArcSize='+IntToStr(Main.seMaxArcSize.AsInteger));

 // Сканирование всех файлов
 Params.Add('ScanFilesMode='+IntToStr(Main.rgFileTypes.ItemIndex));

 // Маска на включение файлов
 if  Main.cbIncludeFiles.Checked then
  Params.Add('IncludeFiles='+Main.edIncludeFiles.Text);

 // Маска на исключение файлов
 if  Main.cbExcludeFiles.Checked then
  Params.Add('ExcludeFiles='+Main.edExcludeFiles.Text);

 // Копирование удаляемых файлов в Infected
 Params.Add('UseInfected='+zBoolToStr(Main.cbInfected.Checked));
 // Копирование подозрительных файлов в карантин
 Params.Add('UseQuarantine='+zBoolToStr(Main.cbQuarantine.Checked));

 // Уровень эвристики
 Params.Add('EvLevel='+IntToStr(3-Main.tbEvLevel.Position));
 Params.Add('ExtEvCheck='+zBoolToStr(Main.cbExtEv.Checked));

 // Противодействие руткитам
 Params.Add('RootKitDetect='+zBoolToStr(Main.cbCheckRootKit.Checked));
 Params.Add('AntiRootKitSystemUser='+zBoolToStr(Main.cbAntiRootKitSystemUser.Checked));
 Params.Add('AntiRootKitSystemKernel='+zBoolToStr(Main.cbAntiRootKitSystemKernel.Checked));

 // LSP
 Params.Add('CheckLSP='+zBoolToStr(Main.cbCheckLSP.Checked));
 if Main.cbCheckLSP.Checked then
  Params.Add('AutoRepairLSP='+zBoolToStr(Main.cbAutoRepairLSP.Checked));

 // Поиск кейлоггеров
 Params.Add('KeyloggerSearch='+zBoolToStr(Main.cbKeyloggerSearch.Checked));
 // Порты троянов
 Params.Add('SearchTrojanPorts='+zBoolToStr(Main.cbSearchTrojanPorts.Checked));

 // Расширенное удаление файлов
 Params.Add('ExtFileDelete='+zBoolToStr(Main.cbExtFileDelete.Checked));

 // отчет о чистых файлах
 Params.Add('RepGoodFiles='+zBoolToStr(Main.cbRepGood.Checked));
 Params.Add('RepGoodCheck='+zBoolToStr(Main.cbRepGoodCheck.Checked));

 // Режимы реакции на вирусы
 Params.Add('ModeVirus='+IntToStr(Main.cbModeVirus.ItemIndex));
 Params.Add('ModeAdvWare='+IntToStr(Main.cbModeAdvWare.ItemIndex));
 Params.Add('ModeSpy='+IntToStr(Main.cbModeSpy.ItemIndex));
 Params.Add('ModePornWare='+IntToStr(Main.cbModePornWare.ItemIndex));
 Params.Add('ModeRiskWare='+IntToStr(Main.cbModeRiskWare.ItemIndex));
 Params.Add('ModeHackTools='+IntToStr(Main.cbModeHackTools.ItemIndex));
 Params.Add('DelVir='+zBoolToStr(Main.cbDeleteVirus.Checked));

end;

function TLast.zBoolToStr(AVal: boolean): string;
begin
 if AVal then
  Result := 'Y' else Result := 'N';
end;

end.
