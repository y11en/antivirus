call %initScriptPath%init.bat

REM this test case asserts that change of LocalPath index tag does not damage update operation

md out
md out\bases


set testNumber=1
:startTest

    copy configuration\sitesToForceDateCheck.xml out\bases\sites.xml
    copy configuration\ss_storage.customsource.localComponentSource ss_storage.ini
    echo inet_update=1 >> ss_storage.ini

    xcopy baseSources\localPathAndRelativeUrlChange\version%testNumber% tmp\UpdateSource\  /E /Y


    REM enable retranslation
        echo updates_retranslation=1 >> .\ss_storage.ini
        echo update_bases=1 >> .\ss_storage.ini
        echo apply_urgent_updates=1 >> .\ss_storage.ini
        echo automatically_apply_available_updates=1 >> .\ss_storage.ini


    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ %CORE_NO_ERROR% goto automaticTestFailed


    if %testNumber% EQU 1 diff.exe -x tmp -r -q out etalonsToCompareResult\localPathAndRelativeUrlChange\version%testNumber%
    if %testNumber% EQU 2 diff.exe -x temporaryFolder -x relativeUrl -r -q out etalonsToCompareResult\localPathAndRelativeUrlChange\version%testNumber%
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    set /A testNumber+=1
    if %testNumber% EQU 3 goto automaticTestSuccess
    goto startTest



:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Local path and relative URL change automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo Local path and relative URL change automatic tests succeed
    goto endScript


:endScript
