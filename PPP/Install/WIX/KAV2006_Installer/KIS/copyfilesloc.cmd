rem Usage:    copyfilesloc <locname> <antispam> <skin> <outname>        [<dir>]
rem Examples: copyfilesloc Dutch     English    nl     Dutch\<TIMESTAMP> O:\Package
rem           copyfilesloc Russian   Russian    ru     Russian

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err
if "%4" == "" goto err

set loc_name=%1
set loc_as_in_name=%2
set loc_skin_name=%3
set loc_out_name=%4

if "%5"=="" (set OUT_DIR=.) else (set OUT_DIR=%5)

set SOURCE_DIR=..\..\..\..\..
if DEFINED SOURCE_DIR_EXT set SOURCE_DIR=%SOURCE_DIR_EXT%

set TARGET_DIR=%OUT_DIR%\DATALOC\%loc_out_name%
set XCOPY_CMD=xcopy /Y /R /Q 
set KRGX=wixbin\krgx

set SERVER=\\VINTIK\DFS\Updates\dit
set BASES=%SERVER%\release
rem set BASES_KDB=%BASES%\bases\av\klava\kdb\i386
rem set BASES_KDB=\\klava-link\kdb
set BASES_KDB=%SERVER%\inbound\bases\av\klava\kdb\i386
set BASES_IDSI386=%BASES%\bases\ids\i386
set BASES_IDSX64=%BASES%\bases\ids\x64
set BASES_AS=%BASES%\bases\as\pas
set INDEX=%BASES%\index\6

if NOT DEFINED _OUT_FOLDER set _OUT_FOLDER=out
if NOT DEFINED _OUT_FOLDER32 set _OUT_FOLDER32=out_win32
if NOT DEFINED _OUT_FOLDER64 set _OUT_FOLDER64=out_x64
if NOT DEFINED OUT_FOLDER set OUT_FOLDER=out_win32

rd /S /Q "%TARGET_DIR%\Program Files\skin\loc"
md "%TARGET_DIR%\Program Files\skin\loc"
%XCOPY_CMD% "%SOURCE_DIR%\PPP\gui\skin\loc\*.*" "%TARGET_DIR%\Program Files\skin\loc"
if ERRORLEVEL 1 goto err

if not "%loc_skin_name%" == "en" (
rd /S /Q "%TARGET_DIR%\Program Files\skin\loc\en"
md "%TARGET_DIR%\Program Files\skin\loc\en"

%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\loc\en\*.*" "%TARGET_DIR%\Program Files\skin\loc\en"
if ERRORLEVEL 1 goto err
)

rd /S /Q "%TARGET_DIR%\Program Files\skin\loc\%loc_skin_name%"
md "%TARGET_DIR%\Program Files\skin\loc\%loc_skin_name%"

%XCOPY_CMD% "%SOURCE_DIR%\PPP\gui\skin\loc\%loc_skin_name%\*.*" "%TARGET_DIR%\Program Files\skin\loc\%loc_skin_name%"
if ERRORLEVEL 1 goto err

if exist "%SOURCE_DIR%\PPP\gui\skin\loc\%loc_skin_name%\images" (
md "%TARGET_DIR%\Program Files\skin\loc\%loc_skin_name%\images"
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\loc\%loc_skin_name%\images\*.*" "%TARGET_DIR%\Program Files\skin\loc\%loc_skin_name%\images"
if ERRORLEVEL 1 goto err
)

if exist "%SOURCE_DIR%\PPP\gui\skin\loc\%loc_skin_name%\skin" (
md "%TARGET_DIR%\Program Files\skinover"
%XCOPY_CMD% /S "%SOURCE_DIR%\PPP\gui\skin\loc\%loc_skin_name%\skin\*.*" "%TARGET_DIR%\Program Files\skinover"
if ERRORLEVEL 1 goto err
)

rd /S /Q "%TARGET_DIR%\CommonAppData\Data\"
rd /S /Q "%TARGET_DIR%\CommonAppData\Bases\"

rem ####### Bases #######

rem ### CORE

md "%TARGET_DIR%\CommonAppData\Data\en"
md "%TARGET_DIR%\CommonAppData\Bases"

%XCOPY_CMD% "%INDEX%\u0607g.xml" "%TARGET_DIR%\CommonAppData\Data\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\upd\updcfg.xml"  "%TARGET_DIR%\CommonAppData\Data\"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\upd\statcfg.xml" "%TARGET_DIR%\CommonAppData\Data\"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%BASES%\bases\blst\black.lst" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\blst\blst-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### Updater

