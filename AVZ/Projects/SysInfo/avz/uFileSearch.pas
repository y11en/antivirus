unit uFileSearch;
// ****************** Поиск файлов на диске **********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ComCtrls, zTreeViews, StdCtrls, ExtCtrls, Menus, ImgList,
  Buttons, zFilterCmp, Masks, zSharedFunctions, uFileDeleteModeDLG,
  wintrust, zLogSystem, zAVKernel, zAVZKernel;

type
  TFileSearchDLG = class(TForm)
    Panel1: TPanel;
    GroupBox2: TGroupBox;
    Label1: TLabel;
    GroupBox1: TGroupBox;
    ZShellTreeView: TZShellTreeView;
    pmDirTreeMenu: TPopupMenu;
    N17: TMenuItem;
    N18: TMenuItem;
    N19: TMenuItem;
    N20: TMenuItem;
    N21: TMenuItem;
    StateImageList: TImageList;
    mbStart: TBitBtn;
    mbStop: TBitBtn;
    GroupBox3: TGroupBox;
    StatusBar: TStatusBar;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    LogMemo: TMemo;
    TabSheet2: TTabSheet;
    lvFoundFiles: TListView;
    BitBtn2: TBitBtn;
    cbSearchText: TCheckBox;
    edText: TEdit;
    BitBtn1: TBitBtn;
    cbAddFiles: TCheckBox;
    Timer1: TTimer;
    cbExcludeGoodFiles: TCheckBox;
    PopupMenu1: TPopupMenu;
    pmSelectAll: TMenuItem;
    pmUnSelectAll: TMenuItem;
    pmInvertSelect: TMenuItem;
    cbExcludeWindowsTrusted: TCheckBox;
    feSize: TFilterEdit;
    feDateC: TFilterEdit;
    feDateM: TFilterEdit;
    edFilter: TComboBox;
    procedure FormCreate(Sender: TObject);
    procedure ZShellTreeViewTreeNodeStateIconClick(Sender: TObject;
      Node: TTreeNode);
    procedure SetParentStatus(ANode : TTreeNode);
    procedure N21Click(Sender: TObject);
    procedure N17Click(Sender: TObject);
    procedure mbStartClick(Sender: TObject);
    procedure mbStopClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    function FormHelp(Command: Word; Data: Integer;
      var CallHelp: Boolean): Boolean;
    procedure pmSelectAllClick(Sender: TObject);
    procedure pmUnSelectAllClick(Sender: TObject);
    procedure pmInvertSelectClick(Sender: TObject);
    procedure ZShellTreeViewKeyPress(Sender: TObject; var Key: Char);
    procedure lvFoundFilesCompare(Sender: TObject; Item1, Item2: TListItem;
      Data: Integer; var Compare: Integer);
    procedure lvFoundFilesColumnClick(Sender: TObject;
      Column: TListColumn);
    function feDateMFilterEditTranslateEvent(Sender: TObject;
      AStr: String): String;
  private
    function RefreshDirInfo: boolean;
  public
   OrderField : integer;           // Код столбца, по которому идет сортировка
   StopF : Boolean;                // Флаг аварийной остановки
   SizeMin, SizeMax    : integer;  // Размер
   DateCMin, DateCMax  : TDate;    // Дата создания
   DateMMin, DateMMax  : TDate;    // Дата модификации
   TextStr             : string;   // Текстовый образец
   ScanCount, FoundCount : integer;
   MackList            : TStringList;
   function Execute : boolean;
   // Сканирование каталогов в соответствии с деревом
   function ScanTreeDirs(ARootNode : TTreeNode): boolean;
   // Сканирование папки
   function ScanDir(ADir: string; AScanSubdir: boolean): boolean;
   // Проверка файла по условиям
   function CheckFileFilter(APath : string; SR : TSearchRec) : boolean;
   // Создание списка масок
   Function CreateMaskList : boolean;
   // Сравнение имени с маской
   function MatchesMask(const Filename: string; AMackList : TStringList): Boolean;
   procedure AddToLogMemo(S : string);
  end;

var
  FileSearchDLG: TFileSearchDLG;

function ExecuteFileSearchDLG : boolean;

implementation
uses zutil, zTranslate;

{$R *.dfm}

