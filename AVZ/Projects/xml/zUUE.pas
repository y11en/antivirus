unit ZUUE;
// ***************************************************************************
// ******** Библиотека классов для работы с MIME UUE кодированием ********
// ********************** (C) Зайцев О.В.,2002 г. ****************************
// ***************************************************************************
interface
uses  classes, Sysutils, messages, dialogs, types;

 type
  // Исключение для ошибок кодирования и декодирования
  EUUEError = class(Exception)
  end;

  // 3-х и 4-х байтные структуры для операций кодирования
  TByte3 = packed array[0..2] of byte;
  PByte3 = ^TByte3;
  TByte4 = packed array[0..3] of byte;
  PByte4 = ^TByte4;

  // Преобразование 3 байта --> 4 символа по кодировке Base64
  Procedure InternalCodeUUE(AByte3 : TByte3; var AByte4 : TByte4);
  // Преобразование 4 символа Base64 --> 3 нормальных байта
  Procedure InternalDeCodeUUE(AByte4 : TByte4; var AByte3 : TByte3);

  // ***** Декодирование строки *****
  // Расширенное декодирование - учитывает все возможные некорректности и особенности
  Function DeCodeStringUUEEx(AInBuffer : AnsiString) : AnsiString;

  // ***** Вычисление размеров буферов *****
  // Размер декодированного буфера (это максимальный размер, реальный может быть меньше из-за "=" в хвосте, переводов строки и т.п.)
  function UUEDecodedSize(const InputSize: Cardinal): Cardinal;
  // Размер закодированного буфера (правильно считается только в случае кодирования без вставки переводов строки)
  function UUEEncodedSize(const InputSize: Cardinal): Cardinal;
implementation

function UUEEncodedSize(const InputSize: Cardinal): Cardinal;
begin
  Result := (InputSize + 2) div 3 * 4;
end;

function UUEDecodedSize(const InputSize: Cardinal): Cardinal;
begin
  Result := (InputSize + 3) div 4 * 3 + (InputSize + 3) div 40;
end;

// Преобразование 3 байта --> 4 символа по кодировке Base64
Procedure InternalCodeUUE(AByte3 : TByte3; var AByte4 : TByte4);
begin
end;

Function DeCodeByteUUE(AIn : byte) : byte;
begin
 if AIn  =$60 then Result :=0 else
  Result := AIn - $20;
end;

// Преобразование 4 символа Base64 --> 3 нормальных байта
Procedure InternalDeCodeUUE(AByte4 : TByte4; var AByte3 : TByte3);
begin
 {$R-}
 AByte3[2] := DeCodeByteUUE(AByte4[3]) + (DeCodeByteUUE(AByte4[2]) and $3) shl 6;
 AByte3[1] := DeCodeByteUUE(AByte4[2]) shr 2 + (DeCodeByteUUE(AByte4[1]) and $0F) shl 4;
 AByte3[0] := DeCodeByteUUE(AByte4[1]) shr 4 + (DeCodeByteUUE(AByte4[0]) shl 2);
 {$R+}
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
 SetLength(Result, UUEEncodedSize(length(AInBuffer)));
 // Кодирование
// CodeBufferBase64(AInBuffer[1], length(AInBuffer), Result[1]);
end;

//
Function DeCodeStringUUEEx(AInBuffer : AnsiString) : AnsiString;
var
 Poz,  EndPoz, Size,
 OutPoz, LineOutPoz       : Cardinal;
 Byte4        : TByte4;
 Byte4dword   : dword absolute Byte4;
 Byte4Poz     : byte;
 LineLen      : integer;
 LineEnd      : integer;
 i            : integer;
begin
 {$R-}{$I-}
 Result := '';
 Poz := pos('begin', AInBuffer);
 if Poz = 0 then exit;
 // Грубое задание размера буфера
 SetLength(Result, UUEDecodedSize(length(AInBuffer)));
 // Подготовка адресов и указателей
 poz    := Cardinal(@AInBuffer[1]) + poz + 5;
 OutPoz := Cardinal(@Result[1]);
 Size   := Length(AInBuffer);
 EndPoz := Poz + Size;
 Byte4Poz := 0;
 // Цикл декодирования
 while Poz < EndPoz do begin
  // Пропуск всех символов кроме перевода строки
  if pbyte(pointer(Poz))^ <> $0D then begin inc(Poz); Continue; end;
  inc(Poz); // Пропуск символа 0D
  if pbyte(pointer(Poz))^ =  $0A then begin
   inc(Poz); // Пропуск символа 0A
   if Poz < EndPoz then begin
    // Это "End" ??
    if (pbyte(pointer(Poz))^ in [byte('e'), byte('E')]) and (pbyte(pointer(Poz+1))^ = byte('n')) then  break;
    // Читаем первый байт - это длинна строки
    LineLen := DeCodeByteUUE(pbyte(pointer(Poz))^);
    inc(Poz);
    if LineLen = 0 then  continue;
    LineEnd :=  LineLen  div 3 * 4 + Poz;
    Byte4Poz := 0;
    LineOutPoz := 0;
    while LineOutPoz < LineLen do begin
     Byte4[Byte4Poz] := pbyte(pointer(Poz))^;
     inc(Poz);
     inc(Byte4Poz);
     if Byte4Poz = 4 then begin
      InternalDeCodeUUE(Byte4, PByte3(pointer(OutPoz))^);
      inc(OutPoz, 3);
      inc(LineOutPoz, 3);
      Byte4Poz := 0;
     end;
    end;
    if LineOutPoz > LineLen then
     OutPoz := OutPoz - (LineOutPoz - LineLen);
    if (Byte4Poz > 0) and (Byte4Poz <> 4) then begin
     for i := Byte4Poz to 3 do
      Byte4[i] := 0;
     InternalDeCodeUUE(Byte4, PByte3(pointer(OutPoz))^);
     inc(OutPoz, LineLen mod 3);
     Byte4Poz := 0;
    end;
   end;
  end;
 end;
 SetLength(Result, OutPoz - Cardinal(@Result[1]));
 {$R+}{$I+}
end;

end.


