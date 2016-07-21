@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\DRIVER_INS_1_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\DRIVER_INS_1_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\DRIVER_INS_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\DRIVER_INS_3.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\DRIVER_INS_4.txt
