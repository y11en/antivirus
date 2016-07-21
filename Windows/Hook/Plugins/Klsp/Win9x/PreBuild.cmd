@echo off

echo PreBuildStep ---- start

if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PreBuildStep ---- ERROR!!! - nor Release nor Debug
goto _Exit

:_Release
:_Debug                      

EDITBIN.EXE @%VTOOLSD%\include\ebin.cmd .\%1\main.OBJ | %VTOOLSD%\bin\fltrwarn LNK4039 LNK4069
%VtoolsD%\bin\vxdver klsp.VRC klsp.RES

goto _Exit

:_Exit
echo PreBuildStep ---- Finished