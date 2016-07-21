# Microsoft Developer Studio Generated NMAKE File, Based on loader.dsp
!IF "$(CFG)" == ""
CFG=Loader - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Loader - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Loader - Win32 Release" && "$(CFG)" != "Loader - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "loader.mak" CFG="Loader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Loader - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Loader - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Loader - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/Loader
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\prloader.dll"

!ELSE 

ALL : "stream - Win32 Release" "metadata - Win32 Release" "IFaceInfo - Win32 Release" "$(OUTDIR)\prloader.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"IFaceInfo - Win32 ReleaseCLEAN" "metadata - Win32 ReleaseCLEAN" "stream - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ansi_win.obj"
	-@erase "$(INTDIR)\codec.obj"
	-@erase "$(INTDIR)\compare.obj"
	-@erase "$(INTDIR)\csect_2l.obj"
	-@erase "$(INTDIR)\csection.obj"
	-@erase "$(INTDIR)\cthread.obj"
	-@erase "$(INTDIR)\dbgout.obj"
	-@erase "$(INTDIR)\env_strings.obj"
	-@erase "$(INTDIR)\fm_folder.obj"
	-@erase "$(INTDIR)\fm_str_plist.obj"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\Loader.res"
	-@erase "$(INTDIR)\memmgr.obj"
	-@erase "$(INTDIR)\mod_load.obj"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\oem_win.obj"
	-@erase "$(INTDIR)\ploader.obj"
	-@erase "$(INTDIR)\plugin.obj"
	-@erase "$(INTDIR)\plugin_string.obj"
	-@erase "$(INTDIR)\pprot.obj"
	-@erase "$(INTDIR)\PrDbgInfo.obj"
	-@erase "$(INTDIR)\prop_util.obj"
	-@erase "$(INTDIR)\pstring.obj"
	-@erase "$(INTDIR)\SA.OBJ"
	-@erase "$(INTDIR)\scheck.obj"
	-@erase "$(INTDIR)\scheck_list.obj"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\shareexclusiveresource.obj"
	-@erase "$(INTDIR)\signchk.obj"
	-@erase "$(INTDIR)\str_val.obj"
	-@erase "$(INTDIR)\syncevent.obj"
	-@erase "$(INTDIR)\token.obj"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wheap.obj"
	-@erase "$(INTDIR)\wheap_check.obj"
	-@erase "$(INTDIR)\widechar_win.obj"
	-@erase "$(OUTDIR)\prloader.dll"
	-@erase "$(OUTDIR)\prloader.exp"
	-@erase "$(OUTDIR)\prloader.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I ".\\" /I ".\win32" /I "..\include" /I "..\..\windows\hook" /I "..\..\CommonFiles" /D "NDEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /D "LOADER_BUILD" /D "OLD_METADATA_SUPPORT" /D "ITS_LOADER" /D "NEW_SIGN_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Loader.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\loader.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winavpio.lib swm.lib kldtser.lib win32utils.lib property.lib user32.lib kernel32.lib sign.lib ole32.lib ifaceinfo.lib Advapi32.lib klsys.lib metadata.lib dskm.lib /nologo /base:"0x64d00000" /dll /incremental:no /pdb:"$(OUTDIR)\prloader.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\prloader.dll" /implib:"$(OUTDIR)\prloader.lib" /pdbtype:sept /libpath:"..\..\out\release" /libpath:"..\..\CommonFiles\ReleaseDll" /opt:ref /IGNORE:4098 
LINK32_OBJS= \
	"$(INTDIR)\csect_2l.obj" \
	"$(INTDIR)\csection.obj" \
	"$(INTDIR)\cthread.obj" \
	"$(INTDIR)\dbgout.obj" \
	"$(INTDIR)\env_strings.obj" \
	"$(INTDIR)\fm_folder.obj" \
	"$(INTDIR)\fm_str_plist.obj" \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\memmgr.obj" \
	"$(INTDIR)\mod_load.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\oem_win.obj" \
	"$(INTDIR)\ploader.obj" \
	"$(INTDIR)\plugin.obj" \
	"$(INTDIR)\pprot.obj" \
	"$(INTDIR)\PrDbgInfo.obj" \
	"$(INTDIR)\prop_util.obj" \
	"$(INTDIR)\SA.OBJ" \
	"$(INTDIR)\scheck.obj" \
	"$(INTDIR)\scheck_list.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\shareexclusiveresource.obj" \
	"$(INTDIR)\signchk.obj" \
	"$(INTDIR)\str_val.obj" \
	"$(INTDIR)\syncevent.obj" \
	"$(INTDIR)\token.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\wheap.obj" \
	"$(INTDIR)\wheap_check.obj" \
	"$(INTDIR)\ansi_win.obj" \
	"$(INTDIR)\codec.obj" \
	"$(INTDIR)\compare.obj" \
	"$(INTDIR)\plugin_string.obj" \
	"$(INTDIR)\pstring.obj" \
	"$(INTDIR)\widechar_win.obj" \
	"$(INTDIR)\Loader.res" \
	"..\..\out\Release\enter.obj" \
	"$(OUTDIR)\IFaceInfo.lib" \
	"..\..\out\ReleaseS\metadata.lib" \
	"$(OUTDIR)\stream.lib"

