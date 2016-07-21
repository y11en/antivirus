"%BASEDIR98%\bin\win98\bin16\rc" -nologo -x -r -I.\..\.. -Fo../../../../temp/release/kl1_9x/kl1.res kl1_9x.rc

"%VTOOLSD%\bin\sethdr" -n kl1 -x %2 -r ../../../../temp/release/kl1_9x/kl1.res

@echo on

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
cd
if not exist ..\..\..\..\out\release md ..\..\..\..\out\release
copy .\Release\kl1.vxd ..\..\..\..\out\release\kl1.vxd
goto _Exit

:_Debug
echo PostBuildStep ---- Debug detected
cd
if not exist ..\..\..\..\out\release md ..\..\..\..\out\release
copy .\Debug\kl1.vxd ..\..\..\..\out\Debug\kl1.vxd

goto _Exit

:_Exit

if %USERNAME%== FoxGen goto _FoxGen_Post

:_Finish
echo PostBuildStep ---- Finished
