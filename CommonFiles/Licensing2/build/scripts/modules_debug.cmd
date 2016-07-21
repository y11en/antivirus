rem *** Building CommonFiles modules
start /wait /min msdev "\CommonFiles\Dskm\Dskm.dsp" /MAKE "Dskm - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Property\Property.dsp" /MAKE "Property - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Serialize\Serialize.dsp" /MAKE "Serialize - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\SWManager\SWManager.dsp" /MAKE "SWManager - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Windows\Win32Utils\Win32Utils.dsp" /MAKE "Win32Utils - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Windows\WinAvpFF\WinAvpFF.dsp" /MAKE "WinAvpFF - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Windows\WinAVPIO\WinAVPIO.dsp" /MAKE "WinAVPIO - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Licensing2\build\win32\licensing.dsp" /MAKE "Licensing - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Licensing2\test\api\build\win32\LicensingTestApi.dsp" /MAKE "LicensingTestApi - Win32 Debug" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

start /wait /min msdev "\CommonFiles\Licensing2\test\unit\build\win32\licutest.dsp" /MAKE "Licutest - Win32 DebugDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err


goto end

:err
echo Error occured while building project.

:end
