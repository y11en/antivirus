unit uSelectDeletedFile;
// *************** Выбор файла для отложенного удаления *****************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Mask, ToolEdit, Buttons;

type
  TSelectDeletedFile = class(TForm)
    feFile: TFilenameEdit;
    Label1: TLabel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    procedure feFileAfterDialog(Sender: TObject; var Name: String;
      var Action: Boolean);
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
   function Execute : boolean;
  end;

var
  SelectDeletedFile: TSelectDeletedFile;

function ExecuteSelectDeletedFile : string;

implementation
uses zTranslate;
{$R *.dfm}

function ExecuteSelectDeletedFile : string;
begin
 SelectDeletedFile := nil;
 Result := '';
 SelectDeletedFile := TSelectDeletedFile.Create(nil);
 try
  if SelectDeletedFile.Execute then
   Result := Trim(SelectDeletedFile.feFile.Text);
 except
 end;
end;

{ TSelectDeletedFile }

function TSelectDeletedFile.Execute: boolean;
begin
 SelectDeletedFile.feFile.Text := '';
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TSelectDeletedFile.feFileAfterDialog(Sender: TObject;
  var Name: String; var Action: Boolean);
begin
  Name := StringReplace(Name, '"', '', [rfReplaceAll]);
end;

procedure TSelectDeletedFile.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
