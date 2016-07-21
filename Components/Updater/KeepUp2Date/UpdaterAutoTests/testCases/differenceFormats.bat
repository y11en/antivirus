call %initScriptPath%init.bat


    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\differenceFormats\version1 >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    diff.exe -r -q -x tmp out etalonsToCompareResult\differenceFormats\version1
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
   



    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\differenceFormats\version2 >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    rd out\tmp\temporaryFolder /s /q

    diff.exe -r -q out etalonsToCompareResult\differenceFormats\version2
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


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

