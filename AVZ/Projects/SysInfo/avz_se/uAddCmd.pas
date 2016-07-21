unit uAddCmd;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, Buttons, RXCtrls, ComCtrls, Mask, ToolEdit,
  Grids, ValEdit, inifiles, zTranslate;

type
  TAddCmdDLG = class(TForm)
    Panel1: TPanel;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    PageControl: TPageControl;
    tsCmdList: TTabSheet;
    TextListBox: TTextListBox;
    GroupBox1: TGroupBox;
    Label1: TLabel;
    cbGroupFilter: TComboBox;
    Label2: TLabel;
    ceFilter: TComboEdit;
    StatusBar: TStatusBar;
    cbComment: TCheckBox;
    tsTop20: TTabSheet;
    Top20ListBox: TTextListBox;
    procedure ceFilterChange(Sender: TObject);
    procedure cbGroupFilterChange(Sender: TObject);
    procedure TextListBoxClick(Sender: TObject);
    procedure TextListBoxDblClick(Sender: TObject);
    procedure FormShow(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure PageControlChange(Sender: TObject);
  private
    function GetCommandTxt: string;
    function GetCmdIndx: integer;
  public
   Top20List : TStringList;
   function LoadTop20List : boolean;
   function SaveTop20List : boolean;
   function AddTop20(ACmd : string) : boolean;
   // Обновление списка команд
   function RefreshList : boolean;
   // Обновление списка Top20
   function RefreshTop20List : boolean;
   // Выполнение диалога
   Function Execute : boolean;
   // Текст команды
   property CommandTxt : string read GetCommandTxt;
   property CmdIndx    : integer read GetCmdIndx;
  end;

var
  AddCmdDLG: TAddCmdDLG;

implementation

uses Unit1, zAntivirus;

{$R *.dfm}

{ TForm2 }

function TAddCmdDLG.Execute: boolean;
var
 i : integer;
 S : string;
 INI : TIniFile;
begin
 LoadTop20List;
 cbGroupFilter.Items.Clear;
 cbGroupFilter.Items.AddObject(TranslateStr('$AVZ0154'), pointer(maxint));
 cbGroupFilter.ItemIndex := 0;
 for i := 0 to Length(Main.VSEData.CmdGroups) - 1 do
  cbGroupFilter.Items.AddObject(TranslateStr(Main.VSEData.CmdGroups[i].Naim),
                                pointer(Main.VSEData.CmdGroups[i].GroupID));
 RefreshList;
 RefreshTop20List;
  // Чтение настроек
 try
  INI := TIniFile.Create(INIFileName);
  cbComment.Checked := INI.ReadBool('AddCmd', 'Comment', true);
  S := LowerCase(INI.ReadString('AddCmd', 'LastCmd', ''));
  if S <> '' then
   if PageControl.ActivePage = tsCmdList then
    for i := 0 to TextListBox.Items.Count do
     if pos(S, LowerCase(TextListBox.Items[i])) = 1 then begin
      TextListBox.ItemIndex := i;
      TextListBoxClick(TextListBox);
      Break;
     end;
  INI.Free;
 except
 end;
 ShowModal;
 Result := ModalResult = mrOk;
 if Result then begin
  AddTop20(Main.VSEData.CmdDescr[CmdIndx].Naim);
  SaveTop20List;
  // Запись настроек
  try
   INI := TIniFile.Create(INIFileName);
   INI.WriteBool('AddCmd', 'Comment', cbComment.Checked);
   if CmdIndx >= 0 then S := Main.VSEData.CmdDescr[CmdIndx].Naim
    else S := '-';
   INI.WriteString('AddCmd', 'LastCmd',  S);
   INI.Free;
  except
  end;
 end;
end;

function TAddCmdDLG.RefreshList: boolean;
var
 i, FilterGroup : integer;
 S, st : string;
begin
 StatusBar.SimpleText := '';
 TextListBox.Items.Clear;
 FilterGroup := maxint;
 if cbGroupFilter.ItemIndex >= 0 then
  FilterGroup := integer(cbGroupFilter.Items.Objects[cbGroupFilter.ItemIndex]);
 S := AnsiLowerCase(trim(ceFilter.Text));
 for i := 0 to Length(Main.VSEData.CmdDescr) - 1 do
  if (FilterGroup = maxint) or (Main.VSEData.CmdDescr[i].GroupID = FilterGroup) then
   if (S = '') or (pos(S, AnsiLowerCase(Main.VSEData.CmdDescr[i].CmdTxt)) > 0) or
     (pos(S, AnsiLowerCase(Main.VSEData.CmdDescr[i].Cmt)) > 0) then begin
      ST := Main.VSEData.CmdDescr[i].CmdTxt;
      ST := StringReplace(ST, 'procedure ', '', [rfIgnoreCase]);
      ST := StringReplace(ST, 'function ', '', [rfIgnoreCase]);
      TextListBox.Items.AddObject(st {+ ' - ' +  Main.VSEData.CmdDescr[i].Cmt},
                                   pointer(i));
   end;
end;

procedure TAddCmdDLG.ceFilterChange(Sender: TObject);
begin
 RefreshList;
end;

procedure TAddCmdDLG.cbGroupFilterChange(Sender: TObject);
begin
 RefreshList;
end;

procedure TAddCmdDLG.TextListBoxClick(Sender: TObject);
var
 Indx : integer;
begin
 StatusBar.SimpleText := '';
 // Индекс команды
 Indx := CmdIndx;
 if Indx < 0 then exit;
 // Текст команды
 StatusBar.SimpleText := TranslateStr(Main.VSEData.CmdDescr[Indx].Cmt);
end;

procedure TAddCmdDLG.TextListBoxDblClick(Sender: TObject);
begin
 if (Sender as TTextListBox).ItemIndex > 0 then
  ModalResult := mrOk;
end;

function TAddCmdDLG.GetCommandTxt: string;
var
 Indx : integer;
begin
 Result := '';
 if CmdIndx >= 0 then
  Result := Main.FormatCommandTxt(CmdIndx, cbComment.Checked);
end;

procedure TAddCmdDLG.FormShow(Sender: TObject);
begin
 try TextListBox.SetFocus; except end;
 try Top20ListBox.SetFocus; except end;
end;

function TAddCmdDLG.RefreshTop20List: boolean;
var
 i : integer;
 st : string;
begin
 StatusBar.SimpleText := '';
 Top20ListBox.Items.Clear;
 for i := 0 to Length(Main.VSEData.CmdDescr) - 1 do
  if Top20List.IndexOf(Main.VSEData.CmdDescr[i].Naim) >= 0 then begin
   ST := Main.VSEData.CmdDescr[i].CmdTxt;
   ST := StringReplace(ST, 'procedure ', '', [rfIgnoreCase]);
   ST := StringReplace(ST, 'function ', '', [rfIgnoreCase]);
   Top20ListBox.Items.AddObject(st {+ ' - ' +  Main.VSEData.CmdDescr[i].Cmt},
                                   pointer(i));
   end;
end;

procedure TAddCmdDLG.FormCreate(Sender: TObject);
begin
 Top20List := TStringList.Create;
 TranslateForm(Self);
end;

function TAddCmdDLG.AddTop20(ACmd: string): boolean;
var
 Poz : integer;
 S   : string;
begin
 Result := false;
 ACmd := Trim(ACmd);
 if ACmd = '' then exit;
 Poz := Top20List.IndexOf(ACmd);
 if Poz < 0 then begin
  Top20List.Add(ACmd);
  while Top20List.Count > 20 do
   Top20List.Delete(0);
 end else begin
  S := Top20List[Poz];
  Top20List.Delete(Poz);
  Top20List.Add(S);
 end;
end;

function TAddCmdDLG.LoadTop20List: boolean;
var
 INI : TIniFile;
 i   : integer;
 S   : string;
begin
 Result := false;
 try
   INI := TIniFile.Create(INIFileName);
   Top20List.Clear;
   for i := 1 to 20 do begin
    S := Trim(INI.ReadString('Top20', 'Item'+inttostr(i), ''));
    if S <> '' then
     if Top20List.IndexOf(S) < 0 then
      Top20List.Add(S);
   end;
  Result := true;
  INI.Free;
 except
 end;
end;

function TAddCmdDLG.SaveTop20List: boolean;
var
 INI : TIniFile;
 i   : integer;
 S   : string;
begin
 Result := false;
 try
   INI := TIniFile.Create(INIFileName);
   INI.EraseSection('Top20');
   for i := 0 to Top20List.Count-1 do
    INI.WriteString('Top20', 'Item'+inttostr(i+1), Top20List[i]);
   Result := true;
   INI.Free;
 except
 end;
end;

function TAddCmdDLG.GetCmdIndx: integer;
begin
 Result := -1;
 if PageControl.ActivePage = tsCmdList then begin
  if TextListBox.ItemIndex < 0 then exit;
  // Индекс команды
  Result := integer(TextListBox.Items.Objects[TextListBox.ItemIndex]);
  exit;
 end;
 if PageControl.ActivePage = tsTop20 then begin
  if Top20ListBox.ItemIndex < 0 then exit;
  // Индекс команды
  Result := integer(Top20ListBox.Items.Objects[Top20ListBox.ItemIndex]);
 end;
end;

procedure TAddCmdDLG.PageControlChange(Sender: TObject);
begin
 if PageControl.ActivePage = tsCmdList then RefreshList;
 if PageControl.ActivePage = tsTop20   then RefreshTop20List;
end;

end.
