unit ZBase64;
// ***************************************************************************
// ******** Библиотека классов для работы с MIME base 64 кодированием ********
// ********************** (C) Зайцев О.В.,2002 г. ****************************
// ***************************************************************************
interface
 uses  classes, Sysutils, messages, dialogs, types;
 const
   // Массив для кодирования по Base64
   CodesBase64   : array[0..63]  of Char = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
   Base64LineSize = 76;
   // Массив для декодирования по Base64 и распознания допустимых символов (у недопустимых коды 255)
   ReCodesBase64 : array[0..$FF] of byte = (
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 062, 255, 255, 255, 063,
     052, 053, 054, 055, 056, 057, 058, 059, 060, 061, 255, 255, 255, 255, 255, 255,
     255, 000, 001, 002, 003, 004, 005, 006, 007, 008, 009, 010, 011, 012, 013, 014,
     015, 016, 017, 018, 019, 020, 021, 022, 023, 024, 025, 255, 255, 255, 255, 255,
     255, 026, 027, 028, 029, 030, 031, 032, 033, 034, 035, 036, 037, 038, 039, 040,
     041, 042, 043, 044, 045, 046, 047, 048, 049, 050, 051, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255);

 type
  // Исключение для ошибок кодирования и декодирования
  EBase64Error = class(Exception)
  end;

  // 3-х и 4-х байтные структуры для операций кодирования
  TByte3 = packed array[0..2] of byte;
  PByte3 = ^TByte3;
  TByte4 = packed array[0..3] of byte;
  PByte4 = ^TByte4;

  // Кодирование и декодирование BASE64 информации
  TBase64 = class
  end;
  // Преобразование 3 байта --> 4 символа по кодировке Base64
  Procedure InternalCodeBase64(AByte3 : TByte3; var AByte4 : TByte4);
  // Преобразование 4 символа Base64 --> 3 нормальных байта
  Procedure InternalDeCodeBase64(AByte4 : TByte4; var AByte3 : TByte3);

  // Кодирование буффера по Base64. Возврат - реально задействованный размер буфера
  Function CodeBufferBase64(const AInBuffer; ACount: Cardinal; var AOutBuffer) : Cardinal;
  // Кодирование строки по Base64
  Function CodeStringBase64(const AInBuffer : AnsiString; CRLFfl : boolean = false) : AnsiString;

  // ***** Декодирование строки *****
  // Простое декодирование - в AInBuffer строка в Base64 коде без переводов строки и лишних символов
  Function DeCodeStringBase64(AInBuffer : AnsiString) : AnsiString;
  // Расширенное декодирование - учитывает все возможные некорректности и особенности
  Function DeCodeStringBase64Ex(AInBuffer : AnsiString) : AnsiString;

  // ***** Вычисление размеров буферов *****
  // Размер декодированного буфера (это максимальный размер, реальный может быть меньше из-за "=" в хвосте, переводов строки и т.п.)
  function Base64DecodedSize(const InputSize: Cardinal): Cardinal;
  // Размер закодированного буфера (правильно считается только в случае кодирования без вставки переводов строки)
  function Base64EncodedSize(const InputSize: Cardinal): Cardinal;
implementation

function Base64EncodedSize(const InputSize: Cardinal): Cardinal;
begin
  Result := (InputSize + 2) div 3 * 4;
end;

function Base64DecodedSize(const InputSize: Cardinal): Cardinal;
begin
  Result := (InputSize + 3) div 4 * 3;
end;

// Преобразование 3 байта --> 4 символа по кодировке Base64
Procedure InternalCodeBase64(AByte3 : TByte3; var AByte4 : TByte4);
begin
 AByte4[0] := byte(CodesBase64[(AByte3[0] SHR 2)]);
 AByte4[1] := byte(CodesBase64[((AByte3[0] SHL 4) AND $30) + ((AByte3[1] SHR 4) AND $0F)]);
 AByte4[2] := byte(CodesBase64[((AByte3[1] SHL 2) AND $3C) + ((AByte3[2] SHR 6) AND $03)]);
 AByte4[3] := byte(CodesBase64[(AByte3[2] AND $3F)]);
end;

// Преобразование 4 символа Base64 --> 3 нормальных байта
Procedure InternalDeCodeBase64(AByte4 : TByte4; var AByte3 : TByte3);
begin
 {$R-}
 AByte3[2] := ReCodesBase64[AByte4[3]] + (ReCodesBase64[AByte4[2]] and $3) shl 6;
 AByte3[1] := ReCodesBase64[AByte4[2]] shr 2 + (ReCodesBase64[AByte4[1]] and $0F) shl 4;
 AByte3[0] := ReCodesBase64[AByte4[1]] shr 4 + (ReCodesBase64[AByte4[0]] shl 2);
 {$R+}
end;

// Преобразование массива данных в Base64
Function CodeBufferBase64(const AInBuffer; ACount: Cardinal; var AOutBuffer) : Cardinal;
var
 Byte3        : TByte3;
 Byte4        : TByte4;
 Poz, Size    : Cardinal;
 OutPoz       : Cardinal;
