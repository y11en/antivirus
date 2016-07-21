@echo off

echo Saving Parental Control DataBase to text file "%1" ...
cscript.exe //nologo export_pctrl.wsf /server:klava.avp.ru /port:16031 /user:nechaev /password: /library:ParentalControl > %1
echo done.
