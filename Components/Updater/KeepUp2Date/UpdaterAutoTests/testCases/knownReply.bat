call %initScriptPath%init.bat

REM loop for all tests
set testNumber=1
:startTest


echo ******* test %testNumber% *******

    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\knownReply\version%testNumber% >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    rd out\tmp\temporaryFolder /s /q

    REM in first test folders are empty (diff.exe can not ignore empty folders)
        if %testNumber% EQU 1 rd out\tmp\update\rollback
        if %testNumber% EQU 1 rd out\tmp\update
        if %testNumber% EQU 1 rd out\tmp
    
    diff.exe -r -q out etalonsToCompareResult\knownReply\version%testNumber%
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
   

    REM switch to next test
    set /A testNumber+=1
    if %testNumber% NEQ 4 goto startTest

    goto automaticTestSuccess



:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Well-known answers automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo well-known answers automatic tests succeed
    goto endScript


:endScript

