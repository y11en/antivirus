# Microsoft Developer Studio Generated NMAKE File, Based on UniArc.dsp
!IF "$(CFG)" == ""
CFG=UniArc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to UniArc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "UniArc - Win32 Release" && "$(CFG)" != "UniArc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UniArc.mak" CFG="UniArc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UniArc - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UniArc - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "UniArc - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/UniArc
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\UniArc.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objenum.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\uacback.obj"
	-@erase "$(INTDIR)\uniarc.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UniArc.exp"
	-@erase "$(OUTDIR)\UniArc.pdb"
	-@erase "$(OUTDIR)\UniArc.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNIARC_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\uniarc.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UniArc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x68600000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\UniArc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UniArc.ppl" /implib:"$(OUTDIR)\UniArc.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objenum.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\uacback.obj" \
	"$(INTDIR)\uniarc.res"

"$(OUTDIR)\UniArc.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\UniArc.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\UniArc.ppl"
   call prconvert "\out\Release\UniArc.ppl"
	tsigner "\out\Release\UniArc.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "UniArc - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/UniArc
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\UniArc.ppl"


CLEAN :
	-@erase "$(INTDIR)\io.obj"
	-@erase "$(INTDIR)\objenum.obj"
	-@erase "$(INTDIR)\os.obj"
	-@erase "$(INTDIR)\uacback.obj"
	-@erase "$(INTDIR)\uniarc.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\UniArc.exp"
	-@erase "$(OUTDIR)\UniArc.ilk"
	-@erase "$(OUTDIR)\UniArc.pdb"
	-@erase "$(OUTDIR)\UniArc.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UNIARC_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\uniarc.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\UniArc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /dll /incremental:yes /pdb:"$(OUTDIR)\UniArc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\UniArc.ppl" /implib:"$(OUTDIR)\UniArc.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\io.obj" \
	"$(INTDIR)\objenum.obj" \
	"$(INTDIR)\os.obj" \
	"$(INTDIR)\uacback.obj" \
	"$(INTDIR)\uniarc.res"

"$(OUTDIR)\UniArc.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("UniArc.dep")
!INCLUDE "UniArc.dep"
!ELSE 
!MESSAGE Warning: cannot find "UniArc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "UniArc - Win32 Release" || "$(CFG)" == "UniArc - Win32 Debug"
SOURCE=.\io.c

"$(INTDIR)\io.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objenum.c

"$(INTDIR)\objenum.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\os.c

"$(INTDIR)\os.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\uacback.c

"$(INTDIR)\uacback.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\uniarc.rc

"$(INTDIR)\uniarc.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

