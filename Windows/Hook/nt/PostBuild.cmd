@echo off

set symfolder=.\
set Dcopydest=..\..\..\Prague\Debug
set Rcopydest=..\..\..\Prague\Release
if "%~2" neq "" (
	set OutDir=%~2
) else (
	set OutDir=..\%~1
)

echo Current user %USERNAME%

if %USERNAME%== Sobko goto _Sobko
if %USERNAME%== sobko goto _Sobko
if %USERNAME% == bes	goto _bes

goto _after_private

rem ----------------------------------------------------------------
rem This section only for Sobko
:_Sobko
goto :_after_private

:_Sobko_Post
rem echo PostBuildStep ---- copy symbols to symserver
rem if %1 == Debug call \\avp.ru\global\symbols\addsym_dev.cmd %OutDir%\klif.pdb "klif.sys debug"
rem if %1 == Debug call \\avp.ru\global\symbols\addsym_dev.cmd %OutDir%\klif.dbg "klif.sys debug"
rem if %1 == Release call \\avp.ru\global\symbols\addsym_dev.cmd %OutDir%\klif.dbg "klif.sys"
rem if %1 == Release call \\avp.ru\global\symbols\addsym_dev.cmd %OutDir%\klif.pdb "klif.sys"
goto _Finish
rem End of Sobko section
rem ----------------------------------------------------------------

:_bes_Post
echo PostBuildStep ---- copy symbols to symserver
if %1 == Debug call \\avp.ru\global\symbols\addsym_dev.cmd %OutDir%\klif.pdb "klif.sys debug"
if %1 == Debug call \\avp.ru\global\symbols\addsym_dev.cmd c:\sym\my\sys\klif.dbg "klif.sys debug"
if %1 == Release call \\avp.ru\global\symbols\addsym_dev.cmd c:\sym\my\sys\klif.dbg "klif.sys"
if %1 == Release call \\avp.ru\global\symbols\addsym_dev.cmd c:\sym\my\sys\klif.pdb "klif.sys"
goto _Finish
rem End of bes_post section
rem ----------------------------------------------------------------

rem ----------------------------------------------------------------
rem This section only for bes
:_bes
echo PostBuildStep ---- Hi Serge! ;-)
set symfolder=c:\sym\my\
set Dcopydest=I:\hook\Debug\
set Rcopydest=I:\hook\Debug\
goto :_after_private
rem End of bes section
rem ----------------------------------------------------------------

:_after_private


if %1 == Release goto _Release
if %1 == Debug goto _Debug

echo PostBuildStep ---- ERROR!!! - nor Release nor Debug

goto _Exit

:_Release
echo PostBuildStep ---- Release detected
rebase -b 0x10000 -x %symfolder% %OutDir%\klif.sys
xcopy %OutDir%\klif.sys %Rcopydest%  /R /D /Y
xcopy %OutDir%\klif.pdb %Rcopydest%  /R /D /Y
goto _Exit

:_Debug
echo PostBuildStep ---- Debug detected
rebase -b 0x10000 -x %symfolder% %OutDir%\klif.sys
xcopy %OutDir%\klif.sys %Dcopydest%   /R /D /Y
xcopy %OutDir%\klif.pdb %Dcopydest%   /R /D /Y
goto _Exit

:_Exit
rem set symfolder = 
rem set Dcopydest = 
rem set Rcopydest = 

if %USERNAME%== Sobko goto _Sobko_Post
if %USERNAME%== sobko goto _Sobko_Post
if %USERNAME% == bes	goto _bes_Post

:_Finish
echo PostBuildStep ---- Finished
