unit uAutoruns;
// **************** Менеджер автозапуска *******************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Grids, RXGrids, zAutoruns, ComCtrls, ExtCtrls, RXSplit,
  StdCtrls, Buttons, Menus, zSharedFunctions, zHTMLTemplate, zAVKernel,
  zAVZKernel, ImgList;

type
  TAutoruns = class(TForm)
    Panel1: TPanel;
    Panel2: TPanel;
    dgAutoruns: TRxDrawGrid;
    tvNavigator: TTreeView;
    RxSplitter1: TRxSplitter;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    mbRepairItem: TBitBtn;
    sbSaveProcLog: TSpeedButton;
    pmAutorunPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    sbRefreshPr: TSpeedButton;
    N1: TMenuItem;
    Regedit1: TMenuItem;
    procedure dgAutorunsDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure FormCreate(Sender: TObject);
    procedure tvNavigatorChange(Sender: TObject; Node: TTreeNode);
    procedure dgAutorunsClick(Sender: TObject);
    procedure dgAutorunsMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
    procedure mbRepairItemClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbRefreshPrClick(Sender: TObject);
    procedure dgAutorunsKeyPress(Sender: TObject; var Key: Char);
    procedure pmiGoogleClick(Sender: TObject);
    procedure pmiYandexClick(Sender: TObject);
    procedure pmiRamblerClick(Sender: TObject);
    procedure Regedit1Click(Sender: TObject);
  public
   AutorunManager  : TAutorunManager;
   AutorunItems    : TAutorunItems;
   // Обновление списка
   function RefreshItems(NoFilter : boolean = false) : boolean;
   // Обновление отфильтрованного списка
   function RefreshFilteredList(ID : integer) : boolean;
   // Выполнение диалога
   function Execute : boolean;
   // Создать дерево для навигатора
   function CreateNavigatorTree : boolean;
   // Создать HTML отчет
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создать XML отчет
   function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
 end;
var
  Autoruns: TAutoruns;

// СОздание окна и его выполнение в диалоговом режиме
function ExecuteAutorunsDLG : boolean;

implementation

uses zutil, uGUIShared, zLogSystem, zTranslate;

{$R *.dfm}

function TAutoruns.CreateNavigatorTree: boolean;
var
 Tmp, FirstNode : TTreeNode;
begin
 // Подготавливаем навигатор к заполнению
 tvNavigator.Items.Clear;
 tvNavigator.Items.BeginUpdate;
 // Группа "Автозапуск"
 Tmp := tvNavigator.Items.AddChildObject(nil, TranslateStr('$AVZ0038'), Pointer(010000));
 FirstNode := Tmp;
 tvNavigator.Items.AddChildObject(Tmp, TranslateStr('$AVZ0692'), Pointer(010100));
 Tmp.Expand(true);
 Tmp := tvNavigator.Items.AddChildObject(Tmp, TranslateStr('$AVZ0909'), Pointer(010200));
 tvNavigator.Items.AddChildObject(Tmp, 'Run*', Pointer(010201));
 tvNavigator.Items.AddChildObject(Tmp, TranslateStr('$AVZ0958'), Pointer(010202));
 tvNavigator.Items.AddChildObject(Tmp, 'ShellServiceObjectDelayLoad', Pointer(010203));
 tvNavigator.Items.AddChildObject(Tmp, 'TerminalServer', Pointer(010204));
 tvNavigator.Items.AddChildObject(Tmp, 'SharedTaskScheduler', Pointer(010205));
 tvNavigator.Items.AddChildObject(Tmp, 'ShellExecuteHooks', Pointer(010206));
 tvNavigator.Items.AddChildObject(Tmp, 'AppInit_DLLs', Pointer(010207));
 tvNavigator.Items.AddChildObject(Tmp, 'Windows Scripting Host', Pointer(010208));
 tvNavigator.Items.AddChildObject(Tmp, TranslateStr('$AVZ0017'), Pointer(010209));
 Tmp.Expand(true);
 // Группа "WinLogon"
 Tmp := tvNavigator.Items.AddChildObject(nil, 'WinLogon', Pointer(020000));
 tvNavigator.Items.AddChildObject(Tmp, 'WinLogon', Pointer(020100));
 tvNavigator.Items.AddChildObject(Tmp, 'GPExtensions', Pointer(020200));
 tvNavigator.Items.AddChildObject(nil, 'MPR Services', Pointer(030000));
 tvNavigator.Items.AddChildObject(nil, 'Utility Manager', Pointer(040000));
 tvNavigator.Items.AddChildObject(nil, 'Security Providers', Pointer(050000));


 tvNavigator.Items.EndUpdate;
 // Подготовка завершена, выделяем первый элемент
 tvNavigator.Selected := FirstNode;
