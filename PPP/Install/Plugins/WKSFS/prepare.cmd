rem @echo off

rem set paths
if not defined CMD_ROOT set CMD_ROOT=o:

set RELEASE=%CMD_ROOT%\out_win32\release
set INSTALL=%CMD_ROOT%\ppp\install
set INSTALL_PLUGIN=%CMD_ROOT%\ppp\install\plugins\wksfs
set BIN=%CMD_ROOT%\ppp\install\plugins\bin

rem set tools
if NOT DEFINED REGMERGER set REGMERGER="%BIN%\regmerger.exe"
set REPLACER="%BIN%\replacer.exe"

rem set strings to be replaced
set SRCSTR=HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\protected\avp7
set DSTSTRFS=HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\Components\34\Products\KAVFS7\7.0.0.0\avp7
set DSTSTRWKS=HKEY_LOCAL_MACHINE\SOFTWARE\KasperskyLab\Components\34\Products\KAVWKS7\7.0.0.0\avp7

rem set files
set REG_PPP="%INSTALL%\ppp.reg"
set REG_PPP_FS="%INSTALL%\ppp_fs.reg"
set REG_PPP_WKS="%INSTALL%\ppp_wks.reg"
set REG_PPP_MSEXCL="%INSTALL%\msexcl.reg"

set REG_AKFS="%INSTALL_PLUGIN%\ak_fs.reg"
set REG_AKWKS="%INSTALL_PLUGIN%\ak_wks.reg"

set REG_FS_OUTPUT="%RELEASE%\ap_fs.reg"
set REG_WKS_OUTPUT="%RELEASE%\ap_wks.reg"

rem --- prepare WKS ---

echo prepearing %REG_WKS_OUTPUT% file

if exist %REG_WKS_OUTPUT% del %REG_WKS_OUTPUT%
%REGMERGER% %REG_WKS_OUTPUT% %REG_PPP% %REG_PPP_WKS% %REG_AKWKS%
%REPLACER% SOURCE=%REG_WKS_OUTPUT% TARGET=%REG_WKS_OUTPUT% SEARCH=%SRCSTR% REPLACE=%DSTSTRWKS% IGNORECASE=Y



rem --- prepare FS ---

echo prepearing %REG_FS_OUTPUT% file

if exist %REG_FS_OUTPUT% del %REG_FS_OUTPUT%
%REGMERGER% %REG_FS_OUTPUT% %REG_PPP% %REG_PPP_FS% %REG_PPP_MSEXCL% %REG_AKFS%
%REPLACER% SOURCE=%REG_FS_OUTPUT% TARGET=%REG_FS_OUTPUT% SEARCH=%SRCSTR% REPLACE=%DSTSTRFS% IGNORECASE=Y

