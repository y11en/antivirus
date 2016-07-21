# Microsoft Developer Studio Generated NMAKE File, Based on ScIEPlugin.dsp
!IF "$(CFG)" == ""
CFG=ScIEPlugin - Win32 Release
!MESSAGE No configuration specified. Defaulting to ScIEPlugin - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "ScIEPlugin - Win32 Release" && "$(CFG)" != "ScIEPlugin - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ScIEPlugin.mak" CFG="ScIEPlugin - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ScIEPlugin - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ScIEPlugin - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ScIEPlugin - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/scieplugin
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ScIEPlugin.dll"

!ELSE 

ALL : "pr_remote - Win32 Release" "$(OUTDIR)\ScIEPlugin.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"pr_remote - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ClsFact.obj"
	-@erase "$(INTDIR)\ExplrBar.obj"
	-@erase "$(INTDIR)\Localizer.obj"
	-@erase "$(INTDIR)\ScIEPlugin.obj"
	-@erase "$(INTDIR)\ScIEPlugin.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ScIEPlugin.dll"
	-@erase "$(OUTDIR)\ScIEPlugin.exp"
	-@erase "$(OUTDIR)\ScIEPlugin.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MD /W3 /GX /Zi /O1 /I "..\..\prague\include" /I "..\include" /I "..\..\ppp\include\wtl" /I "..\..\commonfiles" /I "..\basegui" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ScIEPlugin.res" /i "..\..\commonfiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ScIEPlugin.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x67200000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\ScIEPlugin.pdb" /debug /machine:I386 /def:".\ScIEPlugin.def" /out:"$(OUTDIR)\ScIEPlugin.dll" /implib:"$(OUTDIR)\ScIEPlugin.lib" /IGNORE:4089 /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\ClsFact.obj" \
	"$(INTDIR)\ExplrBar.obj" \
	"$(INTDIR)\Localizer.obj" \
	"$(INTDIR)\ScIEPlugin.obj" \
	"$(INTDIR)\ScIEPlugin.res" \
	"$(OUTDIR)\pr_remote.lib"

"$(OUTDIR)\ScIEPlugin.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\ScIEPlugin.dll
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "pr_remote - Win32 Release" "$(OUTDIR)\ScIEPlugin.dll"
   tsigner "\out\Release\ScIEPlugin.dll"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ScIEPlugin - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../Temp/Debug/ppp/ScIEPlugin
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\ScIEPlugin.dll" "$(OUTDIR)\ScIEPlugin.bsc"

!ELSE 

ALL : "pr_remote - Win32 Debug" "$(OUTDIR)\ScIEPlugin.dll" "$(OUTDIR)\ScIEPlugin.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"pr_remote - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\ClsFact.obj"
	-@erase "$(INTDIR)\ClsFact.sbr"
	-@erase "$(INTDIR)\ExplrBar.obj"
	-@erase "$(INTDIR)\ExplrBar.sbr"
	-@erase "$(INTDIR)\Localizer.obj"
	-@erase "$(INTDIR)\Localizer.sbr"
	-@erase "$(INTDIR)\ScIEPlugin.obj"
	-@erase "$(INTDIR)\ScIEPlugin.res"
	-@erase "$(INTDIR)\ScIEPlugin.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ScIEPlugin.bsc"
	-@erase "$(OUTDIR)\ScIEPlugin.dll"
	-@erase "$(OUTDIR)\ScIEPlugin.exp"
	-@erase "$(OUTDIR)\ScIEPlugin.ilk"
	-@erase "$(OUTDIR)\ScIEPlugin.lib"
	-@erase "$(OUTDIR)\ScIEPlugin.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\prague\include" /I "..\include" /I "..\..\ppp\include\wtl" /I "..\..\commonfiles" /I "..\basegui" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\ScIEPlugin.res" /i "..\..\commonfiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ScIEPlugin.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ClsFact.sbr" \
	"$(INTDIR)\ExplrBar.sbr" \
	"$(INTDIR)\Localizer.sbr" \
	"$(INTDIR)\ScIEPlugin.sbr"

"$(OUTDIR)\ScIEPlugin.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\ScIEPlugin.pdb" /debug /machine:I386 /def:".\ScIEPlugin.def" /out:"$(OUTDIR)\ScIEPlugin.dll" /implib:"$(OUTDIR)\ScIEPlugin.lib" /pdbtype:sept 
DEF_FILE= \
	".\ScIEPlugin.def"
LINK32_OBJS= \
	"$(INTDIR)\ClsFact.obj" \
	"$(INTDIR)\ExplrBar.obj" \
	"$(INTDIR)\Localizer.obj" \
	"$(INTDIR)\ScIEPlugin.obj" \
	"$(INTDIR)\ScIEPlugin.res" \
	"$(OUTDIR)\pr_remote.lib"

"$(OUTDIR)\ScIEPlugin.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("ScIEPlugin.dep")
!INCLUDE "ScIEPlugin.dep"
!ELSE 
!MESSAGE Warning: cannot find "ScIEPlugin.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ScIEPlugin - Win32 Release" || "$(CFG)" == "ScIEPlugin - Win32 Debug"
SOURCE=.\ScIEPlugin.rc

"$(INTDIR)\ScIEPlugin.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ClsFact.cpp

!IF  "$(CFG)" == "ScIEPlugin - Win32 Release"


"$(INTDIR)\ClsFact.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ScIEPlugin - Win32 Debug"


"$(INTDIR)\ClsFact.obj"	"$(INTDIR)\ClsFact.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ExplrBar.cpp

!IF  "$(CFG)" == "ScIEPlugin - Win32 Release"


"$(INTDIR)\ExplrBar.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ScIEPlugin - Win32 Debug"


"$(INTDIR)\ExplrBar.obj"	"$(INTDIR)\ExplrBar.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Localizer.cpp

!IF  "$(CFG)" == "ScIEPlugin - Win32 Release"


"$(INTDIR)\Localizer.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ScIEPlugin - Win32 Debug"


"$(INTDIR)\Localizer.obj"	"$(INTDIR)\Localizer.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ScIEPlugin.cpp

!IF  "$(CFG)" == "ScIEPlugin - Win32 Release"


"$(INTDIR)\ScIEPlugin.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "ScIEPlugin - Win32 Debug"


"$(INTDIR)\ScIEPlugin.obj"	"$(INTDIR)\ScIEPlugin.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

!IF  "$(CFG)" == "ScIEPlugin - Win32 Release"

"pr_remote - Win32 Release" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Release" 
   cd "..\..\PPP\ScIEPlugin"

"pr_remote - Win32 ReleaseCLEAN" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\PPP\ScIEPlugin"

!ELSEIF  "$(CFG)" == "ScIEPlugin - Win32 Debug"

"pr_remote - Win32 Debug" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Debug" 
   cd "..\..\PPP\ScIEPlugin"

"pr_remote - Win32 DebugCLEAN" : 
   cd "\PRAGUE\remote"
   $(MAKE) /$(MAKEFLAGS) /F .\remote.mak CFG="pr_remote - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\PPP\ScIEPlugin"

!ENDIF 


!ENDIF 

