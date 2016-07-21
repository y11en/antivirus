unit uExecProcessDLG;
// ************* Диалог запуска доверенного процесса *************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, Mask, ToolEdit;

type
  TExecProcessDLG = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    feProgramm: TFilenameEdit;
    edCmdLine: TEdit;
    Panel1: TPanel;
    mbOk: TBitBtn;
    BitBtn2: TBitBtn;
    procedure feProgrammChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
   function Execute : boolean;
  end;

var
  ExecProcessDLG: TExecProcessDLG;

 function ExecProcessDLGExecute(var APrg, ACmdLine : string) : boolean;
implementation
uses zTranslate;
{$R *.dfm}

{ TExecProcessDLG }

function TExecProcessDLG.Execute: boolean;
begin
 feProgramm.Text := '';
 edCmdLine.Text := '';
 ShowModal;
 Result := ModalResult = mrOk;
end;

function ExecProcessDLGExecute(var APrg, ACmdLine : string) : boolean;
begin
  Result := false;               
  ExecProcessDLG := nil;
  try
   APrg := ''; ACmdLine := '';
   ExecProcessDLG := TExecProcessDLG.Create(nil);
   Result := ExecProcessDLG.Execute;
   APrg      := Trim(ExecProcessDLG.feProgramm.Text);
   ACmdLine  := Trim(ExecProcessDLG.edCmdLine.Text);
   if APrg = '' then Result := false;
  finally
   ExecProcessDLG.Free;
   ExecProcessDLG := nil;
  end;
end;

procedure TExecProcessDLG.feProgrammChange(Sender: TObject);
begin
 mbOk.Enabled := Trim(feProgramm.Text) <> '';
end;

procedure TExecProcessDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
