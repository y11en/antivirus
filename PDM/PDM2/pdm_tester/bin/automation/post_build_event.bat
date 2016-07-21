@echo off

set UserName=Unknown

call :GET_USER_NAME

if "%UserName%"=="martynenko" (
rem path to pdm_test.exe is relatively to project that call post_build_event.bat
copy ..\bin\debug\pdm_test.exe E:\Common\to_wm\pdm_test.exe
)

exit /b

rem #######################
:GET_USER_NAME

for /f "tokens=1,2,16" %%i in ('ipconfig /all') do (
if "%%i"=="Host" (
if "%%j"=="Name" (
if not "%%k"=="" (
set UserName=%%k
)
)
)
)

exit /b
rem #######################