rem Usage:    copyinstplug <dir>
rem Examples: copyinstplug O:\PackagePlg

echo copy installation files...

if "%1" == "" goto err

set plugin_project=KAVWksConsolePlugin

set SOURCE_DIR=..
set TARGET_DIR=%1
set XCOPY_CMD=xcopy /Y /R /Q 

md "%TARGET_DIR%\%plugin_project%"

%XCOPY_CMD% "%SOURCE_DIR%\%plugin_project%\*.idt" "%TARGET_DIR%\%plugin_project%"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%plugin_project%\*.cmd" "%TARGET_DIR%\%plugin_project%"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%plugin_project%.isv" "%TARGET_DIR%"
if ERRORLEVEL 1 goto err

exit /b 0

:err
echo %0(1) : error CMD0000: Build error.
exit /b 1
