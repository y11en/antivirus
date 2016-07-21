@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_SEND_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_SEND_2.txt

