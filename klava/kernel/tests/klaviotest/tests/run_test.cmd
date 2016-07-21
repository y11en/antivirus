@echo off

set KLAV_BIN=..\..\..\..\..\Out_win32\release

del *.tmp 2>nul

echo --- running test: io_read

%KLAV_BIN%\klaviotest.exe io_data1.tst io_read.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: io_write

%KLAV_BIN%\klaviotest.exe io_write.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: vio_read (0)

%KLAV_BIN%\klaviotest.exe vio_data1.tst vio_read.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: vio_read (0x100)

%KLAV_BIN%\klaviotest.exe vio_data1a.tst vio_read.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: vio_write (0)

%KLAV_BIN%\klaviotest.exe vio_data1.tst vio_write.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: vio_write (0x100)

%KLAV_BIN%\klaviotest.exe vio_data1a.tst vio_write.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: vio_chsize (0)

%KLAV_BIN%\klaviotest.exe vio_data1.tst vio_chsize.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo --- running test: vio_chsize (0x100)

%KLAV_BIN%\klaviotest.exe vio_data1a.tst vio_chsize.tst
if errorlevel 1 goto err_test
del *.tmp 2>nul

echo IO TESTS PASSED
exit /b 0

:err_test
echo IO TESTS FAILED
exit /b 1


