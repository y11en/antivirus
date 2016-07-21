unit uPrintExtManager;
// ************ Менеджер расширений системы печати ****************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, zPrintExt, zHTMLTemplate, Grids, RXGrids, StdCtrls, Buttons, ExtCtrls,
  Menus, zSharedFunctions, zAVZKernel, zLogSystem;

type
  TPrintExtManager = class(TForm)
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    dgPrintExt: TRxDrawGrid;
    pmProcessPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    SpeedButton5: TSpeedButton;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgPrintExtDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgPrintExtMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure dgPrintExtClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure pmiRamblerClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
  private
  public
   // Класс для сбора информации о расширениях печати
   PrintExtList : TPrintExtList;
   // Обновить список расширений
   Function RefreshPrintExtList : boolean;
   // Выполнить диалог
   Function Execute : boolean;
   // Создать HTML отчет
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создать XML отчет
   function CreateXMLReport(ALines: TStrings;  HideGoodFiles: boolean): boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  PrintExtManager: TPrintExtManager;

 function ExecutePrintExtManager : boolean;
implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

{ TPrintExtManager }
function ExecutePrintExtManager : boolean;
begin
 PrintExtManager := nil;
 try
  PrintExtManager := TPrintExtManager.Create(nil);
  Result     := PrintExtManager.Execute;
 finally
  PrintExtManager.Free;
  PrintExtManager := nil;
 end;
end;

function TPrintExtManager.Execute: boolean;
begin
 RefreshPrintExtList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TPrintExtManager.FormCreate(Sender: TObject);
begin
 PrintExtList := TPrintExtList.Create;
 TranslateForm(Self);
end;

procedure TPrintExtManager.FormDestroy(Sender: TObject);
begin
 PrintExtList.Free;
end;

function TPrintExtManager.RefreshPrintExtList: boolean;
begin
 PrintExtList.RefresPrintExtList;
 if length(PrintExtList.PrintExtInfoArray) = 0 then
  dgPrintExt.RowCount := 2
   else dgPrintExt.RowCount := length(PrintExtList.PrintExtInfoArray) + 1;
 dgPrintExt.OnClick(nil);  
end;

procedure TPrintExtManager.dgPrintExtDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TPrintExtInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ1062';
   2 : S := '$AVZ0453';
   3 : S := '$AVZ0582';
   4 : S := '$AVZ0852';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(PrintExtList.PrintExtInfoArray) then begin
   Tmp := PrintExtList.PrintExtInfoArray[ARow - 1];
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
          S := TranslateStr('$AVZ1117');
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        case PrintExtList.GetItemEnabledStatus(Tmp) of
         -1 : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        S := '';
        end;
    1 : begin
         S := '';
         case Tmp.PrintExtType of
          1 : S := TranslateStr('$AVZ0443');
          2 : S := TranslateStr('$AVZ0805');
         end;
        end;
    2 : begin
         S := Tmp.Name;
        end;
    3 : begin
         S := Tmp.Descr;
        end;
    4 : begin
         S := Tmp.LegalCopyright;
        end;
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

procedure TPrintExtManager.dgPrintExtMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
 if (dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray)) and (dgPrintExt.Col = 0) and (X in [2..11]) then
  case PrintExtList.GetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row-1]) of
   -1 : ;
    0 : if PrintExtList.SetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row-1], true) then Refresh;
    1 : if PrintExtList.SetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row-1], false) then Refresh;
  end;
 dgPrintExt.Invalidate;
end;

procedure TPrintExtManager.dgPrintExtClick(Sender: TObject);
var
 Tmp : TPrintExtInfo;
