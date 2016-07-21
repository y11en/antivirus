rem Usage:    copyfilesplug [<dir>]
rem Examples: copyfilesplug O:\PackagePlug
rem           copyfilesplug

if "%1"=="" (set OUT_DIR=.) else (set OUT_DIR=%1)

set plugin_project=KAVWksConsolePlugin

set SOURCE_DIR=..\..\..\..\..
set TARGET_DIR=%OUT_DIR%\%plugin_project%\_DATA

set XCOPY_CMD=xcopy /Y /R /Q 

if NOT DEFINED OUT_FOLDER set OUT_FOLDER=out

md "%TARGET_DIR%\skin"
md "%TARGET_DIR%\skin\images"
md "%TARGET_DIR%\skin\layout"
md "%TARGET_DIR%\skin\sounds"

md "%TARGET_DIR%\Registry"

md "%TARGET_DIR%\Binary"
md "%TARGET_DIR%\WinNT4"

rem ################
%XCOPY_CMD% "%SOURCE_DIR%\PPP\gui60\skin\*.*" "%TARGET_DIR%\skin\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui60\skin\images\*.*" "%TARGET_DIR%\skin\images"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui60\skin\layout\*.*" "%TARGET_DIR%\skin\layout"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui60\skin\sounds\*.*" "%TARGET_DIR%\skin\sounds"
if ERRORLEVEL 1 goto err

rem ################
%XCOPY_CMD% /S "%SOURCE_DIR%\%OUT_FOLDER%\Release\ap_fs.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% /S "%SOURCE_DIR%\%OUT_FOLDER%\Release\ap_wks.reg" "%TARGET_DIR%\Registry\"
if ERRORLEVEL 1 goto err

rem ################
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\ap_fs.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\ap_wks.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\ap_fsi.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\ap_wksi.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\base64.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\basegui.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\bl.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\crpthlpr.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\FwPresets.ini" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\Inflate.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\lic.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\nfio.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\params.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\prremote.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\PrKernel.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\prloader.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\procmon.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\pxstub.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\resip.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\TempFile.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\thpimpl.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\tm.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\WinReg.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\wmihlpr.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\Release\inifile.ppl" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

rem ################
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\Install\Plugins\msi_misc\Release\msi_misc.dll" "%TARGET_DIR%\Binary\"
if ERRORLEVEL 1 goto err

rem ################
%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\Microsoft.VC80.CRT.manifest" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\msvcm80.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\msvcp80.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\msvcr80.dll" "%TARGET_DIR%\"
if ERRORLEVEL 1 goto err

copy /Y "%SOURCE_DIR%\PPP\Install\redist\x86\Microsoft.VC80.CRT\msvcr80_nt.dll" "%TARGET_DIR%\WinNT4\msvcr80.dll"
if ERRORLEVEL 1 goto err

@goto end
:err
@echo %0(1) : error CMD0000: Build error.
:end
