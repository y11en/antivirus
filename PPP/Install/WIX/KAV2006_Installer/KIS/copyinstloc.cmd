rem Usage:    copyfilesloc <locname> <outname>         <dir>
rem Examples: copyfilesloc Dutch     Dutch\<TIMESTAMP> O:\Package

echo copy installation localization files...

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err

set loc_name=%1
set loc_out_name=%2

set SOURCE_DIR=.
set TARGET_DIR=%3
set XCOPY_CMD=xcopy /Y /R /Q 

md "%TARGET_DIR%\localization\%loc_out_name%"

%XCOPY_CMD% /S "%SOURCE_DIR%\localization\%loc_name%\*.*" "%TARGET_DIR%\localization\%loc_out_name%"
if ERRORLEVEL 1 goto err

exit /b 0

:err
echo %0(1) : error CMD0000: Build error.
exit /b 1

