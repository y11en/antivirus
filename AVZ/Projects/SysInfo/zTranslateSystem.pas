unit zTranslateSystem;

interface
uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms,
  StdCtrls, ComCtrls, clipbrd, Dialogs,
  ExtCtrls, Buttons, ToolEdit, Menus, math;

// function CompListDistinct(AList : TStrings; AForm : TForm) : boolean;
// function AddProperty(AComponentName, AName, AValue : String; AForm : TForm) : boolean;
implementation
uses unit1;

function AddProperty(AComponentName, AName, AValue : String; AForm : TForm) : boolean;
begin
end;

function CompListDistinct(AList : TStrings; AForm : TForm) : boolean;
var
 i : integer;
 Cmp : TObject;
begin
{ for i := 0 to AForm.ComponentCount - 1 do
  if AList.IndexOf(AForm.Components[i].ClassName) < 0 then
   AList.Add(AForm.Components[i].ClassName);}

 for i := 0 to AForm.ComponentCount - 1 do begin
  Cmp := AForm.Components[i];
  if Cmp is TControl then begin
   AddProperty((Cmp as TControl).Name, 'Hint', (Cmp as TControl).Hint, AForm);
  end;
  if Cmp is TLabel then begin
   AddProperty((Cmp as TLabel).Name, 'Caption', (Cmp as TLabel).Caption, AForm);
  end;
  if Cmp is TCheckBox then begin
   AddProperty((Cmp as TCheckBox).Name, 'Caption', (Cmp as TCheckBox).Caption, AForm);
  end;
  if Cmp is TMenuItem then begin
   AddProperty((Cmp as TMenuItem).Name, 'Caption', (Cmp as TMenuItem).Caption, AForm);
   AddProperty((Cmp as TMenuItem).Name, 'Hint', (Cmp as TMenuItem).Hint, AForm);
  end;
  if Cmp is TRadioGroup then begin
   AddProperty((Cmp as TRadioGroup).Name, 'Caption', (Cmp as TRadioGroup).Caption, AForm);
  end;
  if Cmp is TComboBox then begin
   AddProperty((Cmp as TComboBox).Name, 'Text', (Cmp as TComboBox).Text, AForm);
  end;
  if Cmp is TBitBtn then begin
   AddProperty((Cmp as TBitBtn).Caption, 'Caption', (Cmp as TBitBtn).Caption, AForm);
  end;
  if Cmp is TButton then begin
   AddProperty((Cmp as TButton).Caption, 'Caption', (Cmp as TButton).Caption, AForm);
  end;
  if Cmp is TSaveDialog then begin
   AddProperty((Cmp as TSaveDialog).Title, 'Title', (Cmp as TSaveDialog).Title, AForm);
  end;
  if Cmp is TOpenDialog then begin
   AddProperty((Cmp as TOpenDialog).Title, 'Title', (Cmp as TOpenDialog).Title, AForm);
  end;
 end;
end;
end.
