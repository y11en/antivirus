rem Usage:    copyfileslocplug <locname> <skin> <outname>         [<dir>]
rem Examples: copyfileslocplug Dutch     nl     Dutch\<TIMESTAMP> O:\PackagePlg
rem           copyfileslocplug Russian   ru     Russian

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err

set plugin_project=KAVSOSConsolePlugin

set loc_name=%1
set loc_skin_name=%2
set loc_out_name=%3

if "%4"=="" (set OUT_DIR=.) else (set OUT_DIR=%4)

set SOURCE_DIR=..\..\..\..\..
set TARGET_DIR=%OUT_DIR%\%plugin_project%\_DATALOC\%loc_out_name%
set XCOPY_CMD=xcopy /Y /R /Q 

if NOT DEFINED OUT_FOLDER set OUT_FOLDER=out

md "%TARGET_DIR%\skin\%loc_skin_name%"
md "%TARGET_DIR%\Doc"

rem ################
if not "%loc_skin_name%" == "en" (
md "%TARGET_DIR%\skin\en"

%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui60\skin\en\*.*" "%TARGET_DIR%\skin\en"
if ERRORLEVEL 1 goto err
)

%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui60\skin\%loc_skin_name%\*.*" "%TARGET_DIR%\skin\%loc_skin_name%"
if ERRORLEVEL 1 goto err

rem ################
copy /Y "%SOURCE_DIR%\PPP\gui60\HelpPlugin\%loc_name%\SOS\context.chm" "%TARGET_DIR%\Doc\context_sos.chm"
if ERRORLEVEL 1 goto err


@goto end
:err
@echo %0(1) : error CMD0000: Build error.
:end
