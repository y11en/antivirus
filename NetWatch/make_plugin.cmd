@echo off

set ROOT=%~dp0..
set IDLC=%ROOT%\Prague\bin\idlc.exe

%IDLC% -I %ROOT%\include\Prague\idl -I %ROOT%\include %*


