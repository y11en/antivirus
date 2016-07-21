set path=C:\Program Files\Microsoft Platform SDK\Bin\win64\x86\AMD64;%PATH%
set lib=C:\Program Files\Microsoft Platform SDK\Lib\AMD64
set include=C:\Program Files\Microsoft Platform SDK\Include\;C:\Program Files\Microsoft Platform SDK\Include\crt;C:\Program Files\Microsoft Platform SDK\Include\atl
nmake /f wmi64.mak CFG="wmi64 - Win32 Release" /A /B