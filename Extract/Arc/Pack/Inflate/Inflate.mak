# Microsoft Developer Studio Generated NMAKE File, Based on Inflate.dsp
!IF "$(CFG)" == ""
CFG=Inflate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Inflate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Inflate - Win32 Release" && "$(CFG)" != "Inflate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Inflate.mak" CFG="Inflate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Inflate - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Inflate - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Inflate - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Extract/Inflate
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\Inflate.ppl"


CLEAN :
	-@erase "$(INTDIR)\inflate.res"
	-@erase "$(INTDIR)\inflate_t.obj"
	-@erase "$(INTDIR)\plugin_inflate.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Inflate.exp"
	-@erase "$(OUTDIR)\Inflate.pdb"
	-@erase "$(OUTDIR)\Inflate.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INFLATE_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\inflate.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Inflate.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x64800000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\Inflate.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Inflate.ppl" /implib:"$(OUTDIR)\Inflate.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\inflate_t.obj" \
	"$(INTDIR)\plugin_inflate.obj" \
	"$(INTDIR)\inflate.res"

"$(OUTDIR)\Inflate.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\Inflate.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\Inflate.ppl"
   call prconvert "\out\Release\Inflate.ppl"
	tsigner "\out\Release\Inflate.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Inflate - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Extract/Inflate
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\Inflate.ppl" "$(OUTDIR)\Inflate.bsc"


CLEAN :
	-@erase "$(INTDIR)\inflate.res"
	-@erase "$(INTDIR)\inflate_t.obj"
	-@erase "$(INTDIR)\inflate_t.sbr"
	-@erase "$(INTDIR)\plugin_inflate.obj"
	-@erase "$(INTDIR)\plugin_inflate.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Inflate.bsc"
	-@erase "$(OUTDIR)\Inflate.exp"
	-@erase "$(OUTDIR)\Inflate.ilk"
	-@erase "$(OUTDIR)\Inflate.pdb"
	-@erase "$(OUTDIR)\Inflate.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "INFLATE_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\inflate.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Inflate.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\inflate_t.sbr" \
	"$(INTDIR)\plugin_inflate.sbr"

"$(OUTDIR)\Inflate.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Inflate.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Inflate.ppl" /implib:"$(OUTDIR)\Inflate.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\inflate_t.obj" \
	"$(INTDIR)\plugin_inflate.obj" \
	"$(INTDIR)\inflate.res"

"$(OUTDIR)\Inflate.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Inflate.dep")
!INCLUDE "Inflate.dep"
!ELSE 
!MESSAGE Warning: cannot find "Inflate.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Inflate - Win32 Release" || "$(CFG)" == "Inflate - Win32 Debug"
SOURCE=.\inflate_t.c

!IF  "$(CFG)" == "Inflate - Win32 Release"


"$(INTDIR)\inflate_t.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Inflate - Win32 Debug"


"$(INTDIR)\inflate_t.obj"	"$(INTDIR)\inflate_t.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_inflate.c

!IF  "$(CFG)" == "Inflate - Win32 Release"


"$(INTDIR)\plugin_inflate.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Inflate - Win32 Debug"


"$(INTDIR)\plugin_inflate.obj"	"$(INTDIR)\plugin_inflate.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\inflate.rc

"$(INTDIR)\inflate.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

