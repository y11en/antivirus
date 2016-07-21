@if not defined build_echo echo off

if "%1" == "" goto err
if "%2" == "" goto err
if "%3" == "" goto err
if "%4" == "" goto err
if "%5" == "" goto err

call ver.cmd

set wix_bin=wixbin
set msxsl_bin=wixbin
set rlman_bin=wixbin

if not defined mt (
set mt=mt
)

set short_name=%1
set product_status=%2
set localization_name=%3
set package_product_name=%4
set localization_short_name=%5
if "%6"=="" (set IncludeAVBases=Bases) else (set IncludeAVBases=NoBases)

if "%out_path%" == "" (
set out_path=out
)

if not defined REGMERGER (
set REGMERGER=wixbin\regmerger.exe
)

set product_type=%short_name%
set res_path=res
set data_path=data
set dataloc_path=dataloc\%localization_name%
set localization_path=localization\%localization_name%
set localization_product_path=localization\%localization_name%\%product_type%
set temp_path=temp

set msi_package_name=%package_product_name%.%localization_short_name%.msi

call %localization_product_path%\set.cmd
if ERRORLEVEL 1 goto err

echo Building (%msi_package_name%)...

rem **** create necessary folders
md %out_path%

rd /s /q %temp_path%
md %temp_path%

rem **** sign files
if defined SIGNTOOL (
echo Signing modules...
%SIGNTOOL% /d %product_name% "%data_path%\Program Files\wmiav.exe"
if ERRORLEVEL 1 goto err
%SIGNTOOL% /d %product_name% "%data_path%\Program Files\wmias.exe"
if ERRORLEVEL 1 goto err
%SIGNTOOL% /d %product_name% "%data_path%\Program Files\wmifw.exe"
if ERRORLEVEL 1 goto err
%SIGNTOOL% /d %product_name% "%data_path%\Program Files\rescue32.exe"
if ERRORLEVEL 1 goto err
%SIGNTOOL% /d %product_name% "%data_path%\Binary\cleanapi\cleanapi.exe"
if ERRORLEVEL 1 goto err
)

rem **** prepare AVS bases files list
echo Prepare AV bases files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Bases\AVS >%temp_path%\avs_bases_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\avs_bases_files.tmp mk_plain_files_list.xsl -o %temp_path%\avs_bases_files.wxi -u 4.0 IdPrefix=AVS_Bases_
if ERRORLEVEL 1 goto err

rem **** prepare AS bases files list
echo Prepare AS bases files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Bases\AS >%temp_path%\as_bases_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\as_bases_files.tmp mk_plain_files_list.xsl -o %temp_path%\as_bases_files.wxi -u 4.0 IdPrefix=AS_Bases_
if ERRORLEVEL 1 goto err

rem **** prepare Anti-Hacker i386 bases files list
echo Prepare Anti-Hacker bases files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Bases\AHI386 >%temp_path%\ahi386_bases_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ahi386_bases_files.tmp mk_plain_files_list.xsl -o %temp_path%\ahi386_bases_files.wxi -u 4.0 IdPrefix=AHI386_Bases_
if ERRORLEVEL 1 goto err

rem **** prepare Anti-Hacker x64 bases files list
echo Prepare Anti-Hacker x64 bases files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Bases\AHX64 >%temp_path%\ahx64_bases_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ahx64_bases_files.tmp mk_plain_files_list.xsl -o %temp_path%\ahx64_bases_files.wxi -u 4.0 IdPrefix=AHX64_Bases_
if ERRORLEVEL 1 goto err

rem **** prepare AVZ bases files list
echo Prepare AV bases files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Bases\AVZ >%temp_path%\avz_bases_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\avz_bases_files.tmp mk_plain_files_list.xsl -o %temp_path%\avz_bases_files.wxi -u 4.0 IdPrefix=AVZ_Bases_
if ERRORLEVEL 1 goto err

