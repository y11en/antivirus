@echo off
title Installing Kaspersky Anti-Virus

echo Applying default settings ...
start %SystemRoot%\System32\regedit.exe /s %SystemDrive%\programs\avp\avp.reg
echo Default settings have been successfully applied.

echo Coping Anti-Virus bases ...
xcopy "%SystemDrive%\Programs\avp\Temp\*.*" "%temp%\Documents and Settings\Application Data\avp\*.*" /Y /S
echo Anti-Virus bases have been successfully copied.

rem Fix Driver Signing
reg add HKLM\Software\Microsoft\Windows\CurrentVersion\Setup /v PrivateHash /t REG_BINARY /d 229e533e5d08abc8771db9d0e606a806 /f >nul

rem Install Intel AMT drivers
if exist "\I386\INF\iresol.inf" DEVCON UPDATENI "\i386\inf\MSPORTS.INF" "ACPI\PNP0501"
if exist "\I386\INF\iresol.inf" DEVCON INSTALL  "\i386\inf\IRESOL.INF"  "PCI\VEN_8086&DEV_108F"

rem Test SOL existence
devcon status =ports | find "SOL (COM3)" >nul
rem start AVP with SOL redirect
if "%errorlevel%"=="0" start %SystemDrive%\Programs\avp\avp.com /shell:com3 /bl /gui
exit