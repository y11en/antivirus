rem *** setting required environment variables
set ssuser=%user_release%
set sspwd=%user_release_pwd%
set ssdir=\\avp.ru\global\sourcesafe
set path=%path%;%programfiles%\Microsoft Visual Studio\Common\VSS\win32

rem *** getting CommonFiles files
rd /s /q "%1\CommonFiles\Release" 2> nul
mkdir "%1\CommonFiles\Release"
rd /s /q "%1\CommonFiles\ReleaseDll" 2> nul
mkdir "%1\CommonFiles\ReleaseDll"
rd /s /q "%1\CommonFiles\Debug" 2> nul
mkdir "%1\CommonFiles\Debug"
rd /s /q "%1\CommonFiles\DebugDll" 2> nul
mkdir "%1\CommonFiles\DebugDll"

ss get "$/Release/Base Technologies 1.0/CommonFiles/Release/tsigner.exe" -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"
ss get "$/Release/Base Technologies 1.0/CommonFiles" -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"
ss get "$/Release/Base Technologies 1.0/CommonFiles/Sign" -R -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"
ss get "$/Release/Base Technologies 1.0/CommonFiles/Stuff" -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"
ss get "$/Release/Base Technologies 1.0/CommonFiles/Version" -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"

rem *** getting KeepUp2Date
ss get "$/Release/Base Technologies 1.0/Components/KeepUp2Date/Client" -R -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"
ss get "$/Release/Base Technologies 1.0/Components/KeepUp2Date/Expat" -R -I-Y -GWS "-VLUpdater SCM4Exchange 1.0.0.%kl_buildno%"

xcopy /E /Y "\Release\Base Technologies 1.0" "\"
rd /S /Q "\Release\"

rem *** creating tsigner configuration file
echo [servers] > %1\CommonFiles\Release\tsigner.ini
echo server_0000 = dump >> %1\Commonfiles\Release\tsigner.ini
echo server_0001 = klspecial >> %1\Commonfiles\Release\tsigner.ini
echo server_0002 = moscow >> %1\Commonfiles\Release\tsigner.ini
echo server_0003 = moscow2 >> %1\Commonfiles\Release\tsigner.ini
echo server_0004 = vintik >> %1\Commonfiles\Release\tsigner.ini