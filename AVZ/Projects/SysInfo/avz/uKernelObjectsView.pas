unit uKernelObjectsView;
// **************** Модули пространства ядра **************************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ntdll, Grids, RXGrids, zSharedFunctions, StdCtrls, Buttons,
  ExtCtrls, Menus, zHTMLTemplate, zAVZDriverRK, RxVerInf, math,
  zAVKernel, zAVZKernel, zLogSystem;

type
   TKernelModuleInformation = packed record
    Base                    : DWORD;   // Базовый адрес
    Size                    : DWORD;   // Размер в байтах
    LoadCount               : DWORD;
    ImageName               : string;  // Имя исполняемого файла
    CheckResult             : integer; // Результаты проверки
    Visible                 : integer; // Уровень видимости
    Descr           : string;  // Описание
    LegalCopyright  : string;  // Копирайты
   end;

  TKernelObjectsView = class(TForm)
    dgKernelModules: TRxDrawGrid;
    Panel3: TPanel;
    sbCopyProcToQurantine: TSpeedButton;
    SpeedButton5: TSpeedButton;
    sbRefresh: TSpeedButton;
    sbSaveLog: TSpeedButton;
    pmKernelTablePopUp: TPopupMenu;
    mmQurantine: TMenuItem;
    MenuItem4: TMenuItem;
    MenuItem5: TMenuItem;
    MenuItem6: TMenuItem;
    MenuItem7: TMenuItem;
    sbCreateDump: TSpeedButton;
    procedure dgKernelModulesDrawCell(Sender: TObject; ACol, ARow: Integer;
      Rect: TRect; State: TGridDrawState);
    procedure dgKernelModulesFixedCellClick(Sender: TObject; ACol,
      ARow: Integer);
    procedure sbRefreshClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure sbSaveLogClick(Sender: TObject);
    procedure FormKeyDown(Sender: TObject; var Key: Word;
      Shift: TShiftState);
    procedure dgKernelModulesClick(Sender: TObject);
    procedure sbCopyProcToQurantineClick(Sender: TObject);
    procedure SpeedButton5Click(Sender: TObject);
    procedure mmQurantineClick(Sender: TObject);
    procedure MenuItem5Click(Sender: TObject);
    procedure MenuItem6Click(Sender: TObject);
    procedure MenuItem7Click(Sender: TObject);
    procedure SpeedButton1Click(Sender: TObject);
    procedure sbCreateDumpClick(Sender: TObject);
  public
   OrderColumnId : integer; // ID столбца, по которому отсортированы данные
   // Список модулей
   ModuleList  : packed array of TKernelModuleInformation;
   // Получение полного имени модуля (базируется на NormalProgramFileName)
   Function ExpandModuleFilename(AModuleName : string) : string;
   // Обновление списка модулей
   Function RefreshModuleList : boolean;
   // Выполнение диалога
   Function Execute : boolean;
   // Сортировка результатов
   Function OrderResults(ACol : integer) : boolean;
   // Добавление данных из AVZ PM
   Function AddDataFromPM : boolean;
   // Прототип - поиск объектов сканированием памяти
   Function AddDataFromMem : boolean;
   // Создание HTML отчета
   Function CreateHTMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   Function CreateXMLReport(ALines : TStrings; HideGoodFiles : boolean) : boolean;
   // Добавить все неопознанные файлы в карантин
   function AddAllToQurantine: boolean;
  end;

var
  KernelObjectsView: TKernelObjectsView;

 function ExecuteKernelObjectsView : boolean;
implementation
uses zutil, uGUIShared, zTranslate;
{$R *.dfm}

{ TKernelObjectsView }

function ExecuteKernelObjectsView : boolean;
begin
  KernelObjectsView := nil;
 try
  KernelObjectsView := TKernelObjectsView.Create(nil);
  KernelObjectsView.Execute;
 finally
  KernelObjectsView.Free;
  KernelObjectsView := nil;
 end;
end;

function TKernelObjectsView.Execute: boolean;
begin
 RefreshModuleList;
 ShowModal;
end;

function TKernelObjectsView.RefreshModuleList: boolean;
var
  R: NTSTATUS;
  NTDLL_DLL : THandle;
  BufSize, ResBufSize: DWORD;
  Buf : Pointer;
  i,  NumEntries : integer;
  ZwQuerySystemInformation: TNativeQuerySystemInformation;
  Tmp : PSystemModuleInformation;
  VersionInfo : TVersionInfo;
