unit uBackupScripts;
// Стандартные скрипты
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, CheckLst, zStringCompressor, Buttons, ExtCtrls,
  zSharedFunctions, zLogSystem, zAVKernel, zTranslate;

type
  TBackupScriptsDLG = class(TForm)
    cbRepairScriptList: TCheckListBox;
    Panel1: TPanel;
    BitBtn1: TBitBtn;
    mbRunScripts: TBitBtn;
    mbStopScript: TBitBtn;
    procedure mbRunScriptsClick(Sender: TObject);
    procedure mbStopScriptClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   function RefreshList : boolean;
   Function Execute : boolean;
  end;

var
  BackupScriptsDLG: TBackupScriptsDLG;

function ExecuteBackupScriptsDLG : boolean;

implementation

uses zScriptingKernel, zAVZKernel;

{$R *.dfm}

{ TSystemRepair }

function TBackupScriptsDLG.Execute: boolean;
begin
 mbStopScript.Enabled := false;
 RefreshList;
 ShowModal;
 Result := ModalResult = mrOk;
 AvzUserScript.LogEnabled := true;
end;

function ExecuteBackupScriptsDLG : boolean;
begin
 try
  BackupScriptsDLG := TBackupScriptsDLG.Create(nil);
  Result   := BackupScriptsDLG.Execute;
 finally
  BackupScriptsDLG.Free;
  BackupScriptsDLG := nil;
 end;
end;

function TBackupScriptsDLG.RefreshList: boolean;
var
 i : integer;
begin
 cbRepairScriptList.Items.Clear;
 Result := BackupScripts.LoadBinDatabase;
 for i := 0 to Length(BackupScripts.BackupScripts)-1 do
  cbRepairScriptList.Items.AddObject(inttostr(i+1)+'. '+TranslateStr(DeCompressString(BackupScripts.BackupScripts[i].CompressedName)),
                                     pointer(i));
end;

procedure TBackupScriptsDLG.mbRunScriptsClick(Sender: TObject);
var
 i, RepairCnt, X : integer;
 S : string;
begin
 try
   mbRunScripts.Enabled := false;
   mbStopScript.Enabled := true;
   Screen.Cursor := crHourGlass;        
   RepairCnt := 0;
   for i := 0 to cbRepairScriptList.Items.Count - 1 do
    if cbRepairScriptList.Checked[i] then inc(RepairCnt);
   // Есть отмеченные элементы ??
   if RepairCnt = 0 then begin
    zMessageDlg('$AVZ0482', mtError, [mbOk], 0);
    exit;
   end;
   if RepairCnt >= 20 then x := RepairCnt mod 10
    else x := RepairCnt mod 20;
   S := '$AVZ1174: '+IntToStr(RepairCnt);
   if zMessageDlg(S+'. $AVZ0849 ?', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
   for i := 0 to cbRepairScriptList.Items.Count - 1 do
    if cbRepairScriptList.Checked[i] then begin
     AddToLog('$AVZ1173: '+cbRepairScriptList.Items[i], logInfo);
     AvzUserScript.ExecuteScript('', DeCompressString(BackupScripts.BackupScripts[i].CompressedText),cbRepairScriptList.Items[i]);
     if StopF then exit;
    end;
   zMessageDlg('$AVZ1169', mtInformation, [mbOk], 0);
 finally
  Screen.Cursor := crDefault;
  mbRunScripts.Enabled := true;
  mbStopScript.Enabled := false;
 end;
end;

procedure TBackupScriptsDLG.mbStopScriptClick(Sender: TObject);
begin
 AvzUserScript.StopF := true;
 StopF := true;
end;

procedure TBackupScriptsDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
