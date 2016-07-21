unit DbgOutp;
{****************************************************************
 * Модуль отладочного вывода (С) Зайцев О.В., АСУ тел. 0-48     * 
 ****************************************************************}
interface
 // Добавить строку к отладочному дампу
 Function DebugOutput(s:string):boolean;

var
 DebugMode : boolean = false;

implementation
uses Forms,Sysutils;

// Добавить строку к отладочному дампу
Function DebugOutput(s:string):boolean;
var
 f     : TextFile;
 FName : String;
begin
 // Если режим отладки выключен, то выход
 if Not(DebugMode) then
  exit;
 Result := false;
 FName := ChangeFileEXT(Application.ExeName,'.dbg');
 AssignFile(f, FName);
 try
  // Если файл существует - дозапись, не существует - создание
  if FileExists(FName) then Append(f)
   else Rewrite(f);
 except
  exit; // При ошибках немедленный выход, дабы не усугублять положение программы
 end;
 try
  Writeln(f, DateTimetoStr(Now)+' : '+s);
 finally
  CloseFile(f); // в конце обязательно закроем файл
 end;
end;

end.