function TFileSearchDLG.RefreshDirInfo: boolean;
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

procedure TFileSearchDLG.FormCreate(Sender: TObject);
begin
 MackList  := TStringList.Create;
 RefreshDirInfo;
 TranslateForm(Self);
end;

function ExecuteFileSearchDLG : boolean;
begin
 try
  FileSearchDLG := TFileSearchDLG.Create(nil);
  Result :=  FileSearchDLG.Execute;
 finally
  FileSearchDLG.Free;
  FileSearchDLG := nil;
 end;
end;

function TFileSearchDLG.Execute: boolean;
begin
 MackList.Clear;
 // Активация переключателя проверки Wintrust только если таковая проверка доступна
 cbExcludeWindowsTrusted.Enabled := LoadWintrustDLL;
 if not(cbExcludeWindowsTrusted.Enabled) then
  cbExcludeWindowsTrusted.Checked := false;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TFileSearchDLG.ZShellTreeViewTreeNodeStateIconClick(
  Sender: TObject; Node: TTreeNode);
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

procedure TFileSearchDLG.SetParentStatus(ANode: TTreeNode);
var
 i, CheckedCount, PartCheckedCount, NewStatus : integer;
begin
 // Случайный вызов ?? тогда выход
 if (ANode = nil) or (ANode.Count <= 0) then exit;
 NewStatus := -1;
 CheckedCount := 0; PartCheckedCount := 0;
 for i := 0 to ANode.Count - 1 do begin
  if ANode.Item[i].StateIndex = 2 then
   inc(CheckedCount);
  if ANode.Item[i].StateIndex = 3 then
   inc(PartCheckedCount);
 end;
 // Анализ количества отмеченных узлов
 if PartCheckedCount > 0 then
  ANode.StateIndex := 3 else
   if CheckedCount = ANode.Count then
    ANode.StateIndex := 2 else
     if CheckedCount = 0 then
      ANode.StateIndex := 1
       else ANode.StateIndex := 3;
 // Установка статуса на уровне N-1
 if ANode.Parent <> nil then
  SetParentStatus(ANode.Parent);
end;

procedure TFileSearchDLG.N21Click(Sender: TObject);
begin
 RefreshDirInfo;
end;

procedure TFileSearchDLG.N17Click(Sender: TObject);
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

procedure TFileSearchDLG.mbStartClick(Sender: TObject);
var
 i : integer;
begin
 ScanCount := 0; FoundCount := 0;
 if not(cbAddFiles.Checked) then
  lvFoundFiles.Clear;
 LogMemo.Clear;
 try
  StopF := false;
  // Парсинг маски
  CreateMaskList;
  // Подготовка образца текста для поиска
  TextStr := Trim(AnsiLowerCase(edText.Text));
  edFilter.Enabled := false;
  mbStop.Enabled  := true;
  mbStart.Enabled := false;
  // Формирование параметров фильра
  // Размер
  SizeMin :=  StrToIntDef(feSize.FilterText1, 0);
  SizeMax :=  StrToIntDef(feSize.FilterText2, 0);
  // Дата создания
  DateCMin :=  StrToDateDef(feDateC.FilterText1, 0);
  DateCMax :=  StrToDateDef(feDateC.FilterText2, 0);
  // Дата модификации
  DateMMin :=  StrToDateDef(feDateM.FilterText1, 0);
  DateMMax :=  StrToDateDef(feDateM.FilterText2, 0);
  // Контроль
  if MackList.Count = 0 then begin
   zMessageDlg('$AVZ0472', mtError, [mbOK], 0);
   exit;
  end;
  if (cbSearchText.Checked) and (TextStr = '') then begin
   zMessageDlg('$AVZ0470', mtError, [mbOK], 0);
   exit;
  end;
  if feSize.FilterOn and not(feSize.ValidateValue) then begin
   zMessageDlg('$AVZ0513 '+feSize.ErrorMessage, mtError, [mbOK], 0);
   exit;
  end;
  if feDateC.FilterOn and not(feDateC.ValidateValue) then begin
   zMessageDlg('$AVZ0508 '+feDateC.ErrorMessage, mtError, [mbOK], 0);
   exit;
  end;
  if feDateM.FilterOn and not(feDateM.ValidateValue) then begin
   zMessageDlg('$AVZ0508 '+feDateM.ErrorMessage, mtError, [mbOK], 0);
   exit;
  end;
  // Начало поиска
  AddToLogMemo('$AVZ0777 '+edFilter.Text);
  // Поиск
  for i := 0 to ZShellTreeView.Items.Count - 1 do begin
   if (ZShellTreeView.Items[i].Parent = nil) then
    ScanTreeDirs(ZShellTreeView.Items[i]);
  end;
 finally
  AddToLogMemo('$AVZ0775');
  AddToLogMemo('$AVZ0864 '+IntToStr(ScanCount)+', $AVZ0459 '+IntToStr(FoundCount));
  StatusBar.Panels[0].Text := '';
  edFilter.Enabled := true;
  mbStop.Enabled   := false;
  mbStart.Enabled  := true;
 end;
