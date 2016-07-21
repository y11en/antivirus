# Microsoft Developer Studio Generated NMAKE File, Based on Util.dsp
!IF "$(CFG)" == ""
CFG=Util - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Util - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Util - Win32 Release" && "$(CFG)" != "Util - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Util.mak" CFG="Util - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Util - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Util - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Util - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/Util
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\PrUtil.ppl"


CLEAN :
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\lru.obj"
	-@erase "$(INTDIR)\ltree.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\Tree.obj"
	-@erase "$(INTDIR)\Util.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PrUtil.exp"
	-@erase "$(OUTDIR)\PrUtil.pdb"
	-@erase "$(OUTDIR)\PrUtil.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Util.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Util.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x69100000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\PrUtil.pdb" /debug /machine:I386 /out:"$(OUTDIR)\PrUtil.ppl" /implib:"$(OUTDIR)\PrUtil.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\lru.obj" \
	"$(INTDIR)\ltree.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\Tree.obj" \
	"$(INTDIR)\Util.res"

"$(OUTDIR)\PrUtil.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\PrUtil.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\PrUtil.ppl"
   call prconvert "\out\Release\PrUtil.ppl"
	tsigner "\out\Release\PrUtil.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Util - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/Util
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\PrUtil.ppl"


CLEAN :
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\lru.obj"
	-@erase "$(INTDIR)\ltree.obj"
	-@erase "$(INTDIR)\stack.obj"
	-@erase "$(INTDIR)\Tree.obj"
	-@erase "$(INTDIR)\Util.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PrUtil.exp"
	-@erase "$(OUTDIR)\PrUtil.pdb"
	-@erase "$(OUTDIR)\PrUtil.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W4 /Gm /GX /Zi /Od /I "..\include" /I "..\..\CommonFiles" /I "..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTIL_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Util.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Util.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:no /pdb:"$(OUTDIR)\PrUtil.pdb" /debug /machine:I386 /out:"$(OUTDIR)\PrUtil.ppl" /implib:"$(OUTDIR)\PrUtil.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\lru.obj" \
	"$(INTDIR)\ltree.obj" \
	"$(INTDIR)\stack.obj" \
	"$(INTDIR)\Tree.obj" \
	"$(INTDIR)\Util.res"

"$(OUTDIR)\PrUtil.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Util.dep")
!INCLUDE "Util.dep"
!ELSE 
!MESSAGE Warning: cannot find "Util.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Util - Win32 Release" || "$(CFG)" == "Util - Win32 Debug"
SOURCE=.\list.c

"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lru.c

"$(INTDIR)\lru.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ltree.c

"$(INTDIR)\ltree.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\stack.c

"$(INTDIR)\stack.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Tree.c

"$(INTDIR)\Tree.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Util.rc

"$(INTDIR)\Util.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

