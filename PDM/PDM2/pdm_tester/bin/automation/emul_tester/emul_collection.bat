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

set TestCasesPath=%1
set DestPath=E:\Work\HeuristicTester\Bin

rd Logs /Q /S
mkdir Logs

set TOTAL_COUNT=0

for /R %TestCasesPath% %%i in (*.*) do (
set /a TOTAL_COUNT+=1
)


for /R %TestCasesPath% %%i in (*.*) do (
echo !TOTAL_COUNT! files left
call emul_pdm_test.bat %%i
set /a TOTAL_COUNT-=1
)

call parce_emul_log.bat %1

set LOG_FOLDER=%DATE%_%TIME:~0,2%.%TIME:~3,2%.%TIME:~6,2%
mkdir %LOG_FOLDER%

copy logs\*.* %LOG_FOLDER%\*.*

rd Logs /Q /S


