@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\SC_AR_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SC_AR_2_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SC_AR_2_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SC_AR_3.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SC_AR_4.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SC_AR_5.txt

