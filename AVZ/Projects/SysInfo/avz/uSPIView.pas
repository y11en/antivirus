unit uSPIView;
// ******************** Менеджер SLI/LSP *******************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, winsock, Grids, RXGrids, zSharedFunctions, registry, StdCtrls,
  Buttons, ComCtrls, zLSP, ExtCtrls, zHTMLTemplate, zAVKernel, zAVZKernel;

type

  TSPIView = class(TForm)
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    dgLSPList: TRxDrawGrid;
    mbDeleteNameSpace: TBitBtn;
    BitBtn2: TBitBtn;
    BitBtn3: TBitBtn;
    dgProtocolList: TRxDrawGrid;
    TabSheet3: TTabSheet;
    BitBtn7: TBitBtn;
    mmLSPErrors: TMemo;
    mbFullAutoRepair: TBitBtn;
    BitBtn1: TBitBtn;
    BitBtn5: TBitBtn;
    Panel1: TPanel;
    mbRefresh: TBitBtn;
    mbCreateHTMLLog: TBitBtn;
    procedure FormCreate(Sender: TObject);
    procedure dgLSPListDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure BitBtn2Click(Sender: TObject);
    procedure dgProtocolListDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure FormDestroy(Sender: TObject);
    procedure mbRefreshClick(Sender: TObject);
    procedure mbDeleteNameSpaceClick(Sender: TObject);
    procedure BitBtn7Click(Sender: TObject);
    procedure mbFullAutoRepairClick(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn5Click(Sender: TObject);
    procedure mbCreateHTMLLogClick(Sender: TObject);
  private
  public
   LSPManager : TLSPManager;
   procedure ResreshLists;
   // Поиск бинарного файла по реестру
   function SearchLSPFileByRegistry(ANameSpaceCatalog : string; AGUID : TGUID) : string;
   // Выполнение диалога
   function Execute : boolean;
   // Формирование HTML отчета
   Function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
  end;

var
  SPIView: TSPIView;

// Выполнение в режиме диалога
Function ExecuteLSPView : boolean;

implementation

uses uGUIShared, zTranslate;

{$R *.dfm}

function TSPIView.Execute: boolean;
begin
 ResreshLists;
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TSPIView.FormCreate(Sender: TObject);
begin
 LSPManager := TLSPManager.Create;
 TranslateForm(Self);
end;

Function ExecuteLSPView : boolean;
begin
 try
  SPIView := TSPIView.Create(nil);
  SPIView.Execute;
 finally
  SPIView.Free;
  SPIView := nil;
 end;
end;

procedure TSPIView.dgLSPListDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
 NameSpaceInfo : TNameSpaceInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0450';
   1 : S := '$AVZ1051';
   2 : S := '$AVZ0318';
   3 : S := '$AVZ0582';
   4 : S := '$AVZ0014';
   5 : S := '$AVZ0929';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  NameSpaceInfo := LSPManager.NameSpaceListReg[ARow-1];
  S := '';
  case ACol of
   0 : S := NameSpaceInfo.lpszIdentifier;
   1 : if NameSpaceInfo.fActive then S := TranslateStr('$AVZ0052') else S := TranslateStr('$AVZ0468');
   2 : S := NameSpaceInfo.BinFileName;
   3 : S := NameSpaceInfo.BinFileInfo;
   4 : S := GUIDToString( NameSpaceInfo.NSProviderId);
  end;
  if not NameSpaceInfo.BinFileExists then
   (Sender as TRxDrawGrid).Canvas.Font.Color := clRed;
  if NameSpaceInfo.CheckResults = 0 then
   (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TSPIView.BitBtn2Click(Sender: TObject);
begin
 if dgLSPList.Row > Length(LSPManager.NameSpaceListReg) then exit;
 LSPManager.SetNameSpaceActive(dgLSPList.Row - 1, false);
 ResreshLists;
end;

procedure TSPIView.dgProtocolListDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 ProtocolInfo : TProtocolInfo;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0787';
   1 : S := '$AVZ0318';
   2 : S := '$AVZ0582';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  ProtocolInfo := LSPManager.ProtocolListReg[ARow-1];
  case ACol of
   0 : S := ProtocolInfo.lpszIdentifier;
   1 : S := ProtocolInfo.BinFileName;
   2 : S := ProtocolInfo.BinFileInfo;
  end;
 if not ProtocolInfo.BinFileExists then
  (Sender as TRxDrawGrid).Canvas.Font.Color := clRed;
 if ProtocolInfo.CheckResults = 0 then
  (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
 (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left, Rect.Top, S);
 end;
end;

function TSPIView.SearchLSPFileByRegistry(ANameSpaceCatalog: string;
  AGUID: TGUID): string;
begin
 ;
end;

procedure TSPIView.FormDestroy(Sender: TObject);
begin
 LSPManager.Free;
 LSPManager := NIL;
end;

procedure TSPIView.mbRefreshClick(Sender: TObject);
begin
 ResreshLists;
end;

procedure TSPIView.mbDeleteNameSpaceClick(Sender: TObject);
begin
 if dgLSPList.Row > Length(LSPManager.NameSpaceListReg) then exit;
 if zMessageDlg('$AVZ0166 "'+
               LSPManager.NameSpaceListReg[dgLSPList.Row - 1].lpszIdentifier+'"',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 // Удаление
 LSPManager.DeleteNameSpaceByGUID(LSPManager.NameSpaceListReg[dgLSPList.Row - 1].NSProviderId);
 ResreshLists;
end;

procedure TSPIView.ResreshLists;
var
 Res, i : integer;
begin
 mmLSPErrors.Lines.Clear;
  // Поиск ошибок
 Res := LSPManager.SearchErrors(mmLSPErrors.Lines);
 if Res > 0 then begin
  mmLSPErrors.Lines.Add(TranslateStr('$AVZ0117 - ')+IntToStr(Res));
  mmLSPErrors.Lines.Add(TranslateStr('$AVZ0132'));
 end else begin
  mmLSPErrors.Lines.Add(TranslateStr('$AVZ0463'));
 end;
 // Обновление данных
 LSPManager.Refresh;
 // Проверка найденных файлов
 for i := 0 to Length(LSPManager.NameSpaceListWS) - 1 do
  if LSPManager.NameSpaceListWS[i].BinFileExists then
   LSPManager.NameSpaceListWS[i].CheckResults := FileSignCheck.CheckFile(LSPManager.NameSpaceListWS[i].BinFileName);
 for i := 0 to Length(LSPManager.NameSpaceListReg) - 1 do
  if LSPManager.NameSpaceListReg[i].BinFileExists then
   LSPManager.NameSpaceListReg[i].CheckResults := FileSignCheck.CheckFile(LSPManager.NameSpaceListReg[i].BinFileName);
 for i := 0 to Length(LSPManager.ProtocolListReg) - 1 do
  if LSPManager.ProtocolListReg[i].BinFileExists then
   LSPManager.ProtocolListReg[i].CheckResults := FileSignCheck.CheckFile(LSPManager.ProtocolListReg[i].BinFileName);
 // Настройка таблиц
 dgLSPList.RowCount      := length(LSPManager.NameSpaceListReg) + 1;
 dgProtocolList.RowCount := length(LSPManager.protocolListReg) + 1;
 dgLSPList.Refresh;
 dgProtocolList.Refresh;
 Refresh;
 Repaint;
end;

procedure TSPIView.BitBtn7Click(Sender: TObject);
begin
 if dgProtocolList.Row > Length(LSPManager.ProtocolListReg) then exit;
 if zMessageDlg('$AVZ0167 "'+
               LSPManager.ProtocolListReg[dgProtocolList.Row - 1].lpszIdentifier+'"',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 LSPManager.DeleteProtocolByID(dgProtocolList.Row - 1);
 ResreshLists;
end;

procedure TSPIView.mbFullAutoRepairClick(Sender: TObject);
begin
 LSPManager.FullAutoRepair(GetQuarantineDirName('BackUp'));
 ResreshLists;
end;

procedure TSPIView.BitBtn3Click(Sender: TObject);
begin
 if dgLSPList.Row > Length(LSPManager.NameSpaceListReg) then exit;
 LSPManager.SetNameSpaceActive(dgLSPList.Row - 1, true);
 ResreshLists;
end;

procedure TSPIView.BitBtn1Click(Sender: TObject);
var
 FileName : string;
begin
 if dgLSPList.Row > Length(LSPManager.NameSpaceListReg) then exit;
 FileName := LSPManager.NameSpaceListReg[dgLSPList.Row - 1].BinFileName;
 CopyToInfected(FileName, '$AVZ0982', 'Quarantine');
 zMessageDlg('$AVZ1109 '+FileName+' $AVZ0980', mtConfirmation, [mbOk], 0);
end;

procedure TSPIView.BitBtn5Click(Sender: TObject);
var
 FileName : string;
begin
 if dgProtocolList.Row > Length(LSPManager.ProtocolListReg) then exit;
 FileName := LSPManager.ProtocolListReg[dgProtocolList.Row - 1].BinFileName;
 CopyToInfected(FileName, '$AVZ0983', 'Quarantine');
 zMessageDlg('$AVZ1109 '+FileName+' $AVZ0980', mtConfirmation, [mbOk], 0);
end;

procedure TSPIView.mbCreateHTMLLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0027</H2>'));
 LSPManager.CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_spi.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TSPIView.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
begin
 LSPManager.CreateHTMLReport(ALines, HideGoodFiles);
end;

end.
