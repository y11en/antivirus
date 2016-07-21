# Microsoft Developer Studio Generated NMAKE File, Based on MKavIO.dsp
!IF "$(CFG)" == ""
CFG=MKavIO - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MKavIO - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MKavIO - Win32 Release" && "$(CFG)" != "MKavIO - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MKavIO.mak" CFG="MKavIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MKavIO - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MKavIO - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "MKavIO - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/MKavIO
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\MKavIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\MKavIO.res"
	-@erase "$(INTDIR)\plugin_mkavio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MKavIO.exp"
	-@erase "$(OUTDIR)\MKavIO.pdb"
	-@erase "$(OUTDIR)\MKavIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /Gz /MD /W3 /GX /Zi /O1 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\MKavIO.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MKavIO.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /base:"0x65680000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\MKavIO.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MKavIO.ppl" /implib:"$(OUTDIR)\MKavIO.lib" /pdbtype:sept /libpath:"..\..\out\Release" 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\plugin_mkavio.obj" \
	"$(INTDIR)\MKavIO.res"

"$(OUTDIR)\MKavIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\MKavIO.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MKavIO.ppl"
   tsigner "\out\Release\MKavIO.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "MKavIO - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/MKavIO
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\MKavIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\MKavIO.res"
	-@erase "$(INTDIR)\plugin_mkavio.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\MKavIO.exp"
	-@erase "$(OUTDIR)\MKavIO.ilk"
	-@erase "$(OUTDIR)\MKavIO.pdb"
	-@erase "$(OUTDIR)\MKavIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G6 /Gz /MDd /W3 /Gm /GX /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\MKavIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\MKavIO.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\MKavIO.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib FSDrvLib.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\MKavIO.pdb" /debug /machine:I386 /out:"$(OUTDIR)\MKavIO.ppl" /implib:"$(OUTDIR)\MKavIO.lib" /pdbtype:sept /libpath:"..\..\Out\Debug" 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\plugin_mkavio.obj" \
	"$(INTDIR)\MKavIO.res"

"$(OUTDIR)\MKavIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Debug\MKavIO.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\MKavIO.ppl"
   tsigner "\out\Debug\MKavIO.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

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
!IF EXISTS("MKavIO.dep")
!INCLUDE "MKavIO.dep"
!ELSE 
!MESSAGE Warning: cannot find "MKavIO.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MKavIO - Win32 Release" || "$(CFG)" == "MKavIO - Win32 Debug"
SOURCE=.\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\MKavIO.rc

"$(INTDIR)\MKavIO.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\plugin_mkavio.c

"$(INTDIR)\plugin_mkavio.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

