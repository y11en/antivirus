unit uDeleteModeDLG;
// ****************** Диалог запроса операции для объекта *********************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, RxGIF, ExtCtrls, StdCtrls, Buttons;

type
  TDeleteModeDLG = class(TForm)
    Image1: TImage;
    GroupBox1: TGroupBox;
    rbLog: TRadioButton;
    rbDelete: TRadioButton;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    BitBtn3: TBitBtn;
    Label1: TLabel;
    Label2: TLabel;
    lbVirusName: TLabel;
    lbFileName: TLabel;
    procedure FormCreate(Sender: TObject);
  public
   Function Execute(AFileName, AVirusName : string) : integer;
  end;

var
  DeleteModeDLG: TDeleteModeDLG;

function ExecuteDeleteModeDLG(AFileName, AVirusName : string) : integer;
implementation
uses zTranslate;
{$R *.dfm}
function ExecuteDeleteModeDLG(AFileName, AVirusName : string) : integer;
begin
 DeleteModeDLG := nil;
 try
  DeleteModeDLG := TDeleteModeDLG.Create(nil);
  Result   := DeleteModeDLG.Execute(AFileName, AVirusName);
 finally
  DeleteModeDLG.Free;
  DeleteModeDLG := nil;
 end;
end;

{ TDeleteModeDLG }

function TDeleteModeDLG.Execute(AFileName, AVirusName: string): integer;
begin
 Result := 0;
 lbVirusName.Caption := AVirusName;
 lbFileName.Caption  := AFileName;
 ShowModal;
 if ModalResult = mrOk then
  if rbDelete.Checked then Result := 1;
 if ModalResult = mrNo then
   Result := 10;
end;

procedure TDeleteModeDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
