program avz_db_compile;
// Утилита для сборки баз данных.
// Зайцев О.В., последняя модификация 20.09.2007
{$APPTYPE CONSOLE}

uses
  SysUtils, Classes, Windows,
  zAntivirus,
  zStringCompressor,
  zutil;

var
 LogFileName : string = '';  // Имя файла для хранения протокола
 Params      : TStringList;  // Список параметров командной строки
 DBFile      : string;       // Имя файла базы данных
 DBSrcPath   : string;       // Путь к папке с исходниками
 RusLangFile : string;       // путь к языковому файлу с русским языком
 ScrFiles    : TStringList;  // Список скриптов, добавляемых в базу

// Добавить строку к протооку в текстовом файле
Function AddToFileLog(s:string):boolean;
var
 f     : TextFile;
begin
 Result := false;
 if LogFileName = '' then exit;
 AssignFile(f, LogFileName);
 try
  // Если файл существует - дозапись, не существует - создание
  if FileExists(LogFileName) then Append(f)
   else Rewrite(f);
 except
  exit;
 end;
 try
  Writeln(f, DateTimetoStr(Now)+' : '+s); // Вывод строки
 finally
  CloseFile(f); // в конце обязательно закроем файл
 end;
end;

// Вывод строки в протокол и на консоль
procedure AddToLog(S  :string);
begin
 writeln(S);
 AddToFileLog(S);
end;

// Парсинг параметров командной строки и построение списка параметов
procedure ParseParams;
var
 i : integer;
begin
 Params.Clear;
 // Построение списка параметров
 for i := 1 to ParamCount do
  if pos('=', ParamStr(i)) > 0 then
   Params.Add(ParamStr(i));
 // Настройка приложения
 if Params.Values['LogFile'] <> '' then
  LogFileName := Params.Values['LogFile'];
end;

// Загрузка профиля настроек
function LoadProfile(AProfileName : string) : boolean;
var
 INI : TStringList;
begin
 DBFile := ''; DBSrcPath   := '';  RusLangFile := '';
 Result := false;
 // Имя файла профиля отсусттвует
 if AProfileName = '' then exit;
 // Файл не найден
 if not(FileExists(AProfileName)) then exit;
 // загрузка профиля
 INI := TStringList.Create;
 INI.LoadFromFile(AProfileName);
 // загрузка настроек
 DBFile      := Trim(INI.Values['DBFile']);
 DBSrcPath   := Trim(INI.Values['DBSrcPath']);
 DBSrcPath   := NormalDir(DBSrcPath);
 RusLangFile := Trim(INI.Values['RusLangFile']);
 LogFileName := Trim(INI.Values['LogFile']);
 Result := true;
 INI.Free;
end;

// Логический контроль профиля настроек
function CheckProfile : boolean;
begin
 Result := false;
 // Контроль папки с путем к базе
 if not(DirectoryExists(DBSrcPath)) then begin
  AddToLog('DB folder not found. Path="'+DBSrcPath+'"');
  exit;
 end;
 // Контроль наличия файла с локализацией
 if not(FileExists(RusLangFile)) then begin
  AddToLog('Russian lang file not found. File name="'+RusLangFile+'"');
  exit;
 end;
 Result := true;
end;

// Создание списка файлов, помещаемых в базу
function CreateScriptList : boolean;
var
 SR  : TSearchRec;
 Res : integer;
begin
 Res := FindFirst(DBSrcPath + '*.txt', faAnyFile, SR);
 ScrFiles.Clear;
 while Res = 0 do begin
  // Добавление имени найденного файла
  ScrFiles.Add(SR.Name);
  Res := FindNext(SR);
 end;
 SysUtils.FindClose(SR);
 // Сортировка списка
 ScrFiles.Sort;
end;

// Нормализация скрипта - удаление из него всех пустых строк
function NormalizeScriptSrc(AScript : TStrings) : boolean;
var
 i : integer;
begin
 Result := false;
 i := 1; // Первую строку (ее индекс 0) не трогаем - это заголовок скрипта
 while i < AScript.Count do begin
  // Удаление пустых строк
  if Trim(AScript[i]) = '' then begin
   AScript.Delete(i);
   Continue;
  end;
  // Удаление комментариев
  if copy(Trim(AScript[i]), 1, 2) = '//' then begin
   AScript.Delete(i);
   Continue;
  end;
  inc(i);
 end;
 Result := true;
end;

// Проверка скрипта
function CheckScript(AScript : TStrings) : boolean;
var
 i : integer;
begin
 Result := false;
 // Скрипт должне содержать минимум три строки (комментарий с именем, begin и end.
 if AScript.Count < 3 then begin
  AddToLog('>> Error: empty script = '+ScrFiles[i]);
  exit;
 end;
 Result := true;
end;

// Извлечение имени скрипта
function ExtractScriptName(AScript : TStrings) : string;
begin
 Result := '';
 // Скрипт должне содержать минимум три строки (комментарий с именем, begin и end.
 if AScript.Count = 0 then exit;
 Result := Trim(AScript[0]);
 if pos('//', Result) = 1 then
  Delete(Result, 1, 2)
   else begin
    Result := '';
    exit;
   end;
 AScript.Delete(0);
