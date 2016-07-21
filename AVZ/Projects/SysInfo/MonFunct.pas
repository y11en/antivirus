unit MonFunct;

interface
uses Types, Windows;

const
 // Криптографический ключ (шифрование симметричное)
 CriptKey1 : array[0..16] of byte = ($53, $7F, $22, $FE, $0D, $79, $31, $24, $04, $9B, $15, $83, $D2, $6F, $1D, $B0, $4A);
 (* ****** Структура сетевого пакета ***
  0     Z \
  1     M  > Сигнатура для опознания пакетов
  2     A /
  3     Длина данных (первый байт)
  4     Длина данных (второй байт)
  5     Контрольная сумма
  6     Контрольная сумма
  7     Контрольная сумма
  8     Контрольная сумма
  9     Битовые флаги, управляют алгоритмом шифрования (задействуют доп. операции)
  A     Резерв
  B     Резерв
  C     Дин. ключ (4 байта, генерируется передающей стороной для повышения стойкости к анализу)
  D     Дин. ключ
  E     Дин. ключ
  F     Дин. ключ
  10 ... Данные
  ------------
  Битовые флаги:
   0    Кол-во бит, на которые следует повернуть биты числа (0-нет поворота)
   1    Кол-во бит, на которые следует повернуть биты числа (0-нет поворота)
   2    Кол-во бит, на которые следует повернуть биты числа (0-нет поворота)
   3    Кол-во бит, на которые следует повернуть биты числа (0-нет поворота)
   4
   5
   6
   7    Инверсия байта
 *)
type
 TMonitoringNETBuffer = packed array of char;
 // Декодировать буфер
 Function DecodeBuffer(var Buf  : TMonitoringNETBuffer) : boolean;
 // Кодировать буфер
 Function EncodeBuffer(var Buf  : TMonitoringNETBuffer) : boolean;
 // Подготовить буффер для передачи
 Function PrepareBufferToSend(S : String; var Buf  : TMonitoringNETBuffer) : boolean;
implementation

// Декодировать буфер
Function DecodeBuffer(var Buf  : TMonitoringNETBuffer) : boolean;
var
 i : integer;
 CurByte : byte;
 RollKey : byte;
begin
 for i := $10 to Length(Buf)-1 do begin
  CurByte := ord(Buf[i]);
  // Операция XOR с ключом
  CurByte := CurByte xor byte(buf[12 + i mod 4]);
  // Операция инверсии
  if byte(Buf[9]) and $80 > 0 then
   CurByte := not CurByte;
  // Операция поворота битов
  RollKey := byte(Buf[9]) and $07;
  if RollKey > 0 then
   asm
    mov cl, RollKey
    rol CurByte, cl;
   end;
  // Используем главный ключ
  CurByte := CurByte xor CriptKey1[i mod $0F];
  // Запоминание результата
  Buf[i]  := Chr(CurByte);
 end;
end;

// Кодировать буфер
Function EncodeBuffer(var Buf  : TMonitoringNETBuffer) : boolean;
var
 i : integer;
 CurByte : byte;
 RollKey : byte;
begin
 for i := $10 to Length(Buf)-1 do begin
  CurByte := ord(Buf[i]);
  // Используем главный ключ
  CurByte := CurByte xor CriptKey1[i mod $0F];
  // Операция поворота битов
  RollKey := byte(Buf[9]) and $07;
  if RollKey > 0 then
   asm
    mov cl, RollKey
    ror CurByte, cl;
   end;
  // Операция инверсии
  if byte(Buf[9]) and $80 > 0 then
   CurByte := not CurByte;
  // Операция XOR с ключом
  CurByte := CurByte xor byte(buf[12 + i mod 4]);
  // Запоминание результата
  Buf[i]  := Chr(CurByte);
 end;
end;

// Подготовить буффер для передачи
Function PrepareBufferToSend(S : String; var Buf  : TMonitoringNETBuffer) : boolean;
var
 i : integer;
 CRC : DWORD;
 CRC_bytes : packed array[1..4] of byte absolute CRC;
 DataSize, ResLength : word;
 DataSize_bytes : packed array[1..2] of byte absolute DataSize;
begin
 // Выделение памяти под буфер
 SetLength(Buf, Length(S) + $10);
 // Очистка буфера
 ZeroMemory(@Buf[0], Length(Buf));
 // Запись сигнатуры
 Buf[0] := 'Z'; Buf[1] := 'M'; Buf[2] := 'A';
 // Запись данных
 for i := 1 to length(s) do
  buf[i+$0F] := s[i];
 // Размер данных
 DataSize   := length(s);
 ResLength  := DataSize+$10;
 Buf[3] := chr(DataSize_Bytes[1]);
 Buf[4] := chr(DataSize_Bytes[2]);
 // Создание кода шифрования (код состоит из ключа и дин. случайного ключа)
 Randomize;
 Buf[9] := chr(byte(random(255)));
 Buf[9] := char(3);
 Buf[12] := chr(byte(random(255)));
 Buf[13] := chr(byte(random(255)));
 Buf[14] := chr(byte(random(255)));
 Buf[15] := chr(byte(random(255)));
 // Шифрование
 MonFunct.EncodeBuffer(Buf);
 // Расчет CRC
 CRC := 0;
 for i := 0 to ResLength-1 do
  CRC := CRC + i*ord(buf[i]);
 // Запись CRC
 for i := 1 to 4 do
  buf[i+4] := chr(CRC_bytes[i]);
 Result := true;
end;

end.
