unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, IdBaseComponent, IdComponent, IdTCPConnection, IdTCPClient,
  IdHTTP, DB, OracleData, Oracle, StdCtrls, Grids, DBGrids, zDBFDataSet,
  ExtCtrls, zFilterCmp, zAntivirus, ComCtrls, md5, RxVerInf, Mask, ToolEdit,
  clipbrd, INIFiles, RXDBCtrl;

type
  THTTPDownloadMode = (hdmIESettings, hdmDirectConnection, hdmProxy);
  // Запись AV базы
  TAVbaseRec = record
   NAIM : string;
   SIZEMIN, SIZEMAX : dword;
   S1, S2, S3, S4   : dword;
   MD5, FILENAME    : string;
   FULLCOMPARE      : boolean;
   SCRIPT, AVZ_FILE : integer;
   PEFileInfo       : TAVZFileInfo;
  end;
  TMain = class(TForm)
    OracleSession1: TOracleSession;
    MainDataSet: TOracleDataSet;
    DataSource1: TDataSource;
    AddChkOracleQuery: TOracleQuery;
    ZDBFDataSet1: TZDBFDataSet;
    pcMain: TPageControl;
    tsData: TTabSheet;
    tsFilter: TTabSheet;
    DBGrid2: TDBGrid;
    TabSheet3: TTabSheet;
    Memo1: TMemo;
    FilterEdit1: TFilterEdit;
    FilterEdit2: TFilterEdit;
    FilterEdit3: TFilterEdit;
    FilterEdit4: TFilterEdit;
    FilterEdit5: TFilterEdit;
    FilterEdit6: TFilterEdit;
    FilterEdit7: TFilterEdit;
    FilterEdit8: TFilterEdit;
    FilterEdit9: TFilterEdit;
    FilterEdit10: TFilterEdit;
    FilterEdit11: TFilterEdit;
    cbExcludeBadLink: TCheckBox;
    CheckFileOracleQuery: TOracleQuery;
    TabSheet1: TTabSheet;
    Button2: TButton;
    cbExcludeNoAutocheck: TCheckBox;
    StatusBar: TStatusBar;
    Memo2: TMemo;
    FilterEdit12: TFilterEdit;
    Timer1: TTimer;
    Panel1: TPanel;
    Button1: TButton;
    Button3: TButton;
    Button4: TButton;
    ceURL: TComboEdit;
    Button5: TButton;
    Button6: TButton;
    Timer2: TTimer;
    cbExtScan: TCheckBox;
    cbDel2: TCheckBox;
    TabSheet2: TTabSheet;
    cbClipbrdCHM: TCheckBox;
    cbSetVirName: TCheckBox;
    GroupBox1: TGroupBox;
    cbAS404: TCheckBox;
    cbAS_1: TCheckBox;
    cbAS300: TCheckBox;
    FilterEdit13: TFilterEdit;
    Button7: TButton;
    TabSheet4: TTabSheet;
    GoogleHunterDataSet: TOracleDataSet;
    DataSource2: TDataSource;
    RxDBGrid1: TRxDBGrid;
    Button8: TButton;
    SearchStatDataSet: TOracleDataSet;
    ceHunterFilter: TComboEdit;
    Button9: TButton;
    Button10: TButton;
    cbAS200: TCheckBox;
    procedure Button1Click(Sender: TObject);
    procedure IdHTTP1Redirect(Sender: TObject; var dest: String;
      var NumRedirect: Integer; var Handled: Boolean;
      var VMethod: TIdHTTPMethod);
    procedure Button2Click(Sender: TObject);
    procedure pcMainChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure ceURLButtonClick(Sender: TObject);
    procedure Button5Click(Sender: TObject);
    procedure Button6Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure Timer2Timer(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure MainDataSetBeforeDelete(DataSet: TDataSet);
    procedure Button7Click(Sender: TObject);
    procedure Button8Click(Sender: TObject);
    procedure MainDataSetAfterInsert(DataSet: TDataSet);
    procedure ceHunterFilterButtonClick(Sender: TObject);
    procedure Button9Click(Sender: TObject);
    procedure Button10Click(Sender: TObject);
  private
   FProxyServer : string;
   StopF : boolean;
   UrlHunterFoundCnt, UrlHunterAddCnt : integer;
   function ExtractFilenameFromURI(S: string): string;
   function CheckFile(AFileName: string): string;
   function AnalizeFile(AFileName, AVirName: string;
      var AAVbaseRec: TAVbaseRec): boolean;
   procedure OnWork(Sender: TObject; AWorkMode: TWorkMode; const AWorkCount: Integer);
  public
   DownloadMode :  THTTPDownloadMode;
   FilesDir : string;
   function RefreshMainDataset : boolean;
   // Загрузка файла
   function LoadFile(AFileName, ASaveName: string; AFileSize : integer = 0): boolean;
   // Проверка текущего URL
   function CheckURL(AReload : boolean) : boolean;
   function ScanTextForURL(S, FExt : string) : boolean;
   function SaveFilter(Aname : string) : boolean;
   function LoadFilter(Aname : string) : boolean;
   function SaveSetup : boolean;
   function LoadSetup : boolean;
   function SaveStr(S : string) : boolean;
   function HuntURLByGoogle(AUrl : string; AHuntMode : integer) : boolean;
   function HuntURLByYahoo(AUrl : string) : boolean;
   function ScanStrForURL(S : string) : boolean;
  end;

var
  Main: TMain;

function AVZ_Initialize: Integer; stdcall; external 'avz_krnl.dll';
function AVZ_InitializeEx(ADbPath : PChar): Integer; stdcall; external 'avz_krnl.dll';
function AVZ_CheckFile(AFileName: PChar; Msg: PChar): Integer; stdcall;external 'avz_krnl.dll';

implementation
uses wininet, StrUtils, orautil;
var
 LastClipboardTxt : string;
{$R *.dfm}

function TMain.LoadFile(AFileName, ASaveName: string; AFileSize : integer = 0): boolean;
const
 BufferSize = 2048;
var
 hSession, hURL : HInternet;
 Buffer         : array[1..BufferSize] of Byte;
 BufferLen      : DWORD;
 f              : File;
 sAppName       : string;
begin
 Result := false;
 // Удаление файла
 DeleteFile(ASaveName);
 sAppName := ExtractFileName(ParamStr(0));
 // Открытие соединения (в обход прокси !!)
 case DownloadMode of
  hdmIESettings       :  hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_PRECONFIG,  nil, nil, 0);
  hdmDirectConnection :  hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_DIRECT,  nil, nil, 0);
  hdmProxy            :  hSession := InternetOpen(PChar(sAppName), INTERNET_OPEN_TYPE_PROXY,  PChar(FProxyServer), nil, 0);
 end;
 if hSession <> nil then
 try
   hURL := InternetOpenURL(hSession, PChar(AFileName),nil,0,INTERNET_FLAG_PRAGMA_NOCACHE or INTERNET_FLAG_RELOAD or INTERNET_FLAG_NO_UI or INTERNET_FLAG_NO_COOKIES or INTERNET_FLAG_NO_CACHE_WRITE, 0);
  if hURL = nil then begin
