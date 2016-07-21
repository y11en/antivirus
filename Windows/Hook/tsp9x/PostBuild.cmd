%VTOOLSD%\bin\sethdr -n tsp -x %2 -r %3.res

rem @echo off

rem if %USERNAME%== Sobko echo PostBuildStep ---- Privet Sobko
if %USERNAME% == bes	goto _bes

set symfolder=.\
set Dcopydest=..\..\..\PRAGUE\Debug
set Rcopydest=..\..\..\PRAGUE\Release


goto _after_bes
rem ----------------------------------------------------------------
rem This section only for bes
:_bes
echo PostBuildStep ---- Hi Serge! ;-)
set symfolder=c:\sym\my\
set Dcopydest=I:\hook\Debug\
set Rcopydest=I:\hook\Debug\
rem End of bes section
rem ----------------------------------------------------------------
:_after_bes


if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PostBuildStep ---- ERROR!!! - nor Release nor Debug

goto _Exit


:_Release
echo PostBuildStep TSP ---- Release detected
xcopy ..\Release9x\tsp.vxd %Rcopydest% /R /D /Y
goto _Exit

:_Debug
echo PostBuildStep TSP ---- Debug detected
rem call nmsym /PROMPT /SOURCE:.;%VTOOLSD%src\ /TRANSLATE:SOURCE,PACKAGE,ALWAYS %2 
%VTOOLSD%\..\SOFTICE\nmsym.exe /PROMPT /SOURCE:.;..;%VTOOLSD%\src /TRANSLATE:SOURCE,PACKAGE,ALWAYS ..\Debug9x\tsp.vxd
xcopy ..\Debug9x\tsp.vxd %Dcopydest% /R /D /Y
xcopy ..\Debug9x\tsp.nms %Dcopydest% /R /D /Y

goto _Exit

:_Exit
set symfolder = 
set Dcopydest = 
set Rcopydest = 
echo PostBuildStep ---- Finished
