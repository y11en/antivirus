REM loop for all tests
set i=1
:startTest

    REM initialization and cleanup folders
    call %initScriptPath%init.bat


    REM copy configuration file
    copy configuration\ss_storage.customSource ss_storage.ini /Y

    echo inet_update=0 >> .\ss_storage.ini
    echo try_inet=0 >> .\ss_storage.ini


    REM ********* no proxy ***********
    if %i% LSS 4 echo proxy=0 >> .\ss_storage.ini
    if %i% LSS 4 echo use_ie_proxy=0 >> .\ss_storage.ini

    REM first attempt is update from FTP (active)
    if %i% EQU 1 echo passive_ftp=0 >> .\ss_storage.ini
    if %i% EQU 1 echo update_srv_url=ftp://no.exist.domain.kaspersky-labs.com>> .\ss_storage.ini
    REM first attempt is update from FTP (passive)
    if %i% EQU 2 echo passive_ftp=1 >> .\ss_storage.ini
    if %i% EQU 2 echo update_srv_url=ftp://no.exist.domain.kaspersky-labs.com>> .\ss_storage.ini
    REM second attempt is update from HTTP
    if %i% EQU 3 echo update_srv_url=http://no.exist.domain.kaspersky-labs.com>> .\ss_storage.ini


    REM ********* with proxy ***********
    if %i% GEQ 4 echo proxy=1 >> .\ss_storage.ini
    if %i% GEQ 4 echo use_ie_proxy=1 >> .\ss_storage.ini

    REM first attempt is update from FTP (active)
    if %i% EQU 4 echo passive_ftp=0 >> .\ss_storage.ini
    if %i% EQU 4 echo update_srv_url=ftp://no.exist.domain.kaspersky-labs.com>> .\ss_storage.ini
    REM first attempt is update from FTP (passive)
    if %i% EQU 5 echo passive_ftp=1 >> .\ss_storage.ini
    if %i% EQU 5 echo update_srv_url=ftp://no.exist.domain.kaspersky-labs.com>> .\ss_storage.ini
    REM second attempt is update from HTTP
    if %i% EQU 6 echo update_srv_url=http://no.exist.domain.kaspersky-labs.com>> .\ss_storage.ini




    REM perform update
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %i% LSS 4 if %ERRORLEVEL% NEQ %CORE_CANT_RESOLVE_NAME% goto automaticTestFailed
    if %i% GEQ 4 if %ERRORLEVEL% NEQ %CORE_CANT_CONNECT_INET_SERVER% goto automaticTestFailed


    REM switch to next test
    set /A i+=1
    if %i% EQU 7 goto automaticTestSuccess
    goto startTest



:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Updater resolve name and inet connect return codes automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo Updater resolve name and inet connect return codes automatic tests succeed
    goto endScript


:endScript

