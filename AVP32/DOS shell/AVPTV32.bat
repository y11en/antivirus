@echo off

:UI
avptv32.exe /ss=tmp.prf
IF ERRORLEVEL 1 GOTO END1
IF ERRORLEVEL 0 GOTO END0

:END1
rem echo Start Scaner
c:\lang\avp\avp32.dos\avp32\face\avpdos32.exe /f=tmp.prf
echo Press any key to return to the AVPTV32
pause > nul
GOTO UI

:END0

