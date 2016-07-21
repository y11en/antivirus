unit zStringCompressor;

interface
 Function CompressString(S : string) : string;
 Function DeCompressString(S : string) : string;
implementation
uses Math, SysUtils;
{$I zStringCompressor.dct}

// Возвращает true, если строка S начинается с образца AObr (сравнение побайтное, регистрочувствительное)
Function StrStartFrom(AObr, S : string) : boolean;
var
 i : integer;
begin
 Result := false;
 if Length(S) < Length(AObr) then exit;
 for i := 1 to Length(AObr) do
  if AObr[i] <> S[i] then exit;
 Result := true;
end;

// Возвращает true, если строка S начинается с корретного CLSID
function StrStartFromCLSID(S : string) : boolean;
var
 i : integer;
begin
 Result := false;
 if Length(S) < 36 then exit;
 if (s[9] <> '-') or (s[14] <> '-') or (s[19] <> '-') or (s[24] <> '-') then exit;
 for i := 1 to 36 do
  if pos(s[i], '0123456789ABCDEFabcdef-') = 0 then exit;
 Result := true;
end;

// Возвращает строку длиной 16 байт, содержащую бинарное представление CLSID
function StrToBinCLSID(S : string) : string;
var
 Tmp : TGUID;
begin
 Result := '';
 if Length(S) < 36 then exit;
 S := '{'+copy(S, 1, 36)+'}';
 Tmp := StringToGUID(S);
 SetLength(Result, 16);
 Move(Tmp, Result[1], 16);
end;

// Возвращает строку длиной 16 байт, содержащую бинарное представление CLSID
function BinCLSIDToCLSIDStr(S : string) : string;
var
 Tmp : TGUID;
begin
 Result := '';
 if Length(S) < 16 then exit;
 Move(S[1], Tmp, 16);
 Result := copy(GUIDToString(Tmp), 2, 36);
end;

Function StrCompareCount(AObr, S : string) : integer;
var
 i : integer;
begin
 Result := 0;
 for i := 1 to min(Length(AObr), length(S)) do
  if AObr[i] <> S[i] then exit else Result := i;
end;

Function CompressString(S : string) : string;
var
 i, CompressIndx, CompressLength  : integer;
begin
 Result := '';
 {R-}
 while Length(S)>0 do begin
  // Сжатие переводов строки
  if copy(S, 1, 2) = #$0D#$0A then begin
   Result := Result + #2;
   Delete(S,1,2);
   Continue;
  end;
  // Сжатие ('
  if copy(S, 1, 2) = '(''' then begin
   Result := Result + #3;
   Delete(S,1,2);
   Continue;
  end;
  // Сжатие ')
  if copy(S, 1, 2) = ''')' then begin
   Result := Result + #4;
   Delete(S,1,2);
   Continue;
  end;
  CompressIndx   := -1;
  CompressLength := 0;
  // Поиск о статическому словарю на предмет полного совпадения
  for i := 0 to High(CompressArray) do
   if (length(CompressArray[i]) > CompressLength) and StrStartFrom(CompressArray[i], S) then begin
    CompressIndx   := i;
    CompressLength := length(CompressArray[i]);
   end;
   // Вхождений нет
   if CompressIndx = -1 then begin
    // Поиск о статическому словарю на предмет частичного совпадения
    CompressIndx   := -1;
    CompressLength := 0;
    for i := 0 to High(CompressArray) do
     if (length(CompressArray[i]) > CompressLength) and (StrCompareCount(CompressArray[i], S) > CompressLength) then begin
      CompressIndx   := i;
      CompressLength := StrCompareCount(CompressArray[i], S);
     end;
    // Частичного совпадения нет или оно слишком маленькое
    if (CompressIndx = -1) or (CompressLength < 3) then begin
     if StrStartFromCLSID(S) then begin
      Result := Result + #06+ StrToBinCLSID(S);
      Delete(S, 1, 36);
     end else begin
      Result := Result + copy(S, 1, 1);
      Delete(S,1,1);
     end;
    end else begin
     // Частичное совпадение есть - записываем код
     Result := Result + #05+chr(CompressIndx)+chr(CompressLength);
     Delete(S,1,CompressLength);
    end;
   end else begin
    Result := Result + #01+chr(CompressIndx);
    Delete(S,1,CompressLength);
   end;
 end;
 {R+}
end;

Function DeCompressString(S : string) : string;
begin
 Result := '';
 while Length(S)>0 do begin
  case byte(s[1]) of
   1 : begin
        Result := Result + CompressArray[byte(s[2])];
        Delete(S, 1, 2);
       end;
   2 : begin
        Result := Result + #$0D#$0A;
        Delete(S, 1, 1);
       end;
   3 : begin
        Result := Result + '(''';
        Delete(S, 1, 1);
       end;
   4 : begin
        Result := Result + ''')';
        Delete(S, 1, 1);
       end;
   5 : begin
        Result := Result + copy(CompressArray[byte(s[2])], 1, byte(s[3]));
        Delete(S, 1, 3);
       end;
   6 : begin
        Result := Result + BinCLSIDToCLSIDStr(copy(S, 2, 16));
        Delete(S, 1, 1 + 16);
       end;
   else begin
       Result := Result + s[1];
       Delete(S, 1, 1);
      end;
  end;
 end;
end;

end.
