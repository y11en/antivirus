unit uTaskScheduler;
// Њенеджер заданий планировщика задач
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, Grids, RXGrids, zTaskScheduler,
  zHTMLTemplate,AuxProcs,zAVZKernel, zLogSystem, zSharedFunctions;

type
  TTaskScheduler = class(TForm)
    dgTasks: TRxDrawGrid;
    Panel1: TPanel;
    sbSaveProcLog: TSpeedButton;
    sbCopyToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    mbDeleteItem: TBitBtn;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure dgTasksDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure SpeedButton5Click(Sender: TObject);
    procedure sbCopyToQurantineClick(Sender: TObject);
    procedure mbDeleteItemClick(Sender: TObject);
    procedure sbSaveProcLogClick(Sender: TObject);
    procedure dgTasksClick(Sender: TObject);
  private
    { Private declarations }
  public
   TaskSchedulerJobs : TTaskSchedulerJobs;
   // Обновление информации
   function RefreshTaskSchedulerJobs : boolean;
   // Выполнение диалога
   function Execute : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
   // Создание HTML отчета
   function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Создание XML отчета
   function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
  end;

var
  TaskScheduler: TTaskScheduler;

 // Выполнение в диалоговом режиме
 Function ExecuteTaskScheduler : boolean;
implementation

uses uGUIShared, zTranslate;



{$R *.dfm}
Function ExecuteTaskScheduler : boolean;
begin
 try
  TaskScheduler := nil;
  TaskScheduler := TTaskScheduler.Create(nil);
  Result := TaskScheduler.Execute;
 finally
  TaskScheduler.Free;
  TaskScheduler := nil;
 end;
end;

{ TTaskScheduler }

function TTaskScheduler.Execute: boolean;
begin
 RefreshTaskSchedulerJobs;
 ShowModal;
 ModalResult := mrOk;
end;

procedure TTaskScheduler.FormCreate(Sender: TObject);
begin
 TaskSchedulerJobs := TTaskSchedulerJobs.Create;
 TranslateForm(Self);
end;

procedure TTaskScheduler.FormDestroy(Sender: TObject);
begin
 TaskSchedulerJobs.Free;
end;

function TTaskScheduler.RefreshTaskSchedulerJobs: boolean;
begin
 TaskSchedulerJobs.RefresTaskSchedulerJobsList;
 if length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) > 0 then
  dgTasks.RowCount := length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) + 1
   else  dgTasks.RowCount := 2;
end;

procedure TTaskScheduler.dgTasksDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TTaskSchedulerJob;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ1109';
   1 : S := '$AVZ0260';
   2 : S := '$AVZ0582';
   3 : S := '$AVZ0852';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) then begin
   Tmp := TaskSchedulerJobs.TaskSchedulerJobInfoArray[ARow - 1];
   if Tmp.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
   case ACol of
    0 : begin
         S := Trim(Tmp.BinFile);
         if S = '' then
          S := TranslateStr('$AVZ1117');
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left, Rect.Top, S);
        {
        (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+16, Rect.Top, S);
        case TaskSchedulerJobs.GetItemEnabledStatus(Tmp) of
         -1 : Main.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 3);
         0  : Main.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 1);
         1  : Main.StateImageList.Draw((Sender as TRxDrawGrid).Canvas, Rect.Left, Rect.Top+1, 2);
        end;
        }
        S := '';
        end;
    1 : begin
         S := Tmp.Name;
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

function TTaskScheduler.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) - 1 do
  if TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult <> 0 then
   CopyToInfected(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].BinFile, '$AVZ0968', 'Quarantine');
end;

procedure TTaskScheduler.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0368',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