begin
 ModuleList        := nil;
 // Адрес ядра по умолчанию
 Result := false;
 // Загрузка библиотеки и поиск функции
 NTDLL_DLL := LoadLibrary(NTDLL_DLL_name);
 // Поиск функции
 if NTDLL_DLL <> 0 then @ZwQuerySystemInformation := GetProcAddress(NTDLL_DLL,'ZwQuerySystemInformation')
  else exit;
 if @ZwQuerySystemInformation = nil then exit;
 // Вывод в протокол адреса функции
 BufSize := 200000;
 GetMem(Buf, BufSize);
 // Получение списка модулей ядра
 R := ZwQuerySystemInformation(SystemModuleInformation, Buf, BufSize, @ResBufSize);
 if R <> STATUS_SUCCESS then exit;
 NumEntries := dword(Buf^);
 if NumEntries > 0 then begin
  Result := true;
  SetLength(ModuleList, NumEntries);
  Tmp := Ptr(dword(Buf) + 4);
  for i := 0 to NumEntries - 1 do begin
   ModuleList[i].Base := Tmp^.Base;
   ModuleList[i].Size := Tmp^.Size;
   ModuleList[i].LoadCount := Tmp^.LoadCount;
   ModuleList[i].ImageName := ExpandModuleFilename(Tmp^.ImageName);
   ModuleList[i].CheckResult := -1;
   ModuleList[i].Visible := 0;
   inc(Tmp);
  end;
  dgKernelModules.RowCount := NumEntries + 1;
 end else
  dgKernelModules.RowCount := 2;
 dgKernelModules.Repaint;
 // Освобождение памяти
 FreeMem(Buf, BufSize);
 FreeLibrary(NTDLL_DLL);
 // Запрос данных у драйвера AVZPM
 AddDataFromPM;
 OrderResults(1);
 AddDataFromMem;
 // Проверка файлов по базе безопасных
 ZProgressClass.AddProgressMax(Length(ModuleList));
 for i := 0 to Length(ModuleList)-1 do begin
  ZProgressClass.ProgressStep;
  ModuleList[i].CheckResult := FileSignCheck.CheckFile(ExpandModuleFilename(ModuleList[i].ImageName));
  try
   ModuleList[i].Descr := '';
   ModuleList[i].LegalCopyright := '';
   VersionInfo := TVersionInfo.Create(ExpandModuleFilename(ModuleList[i].ImageName));
   ModuleList[i].Descr          := VersionInfo.FileDescription;
   ModuleList[i].LegalCopyright := VersionInfo.LegalCopyright;
   VersionInfo.Free;
  except end;
 end;
 OrderResults(OrderColumnId);
 dgKernelModules.Repaint;
end;

procedure TKernelObjectsView.dgKernelModulesDrawCell(Sender: TObject; ACol,
  ARow: Integer; Rect: TRect; State: TGridDrawState);
var
 S : string;
 Tmp : TKernelModuleInformation;
begin
 S := '??';
 if ARow = 0 then begin
  case ACol of
   0 : S := '$AVZ0437';
   1 : S := '$AVZ0073';
   2 : S := '$AVZ0895';
   3 : S := '$AVZ0778';
   4 : S := '$AVZ0581';
   5 : S := '$AVZ0852';
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, TranslateStr(S));
 end else begin
  S := '';
  if ARow - 1 < Length(ModuleList) then begin
   Tmp := ModuleList[ARow - 1];
   case ACol of
    0 : begin
         S := ExtractFileName(Tmp.ImageName);
        end;
    1 : begin
         S := IntToHex(Tmp.Base, 6);
        end;
    2 : begin
         S := IntToHex(Tmp.Size, 6)+' ('+ IntToStr(Tmp.Size)+')';
        end;
    3 : begin
         S := Tmp.ImageName;
        end;
    4 : begin
         S := Tmp.Descr;
        end;
    5 : begin
         S := Tmp.LegalCopyright;
        end;
   end;
  // Цветовое выделение
  if not((gdFocused in State) or (gdSelected in State)) then begin
   if Tmp.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clGreen;
   if Tmp.Visible > 0 then
    (Sender as TRxDrawGrid).Canvas.Font.Color := clRed;
  end else begin
   if Tmp.CheckResult = 0 then
    (Sender as TRxDrawGrid).Canvas.Brush.Color := clGreen;
   if Tmp.Visible > 0 then
    (Sender as TRxDrawGrid).Canvas.Brush.Color := clRed;
  end;
  (Sender as TRxDrawGrid).Canvas.TextRect(Rect, Rect.Left+1, Rect.Top+1, S);
  end;
 end;
