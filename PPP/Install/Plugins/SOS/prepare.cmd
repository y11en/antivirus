rem @echo off

rem set paths
if not defined CMD_ROOT set CMD_ROOT=o:

set RELEASE=%CMD_ROOT%\out_win32\release
set INSTALL=%CMD_ROOT%\ppp\install
set INSTALL_PLUGIN=%CMD_ROOT%\ppp\install\plugins\sos
set BIN=%CMD_ROOT%\ppp\install\plugins\bin

rem set tools
if NOT DEFINED REGMERGER set REGMERGER="%BIN%\regmerger.exe"
set REPLACER="%BIN%\replacer.exe"

rem set strings to be replaced
set SRCSTR=HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\protected\avp8
set DSTSTRSOS=HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\Components\34\Products\KAVSOS8\8.0.0.0\avp8

rem set files
set REG_PPP="%INSTALL%\ppp.reg"
set REG_PPP_SOS="%INSTALL%\ppp_sos.reg"

set REG_AKSOS="%INSTALL_PLUGIN%\ak_sos.reg"

set REG_SOS_OUTPUT="%RELEASE%\ap_sos.reg"

rem --- prepare SOS ---

echo prepearing %REG_SOS_OUTPUT% file

if exist %REG_SOS_OUTPUT% del %REG_SOS_OUTPUT%
%REGMERGER% %REG_SOS_OUTPUT% %REG_PPP% %REG_PPP_SOS% %REG_AKSOS%
%REPLACER% SOURCE=%REG_SOS_OUTPUT% TARGET=%REG_SOS_OUTPUT% SEARCH=%SRCSTR% REPLACE=%DSTSTRSOS% IGNORECASE=Y
