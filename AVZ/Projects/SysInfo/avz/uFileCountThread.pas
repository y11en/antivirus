unit uFileCountThread;
// Поток, подсчитывающий файлы, которые предполагается просканировать
interface

uses
  SysUtils, Classes, ComCtrls, zTreeViews, zAVZKernel;

type
  // Обработчик события "найдена новая порция файлов"
  TFileCountThreadProgress = procedure (Sender: TObject; ScanFilesCount, FullFilesCount : int64; AScanComplete : boolean) of object;
  TFileCountThread = class(TThread)
  private
    FOnProgress: TFileCountThreadProgress;
    function ScanTreeDirs(ARootNode: TTreeNode;
      ADeleteF: boolean): boolean;
    function ScanDir(ADir: string; AScanSubdir: boolean): boolean;
    procedure SetOnProgress(const Value: TFileCountThreadProgress);
    { Private declarations }
  protected
    procedure DoOnProgress;
    procedure Execute; override;
  public
    ScanFilesCount, FullFilesCount : int64;
    StopF, ScanAllFiles, ScanDangerFiles, ScanComplete, cbIncludeFiles, cbExcludeFiles : boolean;
    property OnProgress : TFileCountThreadProgress read FOnProgress write SetOnProgress;
  end;

implementation
uses Unit1, zutil;

{ TFileCountThread }

function TFileCountThread.ScanDir(ADir : string; AScanSubdir : boolean) : boolean;
var
 SR  : TSearchRec;
 FileExt, FileName : string;
 Res : integer;
begin
 Result := false;
 try
   try
    Res := FindFirst(NormalDir(ADir)+'*.*', faAnyFile, SR);
    while Res = 0 do begin
     inc(FullFilesCount);
     // Прерывание операции
     if StopF then begin
      abort;
     end;
     if ((SR.Attr and faDirectory) = 0) then begin
      FileName := SR.Name;
      FileExt := UpperCase(ExtractFileExt(FileName));
      // Фильтр
      if (ScanAllFiles or
         (ScanDangerFiles and (pos(FileExt, ExtToScan) > 0)) or
         (cbIncludeFiles and MatchesMask(FileName, Main.IncludeMackList))) and
         not(cbExcludeFiles and MatchesMask(FileName, Main.ExcludeMaskList)) then
          inc(ScanFilesCount);
      if ScanFilesCount mod 300 = 0 then
        Synchronize(DoOnProgress);
     end;
     if AScanSubdir then
      if (SR.Attr and faDirectory) > 0 then
       if (SR.Name <> '.') and (SR.Name <> '..') then begin
        ScanDir(NormalDir(ADir + SR.Name), AScanSubdir);
       end;
     Res := FindNext(SR);
    end;
    Result := true;
   finally
    try FindClose(SR);except end;
   end;
 except
  // Давим все ошибки - они возможны из-за нехватки прав и т.п.
 end;
end;

function TFileCountThread.ScanTreeDirs(ARootNode: TTreeNode;
  ADeleteF: boolean): boolean;
var
 i : integer;
 res : boolean;
begin
 Result := true;
 if StopF then exit;
 try
   // Дальнейшие действия
   case ARootNode.StateIndex of
    0, 1 : ; // Не выделен - никакмх действий
    2    : Result := ScanDir(NormalDir(Main.ZShellTreeView.GetPath(ARootNode)), true);
    3    : begin
            //Result := ScanDir(NormalDir(Main.ZShellTreeView.GetPath(ARootNode)), false);
            for i := 0 to ARootNode.Count - 1 do
             if ARootNode.Item[i].StateIndex > 1 then begin
              res := ScanTreeDirs(ARootNode.Item[i], ADeleteF);
              Result := Result and res;
             end;
           end;
    end;
  except end;
end;

procedure TFileCountThread.Execute;
var
 i : integer;
begin
 StopF := false;
 ScanComplete := false;
 ScanAllFiles := Main.ScanAllFiles;
 ScanFilesCount := 0;
 FullFilesCount := 0;
 try
   try
    // Первый вызов прогресс-индикатора
    Synchronize(DoOnProgress);
    // Поиск файлов
    for i := 0 to Main.ZShellTreeView.Items.Count - 1 do begin
     if (Main.ZShellTreeView.Items[i].Parent = nil) then
      ScanTreeDirs(Main.ZShellTreeView.Items[i], true);
    end;
   finally
    // Последний вызов - для фиксации окончательного результата
    ScanComplete := true;
    Synchronize(DoOnProgress);
   end;
 except end;
end;

procedure TFileCountThread.SetOnProgress(
  const Value: TFileCountThreadProgress);
begin
  FOnProgress := Value;
end;

procedure TFileCountThread.DoOnProgress;
begin
 try
  if Assigned(FOnProgress) then begin
   OnProgress(Self, ScanFilesCount, FullFilesCount, ScanComplete);
  end;
 except end; 
end;

end.
