"../soapcpp2.exe" -1  soapheader.h

rmdir /S /Q .\xml
rmdir /S /Q .\txt
rmdir /S /Q .\gsoap

mkdir .\xml
mkdir .\txt
mkdir .\gsoap

xcopy .\*.xml .\xml\*.xml
xcopy .\*.xsd .\xml\*.xsd
xcopy .\*.wsdl .\xml\*.wsdl
erase *.xml
erase *.xsd
erase *.wsdl

copy .\mobile.txt .\txt\mobile.txt
copy .\soapheader.h .\gsoap\soapheader.h


pause
