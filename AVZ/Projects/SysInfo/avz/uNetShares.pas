unit uNetShares;
// ****************** Менеджер сетевых ресурсов *******************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, SystemMonitor, StdCtrls, Buttons, ExtCtrls, Grids, RXGrids,
  ComCtrls,Math;

type

 TNetShares = class(TForm)
    Panel1: TPanel;
    BitBtn4: TBitBtn;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    TabSheet3: TTabSheet;
    dgNetShares: TRxDrawGrid;
    dgNetSessions: TRxDrawGrid;
    dgNetFiles: TRxDrawGrid;
    Timer1: TTimer;
    Panel2: TPanel;
    mbCloseSession: TButton;
    Panel3: TPanel;
    mbCloaseFile: TButton;
    procedure FormCreate(Sender: TObject);
    procedure dgNetSharesDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgNetSessionsDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure Timer1Timer(Sender: TObject);
    procedure dgNetFilesDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure FormDestroy(Sender: TObject);
    procedure dgNetSharesFixedCellClick(Sender: TObject; ACol,
      ARow: Integer);
    procedure dgNetSessionsFixedCellClick(Sender: TObject; ACol,
      ARow: Integer);
    procedure dgNetFilesFixedCellClick(Sender: TObject; ACol,
      ARow: Integer);
    procedure mbCloseSessionClick(Sender: TObject);
    procedure mbCloaseFileClick(Sender: TObject);
  public
   NetShareInfoArray   : TNetShareInfoArray;
   NetSessionInfoArray : TNetSessionInfoArray;
   NetFileInfoArray    : TNetFileInfoArray;
   // Ключи сортировки
   NetShareOrderKey, NetSessionOrderKey, NetFileInfoOrderKey : byte;
   MSNetInfo           : TCustomMSNetInfo;
   // Обновление списков
   Function RefreshInfo : boolean;
   // Выполнение диалога
   Function Execute : boolean;
  end;

var
  NetShares: TNetShares;

function ExecuteNetSharesDLG : boolean;
implementation
uses zTranslate;
{$R *.dfm}

{ TNetShares }
function TNetShares.Execute: boolean;
begin
 RefreshInfo;
 ShowModal;
 ModalResult := mrOk;
end;


procedure TNetShares.FormCreate(Sender: TObject);
begin
 NetShareInfoArray   := nil;
 NetSessionInfoArray := nil;
 NetFileInfoArray    := nil;
 NetShareOrderKey    := 0;
 NetSessionOrderKey  := 0;
 NetFileInfoOrderKey := 0;
 TranslateForm(Self);
 // Создание информационного класса
 if ISNT then
  MSNetInfo := TNTMSNetInfo.Create
   else MSNetInfo := T9xMSNetInfo.Create;
end;


function TNetShares.RefreshInfo: boolean;
begin
 // Создание списка расшаренных ресурсов
 MSNetInfo.CreateShareList(NetShareInfoArray);
 MSNetInfo.OrderList(NetShareInfoArray, NetShareOrderKey);
 // Создание списка сессий
 MSNetInfo.CreateSessionsList(NetSessionInfoArray);
 MSNetInfo.OrderList(NetSessionInfoArray, NetSessionOrderKey);
 // Создание списка открытых файлов
 MSNetInfo.CreateOpenResList(NetFileInfoArray);
 MSNetInfo.OrderList(NetFileInfoArray, NetFileInfoOrderKey);
 // Настройка количества столбцов в таблице
 dgNetShares.RowCount   := max(Length(NetShareInfoArray) + 1, 2);
 dgNetShares.Refresh;
 dgNetSessions.RowCount := max(Length(NetSessionInfoArray) + 1, 2);
 dgNetSessions.Refresh;
 dgNetFiles.RowCount    := max(Length(NetFileInfoArray) + 1, 2);
 dgNetFiles.Refresh;
end;

procedure TNetShares.dgNetSharesDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
begin
 S := '';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0948';
   1 : S := '$AVZ0883';
   2 : S := '$AVZ0802';
   3 : S := '$AVZ0012';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow-1 < Length(NetShareInfoArray) then
  case ACol of
   0 : S := NetShareInfoArray[ARow - 1].NetName;
   1 : S := NetShareInfoArray[ARow - 1].Path;
   2 : S := NetShareInfoArray[ARow - 1].Remark;
   3 : S := IntToStr(NetShareInfoArray[ARow - 1].CurUsers);
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TNetShares.dgNetSessionsDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
begin
 S := '';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0780';
   1 : S := '$AVZ0353';
   2 : S := '$AVZ0002';
   3 : S := '$AVZ0153';
   4 : S := '$AVZ0152';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow-1 < Length(NetSessionInfoArray) then
  case ACol of
   0 : S := NetSessionInfoArray[ARow - 1].UserName;
   1 : S := NetSessionInfoArray[ARow - 1].SessionName;
   2 : S := IntToStr(NetSessionInfoArray[ARow - 1].NumOpens);
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TNetShares.Timer1Timer(Sender: TObject);
begin
 RefreshInfo;
 dgNetShares.Invalidate;
 dgNetShares.Repaint;
end;

procedure TNetShares.dgNetFilesDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
begin
 S := '';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0608';
   1 : S := '$AVZ0780';
   2 : S := '$AVZ0948';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow-1 < Length(NetFileInfoArray) then
  case ACol of
   0 : S := NetFileInfoArray[ARow - 1].PathName;
   1 : S := NetFileInfoArray[ARow - 1].UserName;
   2 : S := NetFileInfoArray[ARow - 1].ShareName;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
 end;
end;

procedure TNetShares.FormDestroy(Sender: TObject);
begin
 MSNetInfo.Free;
end;

procedure TNetShares.dgNetSharesFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 case ACol of
  0 : NetShareOrderKey := 0;
  1 : NetShareOrderKey := 2;
  2 : NetShareOrderKey := 1;
  3 : NetShareOrderKey := 3;
 end;
 RefreshInfo;
end;

procedure TNetShares.dgNetSessionsFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 case ACol of
  0 : NetSessionOrderKey := 1;
  1 : NetSessionOrderKey := 0;
  2 : NetSessionOrderKey := 2;
  3 : NetSessionOrderKey := 3;
  4 : NetSessionOrderKey := 4;
 end;
 RefreshInfo;
end;

procedure TNetShares.dgNetFilesFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 case ACol of
  0 : NetFileInfoOrderKey := 3;
  1 : NetFileInfoOrderKey := 4;
  2 : NetFileInfoOrderKey := 5;
 end;
 RefreshInfo;
end;

function ExecuteNetSharesDLG : boolean;
begin
 try
  NetShares := TNetShares.Create(nil);
  NetShares.Execute;
 finally
  NetShares.Free;
  NetShares := nil;
 end;
end;

procedure TNetShares.mbCloseSessionClick(Sender: TObject);
begin
 if (dgNetSessions.Row > 0) and (dgNetSessions.Row-1 < Length(NetSessionInfoArray)) then
  if MSNetInfo.DelSession(NetSessionInfoArray[dgNetSessions.Row-1]) then
   zMessageDlg('$AVZ0943', mtInformation, [mbOk], 0);
 RefreshInfo;
end;

procedure TNetShares.mbCloaseFileClick(Sender: TObject);
begin
 if (dgNetFiles.Row > 0) and (dgNetFiles.Row-1 < Length(NetFileInfoArray)) then
  if MSNetInfo.CloseRes(NetFileInfoArray[dgNetFiles.Row-1]) then
   zMessageDlg('$AVZ1126', mtInformation, [mbOk], 0);
 RefreshInfo;
end;

end.
