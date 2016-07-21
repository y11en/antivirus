# Microsoft Developer Studio Generated NMAKE File, Based on CAB.dsp
!IF "$(CFG)" == ""
CFG=CAB - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CAB - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CAB - Win32 Release" && "$(CFG)" != "CAB - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CAB.mak" CFG="CAB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CAB - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CAB - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "CAB - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/MiniArchiver/CAB
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\CAB.ppl"


CLEAN :
	-@erase "$(INTDIR)\CAB.obj"
	-@erase "$(INTDIR)\CAB.res"
	-@erase "$(INTDIR)\extra.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CAB.exp"
	-@erase "$(OUTDIR)\CAB.pdb"
	-@erase "$(OUTDIR)\CAB.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "../../../CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAB_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CAB.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CAB.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib /nologo /base:"0x62c00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\CAB.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\CAB.ppl" /implib:"$(OUTDIR)\CAB.lib" /pdbtype:sept /libpath:"../../../CommonFiles/releaseDll" /IGNORE:4086 /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\CAB.obj" \
	"$(INTDIR)\extra.obj" \
	"$(INTDIR)\CAB.res"

"$(OUTDIR)\CAB.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\CAB.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\CAB.ppl"
   call prconvert "\out\Release\CAB.ppl"
	tsigner "\out\Release\CAB.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "CAB - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/MiniArchiver/CAB
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\CAB.ppl"


CLEAN :
	-@erase "$(INTDIR)\CAB.obj"
	-@erase "$(INTDIR)\CAB.res"
	-@erase "$(INTDIR)\extra.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\CAB.exp"
	-@erase "$(OUTDIR)\CAB.ilk"
	-@erase "$(OUTDIR)\CAB.pdb"
	-@erase "$(OUTDIR)\CAB.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "../../../CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAB_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\CAB.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CAB.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=m_utils.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\CAB.pdb" /debug /machine:I386 /out:"$(OUTDIR)\CAB.ppl" /implib:"$(OUTDIR)\CAB.lib" /pdbtype:sept /libpath:"../../../commonfiles/debugDll" 
LINK32_OBJS= \
	"$(INTDIR)\CAB.obj" \
	"$(INTDIR)\extra.obj" \
	"$(INTDIR)\CAB.res"

"$(OUTDIR)\CAB.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("CAB.dep")
!INCLUDE "CAB.dep"
!ELSE 
!MESSAGE Warning: cannot find "CAB.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CAB - Win32 Release" || "$(CFG)" == "CAB - Win32 Debug"
SOURCE=.\CAB.c

"$(INTDIR)\CAB.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\extra.c

"$(INTDIR)\extra.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\CAB.rc

"$(INTDIR)\CAB.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

