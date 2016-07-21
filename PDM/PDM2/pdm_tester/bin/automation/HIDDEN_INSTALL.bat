@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_INSTALL_1_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_INSTALL_1_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_INSTALL_2_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_INSTALL_2_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\HIDDEN_INSTALL_3.txt

