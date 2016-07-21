# Microsoft Developer Studio Generated NMAKE File, Based on dtreg.dsp
!IF "$(CFG)" == ""
CFG=dtreg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dtreg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dtreg - Win32 Release" && "$(CFG)" != "dtreg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dtreg.mak" CFG="dtreg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dtreg - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "dtreg - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "dtreg - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/dtreg
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\dtreg.ppl"


CLEAN :
	-@erase "$(INTDIR)\dt_copy.obj"
	-@erase "$(INTDIR)\dtreg.obj"
	-@erase "$(INTDIR)\dtreg.res"
	-@erase "$(INTDIR)\plugin_dtreg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dtreg.exp"
	-@erase "$(OUTDIR)\dtreg.pdb"
	-@erase "$(OUTDIR)\dtreg.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DTREG_EXPORTS" /D "NON_STANDART_NAMES" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dtreg.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dtreg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=property.lib kldtser.lib swm.lib hashutil.lib /nologo /base:"0x63600000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\dtreg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dtreg.ppl" /implib:"$(OUTDIR)\dtreg.lib" /pdbtype:sept /libpath:"..\..\out\release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\dt_copy.obj" \
	"$(INTDIR)\dtreg.obj" \
	"$(INTDIR)\plugin_dtreg.obj" \
	"$(INTDIR)\dtreg.res"

"$(OUTDIR)\dtreg.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\dtreg.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\dtreg.ppl"
   call prconvert "\out\Release\dtreg.ppl"
	tsigner "\out\Release\dtreg.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "dtreg - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/dtreg
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\dtreg.ppl" "$(OUTDIR)\dtreg.bsc"


CLEAN :
	-@erase "$(INTDIR)\dt_copy.obj"
	-@erase "$(INTDIR)\dt_copy.sbr"
	-@erase "$(INTDIR)\dtreg.obj"
	-@erase "$(INTDIR)\dtreg.res"
	-@erase "$(INTDIR)\dtreg.sbr"
	-@erase "$(INTDIR)\plugin_dtreg.obj"
	-@erase "$(INTDIR)\plugin_dtreg.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dtreg.bsc"
	-@erase "$(OUTDIR)\dtreg.exp"
	-@erase "$(OUTDIR)\dtreg.ilk"
	-@erase "$(OUTDIR)\dtreg.pdb"
	-@erase "$(OUTDIR)\dtreg.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DTREG_EXPORTS" /D "NON_STANDART_NAMES" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\dtreg.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dtreg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dt_copy.sbr" \
	"$(INTDIR)\dtreg.sbr" \
	"$(INTDIR)\plugin_dtreg.sbr"

"$(OUTDIR)\dtreg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=property.lib kldtser.lib swm.lib hashutil.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\dtreg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\dtreg.ppl" /implib:"$(OUTDIR)\dtreg.lib" /pdbtype:sept /libpath:"..\..\CommonFiles\DebugDll" /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\dt_copy.obj" \
	"$(INTDIR)\dtreg.obj" \
	"$(INTDIR)\plugin_dtreg.obj" \
	"$(INTDIR)\dtreg.res"

"$(OUTDIR)\dtreg.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("dtreg.dep")
!INCLUDE "dtreg.dep"
!ELSE 
!MESSAGE Warning: cannot find "dtreg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dtreg - Win32 Release" || "$(CFG)" == "dtreg - Win32 Debug"
SOURCE=.\dt_copy.cpp

!IF  "$(CFG)" == "dtreg - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX- /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DTREG_EXPORTS" /D "NON_STANDART_NAMES" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\dt_copy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "dtreg - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DTREG_EXPORTS" /D "NON_STANDART_NAMES" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\dt_copy.obj"	"$(INTDIR)\dt_copy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\dtreg.c

!IF  "$(CFG)" == "dtreg - Win32 Release"


"$(INTDIR)\dtreg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dtreg - Win32 Debug"


"$(INTDIR)\dtreg.obj"	"$(INTDIR)\dtreg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_dtreg.c

!IF  "$(CFG)" == "dtreg - Win32 Release"


"$(INTDIR)\plugin_dtreg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "dtreg - Win32 Debug"


"$(INTDIR)\plugin_dtreg.obj"	"$(INTDIR)\plugin_dtreg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\dtreg.rc

"$(INTDIR)\dtreg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

