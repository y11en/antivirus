
REM   ******************************************************
REM   **********   Test Set 1: Initial values:  FileDate 4, UpdateDate 8   **********


REM loop for all tests
set i=1
:startTestSet1

    REM initialization and cleanup folders
    call %initScriptPath%init.bat


    echo ============= Test %i% Set 1 =============

    md tmp
    md tmp\UpdateSource
    md tmp\UpdateSource\index
    copy baseSources\checkByDate\FileDate4.primaryv0 tmp\UpdateSource\index\master.xml
    copy baseSources\checkByDate\UpdateDate8.secondary tmp\UpdateSource\index\secondary.xml
    copy baseSources\checkByDate\base.avc tmp\UpdateSource\index\base.avc

    REM date check is only performed for Kaspersky Laboratory source
        md out
        md out\bases
        copy configuration\sitesToForceDateCheck.xml out\bases\sites.xml
        copy configuration\ss_storage.customsource.localComponentSource ss_storage.ini
        echo inet_update=1 >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM removing files to prove extra files are not downloaded
    del tmp\UpdateSource\index\master.xml
    del tmp\UpdateSource\index\secondary.xml
    del tmp\UpdateSource\index\base.avc


    goto initSet1Test%i%

        :initSet1Test1
            REM   ***   set 1 test 1:  update to FileDate 3  ***
            copy baseSources\checkByDate\FileDate3.primary tmp\UpdateSource\index\master.xml
            goto updateTestSet1
        :initSet1Test2
            REM   ***   set 1 test 2:  update to FileDate 4  ***
            copy baseSources\checkByDate\FileDate8.primary tmp\UpdateSource\index\master.xml
            goto updateTestSet1


        :initSet1Test3
            REM   ***   set 1 test 3:  update to FileDate 9, UpdateDate7  ***
            copy baseSources\checkByDate\FileDate9.primary tmp\UpdateSource\index\master.xml
            copy baseSources\checkByDate\UpdateDate7.secondary tmp\UpdateSource\index\secondary.xml
            goto updateTestSet1
        :initSet1Test4
            REM   ***   set 1 test 4:  update to FileDate 9, UpdateDate8  ***
            copy baseSources\checkByDate\FileDate9.primary tmp\UpdateSource\index\master.xml
            copy baseSources\checkByDate\UpdateDate8.secondary tmp\UpdateSource\index\secondary.xml
            goto updateTestSet1


        :initSet1Test5
            REM   ***   set 1 test 5:  update to FileDate 9, UpdateDate9  ***
            copy baseSources\checkByDate\FileDate9.primary tmp\UpdateSource\index\master.xml
            copy baseSources\checkByDate\UpdateDate9.secondary tmp\UpdateSource\index\secondary.xml
            goto updateTestSet1



    REM   Update operation for test set 1
    :updateTestSet1
        %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM for all tests base file must present
        diff.exe -q baseSources\checkByDate\base.avc out\bases\base.avc
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    goto checkSet1Test%i%

        
    :checkSet1Test1
        diff.exe -q baseSources\checkByDate\FileDate3.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate8.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet1
    :checkSet1Test2
        diff.exe -q baseSources\checkByDate\FileDate8.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate8.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet1


    :checkSet1Test3
    :checkSet1Test4
        diff.exe -q baseSources\checkByDate\FileDate9.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate8.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet1


    :checkSet1Test5
        diff.exe -q baseSources\checkByDate\FileDate9.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate9.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet1

    :endTestSet1



        set /A i+=1
        if %i% NEQ 6 goto startTestSet1



REM   ******************************************************
REM   **********   Test Set 2: Initial values:  FileDate 8, UpdateDate 4   **********

set i=1
:startTestSet2

    REM initialization and cleanup folders
    call %initScriptPath%init.bat

    echo ============= Test %i% Set 2 =============

    md tmp
    md tmp\UpdateSource
    md tmp\UpdateSource\index
    copy baseSources\checkByDate\FileDate8.primaryv0 tmp\UpdateSource\index\master.xml
    copy baseSources\checkByDate\UpdateDate4.secondary tmp\UpdateSource\index\secondary.xml
    copy baseSources\checkByDate\base.avc tmp\UpdateSource\index\base.avc


    REM date check is only performed for Kaspersky Laboratory source
        md out
        md out\bases
        copy configuration\sitesToForceDateCheck.xml out\bases\sites.xml
        copy configuration\ss_storage.customsource.localComponentSource ss_storage.ini
        echo inet_update=1 >> ss_storage.ini

    %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
    if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM removing files to prove extra files are not downloaded
    del tmp\UpdateSource\index\master.xml
    del tmp\UpdateSource\index\secondary.xml
    del tmp\UpdateSource\index\base.avc


    goto initSet2Test%i%

        :initSet2Test1
            REM   ***   set 2 test 1:  update to FileDate 4  ***
            copy baseSources\checkByDate\FileDate4.primary tmp\UpdateSource\index\master.xml
            goto updateTestSet2


        :initSet2Test2
            REM   ***   set 2 test 2:  update to FileDate 5, UpdateDate  3 ***
            copy baseSources\checkByDate\FileDate5.primary tmp\UpdateSource\index\master.xml
            copy baseSources\checkByDate\UpdateDate3.secondary tmp\UpdateSource\index\secondary.xml
            goto updateTestSet2


        :initSet2Test3
            REM   ***   set 2 test 2:  update to FileDate 5, UpdateDate  5 ***
            copy baseSources\checkByDate\FileDate5.primary tmp\UpdateSource\index\master.xml
            copy baseSources\checkByDate\UpdateDate5.secondary tmp\UpdateSource\index\secondary.xml
            goto updateTestSet2




    REM   Update operation for test set 2
    :updateTestSet2
        %IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    REM for all tests base file must present
        diff.exe -q baseSources\checkByDate\base.avc out\bases\base.avc
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed


    goto checkSet2Test%i%

        
    :checkSet2Test1
        diff.exe -q baseSources\checkByDate\FileDate4.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate4.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet2

    :checkSet2Test2
        diff.exe -q baseSources\checkByDate\FileDate5.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate4.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet2

    :checkSet2Test3
        diff.exe -q baseSources\checkByDate\FileDate5.primary out\defaultlocalpath\master.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
        diff.exe -q baseSources\checkByDate\UpdateDate5.secondary out\bases\secondary.xml
        if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

        goto endTestSet2

    :endTestSet2



        set /A i+=1
        if %i% NEQ 4 goto startTestSet2
        goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! check consistency by date functionality automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo check consistency by date functionality automatic test succeed
    goto endScript

:endScript

