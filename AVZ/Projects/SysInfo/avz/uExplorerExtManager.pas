unit uExplorerExtManager;
// ***************** Менеджер расширений IE ******************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, zExplorerExt, zHTMLTemplate, Grids, RXGrids, StdCtrls, Buttons, ExtCtrls,
  Menus, zSharedFunctions, zAVZKernel;

type
  TExplorerExtManager = class(TForm)
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    dgExplorerExt: TRxDrawGrid;
    pmProcessPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    SpeedButton5: TSpeedButton;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgExplorerExtDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgExplorerExtMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure dgExplorerExtClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure pmiRamblerClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
  public
   ExplorerExtList : TExplorerExtList;
   // Обновление списка расширений
   Function RefreshExplorerExtList : boolean;
   // Выполнение диалога
   Function Execute : boolean;
   // Создание HTML отчета
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создание XML отчета
   function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  ExplorerExtManager: TExplorerExtManager;

 function ExecuteExplorerExtView : boolean;
implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

function ExecuteExplorerExtView : boolean;
begin
 ExplorerExtManager := nil;
 try
  ExplorerExtManager := TExplorerExtManager.Create(nil);
  Result     := ExplorerExtManager.Execute;
 finally
  ExplorerExtManager.Free;
  ExplorerExtManager := nil;
 end;
end;

function TExplorerExtManager.Execute: boolean;
begin
 RefreshExplorerExtList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TExplorerExtManager.FormCreate(Sender: TObject);
begin
 ExplorerExtList := TExplorerExtList.Create;
 TranslateForm(Self);
end;

procedure TExplorerExtManager.FormDestroy(Sender: TObject);
begin
 ExplorerExtList.Free;
end;

function TExplorerExtManager.RefreshExplorerExtList: boolean;
begin
 ExplorerExtList.RefresList;
 if length(ExplorerExtList.ExplorerExtInfoArray) = 0 then
  dgExplorerExt.RowCount := 2
   else dgExplorerExt.RowCount := length(ExplorerExtList.ExplorerExtInfoArray) + 1;
 dgExplorerExt.OnClick(nil);
end;

procedure TExplorerExtManager.dgExplorerExtDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TExplorerExtInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0451';
   2 : S := '$AVZ0581';
   3 : S := '$AVZ0852';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(ExplorerExtList.ExplorerExtInfoArray) then begin
   Tmp := ExplorerExtList.ExplorerExtInfoArray[ARow - 1];
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
         S := Trim(Tmp.BinFile);
         if S = '' then
          S := TranslateStr('$AVZ1117, ')+Tmp.CLSID;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        case ExplorerExtList.GetItemEnabledStatus(Tmp) of
         -1 : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        S := '';
        end;
    1 : begin
         S := Tmp.ExtName;
        end;
    2 : begin
         S := Tmp.Descr;
        end;
    3 : begin
         S := Tmp.LegalCopyright;
        end;
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

procedure TExplorerExtManager.dgExplorerExtMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
 if (dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray)) and (dgExplorerExt.Col = 0) and (X in [2..11]) then
  case ExplorerExtList.GetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row-1]) of
   -1 : ;
    0 : if ExplorerExtList.SetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row-1], true) then Refresh;
    1 : if ExplorerExtList.SetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row-1], false) then Refresh;
  end;
 dgExplorerExt.Invalidate;
end;

procedure TExplorerExtManager.dgExplorerExtClick(Sender: TObject);
var
 Tmp : TExplorerExtInfo;
