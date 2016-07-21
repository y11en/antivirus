#////////////////////////////////////////////////////////////////////
#//
#//  _AVP_IO.MAK
#//  Project: AVP_IO
#//  Author:  Graf
#//  Copyright (c) Kaspersky Labs, 2003
#//
#//  For use in external workspaces - compile AVP_IO with dependances
#//
#//  SS projects required:
#//
#//  $\AVP32\*
#//
#////////////////////////////////////////////////////////////////////

all:
	@ echo Build AVP_IO $(BUILD)...
	@ msdev "../AVP.dsw" /MAKE  "Avp_ioNT - Win32 $(BUILD)"

rebuild:
	@ echo Rebuild AVP_IO $(BUILD)...
	@ msdev "../AVP.dsw" /MAKE  "Avp_ioNT - Win32 $(BUILD)" /REBUILD 

