unit uServiceView;
// Менеджер служб и драйверов
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, RXGrids, StdCtrls, Buttons, zServices, ExtCtrls, Grids, ComCtrls,
  Registry, zHTMLTemplate, zAVKernel, zAVZKernel, zLogSystem;

type
  TServiceView = class(TForm)
    GroupBox1: TGroupBox;
    Label1: TLabel;
    Label2: TLabel;
    cbServiceMode: TComboBox;
    cbServiceStatus: TComboBox;
    pcService: TPageControl;
    tsServiceByAPI: TTabSheet;
    tsServiceByReg: TTabSheet;
    dgService: TRxDrawGrid;
    dgServiceByReg: TRxDrawGrid;
    Panel1: TPanel;
    SpeedButton1: TSpeedButton;
    SpeedButton2: TSpeedButton;
    sbCopyProcToQurantine: TSpeedButton;
    sbCopyAllToQurantine: TSpeedButton;
    sbStartService: TSpeedButton;
    mbStopService: TSpeedButton;
    mbKillService: TSpeedButton;
    procedure FormCreate(Sender: TObject);
    procedure dgServiceDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure SpeedButton1Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure cbServiceModeChange(Sender: TObject);
    procedure pcServiceChange(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
    procedure sbCopyProcToQurantineClick(Sender: TObject);
    procedure sbCopyAllToQurantineClick(Sender: TObject);
    procedure dgServiceClick(Sender: TObject);
    procedure sbStartServiceClick(Sender: TObject);
    procedure mbStopServiceClick(Sender: TObject);
    procedure mbKillServiceClick(Sender: TObject);
    procedure dgServiceFixedCellClick(Sender: TObject; ACol,
      ARow: Integer);
  private
    function GetDriverMode: boolean;
  public
   ServiceManager : TServiceManager;
   SortKey : integer;
   function CreateServiceList: boolean;
   function Execute : boolean;
   // Создание HTML отчета
   function CreateHTMLReport(ALines: TStrings;  HideGoodFiles, ADriverMode: boolean): boolean;
   // Создание XML отчета
   function CreateXMLReport(ALines: TStrings;  HideGoodFiles: boolean): boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  published
   property DiverMode : boolean read GetDriverMode;
  end;

var
  ServiceView: TServiceView;

Function ExecuteServiceView : boolean;

implementation

uses zSharedFunctions, uGUIShared, zTranslate;

{$R *.dfm}
function TServiceView.CreateServiceList: boolean;
var
 i : integer;
 S : string;
begin
 // Обновление списка сервисов
 if pcService.ActivePage = tsServiceByAPI then
  ServiceManager.RefreshServicesByAPI(cbServiceMode.ItemIndex, cbServiceStatus.ItemIndex)
   else ServiceManager.RefreshServicesByReg(cbServiceMode.ItemIndex, cbServiceStatus.ItemIndex);
 // Проверка файлов
  ZProgressClass.AddProgressMax(length(ServiceManager.ServiceArray));
  for i := 0 to length(ServiceManager.ServiceArray) - 1 do begin
   ZProgressClass.ProgressStep;
   S := ServiceManager.NormalServiceFile(ServiceManager.ServiceArray[i].BinFile, '.exe');
   if S <> '' then
    ServiceManager.ServiceArray[i].CheckResult := FileSignCheck.CheckFile(S);
  end;
 // Сортировка
 ServiceManager.SortData(SortKey);
 dgService.RowCount      := length(ServiceManager.ServiceArray)+1;
 dgServiceByReg.RowCount := length(ServiceManager.ServiceArray)+1;
 dgService.Refresh;
 dgServiceByReg.Refresh;
end;

procedure TServiceView.FormCreate(Sender: TObject);
begin
 cbServiceMode.ItemIndex := 0;
 cbServiceStatus.ItemIndex := 0;
 SortKey := 0;
 TranslateForm(Self);
 ServiceManager := TServiceManager.Create;
end;

procedure TServiceView.dgServiceDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0998';
   1 : S := '$AVZ0581';
   2 : S := '$AVZ1051';
   3 : S := '$AVZ1109';
   4 : S := '$AVZ0199';
   5 : S := '$AVZ0245';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(ServiceManager.ServiceArray) then
  case ACol of
   0 : S := ServiceManager.ServiceArray[ARow-1].Name;
   1 : S := ServiceManager.ServiceArray[ARow-1].Descr;
   2 : S := TranslateStr(ServiceManager.ServiceArray[ARow-1].Status);
   3 : S := ServiceManager.ServiceArray[ARow-1].BinFile;
   4 : S := ServiceManager.ServiceArray[ARow-1].Group;
   5 : S := ServiceManager.ServiceArray[ARow-1].Dependencies;
  end;
  // Цветовое выделение
  if not((gdFocused in State) or (gdSelected in State)) then begin
    if ServiceManager.ServiceArray[ARow-1].CheckResult = 0 then
     (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
   end else begin
    if ServiceManager.ServiceArray[ARow-1].CheckResult = 0 then
     (Sender as TRxDrawGrid).Canvas.Brush.Color := clGreen;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

Function ExecuteServiceView : boolean;
begin
 try
  ServiceView := TServiceView.Create(nil);
  ServiceView.Execute;
 finally
  ServiceView.Free;
  ServiceView := nil;
 end;
end;

function TServiceView.Execute: boolean;
begin
 CreateServiceList;
 ShowModal;
end;

procedure TServiceView.SpeedButton1Click(Sender: TObject);
begin
 CreateServiceList;
end;

procedure TServiceView.FormDestroy(Sender: TObject);
begin
 ServiceManager.Free;
end;

procedure TServiceView.cbServiceModeChange(Sender: TObject);
begin
 CreateServiceList;
end;

procedure TServiceView.pcServiceChange(Sender: TObject);
begin
 sbStartService.Enabled :=  pcService.ActivePage = tsServiceByAPI;
 mbStopService.Enabled  :=  pcService.ActivePage = tsServiceByAPI;
 if pcService.ActivePage = tsServiceByReg then begin
  cbServiceStatus.Enabled := false;
  cbServiceStatus.ItemIndex := 2;
 end else begin
  cbServiceStatus.Enabled := true;
 end;
 CreateServiceList;
end;

function TServiceView.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles, ADriverMode: boolean): boolean;
var
 i, GoodFiles : integer;
 S, FileInfoStr : string;
begin
 GoodFiles := 0;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0998'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ1051'+HTML_TableHeaderTD+'$AVZ1109'+HTML_TableHeaderTD+'$AVZ0199'+HTML_TableHeaderTD+'$AVZ0245');
 for i := 0 to Length(ServiceManager.ServiceArray)-1 do begin
  if ServiceManager.ServiceArray[i].CheckResult = 0 then begin
   S := 'bgColor='+HTMP_BgColorTrusted;
   inc(GoodFiles);
  end else S :=  'bgColor='+HTMP_BgColor2;
  if not(HideGoodFiles and (ServiceManager.ServiceArray[i].CheckResult = 0)) then begin
   FileInfoStr := FormatFileInfo(ServiceManager.NormalServiceFile(ServiceManager.ServiceArray[i].BinFile, '.exe'), ', ');
   ALines.Add('<TR '+S+'><TD>'+HTMLNVL(ServiceManager.ServiceArray[i].Name)+GenScriptSvcTxt(ServiceManager.ServiceArray[i].Name, ADriverMode)+
                  '<TD>'+HTMLNVL(ServiceManager.ServiceArray[i].Descr)+
                  '<TD>'+HTMLNVL(ServiceManager.ServiceArray[i].Status)+
                  '<TD>'+'<a title="'+FileInfoStr+'" href="">'+HTMLNVL(ServiceManager.ServiceArray[i].BinFile)+'</a>'+GenScriptTxt(ServiceManager.ServiceArray[i].BinFile)+
                  '<TD>'+HTMLNVL(ServiceManager.ServiceArray[i].Group)+
                  '<TD>'+HTMLNVL(ServiceManager.ServiceArray[i].Dependencies));
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7>$AVZ0538 - '+IntToStr(Length(ServiceManager.ServiceArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TServiceView.SpeedButton2Click(Sender: TObject);
var
 RepLines : TStrings;
