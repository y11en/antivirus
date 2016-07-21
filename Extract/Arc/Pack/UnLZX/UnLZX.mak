# Microsoft Developer Studio Generated NMAKE File, Based on UnLZX.dsp
!IF "$(CFG)" == ""
CFG=UnLZX - Win32 Debug
!MESSAGE No configuration specified. Defaulting to UnLZX - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UnLZX - Win32 Release" && "$(CFG)" != "UnLZX - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UnLZX.mak" CFG="UnLZX - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UnLZX - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UnLZX - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "UnLZX - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/Extract/UnLZX
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\UnLZX.ppl"


CLEAN :
	-@erase "$(INTDIR)\lzx.obj"
	-@erase "$(INTDIR)\plugin_lzxdecompressor.obj"
	-@erase "$(INTDIR)\UnLZX.res"
	-@erase "$(INTDIR)\unzlx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnLZX.exp"
	-@erase "$(OUTDIR)\UnLZX.pdb"
	-@erase "$(OUTDIR)\UnLZX.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNLZX_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\UnLZX.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnLZX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x68700000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\UnLZX.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnLZX.ppl" /implib:"$(OUTDIR)\UnLZX.lib" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\lzx.obj" \
	"$(INTDIR)\plugin_lzxdecompressor.obj" \
	"$(INTDIR)\unzlx.obj" \
	"$(INTDIR)\UnLZX.res"

"$(OUTDIR)\UnLZX.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\UnLZX.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\UnLZX.ppl"
   call prconvert "\out\Release\UnLZX.ppl"
	tsigner "\out\Release\UnLZX.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "UnLZX - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/Extract/UnLZX
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\UnLZX.ppl"


CLEAN :
	-@erase "$(INTDIR)\lzx.obj"
	-@erase "$(INTDIR)\plugin_lzxdecompressor.obj"
	-@erase "$(INTDIR)\UnLZX.res"
	-@erase "$(INTDIR)\unzlx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UnLZX.exp"
	-@erase "$(OUTDIR)\UnLZX.ilk"
	-@erase "$(OUTDIR)\UnLZX.pdb"
	-@erase "$(OUTDIR)\UnLZX.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\include" /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNLZX_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\UnLZX.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UnLZX.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\UnLZX.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UnLZX.ppl" /implib:"$(OUTDIR)\UnLZX.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\lzx.obj" \
	"$(INTDIR)\plugin_lzxdecompressor.obj" \
	"$(INTDIR)\unzlx.obj" \
	"$(INTDIR)\UnLZX.res"

"$(OUTDIR)\UnLZX.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("UnLZX.dep")
!INCLUDE "UnLZX.dep"
!ELSE 
!MESSAGE Warning: cannot find "UnLZX.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "UnLZX - Win32 Release" || "$(CFG)" == "UnLZX - Win32 Debug"
SOURCE=.\lzx.cpp

"$(INTDIR)\lzx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\plugin_lzxdecompressor.c

"$(INTDIR)\plugin_lzxdecompressor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unzlx.c

"$(INTDIR)\unzlx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\UnLZX.rc

"$(INTDIR)\UnLZX.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

