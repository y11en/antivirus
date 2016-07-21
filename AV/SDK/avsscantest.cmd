@echo off
"%~dp0\..\..\out_win32\release\avsscan.exe" -a C:\Bases\avp_x.set -r c:\Bases\toScan\ > C:\Bases\result\avsscan_current.log

fc C:\Bases\result\avsscan_current.log C:\Bases\result\avsscan_results.log
exit /b %errorlevel%
