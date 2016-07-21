unit zFileSearch;

interface
uses Windows, Messages, SysUtils, Classes, Forms, ComCtrls;
type
 TFileSearch = class(TComponent)
  private
    SR : TSearchRec;
    FFileFound: boolean;
    function GetFileName: string;
    function GetFileAttr: byte;
    function GetFileTime: TDateTime;
    function GetFileISDir: boolean;
  public
    Destructor Destroy; override;
    function FindFirst(AFileMask : string) : boolean;
    function FindNext : boolean;
    function FindClose : boolean;
  published
   property Found    : boolean read FFileFound;
   property ISDir    : boolean read GetFileISDir;
   property FileName : string read GetFileName;
   property FileAttr : byte read GetFileAttr;
   property FileTime : TDateTime read GetFileTime;
 end;
implementation

{ TFileSearchRec }

destructor TFileSearch.Destroy;
begin
 SysUtils.FindClose(SR);
 inherited;
end;

function TFileSearch.FindClose: boolean;
begin
 Result := true;
 SysUtils.FindClose(SR);
 FFileFound := false;
end;

function TFileSearch.FindFirst(AFileMask: string): boolean;
begin
 Result := SysUtils.FindFirst(AFileMask, faAnyFile, SR) = 0;
 FFileFound := Result;
end;

function TFileSearch.FindNext: boolean;
begin
 Result := SysUtils.FindNext(SR) = 0;
 FFileFound := Result;
end;

function TFileSearch.GetFileAttr: byte;
begin
 Result := SR.Attr;
end;

function TFileSearch.GetFileISDir: boolean;
begin
 Result := (SR.Attr and faDirectory) > 0;
end;

function TFileSearch.GetFileName: string;
begin
 Result := SR.Name;
end;

function TFileSearch.GetFileTime: TDateTime;
begin
 Result := Sr.Time;
end;

end.
 
