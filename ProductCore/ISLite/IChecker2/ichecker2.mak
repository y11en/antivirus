# Microsoft Developer Studio Generated NMAKE File, Based on ichecker2.dsp
!IF "$(CFG)" == ""
CFG=IChecker2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to IChecker2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "IChecker2 - Win32 ReleaseWithTrace" && "$(CFG)" != "IChecker2 - Win32 Release" && "$(CFG)" != "IChecker2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ichecker2.mak" CFG="IChecker2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IChecker2 - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IChecker2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IChecker2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "IChecker2 - Win32 ReleaseWithTrace"

OUTDIR=.\../../../out/ReleaseWithTrace
INTDIR=.\../../../temp/ReleaseWithTrace/prague/ISLite/IChecker2
# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

ALL : "$(OUTDIR)\ichecker2.ppl"


CLEAN :
	-@erase "$(INTDIR)\calc_file_hash.obj"
	-@erase "$(INTDIR)\ichecker2.obj"
	-@erase "$(INTDIR)\ichecker2_res.res"
	-@erase "$(INTDIR)\plugin_ichecker2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ichecker2.exp"
	-@erase "$(OUTDIR)\ichecker2.pdb"
	-@erase "$(OUTDIR)\ichecker2.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fp"$(INTDIR)\ichecker2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ichecker2_res.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichecker2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ichecker2.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\ichecker2.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\calc_file_hash.obj" \
	"$(INTDIR)\ichecker2.obj" \
	"$(INTDIR)\plugin_ichecker2.obj" \
	"$(INTDIR)\ichecker2_res.res"

"$(OUTDIR)\ichecker2.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\ReleaseWithTrace\ichecker2.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ichecker2.ppl"
   tsigner "\out\ReleaseWithTrace\ichecker2.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "IChecker2 - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/ISLite/IChecker2
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\ichk2.ppl"


CLEAN :
	-@erase "$(INTDIR)\calc_file_hash.obj"
	-@erase "$(INTDIR)\ichecker2.obj"
	-@erase "$(INTDIR)\ichecker2_res.res"
	-@erase "$(INTDIR)\plugin_ichecker2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ichk2.exp"
	-@erase "$(OUTDIR)\ichk2.pdb"
	-@erase "$(OUTDIR)\ichk2.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ichecker2_res.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichecker2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib chkstk.obj libcmt.lib /nologo /base:"0x64400000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ichk2.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\ichk2.ppl" /implib:"$(OUTDIR)\ichk2.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\calc_file_hash.obj" \
	"$(INTDIR)\ichecker2.obj" \
	"$(INTDIR)\plugin_ichecker2.obj" \
	"$(INTDIR)\ichecker2_res.res"

"$(OUTDIR)\ichk2.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ichk2.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ichk2.ppl"
   call prconvert "\out\Release\ichk2.ppl"
	tsigner "\out\Release\ichk2.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "IChecker2 - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/ISLite/IChecker2
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\ichk2.ppl"


CLEAN :
	-@erase "$(INTDIR)\calc_file_hash.obj"
	-@erase "$(INTDIR)\ichecker2.obj"
	-@erase "$(INTDIR)\ichecker2_res.res"
	-@erase "$(INTDIR)\plugin_ichecker2.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ichk2.exp"
	-@erase "$(OUTDIR)\ichk2.ilk"
	-@erase "$(OUTDIR)\ichk2.pdb"
	-@erase "$(OUTDIR)\ichk2.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ichecker2_res.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichecker2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ichk2.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ichk2.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\calc_file_hash.obj" \
	"$(INTDIR)\ichecker2.obj" \
	"$(INTDIR)\plugin_ichecker2.obj" \
	"$(INTDIR)\ichecker2_res.res"

"$(OUTDIR)\ichk2.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ichecker2.dep")
!INCLUDE "ichecker2.dep"
!ELSE 
!MESSAGE Warning: cannot find "ichecker2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "IChecker2 - Win32 ReleaseWithTrace" || "$(CFG)" == "IChecker2 - Win32 Release" || "$(CFG)" == "IChecker2 - Win32 Debug"
SOURCE=.\calc_file_hash.c

"$(INTDIR)\calc_file_hash.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ichecker2.c

"$(INTDIR)\ichecker2.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_ichecker2.c

"$(INTDIR)\plugin_ichecker2.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\ichecker2_res.rc"

"$(INTDIR)\ichecker2_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

