set MYCURRDIR=%CD%

cd ..\kdprint\kernel\
build.exe -cZg

cd %MYCURRDIR%
build.exe -cZg
postbuild.cmd
