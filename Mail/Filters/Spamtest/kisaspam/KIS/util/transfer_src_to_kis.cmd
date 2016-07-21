@echo off
cls

SET tag_name=as-20-for-kis-branch-1
SET p=e:\%tag_name%

set SSEXELOCATION=C:\Program Files\Microsoft Visual Studio\Common\VSS\win32\
set SSDIR=\\kl\global\SourceSafe
set SSUSER=smirnov_e
set FORCE_DIR=Yes
set PATH=%PATH%;%SSEXELOCATION%

echo *** CHECKING VSS LOCKING STATUS ***********************************

:CheckVSSStatus
ss Status "$/ppp/mailcommon/filters/spamtest/kisaspam" -R -O-
if not errorlevel 1 goto bLockStatusOk
echo There are some files locked in the VSS
echo Commit them or break (Ctrl-C)
pause
goto CheckVSSStatus
:bLockStatusOk



echo *** EMPTY PROJECT FOLDER ******************************************

if not exist %p% goto create_dir
	rd /S /Q %p%
	if not exist %p% goto :bRDOK
		echo Can't remove directory.
		echo %p%
		pause
		exit 1
	:bRDOK
:create_dir
md %p%
copy excl_src_to_kis.lst %p%
cd %p%

echo *** CHECKING OUT PROJECT FROM VSS *********************************

ss Checkout -R "$/ppp/mailcommon/filters/spamtest/kisaspam/" -I- -O-

echo *** GETTING ANTISPAM-SOURCES FROM CVS *****************************

md CVS_FILES
cd CVS_FILES
cvs get -r %tag_name% gnu            >> cvs_get.log
cvs get -r %tag_name% smtp-filter    >> cvs_get.log
cvs get -r %tag_name% lib            >> cvs_get.log
cvs get -r %tag_name% MIMEParser     >> cvs_get.log
REM cvs get -r %tag_name% kis            >> cvs_get.log
cvs get -r %tag_name% client-filter  >> cvs_get.log
cvs get -r %tag_name% _include       >> cvs_get.log
cvs get -r %tag_name% Filtration     >> cvs_get.log
cvs get -r %tag_name% libmorph       >> cvs_get.log
cvs get -r %tag_name% ZTools         >> cvs_get.log

echo *** CVS CHANGE LOG ************************************************

kis\util\util.exe printf_str_and_file "cvs diff -bu -r transfer_to_KIS_" kis\util\transfer.num > cvs_changes.cmd
call cvs_changes.cmd > cvs_changes.log
call notepad cvs_changes.log
echo close cvs_changes.log
pause

echo *** VSS CHANGE LOG ************************************************

kis\util\util.exe printf_str_and_file "ss History $/ppp/mailcommon/filters/spamtest/kisaspam/ -R -B  -Ovss_changes.log -V~Lv" kis\util\transfer.num > vss_changes.cmd
call vss_changes.cmd
call notepad vss_changes.log
echo close vss_changes.log
pause

echo *** COPY FILES ****************************************************

xcopy /S /Y /EXCLUDE:..\excl_src_to_kis.lst * ..\

cd ..
echo *** CHECKING IN PROJECT TO VSS **********************************

ss Checkin "$/ppp/mailcommon/filters/spamtest/kisaspam/" -R -I-

CVS_FILES\kis\util\util.exe increment_version_sec CVS_FILES\kis\util\transfer.num 2 0 +
CVS_FILES\kis\util\util.exe printf_str_and_file "ss label $/ppp/mailcommon/filters/spamtest/kisaspam/ -I- -Lv" CVS_FILES\kis\util\transfer.num > label_vss.cmd
call label_vss.cmd
CVS_FILES\kis\util\util.exe printf_str_and_file "cvs tag transfer_to_KIS_" CVS_FILES\kis\util\transfer.num > CVS_FILES\label_cvs.cmd
cd CVS_FILES
cd kis\util\
cvs ci -m "" transfer.num
cd ..\..\
call label_cvs.cmd
cd ..

echo *** FINISH FILES **************************************************