//   InternetGetLastResponseInfo
   exit;
  end;
  try
   AssignFile(f, ASaveName);
   Rewrite(f,1);
   repeat
    // Чтение очередного фрагмента в буфер
    InternetReadFile(hURL, @Buffer, SizeOf(Buffer), BufferLen);
    // Запись принятых данных в файл
    BlockWrite(f, Buffer, BufferLen);
   until BufferLen = 0;
   CloseFile(f);
   Result := true;
  finally
   InternetCloseHandle(hURL)
  end;
 finally
  InternetCloseHandle(hSession)
 end;
end;

function TMain.ExtractFilenameFromURI(S : string) : string;
var
 i : integer;
begin
 result := '';
 S := trim(S);
 i := length(S);
 while i > 0 do begin
  if s[i] = '/' then begin
   Result := copy(S, i+1, length(S));
   exit;
  end;
  dec(i);
 end;
end;

procedure TMain.Button1Click(Sender: TObject);
var
 ChkCount : integer;
begin
 StopF := false;
 MainDataSet.Active := true;
 Application.ProcessMessages;
 ChkCount := 0;
 while not(MainDataSet.Eof) do begin
  Caption :=  inttostr(MainDataSet['ID']) + ' / ' + inttostr(ChkCount) + ' / ' +inttostr(MainDataSet.RecordCount);
  Application.Title :=  Caption;
  Application.ProcessMessages;
  CheckURL(false);
  Application.ProcessMessages;
  MainDataSet.Next;
  Refresh; Repaint;
  Application.ProcessMessages;
  inc(ChkCount);
  if StopF then exit;
 end;
end;

procedure TMain.IdHTTP1Redirect(Sender: TObject; var dest: String;
  var NumRedirect: Integer; var Handled: Boolean;
  var VMethod: TIdHTTPMethod);
begin
 Memo2.lines.Add(dest);
end;

procedure TMain.Button2Click(Sender: TObject);
var
 URL, prim : string;
begin
 ZDBFDataSet1.Active   := false;
 ZDBFDataSet1.FileName := 'NEW.DBF';
 ZDBFDataSet1.Active   := true;
 ZDBFDataSet1.First;
 while not(ZDBFDataSet1.Eof) do begin
  URL := Trim(NVL(ZDBFDataSet1['URL']));
  prim :=  Trim(NVL(ZDBFDataSet1['AVIRUS']))+' '+
           Trim(NVL(ZDBFDataSet1['NAME']));
  if URL <> '' then begin
   AddChkOracleQuery.SetVariable('URL', URL);
   AddChkOracleQuery.Execute;
   if AddChkOracleQuery.RowsProcessed = 0 then begin
    Memo1.Lines.Add(URL + ' уже есть');
    MainDataSet.Append;
    MainDataSet['ID'] := 0;
    MainDataSet['URL'] := URL;
    MainDataSet['NAIM'] := prim;
    MainDataSet['ADD_DATE'] := Now;
    MainDataSet.Post;
   end else begin
    OracleSession1.Commit;
    Memo1.Lines.Add(URL + ' уже есть');
   end;
  end;
  ZDBFDataSet1.Next;
 end;
