rmdir /S /Q .\include
rmdir /S /Q .\lib
perl -w make_sdk.pl
mkdir .\lib
mkdir .\lib\debug
mkdir .\lib\release
copy /B ..\lib\debug\klcsstd.lib .\lib\debug\klcsstd.lib
copy /B ..\lib\debug\klcskca.lib .\lib\debug\klcskca.lib
copy /B ..\lib\release\klcsstd.lib .\lib\release\klcsstd.lib
copy /B ..\lib\release\klcskca.lib .\lib\release\klcskca.lib
pause
