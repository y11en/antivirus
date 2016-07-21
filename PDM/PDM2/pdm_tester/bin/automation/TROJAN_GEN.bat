@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\TROJAN_GEN_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\TROJAN_GEN_2.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\TROJAN_GEN_3.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\TROJAN_GEN_4.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\TROJAN_GEN_5.txt

