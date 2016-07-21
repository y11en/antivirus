unit uRepairWizard;
// Менеджер восстановления системы
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, ComCtrls, CheckLst, Buttons,
  zAntivirus,
  zTranslate,
  zLogSystem,
  zStringCompressor,
  zScriptingKernel,
  zAVKernel,
  zBackupEngine;

type
  TRepairWizard = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    cbCategory: TComboBox;
    Label2: TLabel;
    cbLevel: TComboBox;
    PageControl1: TPageControl;
    tsCheckFix: TTabSheet;
    tsUndo: TTabSheet;
    clUndo: TCheckListBox;
    Panel2: TPanel;
    mbClose: TBitBtn;
    mbUndo: TBitBtn;
    ProgressBar: TProgressBar;
    Panel3: TPanel;
    BitBtn1: TBitBtn;
    mbFixProblems: TBitBtn;
    clProblems: TCheckListBox;
    Panel4: TPanel;
    mbStart: TBitBtn;
    mbStop: TBitBtn;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure mbStartClick(Sender: TObject);
    procedure mbUndoClick(Sender: TObject);
    procedure mbStopClick(Sender: TObject);
    procedure PageControl1Change(Sender: TObject);
    procedure cbCategoryChange(Sender: TObject);
    procedure clProblemsClickCheck(Sender: TObject);
    procedure clUndoClickCheck(Sender: TObject);
    procedure mbFixProblemsClick(Sender: TObject);
  private
    function GetBackUpFile: string;
  public
   StopF : boolean;
   DBName : string;
   CustomScripts : TCustomScripts;
   MinLevel : integer;
   // Выполнение диалога
   Function Execute : boolean;
   // Поиск проблем по указанной базе
   function SearchProblems(ADBName : string) : boolean;
   // Исправление отмеченных проблем
   function FixProblems: boolean;
   // Поиск по базе отката
   function SearchUndo(ADBName : string) : boolean;
   // Откат отмеченных проблем
   function UndoProblems : boolean;
  published
   property BackUpFile : string read GetBackUpFile;
  end;

var
  RepairWizard: TRepairWizard;

function ExecuteRepairWizardDLG : boolean;

implementation
uses zutil;
{$R *.dfm}

function ExecuteRepairWizardDLG : boolean;
begin
 try
  RepairWizard := TRepairWizard.Create(nil);
  Result   := RepairWizard.Execute;
 finally
  RepairWizard.Free;
  RepairWizard := nil;
 end;
end;

{ TRepairWizard }

function TRepairWizard.Execute: boolean;
begin
 cbCategory.ItemIndex := 0;
 cbLevel.ItemIndex := 1;
 ShowModal;
 Result := ModalResult = mrOK;
end;

procedure TRepairWizard.FormCreate(Sender: TObject);
begin
 mbFixProblems.Enabled := false;
 CustomScripts := TCustomScripts.Create;
 TranslateForm(Self);
end;

procedure TRepairWizard.FormDestroy(Sender: TObject);
begin
 CustomScripts.Free;
 CustomScripts := nil;
end;

procedure TRepairWizard.mbStartClick(Sender: TObject);
begin
 try
  StopF := false;
  mbStart.Enabled := false;
  mbStop.Enabled  := true;
  // Определение имени базы по категории
  DBName := '';
  case cbCategory.ItemIndex of
   0 : DBName := 'tsw.avz';
   1 : DBName := 'bt.avz';
   2 : DBName := 'prt.avz';
  end;
  // Определение уровня срабатывания
  MinLevel := 3 - cbLevel.ItemIndex;
  // Процесс поиска проблем
  mbFixProblems.Enabled := false;
  SearchProblems(DBName);
  if StopF then
   zShowMessage('$AVZ0964') else
  begin
   if not(clProblems.Items.Count > 0) then
    zShowMessage('$AVZ1247');
  end;
 finally
  mbStart.Enabled := true;
  mbStop.Enabled  := false;
 end;
end;

function TRepairWizard.SearchProblems(ADBName: string): boolean;
var
 i, Detectedlevel : integer;
 S : string;
