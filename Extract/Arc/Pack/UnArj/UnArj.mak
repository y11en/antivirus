# Microsoft Developer Studio Generated NMAKE File, Based on UnArj.dsp
!IF "$(CFG)" == ""
CFG=UnArj - Win32 Debug
!MESSAGE No configuration specified. Defaulting to UnArj - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UnArj - Win32 Release" && "$(CFG)" != "UnArj - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnArj.mak" CFG="UnArj - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnArj - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UnArj - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "UnArj - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Extract/UnArj
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\UnArj.ppl"


CLEAN :
	-@erase "$(INTDIR)\Decode.obj"
	-@erase "$(INTDIR)\plugin_unarj.obj"
	-@erase "$(INTDIR)\unarj.obj"
	-@erase "$(INTDIR)\UnArj.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnArj.exp"
	-@erase "$(OUTDIR)\UnArj.pdb"
	-@erase "$(OUTDIR)\UnArj.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNARJ_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\UnArj.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnArj.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x68500000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\UnArj.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnArj.ppl" /implib:"$(OUTDIR)\UnArj.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\Decode.obj" \
	"$(INTDIR)\plugin_unarj.obj" \
	"$(INTDIR)\unarj.obj" \
	"$(INTDIR)\UnArj.res"

"$(OUTDIR)\UnArj.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\UnArj.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\UnArj.ppl"
   call prconvert "\out\Release\UnArj.ppl"
	tsigner "\out\Release\UnArj.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "UnArj - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Extract/UnArj
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\UnArj.ppl"


CLEAN :
	-@erase "$(INTDIR)\Decode.obj"
	-@erase "$(INTDIR)\plugin_unarj.obj"
	-@erase "$(INTDIR)\unarj.obj"
	-@erase "$(INTDIR)\UnArj.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnArj.exp"
	-@erase "$(OUTDIR)\UnArj.ilk"
	-@erase "$(OUTDIR)\UnArj.pdb"
	-@erase "$(OUTDIR)\UnArj.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNARJ_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\UnArj.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnArj.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\UnArj.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnArj.ppl" /implib:"$(OUTDIR)\UnArj.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Decode.obj" \
	"$(INTDIR)\plugin_unarj.obj" \
	"$(INTDIR)\unarj.obj" \
	"$(INTDIR)\UnArj.res"

"$(OUTDIR)\UnArj.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("UnArj.dep")
!INCLUDE "UnArj.dep"
!ELSE 
!MESSAGE Warning: cannot find "UnArj.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "UnArj - Win32 Release" || "$(CFG)" == "UnArj - Win32 Debug"
SOURCE=.\Decode.c

"$(INTDIR)\Decode.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_unarj.c

"$(INTDIR)\plugin_unarj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unarj.c

"$(INTDIR)\unarj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UnArj.rc

"$(INTDIR)\UnArj.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

