rem *** setting required environment variables
set ssuser=%user_release%
set sspwd=%user_release_pwd%
set ssdir=\\avp.ru\global\sourcesafe
set path=%path%;%programfiles%\Microsoft Visual Studio\Common\VSS\win32

ss get "$/commonfiles" -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/Dskm" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/Licensing2" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/LoadKeys" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/Property" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/Serialize" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/Stuff" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/StuffIO" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/SWManager" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/windows" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
ss get "$/commonfiles/Version" -R -I-Y -GWS "-VLLicensing 2.0.0.%kl_buildno%"
xcopy /E /Y "D:\ProductParts\Licensing" "\"

rem *** creating tsigner configuration file
rem echo [servers] > %1\CommonFiles\Release\tsigner.ini
rem echo server_0000 = dump >> %1\Commonfiles\Release\tsigner.ini
rem echo server_0001 = klspecial >> %1\Commonfiles\Release\tsigner.ini
rem echo server_0002 = moscow >> %1\Commonfiles\Release\tsigner.ini
rem echo server_0003 = moscow2 >> %1\Commonfiles\Release\tsigner.ini
rem echo server_0004 = vintik >> %1\Commonfiles\Release\tsigner.ini