perl ../createsoapheader.pl stdsoap.h
"../soapcpp2.exe" -1  soapheader.h
erase *.req.xml
erase *.res.xml
perl ../updategsoapsrc.pl
pause
