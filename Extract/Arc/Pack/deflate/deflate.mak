# Microsoft Developer Studio Generated NMAKE File, Based on deflate.dsp
!IF "$(CFG)" == ""
CFG=deflate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to deflate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "deflate - Win32 Release" && "$(CFG)" != "deflate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "deflate.mak" CFG="deflate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "deflate - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "deflate - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "deflate - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Packers/deflate
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\deflate.ppl"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\deflate.res"
	-@erase "$(INTDIR)\deflate_t.obj"
	-@erase "$(INTDIR)\plugin_deflate.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\deflate.exp"
	-@erase "$(OUTDIR)\deflate.pdb"
	-@erase "$(OUTDIR)\deflate.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEFLATE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\deflate.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\deflate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x63200000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\deflate.pdb" /debug /machine:I386 /out:"$(OUTDIR)\deflate.ppl" /implib:"$(OUTDIR)\deflate.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\deflate_t.obj" \
	"$(INTDIR)\plugin_deflate.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\deflate.res"

"$(OUTDIR)\deflate.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\deflate.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\deflate.ppl"
   call prconvert "\out\Release\deflate.ppl"
	tsigner "\out\Release\deflate.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "deflate - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Packers/deflate
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\deflate.ppl"


CLEAN :
	-@erase "$(INTDIR)\adler32.obj"
	-@erase "$(INTDIR)\deflate.obj"
	-@erase "$(INTDIR)\deflate.res"
	-@erase "$(INTDIR)\deflate_t.obj"
	-@erase "$(INTDIR)\plugin_deflate.obj"
	-@erase "$(INTDIR)\trees.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zutil.obj"
	-@erase "$(OUTDIR)\deflate.exp"
	-@erase "$(OUTDIR)\deflate.ilk"
	-@erase "$(OUTDIR)\deflate.pdb"
	-@erase "$(OUTDIR)\deflate.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEFLATE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\deflate.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\deflate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\deflate.pdb" /debug /machine:I386 /out:"$(OUTDIR)\deflate.ppl" /implib:"$(OUTDIR)\deflate.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\adler32.obj" \
	"$(INTDIR)\deflate.obj" \
	"$(INTDIR)\deflate_t.obj" \
	"$(INTDIR)\plugin_deflate.obj" \
	"$(INTDIR)\trees.obj" \
	"$(INTDIR)\zutil.obj" \
	"$(INTDIR)\deflate.res"

"$(OUTDIR)\deflate.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("deflate.dep")
!INCLUDE "deflate.dep"
!ELSE 
!MESSAGE Warning: cannot find "deflate.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "deflate - Win32 Release" || "$(CFG)" == "deflate - Win32 Debug"
SOURCE=.\adler32.c

"$(INTDIR)\adler32.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\deflate.c

"$(INTDIR)\deflate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\deflate_t.c

"$(INTDIR)\deflate_t.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_deflate.c

"$(INTDIR)\plugin_deflate.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\trees.c

"$(INTDIR)\trees.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\zutil.c

"$(INTDIR)\zutil.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\deflate.rc

"$(INTDIR)\deflate.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

