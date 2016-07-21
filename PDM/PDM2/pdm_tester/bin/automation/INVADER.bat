@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\INVADER_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\INVADER_2.txt 

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\INVADER_3.txt 

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\INVADER_4.txt 
