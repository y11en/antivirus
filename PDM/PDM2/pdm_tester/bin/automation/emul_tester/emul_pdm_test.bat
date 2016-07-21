set EmulatorPath=..\..\..\..\..\..\out_win32\debug\imul.exe
set PdmTesterFilesPath=..\..\debug
%EmulatorPath% -l3 Logs\%~n1.log ^
	-e c:\test.txt %1 ^
	-e c:\test_exe.exe %PdmTesterFilesPath%\test_exe.exe ^
	-e c:\set_hooker.exe %PdmTesterFilesPath%\set_hooker.exe ^
	-e c:\test_dll1.dll %PdmTesterFilesPath%\test_dll.dll ^
	-e c:\test_sys.sys %PdmTesterFilesPath%\test_sys.sys ^
	-e c:\test_reg.dat %PdmTesterFilesPath%\test_reg.dat ^
	-e c:\test_doc.doc %PdmTesterFilesPath%\test_doc.doc ^
	-e "c:\program files\Kaspersky Lab\Kaspersky Internet Security 6.0\avp.exe" %PdmTesterFilesPath%\test_exe.exe ^
	-e "c:\program files\mcafee.com\personal firewall\MPFService.exe" %PdmTesterFilesPath%\test_exe.exe ^
	-e "c:\program files\Eset\nod32krn.exe" %PdmTesterFilesPath%\test_exe.exe ^
	-e c:\WINDOWS\system32\MPR.DLL %PdmTesterFilesPath%\set_hooker.exe ^
	-e c:\WINDOWS\system32\WINMM.DLL %PdmTesterFilesPath%\set_hooker.exe ^
	-e c:\WINDOWS\system32\WINSPOOL.DLL %PdmTesterFilesPath%\set_hooker.exe ^
	-e c:\WINDOWS\system32\ODBC32.DLL %PdmTesterFilesPath%\set_hooker.exe ^
	-e c:\WINDOWS\system32\PSTOREC.DLL %PdmTesterFilesPath%\set_hooker.exe ^
	%PdmTesterFilesPath%\pdm_test.exe







