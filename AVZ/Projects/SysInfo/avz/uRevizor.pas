unit uRevizor;
// Ревизор
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ComCtrls, zTreeViews, zRevizor, Mask,
  ToolEdit, ExtCtrls, ImgList, uFileDeleteModeDLG, wintrust, zAVKernel, zAVZKernel;

type
  TRevizorDLG = class(TForm)
    StatusBar: TStatusBar;
    PageControl: TPageControl;
    TabSheet1: TTabSheet;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    Panel1: TPanel;
    feRevizorFileOUT: TFilenameEdit;
    Label1: TLabel;
    ZShellTreeView: TZShellTreeView;
    Timer1: TTimer;
    TabSheet2: TTabSheet;
    Panel2: TPanel;
    Label2: TLabel;
    feRevizorFileIN: TFilenameEdit;
    mbStart1: TBitBtn;
    mbStop1: TBitBtn;
    pcCompareDiskDB: TPageControl;
    TabSheet3: TTabSheet;
    CompareLogMemo: TMemo;
    TabSheet4: TTabSheet;
    lvFoundFiles: TListView;
    mbDeleteChecked: TBitBtn;
    mbQurantineChecked: TBitBtn;
    rgFileTypes: TRadioGroup;
    cbIncludeFiles: TCheckBox;
    edIncludeFiles: TEdit;
    cbExcludeFiles: TCheckBox;
    edExcludeFiles: TEdit;
    TabSheet5: TTabSheet;
    cbExcludeWindowsTrusted: TCheckBox;
    cbExcludeGoodFiles: TCheckBox;
    rgCompareMode: TRadioGroup;
    rgCreateDBMode: TRadioGroup;
    mbStop: TBitBtn;
    mbStart: TBitBtn;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure mbStartClick(Sender: TObject);
    procedure ZShellTreeViewKeyPress(Sender: TObject; var Key: Char);
    procedure ZShellTreeViewTreeNodeStateIconClick(Sender: TObject;
      Node: TTreeNode);
    procedure Timer1Timer(Sender: TObject);
    procedure mbStopClick(Sender: TObject);
    procedure mbStart1Click(Sender: TObject);
    procedure mbQurantineCheckedClick(Sender: TObject);
    procedure mbDeleteCheckedClick(Sender: TObject);
    procedure cbIncludeFilesClick(Sender: TObject);
    procedure cbExcludeFilesClick(Sender: TObject);
    procedure rgFileTypesClick(Sender: TObject);
    procedure mbStop1Click(Sender: TObject);
    procedure PageControlChanging(Sender: TObject;
      var AllowChange: Boolean);
  private
    function RefreshDirInfo: boolean;
    procedure SetParentStatus(ANode: TTreeNode);
    // Событие "сканирование"
    procedure ScanEvent(Sender: TObject; ObjName: string;
      F1: TRevizorFile);
     // Проверка каталога
    function CheckDirectory(AName: string; AState: integer): boolean;
    // Событие "фильтр"
    function FilterEvent(ObjName: string): boolean;
    procedure CompareEvent(Sender: TObject; ObjName: string;
      ACode: integer; F1, F2: TRevizorFile);
    function RefreshInterface: boolean;
  public
   StopF : Boolean;                // Флаг аварийной остановки
   Revizor : TRevizor;
   ScanCount, FoundCount : Integer;
   MackList, ExcludeMackList : TStringList;
   procedure AddToLog(S : string);
   // Создание списка масок
   Function CreateMaskList : boolean;
   function Execute : boolean;
  end;

var
  RevizorDLG: TRevizorDLG;

function ExecuteRevizorDLG : boolean;

implementation
uses zutil, uGUIShared, zTranslate;
{$R *.dfm}

function ExecuteRevizorDLG : boolean;
begin
 try
  RevizorDLG := TRevizorDLG.Create(nil);
  Result :=  RevizorDLG.Execute;
 finally
  RevizorDLG.Free;
  RevizorDLG := nil;
 end;
end;

function TRevizorDLG.Execute: boolean;
begin
 feRevizorFileOUT.FileName := ExtractFilePath(Application.ExeName) + 'Revizor\'+
                              FormatDateTime('yyyy-mm-dd',Now)+'.frz';
 feRevizorFileIN.FileName := ExtractFilePath(Application.ExeName) + 'Revizor\';
 ShowModal;
 Result := ModalResult = mrOk;