%XCOPY_CMD% "%BASES%\bases\upd\upd-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### RT

%XCOPY_CMD% "%BASES%\bases\rt\rt-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%BASES%\bases\rt\en\retr.dat" "%TARGET_DIR%\CommonAppData\Data\en"
if ERRORLEVEL 1 goto err

if not "%loc_skin_name%" == "en" (
md "%TARGET_DIR%\CommonAppData\Data\%loc_skin_name%"

%XCOPY_CMD% "%BASES%\bases\rt\%loc_skin_name%\retr.dat" "%TARGET_DIR%\CommonAppData\Data\%loc_skin_name%"
)

rem ### News

%XCOPY_CMD% "%BASES%\bases\info\*.kln" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

%XCOPY_CMD% "%BASES%\bases\info\info-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### AVS

md "%TARGET_DIR%\CommonAppData\Bases\AVS"

%XCOPY_CMD% "%BASES_KDB%\*.kdc" "%TARGET_DIR%\CommonAppData\Bases\AVS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_KDB%\*.kdl" "%TARGET_DIR%\CommonAppData\Bases\AVS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_KDB%\*.mft" "%TARGET_DIR%\CommonAppData\Bases\AVS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_KDB%\kdb-*.xml" "%TARGET_DIR%\CommonAppData\Bases\AVS"
if ERRORLEVEL 1 goto err

rem ### WAI386

%XCOPY_CMD% "%BASES%\bases\av\klava\wa\i386\*.kdl" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\av\klava\wa\i386\*.kfb" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\av\klava\wa\i386\*.dat" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\av\klava\wa\i386\wa-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### AP,ADB

%XCOPY_CMD% "%BASES%\bases\aspy\aphish.dat" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\aspy\adblock.txt" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\aspy\aspy-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err


rem ### RM, BB

%XCOPY_CMD% "%BASES%\bases\pdm\pdmkl.dat" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\pdm\regmonitor.ini" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\pdm\pdm-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### PCDB

%XCOPY_CMD% "%BASES%\bases\pcdb\*.dat" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\pcdb\pcdb-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### AHI386

%XCOPY_CMD% "%BASES%\bases\ids\i386\fwpresets.ini" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

md "%TARGET_DIR%\CommonAppData\Bases\AHI386"
%XCOPY_CMD% "%BASES%\bases\ids\i386\*.kdz" "%TARGET_DIR%\CommonAppData\Bases\AHI386"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\ids\i386\*.set" "%TARGET_DIR%\CommonAppData\Bases\AHI386"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\ids\i386\ah-*.xml" "%TARGET_DIR%\CommonAppData\Bases\AHI386"
if ERRORLEVEL 1 goto err

rem ### AHX64

md "%TARGET_DIR%\CommonAppData\Bases\AHX64"
%XCOPY_CMD% "%BASES%\bases\ids\x64\*.kdz" "%TARGET_DIR%\CommonAppData\Bases\AHX64"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\ids\x64\*.set" "%TARGET_DIR%\CommonAppData\Bases\AHX64"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\ids\x64\ah-*.xml" "%TARGET_DIR%\CommonAppData\Bases\AHX64"
if ERRORLEVEL 1 goto err

rem ### AS

md "%TARGET_DIR%\CommonAppData\Bases\AS"
%XCOPY_CMD% "%BASES_AS%\*.gsg" "%TARGET_DIR%\CommonAppData\Bases\AS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_AS%\*.trm" "%TARGET_DIR%\CommonAppData\Bases\AS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_AS%\*.pdb" "%TARGET_DIR%\CommonAppData\Bases\AS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_AS%\*.set" "%TARGET_DIR%\CommonAppData\Bases\AS"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES_AS%\pas-*.xml" "%TARGET_DIR%\CommonAppData\Bases\AS"
if ERRORLEVEL 1 goto err

rem ### KAV8EXEC

%XCOPY_CMD% "%BASES%\AutoPatches\kav6\kav6-*.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### HIPS DB

rem %XCOPY_CMD% "%BASES%\bases\hips\hipsdb.xml" "%TARGET_DIR%\CommonAppData\Bases"
%XCOPY_CMD% "%SOURCE_DIR%\UpdatesRobot\DataFiles\hipsdb.xml\dit\hipsdb.xml" "%TARGET_DIR%\CommonAppData\Bases"
if ERRORLEVEL 1 goto err

rem ### AVZ DB

