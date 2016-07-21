unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, Mask, ComCtrls, RxVerInf,
  zAntivirus, ExtCtrls, SystemMonitor, Buttons, ToolEdit, md5,
  RXSpin, Grids, DBGrids, DB, ADODB, OracleData, Oracle;

type
  TFileDescr = packed record
   GroupCode : word;        // Код группы
   FileName  : string[50];  // Имя файла
   FileSize  : DWORD;       // Размер файла
   IsEXE, IsPE : boolean;   // Признаки типа
   S1, S2,
   S3, S4    : DWORD;       // Сигнатуры
   SummCRC   : DWORD;       // Контрольная сумма
   MD5       : TMD5Digest;  // Сигнатура MD5
   FileDate  : TDateTime;   // Дата и время файла
   FileDescription : string[80]; // Описание
   FileVersion     : string[20]; // Версия
  end;

  TMain = class(TForm)
    StatusBar: TStatusBar;
    PageControl1: TPageControl;
    TabSheet1: TTabSheet;
    TabSheet2: TTabSheet;
    Label1: TLabel;
    deStartDir: TDirectoryEdit;
    GroupBox1: TGroupBox;
    LogMemo: TMemo;
    mbStart: TBitBtn;
    mbStop: TBitBtn;
    mbSaveLog: TBitBtn;
    CheckBox1: TCheckBox;
    cbAllPE: TCheckBox;
    Timer1: TTimer;
    SaveDialog: TSaveDialog;
    OracleSession1: TOracleSession;
    GoodFilesDataSet: TOracleDataSet;
    CheckFileOracleQuery: TOracleQuery;
    feGoodFile: TFilenameEdit;
    Button1: TButton;
    procedure mbStartClick(Sender: TObject);
    procedure mbStopClick(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure mbSaveLogClick(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    FAddCount: integer;
    FScanCount: integer;
    procedure SetScanCount(const Value: integer);
    procedure SetAddCount(const Value: integer);
    { Private declarations }
  public
   StopF : boolean;
   function  IsPEFile(AFileName: string): boolean;
   procedure AddToLog(AMsg : string);
   function ScanDir(ADir: string): boolean;
   function FileScan(AFileName: string;  AIncludeInBase : boolean): boolean;
   // Добавление файла в базу
   function AddFileToBase(AFileName : string; AFileDescr : TFileDescr) : boolean;
   // Вычисление CRC
   function CalkFileCRC(AFileName : string) : DWORD;
   // ***** Свойства *********
   property ScanCount : integer read FScanCount write SetScanCount;
   property AddCount  : integer read FAddCount write SetAddCount;
  end;

var
  Main: TMain;

implementation
uses zutil;
{$R *.DFM}

function TMain.FileScan(AFileName: string; AIncludeInBase : boolean): boolean;
var
 FileScanner    : TFileScanner;
 VersionInfo    : TVersionInfo;
 MD5Digest      : TMD5Digest;
 MD5_Hash       : string;
 S : string;
 TMP : TFileDescr;
begin
 FileScanner    := nil;
 VersionInfo    := nil;
 Result := false;
 try
   ScanCount := ScanCount + 1;
   FileScanner := TFileScanner.Create;
   FileScanner.CheckFile(AFileName);
   MD5Digest := MD5File(AFileName);
   S := '';
   with FileScanner.PEFileInfo do begin
    if Sign[1] > 0 then S := S+' S1=''0x'+IntToHex(Sign[1], 8)+'''';
    if Sign[2] > 0 then S := S+' S2=''0x'+IntToHex(Sign[2], 8)+'''';
    if Sign[3] > 0 then S := S+' S3=''0x'+IntToHex(Sign[3], 8)+'''';
    if Sign[4] > 0 then S := S+' S4=''0x'+IntToHex(Sign[4], 8)+'''';
   end;
   // Определение версии файла
   VersionInfo := TVersionInfo.Create(AFileName);
   FileScanner.PEFileInfo.FileVersion    := VersionInfo.FileVersion;
   FileScanner.PEFileInfo.LegalCopyright := VersionInfo.LegalCopyright;
   MD5_Hash := MD5DigestToStr(MD5Digest);
   with TMP do begin
    FileName  := copy(LowerCase(ExtractFileName(AFileName)), 1, 50);
    FileSize  := FileScanner.PEFileInfo.Size;
    IsEXE     := FileScanner.PEFileInfo.IsEXEFile;
    IsPE      := FileScanner.PEFileInfo.IsPEFile;
    S1        := FileScanner.PEFileInfo.Sign[1];
    S2        := FileScanner.PEFileInfo.Sign[2];
    S3        := FileScanner.PEFileInfo.Sign[3];
    S4        := FileScanner.PEFileInfo.Sign[4];
    MD5       := MD5Digest;
    SummCRC   := CalkFileCRC(AFileName);
    FileDate  := GetFileDate(AFileName);
    FileDescription := copy(VersionInfo.FileDescription, 1, 80);
    FileVersion     := copy(VersionInfo.FileVersion, 1, 20);
   end;
   TMP.GroupCode := 0;
   if AIncludeInBase then
    TMP.GroupCode := 255;
   AddFileToBase(AFileName, TMP);
   VersionInfo.Free;  VersionInfo := nil;
  except
   Result := false;
  end;
  FileScanner.Free;
end;

function TMain.ScanDir(ADir : string) : boolean;
var
 SR  : TSearchRec;
 FileName, FileExt : string;
 Res : integer;
 Cnt : integer;
begin
 Res := FindFirst(NormalDir(ADir)+'*.*', faAnyFile, SR);
 Application.ProcessMessages;
 StatusBar.Panels[0].Text := (NormalDir(ADir));
 while Res = 0 do begin
  inc(Cnt);
  if StopF then begin
   LogMemo.Lines.Add('Операция прервана пользователем');
   abort;
  end;
  if Cnt mod 16 = 0 then Application.ProcessMessages;
  if ((SR.Attr and faDirectory) = 0) then begin
   FileName := SR.Name;
   FileName := UpperCase(FileName);
   FileExt := ExtractFileExt(FileName);
   if (pos(FileExt, '.EXE.DLL.OCX.SCR.COM.INF.PIF.CAB.MP3.SYS.ASF.BAS.JAR.CHM.CPL.HLP.CMD.ASP.BPL.DCP.DOS.DRV.AX.VXD.TSK.MOD.DAT.TSP.FMT.FLL.IME') > 0) or
      (cbAllPE.Checked and IsPEFile(ADir + SR.Name))
   then begin
     FileScan(ADir + SR.Name,  false);
   end;
  end;
  if (SR.Attr and faDirectory) > 0 then
   if (SR.Name <> '.') and (SR.Name <> '..') then begin
    ScanDir(NormalDir(ADir + SR.Name));
   end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
end;

procedure TMain.mbStartClick(Sender: TObject);
var
 i : integer;
begin
 // Загрузка базы антивируса
 ScanCount := 0; AddCount := 0;
 mbStart.Enabled := false;
 mbStop.Enabled := true;
 try
  StopF := false;
  AddToLog('Сканирование диска. Рабочая база - ');
  ScanDir(NormalDir(deStartDir.Text));
  AddToLog('Просканировано файлов: '+IntToStr(ScanCount)+', добавлено файлов '+IntToStr(AddCount));
  AddToLog('Сканирование завершено в '+Datetimetostr(now));
 finally
  StatusBar.Panels[0].Text := '';
  mbStart.Enabled := true;
  mbStop.Enabled := false;
 end;
end;

procedure TMain.SetScanCount(const Value: integer);
begin
 FScanCount := Value;
end;

procedure TMain.SetAddCount(const Value: integer);
begin
 FAddCount := Value;
end;

procedure TMain.mbStopClick(Sender: TObject);
begin
 StopF := true;
end;

procedure TMain.Timer1Timer(Sender: TObject);
begin
 StatusBar.Panels[1].Text := IntToStr(ScanCount)+'/'+IntToStr(AddCount);
end;

procedure TMain.AddToLog(AMsg: string);
begin
 LogMemo.Lines.Add(AMsg);
end;
procedure TMain.mbSaveLogClick(Sender: TObject);
begin
 if SaveDialog.Execute and (SaveDialog.FileName <> '') then
  LogMemo.Lines.SaveToFile(SaveDialog.FileName);
end;

function TMain.AddFileToBase(AFileName : string; AFileDescr: TFileDescr): boolean;
var
 i : integer;
 FoundFl : boolean;
begin
 FoundFl := false;
 CheckFileOracleQuery.SetVariable('MD5', UpperCase(MD5DigestToStr(AFileDescr.MD5)));
 CheckFileOracleQuery.Execute;
 FoundFl := not(CheckFileOracleQuery.Eof);
 // Режим "Только проверка"
 if CheckBox1.Checked then begin
  if not(FoundFl) then begin
   AddToLog(AFileName);
  end;
  exit;
 end;
 // Файл с таким размером и сигнатурой не найден - добавляем его
 if not(FoundFl) then begin
  GoodFilesDataSet.Append;
  GoodFilesDataSet['GROUP_CODE'] := AFileDescr.GroupCode;
  GoodFilesDataSet['FILENAME'] := AFileDescr.FileName;
  GoodFilesDataSet['FILESIZE'] := AFileDescr.FileSize;
  if AFileDescr.IsEXE then
   GoodFilesDataSet['ISEXE'] := 1 else GoodFilesDataSet['ISEXE'] := 0;;
  if AFileDescr.IsPE then
    GoodFilesDataSet['ISPE'] := 1 else GoodFilesDataSet['ISPE'] := 0;
  GoodFilesDataSet['S1'] := AFileDescr.S1;
  GoodFilesDataSet['S2'] := AFileDescr.S2;
  GoodFilesDataSet['S3'] := AFileDescr.S3;
  GoodFilesDataSet['S4'] := AFileDescr.S4;
  GoodFilesDataSet['SUMMCRC'] := AFileDescr.SummCRC;
  GoodFilesDataSet['MD5'] := UpperCase(MD5DigestToStr(AFileDescr.MD5));
  GoodFilesDataSet['FILEDATE'] := AFileDescr.FileDate;
  GoodFilesDataSet['FILEDESCRIPTION'] := AFileDescr.FileDescription;
  GoodFilesDataSet['FILEVERSION'] := AFileDescr.FileVersion;
  GoodFilesDataSet['DATE_ADD'] := NOW;
  GoodFilesDataSet['AVZ_FILE'] := 22;
  GoodFilesDataSet.Post;
  inc(FAddCount);
 end;
end;

function TMain.CalkFileCRC(AFileName : string): DWORD;
var
 FS  : TFileStream;
 Buf : array[0..60000] of byte;
 Res, i, poz : integer;
begin
 Result := 0;
 FS := TFileStream.Create(AFileName, fmOpenRead or fmShareDenyNone);
 Poz := 0;
 Res := FS.Read(Buf, sizeof(Buf));
 while Res > 0 do begin
  for i :=0 to Res - 1 do begin
   inc(poz);
   Result := Result + Buf[i]*poz;
  end;
  Res := FS.Read(Buf, sizeof(Buf));
 end;
 FS.Free;
end;

function TMain.IsPEFile(AFileName: string): boolean;
var
 FS : TFileStream;
 B  : array[0..1] of char;
begin
 Result := false;
 FS := nil;
 try
  FS := TFileStream.Create(AFileName, fmOpenRead	or fmShareDenyNone);
  FS.Read(B, 2);
  FS.Free;
  FS := nil;
  Result := (B[0] = 'M') and (B[1] = 'Z');
 except
   FS.Free;
 end;
end;

procedure TMain.Button1Click(Sender: TObject);
begin
 FileScan(feGoodFile.Text, true);
end;

procedure TMain.FormCreate(Sender: TObject);
begin
 if ParamCount > 0 then begin
  deStartDir.Text := NormalDir(ParamStr(1));
 end;
end;

end.


