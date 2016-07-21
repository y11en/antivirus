unit uHostsManager;
// ****************** Менеджер файла Hosts **********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, Grids, RXGrids, zHostsFile,
  zHTMLTemplate, zSharedFunctions;

type
  THostsManager = class(TForm)
    dgHosts: TRxDrawGrid;
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    mbDeleteItem: TBitBtn;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgHostsDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
  public
   HostsFile : THostsFile;
   function FormatHtmlTxt(S : string) : string;
   // Обновить список расширений
   Function RefreshInfo : boolean;
   // Выполнить диалог
   Function Execute : boolean;
   // Создать HTML отчет
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создать XML отчет
   function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
  end;

 function ExecuteHostsManager : boolean;
var
  HostsManager: THostsManager = nil;

implementation
uses Math, uGUIShared, zTranslate;

{$R *.dfm}

function ExecuteHostsManager : boolean;
begin
 try
  HostsManager := nil;
  HostsManager := THostsManager.Create(nil);
  Result := HostsManager.Execute;
 finally
  HostsManager.Free;
  HostsManager := nil;
 end;
end;

function THostsManager.Execute: boolean;
begin
 RefreshInfo;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure THostsManager.FormCreate(Sender: TObject);
begin
 HostsFile :=  THostsFile.Create;
 TranslateForm(Self);
end;

procedure THostsManager.FormDestroy(Sender: TObject);
begin
 HostsFile.Free;
end;

function THostsManager.RefreshInfo: boolean;
begin
 HostsFile.LoadHostFile;
 if Length(HostsFile.HostsFileRecArray) > 0 then
  dgHosts.RowCount := Length(HostsFile.HostsFileRecArray) + 1
   else dgHosts.RowCount := 2;
end;

procedure THostsManager.dgHostsDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : THostsFileRec;
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
  if ARow - 1 < Length(HostsFile.HostsFileRecArray) then begin
   Tmp := HostsFile.HostsFileRecArray[ARow - 1];
   if Tmp.IsComment  then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clGrayText;
  case ACol of
    0 : begin
         S := Tmp.Str;
        end;
   end;
   if S <> '' then
    (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

function THostsManager.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 S : string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0272');
 for i := 0 to length(HostsFile.HostsFileRecArray) - 1 do begin
  S := 'bgColor='+HTMP_BgColor2;
  if not(HostsFile.HostsFileRecArray[i].IsComment) then begin
   ALines.Add('<TR '+S+'>'+
              '<TD><pre><code>'+FormatHtmlTxt(HostsFile.HostsFileRecArray[i].Str)
               );

  end;
 end;
// ALines.Add('<TR bgColor='+HTMP_BgColor1+'><TD colspan=7> Обнаружено элементов - '+IntToStr(length(PrintExtList.PrintExtInfoArray))+', опознано как безопасные - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure THostsManager.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ1112</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_hosts.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

procedure THostsManager.mbDeleteItemClick(Sender: TObject);
begin
 if dgHosts.Row - 1 < length(HostsFile.HostsFileRecArray) then begin
  if zMessageDlg('$AVZ1091 "'+HostsFile.HostsFileRecArray[dgHosts.Row - 1].Str+'" ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  if not(HostsFile.DeleteItem(dgHosts.Row - 1) )then exit;
  HostsFile.SaveHostFile;
  RefreshInfo;
 end;
end;

function THostsManager.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
begin
 Result := false;
 ALines.Add(' <HOSTS>');
 for i := 0 to length(HostsFile.HostsFileRecArray) - 1 do begin
  if not(HostsFile.HostsFileRecArray[i].IsComment) then begin
   if Trim(HostsFile.HostsFileRecArray[i].Str) <> '' then
    ALines.Add('  <ITEM Line="'+XMLStr(HostsFile.HostsFileRecArray[i].Str)+'" />');
  end;
 end;
 ALines.Add(' </HOSTS>');
end;

function THostsManager.FormatHtmlTxt(S: string): string;
var
 i : integer;
begin
 Result := '';
 for i := 1 to Length(S) do
  case S[i] of
   '<' : Result := Result + '&lt;';
   '>' : Result := Result + '&gt;';
   '&' : Result := Result + '&amp;';
   else
    Result := Result + S[i];
  end;
end;

end.
