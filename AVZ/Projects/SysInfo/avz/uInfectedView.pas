unit uInfectedView;
// Просмотр карантина и Infected
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, ComCtrls, CheckLst, ExtCtrls, INIFiles, Buttons, Menus;

type
  TInfectedView = class(TForm)
    GroupBox2: TGroupBox;
    lbDirList: TListBox;
    mbDeleteFolder: TBitBtn;
    mbClearQurantine: TBitBtn;
    PopupMenu1: TPopupMenu;
    N1: TMenuItem;
    PopupMenu2: TPopupMenu;
    N2: TMenuItem;
    N3: TMenuItem;
    N4: TMenuItem;
    N5: TMenuItem;
    N6: TMenuItem;
    sdSaveZip: TSaveDialog;
    GroupBox1: TGroupBox;
    lvFiles: TListView;
    StatusBar: TStatusBar;
    Panel1: TPanel;
    sbDeleteFiles: TSpeedButton;
    sbCreateArchive: TSpeedButton;
    sbRunAutoqurantine: TSpeedButton;
    sbRunAddByList: TSpeedButton;
    SpeedButton1: TSpeedButton;
    procedure lbDirListClick(Sender: TObject);
    procedure BitBtn2Click(Sender: TObject);
    procedure mbDeleteFolderClick(Sender: TObject);
    procedure mbClearQurantineClick(Sender: TObject);
    procedure N1Click(Sender: TObject);
    procedure N2Click(Sender: TObject);
    procedure N3Click(Sender: TObject);
    procedure N4Click(Sender: TObject);
    procedure N6Click(Sender: TObject);
    function FormHelp(Command: Word; Data: Integer;
      var CallHelp: Boolean): Boolean;
    procedure sbDeleteFilesClick(Sender: TObject);
    procedure sbCreateArchiveClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure sbRunAutoqurantineClick(Sender: TObject);
    procedure sbRunAddByListClick(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
  private
   FTekInfectedDir: string;
   // Обновление списка каталогов
   function RefreshDirList : boolean;
   // Обновление списка файлов
   function RefreshFilesList(ADirName : string) : boolean;
   // Обновление текущего списка файлов
   function RefreshTekFilesList : boolean;
   // Удаление папки
   function DeleteFolder(ADirName : string) : boolean;
   // Получение текущей папки Infected
   function GetTekInfectedDir: string;
  public
   InfectedFolder   : string;
   InfectedFolderCaption : string;
   // Создание архива
   function CreateArchive(AArchiveName, APasswd : string) : boolean;
   // Выполнение диалога
   function Execute(AFolder, AFolderCaption : string; ModalVisible : boolean = true) : boolean;
   // Текущий каталог
   property TekInfectedDir : string read GetTekInfectedDir;
  end;

var
  InfectedView: TInfectedView;
// Выполнение в диалоговом режиме
function ExecuteInfectedView(AFolder, AFolderCaption : string) : boolean;
// Выполнение из скрипта
function ExecuteInfectedViewAndCreateArchive(AFolder, AArchiveName, APassword : string) : boolean;

implementation
uses zutil, AbZipper, uAutoQurantine, uQurantineByList, zAVKernel, zTranslate;
{$R *.dfm}

{ TForm2 }

function TInfectedView.Execute(AFolder, AFolderCaption : string; ModalVisible : boolean = true): boolean;
begin
 AFolderCaption        := TranslateStr(AFolderCaption); 
 Caption               := AFolderCaption;
 InfectedFolderCaption := AFolderCaption;
 // Формирование базового пути
 InfectedFolder := GetQuarantineDirName(AFolder, true);
 // Обновление списка папок
 RefreshDirList;
 RefreshFilesList(TekInfectedDir);
 if lbDirList.Items.Count = 0 then begin
 end;
 if ModalVisible then begin
  ShowModal;
  Result := ModalResult = mrOk;
 end else
  Result := true;
end;

function TInfectedView.GetTekInfectedDir: string;
begin
 if lbDirList.ItemIndex >= 0 then
  Result := lbDirList.Items[lbDirList.ItemIndex]
   else Result := '';
end;

function TInfectedView.RefreshDirList: boolean;
var
 SR               : TSearchRec;
 Res              : integer;
 Lines            : TStringList;
begin
 lbDirList.Items.Clear;
 Lines := TStringList.Create;
 // Поиск каталогов
 Res := FindFirst(InfectedFolder+'*', faAnyFile, SR);
 while Res = 0 do begin
  // Это каталог ??
  if (faDirectory and SR.Attr) > 0 then
   if (SR.Name <> '.') and (SR.Name <> '..') then
    Lines.Add(SR.Name);
  Res := FindNext(SR);
 end;
 FindClose(SR);
 // Сортировка списка
 Lines.Sort;
 // Добавление элементов и выделение последнего
 lbDirList.Items.AddStrings(Lines);
 if lbDirList.Items.Count > 0 then
  lbDirList.ItemIndex := lbDirList.Items.Count - 1;
 Lines.Free;
 mbDeleteFolder.Enabled   := lbDirList.Items.Count > 0;
 mbClearQurantine.Enabled := lbDirList.Items.Count > 0;
 RefreshTekFilesList;
end;

function TInfectedView.RefreshFilesList(ADirName : string): boolean;
var
 SR               : TSearchRec;
 Res              : integer;
 INI              : TINIFile;
 ListItem         : TListItem;
 SummCount, SummSize : integer;
begin
 StatusBar.SimpleText := '';
 lvFiles.Items.Clear;
 SummCount := 0;
 SummSize  := 0;
 if ADirName = '' then exit;
 // Формирование базового пути
 Res := FindFirst(InfectedFolder+ADirName+'\*.ini', faAnyFile, SR);
 while Res = 0 do begin
  // Это файл ??
  if (faDirectory and SR.Attr) = 0 then begin
   // Добавление элемента меню
   ListItem := lvFiles.Items.Add;
   // Чтение из INI файла параметров
   INI := TIniFile.Create(InfectedFolder+ADirName+'\'+SR.Name);
   ListItem.Caption := INI.ReadString('InfectedFile','SRC','?');
   ListItem.SubItems.Add(INI.ReadString('InfectedFile','Virus','?'));
   ListItem.SubItems.Add(INI.ReadString('InfectedFile','Infected','?'));
   ListItem.SubItems.Add(INI.ReadString('InfectedFile','Size','?'));
   inc(SummCount); Inc(SummSize, INI.ReadInteger('InfectedFile','Size', 0));
   INI.Free;
  end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
 StatusBar.SimpleText := TranslateStr('$AVZ0350:'+IntToStr(SummCount)+', $AVZ1171: '+FormatFloat('0.00', SummSize/1024)+' $AVZ1191');
end;

procedure TInfectedView.lbDirListClick(Sender: TObject);
begin
 RefreshTekFilesList;
end;

procedure TInfectedView.BitBtn2Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFiles.Items.Count - 1 do
  lvFiles.Items[i].Checked := false;
end;

procedure TInfectedView.mbDeleteFolderClick(Sender: TObject);
var
 S, Path : string;
begin
 // Нет папок
 if lbDirList.Items.Count = 0 then exit;
 // Папка не выбрана
 if lbDirList.ItemIndex < 0   then exit;
 // Ищем имя папки
 S    := Trim(lbDirList.Items[lbDirList.ItemIndex]);
 // Формируем полный путь к папке
 Path := InfectedFolder+S;
 if S = '' then exit;
 // Удаляем папку по запросу (сначала удаляются все файлы, затем сама папка)
 if zMessageDlg('$AVZ0165 '+S,
               mtConfirmation, [mbYes, mbNo], 0) = mrYes then begin
  // Удаление папки
  if DeleteFolder(Path) then zMessageDlg('$AVZ0690 '+S+' $AVZ1105', mtInformation, [mbOk], 0)
   else zMessageDlg('$AVZ0685 '+S, mtError, [mbOk], 0);
 end;
 RefreshDirList;
end;

procedure TInfectedView.mbClearQurantineClick(Sender: TObject);
var
 i : integer;
 Res : boolean;
begin
 // Нет папок
 if lbDirList.Items.Count = 0 then begin
  zMessageDlg('$AVZ0526 ', mtError, [mbOk], 0);
  exit;
 end;
 if zMessageDlg('$AVZ0625', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then
  exit;
 Res := true;
 for i := 0 to lbDirList.Items.Count - 1 do
  Res := Res and DeleteFolder(InfectedFolder+lbDirList.Items[i]);
 // Удаление папки
 if Res then zMessageDlg('$AVZ0627', mtInformation, [mbOk], 0)
  else zMessageDlg('$AVZ0670 ', mtError, [mbOk], 0);
 RefreshDirList;
end;

function TInfectedView.DeleteFolder(ADirName: string): boolean;
var
 SR               : TSearchRec;
 Res              : integer;
begin
 Result := false;
 if pos(LowerCase(ExtractFilePath(Application.ExeName)), LowerCase(ADirName)) <> 1 then begin
  zMessageDlg('$AVZ0635 '+ADirName+' $AVZ0476', mtError, [mbOk], 0);
  Exit;
 end;
 ADirName := NormalDir(ADirName);
 Res := FindFirst(ADirName+'*.*', faAnyFile, SR);
 while Res = 0 do begin
   if (LowerCase(ExtractFileExt(SR.Name)) = '.ini') or
      (LowerCase(ExtractFileExt(SR.Name)) = '.dta') then
        DeleteFile(ADirName+SR.Name);
   Res := FindNext(SR);
  end;
  FindClose(SR);
  // Удаление папки
  RemoveDirectory(PChar(NormalDir(ADirName)));
  RemoveDirectory(PChar(NormalNameDir(ADirName)));
  Result := not(DirectoryExists(ADirName));
end;

procedure TInfectedView.N1Click(Sender: TObject);
begin
 RefreshDirList;
end;

function TInfectedView.RefreshTekFilesList: boolean;
begin
 RefreshFilesList(TekInfectedDir);
end;

procedure TInfectedView.N2Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFiles.Items.Count - 1 do
  lvFiles.Items[i].Checked := true;
end;

procedure TInfectedView.N3Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFiles.Items.Count - 1 do
  lvFiles.Items[i].Checked := false;
end;

procedure TInfectedView.N4Click(Sender: TObject);
var
 i : integer;
begin
 for i := 0 to lvFiles.Items.Count - 1 do
  lvFiles.Items[i].Checked := not(lvFiles.Items[i].Checked);
end;

procedure TInfectedView.N6Click(Sender: TObject);
begin
 RefreshDirList;
end;

function ExecuteInfectedView(AFolder, AFolderCaption : string) : boolean;
begin
 InfectedView := nil;
 try
  InfectedView := TInfectedView.Create(nil);
  Result := InfectedView.Execute(AFolder, AFolderCaption, true);
 finally
  InfectedView.Free;
  InfectedView := nil;
 end;
end;

function ExecuteInfectedViewAndCreateArchive(AFolder, AArchiveName, APassword : string) : boolean;
var
 i : integer;
begin
 InfectedView := nil;
 try
  InfectedView := TInfectedView.Create(nil);
  // Открытие папки
  InfectedView.Execute(AFolder, '', false);
  // Отметка всех файлов
  for i := 0 to InfectedView.lvFiles.Items.Count - 1 do
   InfectedView.lvFiles.Items[i].Checked := true;
  // Создание архива
  InfectedView.CreateArchive(AArchiveName, APassword);
 finally
  InfectedView.Free;
  InfectedView := nil;
 end;
end;

function TInfectedView.FormHelp(Command: Word; Data: Integer;
  var CallHelp: Boolean): Boolean;
begin
 Application.HelpJump('t_infected');
end;

procedure TInfectedView.sbDeleteFilesClick(Sender: TObject);
var
 i : integer;
begin
 if zMessageDlg('$AVZ1088', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then
  exit;
 // Установка текущей папки
 SetCurrentDir(InfectedFolder);
 // Удаление отмеченных файлов
 for i := 0 to lvFiles.Items.Count - 1 do
  if lvFiles.Items[i].Checked then begin
   DeleteFile(TekInfectedDir +'\' + lvFiles.Items[i].SubItems[1]);
   DeleteFile(TekInfectedDir +'\' + ChangeFileExt(lvFiles.Items[i].SubItems[1], '.ini'))
  end;
 RefreshTekFilesList;
end;

procedure TInfectedView.sbCreateArchiveClick(Sender: TObject);
var
 i, FilesCnt : integer;
begin
 FilesCnt := 0;
 for i := 0 to lvFiles.Items.Count - 1 do
  if lvFiles.Items[i].Checked then
   inc(FilesCnt);
 if FilesCnt = 0 then begin
  zMessageDlg('$AVZ0120', mtError, [mbOk], 0);
  exit;
 end;
 sdSaveZip.FileName := 'virus.zip';
 if not(sdSaveZip.Execute and (sdSaveZip.FileName <> '')) then exit;
 CreateArchive(sdSaveZip.FileName, 'virus');
end;

procedure TInfectedView.FormCreate(Sender: TObject);
begin
 StatusBar.SimpleText := '';
 TranslateForm(Self);
end;

procedure TInfectedView.sbRunAutoqurantineClick(Sender: TObject);
begin
 ExecuteAutoQurantine;
 // Обновление списка папок
 RefreshDirList;
 RefreshTekFilesList;
end;

procedure TInfectedView.sbRunAddByListClick(Sender: TObject);
begin
 ExecuteQurantineByList;
 // Обновление списка папок
 RefreshDirList;
 RefreshTekFilesList;
end;

function TInfectedView.CreateArchive(AArchiveName, APasswd: string): boolean;
var
 ZipArchive : TAbZipper;
 i : integer;
 LastCurrentDir : string;
begin
 // Создание папок
 zForceDirectories(ExtractFilePath(AArchiveName));
 // Создание класса и его подготовка
 ZipArchive := TAbZipper.Create(nil);
 ZipArchive.Password := APasswd;
 DeleteFile(AArchiveName);
 ZipArchive.FileName := AArchiveName;
 // Установка текущей папки
 LastCurrentDir := GetCurrentDir;
 SetCurrentDir(InfectedFolder);
 // Добавление файлов
 for i := 0 to lvFiles.Items.Count - 1 do
  if lvFiles.Items[i].Checked then begin
   ZipArchive.AddFiles(TekInfectedDir +'\' + lvFiles.Items[i].SubItems[1], faAnyFile);
   ZipArchive.AddFiles(TekInfectedDir +'\' + ChangeFileExt(lvFiles.Items[i].SubItems[1], '.ini'), faAnyFile);
  end;
 // Создание архива
 ZipArchive.Save;
 ZipArchive.free;
 SetCurrentDir(LastCurrentDir);
end;

procedure TInfectedView.SpeedButton1Click(Sender: TObject);
var
 i    : integer;
 INI  : TINIFile;
 SrcFile : string;
begin
 if zMessageDlg('$AVZ0136', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then
  exit;
 // Установка текущей папки
 SetCurrentDir(InfectedFolder);
 // Восстановление отмеченных файлов
 for i := 0 to lvFiles.Items.Count - 1 do
  if lvFiles.Items[i].Checked then begin
   // Процедура восстановления
   INI  := TINIFile.Create(TekInfectedDir +'\' + ChangeFileExt(lvFiles.Items[i].SubItems[1], '.ini'));
   SrcFile := INI.ReadString('InfectedFile', 'Src', '');
   if (SrcFile <> '') and not(FileExists(SrcFile)) then
    CopyFile(PChar(TekInfectedDir +'\' + lvFiles.Items[i].SubItems[1]),
             PChar(SrcFile),
             false);
   INI.Free;
  end;
end;

end.
