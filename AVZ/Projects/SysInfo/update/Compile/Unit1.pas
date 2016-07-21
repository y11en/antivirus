unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Mask, ToolEdit, StdCtrls, md5, zXML, IdBaseComponent,
  IdComponent, IdTCPConnection, IdTCPClient, IdFTP,
  zAntivirus, ComCtrls, RXSpin;

type
  TFileDescr = record
   Name    : string;      // Имя файла
   Size    : integer;     // Размер файла
   MD5     : TMD5Digest;  // MD5 сумма
  end;
  TMain = class(TForm)
    feBaseDir: TFilenameEdit;
    Button1: TButton;
    mbFTPUpd: TButton;
    IdFTP: TIdFTP;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    Memo1: TMemo;
    GroupBox1: TGroupBox;
    cb_S1: TCheckBox;
    cb_S2: TCheckBox;
    cb_S3: TCheckBox;
    cb_S4: TCheckBox;
    cb_S5: TCheckBox;
    cb_S6: TCheckBox;
    cbPassiveM: TCheckBox;
    Label1: TLabel;
    RxSpinEdit1: TRxSpinEdit;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure mbFTPUpdClick(Sender: TObject);
    procedure IdFTPWork(Sender: TObject; AWorkMode: TWorkMode;
      const AWorkCount: Integer);
  private
    function GetFileInfo(AFileName: string;
      var FileDescr: TFileDescr): boolean;
    { Private declarations }
  public
   BaseFilesArray : array of TFileDescr;
   ResXML     : TXMLParser;
   SrcDir, DestDir    : string;
   FileNum    : integer;
   procedure AddToLog(S : string);
   function AddFile(AFileName : string; XMLToken: TXmlToken) : boolean;
   function UpdateFilesOnFTP(AServer, ALogin, APasswd, ARootDir: string; NoSendList : string = '') : boolean;
   function UpdateInfoOnFTP(AServer, ALogin, APasswd, ATemplate, ARootDir, AFileName: string) : boolean;
  end;

var
  Main: TMain;

implementation
uses zutil, AbZipper;
{$R *.dfm}

function TMain.AddFile(AFileName: string; XMLToken: TXmlToken): boolean;
var
 FileDescr   : TFileDescr;
 FileTag     : TXMLToken;
 ZipArchive  : TAbZipper;
 ZipFileName : string;
 FS          : TFileStream;
