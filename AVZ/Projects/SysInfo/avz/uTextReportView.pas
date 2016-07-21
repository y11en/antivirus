unit uTextReportView;
// Окно для просмотра текстовых отчетов
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls;

type
  TTxtLogView = class(TForm)
    TextMemo: TMemo;
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    function Execute(AText : TStrings; ACaption : string) : boolean;
  end;

var
  TxtLogView: TTxtLogView;

function ExecuteTxtLogView(AText : TStrings; ACaption : string) : boolean;
implementation
uses zTranslate;
{$R *.dfm}

{ TTextLogView }

function ExecuteTxtLogView(AText : TStrings; ACaption : string) : boolean;
begin
 TxtLogView := nil;
 try
  TxtLogView := TTxtLogView.Create(nil);
  Result := TxtLogView.Execute(AText, ACaption);
 finally
  TxtLogView.Free;
  TxtLogView := nil;
 end;
end;

function TTxtLogView.Execute(AText: TStrings; ACaption: string): boolean;
begin
 Caption := ACaption;
 TextMemo.Lines.Clear;
 TextMemo.Lines.AddStrings(AText);
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TTxtLogView.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
