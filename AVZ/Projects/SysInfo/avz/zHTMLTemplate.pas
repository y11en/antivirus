unit zHTMLTemplate;
// Параметры HTML форматирования для генераторов протоколов
interface
uses Windows, Messages, SysUtils, Variants, Classes, Dialogs, ShellApi, Controls;
type
 TScriptCmdDescr = record
  Name    : string;  // название
  CmdText : string;  // Скрипт
  AddMode : integer; // Режим добавления
 end;
var
 HTML_StartLines     : TStringList;
 HTML_BodyLines      : TStringList;
 HTML_Scripts        : TStringList;
 HTML_TableFormat1   : string = 'cellSpacing=1 cellPadding=2 width="100%" bgColor=#ffffff border=0';//#DNL
 HTML_TableHeaderTD  : string = '<TD align=center><b><font color=White>'; //#DNL
 HTMP_TableHeader    : string = 'BgColor=#0000FF';//#DNL
 HTMP_BgColorBg      : string = '#ffc06d';//#DNL // Фон таблицы по умолчанию
 HTMP_BgColorTrusted : string = '#00CC66';//#DNL // Безопасный файл
 HTMP_BgColor2       : string = '#ffc06d';//#DNL // неизвестно
 HTMP_BgColor3       : string = '#FF7C80';//#DNL // зловред
 HTMP_BgColorSusp    : string = '#FFFF99';//#DNL // подозрительный
 HTMP_BgColorFt      : string = '#00CC66';//#DNL // итоги в таблице
 FScriptLogMode      : boolean = false;
threadvar
 ExtMenuScripts      : array of TScriptCmdDescr;

// Отображение HTML протокола. Если AUserAsk = true, то выдается запрос
function ShowHTMLLog(AFileName : string; AUserAsk : boolean) : boolean;
// Генерация ссылок для добавления скриптов по данному файлу
function GenScriptTxt(AFileName : string; ATerminate : boolean = false) : string;
// Генерация скрипта для BHO
function GenScriptTxtBHO(ACLSID : string) : string;
// Генерация скрипта для DPF
function GenScriptTxtDPF(ACLSID : string) : string;
// Генерация меню для автогенерации скрипта
function GenScriptMenu : string;
// Генерация скрипта для службы
function GenScriptSvcTxt(AServiceName : string; ADriverMode : boolean) : string;

implementation
uses zTranslate;

// Отображение HTML протокола. Если AUserAsk = true, то выдается запрос
function ShowHTMLLog(AFileName : string; AUserAsk : boolean) : boolean;
begin
 // Запрос
 if AUserAsk then
  if zMessageDlg('$AVZ0871', mtConfirmation, [mbYes, mbNo], 0) <> mrYes then exit;
 // Отображение HTML файла
 ShellExecute(0, 'opennew', PChar(AFileName),nil,nil,1); //#DNL
end;

