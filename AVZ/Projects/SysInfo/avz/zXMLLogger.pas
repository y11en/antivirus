unit zXMLLogger;
// Класс для регистрации и последующего внесения в XML доп. информации
interface
uses Windows, Messages, SysUtils, Classes, Forms, ComCtrls;
type
 TXMLLogger = class
  TagList : TStringList;
  constructor Create;
  destructor Destroy; override;
  // Очистка
  procedure Clear;
  // Добавление элемента с данными
  function AddItem(ATagName, AItemData : string) : boolean;
  // Добавление данных к некоему массиву строк
  function AddXMLData(ALines : TStringList) : boolean;
  // Очистка массива по имени
  function ClearByName(ATagName : string) : boolean;
 end;

implementation

{ ZXMLLogger }

function TXMLLogger.AddItem(ATagName, AItemData: string): boolean;
var
 i   : integer;
 Tmp : TStringList;
begin
 Result := false;
 ATagName  := Trim(UpperCase(ATagName));
 AItemData := Trim(AItemData);
 if AItemData = '' then exit;
 Tmp := nil;
 // Цикл поиска тега
 for i := 0 to TagList.Count - 1 do
  if ATagName = TagList[i] then begin
   Tmp := (TagList.Objects[i] as TStringList);
   Break;
  end;
 // Тег не найден - создадим и добавим его
 if Tmp = nil then begin
  Tmp := TStringList.Create;
  TagList.AddObject(ATagName, Tmp);
 end;
 Tmp.Add(AItemData);
 Result := true;
end;

function TXMLLogger.AddXMLData(ALines: TStringList): boolean;
var
 i,j : integer;
 Tmp : TStringList;
begin
 // Цикл вывода данных
 for i := 0 to TagList.Count - 1 do begin
  Tmp := (TagList.Objects[i] as TStringList);
  ALines.Add(' <'+Trim(UpperCase(TagList[i]))+'>');
  for j := 0 to Tmp.Count - 1 do
   ALines.Add('  '+Tmp[j]);
  ALines.Add(' </'+Trim(UpperCase(TagList[i]))+'>');
 end;
end;

procedure TXMLLogger.Clear;
var
 i : integer;
begin
 // Очитска вложенной информации
 for i := 0 to TagList.Count - 1 do begin
  (TagList.Objects[i] as TStringList).Free;
  TagList.Objects[i] := nil;
 end;
 // Очистка списка тегов
 TagList.Clear;
end;

function TXMLLogger.ClearByName(ATagName: string): boolean;
var
 i, indx   : integer;
begin
 Result := false;
 ATagName  := Trim(UpperCase(ATagName));
 indx := 0;
 // Цикл поиска тега
 for i := 0 to TagList.Count - 1 do
  if ATagName = TagList[i] then begin
   indx := i;
   Break;
  end;
 // Если тег не найден, то чистить нечего
 if indx = 0 then exit;
 // Чистка
 (TagList.Objects[indx] as TStringList).Free;
 TagList.Objects[indx] := nil;
 // Удаление элемента
 TagList.Delete(indx);
end;

constructor TXMLLogger.Create;
begin
 TagList := TStringList.Create;
end;

destructor TXMLLogger.Destroy;
begin
 Clear;
 TagList.Free;
 inherited;
end;

end.
