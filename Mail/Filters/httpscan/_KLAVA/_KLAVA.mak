#////////////////////////////////////////////////////////////////////
#//
#//  _KLAVA.MAK
#//  Project: External Components
#//
#//  For use in external workspaces - compile klaveng.lib
#//
#//  Author:  Vitaly Denisov
#//  Copyright (c) Kaspersky Lab, 2005
#//
#//  SS projects required:
#//
#//  $\klava\kernel\*
#//
#////////////////////////////////////////////////////////////////////

all:
	@ echo Build _KLAVA $(BUILD)...
	@ msdev "../../../../../klava/kernel/kernel.dsw" /MAKE  "_All_ - Win32 $(BUILD)"

rebuild:
	@ echo Rebuild _KLAVA $(BUILD)...
	@ msdev "../../../../../klava/kernel/kernel.dsw" /MAKE  "_All_ - Win32 $(BUILD)" /REBUILD 


