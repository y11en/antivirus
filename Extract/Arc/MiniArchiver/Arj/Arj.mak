# Microsoft Developer Studio Generated NMAKE File, Based on Arj.dsp
!IF "$(CFG)" == ""
CFG=Arj - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Arj - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Arj - Win32 Release" && "$(CFG)" != "Arj - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Arj.mak" CFG="Arj - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Arj - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Arj - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Arj - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/MiniArchiver/Arj
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\Arj.ppl"


CLEAN :
	-@erase "$(INTDIR)\arj.obj"
	-@erase "$(INTDIR)\arj.res"
	-@erase "$(INTDIR)\extra.obj"
	-@erase "$(INTDIR)\plugin_arj.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Arj.exp"
	-@erase "$(OUTDIR)\Arj.pdb"
	-@erase "$(OUTDIR)\Arj.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARJ_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\arj.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Arj.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=hashutil.lib /nologo /base:"0x61900000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\Arj.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Arj.ppl" /implib:"$(OUTDIR)\Arj.lib" /libpath:"../../../commonfiles/releasedll" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\arj.obj" \
	"$(INTDIR)\extra.obj" \
	"$(INTDIR)\plugin_arj.obj" \
	"$(INTDIR)\arj.res"

"$(OUTDIR)\Arj.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\Arj.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Arj.ppl"
   call prconvert "\out\Release\Arj.ppl"
	tsigner "\out\Release\Arj.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Arj - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/MiniArchiver/Arj
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\Arj.ppl"


CLEAN :
	-@erase "$(INTDIR)\arj.obj"
	-@erase "$(INTDIR)\arj.res"
	-@erase "$(INTDIR)\extra.obj"
	-@erase "$(INTDIR)\plugin_arj.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Arj.exp"
	-@erase "$(OUTDIR)\Arj.ilk"
	-@erase "$(OUTDIR)\Arj.pdb"
	-@erase "$(OUTDIR)\Arj.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARJ_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\arj.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Arj.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=hashutil.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Arj.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Arj.ppl" /implib:"$(OUTDIR)\Arj.lib" /pdbtype:sept /libpath:"../../../commonfiles/debugdll" 
LINK32_OBJS= \
	"$(INTDIR)\arj.obj" \
	"$(INTDIR)\extra.obj" \
	"$(INTDIR)\plugin_arj.obj" \
	"$(INTDIR)\arj.res"

"$(OUTDIR)\Arj.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Arj.dep")
!INCLUDE "Arj.dep"
!ELSE 
!MESSAGE Warning: cannot find "Arj.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Arj - Win32 Release" || "$(CFG)" == "Arj - Win32 Debug"
SOURCE=.\arj.c

"$(INTDIR)\arj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\extra.c

"$(INTDIR)\extra.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_arj.c

"$(INTDIR)\plugin_arj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\arj.rc

"$(INTDIR)\arj.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

