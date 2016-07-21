# Microsoft Developer Studio Generated NMAKE File, Based on SubstIO.dsp
!IF "$(CFG)" == ""
CFG=SubstIO - Win32 Debug
!MESSAGE No configuration specified. Defaulting to SubstIO - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "SubstIO - Win32 Release" && "$(CFG)" != "SubstIO - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SubstIO.mak" CFG="SubstIO - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SubstIO - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "SubstIO - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "SubstIO - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\Release\SubstIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\plugin_substio.obj"
	-@erase "$(INTDIR)\SubstIO.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\SubstIO.exp"
	-@erase "$(OUTDIR)\SubstIO.map"
	-@erase "$(OUTDIR)\SubstIO.pdb"
	-@erase "..\Release\SubstIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\SubstIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\SubstIO.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SubstIO.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\SubstIO.pdb" /map:"$(INTDIR)\SubstIO.map" /debug /machine:I386 /nodefaultlib /out:"..\Release/SubstIO.ppl" /implib:"$(OUTDIR)\SubstIO.lib" /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\plugin_substio.obj" \
	"$(INTDIR)\SubstIO.res"

"..\Release\SubstIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\Prague\Release\SubstIO.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\Release\SubstIO.ppl"
   call prconvert "\Prague\Release\SubstIO.ppl"
	tsigner "\Prague\Release\SubstIO.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "SubstIO - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\Debug\SubstIO.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objptr.obj"
	-@erase "$(INTDIR)\plugin_substio.obj"
	-@erase "$(INTDIR)\SubstIO.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SubstIO.exp"
	-@erase "$(OUTDIR)\SubstIO.pdb"
	-@erase "..\Debug\SubstIO.ilk"
	-@erase "..\Debug\SubstIO.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /ZI /Od /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\SubstIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\SubstIO.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\SubstIO.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\SubstIO.pdb" /debug /machine:I386 /out:"..\Debug/SubstIO.ppl" /implib:"$(OUTDIR)\SubstIO.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objptr.obj" \
	"$(INTDIR)\plugin_substio.obj" \
	"$(INTDIR)\SubstIO.res"

"..\Debug\SubstIO.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\Prague\Debug\SubstIO.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\Debug\SubstIO.ppl"
   tsigner "\Prague\Debug\SubstIO.ppl"
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
!IF EXISTS("SubstIO.dep")
!INCLUDE "SubstIO.dep"
!ELSE 
!MESSAGE Warning: cannot find "SubstIO.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "SubstIO - Win32 Release" || "$(CFG)" == "SubstIO - Win32 Debug"
SOURCE=.\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objptr.c

"$(INTDIR)\objptr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_substio.c

"$(INTDIR)\plugin_substio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SubstIO.rc

"$(INTDIR)\SubstIO.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