procedure TTaskScheduler.sbCopyToQurantineClick(Sender: TObject);
begin
 if dgTasks.Row - 1 < length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) then begin
  if TaskSchedulerJobs.TaskSchedulerJobInfoArray[dgTasks.Row - 1].CheckResult = 0 then exit;
  if zMessageDlg('$AVZ0359 '+TaskSchedulerJobs.TaskSchedulerJobInfoArray[dgTasks.Row - 1].BinFile+' $AVZ0090 ?',
                mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
  CopyToInfected(TaskSchedulerJobs.TaskSchedulerJobInfoArray[dgTasks.Row - 1].BinFile, '$AVZ0968', 'Quarantine');
 end;
end;

procedure TTaskScheduler.mbDeleteItemClick(Sender: TObject);
begin
 if dgTasks.Row - 1 < length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) then begin
  if not(TaskSchedulerJobs.DeleteItem(TaskSchedulerJobs.TaskSchedulerJobInfoArray[dgTasks.Row - 1])) then exit;
  RefreshTaskSchedulerJobs;
 end;
end;

procedure TTaskScheduler.sbSaveProcLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0261</H2>'));
 CreateHTMLReport(RepLines, false);
 RepLines.Add('</BODY></HTML>');
 GUIShared.HTMLSaveDialog.FileName := 'avz_task_shed.htm';
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TTaskScheduler.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S: string;
begin
 Result := false;
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0307'+HTML_TableHeaderTD+'$AVZ0303'+HTML_TableHeaderTD+'$AVZ1026'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852');
 GoodFiles := 0;
 for i := 0 to length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) - 1 do begin
  if TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult = 0 then
   Inc(GoodFiles);
  if not(HideGoodFiles) or (TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult <> 0) then begin
   S := 'bgColor='+HTMP_BgColor2;
   if TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult = 0 then begin
    S := 'bgColor='+HTMP_BgColorTrusted;
   end;
   if not(HideGoodFiles and (TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult = 0)) then
    ALines.Add('<TR '+S+'>'+
               '<TD>'+TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].BinFile+GenScriptTxt(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].BinFile)+
               '<TD>'+TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].Name+
               '<TD>'+MessageFromValue(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].Status)+
               '<TD>'+TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].Descr+
               '<TD>'+TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].LegalCopyright
               );

  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=7> $AVZ0541 - '+IntToStr(length(TaskSchedulerJobs.TaskSchedulerJobInfoArray))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TTaskScheduler.dgTasksClick(Sender: TObject);
var
 Tmp : TTaskSchedulerJob;
begin
 if dgTasks.Row - 1 < length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) then begin
  Tmp :=  TaskSchedulerJobs.TaskSchedulerJobInfoArray[dgTasks.Row - 1];
  sbCopyToQurantine.Enabled := Tmp.CheckResult <> 0;
 end;
end;

function TTaskScheduler.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 FileInfoStr: string;
begin
 Result := false;
 ALines.Add(' <TaskScheduler>');
 ZProgressClass.AddProgressMax(length(TaskSchedulerJobs.TaskSchedulerJobInfoArray));
 for i := 0 to length(TaskSchedulerJobs.TaskSchedulerJobInfoArray) - 1 do begin
  ZProgressClass.ProgressStep;
  if not(HideGoodFiles) or (TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult <> 0) then begin
   if not(HideGoodFiles and (TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult = 0)) then begin
    FileInfoStr := FormatXMLFileInfo(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].BinFile);
    ALines.Add('  <ITEM File="'+XMLStr(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].BinFile)+'" '+
                        'CheckResult="'+IntToStr(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].CheckResult)+'" '+
                        'Enabled="'+IntToStr(TaskSchedulerJobs.GetItemEnabledStatus(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i]))+'" '+
                        'Descr="'+XMLStr(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].Descr)+'" '+
                        'LegalCopyright="'+XMLStr(TaskSchedulerJobs.TaskSchedulerJobInfoArray[i].LegalCopyright)+'" '+
                        FileInfoStr+ ' '+
                  ' />');
   end;
  end;
 end;
 ALines.Add(' </TaskScheduler>');
end;

end.
