# Microsoft Developer Studio Generated NMAKE File, Based on ndetect.dsp
!IF "$(CFG)" == ""
CFG=NetDetect - Win32 Debug
!MESSAGE No configuration specified. Defaulting to NetDetect - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "NetDetect - Win32 Release" && "$(CFG)" != "NetDetect - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ndetect.mak" CFG="NetDetect - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NetDetect - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NetDetect - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "NetDetect - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/netdetect
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\ndetect.ppl"


CLEAN :
	-@erase "$(INTDIR)\ndetect_res.res"
	-@erase "$(INTDIR)\netdetect.obj"
	-@erase "$(INTDIR)\plugin_netdetect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ndetect.exp"
	-@erase "$(OUTDIR)\ndetect.pdb"
	-@erase "$(OUTDIR)\ndetect.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\ppp\include" /I "..\..\prague\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\ndetect_res.res" /i "..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ndetect.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /base:"0x65900000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ndetect.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ndetect.ppl" /implib:"$(OUTDIR)\ndetect.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\netdetect.obj" \
	"$(INTDIR)\plugin_netdetect.obj" \
	"$(INTDIR)\ndetect_res.res"

"$(OUTDIR)\ndetect.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ndetect.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\ndetect.ppl"
   tsigner "\out\Release\ndetect.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "NetDetect - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/netdetect
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\ndetect.ppl"


CLEAN :
	-@erase "$(INTDIR)\ndetect_res.res"
	-@erase "$(INTDIR)\netdetect.obj"
	-@erase "$(INTDIR)\plugin_netdetect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ndetect.exp"
	-@erase "$(OUTDIR)\ndetect.ilk"
	-@erase "$(OUTDIR)\ndetect.pdb"
	-@erase "$(OUTDIR)\ndetect.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\ppp\include" /I "..\..\prague\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\ndetect_res.res" /i "..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ndetect.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ndetect.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ndetect.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\netdetect.obj" \
	"$(INTDIR)\plugin_netdetect.obj" \
	"$(INTDIR)\ndetect_res.res"

"$(OUTDIR)\ndetect.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ndetect.dep")
!INCLUDE "ndetect.dep"
!ELSE 
!MESSAGE Warning: cannot find "ndetect.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "NetDetect - Win32 Release" || "$(CFG)" == "NetDetect - Win32 Debug"
SOURCE=.\netdetect.cpp

"$(INTDIR)\netdetect.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_netdetect.cpp

"$(INTDIR)\plugin_netdetect.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\ndetect_res.rc"

"$(INTDIR)\ndetect_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