end;

// Получение номера скрипта из имени
function GetScriptNum(AFileName : string) : word;
begin
 Result := $FFFF;
 AFileName := Trim(UpperCase(AFileName));
 // Удаление расширения файла
 if pos('.', AFileName) > 0 then
  Delete(AFileName, pos('.', AFileName), maxint);
 AFileName := Trim(AFileName);
 // Удаление нецифровых символов
 while (Length(AFileName) > 0) and (pos(AFileName[1], '0123456789')=0) do
  Delete(AFileName, 1, 1);
 Result := StrToIntDef(AFileName, $FFFF);
end;

// Создение бинарного AVZ файла
function CreateBinScriptsDatabase: boolean;
var
 MS    : TMemoryStream;             // Поток для работы с выходным файлом
 i     : integer;
 S, ScriptName : string;
 buf_w  : word;
 BaseHeader : TVirusBaseHeader;     // Заголовок базы
 RecCount   : integer;              // Кол-во записей
 ScriptText : TStringList;          // Исходный текст скрипта
begin
 // Сигнатура файла
 BaseHeader.Sign[1] := 'A';
 BaseHeader.Sign[2] := 'V';
 BaseHeader.Sign[3] := 'Z';
 // Версия
 BaseHeader.MinAvzVersion := 4.27;
 // Дата сборки
 BaseHeader.DateTime      := Now;
 Result := false;
 RecCount := 0;
 ScriptText := TStringList.Create;
 try
   // Создание потока для результирующих данных
   MS    := TMemoryStream.Create;
   MS.Write(BaseHeader, sizeof(BaseHeader));
   // Цикл обработки
   for i :=0 to ScrFiles.Count - 1 do begin
      // Загрузка исходника скрипта
      ScriptText.LoadFromFile(DBSrcPath + ScrFiles[i]);
      // Нормализация скрипта
      NormalizeScriptSrc(ScriptText);
      // Проверка скрипта
      if not(CheckScript(ScriptText)) then begin
       AddToLog('>>> Invalid script = '+ScrFiles[i]);
       exit;
      end;
      // Извлечение имени скрипта
      ScriptName := ExtractScriptName(ScriptText);
      if ScriptName = '' then begin
       AddToLog('>>> Script name not found = '+ScrFiles[i]);
       exit;
      end;
      // Сохранение номера скрипта
      buf_w := GetScriptNum(ScrFiles[i]);
      MS.Write(buf_w, 2);
      // Сохранение имени скрипта
      S := CompressString(ScriptName);
      buf_w := length(S);
      MS.Write(buf_w, 2);
      MS.Write(S[1], buf_w);
      // Сохранение скрипта
      S := CompressString(Trim(ScriptText.Text));
      buf_w := length(S);
      MS.Write(buf_w, 2);
      MS.Write(S[1], buf_w);
      // Протокол
      AddToLog('Script add to DB: '+ScrFiles[i]);
      inc(RecCount);
   end;
   // Заполнение полей заголовка
   BaseHeader.RecCount := RecCount;
   // Генерация случайного числа, участвующего в шифровке файла
   Randomize;
   // Занесение числа в базу
   BaseHeader.code_num := Random(254)+1;
   // Указатель на слудующий независимый блок отсуствует - в базе идинственный блок данных
   BaseHeader.NextHeaderPtr := 0;
   // Запись заголовка в начало файла
   MS.Seek(0,0);
   MS.Write(BaseHeader, sizeof(BaseHeader));
   // Процесс шифровки (код 4 - номер ключа шифровки для ЛК) и вычисления CRC
   CodeBinData(MS, 4);
   MS.Seek(0,0);
   // Сохранение потока в файл
   MS.SaveToFile(DBFile);
   Result := true;
   MS.Free;
 finally
  ScriptText.free;
  AddToLog('DB creation complete. ScriptCount = '+IntToStr(RecCount))
 end;
end;

begin
 AddToLog('AVZ Script DB compilator. v1.01');
 Params := TStringList.Create;
 // Парсинг параметров
 ParseParams;
 // Если параметров нет, то выход
 if Params.Count = 0 then begin
  AddToLog('Params not found. Run avz_db_compile.exe profile=<full profile file name>');
  exit;
 end;
 // Загрузка профиля настроек
 if not(LoadProfile(Params.Values['Profile'])) then begin
  AddToLog('Profile load error');
  exit;
 end;
 // Контроль параметров, находящизся в профиле
 if not(CheckProfile) then begin
  AddToLog('Profile data error');
  exit;
 end;
 // Создание списка файлов, помещаемых в базу
 ScrFiles  := TStringList.Create;
 // Создание списка скриптов
 CreateScriptList;
 // Проверка, найден ли хоть один скрипт
 if ScrFiles.Count = 0 then begin
  AddToLog('Scripts not found ! DB path = '+DBSrcPath);
  exit;
 end;
 AddToLog('AVZ Script DB compilator. v1.01 - Compilation started ...');
 // Создание базы данных формата AVZ
 if CreateBinScriptsDatabase then
  AddToLog('Compilation sucessefully complete')
   else AddToLog('Compilation ERROR !!');
end.

