# Microsoft Developer Studio Generated NMAKE File, Based on ArjPack.dsp
!IF "$(CFG)" == ""
CFG=ArjPack - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ArjPack - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ArjPack - Win32 Release" && "$(CFG)" != "ArjPack - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ArjPack.mak" CFG="ArjPack - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ArjPack - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ArjPack - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ArjPack - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Packers/ArjPack
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\ArjPack.ppl"


CLEAN :
	-@erase "$(INTDIR)\arjpack.obj"
	-@erase "$(INTDIR)\arjpack.res"
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\plugin_arjpacker.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ArjPack.exp"
	-@erase "$(OUTDIR)\ArjPack.pdb"
	-@erase "$(OUTDIR)\ArjPack.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARJPACK_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\arjpack.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ArjPack.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x61a00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\ArjPack.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ArjPack.ppl" /implib:"$(OUTDIR)\ArjPack.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\arjpack.obj" \
	"$(INTDIR)\encode.obj" \
	"$(INTDIR)\plugin_arjpacker.obj" \
	"$(INTDIR)\arjpack.res"

"$(OUTDIR)\ArjPack.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ArjPack.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ArjPack.ppl"
   call prconvert "\out\Release\ArjPack.ppl"
	tsigner "\out\Release\ArjPack.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ArjPack - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Packers/ArjPack
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\ArjPack.ppl"


CLEAN :
	-@erase "$(INTDIR)\arjpack.obj"
	-@erase "$(INTDIR)\arjpack.res"
	-@erase "$(INTDIR)\encode.obj"
	-@erase "$(INTDIR)\plugin_arjpacker.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ArjPack.exp"
	-@erase "$(OUTDIR)\ArjPack.ilk"
	-@erase "$(OUTDIR)\ArjPack.pdb"
	-@erase "$(OUTDIR)\ArjPack.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ARJPACK_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\arjpack.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ArjPack.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ArjPack.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ArjPack.ppl" /implib:"$(OUTDIR)\ArjPack.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\arjpack.obj" \
	"$(INTDIR)\encode.obj" \
	"$(INTDIR)\plugin_arjpacker.obj" \
	"$(INTDIR)\arjpack.res"

"$(OUTDIR)\ArjPack.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ArjPack.dep")
!INCLUDE "ArjPack.dep"
!ELSE 
!MESSAGE Warning: cannot find "ArjPack.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ArjPack - Win32 Release" || "$(CFG)" == "ArjPack - Win32 Debug"
SOURCE=.\arjpack.c

"$(INTDIR)\arjpack.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\encode.c

"$(INTDIR)\encode.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_arjpacker.c

"$(INTDIR)\plugin_arjpacker.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\arjpack.rc

"$(INTDIR)\arjpack.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

