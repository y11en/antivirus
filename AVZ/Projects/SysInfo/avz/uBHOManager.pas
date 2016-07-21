unit uBHOManager;
// ******************* Менеджер BHO **********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, zBHO, zHTMLTemplate, Grids, RXGrids, StdCtrls, Buttons, ExtCtrls,
  Menus, zSharedFunctions, zAVZKernel, ImgList, zLogSystem;

type
  TBHOManager = class(TForm)
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    dgBHO: TRxDrawGrid;
    pmProcessPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    SpeedButton5: TSpeedButton;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgBHODrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgBHOMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure dgBHOClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure pmiRamblerClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
  public
   // Список BHO
   BHOList : TBHOList;
   // Обновление списка BHO
   Function RefreshBHOList : boolean;
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
  BHOManager: TBHOManager;

 // Выполнение диалога
 function ExecuteBHOView : boolean;
implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

{ TBHOManager }
function ExecuteBHOView : boolean;
begin
 BHOManager := nil;
 try
  BHOManager := TBHOManager.Create(nil);
  Result     := BHOManager.Execute;
 finally
  BHOManager.Free;
  BHOManager := nil;
 end;
end;

function TBHOManager.Execute: boolean;
begin
 RefreshBHOList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TBHOManager.FormCreate(Sender: TObject);
begin
 BHOList := TBHOList.Create;
 TranslateForm(Self);
end;

procedure TBHOManager.FormDestroy(Sender: TObject);
begin
 BHOList.Free;
end;

function TBHOManager.RefreshBHOList: boolean;
begin
 BHOList.RefresBHOList;
 if length(BHOList.BHOInfoArray) = 0 then
  dgBHO.RowCount := 2
   else dgBHO.RowCount := length(BHOList.BHOInfoArray) + 1;
 // Переристовка GRID  
 dgBHO.Refresh;
 dgBHO.Repaint;
 Application.ProcessMessages;
 dgBHO.OnClick(nil);  
end;

procedure TBHOManager.dgBHODrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TBHOInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ1062';
   2 : S := '$AVZ0581';
   3 : S := '$AVZ0852';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(BHOList.BHOInfoArray) then begin
   Tmp := BHOList.BHOInfoArray[ARow - 1];
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
        case BHOList.GetItemEnabledStatus(Tmp) of
         -1 : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        S := '';
        end;
    1 : begin
         S := '';
         case Tmp.BHOType of
          1 : S := 'BHO';
          2 : S := TranslateStr('$AVZ0689');
          3 : S := TranslateStr('$AVZ0440');
         end;
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

procedure TBHOManager.dgBHOMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
 if (dgBHO.Row - 1 < length(BHOList.BHOInfoArray)) and (dgBHO.Col = 0) and (X in [2..11]) then
  case BHOList.GetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row-1]) of
   -1 : ;
    0 : if BHOList.SetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row-1], true) then Refresh;
    1 : if BHOList.SetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row-1], false) then Refresh;
  end;
 dgBHO.Invalidate;
end;

procedure TBHOManager.dgBHOClick(Sender: TObject);
var
 Tmp : TBHOInfo;
begin
 if dgBHO.Row - 1 < length(BHOList.BHOInfoArray) then begin
  Tmp :=  BHOList.BHOInfoArray[dgBHO.Row - 1];
  mbDisableItem.Enabled := BHOList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := BHOList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TBHOManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0432</H2>'));
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

function TBHOManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, ST, FileInfoStr: string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ1062'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852'+HTML_TableHeaderTD+'CLSID');
 GoodFiles := 0;
 for i := 0 to length(BHOList.BHOInfoArray) - 1 do begin
  if BHOList.BHOInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (BHOList.BHOInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if BHOList.BHOInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   case BHOList.BHOInfoArray[i].BHOType of
    1 : ST := 'BHO';
    2 : ST := '$AVZ0689';
    3 : ST := '$AVZ0440';
    else ST := '?';
   end;
   if not(HideGoodFiles and (BHOList.BHOInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(BHOList.BHOInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+BHOList.BHOInfoArray[i].BinFile+'</a>'+GenScriptTxt(BHOList.BHOInfoArray[i].BinFile)+
               '<TD>'+ST+
               '<TD>'+BHOList.BHOInfoArray[i].Descr+
               '<TD>'+BHOList.BHOInfoArray[i].LegalCopyright+
               '<TD>'+BHOList.BHOInfoArray[i].CLSID + GenScriptTxtBHO(BHOList.BHOInfoArray[i].CLSID)
               );
    end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(BHOList.BHOInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TBHOManager.N3Click(Sender: TObject);
begin
 sbCopyToQurantine.Click;
end;

function TBHOManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(BHOList.BHOInfoArray) - 1 do
  if BHOList.BHOInfoArray[i].CheckResult <> 0 then
   CopyToInfected(BHOList.BHOInfoArray[i].BinFile, '$AVZ0973', 'Quarantine');
end;

procedure TBHOManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TBHOManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgBHO.Row - 1 < length(BHOList.BHOInfoArray) then begin
  if BHOList.BHOInfoArray[dgBHO.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+BHOList.BHOInfoArray[dgBHO.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(BHOList.BHOInfoArray[dgBHO.Row - 1].BinFile, '$AVZ0973', 'Quarantine');
 end;
end;

procedure TBHOManager.mbEnableItemClick(Sender: TObject);
begin
 if dgBHO.Row - 1 < length(BHOList.BHOInfoArray) then begin
  if BHOList.GetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row - 1]) <> 0 then exit;
  if BHOList.SetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row - 1], true) then begin
   dgBHO.Repaint;
   dgBHO.OnClick(nil);
  end;
 end;
end;

procedure TBHOManager.mbDisableItemClick(Sender: TObject);
begin
 if dgBHO.Row - 1 < length(BHOList.BHOInfoArray) then begin
  if BHOList.GetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row - 1]) <> 1 then exit;
  if BHOList.SetItemEnabledStatus(BHOList.BHOInfoArray[dgBHO.Row - 1], false) then begin
   dgBHO.OnClick(nil);
   dgBHO.Repaint;
  end;
 end;
end;

procedure TBHOManager.pmiRamblerClick(Sender: TObject);
begin
 if dgBHO.Row - 1 < length(BHOList.BHOInfoArray) then
  case (Sender as TMenuItem).Tag of
   1 : WebSearchInGoogle(ExtractFileName(BHOList.BHOInfoArray[dgBHO.Row-1].BinFile));
   2 : WebSearchInYandex(ExtractFileName(BHOList.BHOInfoArray[dgBHO.Row-1].BinFile));
   3 : WebSearchInRambler(ExtractFileName(BHOList.BHOInfoArray[dgBHO.Row-1].BinFile));
  end;
end;

procedure TBHOManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgBHO.Row - 1 < length(BHOList.BHOInfoArray) then begin
  if not(BHOList.DeleteItem(BHOList.BHOInfoArray[dgBHO.Row - 1])) then exit;
  RefreshBHOList;
 end;
end;

function TBHOManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 FileInfoStr: string;
begin
 Result := false;
 ALines.Add(' <BHO>');
 // Добавление данных в глобальный прогресс-контроль
 ZProgressClass.AddProgressMax(length(BHOList.BHOInfoArray));
 for i := 0 to length(BHOList.BHOInfoArray) - 1 do begin
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles) or (BHOList.BHOInfoArray[i].CheckResult <> 0) then begin
   if not(HideGoodFiles and (BHOList.BHOInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(BHOList.BHOInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(BHOList.BHOInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(BHOList.BHOInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(BHOList.GetItemEnabledStatus(BHOList.BHOInfoArray[i]))+'" '+
                        'BHOType="'+IntToStr(BHOList.BHOInfoArray[i].BHOType)+'" '+
                        'RegKey="'+XMLStr(BHOList.BHOInfoArray[i].RegKey)+'" '+
                        'CLSID="'+XMLStr(BHOList.BHOInfoArray[i].CLSID)+'" '+
                        'Descr="'+XMLStr(BHOList.BHOInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(BHOList.BHOInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');

    end;
  end;
 end;
 ALines.Add(' </BHO>');
end;

end.
