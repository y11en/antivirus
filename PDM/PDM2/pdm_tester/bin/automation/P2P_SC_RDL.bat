@echo off

echo "Press key to start first case"
pause
call debug\pdm_test.exe TestCaseParams\P2P_SC_RDL_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\P2P_SC_RDL_2_1.txt

echo "Press key to start next case"
pause
call debug\pdm_test.exe TestCaseParams\P2P_SC_RDL_2_2.txt
