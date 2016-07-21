unit uStdScripts;
// Стандартные скрипты
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, CheckLst, zStringCompressor, Buttons, ExtCtrls,
  zSharedFunctions, zLogSystem, zAVKernel, zTranslate;

type
  TStdScriptsDLG = class(TForm)
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
  StdScriptsDLG: TStdScriptsDLG;

function ExecuteStdScriptsDLG : boolean;

implementation

uses zScriptingKernel, zAVZKernel;

{$R *.dfm}

{ TSystemRepair }

function TStdScriptsDLG.Execute: boolean;
begin
 mbStopScript.Enabled := false;
 RefreshList;
 ShowModal;
 Result := ModalResult = mrOk;
 AvzUserScript.LogEnabled := true;
end;

function ExecuteStdScriptsDLG : boolean;
begin
 try
  StdScriptsDLG := TStdScriptsDLG.Create(nil);
  Result   := StdScriptsDLG.Execute;
 finally
  StdScriptsDLG.Free;
  StdScriptsDLG := nil;
 end;
end;

function TStdScriptsDLG.RefreshList: boolean;
var
 i : integer;
begin
 cbRepairScriptList.Items.Clear;
 Result := StdScripts.LoadBinDatabase;
 for i := 0 to Length(StdScripts.Scripts)-1 do
  cbRepairScriptList.Items.AddObject(inttostr(i+1)+'. '+TranslateStr(DeCompressString(StdScripts.Scripts[i].CompressedName)),
                                     pointer(i));
end;

procedure TStdScriptsDLG.mbRunScriptsClick(Sender: TObject);
var
 i, RepairCnt, X : integer;
begin
 try
   mbRunScripts.Enabled := false;
   mbStopScript.Enabled := true;
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
   if zMessageDlg('$AVZ0188'+' ?', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
   for i := 0 to cbRepairScriptList.Items.Count - 1 do
    if cbRepairScriptList.Checked[i] then begin
     AddToLog('$AVZ0192: '+cbRepairScriptList.Items[i], logInfo);
     AvzUserScript.ExecuteScript('', DeCompressString(StdScripts.Scripts[i].CompressedText),cbRepairScriptList.Items[i]);
     if StopF then exit;
    end;
   zMessageDlg('$AVZ0995', mtInformation, [mbOk], 0);
 finally
  mbRunScripts.Enabled := true;
  mbStopScript.Enabled := false;
 end;
end;

procedure TStdScriptsDLG.mbStopScriptClick(Sender: TObject);
begin
 AvzUserScript.StopF := true;
 StopF := true;
end;

procedure TStdScriptsDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
