@echo off
rem --------------------------------
rem 1-arg - path to base test-cases folder
rem 2-arg - path to resolved test-cases folder
rem 3-arg - path to ini file with resolve params
rem --------------------------------
setlocal enabledelayedexpansion

echo start !TIME!
set DEF_LOG_FILE=%~n0.log

if "%1"=="" (
echo 1-arg missed>%DEF_LOG_FILE%
exit /b
)
if not exist %1 (
echo folder %1 not exist >%DEF_LOG_FILE%
exit /b
)
if "%2"=="" (
echo 2-arg missed>%DEF_LOG_FILE%
exit /b
)
if "%3"=="" (
echo 3-arg missed>%DEF_LOG_FILE%
exit /b
)
if not exist %3 (
echo file %3 not exist >%DEF_LOG_FILE%
exit /b
)


set /a LEVEL=0
set OUT_STR=
set /a STR_NUM=1
set /a RECURCE_LEVEL=1
set /a WAS_CHANGED=0
set /a WAS_SLASH_CHANGED=0

set INI_FILE_NAME=%3
set TEST_CASES_DIR=%1
set TEMP_DIR=temp
set TEMP_DIR2=temp2
set OUT_DIR=%2

set LAST_REPLACED_STR=

call :PREPARE_DIR %TEMP_DIR%
call :PREPARE_DIR %TEMP_DIR2%
call :PREPARE_DIR %OUT_DIR%

set /a RESOLVED_STR=0
for /f "tokens=1 delims==" %%M in (%INI_FILE_NAME%) do (
set /a RESOLVED_STR+=1
)


for /f "tokens=1 delims==" %%M in (%INI_FILE_NAME%) do (
set /a RESOLVED_STR-=1
echo Line to resolve !RESOLVED_STR!
if "%%M"=="" (
rem 
) else (
if "!LAST_REPLACED_STR!"=="%%M" (
rem same str
) else (
if "!LAST_REPLACED_STR!"=="" (
rem first 
set LAST_REPLACED_STR=%%M
call :CHANGE_FILES_IN_DIR %TEST_CASES_DIR% !LAST_REPLACED_STR! %TEMP_DIR%
rem exit /b
) else (
rem next dif str
set LAST_REPLACED_STR=%%M

if "!WAS_CHANGED!"=="1" (
set /a RECURCE_LEVEL+=1
)

set /a STR_NUM=1

call :PREPARE_DIR %TEMP_DIR2%

call :CHANGE_FILES_IN_DIR %TEMP_DIR% !LAST_REPLACED_STR! %TEMP_DIR2%

call :PREPARE_DIR %TEMP_DIR%

copy %TEMP_DIR2%\*.* %TEMP_DIR%

)
)
)
)

copy %TEMP_DIR%\*.* %OUT_DIR%
rd %TEMP_DIR% /Q /S
rd %TEMP_DIR2% /Q /S
echo end !TIME!
exit /b

rem ####################################
rem 1-arg - source dir
rem 2-arg - replace what
rem 3-arg - out dir
rem ------------------------------------
:CHANGE_FILES_IN_DIR
rem ------------------------------------
rem echo %0\%*

rem try to process test-cases

for /R %1 %%A in (*.*) do (
rem echo #######before STR_NUM=!STR_NUM!
set /a STR_NUM=1
rem echo #######after STR_NUM=!STR_NUM!

call :MAKE_REPLACES_FILES %%A %2 %3
)

exit /b
rem ####################################



rem ####################################
rem 1-arg - case file name
rem 2-arg - replace what
rem 3-arg - out dir
rem ------------------------------------
:MAKE_REPLACES_FILES
rem ------------------------------------
rem echo %0\%*

set /a INI_ITEM_NUM=0

for /f "tokens=1,* delims==" %%I in (%INI_FILE_NAME%) do (
if not "%%I"=="" (
if not "%%J"=="" (

if "%2"=="%%I" (
rem finded what need to replace
set /a INI_ITEM_NUM+=1

rem if "!WAS_CHANGED!"=="1" (
rem set /a STR_NUM+=1
rem )

if "!RECURCE_LEVEL!"=="1" (

if "!INI_ITEM_NUM!"=="1" (
rem first try change
call :SERV_REPLACE_IN_FILE %1 %%I "%%J" %3\%~n1_ag_!STR_NUM!.txt
) else (
if "!WAS_CHANGED!"=="1" (
call :SERV_REPLACE_IN_FILE %1 %%I "%%J" %3\%~n1_ag_!STR_NUM!.txt
)
)

) else (

if "!INI_ITEM_NUM!"=="1" (
rem first try change
call :SERV_REPLACE_IN_FILE %1 %%I "%%J" %3\%~n1_!STR_NUM!.txt
) else (
if "!WAS_CHANGED!"=="1" (
call :SERV_REPLACE_IN_FILE %1 %%I "%%J" %3\%~n1_!STR_NUM!.txt
)
)

)
rem end "!RECURCE_LEVEL!"=="1"

if "!WAS_CHANGED!"=="1" (
set /a STR_NUM+=1
)

)
rem end "%2"=="%%I"

)
)
)


