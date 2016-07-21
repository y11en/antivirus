# Microsoft Developer Studio Generated NMAKE File, Based on iwgen.dsp
!IF "$(CFG)" == ""
CFG=IWGen - Win32 Debug
!MESSAGE No configuration specified. Defaulting to IWGen - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "IWGen - Win32 Release" && "$(CFG)" != "IWGen - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iwgen.mak" CFG="IWGen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IWGen - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "IWGen - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "IWGen - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\..\..\..\temp/Release/prague/iwgen
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\iwgen.ppl"


CLEAN :
	-@erase "$(INTDIR)\converter.obj"
	-@erase "$(INTDIR)\iwgen_res.res"
	-@erase "$(INTDIR)\plugin_iwgen.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\iwgen.exp"
	-@erase "$(OUTDIR)\iwgen.pdb"
	-@erase "$(OUTDIR)\iwgen.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\iwgen_res.res" /i "..\..\include" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iwgen.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib libcmt.lib /nologo /base:"0x64980000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\iwgen.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\iwgen.ppl" /implib:"$(OUTDIR)\iwgen.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\converter.obj" \
	"$(INTDIR)\plugin_iwgen.obj" \
	"$(INTDIR)\iwgen_res.res"

"$(OUTDIR)\iwgen.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\iwgen.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\iwgen.ppl"
   tsigner "\out\Release\iwgen.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "IWGen - Win32 Debug"

OUTDIR=.\..\..\..\out/Debug
INTDIR=.\..\..\..\temp/Debug/prague/iwgen
# Begin Custom Macros
OutDir=.\..\..\..\out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\iwgen.ppl"


CLEAN :
	-@erase "$(INTDIR)\converter.obj"
	-@erase "$(INTDIR)\iwgen_res.res"
	-@erase "$(INTDIR)\plugin_iwgen.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\iwgen.exp"
	-@erase "$(OUTDIR)\iwgen.ilk"
	-@erase "$(OUTDIR)\iwgen.pdb"
	-@erase "$(OUTDIR)\iwgen.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\iwgen_res.res" /i "..\..\include" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\iwgen.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\iwgen.pdb" /debug /machine:I386 /out:"$(OUTDIR)\iwgen.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\converter.obj" \
	"$(INTDIR)\plugin_iwgen.obj" \
	"$(INTDIR)\iwgen_res.res"

"$(OUTDIR)\iwgen.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("iwgen.dep")
!INCLUDE "iwgen.dep"
!ELSE 
!MESSAGE Warning: cannot find "iwgen.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "IWGen - Win32 Release" || "$(CFG)" == "IWGen - Win32 Debug"
SOURCE=.\converter.cpp

"$(INTDIR)\converter.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_iwgen.cpp

"$(INTDIR)\plugin_iwgen.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\iwgen_res.rc"

"$(INTDIR)\iwgen_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

