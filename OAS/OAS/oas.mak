# Microsoft Developer Studio Generated NMAKE File, Based on oas.dsp
!IF "$(CFG)" == ""
CFG=oas - Win32 Debug
!MESSAGE No configuration specified. Defaulting to oas - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "oas - Win32 Release" && "$(CFG)" != "oas - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "oas.mak" CFG="oas - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "oas - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "oas - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "oas - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/oas
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\oas.ppl"


CLEAN :
	-@erase "$(INTDIR)\oas.res"
	-@erase "$(INTDIR)\plugin_oas.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\oas.exp"
	-@erase "$(OUTDIR)\oas.pdb"
	-@erase "$(OUTDIR)\oas.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../include" /I "../../prague/include" /I "../../prague" /I "../../CommonFiles" /D "_PRAGUE_TRACE_" /D "NDEBUG" /D "bl_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "C_STYLE_PROP" /D "DIRECT_SEND_EVENTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\oas.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\oas.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=sign.lib FSSync.lib kernel32.lib advapi32.lib /nologo /base:"0x65f00000" /dll /incremental:no /pdb:"$(OUTDIR)\oas.pdb" /debug /machine:I386 /out:"$(OUTDIR)\oas.ppl" /implib:"$(OUTDIR)\oas.lib" /libpath:"../lib/release" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_oas.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\oas.res"

"$(OUTDIR)\oas.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\oas.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\oas.ppl"
   tsigner "\out\Release\oas.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "oas - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/oas
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\oas.ppl"


CLEAN :
	-@erase "$(INTDIR)\oas.res"
	-@erase "$(INTDIR)\plugin_oas.obj"
	-@erase "$(INTDIR)\process.obj"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\oas.exp"
	-@erase "$(OUTDIR)\oas.ilk"
	-@erase "$(OUTDIR)\oas.pdb"
	-@erase "$(OUTDIR)\oas.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../include" /I "../../prague/include" /I "../../prague" /I "../../CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "oas_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "C_STYLE_PROP" /D "DIRECT_SEND_EVENTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\oas.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\oas.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=sign.lib FSSync.lib kernel32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\oas.pdb" /debug /machine:I386 /out:"$(OUTDIR)\oas.ppl" /implib:"$(OUTDIR)\oas.lib" /pdbtype:sept /libpath:"../lib/Debug" /libpath:"../../CommonFiles/DebugDll" /libpath:"../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_oas.obj" \
	"$(INTDIR)\process.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\oas.res"

"$(OUTDIR)\oas.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("oas.dep")
!INCLUDE "oas.dep"
!ELSE 
!MESSAGE Warning: cannot find "oas.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "oas - Win32 Release" || "$(CFG)" == "oas - Win32 Debug"
SOURCE=.\plugin_oas.cpp

"$(INTDIR)\plugin_oas.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\process.cpp

"$(INTDIR)\process.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\task.cpp

"$(INTDIR)\task.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\oas.rc

"$(INTDIR)\oas.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

