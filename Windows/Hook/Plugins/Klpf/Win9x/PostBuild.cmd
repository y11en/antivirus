%VTOOLSD%\bin\sethdr -n KLPF -x %2 -r %3.res

@echo off

if %USERNAME%== FoxGen goto _FoxGen

goto _after_private

rem ----------------------------------------------------------------
rem This section only for FoxGen
:_FoxGen
goto :_after_private

:_FoxGen_Post
echo PostBuildStep ---- copy symbols to symserver

goto _Finish
rem End of _FoxGen section
rem ----------------------------------------------------------------

:_after_private


if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PostBuildStep ---- ERROR!!! - nor Release nor Debug

goto _Exit

:_Release
echo PostBuildStep ---- Release detected
goto _Exit

:_Debug
echo PostBuildStep ---- Debug detected
goto _Exit

:_Exit

if %USERNAME%== FoxGen goto _FoxGen_Post

:_Finish
echo PostBuildStep ---- Finished
