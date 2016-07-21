unit zDFMParser;

interface
uses classes, sysutils;
type
 TTranslateEvent = function(Sender : TObject; PropName, InStr : string; var OutStr : string) : boolean of object;
 // Базовый класс "свойство"
 TCustomDFMProperty = class
  PropName : string;
  function LoadFromLines(Lines : TStrings) : boolean; virtual; abstract;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean; virtual; abstract;
  function Translate(TranslateEvent : TTranslateEvent) : boolean; virtual; abstract;
 end;
 // Простое свойство (однострочное, вида параметр=значение)
 TDFMSimpleProperty = class(TCustomDFMProperty)
  PropVal : string;
  function LoadFromLines(Lines : TStrings) : boolean; override;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean; override;
  function Translate(TranslateEvent : TTranslateEvent) : boolean; override;
 end;
 // Многострочное свойство
 TDFMMultilineProperty = class(TCustomDFMProperty)
  PropVal : TStringList;
  constructor Create;
  function LoadFromLines(Lines : TStrings) : boolean; override;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean; override;
  function Translate(TranslateEvent : TTranslateEvent) : boolean; override;
 end;
 // Многострочное бинарное свойство
 TDFMMultilineBinProperty = class(TCustomDFMProperty)
  PropVal : TStringList;
  constructor Create;
  function LoadFromLines(Lines : TStrings) : boolean; override;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean; override;
  function Translate(TranslateEvent : TTranslateEvent) : boolean; override;
 end;
 // Элемет типа Item
 TDFMItem = class
  Objects : TList;
  LastStr : string;
  constructor Create;
  function LoadFromLines(Lines : TStrings) : boolean;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean;
  function Translate(TranslateEvent : TTranslateEvent) : boolean;
 end;
 // Свойство типа "коллекция Item"
 TDFMItemsProperty = class(TCustomDFMProperty)
  Objects : TList;
  constructor Create;
  function LoadFromLines(Lines : TStrings) : boolean; override;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean; override;
  function Translate(TranslateEvent : TTranslateEvent) : boolean; override;
 end;
 // Объект
 TDFMObject = class
  ObjName, ObjClassName : string;
  Objects : TList;
  constructor Create;
  function LoadFromLines(Lines : TStrings) : boolean;
  function SaveToLines(Lines : TStrings; ALevel : integer) : boolean;
  function Translate(TranslateEvent : TTranslateEvent) : boolean;
 end;
 // Парсер
 TDFMParser = class
  RootObject : TDFMObject;
  function LoadFromLines(Lines : TStrings) : boolean;
  function SaveToLines(Lines : TStrings) : boolean;
  function Translate(TranslateEvent : TTranslateEvent) : boolean;
 end;

function DFMStringToStr(S: string): string;
function StrToDFMString(S: string): string;

implementation

uses Contnrs;

procedure AddToLog(S : string);
begin

end;

function DFMStringToStr(S: string): string;
var
 ST : string;
 w : word;
Const
 DecodeTbl : string = 'Ё              АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяё';
