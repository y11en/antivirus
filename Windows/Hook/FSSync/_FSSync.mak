#////////////////////////////////////////////////////////////////////
#//
#//  _FSSync.MAK
#//  Project: Prague
#//  Author:  Graf
#//  Copyright (c) Kaspersky Labs, 2004
#//
#//  For use in external workspaces - compile _FSSync
#//
#////////////////////////////////////////////////////////////////////

all:
	@ echo Build _FSSync $(BUILD)...
	@ msdev "../hook.dsw" /MAKE  "FSSync - Win32 $(BUILD)"

rebuild:
	@ echo Rebuild _FSSync $(BUILD)...
	@ msdev "../hook.dsw" /MAKE  "FSSync - Win32 $(BUILD)" /REBUILD 

