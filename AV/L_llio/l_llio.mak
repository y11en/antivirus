# Microsoft Developer Studio Generated NMAKE File, Based on l_llio.dsp
!IF "$(CFG)" == ""
CFG=L_llio - Win32 Debug
!MESSAGE No configuration specified. Defaulting to L_llio - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "L_llio - Win32 Release" && "$(CFG)" != "L_llio - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "l_llio.mak" CFG="L_llio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "L_llio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "L_llio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "L_llio - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/L_llio
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\L_llio.ppl"


CLEAN :
	-@erase "$(INTDIR)\L_llio.obj"
	-@erase "$(INTDIR)\L_llio.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\L_llio.exp"
	-@erase "$(OUTDIR)\L_llio.pdb"
	-@erase "$(OUTDIR)\L_llio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\Include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "L_LLIO_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\L_llio.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\l_llio.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x64b00000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\L_llio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\L_llio.ppl" /implib:"$(OUTDIR)\L_llio.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\L_llio.obj" \
	"$(INTDIR)\L_llio.res"

"$(OUTDIR)\L_llio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\L_llio.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\L_llio.ppl"
   tsigner "\out\Release\L_llio.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "L_llio - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/L_llio
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\L_llio.ppl" "$(OUTDIR)\l_llio.bsc"


CLEAN :
	-@erase "$(INTDIR)\L_llio.obj"
	-@erase "$(INTDIR)\L_llio.res"
	-@erase "$(INTDIR)\L_llio.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\l_llio.bsc"
	-@erase "$(OUTDIR)\L_llio.exp"
	-@erase "$(OUTDIR)\L_llio.ilk"
	-@erase "$(OUTDIR)\L_llio.pdb"
	-@erase "$(OUTDIR)\L_llio.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\Include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "L_LLIO_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\L_llio.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\l_llio.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\L_llio.sbr"

"$(OUTDIR)\l_llio.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\L_llio.pdb" /debug /machine:I386 /out:"$(OUTDIR)\L_llio.ppl" /implib:"$(OUTDIR)\L_llio.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\L_llio.obj" \
	"$(INTDIR)\L_llio.res"

"$(OUTDIR)\L_llio.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("l_llio.dep")
!INCLUDE "l_llio.dep"
!ELSE 
!MESSAGE Warning: cannot find "l_llio.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "L_llio - Win32 Release" || "$(CFG)" == "L_llio - Win32 Debug"
SOURCE=.\L_llio.c

!IF  "$(CFG)" == "L_llio - Win32 Release"


"$(INTDIR)\L_llio.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "L_llio - Win32 Debug"


"$(INTDIR)\L_llio.obj"	"$(INTDIR)\L_llio.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\L_llio.rc

"$(INTDIR)\L_llio.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

