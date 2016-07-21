rem Usage:    copyfiles [<dir>]
rem Examples: copyfiles O:\Package
rem           copyfiles

echo copyfiles...

if "%1"=="" (set OUT_DIR=.) else (set OUT_DIR=%1)

set SOURCE_DIR=..\..\..\..\..
if DEFINED SOURCE_DIR_EXT set SOURCE_DIR=%SOURCE_DIR_EXT%

set TARGET_DIR=%OUT_DIR%\DATA
set XCOPY_CMD=xcopy /Y /R /Q 
set MKLIF_VERSION=8.0.0.60
set KLFLTDEV_VERSION=8.0.0.14
set KLBG_VERSION=8.0.5.2
set KL1_VERSION=6.1.28.0
set KLIM5_VERSION=6.1.23.0
set KLIM6_VERSION=6.1.28.0
set MKLIF_PATH=\\avp.ru\global\testing\drivers\mklif\builds\%MKLIF_VERSION%\modules
set KLFLTDEV_PATH=\\avp.ru\global\testing\drivers\klfltdev\builds\%KLFLTDEV_VERSION%\modules
set KLBG_PATH=\\avp.ru\global\testing\drivers\klbg\builds\%KLBG_VERSION%\modules
set KL1_PATH=\\avp.ru\global\testing\drivers\kl1\builds\%KL1_VERSION%\modules
set KLIM5_PATH=\\avp.ru\global\testing\drivers\klim5\builds\%KLIM5_VERSION%\modules
set KLIM6_PATH=\\avp.ru\global\testing\drivers\klim6\builds\%KLIM6_VERSION%\modules
if NOT DEFINED _OUT_FOLDER set _OUT_FOLDER=out
if NOT DEFINED _OUT_FOLDER32 set _OUT_FOLDER32=out_win32
if NOT DEFINED _OUT_FOLDER64 set _OUT_FOLDER64=out_x64
if NOT DEFINED OUT_FOLDER set OUT_FOLDER=out_win32

if %UNZIP%1 EQU 1 set UNZIP=wzunzip

rem set KL1OUT=\\avp.ru\Global\Testing\KAV 4.6\Personal\5.0.520 Release\modules\System Folder
set KL1OUT=%SOURCE_DIR%\ppp\install\kl1

rd /S /Q "%TARGET_DIR%\Program Files\skin\"
md "%TARGET_DIR%\Program Files\skin\"
md "%TARGET_DIR%\Program Files\skin\images"
md "%TARGET_DIR%\Program Files\skin\layout"
md "%TARGET_DIR%\Program Files\skin\sounds"
md "%TARGET_DIR%\Program Files\skin\loc"
md "%TARGET_DIR%\Program Files\MS"
md "%TARGET_DIR%\Program Files\x64\MS"

rem #######  Binary #######
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\msi_misc.dll" "%TARGET_DIR%\Binary\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\UpgradeW.dll" "%TARGET_DIR%\Binary\"
if ERRORLEVEL 1 goto err

if exist "%SOURCE_DIR%\%OUT_FOLDER%\Release\nagrest.exe" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\nagrest.exe" "%TARGET_DIR%\Binary\"
if ERRORLEVEL 1 goto err
)

rem %XCOPY_CMD% "%KLIF_PATH%\Win32\klif.sys" "%TARGET_DIR%\Binary\"
rem if ERRORLEVEL 1 goto err

rem #######  Binary\Cleanapi #######
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\Purifier\*.dll" "%TARGET_DIR%\Binary\cleanapi\"
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\Purifier\*.ini" "%TARGET_DIR%\Binary\cleanapi\"
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\cleanapi.ini" "%TARGET_DIR%\Binary\cleanapi\"
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\cleanapi.exe" "%TARGET_DIR%\Binary\cleanapi\"
if ERRORLEVEL 1 goto err

rem ####### Program Files #######
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\*.ppl" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\*.dll" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\avp.com" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\avp.exe" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\rescue32.exe" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\wmiav.exe" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\wmias.exe" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\wmifw.exe" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\avpgui.tlb" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\kav.bav" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\kav.tbp" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\msdll.txt" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\rescuecd.zip" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ie_banner_deny.htm" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\opera_banner_deny.vbs" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\load46st\load46st.dll" "%TARGET_DIR%\Program Files\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\*.manifest" "%TARGET_DIR%\Program Files\MS\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\*.dll" "%TARGET_DIR%\Program Files\MS\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\dbghelp.dll" "%TARGET_DIR%\Program Files\MS\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\Skin #######
%XCOPY_CMD% "%SOURCE_DIR%\PPP\gui\skin\*.*" "%TARGET_DIR%\Program Files\skin\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\images\*.*" "%TARGET_DIR%\Program Files\skin\images"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\layout\*.*" "%TARGET_DIR%\Program Files\skin\layout"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\sounds\*.*" "%TARGET_DIR%\Program Files\skin\sounds"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\loc\*.*" "%TARGET_DIR%\Program Files\skin\loc"
if ERRORLEVEL 1 goto err

