unit uFileDeleteModeDLG;
// *********** Диалог настройки режиме удаления файла *************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls;

type
  TFileDeleteModeDLG = class(TForm)
    Panel2: TPanel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    Panel1: TPanel;
    Panel3: TPanel;
    rbDelete: TRadioButton;
    rbHDelete: TRadioButton;
    cbQuarantine: TCheckBox;
    procedure FormCreate(Sender: TObject);
  private
    function GetQuarantineFile: boolean;
    { Private declarations }
  public
   Function Execute : boolean;
  published
   property QuarantineFile : boolean read GetQuarantineFile;
  end;

var
  FileDeleteModeDLG: TFileDeleteModeDLG;

procedure ExecuteFileDeleteModeDLG(var ADelMode : integer; var AQuarantine : boolean);
implementation
uses zTranslate;
{$R *.dfm}

{ TFileDeleteModeDLG }

function TFileDeleteModeDLG.Execute: boolean;
begin
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure ExecuteFileDeleteModeDLG(var ADelMode : integer; var AQuarantine : boolean);
begin
 FileDeleteModeDLG := nil;
 ADelMode := 0;
 try
  FileDeleteModeDLG := TFileDeleteModeDLG.Create(nil);
  if FileDeleteModeDLG.Execute then
   if FileDeleteModeDLG.rbDelete.Checked then
    ADelMode := 1 else ADelMode := 2;
  AQuarantine := FileDeleteModeDLG.QuarantineFile;
 finally
  FileDeleteModeDLG.Free;
  FileDeleteModeDLG := nil;
 end;
end;

function TFileDeleteModeDLG.GetQuarantineFile: boolean;
begin
 Result := cbQuarantine.Checked;
end;

procedure TFileDeleteModeDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
