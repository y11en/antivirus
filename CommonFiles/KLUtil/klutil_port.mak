all:
	@ echo Build KLUTIL_PORT $(BUILD)...
	@ msdev "KLUtil.dsp" /MAKE  "KLUtil - Win32 $(BUILD)port"

rebuild:
	@ echo Rebuild KLUTIL_PORT $(BUILD)...
	@ msdev "KLUtil.dsp" /MAKE  "KLUtil - Win32 $(BUILD)port" /REBUILD 
