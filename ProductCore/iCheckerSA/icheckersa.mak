# Microsoft Developer Studio Generated NMAKE File, Based on icheckersa.dsp
!IF "$(CFG)" == ""
CFG=iCheckerSA - Win32 Debug
!MESSAGE No configuration specified. Defaulting to iCheckerSA - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "iCheckerSA - Win32 Release" && "$(CFG)" != "iCheckerSA - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "icheckersa.mak" CFG="iCheckerSA - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iCheckerSA - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "iCheckerSA - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iCheckerSA - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\icheckersa.ppl"


CLEAN :
	-@erase "$(INTDIR)\ichecker.obj"
	-@erase "$(INTDIR)\icheckersa_res.res"
	-@erase "$(INTDIR)\plugin_icheckersa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\icheckersa.exp"
	-@erase "$(OUTDIR)\icheckersa.pdb"
	-@erase "$(OUTDIR)\icheckersa.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\include" /I "..\..\prague\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fp"$(INTDIR)\icheckersa.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\icheckersa_res.res" /i "..\..\commonfiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\icheckersa.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\icheckersa.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\icheckersa.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\icheckersa_res.res" \
	"$(INTDIR)\ichecker.obj" \
	"$(INTDIR)\plugin_icheckersa.obj"

"$(OUTDIR)\icheckersa.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\icheckersa.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\icheckersa.ppl"
   call prconvert "\out\Release\icheckersa.ppl"
	tsigner "\out\Release\icheckersa.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "iCheckerSA - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\icheckersa.ppl"


CLEAN :
	-@erase "$(INTDIR)\ichecker.obj"
	-@erase "$(INTDIR)\icheckersa_res.res"
	-@erase "$(INTDIR)\plugin_icheckersa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\icheckersa.exp"
	-@erase "$(OUTDIR)\icheckersa.ilk"
	-@erase "$(OUTDIR)\icheckersa.pdb"
	-@erase "$(OUTDIR)\icheckersa.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\prague\include" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\icheckersa_res.res" /i "..\..\commonfiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\icheckersa.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\icheckersa.pdb" /debug /machine:I386 /out:"$(OUTDIR)\icheckersa.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\icheckersa_res.res" \
	"$(INTDIR)\ichecker.obj" \
	"$(INTDIR)\plugin_icheckersa.obj"

"$(OUTDIR)\icheckersa.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("icheckersa.dep")
!INCLUDE "icheckersa.dep"
!ELSE 
!MESSAGE Warning: cannot find "icheckersa.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "iCheckerSA - Win32 Release" || "$(CFG)" == "iCheckerSA - Win32 Debug"
SOURCE=.\ichecker.cpp

"$(INTDIR)\ichecker.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_icheckersa.cpp

"$(INTDIR)\plugin_icheckersa.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\icheckersa_res.rc"

"$(INTDIR)\icheckersa_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

