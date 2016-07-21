# Microsoft Developer Studio Generated NMAKE File, Based on base64p.dsp
!IF "$(CFG)" == ""
CFG=Base64P - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Base64P - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Base64P - Win32 Release" && "$(CFG)" != "Base64P - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "base64p.mak" CFG="Base64P - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Base64P - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Base64P - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Base64P - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\base64p.ppl"


CLEAN :
	-@erase "$(INTDIR)\base64p.obj"
	-@erase "$(INTDIR)\base64p_res.res"
	-@erase "$(INTDIR)\plugin_base64p.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\base64p.exp"
	-@erase "$(OUTDIR)\base64p.pdb"
	-@erase "$(OUTDIR)\base64p.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\base64p_res.res" /i "..\..\commonfiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\base64p.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib /nologo /base:"0x62500000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\base64p.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\base64p.ppl" /implib:"$(OUTDIR)\base64p.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\base64p.obj" \
	"$(INTDIR)\plugin_base64p.obj" \
	"$(INTDIR)\base64p_res.res"

"$(OUTDIR)\base64p.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\base64p.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\base64p.ppl"
   call prconvert "\out\Release\base64p.ppl"
	tsigner "\out\Release\base64p.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Base64P - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\base64p.ppl"


CLEAN :
	-@erase "$(INTDIR)\base64p.obj"
	-@erase "$(INTDIR)\base64p_res.res"
	-@erase "$(INTDIR)\plugin_base64p.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\base64p.exp"
	-@erase "$(OUTDIR)\base64p.ilk"
	-@erase "$(OUTDIR)\base64p.pdb"
	-@erase "$(OUTDIR)\base64p.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\base64p_res.res" /i "..\..\commonfiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\base64p.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\base64p.pdb" /debug /machine:I386 /out:"$(OUTDIR)\base64p.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\base64p.obj" \
	"$(INTDIR)\plugin_base64p.obj" \
	"$(INTDIR)\base64p_res.res"

"$(OUTDIR)\base64p.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("base64p.dep")
!INCLUDE "base64p.dep"
!ELSE 
!MESSAGE Warning: cannot find "base64p.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Base64P - Win32 Release" || "$(CFG)" == "Base64P - Win32 Debug"
SOURCE=.\base64p.cpp

"$(INTDIR)\base64p.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_base64p.cpp

"$(INTDIR)\plugin_base64p.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\base64p_res.rc"

"$(INTDIR)\base64p_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