begin
 clProblems.Clear;
 ProgressBar.Position := 0;
 Screen.Cursor := crHourGlass;
 try
   CustomScripts.LoadBinDatabase(AVPath + ADBName);
   ProgressBar.Max := length(CustomScripts.Scripts);
   for i := 0 to length(CustomScripts.Scripts)-1 do begin
    try
     if StopF then exit;
     ProgressBar.Position := i;
     Application.ProcessMessages;
     // Извлечение из базы скрипта, его декомпрессия и запуск
     S := Trim(DeCompressString(CustomScripts.Scripts[i].CompressedText));
     S := S + #$0D#$0A+' begin if AFilename = ''FIX'' then OutRes := IntToStr(Fix) else OutRes := IntToStr(Check); end.';
     // запуск скрипта
     if AvzUserScript.ExecuteScript('CHECK', S) then begin
      Detectedlevel := StrToIntDef(AvzUserScript.OutRes, 0);
      // Проблема детектирована ?? Если да, то добавим ее к списку
      if (Detectedlevel > 0) and (Detectedlevel >= MinLevel) then
       clProblems.Items.AddObject(TranslateStr(DeCompressString(CustomScripts.Scripts[i].CompressedName)),
                                  pointer(i));
     end;
     // Чистка памяти
     if S <> '' then ZeroMemory(@S[1], length(S));
     S := '';
    except
      on e : Exception do
       AddToDebugLog('Script error: '+IntToStr(i));
    end;
   end;
 finally
  ProgressBar.Position := 0;
  Screen.Cursor := crDefault;
 end;
end;

// Исправление отмеченных проблем
function TRepairWizard.FixProblems : boolean;
var
 i, FixID, FixCnt : integer;
 S : string;
 BackupEngine : TZBackupEngine;
 MS : TMemoryStream;
begin
 FixCnt := 0;
 for i := 0 to clProblems.Items.Count - 1 do
  if clProblems.Checked[i] then inc(FixCnt);
 if FixCnt = 0 then exit;
 // Подготовка системы записи изменений для отката
 BackupEngine := TZBackupEngine.Create;
 MS := TMemoryStream.Create;
 if FileExists(BackUpFile) then begin
  MS.LoadFromFile(BackUpFile);
  BackupEngine.LoadBackupDB(MS);
 end;
 // Цикл выполнения операций восстановления системы
 ProgressBar.Max := FixCnt;
 ProgressBar.Position := 0;
 for i := 0 to clProblems.Items.Count - 1 do
  if clProblems.Checked[i] then begin
   if StopF then exit;
   ProgressBar.Position := ProgressBar.Position + 1;
   Application.ProcessMessages;
   FixID := integer(clProblems.Items.Objects[i]);
   // Извлечение из базы скрипта, его декомпрессия и запуск
   S := Trim(DeCompressString(CustomScripts.Scripts[FixID].CompressedText));
   S := S + #$0D#$0A+' begin if AFilename = ''FIX'' then OutRes := IntToStr(Fix) else OutRes := IntToStr(Check); end.';
   // запуск скрипта
   BackupEngine.BackupId := UpperCase(ChangeFileExt(DBName, '')) + '.' + IntToStr(CustomScripts.Scripts[FixID].ID);
   AvzUserScript.BackupEngine := BackupEngine;
   AvzUserScript.LogEnabled   := false;
   AvzUserScript.ExecuteScript('FIX', S);
   // Чистка памяти
   if S <> '' then ZeroMemory(@S[1], length(S));
   S := '';
  end;
 MS.Clear;
 // Сохранение базы отката
 zForceDirectories(ExtractFilePath(BackUpFile));
 BackupEngine.SaveBackupDB(MS);
 MS.Seek(0,0);
 MS.SaveToFile(BackUpFile);
 MS.Free;
 BackupEngine.Free;
 ProgressBar.Position := 0;
 Result := true;
end;

procedure TRepairWizard.mbUndoClick(Sender: TObject);
var
 i : integer;
begin
 StopF := false;
 mbUndo.Enabled := false;
 UndoProblems;
 zShowMessage('$AVZ1605');
 i := 0;
 while i <= clUndo.Items.Count - 1 do
  if clUndo.Checked[i] then
   clUndo.Items.Delete(i)
    else inc(i);
end;

procedure TRepairWizard.mbStopClick(Sender: TObject);
begin
 StopF := true;
end;

procedure TRepairWizard.PageControl1Change(Sender: TObject);
begin
 cbLevel.Enabled := PageControl1.ActivePage = tsCheckFix;
 // Определение имени базы по категории
 DBName := '';
 case cbCategory.ItemIndex of
  0 : DBName := 'tsw.avz';
  1 : DBName := 'bt.avz';
  2 : DBName := 'prt.avz';
 end;
 if PageControl1.ActivePage = tsUndo then
  SearchUndo(DBName);
end;

