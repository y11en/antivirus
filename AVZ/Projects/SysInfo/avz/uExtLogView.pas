unit uExtLogView;
// Расширенный просмотр протокола
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Grids, RXGrids, ExtCtrls, StdCtrls, Buttons, Menus, uFileDeleteModeDLG,
  zAVKernel, zAVZKernel;

type
  TExtLogView = class(TForm)
    dgLog: TRxDrawGrid;
    Panel1: TPanel;
    mbCopyToQurantine: TBitBtn;
    mbDelete: TBitBtn;
    BitBtn4: TBitBtn;
    PopupMenu1: TPopupMenu;
    N1: TMenuItem;
    N2: TMenuItem;
    N3: TMenuItem;
    N4: TMenuItem;
    N5: TMenuItem;
    Keylogger1: TMenuItem;
    Timer1: TTimer;
    N6: TMenuItem;
    N7: TMenuItem;
    N8: TMenuItem;
    N9: TMenuItem;
    mbSaveAsCSV: TBitBtn;
    SaveDialog: TSaveDialog;
    procedure dgLogDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgLogMouseDown(Sender: TObject; Button: TMouseButton;
      Shift: TShiftState; X, Y: Integer);
    procedure dgLogKeyPress(Sender: TObject; var Key: Char);
    procedure mbCopyToQurantineClick(Sender: TObject);
    procedure mbDeleteClick(Sender: TObject);
    procedure N1Click(Sender: TObject);
    procedure N2Click(Sender: TObject);
    procedure N4Click(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure Keylogger1Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure N8Click(Sender: TObject);
    procedure N9Click(Sender: TObject);
    procedure N7Click(Sender: TObject);
    procedure dgLogFixedCellClick(Sender: TObject; ACol, ARow: Integer);
    procedure mbSaveAsCSVClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   // Выполнение диалога
   function Execute : boolean;
   // Настройка таблицы
   procedure SetupGrid;
  end;

var
  ExtLogView: TExtLogView;

implementation

uses zVirFileList, uGUIShared, zTranslate;

{$R *.dfm}

{ TExtLogView }

function TExtLogView.Execute: boolean;
begin
 if VirFileList.VirFileCount = 0 then begin
  zMessageDlg('$AVZ1037',  mtError, [mbOk], 0);
  exit;
 end;
 SetupGrid;
 ShowModal;
end;

procedure TExtLogView.dgLogDrawCell(Sender: TObject; ACol, ARow: Integer;
  Rect: TRect; State: TGridDrawState);
var
 S : string;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0581';
   2 : S := '$AVZ1062';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < VirFileList.VirFileCount then
  case ACol of
   0 : begin
        S := VirFileList.VirFileList[ARow-1].FileName;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        if VirFileList.VirFileList[ARow-1].Checked then
         GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2)
          else GUIShared.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
       end;
   1 : begin
        S := VirFileList.VirFileList[ARow-1].Descr;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left, Rect.Top, S);
       end;
   2 : begin
        case VirFileList.VirFileList[ARow-1].VirType of
         vrtVirus          : S := '$AVZ0150';
         vrtVirusPartSign  : S := '$AVZ0739';
         vrtVirusEv        : S := '$AVZ0740';
         vrtVirusRootKit   : S := '$AVZ0729';
         vrtKeyLogger      : S := '$AVZ0725';
        end;
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left, Rect.Top, TranslateStr(S));
       end;
  end;
 end;
end;

procedure TExtLogView.dgLogMouseDown(Sender: TObject; Button: TMouseButton;
  Shift: TShiftState; X, Y: Integer);
begin
 if (dgLog.Row - 1 < VirFileList.VirFileCount) and (dgLog.Col = 0) and (X in [2..11]) then
  VirFileList.VirFileList[dgLog.Row-1].Checked := not(VirFileList.VirFileList[dgLog.Row-1].Checked);
 dgLog.InvalidateCell(dgLog.Col, dgLog.Row);
end;

procedure TExtLogView.dgLogKeyPress(Sender: TObject; var Key: Char);
begin
 if (dgLog.Col = 0) and (Key =' ') then
  VirFileList.VirFileList[dgLog.Row-1].Checked := not(VirFileList.VirFileList[dgLog.Row-1].Checked);
 dgLog.InvalidateCell(dgLog.Col, dgLog.Row);
end;