begin
 GUIShared.HTMLSaveDialog.FileName := 'avz_services.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then begin
  RepLines := TStringList.Create;
  RepLines.AddStrings(HTML_StartLines);
  RepLines.AddStrings(HTML_BODYLines);
  RepLines.Add(TranslateStr('<H2 align=center>$AVZ1002</H2>'));
  CreateHTMLReport(RepLines, false, DiverMode);
  RepLines.Add('</BODY></HTML>');
  RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
  ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  RepLines.Free;
 end;
end;

procedure TServiceView.sbCopyProcToQurantineClick(Sender: TObject);
var
 FileName : string;
begin
 if pcService.ActivePage = tsServiceByAPI then begin
  if dgService.Row - 1 < Length(ServiceManager.ServiceArray) then begin
   FileName := ServiceManager.NormalServiceFile(ServiceManager.ServiceArray[dgService.Row - 1].BinFile, '.exe');
   if zMessageDlg('$AVZ0359 '+FileName+' $AVZ0090 ?',
        mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
   CopyToInfected(FileName, '$AVZ0987', 'Quarantine');
  end;
 end;
 if pcService.ActivePage = tsServiceByReg then begin
  if dgServiceByReg.Row - 1 < Length(ServiceManager.ServiceArray) then begin
   FileName := ServiceManager.NormalServiceFile(ServiceManager.ServiceArray[dgServiceByReg.Row - 1].BinFile, '.exe');
   if zMessageDlg('$AVZ0359 '+FileName+' $AVZ0090 ?',
        mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
   CopyToInfected(FileName, '$AVZ0987', 'Quarantine');
  end;
 end;
end;

procedure TServiceView.sbCopyAllToQurantineClick(Sender: TObject);
begin
 if zMessageDlg('$AVZ0367',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TServiceView.dgServiceClick(Sender: TObject);
begin
 if dgService.Row - 1 < Length(ServiceManager.ServiceArray) then begin
  sbCopyProcToQurantine.Enabled := not (ServiceManager.ServiceArray[dgService.Row - 1].CheckResult = 0);
 end else sbCopyProcToQurantine.Enabled := false;
end;

procedure TServiceView.sbStartServiceClick(Sender: TObject);
begin
 if dgService.Row - 1 < Length(ServiceManager.ServiceArray) then
  if zMessageDlg('$AVZ0169 '+ServiceManager.ServiceArray[dgService.Row - 1].Name, mtConfirmation, [mbYes, mbNo], 0) = mrYes then
   if ServiceManager.StartService(ServiceManager.ServiceArray[dgService.Row - 1].Name) then begin
    zMessageDlg('$AVZ1000 '+ServiceManager.ServiceArray[dgService.Row - 1].Name+' $AVZ1103 ', mtInformation, [mbOk], 0);
    CreateServiceList;
   end;
end;

procedure TServiceView.mbStopServiceClick(Sender: TObject);
begin
 if dgService.Row - 1 < Length(ServiceManager.ServiceArray) then
  if zMessageDlg('$AVZ0170 '+ServiceManager.ServiceArray[dgService.Row - 1].Name, mtConfirmation, [mbYes, mbNo], 0) = mrYes then
   if ServiceManager.StopService(ServiceManager.ServiceArray[dgService.Row - 1].Name) then begin
    zMessageDlg('$AVZ1000 '+ServiceManager.ServiceArray[dgService.Row - 1].Name+' $AVZ0602', mtInformation, [mbOk], 0);
    CreateServiceList;
   end;
end;

procedure TServiceView.mbKillServiceClick(Sender: TObject);
var
 Reg : TRegistry;
begin
 if pcService.ActivePage = tsServiceByAPI then begin
   if dgService.Row - 1 < Length(ServiceManager.ServiceArray) then begin
    if ServiceManager.ServiceArray[dgService.Row - 1].CheckResult = 0 then begin
     zMessageDlg('$AVZ1000 '+ServiceManager.ServiceArray[dgService.Row - 1].Name+' $AVZ0585', mtError, [mbOk], 0);
     exit;
    end;
    if zMessageDlg('$AVZ0171 '+ServiceManager.ServiceArray[dgService.Row - 1].Name+'. $AVZ0123', mtConfirmation, [mbYes, mbNo], 0) = mrYes then begin
     if ServiceManager.DeleteService(ServiceManager.ServiceArray[dgService.Row - 1].Name) then
      zMessageDlg('$AVZ1000 '+ServiceManager.ServiceArray[dgService.Row - 1].Name+' $AVZ1070', mtInformation, [mbOk], 0);
     Reg := TRegistry.Create;
     Reg.RootKey := HKEY_LOCAL_MACHINE;
     Reg.DeleteKey('SYSTEM\CurrentControlSet\Services\'+ServiceManager.ServiceArray[dgService.Row - 1].Name);
     Reg.Free;
     CreateServiceList;
    end;
   end;
 end;
 if pcService.ActivePage = tsServiceByReg then begin
   if dgServiceByReg.Row - 1 < Length(ServiceManager.ServiceArray) then begin
    if ServiceManager.ServiceArray[dgServiceByReg.Row - 1].CheckResult = 0 then begin
     zMessageDlg('$AVZ1000 '+ServiceManager.ServiceArray[dgServiceByReg.Row - 1].Name+' $AVZ0585', mtError, [mbOk], 0);
     exit;
    end;
    if zMessageDlg('$AVZ0171 '+ServiceManager.ServiceArray[dgServiceByReg.Row - 1].Name+'. $AVZ0123', mtConfirmation, [mbYes, mbNo], 0) = mrYes then begin
     if ServiceManager.DeleteService(ServiceManager.ServiceArray[dgServiceByReg.Row - 1].Name) then
      zMessageDlg('$AVZ1000 '+ServiceManager.ServiceArray[dgServiceByReg.Row - 1].Name+' $AVZ1070', mtInformation, [mbOk], 0);
     Reg := TRegistry.Create;
     Reg.RootKey := HKEY_LOCAL_MACHINE;
     Reg.DeleteKey('SYSTEM\CurrentControlSet\Services\'+ServiceManager.ServiceArray[dgServiceByReg.Row - 1].Name);
     Reg.Free;
     CreateServiceList;
    end;
   end;
 end;
end;

procedure TServiceView.dgServiceFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 SortKey := ACol;
 ServiceManager.SortData(SortKey);
 dgService.Repaint;
 dgServiceByReg.Repaint;
end;

function TServiceView.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to Length(ServiceManager.ServiceArray) - 1 do
  if ServiceManager.ServiceArray[i].CheckResult <> 0 then
   CopyToInfected(ServiceManager.NormalServiceFile(ServiceManager.ServiceArray[i].BinFile, '.exe'), '$AVZ0977', 'Quarantine');
end;

function TServiceView.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 TagName, FileInfoStr : string;
begin
 if ServiceView.cbServiceMode.ItemIndex = 0 then
   TagName := 'Service' else TagName := 'Drivers';
 ALines.Add(' <'+TagName+'>');
 ZProgressClass.AddProgressMax(Length(ServiceManager.ServiceArray));
 for i := 0 to Length(ServiceManager.ServiceArray)-1 do begin
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles and (ServiceManager.ServiceArray[i].CheckResult = 0)) then begin
   FileInfoStr := FormatXMLFileInfo(ServiceManager.NormalServiceFile(ServiceManager.ServiceArray[i].BinFile, '.exe'));
    ALines.Add('  <ITEM File="'+XMLStr(ServiceManager.ServiceArray[i].BinFile)+'" '+
                        'Name="'+XMLStr(ServiceManager.ServiceArray[i].Name)+'" '+
                        'CheckResult="'+IntToStr(ServiceManager.ServiceArray[i].CheckResult)+'" '+
                        'Type="'+IntToStr(ServiceManager.ServiceArray[i].dwServiceType)+'" '+
                        'State="'+IntToStr(ServiceManager.ServiceArray[i].dwCurrentState)+'" '+
//                        'Descr="'+XMLStr(ServiceManager.ServiceArray[i].FileDescr)+'" '+
//                        'LegalCopyright="'+XMLStr(ServiceManager.ServiceArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
  end;
 end;
 ALines.Add(' </'+TagName+'>');
end;

function TServiceView.GetDriverMode: boolean;
begin
 Result := cbServiceMode.ItemIndex = 1;
end;

end.
