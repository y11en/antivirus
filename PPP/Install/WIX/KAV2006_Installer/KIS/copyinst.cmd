rem Usage:    copyfiles <dir>
rem Examples: copyfiles O:\Package

echo copy installation files...

if "%1" == "" goto err

set SOURCE_DIR=.
set TARGET_DIR=%1
set XCOPY_CMD=xcopy /Y /R /Q 

%XCOPY_CMD% "%SOURCE_DIR%\*.wxs" "%TARGET_DIR%"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\*.wxi" "%TARGET_DIR%"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\*.wxl" "%TARGET_DIR%"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\*.xsl" "%TARGET_DIR%"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\*.cmd" "%TARGET_DIR%"
if ERRORLEVEL 1 goto err

md "%TARGET_DIR%\res"
%XCOPY_CMD% /S "%SOURCE_DIR%\res\*.*" "%TARGET_DIR%\res"
if ERRORLEVEL 1 goto err

md "%TARGET_DIR%\wixbin"
%XCOPY_CMD% /S "%SOURCE_DIR%\wixbin\*.*" "%TARGET_DIR%\wixbin"
if ERRORLEVEL 1 goto err

exit /b 0

:err
echo %0(1) : error CMD0000: Build error.
exit /b 1
