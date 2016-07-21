# Microsoft Developer Studio Generated NMAKE File, Based on AvpMgr.dsp
!IF "$(CFG)" == ""
CFG=AvpMgr - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AvpMgr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AvpMgr - Win32 Release" && "$(CFG)" != "AvpMgr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvpMgr.mak" CFG="AvpMgr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvpMgr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "AvpMgr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "AvpMgr - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/AvpMgr
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\AvpMgr.ppl"


CLEAN :
	-@erase "$(INTDIR)\AvpMgr.res"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\plugin_avpmgr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AvpMgr.exp"
	-@erase "$(OUTDIR)\AvpMgr.pdb"
	-@erase "$(OUTDIR)\AvpMgr.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "..\..\ppp\Include" /I "..\Include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPMGR_EXPORTS" /D "_PRAGUE_TRACE_" /D "KLAVPMGR" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\AvpMgr.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AvpMgr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x62100000" /entry:"DllMain" /dll /incremental:no /pdb:"$(OUTDIR)\AvpMgr.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AvpMgr.ppl" /implib:"$(OUTDIR)\AvpMgr.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\plugin_avpmgr.obj" \
	"$(INTDIR)\AvpMgr.res"

"$(OUTDIR)\AvpMgr.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\AvpMgr.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\AvpMgr.ppl"
   call prconvert "\out\Release\AvpMgr.ppl"
	tsigner "\out\Release\AvpMgr.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "AvpMgr - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/AvpMgr
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\AvpMgr.ppl" "$(OUTDIR)\AvpMgr.bsc"


CLEAN :
	-@erase "$(INTDIR)\AvpMgr.res"
	-@erase "$(INTDIR)\engine.obj"
	-@erase "$(INTDIR)\engine.sbr"
	-@erase "$(INTDIR)\plugin_avpmgr.obj"
	-@erase "$(INTDIR)\plugin_avpmgr.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AvpMgr.bsc"
	-@erase "$(OUTDIR)\AvpMgr.exp"
	-@erase "$(OUTDIR)\AvpMgr.ilk"
	-@erase "$(OUTDIR)\AvpMgr.pdb"
	-@erase "$(OUTDIR)\AvpMgr.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\Include" /I "..\..\ppp\Include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AVPMGR_EXPORTS" /D "_PRAGUE_TRACE_" /D "KLAVPMGR" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\AvpMgr.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AvpMgr.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\engine.sbr" \
	"$(INTDIR)\plugin_avpmgr.sbr"

"$(OUTDIR)\AvpMgr.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\AvpMgr.pdb" /debug /machine:I386 /out:"$(OUTDIR)\AvpMgr.ppl" /implib:"$(OUTDIR)\AvpMgr.lib" /pdbtype:sept /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\engine.obj" \
	"$(INTDIR)\plugin_avpmgr.obj" \
	"$(INTDIR)\AvpMgr.res"

"$(OUTDIR)\AvpMgr.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("AvpMgr.dep")
!INCLUDE "AvpMgr.dep"
!ELSE 
!MESSAGE Warning: cannot find "AvpMgr.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AvpMgr - Win32 Release" || "$(CFG)" == "AvpMgr - Win32 Debug"
SOURCE=.\engine.c

!IF  "$(CFG)" == "AvpMgr - Win32 Release"


"$(INTDIR)\engine.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "AvpMgr - Win32 Debug"


"$(INTDIR)\engine.obj"	"$(INTDIR)\engine.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_avpmgr.c

!IF  "$(CFG)" == "AvpMgr - Win32 Release"


"$(INTDIR)\plugin_avpmgr.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "AvpMgr - Win32 Debug"


"$(INTDIR)\plugin_avpmgr.obj"	"$(INTDIR)\plugin_avpmgr.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\AvpMgr.rc

"$(INTDIR)\AvpMgr.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

