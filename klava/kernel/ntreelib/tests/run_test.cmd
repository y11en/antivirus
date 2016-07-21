@echo off

if not "%1"=="" goto argok
echo error: test file name not specified
exit /b 1
:argok

rem set NTREETEST=..\Debug\ntree.exe
set NTREETEST=..\..\..\..\out_win32\Debug\ntreetest.exe

echo ---
echo performing test: %1

del *.tmp 2>nul
%NTREETEST% "%1"
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo ---
echo NTREE TESTS PASSED
exit /b 0

exit /b

:err_test
echo NTREE TESTS FAILED
exit 1

