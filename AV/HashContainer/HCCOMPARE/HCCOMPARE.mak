# Microsoft Developer Studio Generated NMAKE File, Based on HCCOMPARE.dsp
!IF "$(CFG)" == ""
CFG=HCCOMPARE - Win32 Debug
!MESSAGE No configuration specified. Defaulting to HCCOMPARE - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "HCCOMPARE - Win32 Release" && "$(CFG)" != "HCCOMPARE - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "HCCOMPARE.mak" CFG="HCCOMPARE - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "HCCOMPARE - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "HCCOMPARE - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "HCCOMPARE - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/HashContainer/HCCOMPARE
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\HCCMP.ppl"


CLEAN :
	-@erase "$(INTDIR)\compare.obj"
	-@erase "$(INTDIR)\compare.res"
	-@erase "$(INTDIR)\plugin_comparehashcont.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HCCMP.exp"
	-@erase "$(OUTDIR)\HCCMP.pdb"
	-@erase "$(OUTDIR)\HCCMP.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HCCOMPARE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\compare.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HCCOMPARE.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x64000000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\HCCMP.pdb" /debug /machine:I386 /out:"$(OUTDIR)\HCCMP.ppl" /implib:"$(OUTDIR)\HCCMP.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\compare.obj" \
	"$(INTDIR)\plugin_comparehashcont.obj" \
	"$(INTDIR)\compare.res"

"$(OUTDIR)\HCCMP.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\HCCMP.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\HCCMP.ppl"
   call prconvert "\out\Release\HCCMP.ppl"
	tsigner "\out\Release\HCCMP.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "HCCOMPARE - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/HashContainer/HCCOMPARE
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\HCCMP.ppl"


CLEAN :
	-@erase "$(INTDIR)\compare.obj"
	-@erase "$(INTDIR)\compare.res"
	-@erase "$(INTDIR)\plugin_comparehashcont.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\HCCMP.exp"
	-@erase "$(OUTDIR)\HCCMP.ilk"
	-@erase "$(OUTDIR)\HCCMP.pdb"
	-@erase "$(OUTDIR)\HCCMP.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "HCCOMPARE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\compare.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\HCCOMPARE.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\HCCMP.pdb" /debug /machine:I386 /out:"$(OUTDIR)\HCCMP.ppl" /implib:"$(OUTDIR)\HCCMP.lib" /pdbtype:sept /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\compare.obj" \
	"$(INTDIR)\plugin_comparehashcont.obj" \
	"$(INTDIR)\compare.res"

"$(OUTDIR)\HCCMP.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("HCCOMPARE.dep")
!INCLUDE "HCCOMPARE.dep"
!ELSE 
!MESSAGE Warning: cannot find "HCCOMPARE.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "HCCOMPARE - Win32 Release" || "$(CFG)" == "HCCOMPARE - Win32 Debug"
SOURCE=.\compare.c

"$(INTDIR)\compare.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_comparehashcont.c

"$(INTDIR)\plugin_comparehashcont.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\compare.rc

"$(INTDIR)\compare.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