end;

function TFileSearchDLG.ScanTreeDirs(ARootNode: TTreeNode): boolean;
var
 i : integer;
 res : boolean;
begin
 Result := true;
 // Дальнейшие действия
 case ARootNode.StateIndex of
  0, 1 : ; // Не выделен - никаких действий
  2    : Result := ScanDir(NormalDir(ZShellTreeView.GetPath(ARootNode)),  true);
  3    : begin
          //Result := ScanDir(NormalDir(ZShellTreeView.GetPath(ARootNode)), false);
          for i := 0 to ARootNode.Count - 1 do
           if ARootNode.Item[i].StateIndex > 1 then begin
            res := ScanTreeDirs(ARootNode.Item[i]);
            Result := Result and res;
           end;
         end;
 end;
end;

function TFileSearchDLG.ScanDir(ADir: string; AScanSubdir: boolean): boolean;
var
 SR  : TSearchRec;
 Res : integer;
 Cnt : integer;
begin
 try
  Res := FindFirst(NormalDir(ADir)+'*.*', faAnyFile, SR);
  Application.ProcessMessages;
  StatusBar.Panels[0].Text := (NormalDir(ADir));
  while Res = 0 do begin
   inc(Cnt);
   // Прерывание операции
   if StopF then begin
    AddToLogMemo('$AVZ0580');
    abort;
   end;
   if Cnt mod 32 = 0 then Application.ProcessMessages;
   if ((SR.Attr and faDirectory) = 0) then begin
    inc(ScanCount);
    if CheckFileFilter(ADir, SR) then begin
     AddToLogMemo(ADir + SR.Name);
     with lvFoundFiles.Items.Add do begin
      inc(FoundCount);
      Caption := ADir + SR.Name;
      SubItems.Add(IntToStr(SR.Size));
      SubItems.Add(DateTimeToStr(FileTimeToDateTime(SR.FindData.ftCreationTime)));
      SubItems.Add(DateTimeToStr(FileTimeToDateTime(SR.FindData.ftLastWriteTime)));
     end;
    end;
   end;
   if AScanSubdir then
    if (SR.Attr and faDirectory) > 0 then
     if (SR.Name <> '.') and (SR.Name <> '..') then begin
      ScanDir(NormalDir(ADir + SR.Name), AScanSubdir);
     end;
   Res := FindNext(SR);
  end;
 finally
  FindClose(SR);
 end;
end;

procedure TFileSearchDLG.mbStopClick(Sender: TObject);
begin
 StopF := true;
end;

function TFileSearchDLG.CheckFileFilter(APath: string;
  SR: TSearchRec): boolean;
var
 FDate : TDate;
 FS    : TFileStream;
 S, ST     : string;
 ReadedSize : integer;
 Found : boolean;