end;

function TKernelObjectsView.OrderResults(ACol: integer): boolean;
var
 i, j : integer;
 Tmp  : TKernelModuleInformation;
 function CompareItems(I1, I2: TKernelModuleInformation;
                        AKeyField: integer) : boolean;
 begin
  Result := false;
  case AKeyField of
   0 : Result := AnsiLowerCase(ExtractFileName(I1.ImageName)) > AnsiLowerCase(ExtractFileName(I2.ImageName));
   1 : Result := I1.Base > I2.Base;
   2 : Result := I1.Size > I2.Size;
   3 : Result := AnsiLowerCase(I1.ImageName) > AnsiLowerCase(I2.ImageName);
  end;
 end;
begin
 for i := 0 to Length(ModuleList) - 2 do
  for j := i+1 to Length(ModuleList) - 1 do
   if CompareItems(ModuleList[i], ModuleList[j], ACol) then begin
    Tmp :=  ModuleList[i];
    ModuleList[i] := ModuleList[j];
    ModuleList[j] := Tmp;
   end;
 dgKernelModules.Repaint;
end;

procedure TKernelObjectsView.dgKernelModulesFixedCellClick(Sender: TObject;
  ACol, ARow: Integer);
begin
 OrderColumnId := ACol;
 OrderResults(OrderColumnId);
end;

procedure TKernelObjectsView.sbRefreshClick(Sender: TObject);
begin
 RefreshModuleList;
end;

procedure TKernelObjectsView.FormCreate(Sender: TObject);
begin
 OrderColumnId := 0;
 TranslateForm(Self);
end;

procedure TKernelObjectsView.sbSaveLogClick(Sender: TObject);
var
 RepLines : TStringList;
begin
 RepLines := TStringList.Create;
 RepLines.AddStrings(HTML_StartLines);
 RepLines.AddStrings(HTML_BODYLines);
 RepLines.Add(TranslateStr('<H2 align=center>$AVZ0430</H2>'));
 CreateHTMLReport(RepLines, false);
 GUIShared.HTMLSaveDialog.FileName := 'avz_kernel_mod.htm';
 RepLines.Add('</BODY></HTML>');
 if GUIShared.HTMLSaveDialog.Execute and (GUIShared.HTMLSaveDialog.FileName <> '') then
  try
   RepLines.SaveToFile(GUIShared.HTMLSaveDialog.FileName);
   ShowHTMLLog(GUIShared.HTMLSaveDialog.FileName, true);
  except
  end;
 RepLines.Free;
end;

function TKernelObjectsView.CreateHTMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i, GoodFiles : integer;
 S, FileInfoStr : string;
begin
 ALines.Add('<TABLE '+HTML_TableFormat1+'>');
 ALines.Add('<TR '+HTMP_TableHeader+'>'+HTML_TableHeaderTD+'$AVZ0437'+HTML_TableHeaderTD+'$AVZ0073'+HTML_TableHeaderTD+'$AVZ0895'+HTML_TableHeaderTD+'$AVZ0581'+HTML_TableHeaderTD+'$AVZ0852');
 GoodFiles := 0;
 for i := 0 to Length(ModuleList)-1 do begin
  if ModuleList[i].CheckResult = 0 then begin
   S := 'bgColor='+HTMP_BgColorTrusted;
   inc(GoodFiles);
  end else S := 'bgColor='+HTMP_BgColor2;
  // Выделение маскирующихся красным
  if ModuleList[i].Visible > 0 then
   S := 'bgColor='+HTMP_BgColor3;
  if not(HideGoodFiles and (ModuleList[i].CheckResult = 0)) then begin
   FileInfoStr := FormatFileInfo(ExpandModuleFilename(ModuleList[i].ImageName), ', ');
   ALines.Add('<TR '+S+'><TD>'+'<a title="'+FileInfoStr+'" href="">'+HTMLNVL(ModuleList[i].ImageName)+'</a>'+ GenScriptTxt(ModuleList[i].ImageName)+
                        '<TD>'+IntToHex(ModuleList[i].Base, 6)+
                        '<TD>'+IntToHex(ModuleList[i].Size, 6)+' ('+IntToStr(ModuleList[i].Size)+')'+
                        '<TD>'+ModuleList[i].Descr+
                        '<TD>'+ModuleList[i].LegalCopyright);
  end;
 end;
 ALines.Add('<TR bgColor='+HTMP_BgColorFt+'><TD colspan=5>$AVZ0540 - '+IntToStr(Length(ModuleList))+', $AVZ0586 - '+IntToStr(GoodFiles));
 ALines.Add('</TABLE>');
 // Перевод протокола
 TranslateLines(ALines);
