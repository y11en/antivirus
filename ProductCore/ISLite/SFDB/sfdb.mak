# Microsoft Developer Studio Generated NMAKE File, Based on sfdb.dsp
!IF "$(CFG)" == ""
CFG=sfdb - Win32 Debug
!MESSAGE No configuration specified. Defaulting to sfdb - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "sfdb - Win32 ReleaseWithTrace" && "$(CFG)" != "sfdb - Win32 Release" && "$(CFG)" != "sfdb - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sfdb.mak" CFG="sfdb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "sfdb - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sfdb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "sfdb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "sfdb - Win32 ReleaseWithTrace"

OUTDIR=.\../../../out/ReleaseWithTrace
INTDIR=.\../../../temp/ReleaseWithTrace/prague/ISLite/SFDB
# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

ALL : "$(OUTDIR)\SFDB.PPL"


CLEAN :
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_sfdb.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sfdb.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\SFDB.EXP"
	-@erase "$(OUTDIR)\SFDB.pdb"
	-@erase "$(OUTDIR)\SFDB.PPL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfdb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0xAD900000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\SFDB.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\SFDB.PPL" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_sfdb.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\sfdb.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\SFDB.PPL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\ReleaseWithTrace\SFDB.PPL
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/ReleaseWithTrace
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\SFDB.PPL"
   tsigner "\out\ReleaseWithTrace\SFDB.PPL"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "sfdb - Win32 Release"

OUTDIR=.\../../../out/Release
INTDIR=.\../../../temp/Release/prague/ISLite/SFDB
# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\SFDB.PPL"


CLEAN :
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_sfdb.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sfdb.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SFDB.exp"
	-@erase "$(OUTDIR)\SFDB.pdb"
	-@erase "$(OUTDIR)\SFDB.PPL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfdb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib /nologo /base:"0x67700000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\SFDB.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SFDB.PPL" /implib:"$(OUTDIR)\SFDB.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_sfdb.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\sfdb.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\SFDB.PPL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\SFDB.PPL
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\SFDB.PPL"
   tsigner "\out\Release\SFDB.PPL"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "sfdb - Win32 Debug"

OUTDIR=.\../../../out/Debug
INTDIR=.\../../../temp/Debug/prague/ISLite/SFDB
# Begin Custom Macros
OutDir=.\../../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\SFDB.PPL"


CLEAN :
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_sfdb.obj"
	-@erase "$(INTDIR)\resource.res"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sfdb.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\SFDB.EXP"
	-@erase "$(OUTDIR)\SFDB.ILK"
	-@erase "$(OUTDIR)\SFDB.pdb"
	-@erase "$(OUTDIR)\SFDB.PPL"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\resource.res" /i "..\..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sfdb.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib advapi32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\SFDB.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SFDB.PPL" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_sfdb.obj" \
	"$(INTDIR)\sa.obj" \
	"$(INTDIR)\sfdb.obj" \
	"$(INTDIR)\resource.res"

"$(OUTDIR)\SFDB.PPL" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("sfdb.dep")
!INCLUDE "sfdb.dep"
!ELSE 
!MESSAGE Warning: cannot find "sfdb.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "sfdb - Win32 ReleaseWithTrace" || "$(CFG)" == "sfdb - Win32 Release" || "$(CFG)" == "sfdb - Win32 Debug"
SOURCE=..\HASH_API\MD5\hash_md5.c

"$(INTDIR)\hash_md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\HASH_API\MD5\md5.cpp

"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_sfdb.c

"$(INTDIR)\plugin_sfdb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sa.cpp

"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sfdb.c

"$(INTDIR)\sfdb.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\resource.rc

"$(INTDIR)\resource.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