begin
 Result := false;
 // Фильтр
 // 1. Соответствие имени файла маске
 if not MatchesMask(SR.Name, MackList) then exit;
 // 2. Проверка размера
 if feSize.FilterOn then begin
  case feSize.TekFilterItemIndex of
   0 : if not(SR.Size = SizeMin) then exit;
   1 : if not(SR.Size <> SizeMin) then exit;
   2 : if not(SR.Size > SizeMin) then exit;
   3 : if not(SR.Size < SizeMin) then exit;
   4 : if not((SR.Size >= SizeMin) and (SR.Size <= SizeMax)) then exit;
   5 : if not((SR.Size <= SizeMin) and (SR.Size >= SizeMax)) then exit;
  end;
 end;
 // 2. Проверка даты создания
 if feDateC.FilterOn then begin
  FDate := Trunc(FileTimeToDateTime(SR.FindData.ftCreationTime));
  case feDateC.TekFilterItemIndex of
   0 : if not(FDate = DateCMin) then exit;
   1 : if not(FDate <> DateCMin) then exit;
   2 : if not(FDate > DateCMin) then exit;
   3 : if not(FDate < DateCMin) then exit;
   4 : if not((FDate >= DateCMin) and (FDate <= DateCMax)) then exit;
   5 : if not((FDate <= DateCMin) and (FDate >= DateCMax)) then exit;
  end;
 end;
 // 3. Проверка даты модификации
 if feDateM.FilterOn then begin
  FDate := Trunc(FileTimeToDateTime(SR.FindData.ftLastWriteTime));
  case feDateM.TekFilterItemIndex of
   0 : if not(FDate = DateMMin) then exit;
   1 : if not(FDate <> DateMMin) then exit;
   2 : if not(FDate > DateMMin) then exit;
   3 : if not(FDate < DateMMin) then exit;
   4 : if not((FDate >= DateMMin) and (FDate <= DateMMax)) then exit;
   5 : if not((FDate <= DateMMin) and (FDate >= DateMMax)) then exit;
  end;
 end;
 // 4. Поиск текста внутри файла
 if cbSearchText.Checked then begin
  try
   FS := TFileStream.Create(APath+SR.Name, fmOpenRead or fmShareDenyNone);
   SetLength(S, 65000);
   Found := false;
   ST := AnsiLowerCase(edText.Text);
   while not(Found) and (FS.Position < FS.Size) do begin
    FS.Seek(-(length(ST)), soFromCurrent);
    ReadedSize := FS.Read(S[1], 65000);
    S := AnsiLowerCase(S);
    if pos(ST, S) > 0 then Found := true;
   end;
   FS.Free;
   if not(Found) then exit;
  except
   exit;
  end;
 end;                                        
 // 5. Проверка на системный файл по моей базе
 if cbExcludeGoodFiles.Checked then
  if FileSignCheck.CheckFile(APath + SR.Name, false) = 0 then exit;
 // 5. Проверка на системный файл по каталогу Windows
 if cbExcludeWindowsTrusted.Checked then
  try
   if CheckFileTrust(APath + SR.Name) then exit;
  except
   on e : exception do
    AddToLog(APath + SR.Name + ' = ' + e.Message, logError);
  end;
 Result := true;
end;

procedure TFileSearchDLG.Timer1Timer(Sender: TObject);
begin
 StatusBar.Panels[1].Text := IntToStr(ScanCount)+'/'+IntToStr(FoundCount);
end;

procedure TFileSearchDLG.BitBtn1Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundFiles.Items.Count - 1 do
  if lvFoundFiles.Items[i].Checked then begin
   CopyToInfected(lvFoundFiles.Items[i].Caption, '$AVZ0223', 'Quarantine');
    AddToLogMemo('$AVZ1109 '+lvFoundFiles.Items[i].Caption+' $AVZ0980');
  end;
end;

procedure TFileSearchDLG.FormDestroy(Sender: TObject);
begin
 MackList.Free;
end;

function TFileSearchDLG.CreateMaskList: boolean;
var
 s, st : string;
begin
 MackList.Clear;
 S := Trim(edFilter.Text);
 S := StringReplace(S, ';', ',', [rfReplaceAll]);
 S := StringReplace(S, ' ', ',', [rfReplaceAll]);
 S := S + ',';
 while pos(',', S) > 0 do begin
  st := LowerCase(Trim(copy(S, 1, pos(',', S)-1)));
  delete(S, 1, pos(',', S));
  if st <> '' then
   MackList.Add(st);
 end;
end;

function TFileSearchDLG.MatchesMask(const Filename: string; AMackList : TStringList): Boolean;
begin
 Result := zUtil.MatchesMask(Filename, AMackList);
end;

procedure TFileSearchDLG.BitBtn2Click(Sender: TObject);
var
 i : integer;
 FileList : TStringList;
 DelMode  : integer;
 FQuarantine : boolean;