end;

procedure TAutoruns.dgAutorunsDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TAutorunItem;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0407';
   2 : S := '$AVZ0581';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(AutorunItems) then begin
   Tmp := AutorunItems[ARow - 1];
  // Цветовое выделение
  if not((gdFocused in State) or (gdSelected in State)) then begin
    if Tmp.CheckResult = 0 then
     (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
   end else begin
    if Tmp.CheckResult = 0 then
     (Sender as TRxDrawGrid).Canvas.Brush.Color := clGreen;
  end;

   case ACol of
    0 : begin
         S := Tmp.BinFile;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        case AutorunManager.GetItemEnabledStatus(Tmp) of
         -1 : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        S := '';
        end;
    1 : begin
         case Tmp.AutorunType of
          atRegKeyMulty,
          atRegKey  : S := '$AVZ0338';
          atLnkFile : S := '$AVZ1158';
          atFile    : S := '$AVZ1114';
          atINIFile : S := '$AVZ1109 '+ExtractFileName(Tmp.X1);
         end;
         S := TranslateStr(S);
        end;
    2 : begin
         S := Tmp.X1 + ', ' + Tmp.X2 + ', ' + Tmp.X3;
        end;
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

function ExecuteAutorunsDLG : boolean;
begin
 Autoruns := nil;
 try
  Autoruns := TAutoruns.Create(nil);
  Result   := Autoruns.Execute;
 finally
  Autoruns.Free;
  Autoruns := nil;
 end;
end;

function TAutoruns.Execute: boolean;
begin
 RefreshItems;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TAutoruns.FormCreate(Sender: TObject);
begin
 AutorunManager  := TAutorunManager.Create;
 CreateNavigatorTree;
 TranslateForm(Self);
end;

function TAutoruns.RefreshItems(NoFilter : boolean = false): boolean;
var
 i : integer;
 s : string;
begin
 AutorunItems := nil;
 Screen.Cursor := crHourGlass;
 AutorunManager.Refresh;
 ZProgressClass.AddProgressMax(AutorunManager.RecCount);
 // Проверка по базе безопасных
 for i := 0 to AutorunManager.RecCount - 1 do begin
  ZProgressClass.ProgressStep;
  S := NormalProgramFileName(AutorunManager.AutorunItems[i].BinFile, '.exe');
  AutorunManager.AutorunItems[i].CheckResult := FileSignCheck.CheckFile(S);
  if AutorunManager.AutorunItems[i].CheckResult <> 0 then
   if AutorunManager.AutorunItems[i].CfgParams.DefVal <> '' then
    if Trim(AnsiLowerCase(AutorunManager.AutorunItems[i].CfgParams.DefVal)) = Trim(AnsiLowerCase(AutorunManager.AutorunItems[i].BinFile)) then
     AutorunManager.AutorunItems[i].CheckResult := 0;
 end;
 if NoFilter then
  RefreshFilteredList(0) else
   tvNavigatorChange(nil, nil);
 Screen.Cursor := crDefault;
end;

function TAutoruns.RefreshFilteredList(ID : integer) : boolean;
var
 i : integer;
 Flt1, Flt2, Flt3 : integer;
 Group1, Group2, Group3 : integer;
 procedure AddItem(Item : TAutorunItem);
 begin
  SetLength(AutorunItems, Length(AutorunItems)+1);
  AutorunItems[Length(AutorunItems)-1] := Item;
 end;
 procedure SortItems;
 var
  Tmp : TAutorunItem;
  i, SwapCnt   : integer;
 begin
  if length(AutorunItems) < 2 then exit;
  repeat
   SwapCnt := 0;
   for i := 1 to length(AutorunItems)-1 do
    if AutorunItems[i-1].BinFile > AutorunItems[i].BinFile then begin
     Tmp := AutorunItems[i-1];
     AutorunItems[i-1] := AutorunItems[i];
     AutorunItems[i] := Tmp;
     inc(SwapCnt);
    end;
   until SwapCnt = 0;
 end;
begin
 AutorunItems := nil;
 // Выделение кодов фильтрации (xx xx xx)
 Flt3 := ID mod 100;
 Flt2 := (ID div 100) mod 100;
 Flt1 := (ID div 10000) mod 100;
 // Фильтрация
 for i := 0 to AutorunManager.RecCount - 1 do begin
  // Выделение кодов текущей записи
  Group3 := AutorunManager.AutorunItems[i].GroupCode mod 100;
  Group2 := (AutorunManager.AutorunItems[i].GroupCode div 100) mod 100;
  Group1 := (AutorunManager.AutorunItems[i].GroupCode div 10000) mod 100;
  // Сравнение и добавление
  if ((Flt1 = 0) or ((Flt1 >= 0) and (Flt1 = Group1))) and
     ((Flt2 = 0) or ((Flt2 >= 0) and (Flt2 = Group2))) and
     ((Flt3 = 0) or ((Flt3 >= 0) and (Flt3 = Group3))) then
      AddItem(AutorunManager.AutorunItems[i]);
 end;
 // Сортировка
 SortItems;
 // Настройка таблицы
 if Length(AutorunItems) = 0 then
  dgAutoruns.RowCount := 2 else dgAutoruns.RowCount := Length(AutorunItems) + 1;
 // Перерисовка
 dgAutoruns.Repaint;
 dgAutorunsClick(nil);
end;

procedure TAutoruns.tvNavigatorChange(Sender: TObject; Node: TTreeNode);
var
 item_code : integer;
begin
 // Выход, если не выбран элемент
 if tvNavigator.Selected = nil then exit;
 item_code := integer(tvNavigator.Selected.Data);
 RefreshFilteredList(item_code);
end;

procedure TAutoruns.dgAutorunsClick(Sender: TObject);
var
 Tmp : TAutorunItem;
begin
 if dgAutoruns.Row - 1 < Length(AutorunItems) then begin
  Tmp := AutorunItems[dgAutoruns.Row - 1];
  mbRepairItem.Enabled  := Tmp.CfgParams.RestoreMode > 0;
  mbDeleteItem.Enabled  := Tmp.CfgParams.DeleteMode  > 0;
  mbDisableItem.Enabled := AutorunManager.GetItemEnabledStatus(Tmp)=1;
  mbEnableItem.Enabled  := AutorunManager.GetItemEnabledStatus(Tmp)=0;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
 end;
end;

procedure TAutoruns.dgAutorunsMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) and (dgAutoruns.Col = 0) and (X in [2..11]) then
  case AutorunManager.GetItemEnabledStatus(AutorunItems[dgAutoruns.Row-1]) of
   -1 : ;
    0 : if AutorunManager.OnItemEnable(AutorunItems[dgAutoruns.Row-1]) then RefreshItems;
    1 : if AutorunManager.OnItemDisable(AutorunItems[dgAutoruns.Row-1]) then RefreshItems;
  end;
 dgAutoruns.Invalidate;