end;

function TRevizorDLG.RefreshDirInfo: boolean;
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

function TRevizorDLG.CheckDirectory(AName: string; AState: integer): boolean;
var
 Tmp     : TTreeNode;
begin
 Result := false;
 Tmp := ZShellTreeView.SearchDirNode(AName);
 if Tmp = nil then exit;
 Tmp.StateIndex := AState;
 ZShellTreeView.SetChildNodesState(Tmp, Tmp.StateIndex);
 // Меняем статус родительских элементов
 if Tmp.Parent <> nil then
  SetParentStatus(Tmp.Parent);
 Result := true;
end;

procedure TRevizorDLG.FormCreate(Sender: TObject);
begin
 MackList         := TStringList.Create;
 ExcludeMackList  := TStringList.Create;
 ScanCount := 0; FoundCount := 0;
 ZShellTreeView.StateImages := GUIShared.StateImageList;
 Revizor := TRevizor.Create;
 Revizor.OnScanFile   := ScanEvent;
 Revizor.OnFilterFile := FilterEvent;
 Revizor.OnCmpFile    := CompareEvent;
 RefreshDirInfo;
 CheckDirectory(GetWindowsDirectoryPath, 2);
 TranslateForm(Self);
end;

procedure TRevizorDLG.FormDestroy(Sender: TObject);
begin
 Revizor.Free;
 MackList.Free;
 ExcludeMackList.Free;
end;

procedure TRevizorDLG.mbStartClick(Sender: TObject);
var
 FileName : string;
 MS       : TMemoryStream;
 i        : integer;
 function AddTreeDirs(ARootNode: TTreeNode): boolean;
 var
  i : integer;
  res : boolean;
 begin
  Result := true;
  // Дальнейшие действия
  case ARootNode.StateIndex of
   0, 1 : ; // Не выделен - никаких действий
   2    : Result :=  Revizor.AddRootDir(NormalDir(ZShellTreeView.GetPath(ARootNode)));
   3    : begin
           for i := 0 to ARootNode.Count - 1 do
            if ARootNode.Item[i].StateIndex > 1 then begin
             res := AddTreeDirs(ARootNode.Item[i]);
             Result := Result and res;
            end;
          end;
   end;
 end;
begin
 Revizor.Clear;
 feRevizorFileOUT.FileName := Trim(feRevizorFileOUT.FileName);
 if feRevizorFileOUT.FileName = '' then begin
  zMessageDlg('$AVZ0473', mtError, [mbOk],0);
  exit;
 end;
 try
  StopF := false;
  ZShellTreeView.Enabled := false;
  feRevizorFileOUT.Enabled := false;
  mbStop.Enabled   := true;
  mbStart.Enabled  := false;
  rgFileTypes.Enabled := false;
  ScanCount := 0; FoundCount := 0;
  FileName := Trim(feRevizorFileOUT.Text);
  case rgFileTypes.ItemIndex of
   0 : Revizor.ScanMask := '*.exe *.dll *.ocx *.sys *.cpl *.pif *.inf *.lnk *.bat *.cmd';
   1 : Revizor.ScanMask := '*.*';
   2 : Revizor.ScanMask := '';
  end;
  if cbIncludeFiles.Checked then
   Revizor.ScanMask := Revizor.ScanMask + ' ' + edIncludeFiles.Text;
  // Маска исключения
  if cbExcludeFiles.Checked then
   Revizor.ExcludeMask := edExcludeFiles.Text
  else
   Revizor.ExcludeMask := '';
  CreateMaskList;
   // Поиск
   for i := 0 to ZShellTreeView.Items.Count - 1 do begin
    if (ZShellTreeView.Items[i].Parent = nil) then
     AddTreeDirs(ZShellTreeView.Items[i]);
   end;
  Revizor.Refresh(rgCreateDBMode.ItemIndex = 1);
  MS := TMemoryStream.Create;
  Revizor.SaveToStream(MS);
  ForceDirectories(ExtractFilePath(feRevizorFileOUT.FileName));
  MS.SaveToFile(feRevizorFileOUT.FileName);
  MS.Free;
 finally
 // LogMemo.Lines.Add('Поиск файлов завершен');
 // LogMemo.Lines.Add('Просмотрено '+IntToStr(ScanCount)+', найдено '+IntToStr(FoundCount));
  StatusBar.Panels[0].Text := '';
  ZShellTreeView.Enabled := true;
  mbStop.Enabled   := false;
  mbStart.Enabled  := true;
  feRevizorFileOUT.Enabled := true;
  rgFileTypes.Enabled := true;
 end;
