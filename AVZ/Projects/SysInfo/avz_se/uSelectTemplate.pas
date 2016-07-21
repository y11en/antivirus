unit uSelectTemplate;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Buttons, ExtCtrls, zTranslate;

type
  TemplateInfo = record
   Filename     : string; // Имя файла шаблона
   TemplateName : string; // Имя самого шаблона
   Template     : string; // Шаблона
  end;

  TSelectTemplateDLG = class(TForm)
    Panel1: TPanel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    lbTemplates: TListBox;
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
   // Массив профилей
   Templates : array of TemplateInfo;
   Script : string;
   // Обновление списка профилей
   function RefreshTemplateList : boolean;
   function Execute : boolean;
  end;

var
  SelectTemplateDLG: TSelectTemplateDLG;

implementation

{$R *.dfm}

{ TSelectTemplateDLG }

function TSelectTemplateDLG.Execute: boolean;
begin
 Script := '';
 RefreshTemplateList;
 ShowModal;
 Result := ModalResult = mrOk;
 if lbTemplates.ItemIndex >= 0 then
  Script := Templates[integer(lbTemplates.Items.Objects[lbTemplates.ItemIndex])].Template;
end;

function TSelectTemplateDLG.RefreshTemplateList: boolean;
var
 ProfilePath : string;
 SR : TSearchRec;
 Res : integer;
 Lines : TStringList;
begin
 Templates := nil;
 lbTemplates.Items.Clear;
 Lines := TStringList.Create;
 ProfilePath := ExtractFilePath(Application.ExeName)+'Template\';
 ForceDirectories(ProfilePath);
 Res := FindFirst(ProfilePath + '*.txt', faAnyFile, SR);
 while Res = 0 do begin
  Lines.LoadFromFile(ProfilePath + SR.Name);
  if Lines.Count > 0 then begin
   SetLength(Templates, length(Templates) + 1);
   Templates[length(Templates) - 1].Filename := ProfilePath + SR.Name;
   Templates[length(Templates) - 1].TemplateName := Trim(Lines[0]);
   Lines.Delete(0);
   Templates[length(Templates) - 1].Template := Lines.Text;
   lbTemplates.Items.AddObject(Templates[length(Templates) - 1].TemplateName,
                              pointer(length(Templates) - 1));
  end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
 Lines.Free;
 if lbTemplates.Items.Count > 0 then
  lbTemplates.ItemIndex := 0;
end;

procedure TSelectTemplateDLG.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
