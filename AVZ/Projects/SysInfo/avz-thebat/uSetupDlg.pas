unit uSetupDlg;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Mask, ToolEdit, Buttons, ExtCtrls;

type
  TSetupDlg = class(TForm)
    Panel1: TPanel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    GroupBox1: TGroupBox;
    rbDefPath: TRadioButton;
    rbUserPath: TRadioButton;
    deUserPath: TDirectoryEdit;
    GroupBox2: TGroupBox;
    cbLogMode: TCheckBox;
    cbLogMalware: TCheckBox;
    procedure cbLogModeClick(Sender: TObject);
  private
    { Private declarations }
  public
   BasePathMode : integer;
   BasePath     : string;
   UserLog      : boolean;
   function Execute : boolean;
  end;

var
  SetupDlg: TSetupDlg;

 function ExecuteSetupDlg : boolean;
implementation
uses avz_options, zutil;
{$R *.dfm}
function ExecuteSetupDlg : boolean;
begin
 SetupDlg := nil;
 try
  SetupDlg := TSetupDlg.Create(nil);
  SetupDlg.BasePath     := NormalDir(Trim(OptionsList.Values['BasePath']));
  SetupDlg.BasePathMode := StrToIntDef(OptionsList.Values['BasePathMode'], 0);
  SetupDlg.cbLogMode.Checked := OptionsList.Values['EnableLog']  = '1';
  SetupDlg.cbLogMalware.Checked :=  OptionsList.Values['LogMalware'] = '1';
  Result := SetupDlg.Execute;
  if Result then begin
   OptionsList.Values['BasePath']         := NormalDir(SetupDlg.BasePath);
   OptionsList.Values['BasePathMode']     := IntToStr(SetupDlg.BasePathMode);
   if SetupDlg.cbLogMode.Checked then
    OptionsList.Values['EnableLog']     := '1'
     else OptionsList.Values['EnableLog']     := '0';
   if SetupDlg.cbLogMalware.Checked then                          
    OptionsList.Values['LogMalware']     := '1'
     else OptionsList.Values['LogMalware']     := '0';
  end;
 except
  SetupDlg.Free;
  SetupDlg := nil;
 end;
end;

{ TSetupDlg }

function TSetupDlg.Execute: boolean;
begin
 case BasePathMode of
  0 : rbDefPath.Checked  := true;
  1 : rbUserPath.Checked := true;
 end;
 deUserPath.Text   := Trim(BasePath);
 cbLogMalware.Enabled := cbLogMode.Checked;
 ShowModal;
 Result := ModalResult = mrOk;
 if Result then begin
  if rbUserPath.Checked then
   BasePathMode := 1 else BasePathMode := 0;
 BasePath := Trim(deUserPath.Text);
 end;
end;

procedure TSetupDlg.cbLogModeClick(Sender: TObject);
begin
 cbLogMalware.Enabled := cbLogMode.Checked;
end;

end.