begin
 if dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray) then begin
  Tmp :=  PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1];
  mbDisableItem.Enabled := PrintExtList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := PrintExtList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TPrintExtManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0435</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_prn_ext.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TPrintExtManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, ST, FileInfoStr : string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ1062'+HTML_TableHeaderTD+'$AVZ0453'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852');
 GoodFiles := 0;
 for i := 0 to length(PrintExtList.PrintExtInfoArray) - 1 do begin
  if PrintExtList.PrintExtInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (PrintExtList.PrintExtInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if PrintExtList.PrintExtInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   case PrintExtList.PrintExtInfoArray[i].PrintExtType of
    1 : ST := '$AVZ0443';
    2 : ST := '$AVZ0805';
    else ST := '?';
   end;
   if not(HideGoodFiles and (PrintExtList.PrintExtInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(PrintExtList.PrintExtInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+PrintExtList.PrintExtInfoArray[i].BinFile+'</a>'+GenScriptTxt(PrintExtList.PrintExtInfoArray[i].BinFile)+
               '<TD>'+ST+
               '<TD>'+PrintExtList.PrintExtInfoArray[i].Name+
               '<TD>'+PrintExtList.PrintExtInfoArray[i].Descr+
               '<TD>'+PrintExtList.PrintExtInfoArray[i].LegalCopyright
               );
   end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(PrintExtList.PrintExtInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TPrintExtManager.N3Click(Sender: TObject);
begin
 sbCopyToQurantine.Click;
end;

function TPrintExtManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(PrintExtList.PrintExtInfoArray) - 1 do
  if PrintExtList.PrintExtInfoArray[i].CheckResult <> 0 then
   CopyToInfected(NormalProgramFileName(PrintExtList.PrintExtInfoArray[i].BinFile, '.dll'), '$AVZ0976', 'Quarantine');
end;

procedure TPrintExtManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TPrintExtManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray) then begin
  if PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1].BinFile, '$AVZ0973', 'Quarantine');
 end;
end;

procedure TPrintExtManager.mbEnableItemClick(Sender: TObject);
begin
 if dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray) then begin
  if PrintExtList.GetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1]) <> 0 then exit;
  if PrintExtList.SetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1], true) then begin
   dgPrintExt.Repaint;
   dgPrintExt.OnClick(nil);
  end;
 end;
end;

procedure TPrintExtManager.mbDisableItemClick(Sender: TObject);
begin
 if dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray) then begin
  if PrintExtList.GetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1]) <> 1 then exit;
  if PrintExtList.SetItemEnabledStatus(PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1], false) then begin
   dgPrintExt.OnClick(nil);
   dgPrintExt.Repaint;
  end;
 end;
end;

procedure TPrintExtManager.pmiRamblerClick(Sender: TObject);
begin
 if dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray) then
  case (Sender as TMenuItem).Tag of
   1 : WebSearchInGoogle(ExtractFileName(PrintExtList.PrintExtInfoArray[dgPrintExt.Row-1].BinFile));
   2 : WebSearchInYandex(ExtractFileName(PrintExtList.PrintExtInfoArray[dgPrintExt.Row-1].BinFile));
   3 : WebSearchInRambler(ExtractFileName(PrintExtList.PrintExtInfoArray[dgPrintExt.Row-1].BinFile));
  end;
end;

procedure TPrintExtManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgPrintExt.Row - 1 < length(PrintExtList.PrintExtInfoArray) then begin
  if not(PrintExtList.DeleteItem(PrintExtList.PrintExtInfoArray[dgPrintExt.Row - 1])) then exit;
  RefreshPrintExtList;
 end;
end;

function TPrintExtManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 S, ST, FileInfoStr: string;
begin
 Result := false;
 ALines.Add(' <PrintEXT>');
 if length(PrintExtList.PrintExtInfoArray) > 0 then
 // Добавление данных в глобальный прогресс-контроль
 ZProgressClass.AddProgressMax(length(PrintExtList.PrintExtInfoArray));
 for i := 0 to length(PrintExtList.PrintExtInfoArray) - 1 do begin
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles) or (PrintExtList.PrintExtInfoArray[i].CheckResult <> 0) then begin
   if not(HideGoodFiles and (PrintExtList.PrintExtInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(PrintExtList.PrintExtInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(PrintExtList.PrintExtInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(PrintExtList.PrintExtInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(PrintExtList.GetItemEnabledStatus(PrintExtList.PrintExtInfoArray[i]))+'" '+
                        'RegKey="'+XMLStr(PrintExtList.PrintExtInfoArray[i].RegKey)+'" '+
                        'Descr="'+XMLStr(PrintExtList.PrintExtInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(PrintExtList.PrintExtInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
    end;
  end;
 end;
 ALines.Add(' </PrintEXT>');
end;

end.
