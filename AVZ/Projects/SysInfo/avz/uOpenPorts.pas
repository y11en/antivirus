unit uOpenPorts;
// **************** Менеджер открытых портов **********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, Grids, RXGrids, ComCtrls,  zIPPorts, ExtCtrls,
  winsock, zHTMLTemplate, zSharedFunctions, zAVZKernel, zTranslate;

type
  TNetOpenPorts = class(TForm)
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    dgUDPList: TRxDrawGrid;
    dgTCPList: TRxDrawGrid;
    Timer1: TTimer;
    Panel1: TPanel;
    mbRefresh: TBitBtn;
    SpeedButton2: TSpeedButton;
    procedure dgTCPListDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgUDPListDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure mbRefreshClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure SpeedButton2Click(Sender: TObject);
  private
  public
    IPPorts : TIPPorts;
    function RefresConnections : boolean;
    function Execute : boolean;
    // Создать HTML отчет
    function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
    // Добавить все неопознанные файлы в карантин
    function AddAllToQurantine: boolean;
  end;

  function  ExecuteNetOpenPorts : boolean;
var
  NetOpenPorts: TNetOpenPorts;

implementation

uses uGUIShared;

{$R *.dfm}

function  ExecuteNetOpenPorts : boolean;
begin
 try
  NetOpenPorts := TNetOpenPorts.Create(nil);
  Result := NetOpenPorts.Execute;
 finally
  NetOpenPorts.Free;
  NetOpenPorts := nil;
 end;
end;
{ TForm2 }

function TNetOpenPorts.RefresConnections: boolean;
begin
 try
  Screen.Cursor := crHourGlass;
  // Обновление данных
  try
   IPPorts.Refresh;
  except
  end;
  // Сортировка таблиц
  IPPorts.SotrTable(IPPorts.TCPConnections, 1);
  IPPorts.SotrTable(IPPorts.UDPConnections, 1);
  // Настройка размерности таблиц
  if Length(IPPorts.TCPConnections) > 0 then
   dgTCPList.RowCount := Length(IPPorts.TCPConnections) + 1
    else dgTCPList.RowCount := 2;
  if Length(IPPorts.UDPConnections) > 0 then
   dgUDPList.RowCount := Length(IPPorts.UDPConnections) + 1
    else dgUDPList.RowCount := 2;
  // Перерисовка интерфейса
  Refresh;
 finally
  Screen.Cursor := crDefault;
 end;
end;

function TNetOpenPorts.Execute: boolean;
begin
 // Загрузка базы портов
 try
  IPPorts.LoadPortDescr(ExtractFilePath(Application.ExeName)+'Base\ports.avz');
 except end;
 RefresConnections;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TNetOpenPorts.dgTCPListDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 IPConnection : TIPConnection;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0783';
   1 : S := '$AVZ1051';
   2 : S := 'Remote Host';
   3 : S := 'Remote Port';
   4 : S := '$AVZ0797';
   5 : S := '$AVZ0581';
   6 : S := '$AVZ0331';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  if ARow > Length(IPPorts.TCPConnections) then exit;
  IPConnection :=  IPPorts.TCPConnections[ARow-1];
  S := '';
  case ACol of
   0 : S := IntToStr(IPConnection.LocalPort) + ' ';
   1 : S := IPConnection.State;
   2 : begin
        S := IPConnection.RemoteHost;
        if S = '0.0.0.0' then S := '--';
       end;
   3 : begin
        S := IntToStr(IPConnection.RemotePort);
        if IPConnection.RemoteHost = '0.0.0.0' then S := '--';
       end;
   4 : S := IPConnection.ProcessBin;
   5 : if IPConnection.State = 'LISTENING' then S := IPConnection.Prim else S := '';
   6 : S := IPPorts.DecodeCatBits(IPConnection.Flags);
  end;
  if (IPConnection.State = 'LISTENING') then begin
   (Sender as TRxDrawGrid).Canvas.Font.Color := clBlack;
   (Sender as TRxDrawGrid).Canvas.Font.Style := [];
   if (ARow > 2) and ((IPConnection.Flags and not(7)) > 0) then begin
    (Sender as TRxDrawGrid).Canvas.Font.Color := clRed;
    (Sender as TRxDrawGrid).Canvas.Font.Style := [fsBold];
   end;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TNetOpenPorts.dgUDPListDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 IPConnection : TIPConnection;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0783';
   1 : S := '$AVZ0797';
   2 : S := '$AVZ0581';
   3 : S := '$AVZ0331';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  if ARow > Length(IPPorts.UDPConnections) then exit;
  IPConnection := IPPorts.UDPConnections[ARow-1];
  S := '';
  case ACol of
   0 : S := IntToStr(IPConnection.LocalPort);
   1 : S := IPConnection.ProcessBin;
   2 : S := IPConnection.Prim;
   3 : S := IPPorts.DecodeCatBits(IPConnection.Flags);
  end;
  (Sender as TRxDrawGrid).Canvas.Font.Color := clBlack;
  (Sender as TRxDrawGrid).Canvas.Font.Style := [];
  if (ARow > 2) and ((IPConnection.Flags and not(7)) > 0) then begin
   (Sender as TRxDrawGrid).Canvas.Font.Color := clRed;
   (Sender as TRxDrawGrid).Canvas.Font.Style := [fsBold];
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TNetOpenPorts.FormCreate(Sender: TObject);
begin
 IPPorts := TIPPorts.Create;
 TranslateForm(Self);
