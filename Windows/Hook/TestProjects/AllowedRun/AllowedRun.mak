# Microsoft Developer Studio Generated NMAKE File, Based on AllowedRun.dsp
!IF "$(CFG)" == ""
CFG=AllowedRun - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AllowedRun - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AllowedRun - Win32 Release" && "$(CFG)" != "AllowedRun - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AllowedRun.mak" CFG="AllowedRun - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AllowedRun - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AllowedRun - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AllowedRun - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\Release\AllowedRun.exe"


CLEAN :
	-@erase "$(INTDIR)\AllowedRun.obj"
	-@erase "$(INTDIR)\AllowedRun.res"
	-@erase "$(INTDIR)\AllowedRunDlg.obj"
	-@erase "$(INTDIR)\DApp.obj"
	-@erase "$(INTDIR)\DApplications.obj"
	-@erase "$(INTDIR)\DChanges.obj"
	-@erase "$(INTDIR)\DFolder.obj"
	-@erase "$(INTDIR)\DFolders.obj"
	-@erase "$(INTDIR)\EvCache.obj"
	-@erase "$(INTDIR)\FiltersArray.obj"
	-@erase "$(INTDIR)\FiltersStorage.obj"
	-@erase "$(INTDIR)\OwnTabCtrl.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AllowedRun.pdb"
	-@erase "..\..\Release\AllowedRun.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /Fp"$(INTDIR)\AllowedRun.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\AllowedRun.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AllowedRun.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=Win32Utils.lib FSDrvLib.lib sign.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\AllowedRun.pdb" /debug /machine:I386 /out:"..\..\Release/AllowedRun.exe" /libpath:"..\..\..\..\CommonFiles\ReleaseDll" 
LINK32_OBJS= \
	"$(INTDIR)\AllowedRun.obj" \
	"$(INTDIR)\AllowedRunDlg.obj" \
	"$(INTDIR)\DApp.obj" \
	"$(INTDIR)\DApplications.obj" \
	"$(INTDIR)\DChanges.obj" \
	"$(INTDIR)\DFolder.obj" \
	"$(INTDIR)\DFolders.obj" \
	"$(INTDIR)\EvCache.obj" \
	"$(INTDIR)\FiltersArray.obj" \
	"$(INTDIR)\FiltersStorage.obj" \
	"$(INTDIR)\OwnTabCtrl.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AllowedRun.res"

"..\..\Release\AllowedRun.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\WINDOWS\HOOK\Release\AllowedRun.exe
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\Release\AllowedRun.exe"
   tsigner \WINDOWS\HOOK\Release\AllowedRun.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "AllowedRun - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\Debug\AllowedRun.exe"


CLEAN :
	-@erase "$(INTDIR)\AllowedRun.obj"
	-@erase "$(INTDIR)\AllowedRun.res"
	-@erase "$(INTDIR)\AllowedRunDlg.obj"
	-@erase "$(INTDIR)\DApp.obj"
	-@erase "$(INTDIR)\DApplications.obj"
	-@erase "$(INTDIR)\DChanges.obj"
	-@erase "$(INTDIR)\DFolder.obj"
	-@erase "$(INTDIR)\DFolders.obj"
	-@erase "$(INTDIR)\EvCache.obj"
	-@erase "$(INTDIR)\FiltersArray.obj"
	-@erase "$(INTDIR)\FiltersStorage.obj"
	-@erase "$(INTDIR)\OwnTabCtrl.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AllowedRun.pdb"
	-@erase "..\..\Debug\AllowedRun.exe"
	-@erase "..\..\Debug\AllowedRun.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_UNICODE" /D "UNICODE" /Fp"$(INTDIR)\AllowedRun.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\AllowedRun.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AllowedRun.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=Win32Utils.lib FSDrvLib.lib sign.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\AllowedRun.pdb" /debug /machine:I386 /out:"..\..\Debug/AllowedRun.exe" /pdbtype:sept /libpath:"..\..\..\..\CommonFiles\DebugDll" 
LINK32_OBJS= \
	"$(INTDIR)\AllowedRun.obj" \
	"$(INTDIR)\AllowedRunDlg.obj" \
	"$(INTDIR)\DApp.obj" \
	"$(INTDIR)\DApplications.obj" \
	"$(INTDIR)\DChanges.obj" \
	"$(INTDIR)\DFolder.obj" \
	"$(INTDIR)\DFolders.obj" \
	"$(INTDIR)\EvCache.obj" \
	"$(INTDIR)\FiltersArray.obj" \
	"$(INTDIR)\FiltersStorage.obj" \
	"$(INTDIR)\OwnTabCtrl.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\AllowedRun.res"

"..\..\Debug\AllowedRun.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\WINDOWS\HOOK\Debug\AllowedRun.exe
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\Debug\AllowedRun.exe"
   tsigner \WINDOWS\HOOK\Debug\AllowedRun.exe
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
!IF EXISTS("AllowedRun.dep")
!INCLUDE "AllowedRun.dep"
!ELSE 
!MESSAGE Warning: cannot find "AllowedRun.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AllowedRun - Win32 Release" || "$(CFG)" == "AllowedRun - Win32 Debug"
SOURCE=.\AllowedRun.cpp

"$(INTDIR)\AllowedRun.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\AllowedRun.rc

"$(INTDIR)\AllowedRun.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\AllowedRunDlg.cpp

"$(INTDIR)\AllowedRunDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DApp.cpp

"$(INTDIR)\DApp.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DApplications.cpp

"$(INTDIR)\DApplications.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DChanges.cpp

"$(INTDIR)\DChanges.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DFolder.cpp

"$(INTDIR)\DFolder.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DFolders.cpp

"$(INTDIR)\DFolders.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EvCache.cpp

"$(INTDIR)\EvCache.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\FiltersArray.cpp

"$(INTDIR)\FiltersArray.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\FiltersStorage.cpp

"$(INTDIR)\FiltersStorage.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OwnTabCtrl.cpp

"$(INTDIR)\OwnTabCtrl.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\StdAfx.cpp

"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

