# Microsoft Developer Studio Generated NMAKE File, Based on wmihlpr.dsp
!IF "$(CFG)" == ""
CFG=wmihlpr - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wmihlpr - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wmihlpr - Win32 Release" && "$(CFG)" != "wmihlpr - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wmihlpr.mak" CFG="wmihlpr - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wmihlpr - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "wmihlpr - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "wmihlpr - Win32 Release"

OUTDIR=.\..\..\out\Release
INTDIR=.\..\..\temp\Release/ppp/wmihlpr
# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\wmihlpr.ppl" "..\..\temp\Release\ppp\wmihlpr\wmihlpr.pch"


CLEAN :
	-@erase "$(INTDIR)\plugin_wmihlpr.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wmihlpr.obj"
	-@erase "$(INTDIR)\wmihlpr.pch"
	-@erase "$(INTDIR)\wmihlpr.res"
	-@erase "$(OUTDIR)\wmihlpr.exp"
	-@erase "$(OUTDIR)\wmihlpr.pdb"
	-@erase "$(OUTDIR)\wmihlpr.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /GB /MD /W3 /GX /Zi /O1 /I "../include" /I "../../prague/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WMIHLPR_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\wmihlpr.res" /i "../../CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmihlpr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib WbemUuid.Lib /nologo /base:"0x69600000" /dll /incremental:no /pdb:"$(OUTDIR)\wmihlpr.pdb" /debug /machine:I386 /def:".\wmihlpr.def" /out:"$(OUTDIR)\wmihlpr.ppl" /implib:"$(OUTDIR)\wmihlpr.lib" /OPT:REF 
LINK32_OBJS= \
	"$(INTDIR)\plugin_wmihlpr.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\wmihlpr.obj" \
	"$(INTDIR)\wmihlpr.res"

"$(OUTDIR)\wmihlpr.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\wmihlpr.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\..\out\Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\wmihlpr.ppl" "..\..\temp\Release\ppp\wmihlpr\wmihlpr.pch"
   call tsigner \out\Release\wmihlpr.ppl
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "wmihlpr - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug/ppp/wmihlpr
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\wmihlpr.ppl" "..\..\temp\Debug\ppp\wmihlpr\wmihlpr.pch"


CLEAN :
	-@erase "$(INTDIR)\plugin_wmihlpr.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wmihlpr.obj"
	-@erase "$(INTDIR)\wmihlpr.pch"
	-@erase "$(INTDIR)\wmihlpr.res"
	-@erase "$(OUTDIR)\wmihlpr.exp"
	-@erase "$(OUTDIR)\wmihlpr.ilk"
	-@erase "$(OUTDIR)\wmihlpr.lib"
	-@erase "$(OUTDIR)\wmihlpr.pdb"
	-@erase "$(OUTDIR)\wmihlpr.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../prague/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WMIHLPR_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\wmihlpr.res" /i "../../CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmihlpr.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib WbemUuid.Lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\wmihlpr.pdb" /debug /machine:I386 /def:".\wmihlpr.def" /out:"$(OUTDIR)\wmihlpr.ppl" /implib:"$(OUTDIR)\wmihlpr.lib" /pdbtype:sept 
DEF_FILE= \
	".\wmihlpr.def"
LINK32_OBJS= \
	"$(INTDIR)\plugin_wmihlpr.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\wmihlpr.obj" \
	"$(INTDIR)\wmihlpr.res"

"$(OUTDIR)\wmihlpr.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("wmihlpr.dep")
!INCLUDE "wmihlpr.dep"
!ELSE 
!MESSAGE Warning: cannot find "wmihlpr.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wmihlpr - Win32 Release" || "$(CFG)" == "wmihlpr - Win32 Debug"
SOURCE=.\plugin_wmihlpr.cpp

"$(INTDIR)\plugin_wmihlpr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "wmihlpr - Win32 Release"

CPP_SWITCHES=/nologo /GB /MD /W3 /GX /Zi /O1 /I "../include" /I "../../prague/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WMIHLPR_EXPORTS" /Fp"$(INTDIR)\wmihlpr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\wmihlpr.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wmihlpr - Win32 Debug"

CPP_SWITCHES=/nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../prague/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WMIHLPR_EXPORTS" /Fp"$(INTDIR)\wmihlpr.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\wmihlpr.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\wmihlpr.cpp

"$(INTDIR)\wmihlpr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wmihlpr.rc

"$(INTDIR)\wmihlpr.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

