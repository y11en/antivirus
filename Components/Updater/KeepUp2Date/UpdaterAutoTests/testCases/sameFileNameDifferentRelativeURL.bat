call %initScriptPath%init.bat

copy configuration\ss_storage.customUpdateRetranslationOptions ss_storage.ini /Y
echo update_srv_url=baseSources\sameFileNameDifferentRelativeURL\ >> ss_storage.ini


REM use master.xml primary index file
    echo primaryIndexFileName=master.xml  >> .\ss_storage.ini
    echo primaryIndexRelativeUrlPath=index  >> .\ss_storage.ini
    echo primaryIndexLocalPath=  >> .\ss_storage.ini


REM ======== ITERATION 1:    update enabled and retranslation disabled to have equalFileName.avc file in local bases folder
        echo updates_retranslation=0 >> .\ss_storage.ini
        echo update_bases=1 >> .\ss_storage.ini
        echo apply_urgent_updates=1 >> .\ss_storage.ini
        echo automatically_apply_available_updates=1 >> .\ss_storage.ini


    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    diff.exe -r -q -x tmp out etalonsToCompareResult\sameFileNameDifferentRelativeURL\version1
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed




REM ======== ITERATION 2:    update enabled and retranslation disabled to have equalFileName.avc file in local bases folder

    REM first update enabled and retranslation enabled to check that correct equalFileName.avc file is retranslated
        echo updates_retranslation=1 >> .\ss_storage.ini
        echo update_bases=1 >> .\ss_storage.ini
        echo apply_urgent_updates=1 >> .\ss_storage.ini
        echo automatically_apply_available_updates=1 >> .\ss_storage.ini


    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    diff.exe -r -q -x tmp out etalonsToCompareResult\sameFileNameDifferentRelativeURL\version2
    if %ERRORLEVEL% EQU 0 goto automaticTestSuccess



:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! same file name, different relative URL automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo same file name, different relative URL automatic test failed
    goto endScript

:endScript