md "%TARGET_DIR%\CommonAppData\Bases\AVZ"

%XCOPY_CMD% "%BASES%\bases\ssa\*.avz" "%TARGET_DIR%\CommonAppData\Bases\AVZ"
if ERRORLEVEL 1 goto err
%XCOPY_CMD% "%BASES%\bases\ssa\ssa-*.xml" "%TARGET_DIR%\CommonAppData\Bases\AVZ"
if ERRORLEVEL 1 goto err

rd /S /Q "%TARGET_DIR%\CommonAppData\Dskm\"
md "%TARGET_DIR%\CommonAppData\Dskm\AHI386"
md "%TARGET_DIR%\CommonAppData\Dskm\AHX64"

rem ####### Dskm #######
for %%i in (%TARGET_DIR%\CommonAppData\Data\*.xml) do (
%KRGX% %%i "%TARGET_DIR%\CommonAppData\Dskm"\
if ERRORLEVEL 1 goto err
)
for %%i in (%TARGET_DIR%\CommonAppData\Bases\*.xml) do (
%KRGX% %%i "%TARGET_DIR%\CommonAppData\Dskm"\
if ERRORLEVEL 1 goto err
)
for %%i in (%TARGET_DIR%\CommonAppData\Bases\AVS\*.xml) do (
%KRGX% %%i "%TARGET_DIR%\CommonAppData\Dskm"\
if ERRORLEVEL 1 goto err
)
for %%i in (%TARGET_DIR%\CommonAppData\Bases\AHI386\*.xml) do (
%KRGX% %%i "%TARGET_DIR%\CommonAppData\Dskm\AHI386"\
if ERRORLEVEL 1 goto err
)
for %%i in (%TARGET_DIR%\CommonAppData\Bases\AHX64\*.xml) do (
%KRGX% %%i "%TARGET_DIR%\CommonAppData\Dskm\AHX64"\
if ERRORLEVEL 1 goto err
)

rem ####### Data #######
call x_copy "%SOURCE_DIR%\PPP\Install\as\[50 SPAM %loc_as_in_name%]antispam.sfdb" "%TARGET_DIR%\CommonAppData\Data\" "antispam.sfdb"
if ERRORLEVEL 1 goto err

rem ################ CHM  ################ 
if exist "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\KIS\guide.chm" (
call x_copy "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\KIS\guide.chm" "%TARGET_DIR%\Program Files\Doc\KIS\" "Context.chm"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\KAV\guide.chm" (
call x_copy "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\KAV\guide.chm" "%TARGET_DIR%\Program Files\Doc\KAV\" "Context.chm"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\WKS\guide.chm" (
call x_copy "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\WKS\guide.chm" "%TARGET_DIR%\Program Files\Doc\WKS\" "Context.chm"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\FS\guide.chm" (
call x_copy "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\FS\guide.chm" "%TARGET_DIR%\Program Files\Doc\FS\" "Context.chm"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\SOS\guide.chm" (
call x_copy "%SOURCE_DIR%\PPP\GUI\Help\%loc_name%\SOS\guide.chm" "%TARGET_DIR%\Program Files\Doc\SOS\" "Context.chm"
if ERRORLEVEL 1 goto err
)

rem ####### Setup.exe localization #######
md "%TARGET_DIR%\Bootstrap\KAV"
md "%TARGET_DIR%\Bootstrap\KIS"
md "%TARGET_DIR%\Bootstrap\WKS"
md "%TARGET_DIR%\Bootstrap\FS"
md "%TARGET_DIR%\Bootstrap\SOS"

if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKAV\setupl.dll" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKAV\setupl.dll" "%TARGET_DIR%\Bootstrap\KAV"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKIS\setupl.dll" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseKIS\setupl.dll" "%TARGET_DIR%\Bootstrap\KIS"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseWKS\setupl.dll" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseWKS\setupl.dll" "%TARGET_DIR%\Bootstrap\WKS"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseFS\setupl.dll" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseFS\setupl.dll" "%TARGET_DIR%\Bootstrap\FS"
if ERRORLEVEL 1 goto err
)
if exist "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseSOS\setupl.dll" (
%XCOPY_CMD% "%SOURCE_DIR%\%OUT_FOLDER%\ReleaseSOS\setupl.dll" "%TARGET_DIR%\Bootstrap\SOS"
if ERRORLEVEL 1 goto err
)

exit /b 0

:err
echo %0(1) : error CMD0000: Build error.
exit /b 1
