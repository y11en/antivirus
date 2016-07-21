# Microsoft Developer Studio Generated NMAKE File, Based on minizip.dsp
!IF "$(CFG)" == ""
CFG=minizip - Win32 Debug
!MESSAGE No configuration specified. Defaulting to minizip - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "minizip - Win32 Release" && "$(CFG)" != "minizip - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "minizip.mak" CFG="minizip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "minizip - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "minizip - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "minizip - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/MiniArchiver/Zip
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\minizip.ppl"


CLEAN :
	-@erase "$(INTDIR)\miniarchiver.obj"
	-@erase "$(INTDIR)\minizip.res"
	-@erase "$(INTDIR)\plugin_minizip.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zip.obj"
	-@erase "$(OUTDIR)\minizip.exp"
	-@erase "$(OUTDIR)\minizip.pdb"
	-@erase "$(OUTDIR)\minizip.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIZIP_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\minizip.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\minizip.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=hashutil.lib /nologo /base:"0x65600000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\minizip.pdb" /debug /machine:I386 /out:"$(OUTDIR)\minizip.ppl" /implib:"$(OUTDIR)\minizip.lib" /libpath:"../../../out/release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\miniarchiver.obj" \
	"$(INTDIR)\plugin_minizip.obj" \
	"$(INTDIR)\zip.obj" \
	"$(INTDIR)\minizip.res"

"$(OUTDIR)\minizip.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\minizip.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\minizip.ppl"
   call prconvert "\out\Release\minizip.ppl"
	tsigner "\out\Release\minizip.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "minizip - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/MiniArchiver/Zip
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\minizip.ppl" "$(OUTDIR)\minizip.bsc"


CLEAN :
	-@erase "$(INTDIR)\miniarchiver.obj"
	-@erase "$(INTDIR)\miniarchiver.sbr"
	-@erase "$(INTDIR)\minizip.res"
	-@erase "$(INTDIR)\plugin_minizip.obj"
	-@erase "$(INTDIR)\plugin_minizip.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\zip.obj"
	-@erase "$(INTDIR)\zip.sbr"
	-@erase "$(OUTDIR)\minizip.bsc"
	-@erase "$(OUTDIR)\minizip.exp"
	-@erase "$(OUTDIR)\minizip.ilk"
	-@erase "$(OUTDIR)\minizip.pdb"
	-@erase "$(OUTDIR)\minizip.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MINIZIP_EXPORTS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\minizip.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\minizip.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\miniarchiver.sbr" \
	"$(INTDIR)\plugin_minizip.sbr" \
	"$(INTDIR)\zip.sbr"

"$(OUTDIR)\minizip.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=hashutil.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\minizip.pdb" /debug /machine:I386 /out:"$(OUTDIR)\minizip.ppl" /implib:"$(OUTDIR)\minizip.lib" /pdbtype:sept /libpath:"../../../out/debug" 
LINK32_OBJS= \
	"$(INTDIR)\miniarchiver.obj" \
	"$(INTDIR)\plugin_minizip.obj" \
	"$(INTDIR)\zip.obj" \
	"$(INTDIR)\minizip.res"

"$(OUTDIR)\minizip.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("minizip.dep")
!INCLUDE "minizip.dep"
!ELSE 
!MESSAGE Warning: cannot find "minizip.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "minizip - Win32 Release" || "$(CFG)" == "minizip - Win32 Debug"
SOURCE=.\miniarchiver.c

!IF  "$(CFG)" == "minizip - Win32 Release"


"$(INTDIR)\miniarchiver.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minizip - Win32 Debug"


"$(INTDIR)\miniarchiver.obj"	"$(INTDIR)\miniarchiver.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_minizip.c

!IF  "$(CFG)" == "minizip - Win32 Release"


"$(INTDIR)\plugin_minizip.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minizip - Win32 Debug"


"$(INTDIR)\plugin_minizip.obj"	"$(INTDIR)\plugin_minizip.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\zip.c

!IF  "$(CFG)" == "minizip - Win32 Release"


"$(INTDIR)\zip.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "minizip - Win32 Debug"


"$(INTDIR)\zip.obj"	"$(INTDIR)\zip.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\minizip.rc

"$(INTDIR)\minizip.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