end;

procedure TNetOpenPorts.FormDestroy(Sender: TObject);
begin
 IPPorts.Free;
 IPPorts := nil;
end;

procedure TNetOpenPorts.mbRefreshClick(Sender: TObject);
begin
 RefresConnections;
end;

procedure TNetOpenPorts.Timer1Timer(Sender: TObject);
begin
 RefresConnections;
end;

function TNetOpenPorts.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to Length(IPPorts.TCPConnections) - 1 do
  if IPPorts.TCPConnections[i].ProcessBin <> '' then
   CopyToInfected(IPPorts.TCPConnections[i].ProcessBin, '$AVZ0978', 'Quarantine');;
 for i := 0 to Length(IPPorts.UDPConnections) - 1 do
  if IPPorts.UDPConnections[i].ProcessBin <> '' then
   CopyToInfected(IPPorts.UDPConnections[i].ProcessBin, '$AVZ0979', 'Quarantine');;
end;

function TNetOpenPorts.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, FileInfoStr : string;
begin
 GoodFiles := 0;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0783'+HTML_TableHeaderTD+'$AVZ1051'+HTML_TableHeaderTD+'Remote Host'+HTML_TableHeaderTD+'Remote Port'+HTML_TableHeaderTD+'$AVZ0797'+HTML_TableHeaderTD+'$AVZ0802');
 ALines.Add('<TR bgColor='+HTMP_BgColorBg+'><TD colspan=7><b>$AVZ0784');
 for i := 0 to Length(IPPorts.TCPConnections)-1 do begin
  if IPPorts.TCPConnections[i].CheckResult = 0 then begin
   S := 'bgColor='+HTMP_BgColorTrusted;
   inc(GoodFiles);
  end else S :=  'bgColor='+HTMP_BgColor2;
  if not(HideGoodFiles and (IPPorts.TCPConnections[i].CheckResult = 0)) then begin
   FileInfoStr := FormatFileInfo(IPPorts.TCPConnections[i].ProcessBin, ', ');
   ALines.Add('<TR '+S+'><TD>'+HTMLNVL(IntToStr(IPPorts.TCPConnections[i].LocalPort))+
                  '<TD>'+HTMLNVL(IPPorts.TCPConnections[i].State)+
                  '<TD>'+HTMLNVL(IPPorts.TCPConnections[i].RemoteHost)+
                  '<TD>'+HTMLNVL(IntToStr(IPPorts.TCPConnections[i].RemotePort))+
                  '<TD>'+'<a href="" title="'+FileInfoStr+'">'+'['+Inttostr(IPPorts.TCPConnections[i].ProcessID)+'] '+HTMLNVL(IPPorts.TCPConnections[i].ProcessBin)+'</a>'+GenScriptTxt(IPPorts.TCPConnections[i].ProcessBin, true)+
                  '<TD>'+HTMLNVL(IPPorts.TCPConnections[i].Prim));
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorBg+'><TD colspan=7><b>$AVZ0786');
 for i := 0 to Length(IPPorts.UDPConnections)-1 do begin
  if IPPorts.UDPConnections[i].CheckResult = 0 then begin
   S := 'bgColor='+HTMP_BgColorTrusted;
   inc(GoodFiles);
  end else S :=  'bgColor='+HTMP_BgColor2;
  if not(HideGoodFiles and (IPPorts.UDPConnections[i].CheckResult = 0)) then begin
   FileInfoStr := FormatFileInfo(IPPorts.UDPConnections[i].ProcessBin, ', ');
   ALines.Add('<TR '+S+'><TD>'+HTMLNVL(IntToStr(IPPorts.UDPConnections[i].LocalPort))+
                 '<TD>'+HTMLNVL(IPPorts.UDPConnections[i].State)+
                 '<TD>'+HTMLNVL('--')+
                 '<TD>'+HTMLNVL('--')+
                 '<TD>'+'<a href="" title="'+FileInfoStr+'">'+'['+Inttostr(IPPorts.UDPConnections[i].ProcessID)+'] '+HTMLNVL(IPPorts.UDPConnections[i].ProcessBin)+'</a>'+GenScriptTxt(IPPorts.UDPConnections[i].ProcessBin, true)+
                 '<TD>'+HTMLNVL(TranslateStr(IPPorts.UDPConnections[i].Prim)));
  end;
 end;
// ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7>Обнаружено - '+IntToStr(Length(IPPorts.TCPConnections))+', опознано как безопасные - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TNetOpenPorts.SpeedButton2Click(Sender: TObject);
var
 RepLines : TStrings;
begin
 GUIShared.HTMLSaveDialog.FileName := 'avz_ports.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then begin
  RepLines := TStringList.Create;
  RepLines.AddStrings(HTML_StartLines);
  RepLines.AddStrings(HTML_BODYLines);
  RepLines.Add(TranslateStr('<H2 align=center>$AVZ0785</H2>'));
  CreateHTMLReport(RepLines, false);
  RepLines.Add('</BODY></HTML>');
  RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
  ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  RepLines.Free;
 end;
end;

end.
