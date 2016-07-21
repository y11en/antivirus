@echo off

if %USERNAME%== FoxGen goto _FoxGen

goto _after_private

rem ----------------------------------------------------------------
rem This section only for FoxGen
:_FoxGen
goto :_after_private

:_FoxGen_Post
echo PostBuildStep ---- copy symbols to symserver
rem if %1 == Debug call \\Virlib\Symbols\addsym.cmd ..\..\Debug\true.pdb "true.lib"
rem if %1 == Release call \\Virlib\Symbols\addsym.cmd ..\..\Release\true.pdb "true.lib"
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
rem rebase -b 0x10000 -x %symfolder% ..\Release\avpg.sys
copy .\Release\true.lib ..\Release\true.lib
goto _Exit

:_Debug
echo PostBuildStep ---- Debug detected
rem rebase -b 0x10000 -x %symfolder% ..\Debug\avpg.sys
copy .\Debug\true.lib ..\Debug\true.lib
goto _Exit

:_Exit

if %USERNAME%== FoxGen goto _FoxGen_Post

:_Finish
echo PostBuildStep ---- Finished