begin
 if dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray) then begin
  Tmp :=  ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1];
  mbDisableItem.Enabled := ExplorerExtList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := ExplorerExtList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TExplorerExtManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0434</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_ie_ext.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TExplorerExtManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, ST, FileInfoStr: string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ0451'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852'+HTML_TableHeaderTD+'CLSID');
 GoodFiles := 0;
 for i := 0 to length(ExplorerExtList.ExplorerExtInfoArray) - 1 do begin
  if ExplorerExtList.ExplorerExtInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (ExplorerExtList.ExplorerExtInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if ExplorerExtList.ExplorerExtInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   ST := ExplorerExtList.ExplorerExtInfoArray[i].ExtName;
   if ST = '' then ST := '$AVZ0452';
   if not(HideGoodFiles and (ExplorerExtList.ExplorerExtInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(ExplorerExtList.ExplorerExtInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+ExplorerExtList.ExplorerExtInfoArray[i].BinFile+'</a>'+GenScriptTxt(ExplorerExtList.ExplorerExtInfoArray[i].BinFile)+
               '<TD>'+ST+
               '<TD>'+ExplorerExtList.ExplorerExtInfoArray[i].Descr+
               '<TD>'+ExplorerExtList.ExplorerExtInfoArray[i].LegalCopyright+
               '<TD>'+ExplorerExtList.ExplorerExtInfoArray[i].CLSID
               );
    end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(ExplorerExtList.ExplorerExtInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TExplorerExtManager.N3Click(Sender: TObject);
begin
 sbCopyToQurantine.Click;
end;

function TExplorerExtManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(ExplorerExtList.ExplorerExtInfoArray) - 1 do
  if ExplorerExtList.ExplorerExtInfoArray[i].CheckResult <> 0 then
   CopyToInfected(ExplorerExtList.ExplorerExtInfoArray[i].BinFile, '$AVZ0972', 'Quarantine');
end;

procedure TExplorerExtManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TExplorerExtManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray) then begin
  if ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1].BinFile, '$AVZ0972', 'Quarantine');
 end;
end;

procedure TExplorerExtManager.mbEnableItemClick(Sender: TObject);
begin
 if dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray) then begin
  if ExplorerExtList.GetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1]) <> 0 then exit;
  if ExplorerExtList.SetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1], true) then begin
   dgExplorerExt.Repaint;
   dgExplorerExt.OnClick(nil);
  end;
 end;
end;

procedure TExplorerExtManager.mbDisableItemClick(Sender: TObject);
begin
 if dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray) then begin
  if ExplorerExtList.GetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1]) <> 1 then exit;
  if ExplorerExtList.SetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1], false) then begin
   dgExplorerExt.OnClick(nil);
   dgExplorerExt.Repaint;
  end;
 end;
end;

procedure TExplorerExtManager.pmiRamblerClick(Sender: TObject);
begin
 if dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray) then
  case (Sender as TMenuItem).Tag of
   1 : WebSearchInGoogle(ExtractFileName(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row-1].BinFile));
   2 : WebSearchInYandex(ExtractFileName(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row-1].BinFile));
   3 : WebSearchInRambler(ExtractFileName(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row-1].BinFile));
  end;
end;

procedure TExplorerExtManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgExplorerExt.Row - 1 < length(ExplorerExtList.ExplorerExtInfoArray) then begin
  if not(ExplorerExtList.DeleteItem(ExplorerExtList.ExplorerExtInfoArray[dgExplorerExt.Row - 1])) then exit;
  RefreshExplorerExtList;
 end;
end;

function TExplorerExtManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 FileInfoStr: string;
begin
 Result := false;
 ALines.Add(' <ExplorerExt>');
 for i := 0 to length(ExplorerExtList.ExplorerExtInfoArray) - 1 do begin
  if not(HideGoodFiles) or (ExplorerExtList.ExplorerExtInfoArray[i].CheckResult <> 0) then begin
   if not(HideGoodFiles and (ExplorerExtList.ExplorerExtInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(ExplorerExtList.ExplorerExtInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(ExplorerExtList.ExplorerExtInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(ExplorerExtList.ExplorerExtInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(ExplorerExtList.GetItemEnabledStatus(ExplorerExtList.ExplorerExtInfoArray[i]))+'" '+
                        'ExtName="'+XMLStr(ExplorerExtList.ExplorerExtInfoArray[i].ExtName)+'" '+
                        'RegKey="'+XMLStr(ExplorerExtList.ExplorerExtInfoArray[i].RegKey)+'" '+
                        'CLSID="'+XMLStr(ExplorerExtList.ExplorerExtInfoArray[i].CLSID)+'" '+
                        'Descr="'+XMLStr(ExplorerExtList.ExplorerExtInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(ExplorerExtList.ExplorerExtInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
    end;
  end;
 end;
 ALines.Add(' </ExplorerExt>');
end;

end.