"$(OUTDIR)\prloader.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\prloader.dll
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "stream - Win32 Release" "metadata - Win32 Release" "IFaceInfo - Win32 Release" "$(OUTDIR)\prloader.dll"
   tsigner "\out\Release\prloader.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/Loader
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\prloader.dll" "$(OUTDIR)\loader.bsc"

!ELSE 

ALL : "stream - Win32 Debug" "metadata - Win32 Debug" "IFaceInfo - Win32 Debug" "$(OUTDIR)\prloader.dll" "$(OUTDIR)\loader.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"IFaceInfo - Win32 DebugCLEAN" "metadata - Win32 DebugCLEAN" "stream - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ansi_win.obj"
	-@erase "$(INTDIR)\ansi_win.sbr"
	-@erase "$(INTDIR)\codec.obj"
	-@erase "$(INTDIR)\codec.sbr"
	-@erase "$(INTDIR)\compare.obj"
	-@erase "$(INTDIR)\compare.sbr"
	-@erase "$(INTDIR)\csect_2l.obj"
	-@erase "$(INTDIR)\csect_2l.sbr"
	-@erase "$(INTDIR)\csection.obj"
	-@erase "$(INTDIR)\csection.sbr"
	-@erase "$(INTDIR)\cthread.obj"
	-@erase "$(INTDIR)\cthread.sbr"
	-@erase "$(INTDIR)\dbgout.obj"
	-@erase "$(INTDIR)\dbgout.sbr"
	-@erase "$(INTDIR)\env_strings.obj"
	-@erase "$(INTDIR)\env_strings.sbr"
	-@erase "$(INTDIR)\fm_folder.obj"
	-@erase "$(INTDIR)\fm_folder.sbr"
	-@erase "$(INTDIR)\fm_str_plist.obj"
	-@erase "$(INTDIR)\fm_str_plist.sbr"
	-@erase "$(INTDIR)\loader.obj"
	-@erase "$(INTDIR)\Loader.res"
	-@erase "$(INTDIR)\loader.sbr"
	-@erase "$(INTDIR)\memmgr.obj"
	-@erase "$(INTDIR)\memmgr.sbr"
	-@erase "$(INTDIR)\mod_load.obj"
	-@erase "$(INTDIR)\mod_load.sbr"
	-@erase "$(INTDIR)\mutex.obj"
	-@erase "$(INTDIR)\mutex.sbr"
	-@erase "$(INTDIR)\oem_win.obj"
	-@erase "$(INTDIR)\oem_win.sbr"
	-@erase "$(INTDIR)\ploader.obj"
	-@erase "$(INTDIR)\ploader.sbr"
	-@erase "$(INTDIR)\plugin.obj"
	-@erase "$(INTDIR)\plugin.sbr"
	-@erase "$(INTDIR)\plugin_string.obj"
	-@erase "$(INTDIR)\plugin_string.sbr"
	-@erase "$(INTDIR)\pprot.obj"
	-@erase "$(INTDIR)\pprot.sbr"
	-@erase "$(INTDIR)\PrDbgInfo.obj"
	-@erase "$(INTDIR)\PrDbgInfo.sbr"
	-@erase "$(INTDIR)\prop_util.obj"
	-@erase "$(INTDIR)\prop_util.sbr"
	-@erase "$(INTDIR)\pstring.obj"
	-@erase "$(INTDIR)\pstring.sbr"
	-@erase "$(INTDIR)\SA.OBJ"
	-@erase "$(INTDIR)\SA.SBR"
	-@erase "$(INTDIR)\scheck.obj"
	-@erase "$(INTDIR)\scheck.sbr"
	-@erase "$(INTDIR)\scheck_list.obj"
	-@erase "$(INTDIR)\scheck_list.sbr"
	-@erase "$(INTDIR)\semaphore.obj"
	-@erase "$(INTDIR)\semaphore.sbr"
	-@erase "$(INTDIR)\shareexclusiveresource.obj"
	-@erase "$(INTDIR)\shareexclusiveresource.sbr"
	-@erase "$(INTDIR)\signchk.obj"
	-@erase "$(INTDIR)\signchk.sbr"
	-@erase "$(INTDIR)\str_val.obj"
	-@erase "$(INTDIR)\str_val.sbr"
	-@erase "$(INTDIR)\syncevent.obj"
	-@erase "$(INTDIR)\syncevent.sbr"
	-@erase "$(INTDIR)\token.obj"
	-@erase "$(INTDIR)\token.sbr"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\trace.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wheap.obj"
	-@erase "$(INTDIR)\wheap.sbr"
	-@erase "$(INTDIR)\wheap_check.obj"
	-@erase "$(INTDIR)\wheap_check.sbr"
	-@erase "$(INTDIR)\widechar_win.obj"
	-@erase "$(INTDIR)\widechar_win.sbr"
	-@erase "$(OUTDIR)\loader.bsc"
	-@erase "$(OUTDIR)\prloader.dll"
	-@erase "$(OUTDIR)\prloader.exp"
	-@erase "$(OUTDIR)\prloader.ilk"
	-@erase "$(OUTDIR)\prloader.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I ".\\" /I ".\win32" /I "..\include" /I "..\..\windows\hook" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODULE_EXPORTS" /D "LOADER_BUILD" /D "OLD_METADATA_SUPPORT" /D "ITS_LOADER" /D "NEW_SIGN_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Loader.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\loader.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\csect_2l.sbr" \
	"$(INTDIR)\csection.sbr" \
	"$(INTDIR)\cthread.sbr" \
	"$(INTDIR)\dbgout.sbr" \
	"$(INTDIR)\env_strings.sbr" \
	"$(INTDIR)\fm_folder.sbr" \
	"$(INTDIR)\fm_str_plist.sbr" \
	"$(INTDIR)\loader.sbr" \
	"$(INTDIR)\memmgr.sbr" \
	"$(INTDIR)\mod_load.sbr" \
	"$(INTDIR)\mutex.sbr" \
	"$(INTDIR)\oem_win.sbr" \
	"$(INTDIR)\ploader.sbr" \
	"$(INTDIR)\plugin.sbr" \
	"$(INTDIR)\pprot.sbr" \
	"$(INTDIR)\PrDbgInfo.sbr" \
	"$(INTDIR)\prop_util.sbr" \
	"$(INTDIR)\SA.SBR" \
	"$(INTDIR)\scheck.sbr" \
	"$(INTDIR)\scheck_list.sbr" \
	"$(INTDIR)\semaphore.sbr" \
	"$(INTDIR)\shareexclusiveresource.sbr" \
	"$(INTDIR)\signchk.sbr" \
	"$(INTDIR)\str_val.sbr" \
	"$(INTDIR)\syncevent.sbr" \
	"$(INTDIR)\token.sbr" \
	"$(INTDIR)\trace.sbr" \
	"$(INTDIR)\wheap.sbr" \
	"$(INTDIR)\wheap_check.sbr" \
	"$(INTDIR)\ansi_win.sbr" \
	"$(INTDIR)\codec.sbr" \
	"$(INTDIR)\compare.sbr" \
	"$(INTDIR)\plugin_string.sbr" \
	"$(INTDIR)\pstring.sbr" \
	"$(INTDIR)\widechar_win.sbr"

