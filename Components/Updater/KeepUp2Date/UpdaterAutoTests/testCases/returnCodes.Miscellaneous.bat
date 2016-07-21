call %initScriptPath%init.bat


REM ****** Failed to create folder (CORE_FailedToCreateFolder = 1)
    REM this test case is going to try to write to non existent folder

        copy configuration\returnCodes\ss_storage.CORE_FILE_OPERATION_FAILURE ss_storage.ini /Y
        %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
        if %ERRORLEVEL% NEQ %CORE_FailedToCreateFolder% goto automaticTestFailed


REM ****** File does not exist on source (CORE_NoSuchFileOrDirectory = 3)
    REM loop 1 attemps (first remove primary index, then secondary index, and finaly base file)
    set i=1
    :noSourceFileTestStart

        call clean.bat

        REM make a copy of update source, because test requires delete file to prove return code works
        xcopy baseSources\knownReply\version1 tmp\UpdateSource\  /E

        xcopy configuration\ss_storage.ini .\ /Y
        echo update_srv_url=tmp\UpdateSource/ >> ss_storage.ini

        REM delete file to assert error code
        if %i% EQU 1 del tmp\UpdateSource\index\master.xml
        if %i% EQU 2 del tmp\UpdateSource\bases\Change.xml
        if %i% EQU 3 del tmp\UpdateSource\bases\1\Change.avc

        %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
        if %ERRORLEVEL% NEQ %CORE_NoSuchFileOrDirectory% goto automaticTestFailed

    REM switch to next test
    set /A i+=1
    if %i% EQU 4 goto noSourceFileTestSuccess
    goto noSourceFileTestStart

    :noSourceFileTestSuccess


REM ****** File does not exist on source (CORE_NO_SOURCE_FILE = 6)
    REM loop for HTTP and FTP
    set i=1
    :noSuchFileTestStart

        call clean.bat

        xcopy configuration\ss_storage.ini .\ /Y
        if %i% EQU 1 echo update_srv_url=ftp://d5x.kaspersky-labs.com/noExistentFolder >> ss_storage.ini
        if %i% EQU 2 echo update_srv_url=http://d5x.kaspersky-labs.com/noExistentFolder >> ss_storage.ini

        %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
        if %ERRORLEVEL% NEQ %CORE_NO_SOURCE_FILE% goto automaticTestFailed

    REM switch to next test
    set /A i+=1
    if %i% EQU 3 goto noSuchFileTestSuccess
    goto noSuchFileTestStart

    :noSuchFileTestSuccess


REM ****** No source contains valid update files or source list is not configured (CORE_URLS_EXHAUSTED = 9)
    call clean.bat

    REM copy configuration file, but not specify source
    xcopy configuration\ss_storage.ini .\ /Y

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ %CORE_URLS_EXHAUSTED% goto automaticTestFailed


REM ****** Incorrect get product configuration (CORE_ADMKIT_FAILURE = 15)
    call clean.bat

    REM perform update, but ss_storage.ini is absent
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ %CORE_ADMKIT_FAILURE% goto automaticTestFailed


REM ****** Bases are actual and require no update (CORE_NOTHING_TO_UPDATE = 10)

    xcopy baseSources\knownReply\version1 tmp\UpdateSource\  /E

    REM date check is only performed for Kaspersky Laboratory source
        md out
        md out\bases
        copy configuration\sitesToForceDateCheck.xml out\bases\sites.xml
        copy configuration\ss_storage.customsource.localComponentSource ss_storage.ini
        echo inet_update=1 >> ss_storage.ini


    REM perform update twice from the same source to get CORE_NOTHING_TO_UPDATE result
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ %CORE_NOTHING_TO_UPDATE% goto automaticTestFailed

    diff.exe -r -q -x tmp -x sites.xml out etalonsToCompareResult\knownReply\version1
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


REM ****** File to perform rollback operation is not found or damaged (CORE_NO_RESERVED_UPDATE_DESCRIPTION = 21)
    call clean.bat

    REM perform update from version1
    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\knownReply\version1 >> ss_storage.ini
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    diff.exe -r -q -x tmp out etalonsToCompareResult\knownReply\version1
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

    REM perform rollback (currently files are absent in rollback folder)
    %IUpdaterPath%\IUpdater.exe -r -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ %CORE_NO_RESERVED_UPDATE_DESCRIPTION% goto automaticTestFailed

    diff.exe -r -q -x tmp out etalonsToCompareResult\knownReply\version1
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM perform update from version2
    xcopy configuration\ss_storage.ini .\ /Y
    echo update_srv_url=baseSources\knownReply\version2 >> ss_storage.ini
    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM damage file in rollback folder
    echo "123" >> out\tmp\update\rollback\bases\1\Change.avc


    REM perform rollback (currently file damaged in rollback folder)
    %IUpdaterPath%\IUpdater.exe -r -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ %CORE_INVALID_SIGNATURE% goto automaticTestFailed


    REM check bases not damaged (except files in backup folder which are removed; and except rollback.ini which has been cleaned)
    diff.exe -r -q -x tmp -x rollback.ini out etalonsToCompareResult\knownReply\version2
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


REM ****** Index file damaged. File is not valid XML structure or file does not exist (CORE_UPDATE_DESCRIPTION_DAMAGED = 22)
    call clean.bat
    REM perform update. Update source contains damaged primary index file
        xcopy configuration\ss_storage.ini .\ /Y
        echo update_srv_url=baseSources\returnCodes\secondaryIndex.CORE_UPDATE_DESCRIPTION_DAMAGED >> ss_storage.ini
        %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
        if %ERRORLEVEL% NEQ %CORE_UPDATE_DESCRIPTION_DAMAGED% goto automaticTestFailed


REM ****** Failed to authorize on proxy server (CORE_PROXY_AUTH_ERROR = 32)
    REM TODO: design and implement test case


REM ****** Failed to resolve source DNS name (CORE_CANT_RESOLVE_NAME = 33)
    REM this code is tested in returnCodes.RESOLVE_CONNECT_FAIL.bat


REM ****** CORE_RETRANSLATION_SUCCESSFUL = 35, CORE_NO_OPERATION_REQUESTED = 36
    REM these codes are tested in updateRetranslationFlagCombinations.bat script


REM ****** Skip obsolete codes 37, 40

REM ****** Failed to authorize on server (CORE_SERVER_AUTH_ERROR = 40)
    REM TODO: design and implement test case


REM 38, 41, 42, 
    REM this code is tested in returnCodes.RESOLVE_CONNECT_FAIL.bat
    REM TODE : test cases are not implemented


goto automaticTestSuccess



:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! Updater miscellaneous return codes automatic tests failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo Updater miscellaneous return codes automatic tests succeed
    goto endScript


:endScript

