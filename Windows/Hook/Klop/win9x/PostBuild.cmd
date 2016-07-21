call setbasedir98 "%BASEDIR98%"

"%BASEDIR98%\bin\win98\bin16\rc" -nologo -x -r -I.\..\..\..\..\Common~1 -I%BASEDIR98S%\inc\win98\inc16 -Fo%1\klop_9x.res klop_9x.rc
"%VTOOLSD%\bin\sethdr" -n KLOP -x %2 -r %1\klop_9x.res

rem %BASEDIR98%\bin\win98\bin16\rc -nologo -x -r -I.\..\.. -Fo../../../../temp/release/klick_9x/klick_9x.res klick_9x.rc
rem %BASEDIR98%\bin\win98\adrc2vxd %2 ../../../../temp/release/klick_9x/klick_9x.res

rem %VTOOLSD%\bin\sethdr -n KLOP -x %2 -r ../../../../temp/release/klop_9x/klop_9x.res

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
copy .\Release\klop.vxd ..\..\..\..\out\release\klop.vxd
goto _Exit

:_Debug
echo PostBuildStep ---- Debug detected
copy .\Debug\klop.vxd ..\..\..\..\out\Debug\klop.vxd
goto _Exit

:_Exit

if %USERNAME%== FoxGen goto _FoxGen_Post

:_Finish
echo PostBuildStep ---- Finished
