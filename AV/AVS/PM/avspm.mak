# Microsoft Developer Studio Generated NMAKE File, Based on avspm.dsp
!IF "$(CFG)" == ""
CFG=avspm - Win32 Debug
!MESSAGE No configuration specified. Defaulting to avspm - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "avspm - Win32 Release" && "$(CFG)" != "avspm - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avspm.mak" CFG="avspm - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avspm - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avspm - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avspm - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/ppp/avspm
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\avspm.ppl"


CLEAN :
	-@erase "$(INTDIR)\avspm.obj"
	-@erase "$(INTDIR)\avspm.res"
	-@erase "$(INTDIR)\plugin_avspm.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avspm.exp"
	-@erase "$(OUTDIR)\avspm.pdb"
	-@erase "$(OUTDIR)\avspm.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../../include" /I "../../../prague/include" /I "../../../prague" /D "NDEBUG" /D "WIN32" /D "_PRAGUE_TRACE_" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avspm.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avspm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /base:"0x62300000" /dll /incremental:no /pdb:"$(OUTDIR)\avspm.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avspm.ppl" /implib:"$(OUTDIR)\avspm.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\avspm.obj" \
	"$(INTDIR)\plugin_avspm.obj" \
	"$(INTDIR)\avspm.res"

"$(OUTDIR)\avspm.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\avspm.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\avspm.ppl"
   tsigner "\out\Release\avspm.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "avspm - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/ppp/avspm
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\avspm.ppl"


CLEAN :
	-@erase "$(INTDIR)\avspm.obj"
	-@erase "$(INTDIR)\avspm.res"
	-@erase "$(INTDIR)\plugin_avspm.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avspm.exp"
	-@erase "$(OUTDIR)\avspm.ilk"
	-@erase "$(OUTDIR)\avspm.pdb"
	-@erase "$(OUTDIR)\avspm.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../include" /I "../../../prague/include" /I "../../../prague" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "bl_EXPORTS" /D "C_STYLE_PROP" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avspm.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avspm.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\avspm.pdb" /debug /machine:I386 /out:"$(OUTDIR)\avspm.ppl" /implib:"$(OUTDIR)\avspm.lib" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\avspm.obj" \
	"$(INTDIR)\plugin_avspm.obj" \
	"$(INTDIR)\avspm.res"

"$(OUTDIR)\avspm.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("avspm.dep")
!INCLUDE "avspm.dep"
!ELSE 
!MESSAGE Warning: cannot find "avspm.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avspm - Win32 Release" || "$(CFG)" == "avspm - Win32 Debug"
SOURCE=.\avspm.cpp

"$(INTDIR)\avspm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_avspm.cpp

"$(INTDIR)\plugin_avspm.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\avspm.rc

"$(INTDIR)\avspm.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