end;

procedure TKernelObjectsView.FormKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
 case Key of
  VK_F5 : sbRefresh.Click
 end;
end;

procedure TKernelObjectsView.dgKernelModulesClick(Sender: TObject);
begin
 if dgKernelModules.Row > length(ModuleList) then begin
  sbCopyProcToQurantine.Enabled := false;
  exit;
 end;
 sbCopyProcToQurantine.Enabled := not(ModuleList[dgKernelModules.Row-1].CheckResult = 0);
end;

procedure TKernelObjectsView.sbCopyProcToQurantineClick(Sender: TObject);
var
 FileName : string;
begin
 FileName := ExpandModuleFilename(ModuleList[dgKernelModules.Row-1].ImageName);
 if zMessageDlg('$AVZ0359 '+FileName+' $AVZ0090 ?',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 CopyToInfected(FileName, '$AVZ0985', 'Quarantine');
end;

procedure TKernelObjectsView.SpeedButton5Click(Sender: TObject);
begin
 if zMessageDlg('$AVZ0363',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AddAllToQurantine;
end;

function TKernelObjectsView.ExpandModuleFilename(AModuleName : string): string;
begin
 Result := NormalProgramFileName(AModuleName, '.sys');
end;

procedure TKernelObjectsView.mmQurantineClick(Sender: TObject);
begin
 sbCopyProcToQurantine.Click;
end;

procedure TKernelObjectsView.MenuItem5Click(Sender: TObject);
begin
 WebSearchInGoogle(ExtractFileName(ModuleList[dgKernelModules.Row-1].ImageName));
end;

procedure TKernelObjectsView.MenuItem6Click(Sender: TObject);
begin
 WebSearchInYandex(ExtractFileName(ModuleList[dgKernelModules.Row-1].ImageName));
end;

procedure TKernelObjectsView.MenuItem7Click(Sender: TObject);
begin
  WebSearchInRambler(ExtractFileName(ModuleList[dgKernelModules.Row-1].ImageName));
end;

function TKernelObjectsView.AddAllToQurantine: boolean;
var
 i : integer;
begin
 for i := 0 to Length(ModuleList) - 1 do
  if ModuleList[i].CheckResult <> 0 then
   CopyToInfected(ExpandModuleFilename(ModuleList[i].ImageName), '$AVZ0985', 'Quarantine');
end;

procedure TKernelObjectsView.SpeedButton1Click(Sender: TObject);
var
 Buf : array of byte;
 f   : file;
begin
 if not(AVZDriver.Loaded) then begin
  AVZDriver.InstallDriver;
  AVZDriver.LoadDriver;
 end;
 if not(AVZDriver.Loaded) then
  exit;
 SetLength(Buf, ModuleList[dgKernelModules.Row-1].Size);
 AVZDriver.CallDriver_READ_MEMORY(@Buf[0], ModuleList[dgKernelModules.Row-1].Base, ModuleList[dgKernelModules.Row-1].Size);
 AssignFile(f, 'c:\driver_dump.bin');
 Rewrite(f,1);
 CloseFile(f);
 AVZDriver.UnLoadDriver;
 AVZDriver.UnInstallDriver;
end;

procedure TKernelObjectsView.sbCreateDumpClick(Sender: TObject);
var
 FileName, FilePath : string;
 MS : TMemoryStream;
 Base, Size, DumpedSize, ReadSize : dword;
 Buf : packed array [0..1024] of byte;
begin
 FilePath := ExtractFilePath(Application.ExeName) + 'DMP\';
 FileName := ExtractFileName(ExpandModuleFilename(ModuleList[dgKernelModules.Row-1].ImageName));
 FileName := ChangeFileExt(FileName, '.dmp');
 if zMessageDlg('$AVZ0203 '+FileName+'  ?',
               mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 AVZDriver.InstallDriver;
 AVZDriver.LoadDriver;
 AVZDriver.CallDriver_IOCTL_Z_SETTRUSTEDPID;
 Base := ModuleList[dgKernelModules.Row-1].Base;
 Size := ModuleList[dgKernelModules.Row-1].Size;
 MS := TMemoryStream.Create;
 MS.Clear;
 MS.Seek(0,0);
 DumpedSize := 0;
 while DumpedSize < Size do begin
  ReadSize := min(Size - DumpedSize, SizeOf(Buf));
  ZeroMemory(@Buf[0], ReadSize);
  if not(AVZDriver.CallDriver_DUMP_MEMORY(@Buf[0], Base+DumpedSize, ReadSize)) then begin
   zMessageDlg('$AVZ0655', mtError, [mbOk], 0);
   exit;
  end;
  MS.Write(Buf, ReadSize);
  inc(DumpedSize, ReadSize);
 end;
 zForceDirectories(FilePath);
 MS.SaveToFile(FilePath + FileName);
 MS.Free;
 zMessageDlg('$AVZ0202', mtInformation, [mbOk], 0);
 AVZDriver.UnLoadDriver;
 AVZDriver.UnInstallDriver;
end;

function TKernelObjectsView.AddDataFromPM: boolean;
var
 DRIVER_LIST  : TDRIVER_LIST;
 i, j, poz : integer;
 ST : string;
begin
 if (AVZDriverRK = nil) or not(AVZDriverRK.Loaded) then  exit;
 // Установка нашему процессу права на управление драйвером
 AVZDriverRK.CallDriver_IOCTL_Z_SETTRUSTEDPID;
 if not(AVZDriverRK.IOCTL_Z_GET_DRIVERSLIST(DRIVER_LIST)) then exit;
 for i := 0 to 1023 do
  if DRIVER_LIST[i].Base > 0 then begin
    poz := -1;
    for j := 0 to Length(ModuleList) - 1 do
     if dword(ModuleList[j].Base) = DRIVER_LIST[i].Base then begin
      poz := j;
      break;
     end;
    if poz < 0 then begin
     Setlength(ModuleList, Length(ModuleList)+1);
     ModuleList[Length(ModuleList)-1].Base := DRIVER_LIST[i].Base;
     ModuleList[Length(ModuleList)-1].Size := DRIVER_LIST[i].Size;
     ModuleList[Length(ModuleList)-1].LoadCount := 1;
     ModuleList[Length(ModuleList)-1].ImageName := '??';
     ModuleList[Length(ModuleList)-1].CheckResult := -1;
     ModuleList[Length(ModuleList)-1].Visible := 1;
     if AVZDriverRK.IOCTL_Z_GET_DRIVER_FULLNAME(DRIVER_LIST[i].Base, ST) then
      ModuleList[Length(ModuleList)-1].ImageName := ST;
     // Проверка файла
     ModuleList[Length(ModuleList)-1].CheckResult := FileSignCheck.CheckFile(ExpandModuleFilename(ModuleList[Length(ModuleList)-1].ImageName));
    end;
  end;
end;

function TKernelObjectsView.AddDataFromMem: boolean;
var
 ScanBase, EndScanBase : dword;
 i : integer;
begin
 exit;
 for i := 0 to Length(ModuleList)-2 do
  if  ModuleList[i].Base > $800000 then begin
   ScanBase     := ((ModuleList[i].Base + ModuleList[i].Size) mod $1000) * $1000 + $1000;
   EndScanBase  := ModuleList[i+1].Base;
   while ScanBase < EndScanBase do begin
    inc(ScanBase, $1000);
   end
  end;
end;

function TKernelObjectsView.CreateXMLReport(ALines: TStrings;
  HideGoodFiles: boolean): boolean;
var
 i : integer;
 S, FileInfoStr : string;
begin
 ALines.Add(' <KERNELOBJ>');
 ZProgressClass.AddProgressMax(Length(ModuleList));
 for i := 0 to Length(ModuleList)-1 do begin
  ZProgressClass.ProgressStep;
  // Признак маскировки
  S := 'Hidden="0"';
  if ModuleList[i].Visible > 0 then
   S := 'Hidden="1"';
  if not(HideGoodFiles and (ModuleList[i].CheckResult = 0)) then begin
   FileInfoStr := FormatXMLFileInfo(ExpandModuleFilename(ModuleList[i].ImageName));
   ALines.Add('  <ITEM File="'+ModuleList[i].ImageName+'" '+
                        'CheckResult="'+IntToStr(ModuleList[i].CheckResult)+'" '+
                        'Base="'+IntToHex(ModuleList[i].Base, 6)+'" '+
                        'MemSize="'+IntToHex(ModuleList[i].Size, 6)+'" '+
                        'Descr="'+XMLStr(DelCRLF(ModuleList[i].Descr))+'" '+
                        'LegalCopyright="'+XMLStr(DelCRLF(ModuleList[i].LegalCopyright))+'" '+
                        FileInfoStr+' '+
                        '/>');
  end;
 end;
 ALines.Add(' </KERNELOBJ>');
end;

end.