"$(OUTDIR)\loader.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=swm.lib kldtser.lib property.lib user32.lib kernel32.lib sign.lib ole32.lib ifaceinfo.lib Advapi32.lib klsys.lib metadata.lib dskm.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\prloader.pdb" /debug /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\prloader.dll" /implib:"$(OUTDIR)\prloader.lib" /libpath:"..\..\out\debug" /libpath:"..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\csect_2l.obj" \
	"$(INTDIR)\csection.obj" \
	"$(INTDIR)\cthread.obj" \
	"$(INTDIR)\dbgout.obj" \
	"$(INTDIR)\env_strings.obj" \
	"$(INTDIR)\fm_folder.obj" \
	"$(INTDIR)\fm_str_plist.obj" \
	"$(INTDIR)\loader.obj" \
	"$(INTDIR)\memmgr.obj" \
	"$(INTDIR)\mod_load.obj" \
	"$(INTDIR)\mutex.obj" \
	"$(INTDIR)\oem_win.obj" \
	"$(INTDIR)\ploader.obj" \
	"$(INTDIR)\plugin.obj" \
	"$(INTDIR)\pprot.obj" \
	"$(INTDIR)\PrDbgInfo.obj" \
	"$(INTDIR)\prop_util.obj" \
	"$(INTDIR)\SA.OBJ" \
	"$(INTDIR)\scheck.obj" \
	"$(INTDIR)\scheck_list.obj" \
	"$(INTDIR)\semaphore.obj" \
	"$(INTDIR)\shareexclusiveresource.obj" \
	"$(INTDIR)\signchk.obj" \
	"$(INTDIR)\str_val.obj" \
	"$(INTDIR)\syncevent.obj" \
	"$(INTDIR)\token.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\wheap.obj" \
	"$(INTDIR)\wheap_check.obj" \
	"$(INTDIR)\ansi_win.obj" \
	"$(INTDIR)\codec.obj" \
	"$(INTDIR)\compare.obj" \
	"$(INTDIR)\plugin_string.obj" \
	"$(INTDIR)\pstring.obj" \
	"$(INTDIR)\widechar_win.obj" \
	"$(INTDIR)\Loader.res" \
	"..\..\out\Debug\enter.obj" \
	"$(OUTDIR)\IFaceInfo.lib" \
	"..\..\out\DebugS\metadata.lib" \
	"$(OUTDIR)\stream.lib"

