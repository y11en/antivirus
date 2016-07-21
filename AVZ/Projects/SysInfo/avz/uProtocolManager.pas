unit uProtocolManager;
// ************* Менеджер протоколов и обработчиков **************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Menus, Grids, RXGrids, StdCtrls, Buttons, ExtCtrls, zProtocols,
  zHTMLTemplate, zSharedFunctions, zAVZKernel;

type
  TProtocolManager = class(TForm)
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    dgProtocol: TRxDrawGrid;
    pmProtocolPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    procedure FormCreate(Sender: TObject);
    procedure dgProtocolClick(Sender: TObject);
    procedure dgProtocolDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure mbDeleteItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
  public
   ProtocolList : TProtocolList;
   Function RefreshList : boolean;
   Function Execute : boolean;
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  ProtocolManager: TProtocolManager;

 function ExecuteProtocolView : boolean;
implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

function ExecuteProtocolView : boolean;
begin
 ProtocolManager := nil;
 try
  ProtocolManager := TProtocolManager.Create(nil);
  Result     := ProtocolManager.Execute;
 finally
  ProtocolManager.Free;
  ProtocolManager := nil;
 end;
end;

{ TProtocolManager }

function TProtocolManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(ProtocolList.ProtocolInfoArray) - 1 do
  if ProtocolList.ProtocolInfoArray[i].CheckResult <> 0 then
   CopyToInfected(ProtocolList.ProtocolInfoArray[i].BinFile, '$AVZ0974', 'Quarantine');
end;

function TProtocolManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, ST, FileInfoStr : string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ1062'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852'+HTML_TableHeaderTD+'CLSID');
 GoodFiles := 0;
 for i := 0 to length(ProtocolList.ProtocolInfoArray) - 1 do begin
  if ProtocolList.ProtocolInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (ProtocolList.ProtocolInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if ProtocolList.ProtocolInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   case ProtocolList.ProtocolInfoArray[i].ProtocolType of
    1 : ST := 'Protocol';
    2 : ST := 'Handler';
    else ST := '?';
   end;
   if not(HideGoodFiles and (ProtocolList.ProtocolInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(ProtocolList.ProtocolInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+ProtocolList.ProtocolInfoArray[i].BinFile+'</a>'+GenScriptTxt(ProtocolList.ProtocolInfoArray[i].BinFile)+
               '<TD>'+ST+
               '<TD>'+ProtocolList.ProtocolInfoArray[i].Descr+' ('+ProtocolList.ProtocolInfoArray[i].Prim+')'+
               '<TD>'+ProtocolList.ProtocolInfoArray[i].LegalCopyright+
               '<TD>'+ProtocolList.ProtocolInfoArray[i].CLSID
               );
    end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(ProtocolList.ProtocolInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

function TProtocolManager.Execute: boolean;
begin
 RefreshList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

function TProtocolManager.RefreshList: boolean;
begin
 ProtocolList.RefresList;
 if length(ProtocolList.ProtocolInfoArray) = 0 then
  dgProtocol.RowCount := 2
   else dgProtocol.RowCount := length(ProtocolList.ProtocolInfoArray) + 1;
 dgProtocol.OnClick(nil);  
end;

procedure TProtocolManager.FormCreate(Sender: TObject);
begin
 ProtocolList := TProtocolList.Create;
 TranslateForm(Self);
end;

procedure TProtocolManager.dgProtocolClick(Sender: TObject);
var
 Tmp : TProtocolInfo;
begin
 if dgProtocol.Row - 1 < length(ProtocolList.ProtocolInfoArray) then begin
  Tmp :=  ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1];
  mbDisableItem.Enabled := ProtocolList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := ProtocolList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TProtocolManager.dgProtocolDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TProtocolInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ1062';
   2 : S := '$AVZ0581';
   3 : S := '$AVZ0852';
   4 : S := '$AVZ0802';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(ProtocolList.ProtocolInfoArray) then begin
   Tmp := ProtocolList.ProtocolInfoArray[ARow - 1];
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
          S := '$AVZ1117, '+Tmp.CLSID;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        case ProtocolList.GetItemEnabledStatus(Tmp) of
         -1 : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        S := '';
        end;
    1 : begin
         S := '';
         case Tmp.ProtocolType of
          1 : S := 'Protocol '+Tmp.RegKeyName;
          2 : S := 'Handler '+Tmp.RegKeyName;
         end;
        end;
    2 : begin
         S := Tmp.Descr;
        end;
    3 : begin
         S := Tmp.LegalCopyright;
        end;
    4 : begin
         S := Tmp.Prim;
        end;
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

procedure TProtocolManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgProtocol.Row - 1 < length(ProtocolList.ProtocolInfoArray) then begin
  if not(ProtocolList.DeleteItem(ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1])) then exit;
  RefreshList;
 end;
end;

procedure TProtocolManager.mbDisableItemClick(Sender: TObject);
begin
 if dgProtocol.Row - 1 < length(ProtocolList.ProtocolInfoArray) then begin
  if ProtocolList.GetItemEnabledStatus(ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1]) <> 1 then exit;
  if ProtocolList.SetItemEnabledStatus(ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1], false) then begin
   dgProtocol.OnClick(nil);
   dgProtocol.Repaint;
  end;
 end;
end;

procedure TProtocolManager.mbEnableItemClick(Sender: TObject);
begin
 if dgProtocol.Row - 1 < length(ProtocolList.ProtocolInfoArray) then begin
  if ProtocolList.GetItemEnabledStatus(ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1]) <> 0 then exit;
  if ProtocolList.SetItemEnabledStatus(ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1], true) then begin
   dgProtocol.Repaint;
   dgProtocol.OnClick(nil);
  end;
 end;
end;

procedure TProtocolManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TProtocolManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0432</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_protocol.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

procedure TProtocolManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgProtocol.Row - 1 < length(ProtocolList.ProtocolInfoArray) then begin
  if ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(ProtocolList.ProtocolInfoArray[dgProtocol.Row - 1].BinFile, '$AVZ0974', 'Quarantine');
 end;
end;

end.
