# Microsoft Developer Studio Generated NMAKE File, Based on StdCompare.dsp
!IF "$(CFG)" == ""
CFG=StdCompare - Win32 Debug
!MESSAGE No configuration specified. Defaulting to StdCompare - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "StdCompare - Win32 Release" && "$(CFG)" != "StdCompare - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "StdCompare.mak" CFG="StdCompare - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "StdCompare - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "StdCompare - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "StdCompare - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/StdCompare
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\StdComp.ppl"


CLEAN :
	-@erase "$(INTDIR)\compare.obj"
	-@erase "$(INTDIR)\plugin.res"
	-@erase "$(INTDIR)\plugin_stdcompare.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\StdComp.exp"
	-@erase "$(OUTDIR)\StdComp.pdb"
	-@erase "$(OUTDIR)\StdComp.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDCOMPARE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\plugin.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\StdCompare.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x67c00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\StdComp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\StdComp.ppl" /implib:"$(OUTDIR)\StdComp.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\compare.obj" \
	"$(INTDIR)\plugin_stdcompare.obj" \
	"$(INTDIR)\plugin.res"

"$(OUTDIR)\StdComp.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\StdComp.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\StdComp.ppl"
   call prconvert "\out\Release\StdComp.ppl"
	tsigner "\out\Release\StdComp.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "StdCompare - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/StdCompare
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\StdComp.ppl"


CLEAN :
	-@erase "$(INTDIR)\compare.obj"
	-@erase "$(INTDIR)\plugin.res"
	-@erase "$(INTDIR)\plugin_stdcompare.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\StdComp.exp"
	-@erase "$(OUTDIR)\StdComp.ilk"
	-@erase "$(OUTDIR)\StdComp.pdb"
	-@erase "$(OUTDIR)\StdComp.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "STDCOMPARE_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\plugin.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\StdCompare.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\StdComp.pdb" /debug /machine:I386 /out:"$(OUTDIR)\StdComp.ppl" /implib:"$(OUTDIR)\StdComp.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\compare.obj" \
	"$(INTDIR)\plugin_stdcompare.obj" \
	"$(INTDIR)\plugin.res"

"$(OUTDIR)\StdComp.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("StdCompare.dep")
!INCLUDE "StdCompare.dep"
!ELSE 
!MESSAGE Warning: cannot find "StdCompare.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "StdCompare - Win32 Release" || "$(CFG)" == "StdCompare - Win32 Debug"
SOURCE=.\compare.c

"$(INTDIR)\compare.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_stdcompare.c

"$(INTDIR)\plugin_stdcompare.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin.rc

"$(INTDIR)\plugin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

