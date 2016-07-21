rem *** Building CommonFiles modules
start /wait /min msdev "\CommonFiles\Sign\WinSign.dsp" /MAKE "WinSign - Win32 Release" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err
start /wait /min msdev "\CommonFiles\Sign\WinSign.dsp" /MAKE "WinSign - Win32 ReleaseDll" %2 %3 /NORECURSE /OUT %1
if %errorlevel% NEQ 0 goto err

rem *** Building KeepUp2Date modules
start /wait /min msdev "\Components\KeepUp2Date\Client\IUpdater\IUpdater.dsw" /MAKE "IUpdater - Win32 Release" %2 %3 /OUT %1
if %errorlevel% NEQ 0 goto err

goto end

:err
echo Error occured while building project.

:end

