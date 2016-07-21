unit uRunScript;
// *********** Окно запуска скрипта, вводимого пользователем ***********
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, zUserScripting, zAVKernel, zLogSystem,
  ComCtrls;

type
  TRunScriptDLG = class(TForm)
    Panel1: TPanel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    mbRun: TBitBtn;
    mbCheckScript: TBitBtn;
    meScriptText: TMemo;
    OpenDialog1: TOpenDialog;
    SaveDialog1: TSaveDialog;
    ProgressBar: TProgressBar;
    procedure mbRunClick(Sender: TObject);
    procedure mbCheckScriptClick(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
    // Обработчик прогресс-индикации
    procedure OnProgress(AMax, ACurr : integer);
    // Выполнение диалога
    function Execute : boolean;
  end;

var
  RunScriptDLG: TRunScriptDLG;

// Выполнение диалога
function ExecuteRunScriptDLG : boolean;

implementation
uses zScriptingKernel, zTranslate;
{$R *.dfm}

function ExecuteRunScriptDLG : boolean;
begin
 RunScriptDLG := nil;
 try
  RunScriptDLG := TRunScriptDLG.Create(nil);
  Result     := RunScriptDLG.Execute;
 finally
  RunScriptDLG.Free;
  RunScriptDLG := nil;
 end;
end;


procedure TRunScriptDLG.mbRunClick(Sender: TObject);
var
 OldOnProgress : TProgressEvent;
begin
 mbRun.Enabled := false;
 mbCheckScript.Enabled := false;
 try
  OldOnProgress := ZProgressClass.OnProgress;
  ZProgressClass.OnProgress := OnProgress;
  if AvzUserScript.ExecuteScript('', meScriptText.Text, '') then
   zMessageDlg('$AVZ0990', mtInformation, [mbOk], 0)
  else
   zMessageDlg('$AVZ0630: '+AvzUserScript.LastError+' $AVZ0093 '+AvzUserScript.LastErrorPos, mtError, [mbOk], 0);
 finally
  ZProgressClass.OnProgress := OldOnProgress;
  ProgressBar.Position := 0;
  mbRun.Enabled := true;
  mbCheckScript.Enabled := true;
 end;
end;

procedure TRunScriptDLG.mbCheckScriptClick(Sender: TObject);
begin
 if AvzUserScript.ExecuteScript('', meScriptText.Text, '', true) then
  zMessageDlg('$AVZ0992', mtInformation, [mbOk], 0)
 else
  zMessageDlg('$AVZ0630: '+AvzUserScript.LastError+' $AVZ0093 '+AvzUserScript.LastErrorPos, mtError, [mbOk], 0);
end;

function TRunScriptDLG.Execute: boolean;
begin
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TRunScriptDLG.BitBtn1Click(Sender: TObject);
begin
 if OpenDialog1.Execute and (OpenDialog1.FileName <> '') then
  if FileExists(OpenDialog1.FileName) then
    meScriptText.Lines.LoadFromFile(OpenDialog1.FileName);
end;

procedure TRunScriptDLG.BitBtn2Click(Sender: TObject);
begin
 if SaveDialog1.Execute and (SaveDialog1.FileName <> '') then
    meScriptText.Lines.SaveToFile(SaveDialog1.FileName);
end;

procedure TRunScriptDLG.OnProgress(AMax, ACurr: integer);
begin
 if AMax <> ProgressBar.Max then
  ProgressBar.Max := AMax;
 if ACurr <> ProgressBar.Position then
  ProgressBar.Position := ACurr;
end;

procedure TRunScriptDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