end;

procedure TMain.pcMainChange(Sender: TObject);
var
 i : integer;
 S, SQL : string;
begin
 // Обновление источника данных
 if pcMain.ActivePage = tsData then begin
   SQL := '';
   // Цикл по фильтрам
   for i := 0 to ComponentCount-1 do
    if Components[i] is TFilterEdit then
     if (Components[i] as TFilterEdit).ValidateValue then begin
      S := Trim((Components[i] as TFilterEdit).GetSQL);
      AddToList(SQL, S, ' and ');
     end;
   if cbExcludeBadLink.Checked then
    AddToList(SQL, 'NVL(BAD_LINK, 0) = 0', ' and ');
   if cbExcludeNoAutocheck.Checked then
    AddToList(SQL, 'NVL(AUTO_CHECK, 0) = 0', ' and ');
   // Формирование текста запроса
   if Trim(SQL) = '' then begin
    SQL := '1=1';
    tsFilter.ImageIndex := -1;
   end else
    tsFilter.ImageIndex := 0;
   // Подготовка запроса и его модификация
   if MainDataSet.GetVariable('where_st') <>  SQL then begin
    MainDataSet.SetVariable('where_st', SQL);
    RefreshMainDataset;
   end;
 end;
end;

function TMain.RefreshMainDataset: boolean;
begin
 MainDataSet.Active := false;
 MainDataSet.Active := true;
end;

