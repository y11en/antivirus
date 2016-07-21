pushd %~dp0\..\AVZ\Projects\SysInfo\avz-avp
call _build.bat
copy avzkrnl.dll "%~dp0\%1"
popd

pushd %~dp0\..\AVZ\Projects\SysInfo\db_compile
call _build.bat
call avz_db_compile.bat
popd
