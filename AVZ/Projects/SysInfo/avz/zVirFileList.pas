unit zVirFileList;

interface
uses SysUtils, Classes;
const
 vrtVirus          = 1;
 vrtVirusPartSign  = 2;
 vrtVirusEv        = 3;
 vrtVirusRootKit   = 4;
 vrtKeyLogger      = 5;

type
 // Описание вируса
 TVirusRec = record
  FileName : string;   // Имя файла
  Descr    : string;   // Имя вируса или сообщение
  VirType  : integer;  // Тип
  Script   : integer;  // Скрипт лечения
  Checked  : boolean;  // Признак того, что запись отмечена
 end;

 // Массив с описание файлов, в которых найдены вирусы
 TVirFileList = class
  private
    function GetVirFileCount: integer;
  public
   VirFileList : array of TVirusRec; // Список вирусов
   ChangeFlg   : boolean;
   constructor Create;
   // Добавление нового файла
   procedure AddVir(AFileName, ADescr : string; AVirType : integer; AScript : integer = -1);
   // Удаление записи
   procedure DeleteVir(AIndex : integer);
   // Очистка списка
   procedure Clear;
   // Сортировка списка
   procedure SortData(ACol : integer);
   // Сохранение в CSV формате
   function SaveAsCSV(AFileName : string) : boolean;
   Destructor Destroy; override;
   property VirFileCount : integer read GetVirFileCount;
 end;
implementation
uses ZTranslate;
{ TVirFileList }

procedure TVirFileList.AddVir(AFileName, ADescr: string;
  AVirType: integer; AScript : integer = -1);
var
 i : integer;
begin
 ChangeFlg := true;
 ADescr := TranslateStr(ADescr);
 // Блокировка повторного добавления записи (актуально в особенности для XML) 
 for i := 0 to Length(VirFileList) - 1 do
  if VirFileList[i].VirType = AVirType then
   if VirFileList[i].FileName = AFileName then
    exit;
 // Добавление записи
 SetLength(VirFileList, Length(VirFileList) + 1);
 VirFileList[Length(VirFileList) - 1].FileName := AFileName;
 VirFileList[Length(VirFileList) - 1].Descr    := ADescr;
 VirFileList[Length(VirFileList) - 1].VirType  := AVirType;
 VirFileList[Length(VirFileList) - 1].Checked  := false;
end;

procedure TVirFileList.Clear;
begin
 VirFileList := nil;
 ChangeFlg := true;
end;

constructor TVirFileList.Create;
begin
 VirFileList := nil;
end;

procedure TVirFileList.DeleteVir(AIndex: integer);
var
 i : integer;
begin
 if (AIndex < 0) or (AIndex >  Length(VirFileList) - 1) then exit;
 // Сдвиг записей
 for i := AIndex to Length(VirFileList) - 2 do
  VirFileList[i] := VirFileList[i+1];
 // Удаление последней
 SetLength(VirFileList, Length(VirFileList)-1);
 ChangeFlg := true;
end;

destructor TVirFileList.Destroy;
begin
 VirFileList := nil;
 inherited;
end;

function TVirFileList.GetVirFileCount: integer;
begin
 Result := length(VirFileList);
end;

function TVirFileList.SaveAsCSV(AFileName: string): boolean;
var
 Lines : TStringList;
 i : integer;
begin
 Result := false;
 Lines := TStringList.Create;
 try
  for i := 0 to VirFileCount - 1 do
   Lines.Add(VirFileList[i].FileName+';'+inttostr(VirFileList[i].VirType)+';'+VirFileList[i].Descr);
  Lines.SaveToFile(AFileName);
  Result := true;
 except
 end;
 Lines.Free;
end;

procedure TVirFileList.SortData(ACol: integer);
var
 i, j : integer;
 Tmp  : TVirusRec;
 function CompareItems(I1, I2 : TVirusRec) : boolean;
 begin
  Result := false;
  case ACol of
   0 : Result := AnsiLowerCase(I1.FileName) > AnsiLowerCase(I2.FileName);
   1 : Result := AnsiLowerCase(I1.Descr) > AnsiLowerCase(I2.Descr);
   2 : Result := I1.VirType > I2.VirType;
  end;
 end;
begin
 for i := 0 to Length(VirFileList) - 2 do
  for j := i+1 to Length(VirFileList) - 1 do
   if CompareItems(VirFileList[i], VirFileList[j]) then begin
    Tmp :=  VirFileList[i];
    VirFileList[i] := VirFileList[j];
    VirFileList[j] := Tmp;
   end;
end;

end.
