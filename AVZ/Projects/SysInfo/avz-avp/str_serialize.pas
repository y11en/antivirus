unit str_serialize;

interface
uses SysUtils, Types;
type
 // Поддерживаемые типы полей
 TZFieldType = (ftByte, ftWord, ftDWORD, ftInteger, ftPChar, ftPWChar, ftNotDefined);
 // Описание поля
 TZFiledDef = record
  FieldName : String;      // Имя поля
  FieldType : TZFieldType; // Тип данных поля
  DefVal    : variant;     // Значение по умолчанию
  Value     : variant;     // Значение
 end;
 // Класс - конвертер
 TStr2Bin = class
  FStaticSize : integer;
  FieldDefs : array of TZFiledDef;
  private
    function GetFieldValue(const FieldName: string): Variant;
    procedure SetFieldValue(const AFieldName: string; const Value: Variant);
  public
   constructor Create;
   // Очистка списка описателей полей
   function ClearFieldDefs : boolean;
   // Добавление поля
   function AddFieldDef(AFieldName : string; AFieldType : TZFieldType; ADefVal : variant) : boolean;
   // Вычисление статического размера данных
   procedure CalkStaticDataSize;
   // Установка всем полям значений по умолчанию
   function SetDefFieldsVal : boolean;
   function CalkDynamicDataSize : integer;
   // Парсинг строки с данными и заполнение полей
   function ParseString(AStr : string; ADivStr : string) : boolean;
   // Добавление текущих данных в указанный буфер
   function AddDataToBuf(Buf : pointer; BufSize : integer; var BytesWritten : integer) : boolean;
   // Вычисление размера текущей записи в буфере
   function CalkDataSize : dword;
   property FieldValues[const FieldName: string]: Variant read GetFieldValue write SetFieldValue; default;
 end;
implementation

// Определение размера в байтах по типу
function GetStaticSizeByType(AFieldType: TZFieldType) : integer;
begin
 Result := 0;
 case AFieldType of
  ftByte   : Result := 1;
  ftWord   : Result := 2;
  ftDWORD, ftInteger  : Result := 4;
  ftPWChar, ftPChar : Result := 4; // 4 байта - указатель на саму строку
 end;
end;

{ TStr2Bin }
function TStr2Bin.AddDataToBuf(Buf: pointer; BufSize : integer;
  var BytesWritten: integer): boolean;
var
 i : integer;
 DynamicSize, BufDynamicSize : integer;
 StartPtr, DynDataPtr : pointer;
begin
 Result := false;
 BytesWritten := 0;
 // буфер = NIL, то нет резона и пытаться
 if Buf = nil then exit;
 // Если в буфер не влезет статическая часть, то нет резона и пытаться
 if BufSize < FStaticSize then exit;
 // Если в буфер не влезет статическая + динамическая часть, то выход
 if BufSize < FStaticSize + CalkDynamicDataSize then exit;
 BufDynamicSize := BufSize - FStaticSize;
 // Запоминаем позиции в буфере
 StartPtr    := Buf;
 DynDataPtr  := Pointer(dword(Buf) + FStaticSize);
 // Обнуление указателя
 DWORD(Buf^) := 0;
 // Пропуск указателя на N+1 элемент
 Buf := Pointer(dword(Buf) + 4);
 // Цикл преобразования
 for i := 0 to Length(FieldDefs) - 1 do begin
  case FieldDefs[i].FieldType of
   ftByte  : begin
              byte(Buf^) := byte(FieldDefs[i].Value);
              Buf := Pointer(dword(Buf) + 1);
             end;
   ftWord  : begin
              Word(Buf^)    := Word(FieldDefs[i].Value);
             end;
   ftDWORD : begin
              DWORD(Buf^)   := DWORD(FieldDefs[i].Value);
             end;
   ftInteger : begin
              LongInt(Buf^) := LongInt(FieldDefs[i].Value);
             end;
   ftPWChar : begin
               DWORD(Buf^) := DWORD(DynDataPtr);
               // Запись строки
               StringToWideChar(FieldDefs[i].Value, DynDataPtr, BufDynamicSize);
               // Смещение указателя на L*2 + 2 байта
               DynDataPtr := Pointer(DWORD(DynDataPtr) + Length(FieldDefs[i].Value) * 2 + 2);
               dec(BufDynamicSize, Length(FieldDefs[i].Value) * 2 + 2);
              end;
  end;
  // Смещение указателя в рабочем буфере
  Buf := Pointer(dword(Buf) + GetStaticSizeByType(FieldDefs[i].FieldType));
 end;
 // Вычисление записанного размера
 BytesWritten := DWORD(DynDataPtr) - DWORD(StartPtr);
 Result := true;
