@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\SEC_VIOLATION_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SEC_VIOLATION_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SEC_VIOLATION_3.txt
