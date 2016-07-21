call %initScriptPath%init.bat


    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\optionalComponent >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    diff.exe -r -q -x tmp out etalonsToCompareResult\optionalComponent
    if %ERRORLEVEL% EQU 0 goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Optional component automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo Optional component automatic test succeed
    goto endScript


:endScript

