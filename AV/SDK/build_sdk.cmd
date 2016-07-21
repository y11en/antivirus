@echo off

pushd %~dp0

set XBIN=%~dp0\..\..\..\bin
set TGTDIR=%~dp0\..\..\..\AV_SDK

rd /Q/S %~dp0\av_sdk

%XBIN%\getfiles.exe -o %~dp0\av_sdk -s %~dp0\sdk_sources.lst -t SDK %~dp0\sdk_files.lst
if errorlevel 1 goto err

%XBIN%\zip.exe -r9X av_sdk av_sdk
if errorlevel 1 goto err

rd /Q/S %~dp0\av_sdk

%XBIN%\getfiles.exe -o %~dp0\av_sdk -s %~dp0\sdk_sources.lst -t DBG %~dp0\sdk_files.lst
if errorlevel 1 goto err

%XBIN%\zip.exe -r9X av_dbg_sdk av_sdk
if errorlevel 1 goto err

rd /Q/S %~dp0\av_sdk

copy /Y /B %~dp0\av_sdk.zip %TGTDIR%\av_sdk.zip
copy /Y /B %~dp0\av_dbg_sdk.zip %TGTDIR%\av_dbg_sdk.zip

popd
exit /b 0

:err
popd
exit /b 1
