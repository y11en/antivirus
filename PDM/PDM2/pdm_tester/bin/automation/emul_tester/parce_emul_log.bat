@echo off
rem ------------------------------------
rem 1-arg - path to test_cases collection
rem ------------------------------------
set DEF_LOG_FILE=%~n0.log

if exist %DEF_LOG_FILE% (
del %DEF_LOG_FILE%
)

if "%1"=="" (
echo 1-arg missed>>%DEF_LOG_FILE%
exit /b
)
if not exist %1 (
echo folder %1 not exist >>%DEF_LOG_FILE%
exit /b
)

setlocal enabledelayedexpansion
set LOG_FILE=Logs\~emul_log_parce_res.log 
set NOVERD_LOG_FILE=Logs\~no_verd_file_list.log 
set KNOWN_ERRORS_FILE=KNOWN_ERRORS.txt
set TEST_CASES_PATH=%1
del %LOG_FILE%
del %NOVERD_LOG_FILE%

set /a ALL_FILE_COUNT=0
set /a VERD_FILE_COUNT=0
set /a NO_VERD_FILE_COUNT=0
set /a KNOWN_ERROR_FILE_COUNT=0
set /a IS_KNOWN_ERROR=0
set /a EQUAL_TO_ETALON_VERDICTS=1
set /a ETALON_VERDICT_COUNT=0
set /a EQUAL_VERDICT_COUNT=0
set /a NOT_EQUAL_VERDICT_COUNT=0

for /R Logs %%i in (*.*) do (
call :PARCE_FILE %%i
)


echo ------------------------------------------>>%LOG_FILE%
echo All files: !ALL_FILE_COUNT!>>%LOG_FILE%
echo Equal to etalon verdict: !EQUAL_VERDICT_COUNT!>>%LOG_FILE%
echo Not equal to etalon verdict: !NOT_EQUAL_VERDICT_COUNT!>>%LOG_FILE%
echo Files with verdict: !VERD_FILE_COUNT!>>%LOG_FILE%
echo Known error: !KNOWN_ERROR_FILE_COUNT!>>%LOG_FILE%
echo Files without verdict: !NO_VERD_FILE_COUNT!>>%LOG_FILE%
echo ------------------------------------------>>%LOG_FILE%
rem pause
exit /b
rem end main
rem ###########################################


rem #######################################
:PARCE_FILE
rem echo %*


set /a EQUAL_TO_ETALON_VERDICTS=1
set /a WAS_VERDICT=0
set /a ALL_FILE_COUNT+=1
echo ------------------------------>>%LOG_FILE%
echo %1>>%LOG_FILE%
echo ------------------------------>>%LOG_FILE%
for /f "tokens=1,2,3" %%I in (%1) do (
if not "%%I"=="" (
if "%%I"=="VERDICT:" (
set /a WAS_VERDICT+=1
echo %%J %%K>>%LOG_FILE%
if "!EQUAL_TO_ETALON_VERDICTS!"=="1" (
rem echo EQUAL_TO_ETALON_VERDICTS==1
call :COMPARE_WITH_ETALON %TEST_CASES_PATH%\%~n1.txt !WAS_VERDICT! %%J %%K
) else (
rem echo EQUAL_TO_ETALON_VERDICTS!=1
)

)
)
)
if "!WAS_VERDICT!"=="0" (
call :IS_IN_KNOWN_ERRORS %~n1
if "!IS_KNOWN_ERROR!"=="1" (
set /a KNOWN_ERROR_FILE_COUNT+=1
echo KNOWN ERROR>>%LOG_FILE%
) else (
set /a NO_VERD_FILE_COUNT+=1
echo NO VERDICTS>>%LOG_FILE%
echo %1>>%NOVERD_LOG_FILE%
)
) else (
set /a VERD_FILE_COUNT+=1
)

call :GET_ETALON_VERDICT_COUNT %TEST_CASES_PATH%\%~n1.txt

rem echo "!ETALON_VERDICT_COUNT!"=="!WAS_VERDICT!"
if "!EQUAL_TO_ETALON_VERDICTS!"=="1" (
if "!ETALON_VERDICT_COUNT!"=="!WAS_VERDICT!" (
set /a EQUAL_VERDICT_COUNT+=1
echo OK. EQUAL TO ETALON.>>%LOG_FILE%
) else (
set /a NOT_EQUAL_VERDICT_COUNT+=1
echo NOT EQUAL TO ETALON----------- >>%LOG_FILE%
call :PRINT_ORIG_ETALON %TEST_CASES_PATH%\%~n1.txt
)
) else (
set /a NOT_EQUAL_VERDICT_COUNT+=1
echo NOT EQUAL TO ETALON----------- >>%LOG_FILE%
call :PRINT_ORIG_ETALON %TEST_CASES_PATH%\%~n1.txt
)

exit /b
rem ####################################### 

rem #######################################
:IS_IN_KNOWN_ERRORS
rem echo file to check %*
set /a IS_KNOWN_ERROR=0
for /f "tokens=1,*" %%I in (%KNOWN_ERRORS_FILE%) do (
rem echo str %%I
if "%%I"=="%1" (
rem echo before inc IS_KNOWN_ERROR
set /a IS_KNOWN_ERROR=1
rem echo after inc IS_KNOWN_ERROR(!IS_KNOWN_ERROR!)
exit /b
)
)


exit /b
rem #######################################

rem #######################################
rem 1-arg - etalon test-case file name
rem 2-arg - verdict num
rem 3-arg - weight
rem 4-arg - verdict name
:COMPARE_WITH_ETALON
rem echo %*
set /a LOC_VERDICT_NUM=0
for /f "tokens=1,2,3" %%i in (%1) do (
if not "%%i"=="" (
if "%%i"=="#etalon_verdict" (
set /a LOC_VERDICT_NUM+=1
if "!LOC_VERDICT_NUM!"=="%2" (
if "%%j"=="%3" (
if "%%k"=="%4" (
rem echo equal to etalon verdicts
exit /b
)
)
)
)
)
)
rem echo NOT equal to etalon verdicts
set /a EQUAL_TO_ETALON_VERDICTS=0

exit /b
rem #######################################


rem #######################################
rem 1-arg - etalon test-case file name
:PRINT_ORIG_ETALON
echo ++++++++++++++++++++++++++++++++++>>%LOG_FILE%
echo Original etalon list:>>%LOG_FILE%
for /f "tokens=1,2,3" %%i in (%1) do (
if not "%%i"=="" (
if "%%i"=="#etalon_verdict" (
echo %%j %%k>>%LOG_FILE%
)
)
)
echo ++++++++++++++++++++++++++++++++++>>%LOG_FILE%
exit /b
rem #######################################

rem #######################################
rem 1-arg - etalon test-case file name
:GET_ETALON_VERDICT_COUNT
rem echo %*
rem echo start ETALON_VERDICT_COUNT=!ETALON_VERDICT_COUNT!
set /a ETALON_VERDICT_COUNT=0
for /f "tokens=1,2,3" %%i in (%1) do (
if not "%%i"=="" (
rem echo i not empty %%i
if "%%i"=="#etalon_verdict" (
rem echo before inc echo ETALON_VERDICT_COUNT

set /a ETALON_VERDICT_COUNT+=1
)
)
)
rem echo end ETALON_VERDICT_COUNT=!ETALON_VERDICT_COUNT!

exit /b
rem #######################################
