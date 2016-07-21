
set TOOLS_DIR=..\..\tools

set GENALIAS=%TOOLS_DIR%\genalias\Release\genalias.exe
set GENDATA=%TOOLS_DIR%\gendata\Release\gendata.exe
set GENKUM=%TOOLS_DIR%\genkum\Release\genkum.exe
set KUMTOCNV=%TOOLS_DIR%\kumtocnv\Release\kumtocnv.exe
set GENCASE=%TOOLS_DIR%\gencase\Release\gencase.exe


md out
cd out

%GENCASE% -o=case.kud ..\case.map
echo case.kud >list

%GENALIAS% -o=alias.kud ..\aliases
echo alias.kud >>list


set ENCODINGS=866 1250 1251 1252 1253 1254 1255 1256 1257 1258 932

for %%E in (%ENCODINGS%) do (
%GENKUM% %%E
%KUMTOCNV% windows-%%E.kum
echo windows-%%E.cnv >>list
)

%GENDATA% -o=kulib_l.dat list

cd ..