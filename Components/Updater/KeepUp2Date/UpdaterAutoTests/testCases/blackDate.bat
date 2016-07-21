
REM loop for all tests
set i=1
:startTest

    REM initialization and cleanup folders
    call %initScriptPath%init.bat


    echo ============= Test Case %i% =============

    md tmp
    md tmp\UpdateSource
    md tmp\UpdateSource\index
    copy baseSources\blackDate\master.xml tmp\UpdateSource\index\master.xml
    if %i% EQU 1 copy baseSources\blackDate\valid.secondary.xml tmp\UpdateSource\index\secondary.xml
    if %i% EQU 2 copy baseSources\blackDate\invalid.secondary.xml tmp\UpdateSource\index\secondary.xml
    copy baseSources\blackDate\base.avc tmp\UpdateSource\index\base.avc

    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=tmp\UpdateSource\ >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %i% EQU 1 if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    if %i% EQU 2 if %ERRORLEVEL% NEQ %CORE_WRONG_KEY% goto automaticTestFailed
    if %i% EQU 2 goto automaticTestSuccess


    diff.exe -q baseSources\blackDate\master.xml out\defaultlocalpath\master.xml
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    diff.exe -q baseSources\blackDate\valid.secondary.xml out\bases\secondary.xml
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    diff.exe -q baseSources\blackDate\base.avc out\bases\base.avc
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    set /A i+=1
    goto startTest




:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! black date check functionality automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo black date check functionality automatic test succeed
    goto endScript

:endScript