procedure TExtLogView.mbCopyToQurantineClick(Sender: TObject);
var
 i : integer;
begin
 if zMessageDlg('$AVZ0371 ?', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 // Цикл добавления в карантин
 for i := 0 to VirFileList.VirFileCount - 1 do
  if VirFileList.VirFileList[i].Checked then
   CopyToInfected(VirFileList.VirFileList[i].FileName,
                       VirFileList.VirFileList[i].Descr,
                       'Quarantine');
end;

procedure TExtLogView.mbDeleteClick(Sender: TObject);
var
 i : integer;
 FileList : TStringList;
 DelMode  : integer;
 FQuarantine : boolean;
begin
 i := 0;
 RebootNeed := false;
 // Запрос операции
 if zMessageDlg('$AVZ1178 ?', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 ExecuteFileDeleteModeDLG(DelMode, FQuarantine);
 if DelMode = 0 then exit;
 FileList := TStringList.Create;
 // Цикл удаления
 while i <= VirFileList.VirFileCount - 1 do
  if VirFileList.VirFileList[i].Checked then begin
   FileList.Add(VirFileList.VirFileList[i].FileName);
   if FQuarantine then
    CopyToInfected(VirFileList.VirFileList[i].FileName,
                        '$AVZ0376',
                        'Quarantine'); //#DNL)
   if DeleteVirus(VirFileList.VirFileList[i].FileName,
                       VirFileList.VirFileList[i].Descr,
                       VirFileList.VirFileList[i].Script) then begin
    VirFileList.DeleteVir(i);
   end else inc(i);
  end else inc(i);
 if (DelMode = 2) and (FileList.Count > 0) then
  ExtSystemClean(FileList);
 FileList.Free;
 // Запрос перезагрузки
 if RebootNeed then begin
  zMessageDlg('$AVZ0213', mtInformation, [mbOk], 0);
 end;
 // Обновление и перенастройка таблицы
 SetupGrid;
end;

procedure TExtLogView.SetupGrid;
begin
 if VirFileList.VirFileCount = 0 then dgLog.RowCount := 2
  else dgLog.RowCount := VirFileList.VirFileCount + 1;
 dgLog.Refresh;
end;

procedure TExtLogView.N1Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  if (VirFileList.VirFileList[i].VirType <> vrtVirus) then
   VirFileList.VirFileList[i].Checked := true;
 dgLog.Repaint;
end;

procedure TExtLogView.N2Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  if (VirFileList.VirFileList[i].VirType = vrtVirus) then
   VirFileList.VirFileList[i].Checked := true;
 dgLog.Repaint;
end;

procedure TExtLogView.N4Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  if (VirFileList.VirFileList[i].VirType = vrtVirusPartSign) then
   VirFileList.VirFileList[i].Checked := true;
 dgLog.Repaint;
end;

procedure TExtLogView.N3Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  if (VirFileList.VirFileList[i].VirType = vrtVirusRootKit) then
   VirFileList.VirFileList[i].Checked := true;
 dgLog.Repaint;
end;

procedure TExtLogView.Keylogger1Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  if (VirFileList.VirFileList[i].VirType = vrtKeyLogger) then
   VirFileList.VirFileList[i].Checked := true;
 dgLog.Repaint;
end;

procedure TExtLogView.Timer1Timer(Sender: TObject);
begin
 if VirFileList.ChangeFlg then begin
  SetupGrid;
  VirFileList.ChangeFlg := false;
 end;
end;

procedure TExtLogView.N8Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  VirFileList.VirFileList[i].Checked := true;
 dgLog.Repaint;
end;

procedure TExtLogView.N9Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  VirFileList.VirFileList[i].Checked := false;
 dgLog.Repaint;
end;

procedure TExtLogView.N7Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to VirFileList.VirFileCount - 1 do
  VirFileList.VirFileList[i].Checked := not(VirFileList.VirFileList[i].Checked);
 dgLog.Repaint;
end;

procedure TExtLogView.dgLogFixedCellClick(Sender: TObject; ACol,
  ARow: Integer);
begin
 VirFileList.SortData(ACol);
 dgLog.Repaint;
end;

procedure TExtLogView.mbSaveAsCSVClick(Sender: TObject);
begin
 if SaveDialog.Execute then
  VirFileList.SaveAsCSV(SaveDialog.FileName);
end;

procedure TExtLogView.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
