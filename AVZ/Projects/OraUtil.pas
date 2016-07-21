unit orautil;
// Компонент OraUtil - сервисные функции, облегчающие Oracle программирование
// (C) Зайцев О.В.
// Дата последней правки 22.02.2001

interface
Uses Classes,Sysutils,Variants;

// Замена NULL пустой строкой
Function NVL(AVal:variant) : variant;overload;
// Замена NULL указанным значением
Function NVL(AVal:variant; NullVal  : variant) : variant; overload;
// Проверка на NULL
Function IsNull(AVal:variant):boolean;
// Проверка на не NULL
Function IsNotNull(AVal:variant):boolean;
// Модификация строк запроса, найденных по ключу-комментарию
Function ModifySQLQuery(const ASQL:TStrings;AKey,ANewStr:string):boolean;
// Добавление условия к списку условий
Function AddToList(var AList:string;AItem:string;ADelim:string=','):boolean;
// Разделить полное имя таблицы на схему и таблицу
Function SplitTableName(ATable : string; var Shema, Table : string) : boolean;

Function IntToStrNVL(AVal:variant; NullVal  : string = '') : string;
Function FloatToStrNVL(AVal:variant; NullVal  : string = '') : string;
Function DateToStrNVL(AVal:variant; NullVal  : string = '') : string;
implementation

// Проверка на NULL
Function IsNull(AVal:variant):boolean;
begin
 Result :=  VarIsNull(AVal) or  VarIsEmpty(AVal);
end;

// Проверка на не NULL
Function IsNotNull(AVal:variant):boolean;
begin
 Result :=  Not(IsNull(AVal));
end;

// Замена NULL пустой строкой
Function NVL(AVal:variant) : variant;
begin
 Result := AVal;
 if VarIsNull(AVal) or VarIsEmpty(AVal) then
  Result := '';
end;

// Замена NULL указанным значением
Function NVL(AVal:variant; NullVal  : variant) : variant;
begin
 Result := AVal;
 if VarIsNull(AVal) or VarIsEmpty(AVal) then
  Result := NullVal;
end;

// Модификация строк запроса, найденных по ключу-комментарию
Function ModifySQLQuery(const ASQL:TStrings;AKey,ANewStr:string):boolean;
var
 i : integer;
 Key : string;
begin
 Result := false;
 // Подготовка ключа поиска
 Key := AnsiUpperCase(Trim(AKey));
 // При пустом ключе ничего не ищем
 if AKey='' then exit;
 for i:=0 to ASQL.Count-1 do
  // Нашли ?? Если да, то меняем
  if Pos(Key,ASQL[i]) > 0 then begin
   ASQL[i] := ANewStr + ' '+ Key;
   Result := true;
  end;
end;

// Добавление условия к списку условий
Function AddToList(var AList:string;AItem:string;ADelim:string=','):boolean;
var
 FItem : string;
begin
 Result := false;
 // Удаление лишних пробелов
 FItem := Trim(AItem);
 // Пустые элементы не добавляем
 if FItem='' then exit;
 // Если список не пустой, то добавим к нему разделитель
 if Trim(AList) <> '' then
  AList := AList + ADelim;
 // Добавим элемент
 AList := AList + AItem;
 Result := true;
end;

// Разделить полное имя таблицы на схему и таблицу
Function SplitTableName(ATable : string; var Shema, Table : string) : boolean;
begin
 // Разделение по символу точки
 if pos('.', ATable) > 0 then begin
  Shema := copy(ATable, 1, pos('.', ATable)-1);
  Table := copy(ATable, pos('.', ATable)+1, 100);
 end else begin
  Shema := '';
  Table := ATable;
 end;
 Result := true;
end;

Function IntToStrNVL(AVal:variant; NullVal  : string) : string;
begin
 if IsNull(AVal) then
  Result := NullVal
   else Result := IntToStr(AVal);
end;

Function FloatToStrNVL(AVal:variant; NullVal  : string) : string;
begin
 if IsNull(AVal) then
  Result := NullVal
   else Result := FloatToStr(AVal);
end;

Function DateToStrNVL(AVal:variant; NullVal  : string) : string;
begin
 if IsNull(AVal) then
  Result := NullVal
   else Result := DateToStr(AVal);
end;

end.
