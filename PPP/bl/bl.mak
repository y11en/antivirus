# Microsoft Developer Studio Generated NMAKE File, Based on bl.dsp
!IF "$(CFG)" == ""
CFG=bl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to bl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "bl - Win32 Release" && "$(CFG)" != "bl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bl.mak" CFG="bl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bl - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "bl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "bl - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release/ppp/bl
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : ".\blmsg.rc" "$(OUTDIR)\bl.ppl"


CLEAN :
	-@erase "$(INTDIR)\bl.obj"
	-@erase "$(INTDIR)\bl.res"
	-@erase "$(INTDIR)\blloc.obj"
	-@erase "$(INTDIR)\klop_util.obj"
	-@erase "$(INTDIR)\kloplib.obj"
	-@erase "$(INTDIR)\plugin_bl.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\bl.exp"
	-@erase "$(OUTDIR)\bl.pdb"
	-@erase "$(OUTDIR)\bl.ppl"
	-@erase "blmsg.rc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\prague\include" /I "..\..\commonfiles" /I "..\..\updater60\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /D "KLOPUM_INPLACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\bl.res" /i "..\..\commonfiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib Kernel32.lib advapi32.lib Iphlpapi.lib /nologo /base:"0x62800000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\bl.pdb" /debug /machine:I386 /out:"$(OUTDIR)\bl.ppl" /implib:"$(OUTDIR)\bl.lib" /pdbtype:sept /libpath:"..\..\out\release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\bl.obj" \
	"$(INTDIR)\blloc.obj" \
	"$(INTDIR)\klop_util.obj" \
	"$(INTDIR)\kloplib.obj" \
	"$(INTDIR)\plugin_bl.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\bl.res"

"$(OUTDIR)\bl.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\bl.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : ".\blmsg.rc" "$(OUTDIR)\bl.ppl"
   call prconvert "\out\Release\bl.ppl"
	tsigner "\out\Release\bl.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "bl - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug/ppp/bl
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : ".\blmsg.rc" "$(OUTDIR)\bl.ppl"


CLEAN :
	-@erase "$(INTDIR)\bl.obj"
	-@erase "$(INTDIR)\bl.res"
	-@erase "$(INTDIR)\blloc.obj"
	-@erase "$(INTDIR)\klop_util.obj"
	-@erase "$(INTDIR)\kloplib.obj"
	-@erase "$(INTDIR)\plugin_bl.obj"
	-@erase "$(INTDIR)\update.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\bl.exp"
	-@erase "$(OUTDIR)\bl.ilk"
	-@erase "$(OUTDIR)\bl.pdb"
	-@erase "$(OUTDIR)\bl.ppl"
	-@erase "blmsg.rc"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\prague\include" /I "..\..\commonfiles" /I "..\..\updater60\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /D "KLOPUM_INPLACE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\bl.res" /i "..\..\commonfiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\bl.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib Kernel32.lib advapi32.lib Iphlpapi.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\bl.pdb" /debug /machine:I386 /out:"$(OUTDIR)\bl.ppl" /pdbtype:sept /libpath:"..\..\out\debug" 
LINK32_OBJS= \
	"$(INTDIR)\bl.obj" \
	"$(INTDIR)\blloc.obj" \
	"$(INTDIR)\klop_util.obj" \
	"$(INTDIR)\kloplib.obj" \
	"$(INTDIR)\plugin_bl.obj" \
	"$(INTDIR)\update.obj" \
	"$(INTDIR)\bl.res"

"$(OUTDIR)\bl.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("bl.dep")
!INCLUDE "bl.dep"
!ELSE 
!MESSAGE Warning: cannot find "bl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "bl - Win32 Release" || "$(CFG)" == "bl - Win32 Debug"
SOURCE=.\bl.cpp

"$(INTDIR)\bl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\blloc.cpp

"$(INTDIR)\blloc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\Windows\Hook\Klop\kloplib\klop_util.cpp

"$(INTDIR)\klop_util.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\Windows\Hook\Klop\kloplib\kloplib.cpp

"$(INTDIR)\kloplib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_bl.cpp

"$(INTDIR)\plugin_bl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\update.cpp

"$(INTDIR)\update.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\bl.rc

"$(INTDIR)\bl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\blmsg.mc

!IF  "$(CFG)" == "bl - Win32 Release"

InputPath=.\blmsg.mc
InputName=blmsg

".\blmsg.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	mc $(InputPath)
<< 
	

!ELSEIF  "$(CFG)" == "bl - Win32 Debug"

InputPath=.\blmsg.mc
InputName=blmsg

".\blmsg.rc" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	mc $(InputPath)
<< 
	

!ENDIF 


!ENDIF 