rem **** prepare retr localization files list
echo Prepare retr localization files list...
if "%localization_short_name%"=="en" (
%wix_bin%\tallow.exe -nologo -d "%dataloc_path%\CommonAppData\Data" -df retr.dat -dd en >%temp_path%\retr_dat_loc.tmp
) else (
%wix_bin%\tallow.exe -nologo -d "%dataloc_path%\CommonAppData\Data" -df retr.dat -dd en -dd %localization_short_name% >%temp_path%\retr_dat_loc.tmp
)
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\retr_dat_loc.tmp mk_retr_dat.xsl -o %temp_path%\retr_dat_loc.wxi -u 4.0
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\retr_dat_loc.tmp mk_retr_dat_refs.xsl -o %temp_path%\retr_dat_refs_loc.wxi -u 4.0
if ERRORLEVEL 1 goto err

rem **** Prepare dskm files list
echo Prepare dskm files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Dskm >%temp_path%\dskm_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\dskm_files.tmp mk_plain_files_list.xsl -o %temp_path%\dskm_files.wxi -u 4.0 IdPrefix=DSKM_
if ERRORLEVEL 1 goto err

rem **** Prepare Anti-Hacker i386 dskm files list
echo Prepare Anti-Hacker i386 dskm files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Dskm\AHI386 >%temp_path%\ahi386_dskm_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ahi386_dskm_files.tmp mk_plain_files_list.xsl -o %temp_path%\ahi386_dskm_files.wxi -u 4.0 IdPrefix=AHI386_DSKM_
if ERRORLEVEL 1 goto err

rem **** Prepare Anti-Hacker x64 dskm files list
echo Prepare Anti-Hacker x64 dskm files list...
%wix_bin%\tallow.exe -nologo -d %dataloc_path%\CommonAppData\Dskm\AHX64 >%temp_path%\ahx64_dskm_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ahx64_dskm_files.tmp mk_plain_files_list.xsl -o %temp_path%\ahx64_dskm_files.wxi -u 4.0 IdPrefix=AHX64_DSKM_
if ERRORLEVEL 1 goto err

rem **** prepare GUI skins files list
echo Prepare GUI skins files list...
md "%temp_path%\skin"
xcopy /s /y /r "%data_path%\Program Files\skin\*.*" "%temp_path%\skin">nul
if ERRORLEVEL 1 goto err
xcopy /s /y /r "%dataloc_path%\Program Files\skinover\*.*" "%temp_path%\skin">nul

%wix_bin%\tallow.exe -nologo -d "%temp_path%\skin" -dd tasks -dd tray -dd vkbd -dd images -dd layout -dd sounds >%temp_path%\gui_skins.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\gui_skins.tmp mk_gui_skins.xsl -o %temp_path%\gui_skins.wxi -u 4.0
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\gui_skins.tmp mk_gui_skins_refs.xsl -o %temp_path%\gui_skins_refs.wxi -u 4.0
if ERRORLEVEL 1 goto err

rem **** prepare GUI skins localization files list
echo Prepare GUI skins localization files list...
if "%localization_short_name%"=="en" (
%wix_bin%\tallow.exe -nologo -d "%dataloc_path%\Program Files\skin" >%temp_path%\gui_skins_loc.tmp
) else (
%wix_bin%\tallow.exe -nologo -d "%dataloc_path%\Program Files\skin" >%temp_path%\gui_skins_loc.tmp
)
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\gui_skins_loc.tmp mk_gui_skinsloc.xsl -o %temp_path%\gui_skins_loc.wxi -u 4.0
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\gui_skins_loc.tmp mk_gui_skinsloc_refs.xsl -o %temp_path%\gui_skins_refs_loc.wxi -u 4.0
if ERRORLEVEL 1 goto err