end;

procedure TAutoruns.mbEnableItemClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  if AutorunManager.OnItemEnable(AutorunItems[dgAutoruns.Row-1]) then RefreshItems;
 dgAutoruns.Invalidate;
 dgAutorunsClick(nil);
end;

procedure TAutoruns.mbDisableItemClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  if AutorunManager.OnItemDisable(AutorunItems[dgAutoruns.Row-1]) then RefreshItems;
 dgAutoruns.Invalidate;
 dgAutorunsClick(nil);
end;

procedure TAutoruns.mbDeleteItemClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  if AutorunManager.OnItemDelete(AutorunItems[dgAutoruns.Row-1]) then begin
   RefreshItems;
   tvNavigatorChange(nil, nil);
  end;
 dgAutoruns.Invalidate;
end;

procedure TAutoruns.mbRepairItemClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  if AutorunManager.OnItemRepair(AutorunItems[dgAutoruns.Row-1]) then begin
   RefreshItems;
   tvNavigatorChange(nil, nil);
  end;
 dgAutoruns.Invalidate;
end;

procedure TAutoruns.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0038</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_autorun.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TAutoruns.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, ST, ST1, FilePrim, FileInfoStr : string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ1051'+HTML_TableHeaderTD+'$AVZ0407'+HTML_TableHeaderTD+'$AVZ0581');
 GoodFiles := 0;
 for i := 0 to Length(AutorunItems) - 1 do begin
  if AutorunItems[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (AutorunItems[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if AutorunItems[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   case AutorunItems[i].AutorunType of
    atRegKey, atRegKeyMulty  : ST := '$AVZ0338';
    atLnkFile : ST := '$AVZ1158';
    atFile    : ST := '$AVZ1114';
    atINIFile : ST := '$AVZ1109 '+ExtractFileName(AutorunItems[i].X1);
    else ST := '?';
   end;
   ST1 := '?';
   case AutorunManager.GetItemEnabledStatus(AutorunItems[i]) of
    -1 : ST1 := '--';
     0 : ST1 := '$AVZ0604';
     1 : ST1 := '$AVZ0052';
   end;
   FilePrim := '';
   if not(HideGoodFiles and (AutorunItems[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(AutorunItems[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+AutorunItems[i].BinFile+'</a>'+GenScriptTxt(AutorunItems[i].BinFile)+
               '<TD>'+ST1+
               '<TD>'+ST+
               '<TD>'+AutorunItems[i].X1+', '+AutorunItems[i].X2+', '+AutorunItems[i].X3 + FilePrim);
   end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0542 - '+IntToStr(Length(AutorunItems))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TAutoruns.N3Click(Sender: TObject);
begin
 sbCopyToQurantine.Click;
end;

procedure TAutoruns.sbCopyToQurantineClick(Sender: TObject);
var
 FileName : string;
begin
 if not((dgAutoruns.Row - 1 < length(AutorunItems))) then exit;
 if zMessageDlg('$AVZ0359 '+AutorunItems[dgAutoruns.Row-1].BinFile+' $AVZ0090 ?',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 FileName := ExpandFileNameEx(AutorunItems[dgAutoruns.Row-1].BinFile);
 CopyToInfected(FileName, '$AVZ0984 ', 'Quarantine');
end;

function TAutoruns.AddAllToQurantine: boolean;
var
 i : integer;
 FileName : string;
begin
 for i := 0 to length(AutorunItems) - 1 do begin
  FileName := ExpandFileNameEx(NormalProgramFileName(AutorunItems[i].BinFile, '.exe'));
  if FileExists(FileName) and (AutorunItems[i].CheckResult <> 0) then
   CopyToInfected(FileName, '$AVZ0969 ', 'Quarantine');
 end;
end;

procedure TAutoruns.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TAutoruns.sbRefreshPrClick(Sender: TObject);
begin
 RefreshItems;
 Refresh;
 Repaint;
end;

procedure TAutoruns.dgAutorunsKeyPress(Sender: TObject; var Key: Char);
begin
 if Key = ' ' then begin
  if (dgAutoruns.Row - 1 < length(AutorunItems)) then
   case AutorunManager.GetItemEnabledStatus(AutorunItems[dgAutoruns.Row-1]) of
    -1 : ;
     0 : if AutorunManager.OnItemEnable(AutorunItems[dgAutoruns.Row-1]) then RefreshItems;
     1 : if AutorunManager.OnItemDisable(AutorunItems[dgAutoruns.Row-1]) then RefreshItems;
   end;
  dgAutoruns.Invalidate;
 end;
end;

procedure TAutoruns.pmiGoogleClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  WebSearchInGoogle(AutorunItems[dgAutoruns.Row-1].BinFile);
end;

procedure TAutoruns.pmiYandexClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  WebSearchInYandex(AutorunItems[dgAutoruns.Row-1].BinFile);
end;

procedure TAutoruns.pmiRamblerClick(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  WebSearchInRambler(AutorunItems[dgAutoruns.Row-1].BinFile);
end;

procedure TAutoruns.Regedit1Click(Sender: TObject);
begin
 if (dgAutoruns.Row - 1 < length(AutorunItems)) then
  if AutorunItems[dgAutoruns.Row-1].AutorunType in [atRegKey, atRegKeyMulty] then
   RegEditCtl(true, AutorunItems[dgAutoruns.Row-1].X1 +'\'+ AutorunItems[dgAutoruns.Row-1].X2, AutorunItems[dgAutoruns.Row-1].X3);
end;

function TAutoruns.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 ST, FilePrim, FileInfoStr : string;
begin
 Result := false;
 ALines.Add(' <AUTORUN>');
 for i := 0 to Length(AutorunItems) - 1 do begin
  if not(HideGoodFiles) or (AutorunItems[i].CheckResult <> 0) then begin
   case AutorunItems[i].AutorunType of
    atRegKey, atRegKeyMulty  : ST := 'REG';
    atLnkFile : ST := 'LNK';
    atFile    : ST := 'FILE';
    atINIFile : ST := 'INI';
    else ST := '?';
   end;
   FilePrim := '';
   if not(HideGoodFiles and (AutorunItems[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(AutorunItems[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(AutorunItems[i].BinFile)+'" '+
                       'CheckResult="'+IntToStr(AutorunItems[i].CheckResult)+'" '+
                       'Enabled="'+IntToStr(AutorunManager.GetItemEnabledStatus(AutorunItems[i]))+'" '+
                       'Type="'+ST+'" '+
                       FileInfoStr+ ' '+
                       'X1="'+XMLStr(AutorunItems[i].X1)+'" '+
                       'X2="'+XMLStr(AutorunItems[i].X2)+'" '+
                       'X3="'+XMLStr(AutorunItems[i].X3)+'" '+
                       '/>');
   end;
  end;
 end;
 ALines.Add(' </AUTORUN>');
end;

end.
