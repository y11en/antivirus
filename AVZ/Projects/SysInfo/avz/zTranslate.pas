unit zTranslate;

interface
uses zAVKernel,  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  StdCtrls, ComCtrls, Dialogs,ValEdit, CheckLst,
  ExtCtrls, Buttons, ToolEdit, Menus, math,
  zFilterCmp, // Мой компонент-фильтр
  MSGDLG;     // Подмена для MsgDLG
var
 // Код локализации
 LangCode       : string = 'RU';
 // Шрифт, подставляемый вместо стандартного
 LangLocaleFont : string = '';
 LangLocaleFontCharset : byte = RUSSIAN_CHARSET;

// Перевод строки. Выполняется, если
function TranslateStr(S: string): string;
// Локализованное сообщение типа MessageDLG
function zMessageDlg(const Msg: string; DlgType: TMsgDlgType;
  Buttons: TMsgDlgButtons; HelpCtx: Longint): Integer;
procedure zShowMessage(S : string);
// Перевод формы
procedure TranslateForm(AForm : TForm);
// Перевод списка строк
procedure TranslateLines(ALines : TStrings);
// Перевод глобальных переменных
procedure TranslateApp;
{}


implementation

function TranslateStr(S: string): string;
begin
 Result := TranslateSys.TranslateStr(S);
end;

procedure TranslateStrVar(var S: string);
begin
 S := TranslateSys.TranslateStr(S);
end;

// Перевод списка строк
procedure TranslateLines(ALines : TStrings);
var
 i : integer;
begin
 if ALines = nil then exit;
 for i := 0 to ALines.Count - 1 do
  ALines[i] := TranslateStr(ALines[i]);
end;

// Перевод глобальных переменных
procedure TranslateApp;
var
 i : integer;
begin
 Application.Hint  := TranslateStr(Application.Hint);
 Application.Title := TranslateStr(Application.Title);
 // Перевод сообщений в диалогах
 for i := Low(ButtonCaptions) to High(ButtonCaptions) do
  MSGDLG.ButtonCaptions[i] := TranslateStr(MSGDLG.ButtonCaptions[i]);
 for i := Low(DialogTitles) to High(DialogTitles) do
  DialogTitles[i] := TranslateStr(DialogTitles[i]);
end;

function zMessageDlg(const Msg: string; DlgType: TMsgDlgType;
  Buttons: TMsgDlgButtons; HelpCtx: Longint): Integer;
begin
 Result := Msgdlg.MessageDialog(TranslateStr(Msg), DlgType, Buttons, HelpCtx);
end;

procedure zShowMessage(S : string);
begin
 Msgdlg.MessageDialog(TranslateStr(S), mtInformation, [mbOk], 0);
end;

// Перевод формы
procedure TranslateForm(AForm : TForm);
var
 i, j, x : integer;
 procedure SetupFont(AFont : TFont);
 begin
  if LangLocaleFont <> '' then
   AFont.Name    := LangLocaleFont;
  AFont.Charset := LangLocaleFontCharset;
 end;
