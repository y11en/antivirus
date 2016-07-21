@echo off

set BUILD_NO=%1

if "%BUILD_NO%"=="" (
  echo Use: publish_kav_release.cmd N.N.N.N
  exit 1
)

set BUILDS_ROOT=\\AVP.RU\Global\Testing\KAV for Windows\8.0\builds
set BUILD_PATH=%BUILDS_ROOT%\%BUILD_NO%

if not exist "%BUILD_PATH%\label.txt" (
  echo Build: %BUILD_NO% not found at %BUILDS_ROOT%
  exit 1
)

set BUILD_RELS_DIR=%BUILD_PATH%\Release\KIS\English

dir /B "%BUILD_RELS_DIR%" >"%TEMP%\_rel_dirs.txt"
for /F "tokens=1" %%i IN (%TEMP%\_rel_dirs.txt) do set REL_DATE_DIR=%%i
del /Q "%TEMP%\_rel_dirs.txt"

if "%REL_DATE_DIR%"=="" (
  echo Error locating release module directory
  exit 1
)

set PACKAGE_PATH=%BUILD_RELS_DIR%\%REL_DATE_DIR%\kis.en.msi
if not exist "%PACKAGE_PATH%" (
  echo Package file not found: %PACKAGE_PATH%
  exit 1
)

perl %~dp0\publish_kav_release.pl "%BUILD_NO%" "%PACKAGE_PATH%"
if errorlevel 1 (
  echo Error publishing product build: %BUILD_NO%
  exit 1
)

echo Product build %BUILD_NO% published successfully
exit 0