begin
 if zMessageDlg('$AVZ0164', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 ExecuteFileDeleteModeDLG(DelMode, FQuarantine);
 if DelMode = 0 then exit;
 FileList := TStringList.Create;
 i := 0;
 while i <= lvFoundFiles.Items.Count - 1 do begin
  if lvFoundFiles.Items[i].Checked then begin
   FileList.Add(lvFoundFiles.Items[i].Caption);
   if FQuarantine then
    CopyToInfected(lvFoundFiles.Items[i].Caption,
                        '$AVZ0376',
                        'Quarantine'); //#DNL)
   if DeleteFile(lvFoundFiles.Items[i].Caption) then begin
    AddToLogMemo('$AVZ1109 '+lvFoundFiles.Items[i].Caption+' $AVZ1104');
    lvFoundFiles.Items.Delete(i);
    Continue;
   end else begin
    // Отложенное удаление
    AVZDeleteFile(lvFoundFiles.Items[i].Caption);
   end;
  end;
   inc(i);
 end;
 if (DelMode = 2) and (FileList.Count > 0) then
  ExtSystemClean(FileList);
 FileList.Free;
end;

function TFileSearchDLG.FormHelp(Command: Word; Data: Integer;
  var CallHelp: Boolean): Boolean;
begin
 Application.HelpJump('t_file_search');
end;

procedure TFileSearchDLG.pmSelectAllClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundFiles.Items.Count - 1 do
  lvFoundFiles.Items[i].Checked := true;
end;

procedure TFileSearchDLG.pmUnSelectAllClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundFiles.Items.Count - 1 do
  lvFoundFiles.Items[i].Checked := false;
end;

procedure TFileSearchDLG.pmInvertSelectClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundFiles.Items.Count - 1 do
  lvFoundFiles.Items[i].Checked := not lvFoundFiles.Items[i].Checked;
end;

procedure TFileSearchDLG.ZShellTreeViewKeyPress(Sender: TObject;
  var Key: Char);
begin
 if Key = ' ' then
  if ZShellTreeView.Selected <> nil then
   ZShellTreeViewTreeNodeStateIconClick(Sender, ZShellTreeView.Selected);
end;

procedure TFileSearchDLG.lvFoundFilesCompare(Sender: TObject; Item1,
  Item2: TListItem; Data: Integer; var Compare: Integer);
var
 S1, S2 : string;
 X1, X2 : Extended;
 StrCompareMode : boolean;
begin
 Compare :=0;
 case OrderField of
  0 : begin
       S1 := Item1.Caption;
       S2 := Item2.Caption;
       StrCompareMode := true;
      end;
  1 : begin
       X1 := StrToIntDef(Item1.SubItems[0], 0);
       X2 := StrToIntDef(Item2.SubItems[0], 0);
       StrCompareMode := false;
      end;
  2 : begin
       X1 := StrToDateTimeDef(Item1.SubItems[1], 0);
       X2 := StrToDateTimeDef(Item2.SubItems[1], 0);
       StrCompareMode := false;
      end;
  3 : begin
       X1 := StrToDateTimeDef(Item1.SubItems[2], 0);
       X2 := StrToDateTimeDef(Item2.SubItems[2], 0);
       StrCompareMode := false;
      end;
 end;
 if StrCompareMode then begin
  if S1 > S2 then Compare := 1;
  if S1 < S2 then Compare := -1;
 end else begin
  if X1 > X2 then Compare := 1;
  if X1 < X2 then Compare := -1;
 end;
end;

procedure TFileSearchDLG.lvFoundFilesColumnClick(Sender: TObject;
  Column: TListColumn);
begin
 OrderField := Column.Index;
 lvFoundFiles.CustomSort(nil, OrderField);
 lvFoundFiles.Refresh;
 lvFoundFiles.Repaint;
end;

procedure TFileSearchDLG.AddToLogMemo(S: string);
begin
 LogMemo.Lines.Add(TranslateStr(S));
end;

function TFileSearchDLG.feDateMFilterEditTranslateEvent(Sender: TObject;
  AStr: String): String;
begin
 Result := TranslateStr(AStr);
end;

end.
