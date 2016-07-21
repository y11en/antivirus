call %initScriptPath%init.bat


REM place all files to plain structure folder
xcopy baseSources\knownReply\version1\index tmp\UpdateSource\
xcopy baseSources\knownReply\version1\bases tmp\UpdateSource\
xcopy baseSources\knownReply\version1\bases\1 tmp\UpdateSource\


xcopy configuration\ss_storage.ini .\ /Y
echo update_srv_url=tmp\UpdateSource >> ss_storage.ini

%IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
    
diff.exe -r -q -x tmp out etalonsToCompareResult\knownReply\version1
if %ERRORLEVEL% NEQ 0 goto automaticTestFailed
   

goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! update from plain structure automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo update from plain structure automatic test succeed
    goto endScript


:endScript