begin
 AForm.Caption := TranslateStr(AForm.Caption);
 AForm.Hint    := TranslateStr(AForm.Hint);
 AForm.ParentFont := false;
 SetupFont(AForm.Font);
 // Цикл по всем компонентам
 for i := 0 to AForm.ComponentCount-1 do begin
  // TControl
  if AForm.Components[i] is TControl then begin
   (AForm.Components[i] as TControl).Hint    := TranslateStr((AForm.Components[i] as TControl).Hint);
  end;
  // TCustomEdit
  if AForm.Components[i] is TCustomEdit  then begin
   (AForm.Components[i] as TCustomEdit).Text    := TranslateStr((AForm.Components[i] as TCustomEdit).Text);
  end;
  // TCustomComboEdit
  if AForm.Components[i] is TCustomComboEdit  then begin
   (AForm.Components[i] as TCustomComboEdit).Text    := TranslateStr((AForm.Components[i] as TCustomComboEdit).Text);
  end;
  // Label
  if AForm.Components[i] is TLabel then begin
   (AForm.Components[i] as TLabel).Caption := TranslateStr((AForm.Components[i] as TLabel).Caption);
   (AForm.Components[i] as TLabel).ParentFont := false;
   SetupFont((AForm.Components[i] as TLabel).Font);
   Continue;
  end;
  // CheckBox
  if AForm.Components[i] is TCheckBox then begin
   (AForm.Components[i] as TCheckBox).Caption := TranslateStr((AForm.Components[i] as TCheckBox).Caption);
   (AForm.Components[i] as TCheckBox).ParentFont := false;
   SetupFont((AForm.Components[i] as TCheckBox).Font);
   Continue;
  end;
  // CheckListBox
  if AForm.Components[i] is TCheckListBox then begin
   (AForm.Components[i] as TCheckListBox).ParentFont := false;
   (AForm.Components[i] as TCheckListBox).ParentFont := false;
   SetupFont((AForm.Components[i] as TCheckListBox).Font);
   Continue;
  end;
  // TPanel
  if AForm.Components[i] is TPanel then begin
   (AForm.Components[i] as TPanel).Caption := TranslateStr((AForm.Components[i] as TPanel).Caption);
   (AForm.Components[i] as TPanel).ParentFont := false;
   SetupFont((AForm.Components[i] as TPanel).Font);
   Continue;
  end;
  // TMemo
  if AForm.Components[i] is TMemo then begin
   (AForm.Components[i] as TMemo).ParentFont := false;
   SetupFont((AForm.Components[i] as TMemo).Font);
   Continue;
  end;
  // TRichEdit
  if AForm.Components[i] is TRichEdit then begin
   (AForm.Components[i] as TRichEdit).ParentFont := false;
   SetupFont((AForm.Components[i] as TRichEdit).Font);
   Continue;
  end;
  // TGroupBox
  if AForm.Components[i] is TGroupBox then begin
   (AForm.Components[i] as TGroupBox).Caption := TranslateStr((AForm.Components[i] as TGroupBox).Caption);
   (AForm.Components[i] as TGroupBox).ParentFont := false;
   SetupFont((AForm.Components[i] as TGroupBox).Font);
   Continue;
  end;
  // TButton
  if AForm.Components[i] is TButton then begin
   (AForm.Components[i] as TButton).Caption := TranslateStr((AForm.Components[i] as TButton).Caption);
   (AForm.Components[i] as TButton).ParentFont := false;
   SetupFont((AForm.Components[i] as TButton).Font);
   Continue;
  end;
  // TSpeedButton
  if AForm.Components[i] is TSpeedButton then begin
   (AForm.Components[i] as TSpeedButton).Caption := TranslateStr((AForm.Components[i] as TSpeedButton).Caption);
   (AForm.Components[i] as TSpeedButton).ParentFont := false;
   (AForm.Components[i] as TSpeedButton).ParentFont := false;
   SetupFont((AForm.Components[i] as TSpeedButton).Font);
   Continue;
  end;
  // TTabSheet
  if AForm.Components[i] is TTabSheet then begin
   (AForm.Components[i] as TTabSheet).Caption := TranslateStr((AForm.Components[i] as TTabSheet).Caption);
   (AForm.Components[i] as TTabSheet).ParentFont := false;
   (AForm.Components[i] as TTabSheet).ParentFont := false;
   SetupFont((AForm.Components[i] as TTabSheet).Font);
   Continue;
  end;
  // TSaveDialog
  if AForm.Components[i] is TSaveDialog then begin
   (AForm.Components[i] as TSaveDialog).Title  := TranslateStr((AForm.Components[i] as TSaveDialog).Title);
   (AForm.Components[i] as TSaveDialog).Filter := TranslateStr((AForm.Components[i] as TSaveDialog).Filter);
   Continue;
  end;
  // TOpenDialog
  if AForm.Components[i] is TOpenDialog then begin
   (AForm.Components[i] as TOpenDialog).Title := TranslateStr((AForm.Components[i] as TOpenDialog).Title);
   (AForm.Components[i] as TOpenDialog).Filter := TranslateStr((AForm.Components[i] as TOpenDialog).Filter);
   Continue;
  end;
  // TMenuItem
  if AForm.Components[i] is TMenuItem then begin
   (AForm.Components[i] as TMenuItem).Caption := TranslateStr((AForm.Components[i] as TMenuItem).Caption);
   (AForm.Components[i] as TMenuItem).Hint    := TranslateStr((AForm.Components[i] as TMenuItem).Hint);
   Continue;
  end;
  // TMainMenu
  if AForm.Components[i] is TMainMenu then begin
   for j := 0 to (AForm.Components[i] as TMainMenu).Items.Count - 1 do begin
    (AForm.Components[i] as TMainMenu).Items[j].Caption := TranslateStr((AForm.Components[i] as TMainMenu).Items[j].Caption);
    (AForm.Components[i] as TMainMenu).Items[j].Hint   := TranslateStr((AForm.Components[i] as TMainMenu).Items[j].Hint);
   end;
   Continue;
  end;
  // TComboBox
  if AForm.Components[i] is TComboBox then begin
   x := (AForm.Components[i] as TComboBox).ItemIndex;
   for j := 0 to (AForm.Components[i] as TComboBox).Items.Count - 1 do
    (AForm.Components[i] as TComboBox).Items[j] := TranslateStr((AForm.Components[i] as TComboBox).Items[j]);
   (AForm.Components[i] as TComboBox).ItemIndex := x;
   (AForm.Components[i] as TComboBox).ParentFont := false;
   (AForm.Components[i] as TComboBox).ParentFont := false;
   SetupFont((AForm.Components[i] as TComboBox).Font);
   Continue;
  end;
  // TRadioGroup
  if AForm.Components[i] is TRadioGroup then begin
   (AForm.Components[i] as TRadioGroup).Caption := TranslateStr((AForm.Components[i] as TRadioGroup).Caption);
   x := (AForm.Components[i] as TRadioGroup).ItemIndex;
   for j := 0 to (AForm.Components[i] as TRadioGroup).Items.Count - 1 do
    (AForm.Components[i] as TRadioGroup).Items[j] := TranslateStr((AForm.Components[i] as TRadioGroup).Items[j]);
   (AForm.Components[i] as TRadioGroup).ItemIndex := x;
   (AForm.Components[i] as TRadioGroup).ParentFont := false;
   SetupFont((AForm.Components[i] as TRadioGroup).Font);
   Continue;
  end;
  // TRadioButton
  if AForm.Components[i] is TRadioButton then begin
   (AForm.Components[i] as TRadioButton).Caption := TranslateStr((AForm.Components[i] as TRadioButton).Caption);
   (AForm.Components[i] as TRadioButton).Hint := TranslateStr((AForm.Components[i] as TRadioButton).Hint);
   SetupFont((AForm.Components[i] as TRadioButton).Font);
   Continue;
  end;
  // TListView
  if AForm.Components[i] is TListView then begin
   for j := 0 to (AForm.Components[i] as TListView).Columns.Count - 1 do
    (AForm.Components[i] as TListView).Columns[j].Caption := TranslateStr((AForm.Components[i] as TListView).Columns[j].Caption);
   (AForm.Components[i] as TListView).ParentFont := false;
   SetupFont((AForm.Components[i] as TListView).Font);
   Continue;
  end;
  // TTreeView
  if AForm.Components[i] is TTreeView then begin
   (AForm.Components[i] as TTreeView).ParentFont := false;
   SetupFont((AForm.Components[i] as TTreeView).Font);
   Continue;
  end;
  // TValueListEditor
  if AForm.Components[i] is TValueListEditor then begin
   for j := 0 to (AForm.Components[i] as TValueListEditor).TitleCaptions.Count - 1 do
    (AForm.Components[i] as TValueListEditor).TitleCaptions[j] := TranslateStr((AForm.Components[i] as TValueListEditor).TitleCaptions[j]);
   (AForm.Components[i] as TValueListEditor).ParentFont := false;
   SetupFont((AForm.Components[i] as TValueListEditor).Font);
   Continue;
  end;
  // TFileNameEdit
  if AForm.Components[i] is TFilenameEdit then begin
   (AForm.Components[i] as TFilenameEdit).Filter := TranslateStr((AForm.Components[i] as TFilenameEdit).Filter);
   SetupFont((AForm.Components[i] as TFilenameEdit).Font);
   Continue;
  end;
  // TFilterEdit
  if AForm.Components[i] is TFilterEdit then begin
   (AForm.Components[i] as TFilterEdit).Caption := TranslateStr((AForm.Components[i] as TFilterEdit).Caption);
   (AForm.Components[i] as TFilterEdit).FilterEditMode := (AForm.Components[i] as TFilterEdit).FilterEditMode;
   (AForm.Components[i] as TFilterEdit).ValidateValue;
   (AForm.Components[i] as TFilterEdit).FFilterCombo.ParentFont := false;
   SetupFont((AForm.Components[i] as TFilterEdit).FFilterCombo.Font);
   (AForm.Components[i] as TFilterEdit).FCaptionLabel.ParentFont := false;
   SetupFont((AForm.Components[i] as TFilterEdit).FCaptionLabel.Font);
   (AForm.Components[i] as TFilterEdit).FFilterEdit1.ParentFont := false;
   SetupFont((AForm.Components[i] as TFilterEdit).FFilterEdit1.Font);
   (AForm.Components[i] as TFilterEdit).FFilterEdit2.ParentFont := false;
   SetupFont((AForm.Components[i] as TFilterEdit).FFilterEdit2.Font);
   Continue;
  end;
 end;
 TranslateApp;
end;

{
function MessageDlgEx(Msg: String; DlgType: TMsgDlgtype; Buttons: TMsgDlgButtons): integer;
var
 Dlg: TForm;
 i: cardinal;
begin
 Dlg:=CreateMessageDialog(Msg, DlgType, Buttons);
 if DlgType=mtWarning then Dlg.Caption := '';
 for i := 0 to Dlg.ComponentCount-1 do
  if Dlg.Components[i] is TButton then begin
   if TButton(Dlg.Components[i]).ModalResult=mrYes then TButton(Dlg.Components[i]).Caption:='';
  end;
 Result:=Dlg.Showmodal;
 Dlg.Free
end;
}

begin

end.

