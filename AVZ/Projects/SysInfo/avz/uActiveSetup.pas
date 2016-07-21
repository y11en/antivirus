unit uActiveSetup;
// Менеджер Active Setup
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, Grids, RXGrids, zHTMLTemplate,
  zActiveSetup, zSharedFunctions, Menus, zAVZKernel, ImgList, zLogSystem;

type
  TActiveSetupManager = class(TForm)
    dgAS: TRxDrawGrid;
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    mbEnableItem: TBitBtn;
    mbDisableItem: TBitBtn;
    mbDeleteItem: TBitBtn;
    SaveDialog: TSaveDialog;
    pmProcessPopUp: TPopupMenu;
    N3: TMenuItem;
    N4: TMenuItem;
    pmiGoogle: TMenuItem;
    pmiYandex: TMenuItem;
    pmiRambler: TMenuItem;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure mbEnableItemClick(Sender: TObject);
    procedure mbDisableItemClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
    procedure dgASClick(Sender: TObject);
    procedure dgASDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
  private
    { Private declarations }
  public
    ASList : TASList;
    Function RefreshASList : boolean;
    Function Execute : boolean;
    // Создать HTML отчет
    function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
    // Создать XML отчет
    function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
    // Добавить все неопознанные файлы в карантин
    function AddAllToQurantine: boolean;
  end;

var
  ActiveSetupManager: TActiveSetupManager;

function ExecuteASView : boolean;

implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

function ExecuteASView : boolean;
begin
 ActiveSetupManager := nil;
 try
  ActiveSetupManager := TActiveSetupManager.Create(nil);
  Result     := ActiveSetupManager.Execute;
 finally
  ActiveSetupManager.Free;
  ActiveSetupManager := nil;
 end;
end;

function TActiveSetupManager.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(ASList.ActiveSetupInfoArray) - 1 do
  if ASList.ActiveSetupInfoArray[i].CheckResult <> 0 then
   CopyToInfected(ASList.ActiveSetupInfoArray[i].BinFile, '$AVZ0970', 'Quarantine');
end;

function TActiveSetupManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, FileInfoStr: string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852'+HTML_TableHeaderTD+'CLSID');
 GoodFiles := 0;
 for i := 0 to length(ASList.ActiveSetupInfoArray) - 1 do begin
  if ASList.ActiveSetupInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (ASList.ActiveSetupInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if ASList.ActiveSetupInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   if not(HideGoodFiles and (ASList.ActiveSetupInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatFileInfo(ASList.ActiveSetupInfoArray[i].BinFile, ', ');
    ALines.Add('<TR '+S+'>'+
               '<TD>'+'<a href="" title="'+FileInfoStr+'">'+ASList.ActiveSetupInfoArray[i].BinFile+'</a>'+GenScriptTxt(ASList.ActiveSetupInfoArray[i].BinFile)+
               '<TD>'+ASList.ActiveSetupInfoArray[i].Descr+
               '<TD>'+ASList.ActiveSetupInfoArray[i].LegalCopyright+
               '<TD>'+ASList.ActiveSetupInfoArray[i].CLSID
               );
    end;
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(ASList.ActiveSetupInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;


function TActiveSetupManager.Execute: boolean;
begin
 RefreshASList;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TActiveSetupManager.FormCreate(Sender: TObject);
begin
 ASList := TASList.Create;
 TranslateForm(Self);
end;

procedure TActiveSetupManager.FormDestroy(Sender: TObject);
begin
 ASList.Free;
 ASList := nil;
end;

function TActiveSetupManager.RefreshASList: boolean;
begin
 ASList.RefresASList;
 if length(ASList.ActiveSetupInfoArray) = 0 then
  dgAS.RowCount := 2
   else dgAS.RowCount := length(ASList.ActiveSetupInfoArray) + 1;
 dgAS.OnClick(nil);
end;

procedure TActiveSetupManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0429</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 SaveDialog.FileName := 'avz_as.htm';
 if SaveDialog.Execute and (SaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(SaveDialog.FileName);
   ShowHTMLLog(SaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

procedure TActiveSetupManager.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgAS.Row - 1 < length(ASList.ActiveSetupInfoArray) then begin
  if ASList.ActiveSetupInfoArray[dgAS.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+ASList.ActiveSetupInfoArray[dgAS.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(ASList.ActiveSetupInfoArray[dgAS.Row - 1].BinFile, '$AVZ0970', 'Quarantine');
 end;
end;

procedure TActiveSetupManager.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TActiveSetupManager.mbEnableItemClick(Sender: TObject);
begin
 if dgAS.Row - 1 < length(ASList.ActiveSetupInfoArray) then begin
  if ASList.GetItemEnabledStatus(ASList.ActiveSetupInfoArray[dgAS.Row - 1]) <> 0 then exit;
  if ASList.SetItemEnabledStatus(ASList.ActiveSetupInfoArray[dgAS.Row - 1], true) then begin
   dgAS.Repaint;
   dgAS.OnClick(nil);
  end;
 end;
end;

procedure TActiveSetupManager.mbDisableItemClick(Sender: TObject);
begin
 if dgAS.Row - 1 < length(ASList.ActiveSetupInfoArray) then begin
  if ASList.GetItemEnabledStatus(ASList.ActiveSetupInfoArray[dgAS.Row - 1]) <> 1 then exit;
  if ASList.SetItemEnabledStatus(ASList.ActiveSetupInfoArray[dgAS.Row - 1], false) then begin
   dgAS.OnClick(nil);
   dgAS.Repaint;
  end;
 end;
end;

procedure TActiveSetupManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgAS.Row - 1 < length(ASList.ActiveSetupInfoArray) then begin
  if not(ASList.DeleteItem(ASList.ActiveSetupInfoArray[dgAS.Row - 1])) then exit;
  RefreshASList;
 end;
end;

procedure TActiveSetupManager.dgASClick(Sender: TObject);
var
 Tmp : TActiveSetupInfo;
begin
 if dgAS.Row - 1 < length(ASList.ActiveSetupInfoArray) then begin
  Tmp :=  ASList.ActiveSetupInfoArray[dgAS.Row - 1];
  mbDisableItem.Enabled := ASList.GetItemEnabledStatus(Tmp)=1;
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
  mbEnableItem.Enabled  := ASList.GetItemEnabledStatus(Tmp)=0;
 end;
end;

procedure TActiveSetupManager.dgASDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TActiveSetupInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0581';
   2: S := '$AVZ0852';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(ASList.ActiveSetupInfoArray) then begin
   Tmp := ASList.ActiveSetupInfoArray[ARow - 1];
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
        case ASList.GetItemEnabledStatus(Tmp) of
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
   end;
   if S <> '' then
   (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

function TActiveSetupManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 S, FileInfoStr: string;
begin
 Result := false;
 ALines.Add(' <ActiveSetup>');
 // Добавление данных в глобальный прогресс-контроль
 ZProgressClass.AddProgressMax(length(ASList.ActiveSetupInfoArray));
 for i := 0 to length(ASList.ActiveSetupInfoArray) - 1 do begin
  // Шаг прогресс-индикации
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles) or (ASList.ActiveSetupInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if ASList.ActiveSetupInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   if not(HideGoodFiles and (ASList.ActiveSetupInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(ASList.ActiveSetupInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(ASList.ActiveSetupInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(ASList.ActiveSetupInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(ASList.GetItemEnabledStatus(ASList.ActiveSetupInfoArray[i]))+'" '+
                        'RegKey="'+XMLStr(ASList.ActiveSetupInfoArray[i].RegKey)+'" '+
                        'CLSID="'+XMLStr(ASList.ActiveSetupInfoArray[i].CLSID)+'" '+
                        'Descr="'+XMLStr(ASList.ActiveSetupInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(ASList.ActiveSetupInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
    end;
  end;
 end;
 ALines.Add(' </ActiveSetup>');
end;

end.