procedure TMain.FormCreate(Sender: TObject);
begin
 FilesDir := ExtractFilePath(Application.ExeName) + 'Files\';
 ForceDirectories(FilesDir);
 pcMainChange(nil);
 Memo1.Lines.Add('AVZ_InitializeEx = ' + inttostr(AVZ_InitializeEx('c:\avz4\base\')));
 LoadSetup;
end;

function TMain.CheckURL(AReload: boolean): boolean;
var
 MS : TmemoryStream;
 OldFileDate : TDateTime;
 OldFileSize : integer;
 Fname, S, LastDetect : string;
 IdHTTP1 : TIdHTTP;
 Res : integer;
 MsgBuf : packed array [0..1024] of char;
 DataLines : TStringList;
begin
  DataLines := TStringList.create;
  IdHTTP1 := TIdHTTP.Create(nil);
  IdHTTP1.OnRedirect := IdHTTP1Redirect;
  IdHTTP1.OnWork := OnWork;
  LastDetect := '';
  MS := TmemoryStream.Create;
  try
   StatusBar.SimpleText := 'Запрос заголовка ' + MainDataSet['URL'];
   Application.ProcessMessages;
   // Запрос заголовка
   IdHTTP1.ReadTimeout := 15000;
   IdHTTP1.Response.ResponseCode := 0;
   IdHTTP1.Head(MainDataSet['URL']);
   StatusBar.SimpleText := 'Получен ответ ' + MainDataSet['URL'];
   Application.ProcessMessages;
   // Внесение в базу данных информации
   OldFileSize := NVL(MainDataSet['FILE_SIZE'], -2);
   OldFileDate := NVL(MainDataSet['FILE_DATE'], 0);
   MainDataSet.Edit;
   Memo1.Lines.Add(Trim(MainDataSet['URL']));
   Memo1.Lines.Add(IntToStr(IdHTTP1.Response.ResponseCode) + ' ' +
                  IntToStr(IdHTTP1.Response.ContentLength) + ' '+
                  datetimetostr(IdHTTP1.Response.LastModified));
   Memo1.Lines.Add('');
   MainDataSet['FILE_DATE']  := IdHTTP1.Response.LastModified;
   MainDataSet['FILE_SIZE']  := IdHTTP1.Response.ContentLength;
   MainDataSet['LAST_CHECK'] := Now;
   MainDataSet['LAST_RESP_CODE'] := IdHTTP1.Response.ResponseCode;
   case IdHTTP1.Response.ResponseCode of
    200      : if cbAS200.Checked then if nvl(MainDataSet['BAD_LINK'], 0) <> 0 then MainDataSet['BAD_LINK'] := 0;
    -1       : if cbAS404.Checked then MainDataSet['BAD_LINK'] := 1;
    403, 404, 500..502 : if cbAS404.Checked then MainDataSet['BAD_LINK'] := 1;
    301..304 : if cbAS300.Checked then MainDataSet['BAD_LINK'] := 3;
   end;
  MainDataSet.Post;
  // Обновление
  if (MainDataSet['FILE_SIZE'] <= 0) or
      (MainDataSet['FILE_SIZE'] <> OldFileSize) or
      (MainDataSet['FILE_DATE'] <> OldFileDate) or
      AReload
      then  begin
    StatusBar.SimpleText := 'Загрузка файла с ' + MainDataSet['URL'];
    Application.ProcessMessages;
    MS.Clear;
    Fname := ExtractFilenameFromURI(IdHTTP1.URL.URI);
    if Fname = '' then
     Fname := 'unnamed'+inttostr(MainDataSet['ID']);
    IdHTTP1.Get(MainDataSet['URL'], MS);
    if FileExists(FilesDir+Fname) then begin
     S := Fname;
     Fname := ChangeFileExt(S,'') + '['+inttostr(MainDataSet['ID'])+']' + ExtractFileExt(S);
    end;
    MS.SaveToFile(FilesDir+Fname);
    StatusBar.SimpleText := 'Файл загружен, анализ ' + MainDataSet['URL'];
    LastDetect := CheckFile(FilesDir+Fname);
    if LastDetect <> '' then begin
     Memo1.Lines.Add('>>> Детектируется сигнатурой как '+LastDetect);
     DeleteFile(FilesDir+Fname);
    end else begin
     if cbExtScan.Checked then
      Res := AVZ_CheckFile(PChar(FilesDir+Fname), @MsgBuf);
      if Res = 1 then begin
       LastDetect := Trim(MsgBuf);
       if cbDel2.Checked then begin
        Memo1.Lines.Add('>>> Детектируется ядром как '+LastDetect);
        DeleteFile(FilesDir+Fname);
       end;
       if Res = 2 then begin
        Memo1.Lines.Add('>>> Подозревается ядром как '+LastDetect);
       end;
      end;
    end;
    if FileExists(FilesDir+Fname) then begin
     DataLines.Clear;
     DataLines.Add(Nvl(MainDataSet['NAIM']));
     DataLines.SaveToFile(ChangeFileExt(FilesDir+Fname, '.---'));
    end;
    MainDataSet.Edit;
    MainDataSet['REAL_FILE_SIZE'] := MS.Size;
    MS.Seek(0,0); S := '          ';
    MS.Read(S[1], 7); S := Trim(S);
    MainDataSet['REAL_SIGN'] := S;
    if not(AReload) then
     MainDataSet['UPD_CNT'] := NVL(MainDataSet['UPD_CNT'], 0) + 1;
    if LastDetect <> '' then begin
     MainDataSet['LAST_AVZ_DETECT'] := LastDetect;
     if IsNull(MainDataSet['NAIM']) then
      MainDataSet['NAIM'] := LastDetect;
     if cbSetVirName.Checked then
      MainDataSet['NAIM'] := LastDetect;
    end;
    MainDataSet.Post;
   end else
    Memo1.Lines.Add('>>> Файл не изменился, обновление не требуется ');
  except
   MainDataSet.Edit;
   MainDataSet['LAST_CHECK'] := Now;
   MainDataSet['LAST_RESP_CODE'] := IdHTTP1.Response.ResponseCode;
   case IdHTTP1.Response.ResponseCode of
    200      : if nvl(MainDataSet['BAD_LINK'], 0) <> 0 then MainDataSet['BAD_LINK'] := 0;
    -1       : if cbAS404.Checked then MainDataSet['BAD_LINK'] := 1;
    400, 403, 404, 500..502 : if cbAS404.Checked then MainDataSet['BAD_LINK'] := 1;
    301..304 : if cbAS300.Checked then MainDataSet['BAD_LINK'] := 3;
   end;
   MainDataSet.Post;
  end;
  MS.Clear;
  MS.Free;
  IdHTTP1.Free;
  StatusBar.SimpleText := '';
end;

procedure TMain.Button3Click(Sender: TObject);
begin
  CheckURL(false);
end;

procedure TMain.Button4Click(Sender: TObject);
begin
  CheckURL(true);
end;

function TMain.AnalizeFile(AFileName, AVirName: string;
  var AAVbaseRec: TAVbaseRec): boolean;
var
 VersionInfo : TVersionInfo;
 FileScanner : TFileScanner;
 VirusDescr  : TVirusDescr;
 S           : string;
 MD5Digest      : TMD5Digest;
 MD5_Hash       : string;
begin
 FileScanner := TFileScanner.Create;
 Result := FileScanner.CheckFile(AFileName);
 if not(Result) then exit;
 // Определение версии файла
 VersionInfo := TVersionInfo.Create(AFileName);
 FileScanner.PEFileInfo.FileVersion    := VersionInfo.FileVersion;
 FileScanner.PEFileInfo.LegalCopyright := VersionInfo.LegalCopyright;
 VersionInfo.Free;
 // Хеш MD5
 MD5_Hash := '??';
 try
  MD5Digest := MD5File(AFileName);
  MD5_Hash  := UpperCase(MD5DigestToStr(MD5Digest));
 except end;
 // Заполнение AVbaseRec
 AAVbaseRec.NAIM     := Trim(AVirName);
 AAVbaseRec.SIZEMIN  := FileScanner.PEFileInfo.Size;
 AAVbaseRec.SIZEMAX  := FileScanner.PEFileInfo.Size;
 AAVbaseRec.S1       := FileScanner.PEFileInfo.Sign[1];
 AAVbaseRec.S2       := FileScanner.PEFileInfo.Sign[2];
 AAVbaseRec.S3       := FileScanner.PEFileInfo.Sign[3];
 AAVbaseRec.S4       := FileScanner.PEFileInfo.Sign[4];
 AAVbaseRec.MD5      := MD5_Hash;
 AAVbaseRec.FILENAME := ExtractFileName(FileScanner.PEFileInfo.FileName);
 AAVbaseRec.FULLCOMPARE := false;
 AAVbaseRec.SCRIPT   := 0;
 AAVbaseRec.AVZ_FILE := 0;
 AAVbaseRec.PEFileInfo := FileScanner.PEFileInfo;
 Result := true;
end;

function TMain.CheckFile(AFileName : string) : string;
var
 S           : string;
 AVbaseRec   : TAVbaseRec;
begin
 Result := '';
 if not(AnalizeFile(AFileName, '', AVbaseRec)) then begin
  exit;
 end;
 // Вывод информации
 with AVbaseRec do begin
  // Додстановка параметров
  CheckFileOracleQuery.SetVariable('FSIZE', PEFileInfo.Size);
  CheckFileOracleQuery.SetVariable('S1', PEFileInfo.Sign[1]);
  CheckFileOracleQuery.SetVariable('S2', PEFileInfo.Sign[2]);
  CheckFileOracleQuery.SetVariable('S3', PEFileInfo.Sign[3]);
  CheckFileOracleQuery.SetVariable('S4', PEFileInfo.Sign[4]);
  CheckFileOracleQuery.Execute;
  // Проверка
  if not(CheckFileOracleQuery.Eof) then begin
    Result := CheckFileOracleQuery.Field('Naim');
  end;
 end;
end;


procedure TMain.ceURLButtonClick(Sender: TObject);
var
 URL, S : string;
 i : integer;
begin
  URL := Trim(ceURL.Text);
  // Удаление пробелов
  URL := StringReplace(URL, ' ', '', [rfReplaceAll]);
  // Блокировка добавления явно некорректных URL
  if pos('...', URL) > 0 then exit;
  if pos('::/', URL) > 0 then exit;
  if pos('::\', URL) > 0 then exit;
  if pos('AppInit_DLLs', URL) > 0 then exit;
  if pos('WinlogonNotify', URL) > 0 then exit;
  if pos('C:', URL) > 0 then exit;
  if (pos('{', URL)>0) and (pos('}', URL)>0) then exit;
  // Это имя файл на диске
  if pos('С:', Uppercase(URL)) > 0 then exit;
  if URL <> '' then begin
   inc(UrlHunterFoundCnt);
   if pos('http', LowerCase(URL)) = 0 then
    URL := 'http://'+URL;
   S := URL; URL := '';
   // Подготовка URL с удалением повторов "/"
   for i := 1 to length(S) do
    if i < 8 then URL := URL + S[i] else begin
     if (S[i] = '/') and (copy(URL, length(URL), 1) = '/') then Continue;
     if (S[i] = '\') and (copy(URL, length(URL), 1) = '\') then Continue;
     URL := URL + S[i];
    end;
   // Поиск в базе
   AddChkOracleQuery.SetVariable('URL', URL);
   AddChkOracleQuery.Execute;
   // Не найден ? Тогда добавление
   if AddChkOracleQuery.RowsProcessed = 0 then begin
    MainDataSet.Append;
    MainDataSet['ID']  := 0;
    MainDataSet['URL'] := URL;
    MainDataSet['ADD_DATE'] := Now;
    MainDataSet.Post;
    ceURL.Text := '';
    StatusBar.SimpleText := 'URL добавлен';
    inc(UrlHunterAddCnt);
   end else
    StatusBar.SimpleText := 'URL уже есть в базе: '+URL;
    ceURL.Text := '';
    OracleSession1.Commit;
  end;
end;

procedure TMain.Button5Click(Sender: TObject);
begin
 StopF := true;
end;

procedure TMain.Button6Click(Sender: TObject);
var
 ChkCount : integer;
begin
 StopF := false;
 MainDataSet.Active := true;
 Application.ProcessMessages;
 ChkCount := 0;
 while not(MainDataSet.Eof) do begin
  Caption :=  inttostr(MainDataSet['ID']) + ' / ' + inttostr(ChkCount) + ' / ' +inttostr(MainDataSet.RecordCount);
  Application.Title :=  Caption;
  Application.ProcessMessages;
  CheckURL(true);
  Application.ProcessMessages;
  MainDataSet.Next;
  Refresh; Repaint;
  Application.ProcessMessages;
  inc(ChkCount);
  if StopF then exit;
 end;
end;

procedure TMain.Timer1Timer(Sender: TObject);
var
 S : string;
begin
 try
  S := Trim(Clipboard.AsText);
  if LowerCase(S) <> LowerCase(LastClipboardTxt) then begin
   if pos('.chm', LowerCase(S)) >0 then begin
    ceURL.Text := S;
    ceURL.Button.Click;
   end;
  end;
  LastClipboardTxt := S;
 except
 end;
end;

procedure TMain.Timer2Timer(Sender: TObject);
var
 S : string;
begin
 if not(cbClipbrdCHM.Checked) then exit;
 try
  S := Trim(Clipboard.AsText);
  if LowerCase(S) <> LowerCase(LastClipboardTxt) then begin
   LastClipboardTxt := S;
   if (Length(S) > 200) and (pos('http', LowerCase(S)) > 0 ) then
    SaveStr(S);
    ScanStrForURL(S);
  end;
 except
 end; 
end;

procedure TMain.OnWork(Sender: TObject; AWorkMode: TWorkMode;
  const AWorkCount: Integer);
begin
end;

function TMain.LoadFilter(Aname: string): boolean;
var
 INI : TIniFile;
 i : integer;
begin
 INI := TIniFile.Create(ChangeFileExt(AppFileName, '.ini'));
 for i := 0 to ComponentCount - 1 do
  if (Components[i] is TFilterEdit) then
   (Components[i] as TFilterEdit).TextStateDescr := INI.ReadString(Aname, (Components[i] as TFilterEdit).Name, '');
 INI.Free;
end;

function TMain.SaveFilter(Aname: string): boolean;
var
 INI : TIniFile;
 i : integer;
begin
 INI := TIniFile.Create(ChangeFileExt(AppFileName, '.ini'));
 for i := 0 to ComponentCount - 1 do
  if (Components[i] is TFilterEdit) then
   INI.WriteString(Aname, (Components[i] as TFilterEdit).Name, (Components[i] as TFilterEdit).TextStateDescr);
 INI.Free;
end;

procedure TMain.FormDestroy(Sender: TObject);
begin
 SaveSetup;
end;

function TMain.ScanTextForURL(S, FExt: string): boolean;
var
 poz, i : integer;
 URL : string;
begin
 repeat
  poz := pos(FExt, LowerCase(S));
  if poz > 10 then begin
   URL := copy(S, 1, poz+3);
   i := Length(URL)-8;
   while (i > 0) and (LowerCase(copy(URL, i, 5))<>'http:') and (LowerCase(copy(URL, i, 6))<>'https:') do
    dec(i);
   if LowerCase(copy(URL, i, 4))='http' then begin
    URL := Copy(URL, i, maxint);
    if pos('https:', URL) > 1 then
     URL := copy(URL, pos('https:', URL), maxint);
    Memo1.Lines.Add(URL);
    ceURL.Text := URL;
    ceURL.Button.Click;
   end
  end;
  Delete(S, 1, poz+4);
 until poz = 0;
end;

function TMain.LoadSetup: boolean;
var
 INI : TIniFile;
begin
 INI := TIniFile.Create(ChangeFileExt(AppFileName, '.ini'));
 LoadFilter('MainFilter');
 cbClipbrdCHM.Checked := INI.ReadBool('Main', 'ClipbrdMon', false);
 cbSetVirName.Checked := INI.ReadBool('Main', 'SetVirName', false);
 cbAS404.Checked      := INI.ReadBool('Main', 'AS404', false);
 cbAS300.Checked      := INI.ReadBool('Main', 'AS300', false);
 cbAS200.Checked      := INI.ReadBool('Main', 'AS200', false);
 cbAS_1.Checked       := INI.ReadBool('Main', 'AS_1', false);
 cbExtScan.Checked    := INI.ReadBool('Main', 'ExtScan', false);
 cbDel2.Checked       := INI.ReadBool('Main', 'Del2', false);
 INI.Free;
end;

function TMain.SaveSetup: boolean;
var
 INI : TIniFile;
begin
 INI := TIniFile.Create(ChangeFileExt(AppFileName, '.ini'));
 SaveFilter('MainFilter');
 INI.WriteBool('Main', 'ClipbrdMon', cbClipbrdCHM.Checked);
 INI.WriteBool('Main', 'SetVirName', cbSetVirName.Checked);
 INI.WriteBool('Main', 'AS404',      cbAS404.Checked);
 INI.WriteBool('Main', 'AS300',      cbAS300.Checked);
 INI.WriteBool('Main', 'AS200',      cbAS200.Checked);
 INI.WriteBool('Main', 'AS_1',       cbAS_1.Checked);
 INI.WriteBool('Main', 'ExtScan',    cbExtScan.Checked);
 INI.WriteBool('Main', 'Del2',       cbDel2.Checked);
 INI.Free;
end;

function TMain.SaveStr(S: string): boolean;
var
 Path, Fname : string;
 Lines : TStringList;
begin
 Path := ExtractFilePath(AppFileName) + 'HuntHtml\';
 ForceDirectories(Path);
 FName := FormatDateTime('yyyymmdd_hhnnss', now);
 if FileExists(Path + Fname+'.htm') then
  Fname := Fname + '_';
 Lines := TStringList.Create;
 Lines.Text := Trim(S);
 try Lines.SaveToFile(Path + Fname+'.htm'); except end;
 Lines.Free;
end;

procedure TMain.MainDataSetBeforeDelete(DataSet: TDataSet);
begin
 MainDataSet.RefreshRecord;
end;

procedure TMain.Button7Click(Sender: TObject);
begin
 RefreshMainDataset;
end;

function TMain.HuntURLByGoogle(AUrl: string; AHuntMode : integer): boolean;
var
 MS : TmemoryStream;
 S, ST : string;
 InTag : boolean;
 i, pagen : integer;
 IdHTTP1 : TIdHTTP;
 Res : integer;
begin
  AUrl := trim(AUrl);
  if AUrl = '' then exit;
  IdHTTP1 := TIdHTTP.Create(nil);
  IdHTTP1.OnRedirect := IdHTTP1Redirect;
  IdHTTP1.OnWork := OnWork;
  MS := TmemoryStream.Create;
  try
   pagen := 0;
   repeat
   StatusBar.SimpleText := 'Запрос заголовка ' + AUrl + ' '+inttostr(pagen);
   Application.ProcessMessages;
   // Запрос заголовка
   IdHTTP1.ReadTimeout := 15000;
   IdHTTP1.Response.ResponseCode := 0;
   MS.Clear;
   case AHuntMode of
    1 : begin
         ST := 'http://www.google.ru/search?q=O16+-+DPF+'+AUrl+'&num=100&hl=ru&lr=&pwst=1&filter=0';
         if pagen > 0 then
           ST := 'http://www.google.ru/search?q=O16+-+DPF+'+AUrl+'&num=100&hl=ru&lr=&pwst=1&filter=0&start='+inttostr(100*pagen);
        end;
    2 : begin
         ST := 'http://www.google.ru/search?q=fix+O16+-+DPF+'+AUrl+'+O16'+'&num=100&hl=ru&lr=&pwst=1&filter=0';
         if pagen > 0 then
           ST := 'http://www.google.ru/search?q=fix+O16+-+DPF+'+AUrl+'+O16'+'&num=100&hl=ru&lr=&pwst=1&filter=0&start='+inttostr(100*pagen);
        end;
   end;
   IdHTTP1.Get(ST, MS);
   StatusBar.SimpleText := 'Получен ответ ' + AUrl;
   Application.ProcessMessages;
  // Внесение в базу данных информации
  Memo1.Lines.Add(AUrl);
  Memo1.Lines.Add(IntToStr(IdHTTP1.Response.ResponseCode) + ' ' +
                  IntToStr(IdHTTP1.Response.ContentLength) + ' '+
                  datetimetostr(IdHTTP1.Response.LastModified));
  if IdHTTP1.Response.ResponseCode <> 200 then exit;
  if MS.Size = 0 then exit;
  MS.Seek(0,0);
  SetLength(S, MS.Size);
  MS.Read(S[1], Length(S));
  ST := '';
  if S <> '' then
  for i := 1 to length(S) do begin
   if S[i] = '<' then begin InTag := true; Continue; end;
   if S[i] = '>' then begin InTag := false; Continue; end;
   if not(InTag) and (S[i]<>#$0D) and (S[i]<>#$0A) then
    ST := ST + S[i];
  end;
  if Pos('http', LowerCase(ST)) > 0 then
   SaveStr(S);
  ScanStrForURL(ST);
  inc(pagen);
  if pagen > 20 then break;
 until pos('Следующая', copy(ST, length(ST)-500, 500)) = 0;
 finally
  MS.Clear;
  MS.Free;
  IdHTTP1.Free;
  StatusBar.SimpleText := '';
 end;
end;

function TMain.ScanStrForURL(S: string): boolean;
begin
   // Базовая нормализация
   S := StringReplace(S, #$0D, '', [rfReplaceAll]);
   S := StringReplace(S, #$0A, '', [rfReplaceAll]);
   S := StringReplace(S, ' ', '', [rfReplaceAll]);
   S := StringReplace(S, 'хттп', 'http', [rfReplaceAll,rfIgnoreCase]);
   S := StringReplace(S, 'hxxp', 'http', [rfReplaceAll,rfIgnoreCase]);
   S := StringReplace(S, 'hoop', 'http', [rfReplaceAll,rfIgnoreCase]);
   S := StringReplace(S, 'http:??', 'http://', [rfReplaceAll,rfIgnoreCase]);
   ScanTextForURL(S, '.chm');
   ScanTextForURL(S, '.exe');
   ScanTextForURL(S, '.cab');
   ScanTextForURL(S, '.ocx');
   ScanTextForURL(S, '.dll');
end;

procedure TMain.Button8Click(Sender: TObject);
var
  TotalUrlHunterAddCnt,  TotalUrlHunterFoundCnt, TotalCnt : integer;
begin
 StopF := false;
 TotalUrlHunterAddCnt   := 0;
 TotalUrlHunterFoundCnt := 0;
 TotalCnt := 0;
 while not(GoogleHunterDataSet.Eof) do begin
  Application.ProcessMessages;
  UrlHunterAddCnt   := 0;
  UrlHunterFoundCnt := 0;
  HuntURLByGoogle(NVL(GoogleHunterDataSet['X']), 2);
  SearchStatDataSet.Append;
  SearchStatDataSet['DATE_OP'] := Now;
  SearchStatDataSet['URL'] := NVL(GoogleHunterDataSet['X']);
  SearchStatDataSet['SEARCH_SYS'] := 3;
  SearchStatDataSet['ADD_CNT']    := UrlHunterAddCnt;
  SearchStatDataSet['FOUND_CNT']  := UrlHunterFoundCnt;
  inc(TotalUrlHunterAddCnt, UrlHunterAddCnt);
  inc(TotalUrlHunterFoundCnt, UrlHunterFoundCnt);
  inc(TotalCnt);
  SearchStatDataSet.Post;
  GoogleHunterDataSet.Next;
  Caption := IntToStr(TotalUrlHunterAddCnt)+ ' '+ IntToStr(TotalUrlHunterFoundCnt) + ' '+ IntToStr(TotalCnt);
  if StopF then exit;
 end;
end;

procedure TMain.MainDataSetAfterInsert(DataSet: TDataSet);
begin
 DataSet['URL_TYPE'] := 0;
end;

procedure TMain.ceHunterFilterButtonClick(Sender: TObject);
begin
 GoogleHunterDataSet.Active := false;
 GoogleHunterDataSet.SetVariable('x', ceHunterFilter.text);
 GoogleHunterDataSet.Active := true;
end;

procedure TMain.Button9Click(Sender: TObject);
begin
 while not(GoogleHunterDataSet.Eof) do begin
  Application.ProcessMessages;
  UrlHunterAddCnt   := 0;
  UrlHunterFoundCnt := 0;
  HuntURLByYahoo(NVL(GoogleHunterDataSet['X']));
  SearchStatDataSet.Append;
  SearchStatDataSet['DATE_OP'] := Now;
  SearchStatDataSet['URL'] := NVL(GoogleHunterDataSet['X']);
  SearchStatDataSet['SEARCH_SYS'] := 2;
  SearchStatDataSet['ADD_CNT']    := UrlHunterAddCnt;
  SearchStatDataSet['FOUND_CNT']  := UrlHunterFoundCnt;
  SearchStatDataSet.Post;
  GoogleHunterDataSet.Next;
 end;
end;

function TMain.HuntURLByYahoo(AUrl: string): boolean;
var
 MS : TmemoryStream;
 S, ST : string;
 InTag : boolean;
 i, pagen : integer;
 IdHTTP1 : TIdHTTP;
 Res : integer;
begin
  AUrl := trim(AUrl);
  if AUrl = '' then exit;
  IdHTTP1 := TIdHTTP.Create(nil);
  IdHTTP1.OnRedirect := IdHTTP1Redirect;
  IdHTTP1.OnWork := OnWork;
  MS := TmemoryStream.Create;
  try
   pagen := 0;
   repeat
   StatusBar.SimpleText := 'Запрос заголовка ' + AUrl + ' '+inttostr(pagen);
   Application.ProcessMessages;
   // Запрос заголовка
   IdHTTP1.ReadTimeout := 15000;
   IdHTTP1.Response.ResponseCode := 0;
   MS.Clear;
   ST := 'http://search.yahoo.com/search?p=O16+-+DPF+%22'+AUrl+'%22&ei=UTF-8&fr=FP-tab-web-t400&x=wrt';
   IdHTTP1.Get(ST, MS);
   StatusBar.SimpleText := 'Получен ответ ' + AUrl;
   Application.ProcessMessages;
  // Внесение в базу данных информации
  Memo1.Lines.Add(AUrl);
  Memo1.Lines.Add(IntToStr(IdHTTP1.Response.ResponseCode) + ' ' +
                  IntToStr(IdHTTP1.Response.ContentLength) + ' '+
                  datetimetostr(IdHTTP1.Response.LastModified));
  if IdHTTP1.Response.ResponseCode <> 200 then exit;
  if MS.Size = 0 then exit;
  MS.Seek(0,0);
  SetLength(S, MS.Size);
  MS.Read(S[1], Length(S));
  ST := '';
  if S <> '' then
  for i := 1 to length(S) do begin
   if S[i] = '<' then begin InTag := true; Continue; end;
   if S[i] = '>' then begin InTag := false; Continue; end;
   if not(InTag) and (S[i]<>#$0D) and (S[i]<>#$0A) then
    ST := ST + S[i];
  end;
  if Pos('http', LowerCase(ST)) > 0 then
   SaveStr(S);
  ScanStrForURL(ST);
  inc(pagen);
  if pagen > 10 then break;
 until pos('Следующая', copy(ST, length(ST)-500, 500)) = 0;
 finally
  MS.Clear;
  MS.Free;
  IdHTTP1.Free;
  StatusBar.SimpleText := '';
 end;
end;

procedure TMain.Button10Click(Sender: TObject);
var
 ChkCount, X, i : integer;
begin
 Randomize;
 StopF := false;
 MainDataSet.Active := true;
 Application.ProcessMessages;
 ChkCount := 0;
 while not(MainDataSet.Eof) do begin
  Caption :=  inttostr(MainDataSet['ID']) + ' / ' + inttostr(ChkCount) + ' / ' +inttostr(MainDataSet.RecordCount);
  Application.Title :=  Caption;
  Application.ProcessMessages;
  CheckURL(false);
  Application.ProcessMessages;
  X := random(500)+1;
  for i := 1 to X do
   MainDataSet.Next;
  if MainDataSet.Eof then begin
   MainDataSet.Refresh;
   X := random(500)+1;
   for i := 1 to X do
    MainDataSet.Next;
  end;
  Refresh; Repaint;
  Application.ProcessMessages;
  inc(ChkCount);
  if StopF then exit;
 end;
end;

end.
