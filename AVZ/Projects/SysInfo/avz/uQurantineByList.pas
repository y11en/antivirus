unit uQurantineByList;
// ********************** Карантин по списку *************
interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, Buttons, zSharedFunctions, zAVZKernel;

type
  TQurantineByList = class(TForm)
    GroupBox1: TGroupBox;
    mmFileList: TMemo;
    GroupBox2: TGroupBox;
    mmLog: TMemo;
    Panel1: TPanel;
    mbStart: TBitBtn;
    BitBtn1: TBitBtn;
    procedure mbStartClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    procedure AddByMask(AFileMask, ACmt: string);
    { Private declarations }
  public
   Function AddToLog(S : string) : boolean;
   Function Execute : boolean;
  end;

var
  QurantineByList: TQurantineByList;

 function ExecuteQurantineByList : boolean;
implementation

uses zutil, zTranslate;

{$R *.dfm}

function ExecuteQurantineByList : boolean;
begin
 QurantineByList := nil;
 try
  QurantineByList := TQurantineByList.Create(nil);
  Result := QurantineByList.Execute;
 finally
  QurantineByList.Free;
  QurantineByList := nil;
 end;
end;

{ TQurantineByList }

function TQurantineByList.AddToLog(S: string): boolean;
begin
 mmLog.Lines.Add(TranslateStr(S));
end;

function TQurantineByList.Execute: boolean;
begin
 ShowModal;
 Result := ModalResult = mrOk;
end;

procedure TQurantineByList.AddByMask(AFileMask, ACmt : string);
var
 SR  : TSearchRec;
 Res : integer;
 FileName : string;
begin
 Res := FindFirst(AFileMask, faAnyFile, SR);
 while Res = 0 do begin
  if (SR.Name <> '.') and (SR.Name <> '..') and (SR.Attr and faDirectory = 0) then begin
   FileName := NormalDir(ExtractFilePath(AFileMask)) + SR.Name;
   if CopyToInfected(FileName, ACmt, 'Quarantine') then
    AddToLog('$AVZ1109 '+FileName+' $AVZ0220');
  end;
  Res := FindNext(SR);
 end;
 FindClose(SR);
end;

procedure TQurantineByList.mbStartClick(Sender: TObject);
var
 i : integer;
 FileMask, Cmt : string;
begin
 mmLog.Clear;
 AddToLog('$AVZ0876');
 for i := 0 to mmFileList.Lines.Count - 1 do begin
  FileMask := Trim(mmFileList.Lines[i]);
  Cmt      := '$AVZ0224 ($AVZ0326)';
  // Выделение комментария
  if pos('>', FileMask) > 0 then begin
   Cmt := '$AVZ0224 ('+Trim(copy(FileMask, pos('>', FileMask)+1, length(FileMask)))+')';
   Delete(FileMask, pos('>', FileMask), Length(FileMask));
  end;
  FileMask := NTFileNameToNormalName(FileMask);
   // Это имя файла в чистом виде
   if (pos('*', FileMask) = 0) and (pos('?', FileMask) = 0) then begin
    if CopyToInfected(FileMask, Cmt, 'Quarantine') then
     AddToLog('$AVZ1109 '+FileMask+' $AVZ0220')
    else begin
     if pos(':', FileMask) = 0 then begin
      if CopyToInfected(GetWindowsDirectoryPath + FileMask, Cmt, 'Quarantine') then
       AddToLog('$AVZ1109 '+GetWindowsDirectoryPath + FileMask+' $AVZ0220');
      if CopyToInfected(GetSystemDirectoryPath + FileMask, Cmt, 'Quarantine') then
       AddToLog('$AVZ1109 '+GetSystemDirectoryPath + FileMask+' $AVZ0220');
     end;
    end;
   end else begin
    AddByMask(FileMask, Cmt);
   end;
 end;
 AddToLog('$AVZ0875');
end;

procedure TQurantineByList.FormCreate(Sender: TObject);
begin
 TranslateForm(Self);
end;

end.