rem **** process ppp.reg file
echo Process ppp.reg file...
if EXIST "%data_path%\Registry\ppp_%localization_short_name%.local.reg" (
  if DEFINED REGMERGER (
    %REGMERGER% "%data_path%\Registry\ppp_%localization_short_name%.reg" "%data_path%\Registry\ppp.reg" "%data_path%\Registry\ppp_%localization_short_name%.local.reg"
    if ERRORLEVEL 1 goto err
    %wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_%localization_short_name%.reg -o %temp_path%\ppp_reg_extract.tmp
    if ERRORLEVEL 1 goto err
  ) else (
    echo REGMERGER not defined
    goto err
  )
) else (
  %wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp.reg -o %temp_path%\ppp_reg_extract.tmp
  if ERRORLEVEL 1 goto err
)
%msxsl_bin%\msxsl.exe %temp_path%\ppp_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_reg_extract.wxi -u 4.0 IdPrefix=ppp_reg_
if ERRORLEVEL 1 goto err

rem **** process ppp_%short_name%.reg file
echo Process ppp_%short_name%.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_%short_name%.reg >%temp_path%\ppp_%short_name%_reg_extract.tmp
rem if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_%short_name%_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_%short_name%_reg_extract.wxi -u 4.0 IdPrefix=ppp_%short_name%_reg_
rem if ERRORLEVEL 1 goto err

rem **** process ppp_%short_name%_otheros.reg file
echo Process ppp_%short_name%_otheros.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_%short_name%_otheros.reg >%temp_path%\ppp_%short_name%_otheros_reg_extract.tmp
rem if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_%short_name%_otheros_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_%short_name%_otheros_reg_extract.wxi -u 4.0 IdPrefix=ppp_%short_name%_otheros_reg_
rem if ERRORLEVEL 1 goto err

rem **** process ppp_%short_name%_xpx64.reg file
echo Process ppp_%short_name%_xpx64.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_%short_name%_xpx64.reg >%temp_path%\ppp_%short_name%_xpx64_reg_extract.tmp
rem if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_%short_name%_xpx64_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_%short_name%_xpx64_reg_extract.wxi -u 4.0 IdPrefix=ppp_%short_name%_xpx64_reg_
rem if ERRORLEVEL 1 goto err

rem **** process ppp_%short_name%_vista.reg file
echo Process ppp_%short_name%_vista.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_%short_name%_vista.reg >%temp_path%\ppp_%short_name%_vista_reg_extract.tmp
rem if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_%short_name%_vista_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_%short_name%_vista_reg_extract.wxi -u 4.0 IdPrefix=ppp_%short_name%_vista_reg_
rem if ERRORLEVEL 1 goto err

rem **** process ppp_%short_name%_vistax64.reg file
echo Process ppp_%short_name%_vistax64.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_%short_name%_vistax64.reg >%temp_path%\ppp_%short_name%_vistax64_reg_extract.tmp
rem if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_%short_name%_vistax64_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_%short_name%_vistax64_reg_extract.wxi -u 4.0 IdPrefix=ppp_%short_name%_vistax64_reg_
rem if ERRORLEVEL 1 goto err

rem **** process ppp_AHI386.reg file
echo Process ppp_AHI386.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_AHI386.reg >%temp_path%\ppp_AHI386_reg_extract.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_AHI386_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_AHI386_reg_extract.wxi -u 4.0 IdPrefix=ppp_AHI386_reg_
if ERRORLEVEL 1 goto err

rem **** process ppp_AHX64.reg file
echo Process ppp_AHX64.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\ppp_AHX64.reg >%temp_path%\ppp_AHX64_reg_extract.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\ppp_AHX64_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\ppp_AHX64_reg_extract.wxi -u 4.0 IdPrefix=ppp_AHX64_reg_
if ERRORLEVEL 1 goto err

rem **** process msexcl.reg file
echo Process msexcl.reg file...
%wix_bin%\tallow.exe -nologo -reg %data_path%\Registry\msexcl.reg >%temp_path%\msexcl_reg_extract.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\msexcl_reg_extract.tmp mk_registry_extract.xsl -o %temp_path%\msexcl_reg_extract.wxi -u 4.0 IdPrefix=msexcl_reg_
if ERRORLEVEL 1 goto err

