unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, str_serialize;

type
  TForm1 = class(TForm)
    procedure FormCreate(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;

implementation

{$R *.dfm}

procedure TForm1.FormCreate(Sender: TObject);
var
 Str2Bin : TStr2Bin;
 Buf     : array[0..2048] of char;
 i, BW : integer;
 Ptr, PrevPtr : pointer;

begin
 ZeroMemory(@Buf[0], 2048);
 Str2Bin := TStr2Bin.Create;
 // Настройка
 Str2Bin.AddFieldDef('REGKEY',         ftPWChar, '');
 Str2Bin.AddFieldDef('CLSID',          ftPWChar, '');
 Str2Bin.AddFieldDef('BinFile',        ftPWChar, '');
 Str2Bin.AddFieldDef('Descr',          ftPWChar, '');
 Str2Bin.AddFieldDef('LegalCopyright', ftPWChar, '');
 Str2Bin.AddFieldDef('CheckResult',    ftDWORD, 0);
 Str2Bin.AddFieldDef('BHOType',        ftDWORD, 0);
 Str2Bin.CalkStaticDataSize;
 Ptr := @Buf[0];
 PrevPtr := nil;
 for i := 0 to 3 do begin
  // Заполнение полей
  Str2Bin.SetDefFieldsVal;
  Str2Bin['REGKEY']         := 'HKEY_LOCAL_MACHINE\SOFTWARE\Key'+IntToStr(i);
  Str2Bin['CLSID']          := '{00021A00-0000-0000-C000-000000000046}';
  Str2Bin['BinFile']        := 'c:\trojan'+IntToStr(i)+'.dll';
  Str2Bin['Descr']          := 'Это троян';
  Str2Bin['LegalCopyright'] := '(С) Вася Пупкин';
  Str2Bin['CheckResult']    := 0;
  Str2Bin['BHOType']        := 1;
  // Запись в буфер
  if Str2Bin.AddDataToBuf(Ptr, 2048, BW) then
   if PrevPtr <> nil then
    DWORD(PrevPtr^) := DWORD(Ptr);
  // Перенастройка указателей
  PrevPtr := Ptr;
  Ptr     := Pointer(DWORD(Ptr)+BW);
 end;
 Caption := inttostr(BW);
end;

end.
