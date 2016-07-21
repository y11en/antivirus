# Microsoft Developer Studio Generated NMAKE File, Based on ichecker.dsp
!IF "$(CFG)" == ""
CFG=ichecker - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ichecker - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ichecker - Win32 Release" && "$(CFG)" != "ichecker - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ichecker.mak" CFG="ichecker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ichecker - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ichecker - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "ichecker - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\Release\ichecker.ppl"

!ELSE 

ALL : "sfdb - Win32 Release" "..\..\Release\ichecker.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"sfdb - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\ifilesec.obj"
	-@erase "$(INTDIR)\ifilesec.res"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_ifilesec.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ichecker.exp"
	-@erase "..\..\Release\ichecker.map"
	-@erase "..\..\Release\ichecker.pdb"
	-@erase "..\..\Release\ichecker.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICHECKER_EXPORTS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\ifilesec.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichecker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0xAD200000" /dll /incremental:no /pdb:"../../Release/ichecker.pdb" /map:"../../Release/ichecker.map" /debug /machine:I386 /out:"../../Release/ichecker.ppl" /implib:"$(OUTDIR)\ichecker.lib" /pdbtype:sept /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\ifilesec.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_ifilesec.obj" \
	"$(INTDIR)\ifilesec.res"

"..\..\Release\ichecker.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=\Prague\Release
TargetPath=\Prague\Release\ichecker.ppl
TargetName=ichecker
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "sfdb - Win32 Release" "..\..\Release\ichecker.ppl"
   tsigner "\Prague\Release\ichecker.ppl"
	addsym \Prague\Release\ichecker.pdb ichecker
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ichecker - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "..\..\Debug\ichecker.ppl"

!ELSE 

ALL : "sfdb - Win32 Debug" "..\..\Debug\ichecker.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"sfdb - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\hash_md5.obj"
	-@erase "$(INTDIR)\ifilesec.obj"
	-@erase "$(INTDIR)\ifilesec.res"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\plugin_ifilesec.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ichecker.exp"
	-@erase "..\..\Debug\ichecker.ilk"
	-@erase "..\..\Debug\ichecker.pdb"
	-@erase "..\..\Debug\ichecker.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\..\CommonFiles" /I "..\..\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ICHECKER_EXPORTS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\ifilesec.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ichecker.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0xAD200000" /dll /incremental:yes /pdb:"../../Debug/ichecker.pdb" /debug /machine:I386 /out:"../../Debug/ichecker.ppl" /implib:"$(OUTDIR)\ichecker.lib" /pdbtype:sept /IGNORE:6004 
LINK32_OBJS= \
	"$(INTDIR)\hash_md5.obj" \
	"$(INTDIR)\ifilesec.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\plugin_ifilesec.obj" \
	"$(INTDIR)\ifilesec.res"

"..\..\Debug\ichecker.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=\Prague\Debug
TargetName=ichecker
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "sfdb - Win32 Debug" "..\..\Debug\ichecker.ppl"
   addsym \Prague\Debug\ichecker.pdb ichecker
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

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
!IF EXISTS("ichecker.dep")
!INCLUDE "ichecker.dep"
!ELSE 
!MESSAGE Warning: cannot find "ichecker.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ichecker - Win32 Release" || "$(CFG)" == "ichecker - Win32 Debug"
SOURCE=..\HASH_API\MD5\hash_md5.c

"$(INTDIR)\hash_md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ifilesec.c

"$(INTDIR)\ifilesec.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ifilesec.rc

"$(INTDIR)\ifilesec.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\HASH_API\MD5\md5.cpp

"$(INTDIR)\md5.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\plugin_ifilesec.c

"$(INTDIR)\plugin_ifilesec.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "ichecker - Win32 Release"

"sfdb - Win32 Release" : 
   cd "\Prague\ISLite\SFDB"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Release" 
   cd "..\iChecker"

"sfdb - Win32 ReleaseCLEAN" : 
   cd "\Prague\ISLite\SFDB"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Release" RECURSE=1 CLEAN 
   cd "..\iChecker"

!ELSEIF  "$(CFG)" == "ichecker - Win32 Debug"

"sfdb - Win32 Debug" : 
   cd "\Prague\ISLite\SFDB"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Debug" 
   cd "..\iChecker"

"sfdb - Win32 DebugCLEAN" : 
   cd "\Prague\ISLite\SFDB"
   $(MAKE) /$(MAKEFLAGS) /F .\sfdb.mak CFG="sfdb - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\iChecker"

!ENDIF 


!ENDIF 