begin
 Result := 0;
 Poz := 0; OutPoz := 0;
 while Poz < ACount do begin
  Size  := ACount - Poz;
  Byte3 := PByte3( Pointer(Cardinal(@AInBuffer) + Poz) )^;
  case Size of
   1 : begin
        Poz := Poz + 1;
        Byte3[1] := 0;
        Byte3[2] := 0;
       end;
   2 : begin
        Poz := Poz + 2;
        Byte3[2] := 0;
       end;
   else
     Poz := Poz + 3;
   end;
  InternalCodeBase64(Byte3, PByte4(Pointer(Cardinal(@AOutBuffer) + OutPoz))^);
  inc(OutPoz, 4);
 end;
 Result := OutPoz;
 case Size of
  1 : begin
       PByte4(Pointer(Cardinal(@AOutBuffer) + OutPoz-4))^[3] := byte('=');
       PByte4(Pointer(Cardinal(@AOutBuffer) + OutPoz-4))^[2] := byte('=');
      end;
  2 : begin
       PByte4(Pointer(Cardinal(@AOutBuffer) + OutPoz-4))^[3] := byte('=');
      end;
 end;
end;


// Преобразование массива
Function CodeStringBase64(const AInBuffer : AnsiString; CRLFfl : boolean = false) : AnsiString;
begin
 // Кодирование пустой строки
 if AInBuffer = '' then begin
  Result := '';
  exit;
 end;
 // Настройка длинны строки
 SetLength(Result, Base64EncodedSize(length(AInBuffer)));
 // Кодирование
 CodeBufferBase64(AInBuffer[1], length(AInBuffer), Result[1]);
end;

//
Function DeCodeStringBase64(AInBuffer : AnsiString) : AnsiString;
var
 Poz, Size,
 OutPoz       : Cardinal;
 i            : integer;
begin
 {$R-}{$I-}
 Result := '';
 if Length(AInBuffer) = 0 then exit;
 // Дополнение строки
 if Length(AInBuffer) mod 4 <> 0 then
  AInBuffer := AInBuffer + copy('====', 1,  4 - Length(AInBuffer) mod 4);
 // Настройка размера буфера (грубая - с запасом, т.к. не известно заполнение последней четверки байт)
 SetLength(Result, Base64DecodedSize(length(AInBuffer)));
 poz    := Cardinal(@AInBuffer[1]);
 OutPoz := Cardinal(@Result[1]);
 Size := Length(AInBuffer);
 // Цикл декодирования
 for i := 0 to (Size div 4) - 1 do begin
  InternalDeCodeBase64(PByte4(pointer(Poz))^, PByte3(pointer(OutPoz))^);
  inc(Poz, 4);
  inc(OutPoz, 3);
 end;
 // Точное определение размера (если )
 if copy(AInBuffer, Size-1, 2) = '==' then
  SetLength(Result, length(Result) - 2)
 else if copy(AInBuffer, Size, 1) = '=' then
  SetLength(Result, length(Result) - 1);
 {$R+}{$I+}
end;

// Декодирование строки из BASE64
Function DeCodeStringBase64Ex(AInBuffer : AnsiString) : AnsiString;
var
 Poz,  EndPoz, Size,
 OutPoz       : Cardinal;
 Byte4        : TByte4;
 Byte4dword   : dword absolute Byte4;
 Byte4Poz     : byte;
begin
 {$R-}{$I-}
 Result := '';
 if Length(AInBuffer) = 0 then exit;
 // Настройка размера результирующего буфера (грубая - с запасом, т.к. не известно заполнение последней четверки байт и неизвестно кол-во переводов строки)
 SetLength(Result, Base64DecodedSize(length(AInBuffer)));
 // Подготовка адресов и указателей
 poz    := Cardinal(@AInBuffer[1]);
 OutPoz := Cardinal(@Result[1]);
 Size   := Length(AInBuffer);
 EndPoz := Poz + Size;
 Byte4Poz := 0;
 // Цикл декодирования
 while Poz < EndPoz do begin
  // Пропуск перевода строки, перевода каретки и символов, которых нет в алфавите base64 кода
  if ReCodesBase64[pbyte(pointer(Poz))^] = 255 then begin  inc(Poz);  continue; end
   else begin
    Byte4[Byte4Poz] := pbyte(pointer(Poz))^;
    inc(Poz);
    inc(Byte4Poz);
   end;
  if Byte4Poz = 4 then begin
   InternalDeCodeBase64(Byte4, PByte3(pointer(OutPoz))^);
   inc(OutPoz, 3);
   Byte4Poz := 0;
  end;
 end;
 // Обработка последней группы байт (такая ситуация возможна на некорректном файле, где число байт не кратно 4-м)
 if (Byte4Poz > 0) and (Byte4Poz < 4) then begin
  while Byte4Poz <> 4 do begin
    Byte4[Byte4Poz] := byte('=');
    inc(Byte4Poz);
  end;
  InternalDeCodeBase64(Byte4, PByte3(pointer(OutPoz))^);
  inc(OutPoz, 3);
 end;
 // Точное определение размера
 Size := OutPoz - Cardinal(@Result[1]);
 // Уточнение размера по символам "=" в хвосте закодированного текста
 if (Byte4[3] = byte('=')) and (Byte4[2] = byte('=')) then
  Size := Size - 2
 else if (Byte4[3] = byte('=')) then
  Size := Size - 1;
 SetLength(Result, Size)
 {$R+}{$I+}
end;

end.


