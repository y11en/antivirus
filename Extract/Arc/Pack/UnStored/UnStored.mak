# Microsoft Developer Studio Generated NMAKE File, Based on UnStored.dsp
!IF "$(CFG)" == ""
CFG=UnStored - Win32 Debug
!MESSAGE No configuration specified. Defaulting to UnStored - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UnStored - Win32 Release" && "$(CFG)" != "UnStored - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnStored.mak" CFG="UnStored - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnStored - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UnStored - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "UnStored - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Extract/UnStored
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\UnStored.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_unstore.obj"
	-@erase "$(INTDIR)\unstored.res"
	-@erase "$(INTDIR)\unstored_t.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnStored.exp"
	-@erase "$(OUTDIR)\UnStored.pdb"
	-@erase "$(OUTDIR)\UnStored.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNSTORED_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\unstored.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnStored.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x68a00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\UnStored.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnStored.ppl" /implib:"$(OUTDIR)\UnStored.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_unstore.obj" \
	"$(INTDIR)\unstored_t.obj" \
	"$(INTDIR)\unstored.res"

"$(OUTDIR)\UnStored.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\UnStored.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\UnStored.ppl"
   call prconvert "\out\Release\UnStored.ppl"
	tsigner "\out\Release\UnStored.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "UnStored - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Extract/UnStored
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\UnStored.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_unstore.obj"
	-@erase "$(INTDIR)\unstored.res"
	-@erase "$(INTDIR)\unstored_t.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnStored.exp"
	-@erase "$(OUTDIR)\UnStored.ilk"
	-@erase "$(OUTDIR)\UnStored.pdb"
	-@erase "$(OUTDIR)\UnStored.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNSTORED_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\unstored.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnStored.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\UnStored.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnStored.ppl" /implib:"$(OUTDIR)\UnStored.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_unstore.obj" \
	"$(INTDIR)\unstored_t.obj" \
	"$(INTDIR)\unstored.res"

"$(OUTDIR)\UnStored.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("UnStored.dep")
!INCLUDE "UnStored.dep"
!ELSE 
!MESSAGE Warning: cannot find "UnStored.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "UnStored - Win32 Release" || "$(CFG)" == "UnStored - Win32 Debug"
SOURCE=.\plugin_unstore.c

"$(INTDIR)\plugin_unstore.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unstored_t.c

"$(INTDIR)\unstored_t.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unstored.rc

"$(INTDIR)\unstored.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

