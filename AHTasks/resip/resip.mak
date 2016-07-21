# Microsoft Developer Studio Generated NMAKE File, Based on resip.dsp
!IF "$(CFG)" == ""
CFG=resip - Win32 Debug
!MESSAGE No configuration specified. Defaulting to resip - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "resip - Win32 Release" && "$(CFG)" != "resip - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "resip.mak" CFG="resip - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "resip - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "resip - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "resip - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/resip
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\resip.ppl"


CLEAN :
	-@erase "$(INTDIR)\ipresolver.obj"
	-@erase "$(INTDIR)\plugin_resip.obj"
	-@erase "$(INTDIR)\resip.res"
	-@erase "$(INTDIR)\resip_imp.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\resip.exp"
	-@erase "$(OUTDIR)\resip.pdb"
	-@erase "$(OUTDIR)\resip.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "resip_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\resip.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\resip.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib /nologo /base:"0x66f00000" /dll /incremental:no /pdb:"$(OUTDIR)\resip.pdb" /debug /machine:I386 /out:"$(OUTDIR)\resip.ppl" /implib:"$(OUTDIR)\resip.lib" /pdbtype:sept /libpath:"../../out/Release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\ipresolver.obj" \
	"$(INTDIR)\plugin_resip.obj" \
	"$(INTDIR)\resip_imp.obj" \
	"$(INTDIR)\resip.res"

"$(OUTDIR)\resip.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\resip.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\resip.ppl"
   tsigner "\out\Release\resip.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "resip - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/resip
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\resip.ppl" "$(OUTDIR)\resip.bsc"


CLEAN :
	-@erase "$(INTDIR)\ipresolver.obj"
	-@erase "$(INTDIR)\ipresolver.sbr"
	-@erase "$(INTDIR)\plugin_resip.obj"
	-@erase "$(INTDIR)\plugin_resip.sbr"
	-@erase "$(INTDIR)\resip.res"
	-@erase "$(INTDIR)\resip_imp.obj"
	-@erase "$(INTDIR)\resip_imp.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\resip.bsc"
	-@erase "$(OUTDIR)\resip.exp"
	-@erase "$(OUTDIR)\resip.ilk"
	-@erase "$(OUTDIR)\resip.map"
	-@erase "$(OUTDIR)\resip.pdb"
	-@erase "$(OUTDIR)\resip.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "resip_EXPORTS" /D "_PRAGUE_TRACE_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\resip.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\resip.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ipresolver.sbr" \
	"$(INTDIR)\plugin_resip.sbr" \
	"$(INTDIR)\resip_imp.sbr"

"$(OUTDIR)\resip.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\resip.pdb" /map:"../../out/Debug/resip.map" /debug /machine:I386 /out:"$(OUTDIR)\resip.ppl" /implib:"$(OUTDIR)\resip.lib" /pdbtype:sept /libpath:"../../out/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\ipresolver.obj" \
	"$(INTDIR)\plugin_resip.obj" \
	"$(INTDIR)\resip_imp.obj" \
	"$(INTDIR)\resip.res"

"$(OUTDIR)\resip.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("resip.dep")
!INCLUDE "resip.dep"
!ELSE 
!MESSAGE Warning: cannot find "resip.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "resip - Win32 Release" || "$(CFG)" == "resip - Win32 Debug"
SOURCE=.\ipresolver.cpp

!IF  "$(CFG)" == "resip - Win32 Release"


"$(INTDIR)\ipresolver.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "resip - Win32 Debug"


"$(INTDIR)\ipresolver.obj"	"$(INTDIR)\ipresolver.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_resip.cpp

!IF  "$(CFG)" == "resip - Win32 Release"


"$(INTDIR)\plugin_resip.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "resip - Win32 Debug"


"$(INTDIR)\plugin_resip.obj"	"$(INTDIR)\plugin_resip.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\resip_imp.cpp

!IF  "$(CFG)" == "resip - Win32 Release"


"$(INTDIR)\resip_imp.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "resip - Win32 Debug"


"$(INTDIR)\resip_imp.obj"	"$(INTDIR)\resip_imp.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\resip.rc

"$(INTDIR)\resip.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

