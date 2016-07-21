# Microsoft Developer Studio Generated NMAKE File, Based on ichstrms.dsp
!IF "$(CFG)" == ""
CFG=iCheckerStreams - Win32 Debug
!MESSAGE No configuration specified. Defaulting to iCheckerStreams - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "iCheckerStreams - Win32 ReleaseWithTrace" && "$(CFG)" != "iCheckerStreams - Win32 Release" && "$(CFG)" != "iCheckerStreams - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ichstrms.mak" CFG="iCheckerStreams - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iCheckerStreams - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iCheckerStreams - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iCheckerStreams - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "iCheckerStreams - Win32 ReleaseWithTrace"

OUTDIR=.\../../../out/ReleaseWithTrace
INTDIR=.\../../../temp/ReleaseWithTrace/prague/ISLite/iCheckerStreams
# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

ALL : "$(OUTDIR)\ichstrms.ppl"


CLEAN :
	-@erase "$(INTDIR)\ichecker.obj"
	-@erase "$(INTDIR)\ichstrms_res.res"
	-@erase "$(INTDIR)\plugin_ichstrms.obj"
	-@erase "$(INTDIR)\sfc_api.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ichstrms.exp"
	-@erase "$(OUTDIR)\ichstrms.pdb"
	-@erase "$(OUTDIR)\ichstrms.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fp"$(INTDIR)\ichstrms.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ichstrms_res.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichstrms.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ichstrms.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\ichstrms.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ichecker.obj" \
	"$(INTDIR)\plugin_ichstrms.obj" \
	"$(INTDIR)\sfc_api.obj" \
	"$(INTDIR)\ichstrms_res.res"

"$(OUTDIR)\ichstrms.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\ReleaseWithTrace\ichstrms.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ichstrms.ppl"
   tsigner "\out\ReleaseWithTrace\ichstrms.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "iCheckerStreams - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/ISLite/iCheckerStreams
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\ichstrms.ppl"


CLEAN :
	-@erase "$(INTDIR)\ichecker.obj"
	-@erase "$(INTDIR)\ichstrms_res.res"
	-@erase "$(INTDIR)\plugin_ichstrms.obj"
	-@erase "$(INTDIR)\sfc_api.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ichstrms.exp"
	-@erase "$(OUTDIR)\ichstrms.pdb"
	-@erase "$(OUTDIR)\ichstrms.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ichstrms_res.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichstrms.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /base:"0x5E100000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ichstrms.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ichstrms.ppl" /implib:"$(OUTDIR)\ichstrms.lib" /pdbtype:sept /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\ichecker.obj" \
	"$(INTDIR)\plugin_ichstrms.obj" \
	"$(INTDIR)\sfc_api.obj" \
	"$(INTDIR)\ichstrms_res.res"

"$(OUTDIR)\ichstrms.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ichstrms.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ichstrms.ppl"
   tsigner "\out\Release\ichstrms.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "iCheckerStreams - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/ISLite/iCheckerStreams
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\ichstrms.ppl"


CLEAN :
	-@erase "$(INTDIR)\ichecker.obj"
	-@erase "$(INTDIR)\ichstrms_res.res"
	-@erase "$(INTDIR)\plugin_ichstrms.obj"
	-@erase "$(INTDIR)\sfc_api.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ichstrms.exp"
	-@erase "$(OUTDIR)\ichstrms.ilk"
	-@erase "$(OUTDIR)\ichstrms.pdb"
	-@erase "$(OUTDIR)\ichstrms.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ichstrms_res.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichstrms.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ichstrms.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ichstrms.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ichecker.obj" \
	"$(INTDIR)\plugin_ichstrms.obj" \
	"$(INTDIR)\sfc_api.obj" \
	"$(INTDIR)\ichstrms_res.res"

"$(OUTDIR)\ichstrms.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ichstrms.dep")
!INCLUDE "ichstrms.dep"
!ELSE 
!MESSAGE Warning: cannot find "ichstrms.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "iCheckerStreams - Win32 ReleaseWithTrace" || "$(CFG)" == "iCheckerStreams - Win32 Release" || "$(CFG)" == "iCheckerStreams - Win32 Debug"
SOURCE=.\ichecker.c

"$(INTDIR)\ichecker.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_ichstrms.c

"$(INTDIR)\plugin_ichstrms.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sfc_api.c

"$(INTDIR)\sfc_api.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\ichstrms_res.rc"

"$(INTDIR)\ichstrms_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

