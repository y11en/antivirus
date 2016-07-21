# Microsoft Developer Studio Generated NMAKE File, Based on MultiDMAP.dsp
!IF "$(CFG)" == ""
CFG=MultiDMAP - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MultiDMAP - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MultiDMAP - Win32 Release" && "$(CFG)" != "MultiDMAP - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MultiDMAP.mak" CFG="MultiDMAP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MultiDMAP - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MultiDMAP - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MultiDMAP - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Wrappers/MultiDMAP
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\MDMAP.ppl"


CLEAN :
	-@erase "$(INTDIR)\mdmap.obj"
	-@erase "$(INTDIR)\mdmap.res"
	-@erase "$(INTDIR)\plugin_mdmap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MDMAP.exp"
	-@erase "$(OUTDIR)\MDMAP.pdb"
	-@erase "$(OUTDIR)\MDMAP.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MULTIDMAP_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mdmap.res" /i "..\..\CommonFiles" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MultiDMAP.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x65800000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\MDMAP.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MDMAP.ppl" /implib:"$(OUTDIR)\MDMAP.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\mdmap.obj" \
	"$(INTDIR)\plugin_mdmap.obj" \
	"$(INTDIR)\mdmap.res"

"$(OUTDIR)\MDMAP.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\MDMAP.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MDMAP.ppl"
   call prconvert "\out\Release\MDMAP.ppl"
	tsigner "\out\Release\MDMAP.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MultiDMAP - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Wrappers/MultiDMAP
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\MDMAP.ppl"


CLEAN :
	-@erase "$(INTDIR)\mdmap.obj"
	-@erase "$(INTDIR)\mdmap.res"
	-@erase "$(INTDIR)\plugin_mdmap.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MDMAP.exp"
	-@erase "$(OUTDIR)\MDMAP.ilk"
	-@erase "$(OUTDIR)\MDMAP.map"
	-@erase "$(OUTDIR)\MDMAP.pdb"
	-@erase "$(OUTDIR)\MDMAP.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MULTIDMAP_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\mdmap.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MultiDMAP.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\MDMAP.pdb" /map:"../../../out/Debug/MDMAP.map" /debug /machine:I386 /out:"$(OUTDIR)\MDMAP.ppl" /implib:"$(OUTDIR)\MDMAP.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\mdmap.obj" \
	"$(INTDIR)\plugin_mdmap.obj" \
	"$(INTDIR)\mdmap.res"

"$(OUTDIR)\MDMAP.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("MultiDMAP.dep")
!INCLUDE "MultiDMAP.dep"
!ELSE 
!MESSAGE Warning: cannot find "MultiDMAP.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MultiDMAP - Win32 Release" || "$(CFG)" == "MultiDMAP - Win32 Debug"
SOURCE=.\mdmap.c

"$(INTDIR)\mdmap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_mdmap.c

"$(INTDIR)\plugin_mdmap.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mdmap.rc

"$(INTDIR)\mdmap.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

