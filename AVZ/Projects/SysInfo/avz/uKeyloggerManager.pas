unit uKeyloggerManager;
// Менеджер кейлоггеров
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Menus, Grids, RXGrids, Buttons, ExtCtrls, zProcess, SystemMonitor,
  zSharedFunctions, zHTMLTemplate, zKeyloggerHunter, zAVKernel;

type
  TKeyloggerInfo = record
   FileName    : string; // Исполняемый файл
   Msg         : string; // Ссобщение системы с вердиктом
   TraceResult : string; // Результат поведенческого анализа 
   CheckResult : integer; // Результат проверки по базе чистых
   NeiroResult : double;  // Рейтинг, присвоенный нейросетью
  end;
  TKeyloggerInfoArray = array of TKeyloggerInfo;

  TKeyloggerManager = class(TForm)
    Panel3: TPanel;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    sbRefresh: TSpeedButton;
    sbSaveLog: TSpeedButton;
    dgKeyloggerModules: TRxDrawGrid;
    pmKernelTablePopUp: TPopupMenu;
    mmQurantine: TMenuItem;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
    MenuItem7: TMenuItem;
    procedure FormCreate(Sender: TObject);
    procedure dgKeyloggerModulesDrawCell(Sender: TObject; ACol,
      ARow: Integer; Rect: TRect; State: TGridDrawState);
    procedure dgKeyloggerModulesClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure sbRefreshClick(Sender: TObject);
    procedure sbSaveLogClick(Sender: TObject);
    procedure MenuItem5Click(Sender: TObject);
    procedure MenuItem6Click(Sender: TObject);
    procedure MenuItem7Click(Sender: TObject);
  private
    function CheckDLLforKeyLogger(ModuleInfo: TModuleInfo): boolean;
    procedure AddKeyloggerEv(AFileName, AMsg: string; ModuleInfo: TModuleInfo);
    { Private declarations }
  public
   FModulesList    : TKeyloggerInfoArray;
   Function RefreshList : boolean;
   Function Execute : boolean;
   Function OrderResults(ACol : integer) : boolean;
   Function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  KeyloggerManager: TKeyloggerManager;

function   ExecuteKeyloggerManager : boolean;
implementation

uses zutil, zAVZKernel, uGUIShared, zTranslate;
{$R *.dfm}

function   ExecuteKeyloggerManager : boolean;
begin
 KeyloggerManager := nil;
 try
  KeyloggerManager := TKeyloggerManager.Create(nil);
  Result := KeyloggerManager.Execute;
 finally
  KeyloggerManager.Free;
  KeyloggerManager := nil;
 end;

end;

{ TKeyloggerManager }

function TKeyloggerManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(FModulesList) - 1 do
  if FModulesList[i].CheckResult <> 0 then
   CopyToInfected(FModulesList[i].FileName, '$AVZ0973', 'Quarantine');
end;

function TKeyloggerManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S: string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0496');
 GoodFiles := 0;
 for i := 0 to length(FModulesList) - 1 do begin
  if FModulesList[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (FModulesList[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if FModulesList[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   if not(HideGoodFiles and (FModulesList[i].CheckResult = 0)) then
    ALines.Add('<TR '+S+'>'+
               '<TD>'+FModulesList[i].FileName+
               '<TD>'+FModulesList[i].Msg+
               '<TD>'+FormatFloat('#0.0', FModulesList[i].NeiroResult)+'%'
               );

  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(FModulesList))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

function TKeyloggerManager.Execute: boolean;
begin
 RefreshList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

function TKeyloggerManager.OrderResults(ACol: integer): boolean;
begin

end;

// Добавление кейлоггера
procedure TKeyloggerManager.AddKeyloggerEv(AFileName, AMsg : string; ModuleInfo: TModuleInfo);
begin
 AMsg := TranslateStr(AMsg);
 SetLength(FModulesList, Length(FModulesList)+1);
 FModulesList[Length(FModulesList)-1].FileName := NTFileNameToNormalName(AFileName);
 FModulesList[Length(FModulesList)-1].Msg      := AMsg;
 FModulesList[Length(FModulesList)-1].CheckResult := FileSignCheck.CheckFile(NTFileNameToNormalName(AFileName));
 FModulesList[Length(FModulesList)-1].NeiroResult := NeuroKeyloggerCheck(AFileName, false);
 FModulesList[Length(FModulesList)-1].TraceResult := GetModuleTraceResult(ModuleInfo.hModule, ModuleInfo.ModuleSize);
end;

function TKeyloggerManager.CheckDLLforKeyLogger(ModuleInfo: TModuleInfo) : boolean;
var
 Buf           : array[0..1000] of byte;
 dwBytesReaded : DWORD;
 DLLPath       : string;
 hLib          : THandle;
begin
 Result := false;
 // Это наш собственный процесс
 if UpperCase(ModuleInfo.ModuleName) = UpperCase(Application.ExeName) then begin
  Result := true;
  exit;
 end;
 hLib := LoadLibrary(PChar(ModuleInfo.ModuleName));
 FreeLibrary(hLib);
 // Опасно - подделка имени DLL
 if hLib <> ModuleInfo.hModule then begin
  AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0737', ModuleInfo);
  exit;
 end;
 // Сравнение имени DLL с известным именем
 if KeyloggerBase.SysDLL.IndexOf(ExtractFileName(ModuleInfo.ModuleName)) >= 0 then begin
  // Читаем ее память
  ReadProcessMemory(GetCurrentProcess, pointer(ModuleInfo.hModule), @Buf, SizeOf(Buf), dwBytesReaded);
  DLLPath := '';
  // Поиск в папке System
  if FileExists(GetSystemDirectoryPath+ExtractFileName(ModuleInfo.ModuleName)) then
   DLLPath := GetSystemDirectoryPath;
  // Поиск в папке Windows
  if FileExists(GetWindowsDirectoryPath+ExtractFileName(ModuleInfo.ModuleName)) then
   DLLPath := GetWindowsDirectoryPath;
  // Поиск в папке с конфликными версиями
  if pos(AnsiLowerCase(NormalDir(GetWindowsDirectoryPath)+'WinSxS\x86_Microsoft'), AnsiLowerCase(ModuleInfo.ModuleName)) = 1 then
   DLLPath := ExtractFilePath(ModuleInfo.ModuleName);
  if DLLPath = '' then begin
   AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0728', ModuleInfo);
   Result := true;
   exit;
  end;
  if not(AnsiUpperCase(DLLPath + ExtractFileName(ModuleInfo.ModuleName)) = AnsiUpperCase(ModuleInfo.ModuleName)) then begin
   AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0727', ModuleInfo);
   Result := true;
  end;
 end else begin
  AddKeyloggerEv(ModuleInfo.ModuleName, '$AVZ0726', ModuleInfo);
  Result := true;
 end;
end;

function TKeyloggerManager.RefreshList: boolean;
var
 SysProcessInfo : TSysProcessInfo;
 Lines, NewList : TStringList;
 i,j            : integer;
 FoundFl        : boolean;
begin
 FModulesList  := nil;
 // Загрузка базы
 if not(KeyloggerBase.LoadBinDatabase) then begin
  exit;
 end;
 MainAntivirus.LoadBinDatabase;
 // Создание анализатора списка модулей
 if ISNT then
  SysProcessInfo := TPSAPIProcessInfo.Create
   else SysProcessInfo := TToolHelpProcessInfo.Create;
 Lines   := TStringList.Create;
 NewList := TStringList.Create;
 // Первичное пострение списка модулей
 SysProcessInfo.CreateModuleList(Lines, GetCurrentProcessId);
 // "Дразнение" потенциальных кейлоггеров и анализ результатов
 KeyloggerHunter;
 // Повторное построение списка модулей
 SysProcessInfo.CreateModuleList(NewList, GetCurrentProcessId);
 for i := 1 to Lines.Count - 1 do begin
  CheckDLLforKeyLogger(TModuleInfo(Lines.Objects[i]));
 end;
 // Сравнение списков
 if NewList.Count <> Lines.Count then
 for i := 0 to NewList.Count - 1 do begin
  FoundFl := false;
  for j := 0 to Lines.Count - 1 do
   if TModuleInfo(NewList.Objects[i]).ModuleName = TModuleInfo(Lines.Objects[j]).ModuleName then begin
    FoundFl := true;
    Break;
   end;
  // Эта DLL не имеет системного имени - значит, это точно троян
  if not(FoundFl) then
   AddKeyloggerEv(NTFileNameToNormalName(TModuleInfo(NewList.Objects[i]).ModuleName), '$AVZ0933', TModuleInfo(NewList.Objects[i]));
 end;
 SysProcessInfo.ClearList(Lines);
 SysProcessInfo.ClearList(NewList);
 SysProcessInfo.Free;
 dgKeyloggerModules.RowCount := 2;
 if Length(FModulesList) > 0 then
  dgKeyloggerModules.RowCount :=  Length(FModulesList) + 1;
end;

procedure TKeyloggerManager.FormCreate(Sender: TObject);
begin
 FModulesList  := nil;
 TranslateForm(Self);
end;

procedure TKeyloggerManager.dgKeyloggerModulesDrawCell(Sender: TObject;
  ACol, ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S   : string;
 Tmp : TKeyloggerInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0581';
   2 : S := '$AVZ0496';
   3 : S := '$AVZ0061';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(FModulesList) then begin
   Tmp := FModulesList[ARow - 1];
   case ACol of
    0 : begin
         S := Trim(Tmp.FileName);
        end;
    1 : begin
         S := Tmp.Msg;
        end;
    2 : begin
         S := FormatFloat('#0.0', Tmp.NeiroResult)+'%';
        end;
    3 : begin
         S := Tmp.TraceResult;
        end;
   end;
  // Цветовое выделение
  if not((gdFocused in State) or (gdSelected in State)) then begin
   if Tmp.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
  end else begin
   if Tmp.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Brush.Color := clGreen;
  end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

procedure TKeyloggerManager.dgKeyloggerModulesClick(Sender: TObject);
var
 Tmp : TKeyloggerInfo;
begin
 if dgKeyloggerModules.Row - 1 < length(FModulesList) then begin
  Tmp :=  FModulesList[dgKeyloggerModules.Row - 1];
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
 end;
end;

procedure TKeyloggerManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TKeyloggerManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgKeyloggerModules.Row - 1 < length(FModulesList) then begin
  if FModulesList[dgKeyloggerModules.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+FModulesList[dgKeyloggerModules.Row - 1].FileName+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(FModulesList[dgKeyloggerModules.Row - 1].FileName, '$AVZ0973', 'Quarantine');
 end;
end;

procedure TKeyloggerManager.sbRefreshClick(Sender: TObject);
begin
 RefreshList;
 dgKeyloggerModules.Repaint;
end;

procedure TKeyloggerManager.sbSaveLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add('<H2 align=center>$AVZ0394</H2>');
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_injdll.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;


procedure TKeyloggerManager.MenuItem5Click(Sender: TObject);
begin
 if dgKeyloggerModules.Row - 1 < length(FModulesList) then
  WebSearchInGoogle(FModulesList[dgKeyloggerModules.Row-1].FileName);
end;

procedure TKeyloggerManager.MenuItem6Click(Sender: TObject);
begin
 if dgKeyloggerModules.Row - 1 < length(FModulesList) then
  WebSearchInYandex(FModulesList[dgKeyloggerModules.Row-1].FileName);
end;

procedure TKeyloggerManager.MenuItem7Click(Sender: TObject);
begin
 if dgKeyloggerModules.Row - 1 < length(FModulesList) then
  WebSearchInRambler(FModulesList[dgKeyloggerModules.Row-1].FileName);
end;

end.