rem ####### Program Files\X64 #######
%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\*.ppl" "%TARGET_DIR%\Program Files\x64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\*.dll" "%TARGET_DIR%\Program Files\x64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\wmi64.exe" "%TARGET_DIR%\Program Files\x64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x64\Microsoft.VC80.CRT\*.manifest" "%TARGET_DIR%\Program Files\x64\MS\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x64\Microsoft.VC80.CRT\*.dll" "%TARGET_DIR%\Program Files\x64\MS\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLIFW2K #######
%XCOPY_CMD% "%MKLIF_PATH%\2k\klif.sys" "%TARGET_DIR%\Program Files\KLIFW2K\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%MKLIF_PATH%\2k\klif.inf" "%TARGET_DIR%\Program Files\KLIFW2K\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%MKLIF_PATH%\2k\klif.cat" "%TARGET_DIR%\Program Files\KLIFW2K\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLIFX86 #######
%XCOPY_CMD% "%MKLIF_PATH%\Win32\klif.sys" "%TARGET_DIR%\Program Files\KLIFX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%MKLIF_PATH%\Win32\klif.inf" "%TARGET_DIR%\Program Files\KLIFX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%MKLIF_PATH%\Win32\klif.cat" "%TARGET_DIR%\Program Files\KLIFX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\out_win32\Release\drvins32.exe" "%TARGET_DIR%\Program Files\KLIFX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\fssync.dll" "%TARGET_DIR%\Program Files\KLIFX86\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLIFX64 #######
%XCOPY_CMD% "%MKLIF_PATH%\x64\klif.sys" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%MKLIF_PATH%\x64\klif.inf" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%MKLIF_PATH%\x64\klif.cat" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\drvins64.exe" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\klogon.dll" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\64Bit\system64.inf" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\klop.dat" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\64Bit\klopx64.inf" "%TARGET_DIR%\Program Files\KLIFX64\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLFLDW2K #######
%XCOPY_CMD% "%KLFLTDEV_PATH%\2k\klfltdev.sys" "%TARGET_DIR%\Program Files\KLFLDW2K\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLFLTDEV_PATH%\2k\klfltdev.inf" "%TARGET_DIR%\Program Files\KLFLDW2K\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLFLTDEV_PATH%\2k\klfltdev.cat" "%TARGET_DIR%\Program Files\KLFLDW2K\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLFLDX86 #######
%XCOPY_CMD% "%KLFLTDEV_PATH%\Win32\klfltdev.sys" "%TARGET_DIR%\Program Files\KLFLDX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLFLTDEV_PATH%\Win32\klfltdev.inf" "%TARGET_DIR%\Program Files\KLFLDX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLFLTDEV_PATH%\Win32\klfltdev.cat" "%TARGET_DIR%\Program Files\KLFLDX86\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLFLDX64 #######
%XCOPY_CMD% "%KLFLTDEV_PATH%\x64\klfltdev.sys" "%TARGET_DIR%\Program Files\KLFLDX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLFLTDEV_PATH%\x64\klfltdev.inf" "%TARGET_DIR%\Program Files\KLFLDX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLFLTDEV_PATH%\x64\klfltdev.cat" "%TARGET_DIR%\Program Files\KLFLDX64\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLBG86 #######
%XCOPY_CMD% "%KLBG_PATH%\Win32\klbg.sys" "%TARGET_DIR%\Program Files\KLBG86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLBG_PATH%\Win32\klbg.inf" "%TARGET_DIR%\Program Files\KLBG86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLBG_PATH%\Win32\klbg.cat" "%TARGET_DIR%\Program Files\KLBG86\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLBG64 #######
%XCOPY_CMD% "%KLBG_PATH%\x64\klbg.sys" "%TARGET_DIR%\Program Files\KLBG64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLBG_PATH%\x64\klbg.inf" "%TARGET_DIR%\Program Files\KLBG64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLBG_PATH%\x64\klbg.cat" "%TARGET_DIR%\Program Files\KLBG64\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KL1X86 #######
%XCOPY_CMD% "%KL1_PATH%\Win32\kl1.sys" "%TARGET_DIR%\Program Files\KL1X86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KL1_PATH%\Win32\kl1.inf" "%TARGET_DIR%\Program Files\KL1X86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KL1_PATH%\Win32\kl1.cat" "%TARGET_DIR%\Program Files\KL1X86\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KL1X64 #######
%XCOPY_CMD% "%KL1_PATH%\x64\kl1.cat" "%TARGET_DIR%\Program Files\KL1X64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KL1_PATH%\x64\kl1.inf" "%TARGET_DIR%\Program Files\KL1X64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KL1_PATH%\x64\kl1.sys" "%TARGET_DIR%\Program Files\KL1X64\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLIMX64\ #######
%XCOPY_CMD% "%KLIM5_PATH%\x64\klim5.cat" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM5_PATH%\x64\klim5.inf" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM5_PATH%\x64\klim5_m.inf" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM5_PATH%\x64\klim5.sys" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%KLIM6_PATH%\x64\klim6.cat" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM6_PATH%\x64\klim6.inf" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM6_PATH%\x64\klim6.sys" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\netcfg.dll" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%_OUT_FOLDER64%\Release\netcfg.exe" "%TARGET_DIR%\Program Files\KLIMX64\"
if ERRORLEVEL 1 goto err

