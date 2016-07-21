set _BUILD_=%1
set TargetPath=%2
set TargetName=%3
echo %0 %_BUILD_%

If "%MSVC16%" == ""		echo Error P0001: MSVC16 envitonment variable should be defined.
If "%VTOOLSD%" == ""		echo Error P0002: VTOOLSD envitonment variable should be defined.

%MSVC16%\bin\rc -i "..\..\Common~1" -i %MSVC16%\Include -r %TargetName%.rc
%VTOOLSD%\bin\sethdr -n %TargetName% -x %TargetPath% -r %TargetName%.res
tsigner %TargetPath%

If "%SOFTICE%" == ""	goto end
If "%_BUILD_%" == "DEBUG" %SOFTICE%\nmsym /TRANS:SOURCE,PACKAGE,ALWAYS /LOAD %TargetPath%
:end
