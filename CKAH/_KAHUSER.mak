#////////////////////////////////////////////////////////////////////
#//
#//  _KAHUSER.MAK
#//  Project: External Components
#//
#//  For use in external workspaces - compile CKAHUM
#//
#//  Author:  Nikolay Lebedev
#//  Copyright (c) Kaspersky Lab, 2004
#//
#//  SS projects required:
#//
#//  $\CommonFiles\*
#//  $\Windows\Hook\*
#//  $\Components\Windows\KAH
#//
#////////////////////////////////////////////////////////////////////

all:
	@ echo Build CHKAHUM $(BUILD)...
	@ msdev "KAH.dsw" /MAKE  "Ckahum - Win32 $(BUILD)"

rebuild:
	@ echo Rebuild CHKAHUM $(BUILD)...
	@ msdev "KAH.dsw" /MAKE  "Ckahum - Win32 $(BUILD)" /REBUILD 


