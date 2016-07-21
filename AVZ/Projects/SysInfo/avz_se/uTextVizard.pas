unit uTextVizard;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ExtCtrls, Buttons, clipbrd, INIFiles, zTranslate;

type
  TemplateInfo = record
   Filename     : string; // Имя файла шаблона
   TemplateName : string; // Имя самого шаблона
   Template     : string; // Шаблона
  end;

  TTextWizard = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    cbTemplate: TComboBox;
    mmRes: TMemo;
    Panel2: TPanel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    procedure cbTemplateChange(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  public
   FScript : string;
   // Массив профилей
   Templates : array of TemplateInfo;
   // Обновление списка профилей
   function RefreshTemplateList(ADefTemplate : string) : boolean;
   // Обновление спика профилей
   function RefreshTemplate : boolean;
   // Выполнение диалога
   function Execute(AScript : string) : boolean;
  end;

var
  TextWizard: TTextWizard;

implementation
uses unit1;
{$R *.dfm}

{ TTextVizard }

function TTextWizard.Execute(AScript : string): boolean;
var
 S : string;
 INI : TIniFile;
begin
 FScript := trim(AScript);
 // Загрузка настроек
 try
  INI := TIniFile.Create(INIFileName);
  S   := INI.ReadString('TextWizard', 'LastTemplate', '');
  INI.Free;
 except
 end;
 RefreshTemplateList(S);
 ShowModal;
 Result := ModalResult = mrOk;
 // Запись настроек
 try
  INI := TIniFile.Create(INIFileName);
  if cbTemplate.ItemIndex >= 0 then S := ExtractFilename(Templates[cbTemplate.ItemIndex].FileName)
   else S := '-';
  INI.WriteString('TextWizard', 'LastTemplate', S);
  INI.Free;
 except
 end;
end;

function TTextWizard.RefreshTemplateList(ADefTemplate : string) : boolean;
var
 ProfilePath : string;
 SR : TSearchRec;
 Res : integer;
 Lines : TStringList;
begin
 Templates := nil;
 cbTemplate.Items.Clear;
 Lines := TStringList.Create;
 ProfilePath := ExtractFilePath(Application.ExeName)+'Profile\';
 ForceDirectories(ProfilePath);
 Res := FindFirst(ProfilePath + '*.prf', faAnyFile, SR);
 while Res = 0 do begin
  Lines.LoadFromFile(ProfilePath + SR.Name);
  if Lines.Count > 0 then begin
   SetLength(Templates, length(Templates) + 1);
   Templates[length(Templates) - 1].Filename := ProfilePath + SR.Name;
   Templates[length(Templates) - 1].TemplateName := Trim(Lines[0]);
   Lines.Delete(0);
   Templates[length(Templates) - 1].Template := Lines.Text;
   cbTemplate.Items.AddObject(Templates[length(Templates) - 1].TemplateName,
                              pointer(length(Templates) - 1));
   if AnsiLowerCase(SR.Name) = AnsiLowerCase(ADefTemplate) then
    cbTemplate.ItemIndex := cbTemplate.Items.Count - 1;
  end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
 Lines.Free;
 if cbTemplate.ItemIndex < 0 then
  if cbTemplate.Items.Count > 0 then
   cbTemplate.ItemIndex := 0;
 RefreshTemplate;
end;

procedure TTextWizard.cbTemplateChange(Sender: TObject);
begin
 RefreshTemplate;
end;

function TTextWizard.RefreshTemplate: boolean;
var
 S : string;
begin
 if cbTemplate.ItemIndex < 0 then exit;
 S := Templates[integer(cbTemplate.Items.Objects[cbTemplate.ItemIndex])].Template;
 s := StringReplace(S, '$SCRIPT$', FScript, [rfReplaceAll, rfIgnoreCase]);
 mmRes.Lines.Text := S;
end;

procedure TTextWizard.BitBtn1Click(Sender: TObject);
begin
 Clipboard.AsText := mmRes.Lines.Text;
end;

procedure TTextWizard.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