// Генерация скрипта для файла
function GenScriptTxt(AFileName : string; ATerminate : boolean) : string;
begin
 Result := '';
 if not(FScriptLogMode) then exit;
 AFileName := Trim(StringReplace(AFileName, '\', '\\', [rfReplaceAll]));//#DNL
 AFileName := StringReplace(AFileName, '''', '', [rfReplaceAll]);//#DNL
 AFileName := StringReplace(AFileName, '"', '', [rfReplaceAll]);//#DNL
 if AFileName = '' then exit;
// Result := '<a href=''Javascript:add_scr_d("'+AFileName+'")''>Д</a><a href=''Javascript:add_scr_k("'+AFileName+'")''>K</a>';
 Result := '<br><font size=-2>'+'$AVZ0988:'+' <a href=''Javascript:add_scr_k("'+AFileName+'")''>'+'$AVZ0018'+'</a>, '+
           ' <a href=''Javascript:add_scr_d("'+AFileName+'")''>'+'$AVZ1084'+'</a>, '+
           ' <a href=''Javascript:add_scr_db("'+AFileName+'")''>'+'$AVZ1094'+'</a>';
 if ATerminate then
 Result := Result + ', '+
           ' <a href=''Javascript:add_scr_t("'+AFileName+'")''>'+'$AVZ1599'+'</a>';
 Result := TranslateStr(Result);
end;

// Генерация скрипта для BHO
function GenScriptTxtBHO(ACLSID : string) : string;
begin
 Result := '';
 if ACLSID = '' then exit;
 Result := '<br><font size=-2> <a href=''Javascript:add_scr_bho("'+ACLSID+'")''>'+'$AVZ1084'+'</a>';
 Result := TranslateStr(Result);
end;

// Генерация скрипта для DPF
function GenScriptTxtDPF(ACLSID : string) : string;
begin
 Result := '';
 if ACLSID = '' then exit;
 if (ACLSID = '\') or (ACLSID = '/') then exit;
 Result := '<br><font size=-2> <a href=''Javascript:add_scr_dpf("'+ACLSID+'")''>'+'$AVZ1084'+'</a>';
 Result := TranslateStr(Result);
end;

// Генерация скрипта для службы
function GenScriptSvcTxt(AServiceName : string; ADriverMode : boolean) : string;
begin
 Result := '';
 if not(FScriptLogMode) then exit;
 AServiceName := Trim(AServiceName);//#DNL
 if AServiceName = '' then exit;
 if not(ADriverMode) then
  Result := '<br><font size=-2>'+'$AVZ0998:'+
            ' <a href=''Javascript:add_scr_s1("'+AServiceName+'")''>'+'$AVZ1053'+'</a>, '+
            ' <a href=''Javascript:add_scr_s2("'+AServiceName+'")''>'+'$AVZ1084'+'</a>, '+
            ' <a href=''Javascript:add_scr_s3("'+AServiceName+'")''>'+'$AVZ1601'+'</a>'
 else
  Result := '<br><font size=-2>'+'$AVZ1603:'+
            ' <a href=''Javascript:add_scr_s1("'+AServiceName+'")''>'+'$AVZ1602'+'</a>, '+
            ' <a href=''Javascript:add_scr_s2("'+AServiceName+'")''>'+'$AVZ1084'+'</a>, '+
            ' <a href=''Javascript:add_scr_s3("'+AServiceName+'")''>'+'$AVZ1601'+'</a>';
 Result := TranslateStr(Result);
end;


function GenScriptMenu : string;
  // Добавление команды в скрипт
  procedure AddCmd(ACmd : string; PozCode : integer; Naim : string);
  begin
   Result := Result + '<li><a href=''Javascript:add_scr_line("'+ACmd+'", '+inttostr(PozCode)+')''>'+Naim+'</a><br>';
  end;
  procedure AddCmdQ(ACmd : string; PozCode : integer; Naim : string);
  var
   i : integer;
   S : string;
  begin
   S := '';
   ACmd := StringReplace(ACmd, '\', '\\', [rfReplaceAll]);
   for i := 1 to length(ACmd) do
    S := S + '%'+IntToHex(Byte(ACmd[i]), 2);
   Result := Result + '<li><a href=''Javascript:add_scr_line_q("'+S+'", '+inttostr(PozCode)+')''>'+TranslateStr(Naim)+'</a><br>';
  end;
var
 i : integer;
begin
 Result := '<ul>';
 // Добавляются в начало
 AddCmd('SearchRootkit(true, true);', 1, '$AVZ0499');
 AddCmd('SetAVZGuardStatus(True);', 1, '$AVZ0110');
 AddCmd('BC_ImportDeletedList;', 0, '$AVZ0011');
 AddCmd('ExecuteSysClean;', 0, '$AVZ1144');
 AddCmd('BC_Activate;', 0, '$AVZ0010');
 AddCmd('RebootWindows(true);', 0, '$AVZ0703');
 // Добавляются в хвост скрипта
 AddCmdQ('QuarantineFile('''','''');', 1, '$AVZ0162');
 AddCmdQ('BC_QrFile('''');', 1, '$AVZ1164');
 AddCmdQ('DeleteFile('''');', 1, '$AVZ0161');
 AddCmdQ('DelCLSID('''');', 1, '$AVZ0160');
 Result := Result + '</ul>';
 if ExtMenuScripts <> nil then begin
  Result := Result + TranslateStr('$AVZ0229:<ul>');
  for i := 0 to Length(ExtMenuScripts) - 1 do
   AddCmdQ(ExtMenuScripts[i].CmdText, ExtMenuScripts[i].AddMode, ExtMenuScripts[i].Name);
  Result := Result + '</ul>';
 end;
 Result := Result + '<hr>';
end;


begin
  HTML_StartLines := TStringList.Create;
  HTML_StartLines.Add('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">');//#DNL
  HTML_StartLines.Add('<HTML>');//#DNL
  HTML_StartLines.Add('<HEAD>');//#DNL
  HTML_StartLines.Add(' <META http-equiv=Content-Type content="text/html; charset=windows-1251">');//#DNL
  HTML_StartLines.Add(' <META http-equiv="nocache">');//#DNL
  HTML_StartLines.Add(' <META content="MSHTML 6.00.2900.2180" name=GENERATOR>');//#DNL
  HTML_StartLines.Add('</HEAD>');//#DNL
  HTML_BodyLines  := TStringList.Create;
  HTML_BodyLines.Add('<BODY bgColor=#ffdfb7>');//#DNL
  HTML_Scripts := TStringList.Create;
  HTML_Scripts.Add('<script language="JavaScript">');//#DNL
  HTML_Scripts.Add('function add_scr_line (s, InsMode) {');//#DNL
  HTML_Scripts.Add(' var szStr, szOldStr;');//#DNL
  HTML_Scripts.Add(' var InsPoz;');//#DNL
  HTML_Scripts.Add(' szStr    = document.forms.ScriptForm.CureScript.value;');//#DNL
  HTML_Scripts.Add(' szOldStr = szStr;');//#DNL
  HTML_Scripts.Add(' if (!(szStr.length > 5))');//#DNL
  HTML_Scripts.Add('  szStr = "begin\nend.";');//#DNL
  HTML_Scripts.Add(' if (szStr.substr(0, 5).toLowerCase() != "begin")');//#DNL
  HTML_Scripts.Add('  szStr = "begin\n" + szStr;');//#DNL
  HTML_Scripts.Add(' InsPoz = szStr.lastIndexOf("end.");');//#DNL
  HTML_Scripts.Add(' if (InsPoz < 5) {');//#DNL
  HTML_Scripts.Add('  szStr = szStr + "\nend.";');//#DNL
  HTML_Scripts.Add('  InsPoz = szStr.length-4;');//#DNL
  HTML_Scripts.Add(' }');//#DNL
  HTML_Scripts.Add(' if (InsMode == 1)');//#DNL
  HTML_Scripts.Add('  InsPoz = 5;');//#DNL
  HTML_Scripts.Add(' if (!(szStr.indexOf(s) >= 5)) {');//#DNL
  HTML_Scripts.Add('   var szStr1, szStr2;');//#DNL
  HTML_Scripts.Add('   szStr1 = szStr.substr(0, InsPoz);');//#DNL
  HTML_Scripts.Add('   szStr2 = szStr.substr(InsPoz);');//#DNL
  HTML_Scripts.Add('   if  (InsMode == 0)');//#DNL
  HTML_Scripts.Add('    szStr2 =  "\n" + szStr2;');//#DNL
  HTML_Scripts.Add('   else');//#DNL
  HTML_Scripts.Add('    szStr1 =  szStr1 + "\n";');//#DNL
  HTML_Scripts.Add('   szStr  = szStr1 + s +  szStr2;'); //#DNL
  HTML_Scripts.Add(' }');//#DNL
  HTML_Scripts.Add(' if (szStr != szOldStr)');//#DNL
  HTML_Scripts.Add('  document.forms.ScriptForm.CureScript.value = szStr;');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_f_line (s) {');//#DNL
  HTML_Scripts.Add(' var szStr;');//#DNL
  HTML_Scripts.Add(' szStr = document.forms.FilesForm.FileList.value;');//#DNL
  HTML_Scripts.Add(' if (szStr.length > 0)');//#DNL
  HTML_Scripts.Add('  szStr = szStr  + "\n";');//#DNL
  HTML_Scripts.Add(' if (szStr.indexOf(s) == -1)');//#DNL
  HTML_Scripts.Add('  document.forms.FilesForm.FileList.value =  szStr + s;');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_d (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" DeleteFile(''"+s+"'');", 0);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_bho (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" DelBHO(''"+s+"'');", 1);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_dpf (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" RegKeyDel(''HKLM'',''SOFTWARE\\Microsoft\\Code Store Database\\Distribution Units\\"+s+"'');", 1);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_s1 (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" StopService(''"+s+"'');", 1);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_s2 (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" DeleteService(''"+s+"'');", 1);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_s3 (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" SetServiceStart(''"+s+"'', 4);", 1);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_db (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" BC_DeleteFile(''"+s+"'');", 0);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_k (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" QuarantineFile(''"+s+"'','''');",1);');//#DNL
  HTML_Scripts.Add(' add_f_line(s);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_t (s) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(" TerminateProcessByName(''"+s+"'');",1);');//#DNL
  HTML_Scripts.Add(' add_f_line(s);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('function add_scr_line_q (s, InsMode) {');//#DNL
  HTML_Scripts.Add(' add_scr_line(unescape(s),InsMode);');//#DNL
  HTML_Scripts.Add('}');//#DNL
  HTML_Scripts.Add('</script>');//#DNL
end.
