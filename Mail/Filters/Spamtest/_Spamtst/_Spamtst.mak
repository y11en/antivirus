#////////////////////////////////////////////////////////////////////
#//
#//  _Spamtst.MAK
#//  Project: External Components
#//
#//  For use in external workspaces - compile Samtst.ppl
#//
#//  Author:  Vitaly Denisov
#//  Copyright (c) Kaspersky Lab, 2005
#//
#//  SS projects required:
#//
#//  $\ppp\MailCommon\Filters\*
#//
#////////////////////////////////////////////////////////////////////

all:
	@ echo Build Spamtest $(BUILD)...
	@ msdev "../Spamtest.dsw" /MAKE  "Spamtest - Win32 $(BUILD)"

rebuild:
	@ echo Rebuild Spamtest $(BUILD)...
	@ msdev "../Spamtest.dsw" /MAKE  "Spamtest - Win32 $(BUILD)" /REBUILD 