begin
 S := Trim(S);
 Result := '';
 while Length(S) > 0 do begin
  // Текст в кавычках
  if s[1] = '''' then begin
   Delete(S, 1, 1);
   Result := Result + copy(S, 1, pos('''', S)-1);
   Delete(S, 1, pos('''', S));
   Continue;
  end;
  // Код символа
  if s[1] = '#' then begin
   Delete(S, 1, 1);
   ST := '';
   while (copy(S, 1, 1)+' ')[1] in ['0', '1'..'9'] do begin
    ST := ST + S[1];
    Delete(S, 1, 1);
   end;
   w := StrToInt(ST);
   case w of
    0..255 : ST := chr(w);
    8470 : ST := '№';
    1025..1105 : ST := DecodeTbl[w-1024];
   end;
   Result := Result + ST;
   Continue;
  end;
  Result := Result + s[1];
  Delete(S, 1, 1);
 end;
end;

function StrToDFMString(S: string): string;
begin
 Result := ''''+S+'''';
 Result := StringReplace(Result, '#$0D#$0A', '''#13#10''', [rfReplaceAll]);
 Result := StringReplace(Result, #$0D#$0A, '''#13#10''', [rfReplaceAll]);
end;

function ExtractLine(Lines : TStrings) : string;
begin
 Result := '';
 if Lines.Count > 0 then begin
  Result := Lines[0];
  Lines.Delete(0);
 end;
end;

{ TDFMParser }
function TDFMParser.LoadFromLines(Lines: TStrings): boolean;
begin
 Result := false;
 if Lines.Count < 2 then begin
  AddToLog('Ошибка: недопустимо малелький размер DFM файла');
  exit;
 end;

 RootObject := TDFMObject.Create;
 RootObject.LoadFromLines(Lines);
end;

function TDFMParser.SaveToLines(Lines: TStrings): boolean;
begin
 RootObject.SaveToLines(Lines, 0);
end;

function TDFMParser.Translate(TranslateEvent: TTranslateEvent): boolean;
begin
 Result := RootObject.Translate(TranslateEvent);
end;

{ TDFMObject }

constructor TDFMObject.Create;
begin
 Objects := TList.Create;
end;

function TDFMObject.LoadFromLines(Lines: TStrings): boolean;
var
 S : string;
 TmpO : TDFMObject;
 TmpI : TCustomDFMProperty;
 PropName, PropVal : string;
begin
 S := ExtractLine(Lines);
 // Проверка сигнатур
 if pos('object ', LowerCase(S)) = 0 then begin
  AddToLog('Ошибка: нет сигнатуры object в начале объекта');
  exit;
 end;
 // Выделение имени класса и его типа
 S := trim(S);
 ObjName      := Trim(copy(S, 7, pos(':', S) - 7));
 ObjClassName := Trim(copy(S, pos(':', S) + 1, maxint));
 repeat
  if Lines.Count = 0 then break;
  // Извлечение строки
  S := Trim(Lines[0]);
  // Пропуск пустых строк
  if S = '' then begin
   ExtractLine(Lines);
   Continue;
  end;
  // Это вложенный объект ?
  if pos('object ', LowerCase(S)) = 1 then begin
   TmpO := TDFMObject.Create;
   TmpO.LoadFromLines(Lines);
   Objects.Add(TmpO);
   Continue;
  end;
  // Это конец описания объекта ?
  if  LowerCase(S) = 'end' then begin
   Result := true;
   ExtractLine(Lines);
   break;
  end;
  // Это свойство
  if  pos('=', S) > 0 then begin
   PropName := Trim(copy(S, 1, Pos('=', S)-1));
   PropVal  := Trim(copy(S, Pos('=', S)+1, maxint));
   if  PropVal = '(' then begin
    TmpI := TDFMMultilineProperty.Create;
    TmpI.LoadFromLines(Lines);
    Objects.Add(TmpI);
   end else
   if  PropVal = '{' then begin
    TmpI := TDFMMultilineBinProperty.Create;
    TmpI.LoadFromLines(Lines);
    Objects.Add(TmpI);
   end else
   if  PropVal = '<' then begin
    TmpI := TDFMItemsProperty.Create;
    TmpI.LoadFromLines(Lines);
    Objects.Add(TmpI);
   end else begin
    TmpI := TDFMSimpleProperty.Create;
    TmpI.LoadFromLines(Lines);
    Objects.Add(TmpI);
   end;
  end;
 until false;
end;

function TDFMObject.SaveToLines(Lines: TStrings; ALevel : integer): boolean;
var
 LevST : string;
 i     : Integer;
begin
 LevST := ''; for i := 1 to ALevel do LevST := LevST + '  ';
 Lines.Add(LevST + 'object ' + ObjName + ': ' + ObjClassName);
 for i := 0 to Objects.Count - 1 do begin
  if (TObject(Objects[i]) is TDFMObject) then
   (TObject(Objects[i]) as TDFMObject).SaveToLines(Lines, ALevel + 1);
  if (TObject(Objects[i]) is TCustomDFMProperty) then
   (TObject(Objects[i]) as TCustomDFMProperty).SaveToLines(Lines, ALevel + 1);
 end;
 Lines.Add(LevST + 'end');
end;

function TDFMObject.Translate(TranslateEvent: TTranslateEvent): boolean;
var
 i     : Integer;
begin
 for i := 0 to Objects.Count - 1 do begin
  if (TObject(Objects[i]) is TDFMObject) then
   (TObject(Objects[i]) as TDFMObject).Translate(TranslateEvent);
  if (TObject(Objects[i]) is TCustomDFMProperty) then
   (TObject(Objects[i]) as TCustomDFMProperty).Translate(TranslateEvent);
 end;
 Result := true;
end;

{ TDFMSimpleProperty }

function TDFMSimpleProperty.LoadFromLines(Lines: TStrings): boolean;
var
 S, ST : String;
begin
 S := Trim(ExtractLine(Lines));
 PropName := Trim(copy(S, 1, Pos('=', S)-1));
 ST  := Trim(copy(S, Pos('=', S)+1, maxint));
 while ST = '' do
  ST := Trim(ExtractLine(Lines));
 // Длинная строка, разбитая на части ?
 while copy(ST, length(ST), 1) = '+' do begin
  delete(ST, length(ST), 1);
  ST := Trim(ST);
  S  := Trim(ExtractLine(Lines));
  if (copy(ST, length(ST), 1) = '''') and (copy(S, 1, 1) = '''') then begin
   delete(ST, length(ST), 1);
   delete(S, 1, 1);
  end;
  ST := ST + S;
 end;
 PropVal := ST;
end;

function TDFMSimpleProperty.SaveToLines(Lines: TStrings;
  ALevel: integer): boolean;
var
 LevST : string;
 i     : Integer;
begin
 LevST := ''; for i := 1 to ALevel do LevST := LevST + '  ';
 Lines.Add(LevST + PropName + ' = ' + PropVal);
end;

function TDFMSimpleProperty.Translate(
  TranslateEvent: TTranslateEvent): boolean;
var
 S     : string;
begin
 S := '';
 if Assigned(TranslateEvent) then
  if TranslateEvent(Self, PropName, PropVal, S) then
   PropVal := S;
end;

{ TDFMMultilineProperty }

constructor TDFMMultilineProperty.Create;
begin
 PropVal := TStringList.Create;
end;

function TDFMMultilineProperty.LoadFromLines(Lines: TStrings): boolean;
var
 S : String;
begin
 S := Trim(ExtractLine(Lines));
 PropName := Trim(copy(S, 1, Pos('=', S)-1));
 repeat
  S := Trim(ExtractLine(Lines));
  if copy(S, length(S), 1) <> ')' then
   PropVal.Add(S)
  else begin
   PropVal.Add(copy(S, 1, length(S)-1));
   Break;
  end;
 until false;
end;

function TDFMMultilineProperty.SaveToLines(Lines: TStrings;
  ALevel: integer): boolean;
var
 LevST, S : string;
 i     : Integer;
begin
 LevST := ''; for i := 1 to ALevel do LevST := LevST + '  ';
 Lines.Add(LevST + PropName + ' = (');
 LevST := LevST + '  ';
 for i := 0 to PropVal.Count - 1 do begin
  S := PropVal[i];
  if i = PropVal.Count - 1 then
   S := S + ')';
  Lines.Add(LevST + S);
 end;
end;

function TDFMMultilineProperty.Translate(
  TranslateEvent: TTranslateEvent): boolean;
var
 i     : Integer;
 S     : string;
begin
 for i := 0 to PropVal.Count - 1 do
  if Assigned(TranslateEvent) then begin
   S := '';
   if TranslateEvent(Self, PropName+'['+IntToStr(i)+']', PropVal[i], S) then
    PropVal[i] := S;
  end;
end;

{ TDFMMultilineBinProperty }

constructor TDFMMultilineBinProperty.Create;
begin
 PropVal := TStringList.Create;
end;

function TDFMMultilineBinProperty.LoadFromLines(Lines: TStrings): boolean;
var
 S : String;
begin
 S := Trim(ExtractLine(Lines));
 PropName := Trim(copy(S, 1, Pos('=', S)-1));
 repeat
  S := Trim(ExtractLine(Lines));
  if copy(S, length(S), 1) <> '}' then
   PropVal.Add(S)
  else begin
   PropVal.Add(copy(S, 1, length(S)-1));
   Break;
  end;
 until false;
end;


function TDFMMultilineBinProperty.SaveToLines(Lines: TStrings;
  ALevel: integer): boolean;
var
 LevST, S : string;
 i     : Integer;
begin
 LevST := ''; for i := 1 to ALevel do LevST := LevST + '  ';
 Lines.Add(LevST + PropName + ' = {');
 LevST := LevST + '  ';
 for i := 0 to PropVal.Count - 1 do begin
  S := PropVal[i];
  if i = PropVal.Count - 1 then
   S := S + '}';
  Lines.Add(LevST + S);
 end;
end;

function TDFMMultilineBinProperty.Translate(
  TranslateEvent: TTranslateEvent): boolean;
begin
 // В данном случае ничего не переводим, так как это бинарное свойство
 Result := true;
end;

{ TDFMItemsProperty }

constructor TDFMItemsProperty.Create;
begin
 Objects := TObjectList.Create
end;

function TDFMItemsProperty.LoadFromLines(Lines: TStrings): boolean;
var
 S : String;
 Tmp : TDFMItem;
begin
 S := Trim(ExtractLine(Lines));
 PropName := Trim(copy(S, 1, Pos('=', S)-1));
 repeat
  if Lines.Count = 0 then break;
  // Извлечение строки
  S := Trim(Lines[0]);
  // Пропуск пустых строк
  if S = '' then begin
   ExtractLine(Lines);
   Continue;
  end;
  if S = '>' then
   Break;
  if LowerCase(S) = 'item' then begin
   Tmp := TDFMItem.Create;
   Tmp.LoadFromLines(Lines);
   Objects.Add(Tmp);
   if copy(Tmp.LastStr, length(Tmp.LastStr), 1) = '>' then
    Break
  end;
 until false;
end;

function TDFMItemsProperty.SaveToLines(Lines: TStrings;
  ALevel: integer): boolean;
var
 LevST : string;
 i     : Integer;
begin
 LevST := ''; for i := 1 to ALevel do LevST := LevST + '  ';
 Lines.Add(LevST + PropName + ' = <');
 for i := 0 to Objects.Count - 1 do begin
  if (TObject(Objects[i]) is TDFMItem) then
   (TObject(Objects[i]) as TDFMItem).SaveToLines(Lines, ALevel + 1);
 end;
 Lines[Lines.Count-1] := Lines[Lines.Count-1] + '>';
end;

function TDFMItemsProperty.Translate(
  TranslateEvent: TTranslateEvent): boolean;
var
 i     : Integer;
begin
 for i := 0 to Objects.Count - 1 do begin
  if (TObject(Objects[i]) is TDFMItem) then
   (TObject(Objects[i]) as TDFMItem).Translate(TranslateEvent);
 end;
end;

{ TDFMItem }

constructor TDFMItem.Create;
begin
 Objects := TList.Create;
end;

function TDFMItem.LoadFromLines(Lines: TStrings): boolean;
var
 S, PropName, PropVal : string;
 TmpI : TCustomDFMProperty;
begin
 S := Trim(ExtractLine(Lines));
 // Проверка сигнатур
 if pos('item', LowerCase(S)) = 0 then begin
  AddToLog('Ошибка: нет сигнатуры item в начале объекта');
  exit;
 end;
 repeat
  if Lines.Count = 0 then break;
  // Извлечение строки
  S := Trim(Lines[0]);
  LastStr := S;
  // Пропуск пустых строк
  if S = '' then begin
   ExtractLine(Lines);
   Continue;
  end;
  // Это конец описания объекта ?
  if  (LowerCase(S) = 'end') or (LowerCase(S) = 'end>') then begin
   Result := true;
   ExtractLine(Lines);
   break;
  end;
  // Это свойство
  if  pos('=', S) > 0 then begin
   PropName := Trim(copy(S, 1, Pos('=', S)-1));
   PropVal  := Trim(copy(S, Pos('=', S)+1, maxint));
   if  PropVal = '(' then begin
   end else
   if  PropVal = '{' then begin
   end else
   if  PropVal = '<' then begin
   end else begin
    TmpI := TDFMSimpleProperty.Create;
    TmpI.LoadFromLines(Lines);
    Objects.Add(TmpI);
   end;
  end;
 until false;

end;

function TDFMItem.SaveToLines(Lines: TStrings; ALevel: integer): boolean;
var
 LevST : string;
 i     : Integer;
begin
 LevST := ''; for i := 1 to ALevel do LevST := LevST + '  ';
 Lines.Add(LevST + 'item');
 for i := 0 to Objects.Count - 1 do begin
  if (TObject(Objects[i]) is TCustomDFMProperty) then
   (TObject(Objects[i]) as TCustomDFMProperty).SaveToLines(Lines, ALevel + 1);
 end;
 Lines.Add(LevST + 'end');
end;

function TDFMItem.Translate(TranslateEvent: TTranslateEvent): boolean;
var
 i     : Integer;
begin
 for i := 0 to Objects.Count - 1 do begin
  if (TObject(Objects[i]) is TCustomDFMProperty) then
   (TObject(Objects[i]) as TCustomDFMProperty).Translate(TranslateEvent);
 end;
end;

end.