end;

procedure TRevizorDLG.ScanEvent(Sender : TObject; ObjName : string; F1 : TRevizorFile);
begin
 if StopF then Abort;
 inc(ScanCount);
 if ScanCount mod 32 = 0 then begin
  StatusBar.Panels[0].Text := ObjName;
  Application.ProcessMessages;
 end;
end;

function TRevizorDLG.FilterEvent(ObjName : string) : boolean;
begin
 Result := zUtil.MatchesMask(ObjName, MackList);
 if Result and (ExcludeMackList.Count <> 0) then
  Result := not(zUtil.MatchesMask(ObjName, ExcludeMackList));
 if StopF then Abort;
end;

procedure TRevizorDLG.ZShellTreeViewKeyPress(Sender: TObject;
  var Key: Char);
begin
 if Key = ' ' then
  if ZShellTreeView.Selected <> nil then
   ZShellTreeViewTreeNodeStateIconClick(Sender, ZShellTreeView.Selected);
end;

procedure TRevizorDLG.SetParentStatus(ANode: TTreeNode);
var
 i, CheckedCount, PartCheckedCount : integer;
begin
 // Случайный вызов ?? тогда выход
 if (ANode = nil) or (ANode.Count <= 0) then exit;
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

procedure TRevizorDLG.ZShellTreeViewTreeNodeStateIconClick(Sender: TObject;
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

procedure TRevizorDLG.Timer1Timer(Sender: TObject);
begin
 StatusBar.Panels[1].Text := IntToStr(ScanCount)+'/'+IntToStr(FoundCount);
end;

function TRevizorDLG.CreateMaskList: boolean;
var
 s, st : string;
begin
 // Список масок на включение
 MackList.Clear;
 S := Trim(Revizor.ScanMask);
 S := StringReplace(S, ';', ',', [rfReplaceAll]);
 S := StringReplace(S, ' ', ',', [rfReplaceAll]);
 S := S + ',';
 while pos(',', S) > 0 do begin
  st := LowerCase(Trim(copy(S, 1, pos(',', S)-1)));
  delete(S, 1, pos(',', S));
  if st <> '' then
   MackList.Add(st);
 end;
 // Список масок на исключение
 ExcludeMackList.Clear;
 S := Trim(Revizor.ExcludeMask);
 S := StringReplace(S, ';', ',', [rfReplaceAll]);
 S := StringReplace(S, ' ', ',', [rfReplaceAll]);
 S := S + ',';
 while pos(',', S) > 0 do begin
  st := LowerCase(Trim(copy(S, 1, pos(',', S)-1)));
  delete(S, 1, pos(',', S));
  if st <> '' then
   ExcludeMackList.Add(st);
 end;
end;

procedure TRevizorDLG.mbStopClick(Sender: TObject);
begin
 StopF := true;
end;

procedure TRevizorDLG.CompareEvent(Sender : TObject; ObjName : string; ACode : integer; F1, F2 : TRevizorFile);
var
 S, FullObjName : string;
 Tmp : TRevizorFile;
begin
 S := '?';
 FullObjName := NormalDir((Sender as TRevizorDir).FullDirName) + ObjName;
 inc(FoundCount);
 case ACode of
  1 : S := '$AVZ1125';
  2 : S := '$AVZ1124';
  3 : begin
        S := '$AVZ1115';
        if F1.Size <> F2.Size then S := S + ', $AVZ0893 '+IntToStr(F2.Size)+' -> '+IntToStr(F1.Size);
        if F1.CRC  <> F2.CRC  then S := S + ', $AVZ0296 ';
      end;
  4 : S := '$AVZ0330';
  5 : S := '$AVZ0329';
 end;
 if ACode in [2,3] then begin
  // Проверка на системный файл по моей базе
  if cbExcludeGoodFiles.Checked then
   if FileSignCheck.CheckFile(FullObjName, false) = 0 then exit;
  // Проверка на системный файл по каталогу Windows
  if cbExcludeWindowsTrusted.Checked then
   try
    if CheckFileTrust(FullObjName) then exit;
   except
   end;
 end;
 AddToLog((Sender as TRevizorDir).FullDirName +  ObjName + ' = '+TranslateStr(S));
 with lvFoundFiles.Items.Add do begin
  Caption := (Sender as TRevizorDir).FullDirName +  ObjName;
  Tmp := F1;
  if F1 = nil then Tmp := F2;
  if Tmp <> nil then
   SubItems.Add(IntToStr(Tmp.Size)) else SubItems.Add('-');
  SubItems.Add(TranslateStr(S));
 end;
end;

procedure TRevizorDLG.mbStart1Click(Sender: TObject);
var
 MS : TMemoryStream;
begin
 StopF := false;
 Revizor.Clear;
 feRevizorFileIN.FileName := Trim(feRevizorFileIN.FileName);
 if feRevizorFileIN.FileName = '' then begin
  zMessageDlg('$AVZ0473', mtError, [mbOk],0);
  exit;
 end;
 if not(FileExists(feRevizorFileIN.FileName)) then begin
  zMessageDlg('$AVZ0478 '+feRevizorFileIN.FileName, mtError, [mbOk],0);
  exit;
 end;
 ScanCount := 0;
 try
  mbStart1.Enabled := false;
  mbStop1.Enabled  := true;
  lvFoundFiles.Clear;
  CompareLogMemo.Clear;
  AddToLog('$AVZ0288 ');
  MS := TMemoryStream.Create;
  MS.LoadFromFile(feRevizorFileIN.FileName);
  pcCompareDiskDB.ActivePageIndex := 1;
  if not(Revizor.LoadFromStream(MS)) then begin
   zMessageDlg('$AVZ0656 '+feRevizorFileIN.FileName, mtError, [mbOk],0);
   exit;
  end;
  AddToLog('$AVZ0072 '+DateTimeToStr(Revizor.RevizorHDR.DateTime));
  MS.Free;
  CreateMaskList;
  if (Revizor.RevizorHDR.Mode and 1) > 0 then begin
   AddToLog('$AVZ0249');
   rgCompareMode.ItemIndex := 1;
  end;
  if rgCompareMode.ItemIndex = 1 then begin
   AddToLog('$AVZ0121');
   Revizor.Compare(true);
  end else begin
   AddToLog('$AVZ1043');
   Revizor.Compare(false);
  end;
 finally
  if StopF then
   AddToLog('$AVZ1045')
  else
   AddToLog('$AVZ1046');
  StatusBar.Panels[0].Text := '';
  mbStart1.Enabled := true;
  mbStop1.Enabled  := false;
 end;
end;

procedure TRevizorDLG.mbQurantineCheckedClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundFiles.Items.Count - 1 do
  if lvFoundFiles.Items[i].Checked then begin
   CopyToInfected(lvFoundFiles.Items[i].Caption, TranslateStr('$AVZ0222'), 'Quarantine');
    AddToLog('$AVZ1109 '+lvFoundFiles.Items[i].Caption+' $AVZ0980');
  end;
end;

procedure TRevizorDLG.mbDeleteCheckedClick(Sender: TObject);
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
    AddToLog('$AVZ1109 '+lvFoundFiles.Items[i].Caption+' $AVZ1104');
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

function TRevizorDLG.RefreshInterface: boolean;
begin
 // В режиме 1 (все файлы) включение по маске теряет свой смысл
 if rgFileTypes.ItemIndex = 1 then begin
  cbIncludeFiles.Checked := false; cbIncludeFiles.Enabled := false;
 end else begin
  cbIncludeFiles.Enabled := true;
 end;
 // Фильтры
 edIncludeFiles.Enabled := cbIncludeFiles.Checked;
 edExcludeFiles.Enabled := cbExcludeFiles.Checked;
end;

procedure TRevizorDLG.cbIncludeFilesClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TRevizorDLG.cbExcludeFilesClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TRevizorDLG.rgFileTypesClick(Sender: TObject);
begin
 RefreshInterface;
end;

procedure TRevizorDLG.mbStop1Click(Sender: TObject);
begin
 StopF := true;
end;

procedure TRevizorDLG.AddToLog(S: string);
begin
 CompareLogMemo.Lines.Add(TranslateStr(S));
end;

procedure TRevizorDLG.PageControlChanging(Sender: TObject;
  var AllowChange: Boolean);
begin
 AllowChange := mbStart.Enabled and mbStart1.Enabled;
end;

end.
