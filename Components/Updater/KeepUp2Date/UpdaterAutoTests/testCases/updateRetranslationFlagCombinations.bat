REM loop for all tests
set i=1
:startTest
    call %initScriptPath%init.bat


    copy configuration\ss_storage.customUpdateRetranslationOptions ss_storage.ini /Y 
    echo update_srv_url=baseSources\knownReply\version1 >> ss_storage.ini


    REM 1) retranslation disabled, update disabled
    if %i% EQU 1 echo updates_retranslation=0 >> .\ss_storage.ini
    if %i% EQU 1 echo update_bases=0 >> .\ss_storage.ini
    if %i% EQU 1 echo apply_urgent_updates=0 >> .\ss_storage.ini
    if %i% EQU 1 echo automatically_apply_available_updates=0 >> .\ss_storage.ini

    REM 2) retranslation disabled, update enabled
    if %i% EQU 2 echo updates_retranslation=0 >> .\ss_storage.ini
    if %i% EQU 2 echo update_bases=1 >> .\ss_storage.ini
    if %i% EQU 2 echo apply_urgent_updates=1 >> .\ss_storage.ini
    if %i% EQU 2 echo automatically_apply_available_updates=1 >> .\ss_storage.ini

    REM 3) retranslation enabled, update disabled
    if %i% EQU 3 echo updates_retranslation=1 >> .\ss_storage.ini
    if %i% EQU 3 echo update_bases=0 >> .\ss_storage.ini
    if %i% EQU 3 echo apply_urgent_updates=0 >> .\ss_storage.ini
    if %i% EQU 3 echo automatically_apply_available_updates=0 >> .\ss_storage.ini

    REM 4) retranslation enabled, update enabled
    if %i% EQU 4 echo updates_retranslation=1 >> .\ss_storage.ini
    if %i% EQU 4 echo update_bases=1 >> .\ss_storage.ini
    if %i% EQU 4 echo apply_urgent_updates=1 >> .\ss_storage.ini
    if %i% EQU 4 echo automatically_apply_available_updates=1 >> .\ss_storage.ini



    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c



    REM 1) at first attempt it is going to be 'Neither update nor retranslation have been requested' code
    if %i% EQU 1 if %ERRORLEVEL% NEQ %CORE_NO_OPERATION_REQUESTED% goto automaticTestFailed


    REM 2) check result of second attempt
    if %i% EQU 2 if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    if %i% EQU 2 diff.exe -r -q -x tmp out etalonsToCompareResult\knownReply\version1
    if %i% EQU 2 if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM 3) check result of 3rd attempt
    if %i% EQU 3 if  %ERRORLEVEL% NEQ %CORE_RETRANSLATION_SUCCESSFUL% goto automaticTestFailed
    if %i% EQU 3 diff.exe -r -q -x tmp -x installation out etalonsToCompareResult\updateRetranslationFlagCombinations\retranslationOnly
    if %i% EQU 3 if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    REM 4) check result of 4th attempt
    if %i% EQU 4 if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    if %i% EQU 4 diff.exe -r -q -x tmp out etalonsToCompareResult\updateRetranslationFlagCombinations\retranslationAndUpdate
    if %i% EQU 4 if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    


    REM switch to next test
    set /A i+=1
    if %i% EQU 5 goto automaticTestSuccess
    goto startTest



:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Update/Retranslation flag combination automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo Update/Retranslation flag combination automatic tests succeed
    goto endScript


:endScript