rem **** prepare Binary files list
echo Prepare Binary files list...
%wix_bin%\tallow.exe -nologo -d %data_path%\Binary\Cleanapi >%temp_path%\cleanapi_files.tmp
if ERRORLEVEL 1 goto err
%msxsl_bin%\msxsl.exe %temp_path%\cleanapi_files.tmp mk_binary_files_list.xsl -o %temp_path%\cleanapi_files.wxi -u 4.0 
if ERRORLEVEL 1 goto err

rem **** prepare kpd file
echo Prepare kpd file...
del /f /q "%temp_path%\kav.kpd"
if exist "%localization_product_path%\kav.kpd" (
xcopy /y /r "%localization_product_path%\kav.kpd" "%temp_path%"
attrib -R "%temp_path%\kav.kpd"
echo PluginVersion=%PRODUCT_VERSION_STR% >> "%temp_path%\kav.kpd"
echo DisplayVersion=%PRODUCT_VERSION_STR% >> "%temp_path%\kav.kpd"
)

del /f /q "%temp_path%\uninstall.kpd"
if exist "%localization_product_path%\uninstall.kpd" (
xcopy /y /r "%localization_product_path%\uninstall.kpd" "%temp_path%"
attrib -R "%temp_path%\uninstall.kpd"
echo PluginVersion=%PRODUCT_VERSION_STR% >> "%temp_path%\uninstall.kpd"
echo DisplayVersion=%PRODUCT_VERSION_STR% >> "%temp_path%\uninstall.kpd"
)

rem **** compiling all *.wxs files (%msi_package_name%)...
echo Compiling WiX files...
%wix_bin%\candle.exe -nologo *.wxs -dProductStatus="%product_status%" -out %temp_path%\
if ERRORLEVEL 1 goto err

rem **** Creating MSI package 
echo Creating MSI package (%msi_package_name%)...
%wix_bin%\light.exe -nologo %temp_path%\*.wixobj -loc %localization_product_path%\main.wxl -loc %localization_path%\ui_common.wxl -out "%temp_path%\%msi_package_name%"
if ERRORLEVEL 1 goto err

if defined SIGNTOOL (
%SIGNTOOL% /d %product_name% "%temp_path%\%msi_package_name%"
if ERRORLEVEL 1 goto err
)

rem **** Creating bootstrapper
echo Creating bootstrapper
if not exist "%dataloc_path%\Bootstrap\%short_name%\setupl.dll" (
echo %dataloc_path%\Bootstrap\%short_name%\setupl.dll not found.
goto err
)
if not exist "%data_path%\Bootstrap\%short_name%\setup.exe" (
echo %data_path%\Bootstrap\%short_name%\setup.exe not found.
goto err
)

%rlman_bin%\rlman.exe -w -e "%dataloc_path%\Bootstrap\%short_name%\setupl.dll" "%temp_path%\setup.res"
%rlman_bin%\rlman.exe -w -r "%data_path%\Bootstrap\%short_name%\setup.exe" "%temp_path%\setup.res" "%temp_path%\setup.exe"
%mt% -manifest "%data_path%\Bootstrap\setup.manifest" -outputresource:"%temp_path%\setup.exe"

if defined SIGNTOOL (
%SIGNTOOL% /d %product_name% "%temp_path%\setup.exe"
if ERRORLEVEL 1 goto err
)


rem **** copy to out
echo Copying to out
xcopy /y /r "%temp_path%\%msi_package_name%" "%out_path%"
xcopy /y /r "%temp_path%\setup.exe" "%out_path%"
if exist "%temp_path%\kav.kpd" (
xcopy /y /r "%temp_path%\kav.kpd" "%out_path%"
)
if exist "%data_path%\Package\%short_name%\installer.ini" (
xcopy /y /r "%data_path%\Package\%short_name%\installer.ini" "%out_path%"
)

echo Project build process finished successfully. (%msi_package_name%)

goto end
:err
echo %0(1) : error CMD0000: Build error. (%msi_package_name%)
:end
