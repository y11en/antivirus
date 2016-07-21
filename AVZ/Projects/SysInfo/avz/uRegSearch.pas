unit uRegSearch;
// ************** Поиск в реестре **************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Menus, StdCtrls, ComCtrls, Buttons, ExtCtrls, registry, zSharedFunctions;

type
  TRegSearch = class(TForm)
    StatusBar1: TStatusBar;
    Panel1: TPanel;
    Label1: TLabel;
    edSearchTest: TEdit;
    mbStart: TBitBtn;
    mbStop: TBitBtn;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    LogMemo: TMemo;
    TabSheet2: TTabSheet;
    lvFoundKeys: TListView;
    BitBtn1: TBitBtn;
    BitBtn2: TBitBtn;
    TabSheet3: TTabSheet;
    gbSearchRootKeys: TGroupBox;
    cbSearchInHKCU: TCheckBox;
    cbSearchInHKLM: TCheckBox;
    cbSearchInHKUS: TCheckBox;
    cbSearchInHKCR: TCheckBox;
    gbSearchInVal: TGroupBox;
    cbSearchInParamName: TCheckBox;
    cbSearchInKeyName: TCheckBox;
    cbSearchInParamVal: TCheckBox;
    SaveDialog1: TSaveDialog;
    PopupMenu1: TPopupMenu;
    pmSelectAll: TMenuItem;
    pmUnSelectAll: TMenuItem;
    pmInvertSelect: TMenuItem;
    SaveDialog2: TSaveDialog;
    BitBtn3: TBitBtn;
    procedure mbStartClick(Sender: TObject);
    procedure mbStopClick(Sender: TObject);
    procedure pmSelectAllClick(Sender: TObject);
    procedure pmUnSelectAllClick(Sender: TObject);
    procedure pmInvertSelectClick(Sender: TObject);
    procedure BitBtn1Click(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure BitBtn3Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
   procedure AddFoundKey(ARootKey: HKEY; APath, AKeyName, AValue: string; AType : integer);
   procedure AddToLog(s : string);
    { Private declarations }
  public
   GlobalKeyCounter, GlobalCounter : integer;
   StopF : boolean;
   function ScanRegistryKey(ARootKey : HKEY; APath : string) : boolean;
   function Execute(ASearchStr : string) : boolean;
  end;

var
  RegSearch: TRegSearch;

Function ExecuteRegSearch(ASearchStr : string) : boolean;

implementation
uses zTranslate;
{$R *.dfm}


function GetKeyName(ARootKey : HKEY) : string;
begin
 Result := '';
 case ARootKey of
  HKEY_LOCAL_MACHINE : Result := 'HKEY_LOCAL_MACHINE';
  HKEY_CURRENT_USER  : Result := 'HKEY_CURRENT_USER';
  HKEY_CLASSES_ROOT  : Result := 'HKEY_CLASSES_ROOT';
  HKEY_USERS         : Result := 'HKEY_USERS';
 end;
end;

procedure TRegSearch.AddFoundKey(ARootKey : HKEY; APath, AKeyName, AValue : string; AType : integer);
begin
 AddToLog(GetKeyName(ARootKey)+APath + '\'+ AKeyName + ' = ' + AValue);
 with lvFoundKeys.Items.Add do begin
  Caption := GetKeyName(ARootKey)+APath;
  Data    := pointer(ARootKey);
  SubItems.Add(AKeyName);
  SubItems.Add(AValue);
  SubItems.Add(APath);
  SubItems.Add(IntToStr(AType));
 end;
end;

function TRegSearch.ScanRegistryKey(ARootKey : HKEY; APath : string): boolean;
var
 Reg   : TRegistry;
 Lines : TStrings;
 i, KeyNum : integer;
 S : string;
begin
 if StopF then exit;
 KeyNum := 0;
 StatusBar1.SimpleText := APath;
 Reg := TRegistry.Create;
 Lines := TStringList.Create;
 Reg.RootKey := ARootKey;
 // Поиск в именах ключей
 if cbSearchInKeyName.Checked and (pos(edSearchTest.Text, AnsiLowerCase(APath)) > 0) then
  AddFoundKey(ARootKey, APath, '', '', 0);
 // Сканирование ключей реестра
 if Reg.OpenKey(APath, false) then begin
  Reg.GetKeyNames(Lines);
  for i := 0 to Lines.Count - 1 do
   if Lines[i] <> '' then begin
    ScanRegistryKey(ARootKey, APath + '\'+ Lines[i]);
   end;
  // Проверка значений
  Lines.Clear;
  Reg.GetValueNames(Lines);
  for i := 0 to Lines.Count - 1 do
   if (Reg.GetDataType(Lines[i]) = rdString) or (Reg.GetDataType(Lines[i]) = rdExpandString) then begin
    S := AnsiLowerCase(Reg.ReadString(Lines[i]));
    // Поиск в именах разделов
    if cbSearchInParamName.Checked and (pos(edSearchTest.Text, AnsiLowerCase(Lines[i])) > 0) then
     AddFoundKey(ARootKey, APath, Lines[i], '', 1);
    // Поиск в значении
    if cbSearchInParamVal.Checked and (pos(edSearchTest.Text, S) > 0) then
     AddFoundKey(ARootKey, APath, Lines[i], Reg.ReadString(Lines[i]), 2);
    inc(KeyNum);
    if KeyNum mod $FF = 0 then
     Application.ProcessMessages;
   end;
  inc(GlobalKeyCounter);
  if GlobalKeyCounter mod 10 = 0 then
   Application.ProcessMessages;
  // Закрыть ключ
  Reg.CloseKey;
 end;
 Lines.Free;
 // Проверка занчений текущего ключа
 Reg.Free;
end;

procedure TRegSearch.mbStartClick(Sender: TObject);
begin
 edSearchTest.Text := Trim(AnsiLowerCase(edSearchTest.Text));
 if edSearchTest.Text = '' then begin
  zMessageDlg('$AVZ0489', mtError, [mbOk], 0);
  exit;
 end;
 LogMemo.Lines.Clear;
 lvFoundKeys.Clear;
 lvFoundKeys.SortType := stNone;
 try
  StopF := false;
  edSearchTest.Enabled := false;
  mbStart.Enabled := false;
  mbStop.Enabled  := true;
  AddToLog('$AVZ0438');
  AddToLog('$AVZ0286 "'+edSearchTest.Text+'"');
  if cbSearchInHKLM.Checked then begin
   AddToLog('-- $AVZ0751 HKEY_LOCAL_MACHINE --');
   ScanRegistryKey(HKEY_LOCAL_MACHINE, '');
  end;
  if cbSearchInHKCU.Checked then begin
   AddToLog('-- $AVZ0751 HKEY_CURRENT_USER --');
   ScanRegistryKey(HKEY_CURRENT_USER, '');
  end;
  if cbSearchInHKCR.Checked then begin
   AddToLog('-- $AVZ0751 HKEY_CLASSES_ROOT --');
   ScanRegistryKey(HKEY_CLASSES_ROOT, '');
  end;
  if cbSearchInHKUS.Checked then begin
   AddToLog('-- $AVZ0751 HKEY_USERS --');
   ScanRegistryKey(HKEY_USERS, '');
  end;
  if StopF then AddToLog('$AVZ0774')
   else AddToLog('-- $AVZ0759 --');
  AddToLog('$AVZ0866: '+IntToStr(GlobalKeyCounter));
 finally
  StatusBar1.SimpleText := '';
  edSearchTest.Enabled   := true;
  mbStart.Enabled := true;
  mbStop.Enabled  := false;
  lvFoundKeys.SortType := stText;
 end;
end;

procedure TRegSearch.mbStopClick(Sender: TObject);
begin
 StopF := true;
end;

procedure TRegSearch.BitBtn1Click(Sender: TObject);
var
 Lines          : TStringList;
 s, st, LastKey     : string;
 i,j : integer;
begin
 if SaveDialog1.Execute and (SaveDialog1.FileName <> '') then begin
   Lines := TStringList.Create;
   Lines.Add('REGEDIT4');
   for i := 0 to lvFoundKeys.Items.Count - 1 do
    if lvFoundKeys.Items[i].Checked then begin
     // Создание группы с именем ключа
     if lvFoundKeys.Items[i].Caption <> LastKey then begin
      Lines.Add('');
      Lines.Add('['+lvFoundKeys.Items[i].Caption+']');
      LastKey := lvFoundKeys.Items[i].Caption;
     end;
     // Создание записи для экспорта ключа
     if lvFoundKeys.Items[i].SubItems[0] <> '' then
      S := '"' + lvFoundKeys.Items[i].SubItems[0] + '"'
       else S := '@';
     st := '';
     for j := 1 to length(lvFoundKeys.Items[i].SubItems[1]) do
      case lvFoundKeys.Items[i].SubItems[1][j] of
        '\'  : st := st+'\\';
        '"'  : st := st+'\"';
        '''' : st := st+'\''';
       else
        st := st+lvFoundKeys.Items[i].SubItems[1][j];
      end;
     S := S + '="' + st + '"';
     Lines.Add(S);
    end;
    Lines.SaveToFile(SaveDialog1.FileName);
  end;
end;

procedure TRegSearch.AddToLog(s: string);
begin
 LogMemo.Lines.Add(TranslateStr(S));
end;

procedure TRegSearch.BitBtn2Click(Sender: TObject);
var
 i : integer;
 Reg : TRegistry;
begin
 if zMessageDlg('$AVZ1160 ??', mtConfirmation, [mbOk, mbCancel], 0) <> mrOk then exit;
 try
   Screen.Cursor := crHourGlass;
   AddToLog('-- $AVZ1077 --');
   Reg := TRegistry.Create;
   // Цикл по ключам
   i := 0;
   while i < lvFoundKeys.Items.Count do
    if lvFoundKeys.Items[i].Checked then begin // Обработка только отмеченных ключей
     // Устнавка ROOT ключа
     Reg.RootKey := HKEY(lvFoundKeys.Items[i].data);
     // Проверка типа - если есть имя параметра, то это параметр
    if StrToInt(lvFoundKeys.Items[i].SubItems[3]) > 0 then begin
      if Reg.OpenKey(lvFoundKeys.Items[i].SubItems[2], False) then begin
       if Reg.DeleteValue(lvFoundKeys.Items[i].SubItems[0]) then begin
        AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].Caption + ', $AVZ0694 '+ lvFoundKeys.Items[i].SubItems[0] + ' $AVZ1070');
        lvFoundKeys.Items[i].Delete;
       end else begin
         if not(Reg.ValueExists(lvFoundKeys.Items[i].SubItems[0])) then begin
          AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].Caption + ', $AVZ0694 '+ lvFoundKeys.Items[i].Caption + ' - $AVZ0622');
          lvFoundKeys.Items[i].Delete;
         end else begin
          AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].Caption + ', $AVZ0694 '+ lvFoundKeys.Items[i].Caption + ' - $AVZ0683');
          inc(i);
         end;
        end;
       Reg.CloseKey;
    end else
     if not(Reg.KeyExists(lvFoundKeys.Items[i].SubItems[2])) then begin
      AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].SubItems[2] + ' - $AVZ0337');
      lvFoundKeys.Items[i].Delete;
     end else begin
      AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].SubItems[2] + ' - $AVZ0683');
      inc(i);
     end;
    end else
      // Это раздел - удаляем его
      if Reg.DeleteKey(lvFoundKeys.Items[i].SubItems[2]) then begin
       AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].Caption + ' $AVZ1070');
       lvFoundKeys.Items[i].Delete;
      end else begin
       // При ошибке проверим причину - быть может, ключа уже нет
       if not(Reg.KeyExists(lvFoundKeys.Items[i].Caption)) then begin
        AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].Caption + ' - $AVZ0337');
        lvFoundKeys.Items[i].Delete;
       end else begin
        AddToLog('$AVZ0336 '+lvFoundKeys.Items[i].Caption + ' - $AVZ0683');
        inc(i);
       end;
      end;
    end else inc(i);
   Reg.Free;
   AddToLog('-- $AVZ1076 --');
 finally
  Screen.Cursor := crDefault;
 end;
end;

procedure TRegSearch.pmSelectAllClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundKeys.Items.Count - 1 do
  lvFoundKeys.Items[i].Checked := true;
end;

procedure TRegSearch.pmUnSelectAllClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundKeys.Items.Count - 1 do
  lvFoundKeys.Items[i].Checked := false;
end;

procedure TRegSearch.pmInvertSelectClick(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFoundKeys.Items.Count - 1 do
  lvFoundKeys.Items[i].Checked := not lvFoundKeys.Items[i].Checked;
end;

function TRegSearch.Execute(ASearchStr : string): boolean;
begin
 edSearchTest.Text := Trim(ASearchStr);
 ShowModal;
 Result := ModalResult = mrOk;
end;

Function ExecuteRegSearch(ASearchStr : string) : boolean;
begin
 try
  RegSearch := TRegSearch.Create(nil);
  RegSearch.Execute(ASearchStr);
 finally
  RegSearch.Free;
  RegSearch := nil;
 end;
end;

procedure TRegSearch.BitBtn3Click(Sender: TObject);
begin
 if SaveDialog2.Execute and (SaveDialog2.FileName <> '') then
  LogMemo.Lines.SaveToFile(SaveDialog2.FileName);
end;

procedure TRegSearch.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
