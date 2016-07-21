if NOT DEFINED OUT_FOLDER set OUT_FOLDER=out_win32
if NOT DEFINED REGMERGER set REGMERGER=O:\%OUT_FOLDER%\release\regmerger.exe
set INSTALL=O:\PPP\Install
set RESCUECD=O:\PPP\Install\RescueCD
set RESCUECDARCH=O:\PPP\Install\rescuecd.zip
set BARTPE=%RESCUECD%\BartPE
set WINPE=%RESCUECD%\WinPE
set FixUP=%RESCUECD%\FixUP
set REG=avp.reg


%REGMERGER% %RESCUECD%\%REG% %INSTALL%\ppp.reg %RESCUECD%\customize.reg

if exist %BARTPE%\%REG% del %BARTPE%\%REG%
copy %RESCUECD%\%REG% %BARTPE%
if ERRORLEVEL 1 goto err

if exist %WINPE%\%REG% del %WINPE%\%REG%
copy %RESCUECD%\%REG% %WINPE%
if ERRORLEVEL 1 goto err

del %RESCUECD%\%REG%

if exist %RESCUECDARCH% attrib -R %RESCUECDARCH%
if exist %RESCUECDARCH% del %RESCUECDARCH%

set PREPARE_SD=%CD%
cd %RESCUECD%
pkzip25.exe -add -dir=current %RESCUECDARCH% %BARTPE%\*.* %WINPE%\*.* %FixUP%\*.*
if ERRORLEVEL 1 goto err
cd %PREPARE_SD%

exit /b 0

:err
exit /b 1
