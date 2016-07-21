call %initScriptPath%init.bat


REM perform 2 Updates loop
set i=1
:startTest

    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\knownReply\version%i% >> ss_storage.ini


    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    
    REM check if update succeed and well-know result is obtained
    diff.exe -r -q -x tmp out etalonsToCompareResult\knownReply\version%i%
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
   

    REM switch to next test
    set /A i+=1
    if %i% EQU 3 goto rollback
    goto startTest


:rollback
REM perform Rollback
%IUpdaterPath%\IUpdater.exe -r -tf IUpdater.log -c
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    
REM check if update succeed and well-know result is obtained
 REM files are not removed and set becomes different (added files are not rolled back)
diff.exe -r -q -x tmp -x rollback.ini -x Add.avc -x Add.xml out etalonsToCompareResult\knownReply\version1
if %ERRORLEVEL% EQU 0 goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! rollback automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo rollback automatic tests succeed
    goto endScript


:endScript

