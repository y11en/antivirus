# Microsoft Developer Studio Generated NMAKE File, Based on xorio.dsp
!IF "$(CFG)" == ""
CFG=xorio - Win32 Debug
!MESSAGE No configuration specified. Defaulting to xorio - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xorio - Win32 Release" && "$(CFG)" != "xorio - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xorio.mak" CFG="xorio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xorio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "xorio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "xorio - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Wrappers/xorio
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\xorio.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\plugin_xorio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xorio.res"
	-@erase "$(OUTDIR)\xorio.exp"
	-@erase "$(OUTDIR)\xorio.pdb"
	-@erase "$(OUTDIR)\xorio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XORIO_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\xorio.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xorio.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=hashutil.lib /nologo /base:"0x69700000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\xorio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\xorio.ppl" /implib:"$(OUTDIR)\xorio.lib" /libpath:"../../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\plugin_xorio.obj" \
	"$(INTDIR)\xorio.res"

"$(OUTDIR)\xorio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\xorio.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\xorio.ppl"
   call prconvert "\out\Release\xorio.ppl"
	tsigner "\out\Release\xorio.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "xorio - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Wrappers/xorio
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\xorio.ppl" "$(OUTDIR)\xorio.bsc"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\io.sbr"
	-@erase "$(INTDIR)\plugin_xorio.obj"
	-@erase "$(INTDIR)\plugin_xorio.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xorio.res"
	-@erase "$(OUTDIR)\xorio.bsc"
	-@erase "$(OUTDIR)\xorio.exp"
	-@erase "$(OUTDIR)\xorio.ilk"
	-@erase "$(OUTDIR)\xorio.pdb"
	-@erase "$(OUTDIR)\xorio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XORIO_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\xorio.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xorio.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\io.sbr" \
	"$(INTDIR)\plugin_xorio.sbr"

"$(OUTDIR)\xorio.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=hashutil.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\xorio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\xorio.ppl" /implib:"$(OUTDIR)\xorio.lib" /pdbtype:sept /libpath:"../../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\plugin_xorio.obj" \
	"$(INTDIR)\xorio.res"

"$(OUTDIR)\xorio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("xorio.dep")
!INCLUDE "xorio.dep"
!ELSE 
!MESSAGE Warning: cannot find "xorio.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xorio - Win32 Release" || "$(CFG)" == "xorio - Win32 Debug"
SOURCE=.\io.c

!IF  "$(CFG)" == "xorio - Win32 Release"


"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xorio - Win32 Debug"


"$(INTDIR)\io.obj"	"$(INTDIR)\io.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_xorio.c

!IF  "$(CFG)" == "xorio - Win32 Release"


"$(INTDIR)\plugin_xorio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "xorio - Win32 Debug"


"$(INTDIR)\plugin_xorio.obj"	"$(INTDIR)\plugin_xorio.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\xorio.rc

"$(INTDIR)\xorio.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