rem ####### Program Files\KLIMX86 #######
%XCOPY_CMD% "%KLIM5_PATH%\Win32\klim5.cat" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM5_PATH%\Win32\klim5.inf" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM5_PATH%\Win32\klim5_m.inf" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM5_PATH%\Win32\klim5.sys" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%KLIM6_PATH%\Win32\klim6.cat" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM6_PATH%\Win32\klim6.inf" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%KLIM6_PATH%\Win32\klim6.sys" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\out_win32\release\netcfg.dll" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\out_win32\release\netcfg.exe" "%TARGET_DIR%\Program Files\KLIMX86\"
if ERRORLEVEL 1 goto err

rem ####### Registry #######
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\msexcl.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_*.local.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_kav*.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_kis*.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_wks*.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_fs*.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_sos*.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_AHI386.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\ppp_AHX64.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err

rem ####### System32 #######
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\klogon.dll" "%TARGET_DIR%\System32\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%KL1_PATH%\Win32\kl1.sys" "%TARGET_DIR%\System32\Drivers\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\xp\klop.dat" "%TARGET_DIR%\System32\Drivers\xp\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\klopp.dat" "%TARGET_DIR%\System32\Drivers\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\BinModules\klnetinf.sys" "%TARGET_DIR%\System32\Drivers\"
if ERRORLEVEL 1 goto err

rem ####### Setup.exe #######
md "%TARGET_DIR%\Bootstrap\KAV"
md "%TARGET_DIR%\Bootstrap\KIS"
md "%TARGET_DIR%\Bootstrap\WKS"
md "%TARGET_DIR%\Bootstrap\FS"
md "%TARGET_DIR%\Bootstrap\SOS"

if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKAV\setup.exe" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKAV\setup.exe" "%TARGET_DIR%\Bootstrap\KAV"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKIS\setup.exe" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKIS\setup.exe" "%TARGET_DIR%\Bootstrap\KIS"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseWKS\setup.exe" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseWKS\setup.exe" "%TARGET_DIR%\Bootstrap\WKS"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseFS\setup.exe" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseFS\setup.exe" "%TARGET_DIR%\Bootstrap\FS"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseSOS\setup.exe" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseSOS\setup.exe" "%TARGET_DIR%\Bootstrap\SOS"
if ERRORLEVEL 1 goto err
)
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\WIX\KAV2006_Installer\Setup\setup.manifest" "%TARGET_DIR%\Bootstrap"
if ERRORLEVEL 1 goto err

md "%TARGET_DIR%\Package\WKS"
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\Package\WKS\installer.ini" "%TARGET_DIR%\Package\WKS"
if ERRORLEVEL 1 goto err
md "%TARGET_DIR%\Package\FS"
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\Package\FS\installer.ini" "%TARGET_DIR%\Package\FS"
if ERRORLEVEL 1 goto err
md "%TARGET_DIR%\Package\SOS"
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\Package\SOS\installer.ini" "%TARGET_DIR%\Package\SOS"
if ERRORLEVEL 1 goto err

exit /b 0

:err
echo %0(1) : error CMD0000: Build error.
exit /b 1
