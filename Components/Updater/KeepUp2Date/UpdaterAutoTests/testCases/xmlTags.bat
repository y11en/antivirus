call %initScriptPath%init.bat

copy configuration\ss_storage.customUpdateRetranslationOptions ss_storage.ini /Y
echo update_srv_url=baseSources\xmlTags\ >> ss_storage.ini


REM use masterv2.xml primary index file
    echo primaryIndexFileName=masterv2.xml  >> .\ss_storage.ini
    echo primaryIndexRelativeUrlPath=index  >> .\ss_storage.ini
    echo primaryIndexLocalPath=  >> .\ss_storage.ini


REM enable retranslation
    echo updates_retranslation=1 >> .\ss_storage.ini
    echo update_bases=1 >> .\ss_storage.ini
    echo apply_urgent_updates=1 >> .\ss_storage.ini
    echo automatically_apply_available_updates=1 >> .\ss_storage.ini



%IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

diff.exe -r -q -x tmp out etalonsToCompareResult\xmlTags
if %ERRORLEVEL% EQU 0 goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! XML tags parse functionality automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo XML tags parse functionality automatic test succeed
    goto endScript

:endScript

