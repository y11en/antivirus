call %initScriptPath%init.bat


REM updater with 'MATCH' filter
  xcopy configuration\ss_storage.ini .\ /Y
  echo update_srv_url=baseSources\duplicates\version1 >> ss_storage.ini

%IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
  if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

diff.exe -r -q -x tmp out etalonsToCompareResult\duplicates\version1
  if %ERRORLEVEL% NEQ 0 goto automaticTestFailed



REM updater with 'MATCH' and 'FILTER' filter
  copy configuration\ss_storage.duplicateWithFilter ss_storage.ini /Y
  echo update_srv_url=baseSources\duplicates\version1 >> ss_storage.ini

%IUpdaterPath%\IUpdater.exe -u -tf IUpdater.log -c
  if %ERRORLEVEL% NEQ 0 goto automaticTestFailed

diff.exe -r -q -x tmp out etalonsToCompareResult\duplicates\version1.secondTest
if %ERRORLEVEL% EQU 0 goto automaticTestSuccess


:automaticTestFailed
    @echo .
    @echo .
    @echo WARNING! duplicate entry filtering functionality automatic test failed
    pause
    goto endScript

:automaticTestSuccess
    @echo .
    @echo .
    @echo duplicate entry filtering functionality automatic test succeed
    goto endScript

:endScript