end;

function TStr2Bin.AddFieldDef(AFieldName: string; AFieldType: TZFieldType;
  ADefVal: variant): boolean;
begin
 Result := false;
 // Добавление описания
 SetLength(FieldDefs, Length(FieldDefs) + 1);
 // Заполнение полей
 with FieldDefs[Length(FieldDefs) - 1] do begin
  FieldName := Trim(UpperCase(AFieldName));
  FieldType := AFieldType;
  DefVal    := ADefVal;
 end;
 FStaticSize := FStaticSize + GetStaticSizeByType(AFieldType);
 Result := true;
end;

function TStr2Bin.CalkDataSize: dword;
begin
 Result := FStaticSize + CalkDynamicDataSize;
end;

function TStr2Bin.CalkDynamicDataSize: integer;
var
 i : integer;
begin
 Result := 0;
 try
  for i := 0 to Length(FieldDefs) - 1 do
   case FieldDefs[i].FieldType of
    ftPWChar : Result := Result + Length(String(FieldDefs[i].Value)) * 2 + 2;
   end;
 except
 end;
end;

procedure TStr2Bin.CalkStaticDataSize;
var
 i : integer;
begin
 // Учтем размер служебного поля, хранящего ссылку на N+1 элемент
 FStaticSize := 4;
 // Вычисление суммарного размера полей
 for i := 0 to Length(FieldDefs) - 1 do
  FStaticSize := FStaticSize + GetStaticSizeByType(FieldDefs[i].FieldType);
end;

function TStr2Bin.ClearFieldDefs: boolean;
begin
 // Очистка списка полей
 FieldDefs := nil;
 // Учтем размер служебного поля, хранящего ссылку на N+1 элемент
 FStaticSize := 4;
end;

constructor TStr2Bin.Create;
begin
 ClearFieldDefs;
end;

// Парсинг строки с данными и заполнение полей
function TStr2Bin.GetFieldValue(const FieldName: string): Variant;
begin
 ;;
end;

function TStr2Bin.ParseString(AStr: string; ADivStr : string): boolean;
var
 i : integer;
begin
 Result := false;
 // Очистка полей (в них заносятся дефолтные значения)
 SetDefFieldsVal;
 AStr := AStr + ADivStr;
 // Цикл парсинга
 for i := 0 to Length(FieldDefs) - 1 do begin
 end;
 Result := true;
end;

// Установка всем полям значений по умолчанию
function TStr2Bin.SetDefFieldsVal: boolean;
var
 i : integer;
begin
 for i := 0 to Length(FieldDefs) - 1 do
  FieldDefs[i].Value := FieldDefs[i].DefVal;
 Result := true;
end;

procedure TStr2Bin.SetFieldValue(const AFieldName: string;
  const Value: Variant);
var
 i : integer;
 FieldName : string;
begin
 FieldName := Trim(UpperCase(AFieldName));
 for i := 0 to Length(FieldDefs) - 1 do
  if FieldName = FieldDefs[i].FieldName then begin
   FieldDefs[i].Value := Value;
   exit;
  end;
end;

end.
