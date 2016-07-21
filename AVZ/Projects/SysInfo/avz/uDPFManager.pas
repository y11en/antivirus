unit uDPFManager;
// **************** Менеджер DPF *********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, Grids, RXGrids, zDPF, zHTMLTemplate,
  zSharedFunctions, zAVZKernel, zLogSystem;

type
  TDPFManager = class(TForm)
    dgDPF: TRxDrawGrid;
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgDPFDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgDPFClick(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure dgDPFMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure mbDeleteItemClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
  public
   DPFList : TDPFList;
   Function RefreshDPFList : boolean;
   Function Execute : boolean;
   // Создание HTML отчета
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создание XML отчета
   function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  DPFManager: TDPFManager;

function ExecuteDPFManager : boolean;

implementation

uses uGUIShared, zTranslate;

{$R *.dfm}
function ExecuteDPFManager : boolean;
begin
 DPFManager := nil;
 try
  DPFManager := TDPFManager.Create(nil);
  Result     := DPFManager.Execute;
 finally
  DPFManager.Free;
  DPFManager := nil;
 end;
end;

{ TDPFManager }

function TDPFManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(DPFList.DPFInfoArray) - 1 do
  if DPFList.DPFInfoArray[i].CheckResult <> 0 then
   CopyToInfected(DPFList.DPFInfoArray[i].BinFile, '$AVZ0971', 'Quarantine');
end;

function TDPFManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, ST, FileInfoStr: string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852'+HTML_TableHeaderTD+'CLSID'+HTML_TableHeaderTD+'$AVZ0033');
 GoodFiles := 0;
 if length(DPFList.DPFInfoArray) > 0 then
 for i := 0 to length(DPFList.DPFInfoArray) - 1 do begin
  if DPFList.DPFInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (DPFList.DPFInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if DPFList.DPFInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   if not(HideGoodFiles and (DPFList.DPFInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(DPFList.DPFInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+DPFList.DPFInfoArray[i].BinFile+'</a>'+GenScriptTxt(DPFList.DPFInfoArray[i].BinFile)+
               '<TD>'+DPFList.DPFInfoArray[i].Descr+
               '<TD>'+DPFList.DPFInfoArray[i].LegalCopyright+
               '<TD>'+DPFList.DPFInfoArray[i].CLSID+GenScriptTxtDPF(DPFList.DPFInfoArray[i].CLSID)+
               '<TD>'+DPFList.DPFInfoArray[i].CodeBase
               );
    end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(DPFList.DPFInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

function TDPFManager.Execute: boolean;
begin
 RefreshDPFList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

function TDPFManager.RefreshDPFList: boolean;
begin
 DPFList.RefresDPFList;
 if length(DPFList.DPFInfoArray) = 0 then
  dgDPF.RowCount := 2
   else dgDPF.RowCount := length(DPFList.DPFInfoArray) + 1;
 dgDPF.OnClick(nil);
 Repaint;
 dgDPF.Repaint;
end;

procedure TDPFManager.FormCreate(Sender: TObject);
begin
 DPFList := TDPFList.Create;
 TranslateForm(Self);
end;

procedure TDPFManager.FormDestroy(Sender: TObject);
begin
 DPFList.Free;
end;

procedure TDPFManager.dgDPFDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TDPFInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0581';
   2 : S := '$AVZ0852';
   3 : S := '$AVZ0034';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(DPFList.DPFInfoArray) then begin
   Tmp := DPFList.DPFInfoArray[ARow - 1];
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
        case DPFList.GetItemEnabledStatus(Tmp) of
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
         S := Tmp.CodeBase;
        end;
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

procedure TDPFManager.dgDPFClick(Sender: TObject);
var
 Tmp : TDPFInfo;
begin
 if dgDPF.Row - 1 < length(DPFList.DPFInfoArray) then begin
  Tmp :=  DPFList.DPFInfoArray[dgDPF.Row - 1];
  mbDisableItem.Enabled := DPFList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := DPFList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TDPFManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgDPF.Row - 1 < length(DPFList.DPFInfoArray) then begin
  if DPFList.DPFInfoArray[dgDPF.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+DPFList.DPFInfoArray[dgDPF.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(DPFList.DPFInfoArray[dgDPF.Row - 1].BinFile, '$AVZ0967', 'Quarantine');
 end;
end;

procedure TDPFManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TDPFManager.mbEnableItemClick(Sender: TObject);
begin
 if dgDPF.Row - 1 < length(DPFList.DPFInfoArray) then begin
  if DPFList.GetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row - 1]) <> 0 then exit;
  if DPFList.SetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row - 1], true) then begin
   RefreshDPFList;
  end;
 end;
end;

procedure TDPFManager.mbDisableItemClick(Sender: TObject);
begin
 if dgDPF.Row - 1 < length(DPFList.DPFInfoArray) then begin
  if DPFList.GetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row - 1]) <> 1 then exit;
  if DPFList.SetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row - 1], false) then begin
   RefreshDPFList;
  end;
 end;
end;

procedure TDPFManager.dgDPFMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
 if (dgDPF.Row - 1 < length(DPFList.DPFInfoArray)) and (dgDPF.Col = 0) and (X in [2..11]) then
  case DPFList.GetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row-1]) of
   -1 : ;
    0 : if DPFList.SetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row-1], true) then Refresh;
    1 : if DPFList.SetItemEnabledStatus(DPFList.DPFInfoArray[dgDPF.Row-1], false) then Refresh;
  end;
 dgDPF.Invalidate;
end;

procedure TDPFManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgDPF.Row - 1 < length(DPFList.DPFInfoArray) then begin
  if not(DPFList.DeleteItem(DPFList.DPFInfoArray[dgDPF.Row - 1])) then exit;
  RefreshDPFList;
 end;
end;

procedure TDPFManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>Downloaded Program Files</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_dpf.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TDPFManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 S, ST, FileInfoStr: string;
begin
 Result := false;
 ALines.Add(' <DPF>');
 if length(DPFList.DPFInfoArray) > 0 then
 // Добавление данных в глобальный прогресс-контроль
 ZProgressClass.AddProgressMax(length(DPFList.DPFInfoArray));
 for i := 0 to length(DPFList.DPFInfoArray) - 1 do begin
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles) or (DPFList.DPFInfoArray[i].CheckResult <> 0) then begin
   if not(HideGoodFiles and (DPFList.DPFInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(DPFList.DPFInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(DPFList.DPFInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(DPFList.DPFInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(DPFList.GetItemEnabledStatus(DPFList.DPFInfoArray[i]))+'" '+
                        'RegKey="'+XMLStr(DPFList.DPFInfoArray[i].RegKey)+'" '+
                        'CLSID="'+XMLStr(DPFList.DPFInfoArray[i].CLSID)+'" '+
                        'CodeBase="'+XMLStr(DPFList.DPFInfoArray[i].CodeBase)+'" '+
                        'Descr="'+XMLStr(DPFList.DPFInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(DPFList.DPFInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
    end;
  end;
 end;
 ALines.Add(' </DPF>');
end;

end.
