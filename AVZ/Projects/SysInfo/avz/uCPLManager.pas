unit uCPLManager;
// ********************* Менеджер CPL ***********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Menus, Grids, RXGrids, StdCtrls, Buttons, ExtCtrls, zCPLExt, zHTMLTemplate, zSharedFunctions,
  zAVZKernel, zLogSystem;

type
  TCPLManager = class(TForm)
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    dgCPLExt: TRxDrawGrid;
    pmProcessPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgCPLExtClick(Sender: TObject);
    procedure dgCPLExtDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgCPLExtMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
  public
   CPLList : TCPLList;
   // Обновить список расширений
   Function RefreshCPLList : boolean;
   // Выполнить диалог
   Function Execute : boolean;
   // Создать HTML отчет
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создать XML отчет
   function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  CPLManager: TCPLManager;

function ExecuteCPLManager : boolean;

implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

function ExecuteCPLManager : boolean;
begin
 CPLManager := nil;
 try
  CPLManager := TCPLManager.Create(nil);
  Result     := CPLManager.Execute;
 finally
  CPLManager.Free;
  CPLManager := nil;
 end;
end;

{ TCPLManager }

function TCPLManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(CPLList.CPLInfoArray) - 1 do
  if CPLList.CPLInfoArray[i].CheckResult <> 0 then
   CopyToInfected(CPLList.CPLInfoArray[i].BinFile, '$AVZ0966', 'Quarantine');
end;

function TCPLManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, FileInfoStr : string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852');
 GoodFiles := 0;
 for i := 0 to length(CPLList.CPLInfoArray) - 1 do begin
  if CPLList.CPLInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (CPLList.CPLInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if CPLList.CPLInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   if not(HideGoodFiles and (CPLList.CPLInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(CPLList.CPLInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+CPLList.CPLInfoArray[i].BinFile+'</a>'+GenScriptTxt(CPLList.CPLInfoArray[i].BinFile)+
               '<TD>'+CPLList.CPLInfoArray[i].Descr+
               '<TD>'+CPLList.CPLInfoArray[i].LegalCopyright
               );
   end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(CPLList.CPLInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

function TCPLManager.Execute: boolean;
begin
 RefreshCPLList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

function TCPLManager.RefreshCPLList: boolean;
begin
 CPLList.RefresCPLList;
 if length(CPLList.CPLInfoArray) = 0 then
  dgCPLExt.RowCount := 2
   else dgCPLExt.RowCount := length(CPLList.CPLInfoArray) + 1;
 dgCPLExt.OnClick(nil);
end;

procedure TCPLManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0066</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_cpl_ext.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

procedure TCPLManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgCPLExt.Row - 1 < length(CPLList.CPLInfoArray) then begin
  if CPLList.CPLInfoArray[dgCPLExt.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+CPLList.CPLInfoArray[dgCPLExt.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(CPLList.CPLInfoArray[dgCPLExt.Row - 1].BinFile, '$AVZ0966', 'Quarantine');
 end;
end;

procedure TCPLManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TCPLManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgCPLExt.Row - 1 < length(CPLList.CPLInfoArray) then begin
  if not(CPLList.DeleteItem(CPLList.CPLInfoArray[dgCPLExt.Row - 1])) then exit;
  RefreshCPLList;
 end;
end;

procedure TCPLManager.mbEnableItemClick(Sender: TObject);
begin
 if dgCPLExt.Row - 1 < length(CPLList.CPLInfoArray) then begin
  if CPLList.GetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row - 1]) <> 0 then exit;
  if CPLList.SetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row - 1], true) then begin
   dgCPLExt.Repaint;
   dgCPLExt.OnClick(nil);
  end;
 end;
end;

procedure TCPLManager.mbDisableItemClick(Sender: TObject);
begin
 if dgCPLExt.Row - 1 < length(CPLList.CPLInfoArray) then begin
  if CPLList.GetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row - 1]) <> 1 then exit;
  if CPLList.SetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row - 1], false) then begin
   dgCPLExt.OnClick(nil);
   dgCPLExt.Repaint;
  end;
 end;
end;

procedure TCPLManager.FormCreate(Sender: TObject);
begin
 CPLList := TCPLList.Create;
 TranslateForm(Self);
end;

procedure TCPLManager.FormDestroy(Sender: TObject);
begin
 CPLList.Free;
end;

procedure TCPLManager.dgCPLExtClick(Sender: TObject);
var
 Tmp : TCPLInfo;
begin
 if dgCPLExt.Row - 1 < length(CPLList.CPLInfoArray) then begin
  Tmp :=  CPLList.CPLInfoArray[dgCPLExt.Row - 1];
  mbDisableItem.Enabled := CPLList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := CPLList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TCPLManager.dgCPLExtDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TCPLInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0582';
   2 : S := '$AVZ0852';
   3 : S := '$AVZ0778';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(CPLList.CPLInfoArray) then begin
   Tmp := CPLList.CPLInfoArray[ARow - 1];
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
         S := Tmp.Name;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        case CPLList.GetItemEnabledStatus(Tmp) of
         -1 : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        S := '';
        end;
    1 : begin
         S := Tmp.Descr;
        end;
    2 : begin
         S := Tmp.LegalCopyright;
        end;
    3 : begin
         S := Tmp.BinFile;
        end;
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

procedure TCPLManager.dgCPLExtMouseDown(Sender: TObject;
  Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
 if (dgCPLExt.Row - 1 < length(CPLList.CPLInfoArray)) and (dgCPLExt.Col = 0) and (X in [2..11]) then
  case CPLList.GetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row-1]) of
   -1 : ;
    0 : if CPLList.SetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row-1], true) then Refresh;
    1 : if CPLList.SetItemEnabledStatus(CPLList.CPLInfoArray[dgCPLExt.Row-1], false) then Refresh;
  end;
 dgCPLExt.Invalidate;
end;

function TCPLManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 FileInfoStr : string;
begin
 Result := false;
 ALines.Add(' <CPL>');
 // Добавление данных в глобальный прогресс-контроль
 ZProgressClass.AddProgressMax(length(CPLList.CPLInfoArray));
 for i := 0 to length(CPLList.CPLInfoArray) - 1 do begin
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles) or (CPLList.CPLInfoArray[i].CheckResult <> 0) then begin
   if not(HideGoodFiles and (CPLList.CPLInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(CPLList.CPLInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(CPLList.CPLInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(CPLList.CPLInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(CPLList.GetItemEnabledStatus(CPLList.CPLInfoArray[i]))+'" '+
                        'Descr="'+XMLStr(CPLList.CPLInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(CPLList.CPLInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
   end;
  end;
 end;
 ALines.Add(' </CPL>');
end;

end.
