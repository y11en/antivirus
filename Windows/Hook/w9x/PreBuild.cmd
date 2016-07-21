@echo off

echo PreBuildStep ---- start

if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PreBuildStep ---- ERROR!!! - nor Release nor Debug
goto _Exit

:_Release
EDITBIN.EXE @%VTOOLSD%\include\ebin.cmd ..\..\..\temp\Release9x\vxdhook.OBJ | %VTOOLSD%\bin\fltrwarn LNK4039 LNK4069
goto _cont
:_Debug
EDITBIN.EXE @%VTOOLSD%\include\ebin.cmd ..\..\..\temp\Debug9x\vxdhook.OBJ | %VTOOLSD%\bin\fltrwarn LNK4039 LNK4069
goto _cont

:_cont
%VTOOLSD%\bin\vxdver AVPG.VRC KLIF.RES

goto _Exit

:_Exit
echo PreBuildStep ---- Finished
