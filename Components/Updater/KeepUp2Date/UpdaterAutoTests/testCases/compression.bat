call %initScriptPath%init.bat


xcopy configuration\ss_storage.ini .\ /Y
echo update_srv_url=baseSources\knownReply\version1 >> ss_storage.ini


REM first update operation to get bases for compression tests
%IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

diff.exe -r -q -x tmp out etalonsToCompareResult\knownReply\version1
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed



REM make a copy of update source, because test requires delete some files on source
xcopy baseSources\knownReply\version3 tmp\UpdateSource\  /E

xcopy configuration\ss_storage.ini ss_storage.ini /Y
echo update_srv_url=tmp\UpdateSource/ >> ss_storage.ini



REM delete files which will prove that update with compression mechanism succeed
REM  without this files nether reccursive difference update can happen, no update with klz
del tmp\UpdateSource\bases\RecursiveDiff.xml
del tmp\UpdateSource\bases\1\Recursive.avc
del tmp\UpdateSource\bases\Klz.xml

%IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

diff.exe -r -q -x temporaryFolder out etalonsToCompareResult\compression
if %ERRORLEVEL% EQU 0 goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! compression functionality automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo compression functionality automatic test succeed
    goto endScript

:endScript