"$(OUTDIR)\prloader.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("loader.dep")
!INCLUDE "loader.dep"
!ELSE 
!MESSAGE Warning: cannot find "loader.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Loader - Win32 Release" || "$(CFG)" == "Loader - Win32 Debug"
SOURCE=.\win32\csect_2l.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\csect_2l.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\csect_2l.obj"	"$(INTDIR)\csect_2l.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\csection.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\csection.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\csection.obj"	"$(INTDIR)\csection.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\cthread.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\cthread.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\cthread.obj"	"$(INTDIR)\cthread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\dbgout.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\dbgout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\dbgout.obj"	"$(INTDIR)\dbgout.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\enter.asm

!IF  "$(CFG)" == "Loader - Win32 Release"

OutDir=.\../../out/Release
InputPath=.\win32\enter.asm
InputName=enter

"..\..\out\Release\enter.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

OutDir=.\../../out/Debug
InputPath=.\win32\enter.asm
InputName=enter

"..\..\out\Debug\enter.obj" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	echo ml /c /coff /Zi /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
	ml /c /coff /Zi /Fo$(OutDir)\$(InputName).obj /FR$(OutDir)\$(InputName).sbr $(InputPath) 
<< 
	

!ENDIF 

SOURCE=.\env_strings.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\env_strings.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\env_strings.obj"	"$(INTDIR)\env_strings.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\fm_folder.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\fm_folder.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\fm_folder.obj"	"$(INTDIR)\fm_folder.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\fm_str_plist.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\fm_str_plist.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\fm_str_plist.obj"	"$(INTDIR)\fm_str_plist.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\loader.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\loader.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\loader.obj"	"$(INTDIR)\loader.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\memmgr.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\memmgr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\memmgr.obj"	"$(INTDIR)\memmgr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\mod_load.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\mod_load.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\mod_load.obj"	"$(INTDIR)\mod_load.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32\mutex.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\mutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\mutex.obj"	"$(INTDIR)\mutex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\string\oem_win.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\oem_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\oem_win.obj"	"$(INTDIR)\oem_win.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\ploader.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\ploader.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\ploader.obj"	"$(INTDIR)\ploader.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\plugin.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\plugin.obj"	"$(INTDIR)\plugin.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32\pprot.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\pprot.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\pprot.obj"	"$(INTDIR)\pprot.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\CodeGen\PrDbgInfo\PrDbgInfo.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\PrDbgInfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\PrDbgInfo.obj"	"$(INTDIR)\PrDbgInfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\prop_util.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\prop_util.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\prop_util.obj"	"$(INTDIR)\prop_util.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\CommonFiles\Service\SA.CPP

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\SA.OBJ" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\SA.OBJ"	"$(INTDIR)\SA.SBR" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\scheck.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\scheck.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\scheck.obj"	"$(INTDIR)\scheck.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\scheck_list.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\scheck_list.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\scheck_list.obj"	"$(INTDIR)\scheck_list.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32\semaphore.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\semaphore.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\semaphore.obj"	"$(INTDIR)\semaphore.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\shareexclusiveresource.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\shareexclusiveresource.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\shareexclusiveresource.obj"	"$(INTDIR)\shareexclusiveresource.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\signchk.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\signchk.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\signchk.obj"	"$(INTDIR)\signchk.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\str_val.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\str_val.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\str_val.obj"	"$(INTDIR)\str_val.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\win32\syncevent.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\syncevent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\syncevent.obj"	"$(INTDIR)\syncevent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\token.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\token.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\token.obj"	"$(INTDIR)\token.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\trace.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\trace.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\trace.obj"	"$(INTDIR)\trace.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\wheap.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\wheap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\wheap.obj"	"$(INTDIR)\wheap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\win32\wheap_check.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\wheap_check.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\wheap_check.obj"	"$(INTDIR)\wheap_check.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Loader.rc

