@echo off

echo PreBuildStep ---- start

if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PreBuildStep ---- ERROR!!! - nor Release nor Debug
goto _Exit

:_Release
:_Debug                      

EDITBIN.EXE @%VTOOLSD%\include\ebin.cmd ..\..\..\..\temp\%1\KL1_9x\kl1.OBJ | %VTOOLSD%\bin\fltrwarn LNK4039 LNK4069
rem %VtoolsD%\bin\vxdver KL1.VRC KL1.RES

goto _Exit

:_Exit
echo PreBuildStep ---- Finished
