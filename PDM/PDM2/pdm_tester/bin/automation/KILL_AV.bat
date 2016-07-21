
@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\KILL_AV_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\KILL_AV_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\KILL_AV_3.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\KILL_AV_4.txt


