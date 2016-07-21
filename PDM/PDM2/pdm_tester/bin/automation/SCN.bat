@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\SCN_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\SCN_2.txt