begin
 if not(GetFileInfo(AFileName, FileDescr)) then exit;
 AFileName := AnsiLowerCase(AFileName);
 // Создание тега "Файл"
 FileTag := TXMLToken.Create(XMLToken, xttTag, '');
 XMLToken.SubNodes.Add(FileTag);
 FileTag.TokenName := 'FILE';
 FileTag.WriteString('Name', ExtractFileName(AFileName));
 FileTag.WriteInteger('Size', FileDescr.Size);
 FileTag.WriteString('MD5', MD5DigestToStr(FileDescr.MD5));
 // Копируем файл
 CopyFile(pchar(AFileName), PChar('Distr\'+ExtractFileName(AFileName)), false);
 Memo1.Lines.Add(AFileName);
end;

procedure TMain.Button1Click(Sender: TObject);
var
 SR  : TSearchRec;
 Res : integer;
 BaseTag, ProgTag, Tmp : TXMLToken;
 Lines : TStringList;
 BaseDir, ProgDir : string;
 FS          : TFileStream;
 ZipArchive  : TAbZipper;
 FullZipArchive  : TAbZipper;
begin
 Memo1.Lines.Clear;
 FullZipArchive  := TAbZipper.Create(nil);
 FileNum := 0;
 ProgDir := NormalDir(feBaseDir.Text);
 BaseDir := NormalDir(ProgDir + 'Base\');
 FullZipArchive.FileName := DestDir+'avzbase.zip';
 DeleteFile(DestDir+'avzbase.zip');
 // Удаление файлов старого дистрибутива
 Res := FindFirst(DestDir+'*.*', faAnyFile, SR);
 while Res = 0 do begin
  DeleteFile(DestDir + SR.Name);
  Res := FindNext(SR);
 end;
 FindClose(SR);
 // Создание глaвного тега
 ResXML.XMLRootToken := TXMLToken.Create(nil, xttTag, 'ROOT');
 Tmp := TXMLToken.Create(ResXML.XMLRootToken, xttTag, '');
 Tmp.TokenName := 'AVZ';
 Tmp.WriteString('CurVer', '4.22');
 Tmp.WriteString('MinVer', '4.22');
 ResXML.XMLRootToken.SubNodes.Add(Tmp);
 BaseTag := TXMLToken.Create(Tmp, xttTag, '');
 BaseTag.TokenName := 'BASE';
 BaseTag.WriteString('Dest', '%AVZ%\Base');
 Tmp.SubNodes.Add(BaseTag);
 ProgTag := TXMLToken.Create(Tmp, xttTag, '');
 ProgTag.TokenName := 'SOFT';
 ProgTag.WriteString('Dest', '%AVZ%');
 Tmp.SubNodes.Add(ProgTag);

 Res := FindFirst(BaseDir+'*.avz', faAnyFile, SR);
 while Res = 0 do begin
  AddFile(BaseDir + SR.Name, BaseTag);
  Res := FindNext(SR);
 end;
 FindClose(SR);

{ Res := FindFirst(ProgDir+'*.*', faAnyFile, SR);
 while Res = 0 do begin
  AddFile(feBaseDir.Text + SR.Name, ProgTag);
  Res := FindNext(SR);
 end;
 FindClose(SR);}
 // -------------------
 Lines := TStringList.Create;
 ResXML.SaveToStrings(Lines);
 Lines.Insert(0, '<?xml version="1.0" encoding="windows-1251"?>');
 Lines.SaveToFile('install.xml');

 // Открываем поток
 FS := TFileStream.Create('install.xml', fmOpenRead);
 // Создание архива
 ZipArchive := TAbZipper.Create(nil);
 ZipArchive.FileName := DestDir+'avzupd.zip';
 ZipArchive.AddFromStream('file.dta', FS);
 ZipArchive.Save;
 ZipArchive.Free;
 FS.Free;
 Lines.Free;
 ChDir(BaseDir);
 // Создание полного дистибутива базы
 DeleteFile(FullZipArchive.FileName);
 FullZipArchive.AddFiles('*.avz', faAnyFile);
 FullZipArchive.Save;
 FullZipArchive.Free;
end;

procedure TMain.FormCreate(Sender: TObject);
begin
 ResXML  := TXMLParser.Create;
 DestDir := ExtractFilePath(Application.ExeName) + '\Distr\'; ForceDirectories(DestDir);
 Application.ProcessMessages;
// caption := inttostr();
// Windows.SetParent(Handle, FindWindow ('Shell_TrayWnd',''));
end;

function TMain.GetFileInfo(AFileName: string; var FileDescr : TFileDescr): boolean;
var
 FS : TFileStream;
begin
 Result := false;
 // Проверка наличия файла
 if Not(FileExists(AFileName)) then exit;
 // Открытие файла
 FS := TFileStream.Create(AFileName, fmOpenRead);
 // Имя
 FileDescr.Name := AFileName;
 // Размер
 FileDescr.Size := FS.Size;
 // MD5
 FileDescr.MD5  := MD5Stream(FS);
 FS.Free;
 Result := true;
end;

function TMain.UpdateFilesOnFTP(AServer, ALogin, APasswd,
  ARootDir: string; NoSendList : string = ''): boolean;
var
 SR  : TSearchRec;
 Res : integer;
begin
 Result := false;
 IdFTP.Username := ALogin;
 IdFTP.Host := AServer;
 IdFTP.Password := APasswd;
 IdFTP.Connect(true);
 AddToLog('Подключились к серверу '+AServer);
 IdFTP.ChangeDir(ARootDir);
 AddToLog('Переход в папку '+ARootDir);

 Res := FindFirst(DestDir+'*.*', faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Attr and faDirectory) = 0 then
   try
    IdFTP.Delete(AnsiLowerCase(SR.Name));
    AddToLog('Удален файл  '+SR.Name);
   except
    AddToLog('Ошибка удаления файла  '+SR.Name);
   end;
  Res := FindNext(SR);
 end;
 FindClose(SR);

 Res := FindFirst(DestDir+'*.*', faAnyFile, SR);
 while Res = 0 do begin
  if pos(LowerCase(SR.Name)+';', LowerCase(NoSendList)+';')=0 then
  if (SR.Attr and faDirectory) = 0 then
   try
    IdFTP.Put(AnsiLowerCase(DestDir + SR.Name), AnsiLowerCase(SR.Name));
    AddToLog('Передан файл файл  '+SR.Name);
   except
    AddToLog('ОШИБКА !!! Не передан файл файл  '+SR.Name);
   end;
  Res := FindNext(SR);
 end;
 FindClose(SR);

 IdFTP.Disconnect;
 AddToLog('Отключение выполнено');
end;

procedure TMain.mbFTPUpdClick(Sender: TObject);
begin
 IdFTP.Passive := cbPassiveM.Checked;
 IdFTP.RecvBufferSize := RxSpinEdit1.AsInteger;
 IdFTP.SendBufferSize := RxSpinEdit1.AsInteger;
 if cb_S1.Checked then begin
  UpdateFilesOnFTP('ftp://z-olegcom.57.com1.ru', 'zolegh', '********', '/public_html/secur/avz_up');
  Application.ProcessMessages;
  UpdateInfoOnFTP('ftp://z-olegcom.57.com1.ru', 'zolegh', '********', 'index_inc.php', '/public_html/','index_inc.php');
  Application.ProcessMessages;
  UpdateInfoOnFTP('ftp://z-olegcom.57.com1.ru', 'zolegh', '********', 'index_inc.php', '/public_html/secur','index_inc.php');
  Application.ProcessMessages;
 end;
 if cb_S2.Checked then begin
  UpdateFilesOnFTP('avz.virusinfo.info', 'avz', '**********', '/avz_up','avzbase.zip');
  Application.ProcessMessages;
 end;
 if cb_S3.Checked then begin
  UpdateFilesOnFTP('172.20.97.200', 'root', '*********', '/data/apps/_avz/base/','avzbase.zip');
  Application.ProcessMessages;
 end;
 if cb_S4.Checked then begin
  UpdateFilesOnFTP('172.20.99.20', 'admin', '*******', '/data/apps/_avz/base/','avzbase.zip');
  Application.ProcessMessages;
 end;
 if cb_S5.Checked then begin
  UpdateFilesOnFTP('172.20.100.200', 'admin', '*****', '/data/apps/_avz/base/','avzbase.zip');
  Application.ProcessMessages;
 end;
 if cb_S6.Checked then begin
  UpdateFilesOnFTP('172.20.98.200', 'admin', '***', '/data/apps/_avz/base/','avzbase.zip');
  Application.ProcessMessages;
 end;
 AddToLog('****** Обновление завершено ******');
end;

procedure TMain.AddToLog(S: string);
begin
 Application.ProcessMessages;
 Refresh;
 Application.ProcessMessages;
 Memo1.Lines.Add(S);
 Application.ProcessMessages;
 Refresh;
 Application.ProcessMessages;
end;

function TMain.UpdateInfoOnFTP(AServer, ALogin, APasswd, ATemplate,
  ARootDir, AFileName: string): boolean;
var
 S, BaseDir : string;
 Lines : TStrings;
 AV : TAntivirus;
 FS : TFileSignCheck;
 ES : TSystemCheck;
 SR : TSystemRepair;
begin
 Lines := TStringList.Create;
 BaseDir := NormalDir(NormalDir(feBaseDir.Text) + 'Base\');
 Lines.LoadFromFile(ExtractFilePath(Application.ExeName) + '\Template\'+ATemplate);
 S := Lines.Text;
 AV := TAntivirus.Create(BaseDir);
 AV.LoadBinDatabase;
 S := StringReplace(S, '#DBDATE#', DateToStr(Now), [rfReplaceAll, rfIgnoreCase]);
 S := StringReplace(S, '#SIGN#', IntToStr(AV.SignCount), [rfReplaceAll, rfIgnoreCase]);
 S := StringReplace(S, '#MP_L#', IntToStr(AV.CureScrCount), [rfReplaceAll, rfIgnoreCase]);
 S := StringReplace(S, '#NN#', IntToStr(AV.NNCount), [rfReplaceAll, rfIgnoreCase]);
 AV.Free;
 FS := TFileSignCheck.Create(BaseDir);
 FS.LoadBinDatabase;
 S := StringReplace(S, '#GOODF#', IntToStr(FS.SignCount), [rfReplaceAll, rfIgnoreCase]);
 FS.Free;
 SR := TSystemRepair.Create(BaseDir);
 SR.LoadBinDatabase;
 S := StringReplace(S, '#MP_V#', IntToStr(Length(SR.RepairScripts)), [rfReplaceAll, rfIgnoreCase]);
 SR.Free;
 ES := TSystemCheck.Create(BaseDir);
 ES.LoadBinDatabase;
 S := StringReplace(S, '#MP_E#', IntToStr(Length(ES.CheckScripts)), [rfReplaceAll, rfIgnoreCase]);
 Lines.Text := S;
 Lines.SaveToFile(ExtractFilePath(Application.ExeName) + '\Template\tmp.htm');
 // Передача файла
 AddToLog('********* Передача описания '+ATemplate+' **********');
 IdFTP.Username := ALogin;
 IdFTP.Host := AServer;
 IdFTP.Password := APasswd;
 IdFTP.Connect(true);
 AddToLog('Подключились к серверу '+AServer);
 IdFTP.ChangeDir(ARootDir);
 AddToLog('Переход в папку '+ARootDir);
 IdFTP.Delete(AFileName);
 IdFTP.Put(ExtractFilePath(Application.ExeName) + '\Template\tmp.htm', AnsiLowerCase(AFileName));
 IdFTP.Disconnect;
 AddToLog('Отключение выполнено');
end;

procedure TMain.IdFTPWork(Sender: TObject; AWorkMode: TWorkMode;
  const AWorkCount: Integer);
begin
 Application.ProcessMessages;
 Caption           := IntToStr(AWorkCount);
 Application.ProcessMessages;
 Application.Title := IntToStr(AWorkCount);
 Application.ProcessMessages;
 Application.ProcessMessages;
end;

end.
