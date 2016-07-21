set _BUILD_=%1
set TargetPath=%2
set TargetName=%3

If "%BASEDIR98%" == ""		echo Error P0001: BASEDIR98 envitonment variable should be defined.
If "%VTOOLSD%" == ""		echo Error P0002: VTOOLSD envitonment variable should be defined.

call setbasedir98 "%BASEDIR98%"

"%BASEDIR98%\bin\win98\bin16\rc" -nologo -x -r -I.\..\..\Common~1 -I%BASEDIR98S%\inc\win98\inc16 -Fo..\..\temp\Release\avpiovxd\avp_io.res avp_io.rc
"%VTOOLSD%\bin\sethdr" -n avp_io -x %2 -r ..\..\temp\Release\avpiovxd\avp_io.res

rem %MSVC16%\bin\rc -i "..\..\Common~1" -i %MSVC16%\Include -r %TargetName%.rc
rem %VTOOLSD%\bin\sethdr -n %TargetName% -x %TargetPath% -r %TargetName%.res
tsigner %TargetPath%

If "%SOFTICE%" == ""	goto end
If "%_BUILD_%" == "DEBUG" %SOFTICE%\nmsym /TRANS:SOURCE,PACKAGE,ALWAYS /LOAD %TargetPath%
:end