exit /b
rem ####################################


rem ####################################
rem 1-arg - case file name
rem 2-arg - replace what
rem 3-arg - replace with
rem 4-arg - out file name
rem ------------------------------------
:SERV_REPLACE_IN_FILE
rem ------------------------------------
rem echo %0\%*
set /a WAS_CHANGED=0

for /f "tokens=*" %%L in (%1) do (
if not "%%L"=="" (
call :REPLACE_IN_STR "%%L" %2 %3
echo !OUT_STR!>>%4
)
)

if "!WAS_CHANGED!"=="0" (
rem echo file %1 was not changed
rem echo try to del %4 
del %4
rem echo try to copy %1 to %~dp4
if not exist %~dp4%~nx1 (
rem echo %~dp4%~nx1 not exist, copy
copy %1 %~dp4
) else (
rem echo %~dp4%~nx1 already exist
)
rem pause
)

exit /b
rem ####################################


rem ####################################
rem 1-arg - str
rem 2-arg - replace what
rem 3-arg - replace with
rem ------------------------------------
:REPLACE_IN_STR
rem ------------------------------------
rem echo %0 %*
rem exit /b
rem echo befor OUT_STR1=!OUT_STR!

if "!LEVEL!"=="0" (
set OUT_STR=
)

set /a LEVEL+=1
for /f "tokens=1,*" %%i in ("%~1") do (

if not "%%i"=="" (

if "%%i"=="%2" (
rem need to change
set /a WAS_CHANGED=1

if "!LEVEL!"=="1" (
rem add first changed substr
set OUT_STR=%~3
) else (
rem add next changed substr
set OUT_STR=!OUT_STR! %~3
)

) else (
rem need to save orig

if "!LEVEL!"=="1" (
rem first word

if "%%i"=="#" (
rem this is comment
set OUT_STR=%%i %%j
set /a LEVEL-=1
exit /b
)

rem add first orig substr
set OUT_STR=%%i

) else (
rem not first, add next orig substr

call :REPLACE_SLASH_STR "%%i" %2 %3

if "!WAS_SLASH_CHANGED!"=="0" (
rem slash was not found
set OUT_STR=!OUT_STR! %%i
) else (
set /a WAS_CHANGED=1
)


)
)
rem end of "%%i"=="%2"

rem echo befor OUT_STR1_1=!OUT_STR!

if not "%%j"=="" (
call :REPLACE_IN_STR "%%j" %2 %3
)

)
)

set /a LEVEL-=1
rem echo after OUT_STR1=!OUT_STR!

exit /b
rem ####################################

rem ####################################
rem 1-arg - str
rem 2-arg - replace what
rem 3-arg - replace with
rem ------------------------------------
:REPLACE_SLASH_STR
rem ------------------------------------
rem echo %0 %*
rem echo befor OUT_STR2=!OUT_STR!

set /a WAS_SLASH_CHANGED=0
for /f "tokens=1,* delims=\" %%n in ("%~1") do (
if "%%n"=="" (
exit /b
)
rem if "%%o"=="" (
rem exit /b
rem )

if "%%n"=="%2" (
rem need replase str befor slash
rem echo n=%%n o=%%o
for /f "tokens=1,* delims='" %%p in ("%~3") do (
if "%%p"=="" (
exit /b
)

rem echo p=%%p
set OUT_STR=!OUT_STR! '%%p\%%o'
set /a WAS_SLASH_CHANGED=1
)
)

)
rem echo after OUT_STR2=!OUT_STR!
exit /b
rem ####################################

rem ####################################
rem 1-arg - dir
rem ------------------------------------
:PREPARE_DIR
rem ------------------------------------
if exist %1 (
rd %1 /Q /S
)
mkdir %1
exit /b
rem ####################################
