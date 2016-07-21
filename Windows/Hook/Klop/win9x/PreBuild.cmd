@echo off

echo PreBuildStep ---- start

if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PreBuildStep ---- ERROR!!! - nor Release nor Debug
goto _Exit

:_Release
:_Debug                      

EDITBIN.EXE @%VTOOLSD%\include\ebin.cmd ..\..\..\..\temp\%1\klop_9x\main.OBJ | %VTOOLSD%\bin\fltrwarn LNK4039 LNK4069
rem %VtoolsD%\bin\vxdver klop.VRC klop.RES
rem %VtoolsD%\bin\vxdver klop.VRC ../../../../temp/release/klop_9x/klop_9x.res

goto _Exit

:_Exit
echo PreBuildStep ---- Finished