"$(INTDIR)\Loader.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\win32\fm_folder_inc.c
SOURCE=.\win32\loader_inc.c
SOURCE=.\win32\mod_load_inc.c
SOURCE=.\win32\ploader_inc.c
SOURCE=.\win32\plugin_inc.c
SOURCE=.\win32\signchk_inc.c
SOURCE=..\string\ansi_win.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\ansi_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\ansi_win.obj"	"$(INTDIR)\ansi_win.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\string\codec.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\codec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\codec.obj"	"$(INTDIR)\codec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\string\compare.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\compare.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\compare.obj"	"$(INTDIR)\compare.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\string\plugin_string.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\plugin_string.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\plugin_string.obj"	"$(INTDIR)\plugin_string.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\string\pstring.c

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\pstring.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\pstring.obj"	"$(INTDIR)\pstring.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\string\widechar_win.cpp

!IF  "$(CFG)" == "Loader - Win32 Release"


"$(INTDIR)\widechar_win.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"


"$(INTDIR)\widechar_win.obj"	"$(INTDIR)\widechar_win.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "Loader - Win32 Release"

"IFaceInfo - Win32 Release" : 
   cd "\Prague\CodeGen\IFaceInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\IFaceInfo.mak CFG="IFaceInfo - Win32 Release" 
   cd "..\..\loader"

"IFaceInfo - Win32 ReleaseCLEAN" : 
   cd "\Prague\CodeGen\IFaceInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\IFaceInfo.mak CFG="IFaceInfo - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\loader"

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

"IFaceInfo - Win32 Debug" : 
   cd "\Prague\CodeGen\IFaceInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\IFaceInfo.mak CFG="IFaceInfo - Win32 Debug" 
   cd "..\..\loader"

"IFaceInfo - Win32 DebugCLEAN" : 
   cd "\Prague\CodeGen\IFaceInfo"
   $(MAKE) /$(MAKEFLAGS) /F .\IFaceInfo.mak CFG="IFaceInfo - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\loader"

!ENDIF 

!IF  "$(CFG)" == "Loader - Win32 Release"

"metadata - Win32 Release" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Release" 
   cd "..\..\loader"

"metadata - Win32 ReleaseCLEAN" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\loader"

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

"metadata - Win32 Debug" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Debug" 
   cd "..\..\loader"

"metadata - Win32 DebugCLEAN" : 
   cd "\prague\metadata\metadata"
   $(MAKE) /$(MAKEFLAGS) /F .\metadata.mak CFG="metadata - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\loader"

!ENDIF 

!IF  "$(CFG)" == "Loader - Win32 Release"

"stream - Win32 Release" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" 
   cd "..\loader"

"stream - Win32 ReleaseCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Release" RECURSE=1 CLEAN 
   cd "..\loader"

!ELSEIF  "$(CFG)" == "Loader - Win32 Debug"

"stream - Win32 Debug" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" 
   cd "..\loader"

"stream - Win32 DebugCLEAN" : 
   cd "\Prague\stream"
   $(MAKE) /$(MAKEFLAGS) /F .\stream.mak CFG="stream - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\loader"

!ENDIF 


!ENDIF 

