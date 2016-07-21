unit zxml;
// ***************************************************************************
// ***************** Библиотека классов для работы с  XML ********************
// ********************** (C) Зайцев О.В.,2002 г. ****************************
// ***************************************************************************
interface
 uses classes, Sysutils;
 const
  // Символы, считаемые пробелом
  SpaceChars : set of char = [' ', #9, #$0D, #$0A];
  // Символы, считаемые кавычками
  QuoteChars : set of char = ['"', ''''];
  // Символы "Перевод строки/перевод каретки"
  CRLF = #$0D#$0A;
  // Таблица соответствия символа и его кода в HTML
  HTMLCodes : array[1..5,1..2] of string = (
          ('&',  '&amp;'),
          ('<',  '&lt;'),
          ('>',  '&gt;'),
          ('"',  '&quot;'),
          ('''', '&apos;'));

 type
  // Исключение для ошибок парсинга документа
  EXMLParserError = class(Exception)
  end;

  // Тип тега XML
  TXmlTokenType = (
    xttProcessingInstruction,  // Инструкция обработки <? ... ?>
    xttDocumentType,           // Тип документа <!DOCTYPE ... >
    xttComment,                // Комментарий <!-- -->
    xttCData,                  // Неанализируемые данные <![CDATA[ ... ]]>
    xttEntity,                 // Неанализируемые данные <!ENTITY ... ]]>
    xttTag,                    // Обычный тег
    xttText                    // Текстовое вхождение внутри тега
  );

  TXMLToken = class;
  TXMLParser = class;

  // Список ветвей XML
  TXMLTokenList = class
   protected
    FTokenList : TList;
   private
    function GetCount: integer;
    function GetToken(Index: integer): TXMLToken;
    procedure SetToken(Index: integer; const Value: TXMLToken);
   public
    Constructor Create;
    Destructor  Destroy; override;
    // Удаление всех тегов
    Procedure Clear;
    // Добавление тега
    Procedure Add(XMLToken : TXMLToken);
    // Удаление по индексу
    Procedure Delete(AIndex    : integer);
    // Удаление по тегу
    Procedure DeleteToken(AToken  : TXmlToken);
    // Поиск тега по имени
    Function  GetTokenByName(AName : string) : TXMLToken;
    // Сортировка
    procedure Sort(Compare: TListSortCompare);
    // Кол-во тегов
    property  Count : integer read GetCount;
    // Массив тегов
    property  Tokens[Index : integer] : TXMLToken read GetToken write SetToken; default;
  end;

  // Ветвь XML
  TXMLToken = class
   private
    FParentToken : TXMLToken;
    FParams     : TStringList;
    FSubNodes   : TXMLTokenList;
    FFTokenType : TXmlTokenType;
    FTokenType  : TXmlTokenType;
    FTokenName: string;
    FText: String;
    procedure SetTokenType(const Value: TXmlTokenType);
    function GetIsEmpty: boolean;
    function GetUpperTokenName: string;
   public
    Constructor Create(AParentToken : TXmlToken; ATokenType : TXmlTokenType; ATagText : string);
    Destructor  Destroy; override;
    // Добавление узла
    Function AddToken(ATokenType : TXmlTokenType; ATagText : string) : TXMLToken;
    // Парсер параметров
    Function ParseParams(ATagText : string) : boolean;
    // Получение текста = сумме всех текстовых вхождений
    Function GetFullText : String;
    // Чтение параметров
    Function ReadString(AName : string;  ADefVal : String)   : String;
    Function ReadInteger(AName : string; ADefVal : integer)  : Integer;
    Function ReadFloat(AName : string;   ADefVal : Double)   : Double;
    Function ReadBool(AName : string;    ADefVal : Boolean)  : Boolean;
    // Запись параметров
    Procedure WriteString(AName : string;  AValue : String);
    Procedure WriteInteger(AName : string; AValue : integer);
    Procedure WriteFloat(AName : string;   AValue : Double);
    Procedure WriteBool(AName : string;    AValue : Boolean);
    // Проверка существования параметра
    Function ValueExists(AName : string) : boolean;
    // Удаление параметра
    Function DeleteParam(AName : string) : boolean;
    //                 **** Свойства ****
    // Имя тега
    property TokenName        : string read FTokenName write FTokenName;
    property UpperTokenName   : string read GetUpperTokenName;
    // Тип тега
    property TokenType   : TXmlTokenType read FTokenType write SetTokenType;
    // Родительский тег (Nil у корневого)
    property ParentToken : TXmlToken read FParentToken;
    // Список параметров
    property Params   : TStringList read FParams;
    // Список подчиненных узлов
    property SubNodes : TXMLTokenList read FSubNodes;
    // Полный текст всех текстовых вхождений
    property Text     : String read FText write FText;
    // Проверка, является ли тег пустым
    property IsEmpty  : boolean read GetIsEmpty;
  end;

  // Обрабочтик отладочных сообщений
  TXMLDebugHandler = procedure(Sender:TXMLParser; Msg : String) of object;

  // Парсер XML файлов
  TXMLParser = class
   private
    FXMLText        : String;            // Текст XML
    FXmlTextPoz     : integer;           // Позиция внутри XML
    FCurrentXMLNode : TXMLToken;         // Текущая ветвь XML
    FXMLRootToken   : TXMLToken;         // Корневая ветвь XML
    FOnXMLDebug     : TXMLDebugHandler;  // Указатель на отладчик XML
    function GetEOF : boolean;
    procedure SkipSpaces;
    procedure SkipChar;
    procedure SkipChars(ACount : integer);
    Function SearchOpenTag : boolean;
    Function SearchCloseTag(ATagEnd : string) : integer;
    Function GetXMLChar(Index: Integer): char;
    Function SaveXMLToken(AXMLToken : TXMLToken; Lines : TStrings; Level : integer) : boolean;
    Function doDebugLog(S : string) : boolean;
    procedure SetOnXMLDebug(const Value: TXMLDebugHandler);
   public
    // Создание
    Constructor Create;
    // Очистка
    Procedure Clear;
    // Загрузка и парсинг XML
    Function LoadAndParseXML(AXMLFile : String) : boolean;
    // Парсинг XML
    Function ParseXML(AXMLText : String) : boolean;
    // Обработчик добавление тега к дереву тегов
    Function AddTag(ATokenType : TXmlTokenType;ATagText : String)  : boolean;
    // Добавления текстового вхождения в текущий тег
    Function AddTagText(ATagText : String) : boolean;
    // Поск тега
    Function SearchToken(ATagName : String) : TXMLToken;
    // Сохранение в Strings
    Function SaveToStrings(Lines : TStrings) : boolean;
    // Сохранение в файл
    Function SaveToFile(AFileName : string) : boolean;
    // Признак конца файла
    property EOF : boolean Read GetEOF;
    // Символ XML в позиции
    property XMLChar[Index: Integer] : char Read GetXMLChar;
    // Указатель на корневой элемент
    property XMLRootToken : TXMLToken read FXMLRootToken write FXMLRootToken;
    // Обработчик отладочных сообщений
    property OnXMLDebug : TXMLDebugHandler read FOnXMLDebug write SetOnXMLDebug;
  end;

  // Извлечение имени тега (удаление / в начале и конце имени)
  Function ExtractTagName(ATagText : string) : string;
implementation

// Извлечение имени тега
Function ExtractTagName(ATagText : string) : string;
var
 i : integer;
begin
 // Поиск первого символа, считаемого пробелом
 i := 1;
 while (i <= length(ATagText)) and not(ATagText[i] in SpaceChars) do
  inc(i);
 // Выделение имени тега
 Result := Trim(copy(ATagText, 1, i));
 // Удаление лишних символов / в начале и конце имени
 if copy(Result, 1, 1) = '/' then
  delete(Result, 1, 1);
 if copy(Result, length(Result), 1) = '/' then
  delete(Result, length(Result), 1);
end;

// Перевод 3 байт в 4 символа по Base64 кодированию
Function Code3to4(AValue : string) : ShortString;
begin

end;

{ TXMLParser }
// Конструктор
constructor TXMLParser.Create;
begin
 // Создание списка
 FCurrentXMLNode := nil;
 FOnXMLDebug     := nil;
end;

// Проверка, достигли ли мы конца текста
function TXMLParser.GetEOF: boolean;
begin
 Result := FXmlTextPoz > Length(FXMLText);
end;

// Считываение символа XML документа
function TXMLParser.GetXMLChar(Index: Integer): char;
begin
 if (Index > 0) and (Index < Length(FXMLText)) then
  Result := FXMLText[index]
   else Result := #0;
end;

// Пропуск пробелов
procedure TXMLParser.SkipSpaces;
begin
 while not(EOF) and (FXMLText[FXmlTextPoz] in SpaceChars) do
  inc(FXmlTextPoz);
end;

// Пропуск одного символа
procedure TXMLParser.SkipChar;
begin
 if not(EOF) then
  inc(FXmlTextPoz);
end;

// Пропуск нескольких символов
procedure TXMLParser.SkipChars(ACount: integer);
begin
 inc(FXmlTextPoz, ACount);
end;

// Поиск открывающего тега
function TXMLParser.SearchOpenTag: boolean;
begin
 while not(EOF) and (FXMLText[FXmlTextPoz] <> '<') do
  inc(FXmlTextPoz);
 Result := not(EOF);
end;

// Поиск закрывающего тега
function TXMLParser.SearchCloseTag(ATagEnd: string): integer;
var
 XMLLength, TagEndLen, poz : integer;
 FirstTagChar : char;
begin
 Result := 0;
 TagEndLen := Length(ATagEnd);
 if TagEndLen = 0 then exit;
 FirstTagChar := ATagEnd[1];
 XMLLength := Length(FXMLText);
 poz       := FXmlTextPoz;
 if ATagEnd = '' then exit;
  while not(poz > XMLLength) do
   if FXMLText[poz] <> FirstTagChar then inc(Poz)
    else
     if copy(FXMLText, Poz, TagEndLen) = ATagEnd then Break
      else inc(Poz);
 if copy(FXMLText, Poz, TagEndLen) = ATagEnd then
  Result := Poz;
end;


// Разбор XML документа
function TXMLParser.ParseXML(AXMLText: String): boolean;
var
 s   : string;
 poz : integer;
begin
 Result := false;
 // Очистка предыдущих результатов
 FCurrentXMLNode := nil;
 FXMLRootToken   := nil;
 FXMLText := AXMLText;
 FXmlTextPoz := 1;
 // Удаление лишних пробелов в начале файла
 SkipSpaces;
 while not EOF do begin
  SkipSpaces;
  poz := FXmlTextPoz;
  // Поиск первого открывающего тега
  if not SearchOpenTag then begin Result := true; exit; end;
  // До "<" тега были данные - если да, то добавим их к текущем тегу
  if (poz <> FXmlTextPoz) then
   AddTagText(Copy(FXMLText, poz, FXmlTextPoz - poz));
  // Пропуск символа '<'
  SkipChar;
  // Выделение 8-ми символов тега
  // *** Проверим, не является ли тег тегом специального типа ***
  if (XMLChar[FXmlTextPoz] in ['!','?']) then begin
    S := copy(FXMLText, FXmlTextPoz, 8);
    // Это комментарий ?
    if pos('!--', S) = 1 then begin
     SkipChars(3);
     poz := FXmlTextPoz;
     while not(EOF) and (copy(FXMLText, FXmlTextPoz, 3) <> '-->') do
      inc(FXmlTextPoz);
     AddTag(xttComment, Copy(FXMLText, poz, FXmlTextPoz - poz));
     // Пропуск символов "-->"
     SkipChars(3);
     Continue;
    end;
    // Это CDATA ??
    if pos('![CDATA[', S) = 1 then begin
     SkipChars(8);
     poz := SearchCloseTag(']]>');
     if poz >= FXmlTextPoz then begin
      AddTag(xttCData, copy(FXMLText, FXmlTextPoz, poz - FXmlTextPoz));
      FXmlTextPoz := poz + 3;
     end;
     Continue;
    end;
    // Это !DOCTUPE ??
    if pos('!DOCTYPE', S) = 1 then begin
     SkipChars(8);
     poz := SearchCloseTag('>');
     if poz >= FXmlTextPoz then begin
      AddTag(xttDocumentType, copy(FXMLText, FXmlTextPoz, poz - FXmlTextPoz));
      FXmlTextPoz := poz + 1;
     end;
     Continue;
    end;
    // Это !ENTITY ??
    if pos('!ENTITY', S) = 1 then begin
     SkipChars(6);
     poz := SearchCloseTag('>');
     if poz >= FXmlTextPoz then begin
      AddTag(xttEntity, copy(FXMLText, FXmlTextPoz, poz - FXmlTextPoz));
      FXmlTextPoz := poz + 1;
     end;
     Continue;
    end;
    // Это тег типа <? ?>
    if copy(S,1,1) = '?' then begin
     SkipChar;
     poz := SearchCloseTag('?>');
     if poz >= FXmlTextPoz then begin
      AddTag(xttProcessingInstruction, copy(FXMLText, FXmlTextPoz, poz - FXmlTextPoz));
      FXmlTextPoz := poz + 3;
     end;
     Continue;
    end;
   end;
   // *** Это обычный тег ***
   poz := SearchCloseTag('>');
   if poz >= FXmlTextPoz then begin
    AddTag(xttTag, copy(FXMLText, FXmlTextPoz, poz - FXmlTextPoz));
    FXmlTextPoz := poz + 1;
   end;
 end;
 FXMLText := '';
 Result := true;
end;


// Обработчик добавление тега к дереву тегов
function TXMLParser.AddTag(ATokenType : TXmlTokenType;ATagText : String)  : boolean;
var
 s, TagName : string;
begin
 ATagText := Trim(ATagText);
 if ATagText = '' then exit;
 // Формирование отладочного сообщения
 s := '';
 case ATokenType of
  xttProcessingInstruction : s := ' xttProcInstr : ';
  xttDocumentType          : s := ' xttDocType   : ';
  xttComment               : s := ' xttComment   : ';
  xttCData                 : s := ' xttCData     : ';
  xttEntity                : s := ' xttEntity    : ';
  xttTag                   : s := ' xttTag       : ';
 end;
 doDebugLog(s + ATagText);
 // Если текущего узла нет, то создаем его
 if FCurrentXMLNode = nil then begin
  FCurrentXMLNode := TXMLToken.Create(nil, xttTag, 'ROOT');
  FXMLRootToken   := FCurrentXMLNode;
 end;
 // Для служебных тегов добавляем ветвь без создания "директории"
 if ATokenType in [xttProcessingInstruction, xttDocumentType,
                   xttCData, xttComment, xttEntity] then begin
  FCurrentXMLNode.AddToken(ATokenType, ATagText);
 end else begin
  // Это пустой тег ?? Если да, то добавим его без создания директории
  if copy(ATagText, length(ATagText), 1) = '/' then begin
   FCurrentXMLNode.AddToken(ATokenType, ATagText);
  end else
  // Это закрывающий тег ?? Если да, то поднимаемся на уровень вверх
  if copy(ATagText,1,1) = '/' then begin
   if FCurrentXMLNode.ParentToken = nil then
    doDebugLog('Error: Закрывающий тег без открывающего: '+ATagText);
   // Проверим, еслт ли что закрывать
   if FCurrentXMLNode.ParentToken <> nil then begin
    // Выделение имени тега
    TagName := ExtractTagName(ATagText);
    // Проверка корректности имени тега
    if (FCurrentXMLNode.TokenName <> TagName) then
     doDebugLog('Error: Несоответствие закрывающего и открывающего тегов: найден "'+TagName+'", ожидался "'+FCurrentXMLNode.TokenName+'"')
    else FCurrentXMLNode := FCurrentXMLNode.ParentToken;
   end;
  end else begin
   // Это не пустой тег и не закрывающий - создаем "директорию"
   FCurrentXMLNode := FCurrentXMLNode.AddToken(ATokenType, ATagText);
  end;
 end;
end;

// Добавления текстового вхождения в текущий тег
function TXMLParser.AddTagText(ATagText: String): boolean;
begin
 doDebugLog('   TAG text  : ' + ATagText);
 FCurrentXMLNode.AddToken(xttText, ATagText);
end;

{ TXMLNode }

// Добавление тега
function TXMLToken.AddToken(ATokenType: TXmlTokenType;
  ATagText: string): TXMLToken;
begin
 // Создание нового узла
 Result := TXMLToken.Create(Self, ATokenType, ATagText);
 // Добавляем его к списку
 FSubNodes.Add(Result);
end;

// Создание
constructor TXMLToken.Create(AParentToken : TXmlToken; ATokenType : TXmlTokenType; ATagText : string);
begin
 FParams      := TStringList.Create;
 FSubNodes    := TXMLTokenList.Create;
 FParentToken := AParentToken;
 FTokenType   := ATokenType;
 FText        := '';
 case FTokenType of
  xttText    : begin
                FTokenName := 'TEXT';
                FText      := ATagText;
               end;
  xttComment : begin
                FTokenName := 'COMMENT';
                FText      := ATagText;
               end;
  xttCData   : begin
                FTokenName := 'CDATA';
                FText      := ATagText;
               end;
  else FTokenName   := ExtractTagName(ATagText);

 end;
 // Для инструкций обработки и тегов могут встречаться параметры
 if FTokenType in [xttProcessingInstruction, xttTag] then
   ParseParams(ATagText);
end;

// Парсер параметров
function TXMLToken.ParseParams(ATagText: string): boolean;
var
 i, poz : integer;
 ParamName, ParamVal : string; // Имя и значение параметра
 QuoteChar  : char;            // Символ кавычки
begin
 FParams.Clear;
 ATagText := Trim(ATagText);
 if ATagText = '' then exit;
 // У закрывающего тега не может быть параметров
 if copy(ATagText, 1, 1) = '/' then exit;
 // Последний символ может быть равен "/" - удалим его
 if copy(ATagText, length(ATagText), 1) = '/' then
  delete(ATagText, length(ATagText), 1);
 // Выделяем имя и удаляем его до поиск первого символа, считаемого пробелом
 i := 1;
 while (i <= length(ATagText)) and not(ATagText[i] in SpaceChars) do
  inc(i);
 delete(ATagText, 1, i);
 // Итак, параметры выделены, начинаем парсинг
 ATagText := Trim(ATagText);
 if ATagText <> '' then
  repeat
   poz := pos('=', ATagText);
   if poz > 0 then begin
    // Выделение имени тега
    ParamName := Trim(copy(ATagText, 1, poz - 1));
    Delete(ATagText, 1, poz);
    // ** Поиск открывающей и закрывающей кавычки **
    ATagText := Trim(ATagText);
    // Выделение первого значащего символа
    QuoteChar := #0;
    if length(ATagText) > 0 then
     QuoteChar := ATagText[1];
    // Если это символ является кавычкой, то ищем такую-же закрывающую
    if QuoteChar in QuoteChars then begin
     // Удалили открывающую кавычку
     Delete(ATagText, 1, 1);
     // Поиск закрывающей
     i := pos(QuoteChar, ATagText);
     // Найдена ??
     if i > 0 then begin
      ParamVal := copy(ATagText, 1, i-1);
      Delete(ATagText, 1, i);
      FParams.Add(ParamName + '=' + ParamVal);
     end;
    end;
   end;
  until poz = 0; // Поиск до тех пор, пока есть параметры
end;

// Проверка, является ли тег пустым
function TXMLToken.GetIsEmpty: boolean;
begin
 Result := FSubNodes.Count = 0;
end;

// Получение имени тега
procedure TXMLToken.SetTokenType(const Value: TXmlTokenType);
begin
  FTokenType := Value;
end;

// Сохрание узла
function TXMLParser.SaveXMLToken(AXMLToken: TXMLToken; Lines: TStrings;
  Level: integer): boolean;
var
 ParamsStr : string;  // Отформатированный список параметров
 i         : integer;
 st, s, TagText, CloseTagText, FormatText : string;
begin
 // Пустой тег не сохранять
 if AXMLToken = nil then
  exit;
 FormatText := '';
 for i := 1 to Level do
  FormatText := FormatText + ' ';
 // **** Форматирование параметров ****
 ParamsStr := '';
 for i := 0 to AXMLToken.Params.Count - 1 do begin
  if ParamsStr <> '' then
   ParamsStr := ParamsStr + ' ';
  s := copy(AXMLToken.Params[i], 1, pos('=', AXMLToken.Params[i]));
  st := copy(AXMLToken.Params[i], pos('=', AXMLToken.Params[i])+1, MaxInt);
  // Перекодировка специальных символов
  st := StringReplace(st, '<', '&lt;',[rfReplaceAll, rfIgnoreCase]);
  st := StringReplace(st, '>', '&gt;',[rfReplaceAll, rfIgnoreCase]);
  st := StringReplace(st, '"', '&quot;',[rfReplaceAll, rfIgnoreCase]);
  st := StringReplace(st, '''', '&apos;',[rfReplaceAll, rfIgnoreCase]);
  s := s+'"'+st+'"';
  ParamsStr := ParamsStr + s;
 end;
 // Текст открывающего и закрывающего тегов
 TagText := ''; CloseTagText := '';
 // Отделение строки параметров пробелом (если строка непустая)
 if ParamsStr <> '' then
  ParamsStr := ' ' + ParamsStr;
 // Подготоска тега в зависимости от типа и параметров
 case AXMLToken.TokenType of
  xttProcessingInstruction : TagText  := '<?'+AXMLToken.TokenName+ParamsStr+'?>';
  xttDocumentType          : TagText  := '<!DOCTYPE '+AXMLToken.Text+'>';
  xttComment               : TagText  := '<!--'+AXMLToken.Text+'-->';
  xttCData                 : TagText  := '<![CDATA['+AXMLToken.Text+']]>';
  xttEntity                : TagText  := '<!ENTITY'+AXMLToken.Text+'>';
  xttTag                   : begin
                              TagText  := '<'+AXMLToken.TokenName+ParamsStr;
                              if AXMLToken.IsEmpty then TagText := TagText + '/>'
                               else begin
                                TagText := TagText + '>';
                                CloseTagText := '</'+AXMLToken.TokenName+'>';
                               end;
                             end;
  xttText                  : TagText  := AXMLToken.Text;
 end;
 // Сохранение тега
 if AXMLToken <> XMLRootToken then begin
  Lines.Add(FormatText + TagText);
  // Сохранение всех вложенных тегов
  for i := 0 to AXMLToken.SubNodes.Count - 1 do
   SaveXMLToken(AXMLToken.SubNodes[i], Lines, Level+1);
  // Добавление закрывающего тега (если есть)
  if CloseTagText <> '' then
   Lines.Add(FormatText + CloseTagText);
 end else begin
  // Сохранение всех вложенных тегов
  for i := 0 to AXMLToken.SubNodes.Count - 1 do
   SaveXMLToken(AXMLToken.SubNodes[i], Lines, Level);
 end;
end;

// Сохрание в массив строк
function TXMLParser.SaveToStrings(Lines : TStrings): boolean;
begin
 Result := false;
 // Если нет корневого узла, то выход
 if FXMLRootToken = nil then exit;
 // Очистка массива строк
 Lines.Clear;
 // Рекурсивное сохрание
 SaveXMLToken(XMLRootToken, Lines, 0);
end;

// Выполнение отладочных действий
function TXMLParser.doDebugLog(S: string): boolean;
begin
 if Assigned(FOnXMLDebug) then
  FOnXMLDebug(Self, S);
end;


// Чтение строки
function TXMLToken.ReadString(AName, ADefVal: String): String;
var
  I: Integer;
begin
 I := FParams.IndexOfName(AName);
 if I >= 0 then begin
  Result := Copy(FParams[I], Length(AName) + 2, Maxint);
  Exit;
 end;
 Result := ADefVal;
end;

// Чтение логической переменной
function TXMLToken.ReadBool(AName: string; ADefVal: Boolean): Boolean;
var
 BoolStr : string;
begin
 Result := ADefVal;
 BoolStr := Trim(ReadString(AName, ''));
 if BoolStr <> '' then begin
  // Это символ "Y" или "1" ?
  if BoolStr[1] in ['Y', 'y', '1'] then begin
   Result := true;
   exit;
  end;
  // Это ненулевое число ?
  Result := ReadInteger(AName, Ord(ADefVal)) <> 0;
 end;
end;

// Чтение дробного числа
function TXMLToken.ReadFloat(AName: string; ADefVal: Double): Double;
var
  FloatStr: string;
begin
  FloatStr := ReadString(AName, '');
  Result := ADefVal;
  if FloatStr <> '' then
  try
    Result := StrToFloat(FloatStr);
  except
    on EConvertError do
    else raise;
  end;
end;

// Чтение целого числа
function TXMLToken.ReadInteger(AName: string; ADefVal: integer): Integer;
var
  IntStr: string;
begin
  IntStr := ReadString(AName, '');
  if (Length(IntStr) > 2) and (UpperCase(copy(IntStr,1,2)) = '0X') then
    IntStr := '$' + Copy(IntStr, 3, Maxint);
  Result := StrToIntDef(IntStr, ADefVal);
end;

// Запись логической переменной
procedure TXMLToken.WriteBool(AName: string; AValue: Boolean);
const
  Values: array[Boolean] of string = ('0', '1');
begin
  WriteString(AName, Values[AValue]);
end;

// Запись дробного числа
procedure TXMLToken.WriteFloat(AName: string; AValue: Double);
begin
 WriteString(AName, FloatToStr(AValue));
end;

// Запись целого числа
procedure TXMLToken.WriteInteger(AName: string; AValue: integer);
begin
 WriteString(AName, IntToStr(AValue));
end;

// Запись строки
procedure TXMLToken.WriteString(AName, AValue: String);
var
  I: Integer;
  S: string;
begin
  S := AName + '=' + AValue;
  I := FParams.IndexOfName(AName);
  if I >= 0 then FParams[I] := S else FParams.Add(S);
end;

// Проверка наличия параметра с указанным именем
function TXMLToken.ValueExists(AName : string): boolean;
begin
 Result := FParams.IndexOfName(AName) >= 0;
end;

// Получение текста = сумме всех текстовых вхождений
function TXMLToken.GetFullText: String;
var
 i : integer;
begin
 Result := '';
 // Это текстовый тег - вернем текст
 if FTokenType = xttText then begin
  Result := Text;
  exit;
 end;
 // Это обычный тег - ищем текстовые вхождения
 for i := 0 to FSubNodes.Count - 1 do
  if FSubNodes[i].TokenType = xttText then
   Result := Result + ' ' + FSubNodes[i].Text;
end;

// Передача сообщения отладчику
procedure TXMLParser.SetOnXMLDebug(const Value: TXMLDebugHandler);
begin
  FOnXMLDebug := Value;
end;

{ ********************** TXMLTokenList ********************* }

// Создание
constructor TXMLTokenList.Create;
begin
 FTokenList := TList.Create;
end;

// Добавление тега
procedure TXMLTokenList.Add(XMLToken: TXMLToken);
begin
 FTokenList.Add(XMLToken);
end;

// Получене количества тегов
function TXMLTokenList.GetCount: integer;
begin
 Result := FTokenList.Count;
end;

// Получение тега по индексу
function TXMLTokenList.GetToken(Index: integer): TXMLToken;
begin
 Result := FTokenList[Index];
end;

// Установка тега по индексу
procedure TXMLTokenList.SetToken(Index: integer; const Value: TXMLToken);
begin
 FTokenList[Index] := Value;
end;

// Поиск тега по имени (выбирается первый с заданным именем или NIL)
function TXMLTokenList.GetTokenByName(AName: string): TXMLToken;
var
 i : integer;
begin
 Result := nil;
 // Подготовка имени (поиск не регистрочувствительный)
 AName := UpperCase(Trim(AName));
 // Поиск
 for i := 0 to FTokenList.Count - 1 do
  if AName = UpperCase(Tokens[i].TokenName) then begin
   Result := Tokens[i]; // Тег найден - возврат и выход
   break;
  end;
end;


// Удаление всех тегов
procedure TXMLTokenList.Clear;
var
 i : integer;
begin
 for i := 0 to FTokenList.Count - 1 do
  TXMLToken(FTokenList[i]).Free;
 FTokenList.Clear;
end;

// Удаление по индексу
procedure TXMLTokenList.Delete(AIndex: integer);
begin
 FTokenList.Delete(AIndex);
end;

// Удаление по тегу
procedure TXMLTokenList.DeleteToken(AToken: TXmlToken);
var
 i : integer;
begin
 // Поиск тега
 for i := 0 to FTokenList.Count - 1 do
  if FTokenList[i] = AToken then begin
   Delete(i); // Тег найден - удаление по индексу
   exit;
  end;
end;

function TXMLParser.SearchToken(ATagName: String): TXMLToken;
var
 Tmp : TXMLToken;
 s : string;
begin
 Result := nil;
 ATagName := trim(UpperCase(ATagName));
 if ATagName = '' then exit;
 if FXMLRootToken = nil then exit;
 tmp := FXMLRootToken;
 while Length(ATagName) > 0 do begin
  if pos('\',ATagName) > 0 then begin
   s := copy(ATagName, 1, pos('\',ATagName) - 1);
   delete(ATagName, 1, pos('\',ATagName));
  end else begin
   s := ATagName;
   ATagName := '';
  end;
  Tmp  := Tmp.SubNodes.GetTokenByName(s);
  if Tmp = nil then exit;
 end;
 Result := Tmp;
end;

function TXMLToken.DeleteParam(AName: string): boolean;
var
  I: Integer;
begin
 I := FParams.IndexOfName(AName);
 if I >= 0 then
  FParams.Delete(i);
 Result := i>=0;
end;

function TXMLToken.GetUpperTokenName: string;
begin
 Result := UpperCase(Trim(TokenName));
end;

// Загрузка и парсинг XML
function TXMLParser.LoadAndParseXML(AXMLFile: String): boolean;
var
 FS : TFileStream;
 AFileData : AnsiString;
begin
 Result := false;
 // Создание потока
 FS := TFileStream.Create(AXMLFile, fmOpenRead);
 try
   // Загрузка данных
   FS.Position := 0;
   SetLength(AFileData, FS.Size);
   FS.ReadBuffer(AFileData[1], FS.Size);
   FS.Free;
   // Парсинг
   Result := ParseXML(AFileData);
   AFileData := '';
 except
  raise;
 end;
end;

procedure TXMLParser.Clear;
begin
 FCurrentXMLNode := nil;
// FXMLRootToken.Free;
 FXMLRootToken   := nil;
end;

destructor TXMLToken.Destroy;
begin
 FParams.Free;
 FSubNodes.Free;
 FParentToken := nil;
 inherited;
end;

destructor TXMLTokenList.Destroy;
begin
 Clear;
 FTokenList.Free;
 FTokenList := nil;
 inherited;
end;

procedure TXMLTokenList.Sort(Compare: TListSortCompare);
begin
 FTokenList.Sort(Compare);
end;

function TXMLParser.SaveToFile(AFileName: string): boolean;
var
 Lines : TStrings;
begin
 try
  Lines := TStringList.Create;
  // Создание Strings с результирующим XML
  SaveToStrings(Lines);
  // Сохранение полученного XML в файл
  Lines.SaveToFile(AFileName);;
 finally
  Lines.Free;
 end;
end;

end.


