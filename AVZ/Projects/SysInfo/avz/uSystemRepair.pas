unit uSystemRepair;
// Восстановление настроек системы
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, CheckLst, zStringCompressor, Buttons, ExtCtrls,
  zLogSystem, zAVKernel, zTranslate;

type
  TSysRepair = class(TForm)
    cbRepairScriptList: TCheckListBox;
    Panel1: TPanel;
    mbClose: TBitBtn;
    mbRunRepair: TBitBtn;
    procedure mbRunRepairClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   function RefreshList : boolean;
   Function Execute : boolean;
  end;

var
  SysRepair: TSysRepair;

function ExecuteSysRepairDLG : boolean;

implementation

uses zScriptingKernel;

{$R *.dfm}

{ TSystemRepair }

function TSysRepair.Execute: boolean;
begin
 RefreshList;
 ShowModal;
 Result := ModalResult = mrOk;
 AvzUserScript.LogEnabled := true;
end;

function ExecuteSysRepairDLG : boolean;
begin
 try
  SysRepair := TSysRepair.Create(nil);
  Result   := SysRepair.Execute;
 finally
  SysRepair.Free;
  SysRepair := nil;
 end;
end;

function TSysRepair.RefreshList: boolean;
var
 i : integer;
begin
 cbRepairScriptList.Items.Clear;
 Result := SystemRepair.LoadBinDatabase;
 for i := 0 to Length(SystemRepair.RepairScripts)-1 do
  cbRepairScriptList.Items.AddObject(inttostr(i+1)+'. '+TranslateStr(DeCompressString(SystemRepair.RepairScripts[i].CompressedName)),
                                     pointer(i));
end;

procedure TSysRepair.mbRunRepairClick(Sender: TObject);
var
 i, RepairCnt, X : integer;
begin
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
 if zMessageDlg('$AVZ1293 '+IntToStr(RepairCnt), mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 if not(DebugLogMode) then
  AvzUserScript.LogEnabled := false;
 for i := 0 to cbRepairScriptList.Items.Count - 1 do
  if cbRepairScriptList.Checked[i] then
   AvzUserScript.ExecuteScript('', DeCompressString(SystemRepair.RepairScripts[i].CompressedText), '');
 zMessageDlg('$AVZ0146', mtInformation, [mbOk], 0);
 AvzUserScript.LogEnabled := true;
end;

procedure TSysRepair.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