function TRepairWizard.SearchUndo(ADBName: string): boolean;
var
 BackupEngine : TZBackupEngine;
 i            : integer;
 MS           : TMemoryStream;
begin
 Result := false;
 ProgressBar.Position := 0;
 clUndo.Items.Clear;
 mbUndo.Enabled := false;
 // Загрузка базы скриптов
 if not(CustomScripts.LoadBinDatabase(AVPath + ADBName)) then exit;
 // Загрузка базы бекапа
 if not(FileExists(BackUpFile)) then exit;
 BackupEngine := TZBackupEngine.Create;
 MS := TMemoryStream.Create;
 try
  if FileExists(BackUpFile) then begin
   MS.LoadFromFile(BackUpFile);
   BackupEngine.LoadBackupDB(MS);
  end;
  // В базе бекапа есть данные ? Если нет, то выход
  if BackupEngine.BackupDB.Count = 0 then exit;
  // Что-то есть - ищем, что можно откатить
  for i := 0 to Length(CustomScripts.Scripts) - 1 do
   if BackupEngine.BackUpExists(UpperCase(ChangeFileExt(DBName, ''))+'.'+IntToStr(CustomScripts.Scripts[i].ID)) then
    clUndo.Items.AddObject(TranslateStr(DeCompressString(CustomScripts.Scripts[i].CompressedName)),
                                  pointer(CustomScripts.Scripts[i].ID));
 finally
  MS.Free;
  BackupEngine.Free;
 end;
end;

procedure TRepairWizard.cbCategoryChange(Sender: TObject);
begin
 if PageControl1.ActivePage = tsUndo then begin
  // Определение имени базы по категории
  DBName := '';
  case cbCategory.ItemIndex of
   0 : DBName := 'tsw.avz';
   1 : DBName := 'bt.avz';
   2 : DBName := 'prt.avz';
  end;
  StopF := false;
  SearchUndo(DBName);
 end;
end;

function TRepairWizard.GetBackUpFile: string;
begin
 Result := ExtractFilePath(Application.ExeName)+'Backup\'+ChangeFileExt(ExtractFileName(DBName),'.zbk');
end;

procedure TRepairWizard.clProblemsClickCheck(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to clProblems.Count - 1 do
  if clProblems.Checked[i] then begin
   mbFixProblems.Enabled := true;
   exit;
  end;
 mbFixProblems.Enabled := false;
end;

procedure TRepairWizard.clUndoClickCheck(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to clUndo.Count - 1 do
  if clUndo.Checked[i] then begin
   mbUndo.Enabled := true;
   exit;
  end;
 mbUndo.Enabled := false;
end;

procedure TRepairWizard.mbFixProblemsClick(Sender: TObject);
var
 i : integer;
begin
 StopF := false;
 mbFixProblems.Enabled := false;
 FixProblems;
 zShowMessage('$AVZ1246');
 i := 0;
 while i <= clProblems.Items.Count - 1 do
  if clProblems.Checked[i] then
   clProblems.Items.Delete(i)
    else inc(i);
end;

function TRepairWizard.UndoProblems: boolean;
var
 BackupEngine : TZBackupEngine;
 i            : integer;
 MS           : TMemoryStream;
begin
 Result := false;
 ProgressBar.Position := 0;
 mbUndo.Enabled := false;
 // Загрузка базы бекапа
 if not(FileExists(BackUpFile)) then exit;
 BackupEngine := TZBackupEngine.Create;
 MS := TMemoryStream.Create;
 try
  if FileExists(BackUpFile) then begin
   MS.LoadFromFile(BackUpFile);
   BackupEngine.LoadBackupDB(MS);
  end;
  // В базе бекапа есть данные ? Если нет, то выход
  if BackupEngine.BackupDB.Count = 0 then exit;
  // Что-то есть - ищем, что можно откатить
  for i := 0 to clUndo.Items.Count - 1 do
   if clUndo.Checked[i] then
    BackupEngine.UndoChanges(UpperCase(ChangeFileExt(DBName, ''))+'.'+IntToStr(integer(clUndo.Items.Objects[i])), true);
  // Сохранение базы
  if BackupEngine.BackupDB.Count = 0 then
   DeleteFile(BackUpFile)
  else begin
   MS.Clear;
   MS.Size := 0;
   BackupEngine.SaveBackupDB(MS);
   try
   except
    MS.SaveToFile(BackUpFile);
   end;
  end;
 finally
  MS.Free;
  BackupEngine.Free;
 end;
end;

end.
