@echo off

set PB_CONFIG=%1
echo PreBuildStep ---- %PB_CONFIG% ...

goto _%PB_CONFIG%

:_Release
set PB_SRC=ReleaseDll
goto _DoJob

:_ReleaseS
set PB_SRC=Release
goto _DoJob

:_Debug
set PB_SRC=DebugDll
goto _DoJob

:_DebugS
set PB_SRC=Debug
goto _DoJob

:_DoJob
del /F /Q ..\..\..\CommonFiles\%PB_SRC%\FSDrvLib.* >nul

:_Exit
echo PreBuildStep ---- %PB_CONFIG% Finished
set PB_SRC=
set PB_CONFIG=
