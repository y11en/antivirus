@echo off

rem set NTREETEST=..\Debug\ntree.exe
set NTREETEST=..\..\..\..\out_win32\Debug\ntreetest.exe


for /F "tokens=*" %%i in (tests.lst) do call :perform_test %%i

echo ---
echo NTREE TESTS PASSED
exit /b 0

:perform_test
echo ---
echo performing test: %*
del *.tmp 2>nul
%NTREETEST% %*
if errorlevel 1 goto err_test
del *.tmp 2>nul
exit /b

:err_test
echo NTREE TESTS FAILED
exit 1

