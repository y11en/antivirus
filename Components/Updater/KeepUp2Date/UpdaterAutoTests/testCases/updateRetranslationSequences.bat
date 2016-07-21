REM The Test asserts case: retranslation bases are not up-to-date, but update folder is up-to-date, then retranslation IS performed

REM initialization and cleanup folders
    call %initScriptPath%init.bat


REM set update source
    copy configuration\ss_storage.customUpdateRetranslationOptions ss_storage.ini /Y 
    echo update_srv_url=baseSources\knownReply\version1 >> ss_storage.ini

REM retranslation *disabled*, update enabled
    echo updates_retranslation=0 >> .\ss_storage.ini
    echo update_bases=1 >> .\ss_storage.ini
    echo apply_urgent_updates=1 >> .\ss_storage.ini
    echo automatically_apply_available_updates=1 >> .\ss_storage.ini

REM perform update
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c

    REM check results
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    diff.exe -r -q -x tmp -x backup out etalonsToCompareResult\knownReply\version1
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


REM set THE SAME update source
    copy configuration\ss_storage.customUpdateRetranslationOptions ss_storage.ini /Y 
    echo update_srv_url=baseSources\knownReply\version1 >> ss_storage.ini

REM retranslation *enable*, update enabled
    echo updates_retranslation=1 >> .\ss_storage.ini
    echo update_bases=1 >> .\ss_storage.ini
    echo apply_urgent_updates=1 >> .\ss_storage.ini
    echo automatically_apply_available_updates=1 >> .\ss_storage.ini

REM perform update
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c

REM check results
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    diff.exe -r -q -x backup -x tmp -x rollback.ini out etalonsToCompareResult\updateRetranslationFlagCombinations\retranslationAndUpdate
    if %ERRORLEVEL% EQU 0 goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Update/Retranslation call sequences automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo Update/Retranslation call sequences automatic tests succeed
    goto endScript


:endScript
