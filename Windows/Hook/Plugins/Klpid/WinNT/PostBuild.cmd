@echo off

echo hello %USERNAME% !

if %USERNAME%== Foxgen goto _FoxGen
if %USERNAME%== Sobko goto _Sobko

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

rem ----------------------------------------------------------------
rem This section only for Sobko
:_Sobko
goto :_after_private

:_Sobko_Post
echo PostBuildStep ---- copy symbols to symserver
goto _Finish
rem End of _Sobko section
rem ----------------------------------------------------------------


:_after_private


if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PostBuildStep ---- ERROR!!! - nor Release nor Debug

goto _Exit

:_Release
echo PostBuildStep ---- Release detected
copy .\Release\klpid.sys ..\..\..\Release\klpid.sys
copy .\Release\klpid.pdb ..\..\..\Release\klpid.pdb
goto _Exit

:_Debug
echo PostBuildStep ---- Debug detected
copy .\Debug\klpid.sys ..\..\..\Debug\klpid.sys
copy .\Debug\klpid.pdb ..\..\..\Debug\klpid.pdb
goto _Exit

:_Exit
if %USERNAME%== Foxgen goto _FoxGen_Post
if %USERNAME%== Sobko goto _Sobko_Post

:_Finish
echo PostBuildStep ---- Finished
