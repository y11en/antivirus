# Microsoft Developer Studio Generated NMAKE File, Based on avpgui.dsp
!IF "$(CFG)" == ""
CFG=avpgui - Win32 Debug
!MESSAGE No configuration specified. Defaulting to avpgui - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "avpgui - Win32 Release" && "$(CFG)" != "avpgui - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avpgui.mak" CFG="avpgui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avpgui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avpgui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avpgui - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/avpgui
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\avpgui.ppl" "$(OUTDIR)\avpgui.tlb" ".\comgui.h" ".\avpgui_i.c" "..\..\temp\Release\ppp\avpgui\avpgui.pch"

!ELSE 

ALL : "params - Win32 Release" "basegui - Win32 Release" "$(OUTDIR)\avpgui.ppl" "$(OUTDIR)\avpgui.tlb" ".\comgui.h" ".\avpgui_i.c" "..\..\temp\Release\ppp\avpgui\avpgui.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"basegui - Win32 ReleaseCLEAN" "params - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AlertDialog.obj"
	-@erase "$(INTDIR)\avpgui.obj"
	-@erase "$(INTDIR)\avpgui.pch"
	-@erase "$(INTDIR)\avpgui.res"
	-@erase "$(INTDIR)\ComAvpGui.obj"
	-@erase "$(INTDIR)\FWRules.obj"
	-@erase "$(INTDIR)\ListsView.obj"
	-@erase "$(INTDIR)\MainWindow.obj"
	-@erase "$(INTDIR)\NewsReader.obj"
	-@erase "$(INTDIR)\plugin_gui.obj"
	-@erase "$(INTDIR)\ProductInfoCab.obj"
	-@erase "$(INTDIR)\ProfilesView.obj"
	-@erase "$(INTDIR)\ReportWindow.obj"
	-@erase "$(INTDIR)\RootSink.obj"
	-@erase "$(INTDIR)\ScanObjects.obj"
	-@erase "$(INTDIR)\ServiceSettings.obj"
	-@erase "$(INTDIR)\ServicesView.obj"
	-@erase "$(INTDIR)\SettingsDlgs.obj"
	-@erase "$(INTDIR)\SettingsWindow.obj"
	-@erase "$(INTDIR)\ss.obj"
	-@erase "$(INTDIR)\StatusWindow.obj"
	-@erase "$(INTDIR)\TaskProfile.obj"
	-@erase "$(INTDIR)\TaskProfileProduct.obj"
	-@erase "$(INTDIR)\TaskView.obj"
	-@erase "$(INTDIR)\Test.obj"
	-@erase "$(INTDIR)\ThreatsView.obj"
	-@erase "$(INTDIR)\tmclient.obj"
	-@erase "$(INTDIR)\TrayIcon.obj"
	-@erase "$(INTDIR)\UpgradeFrom46.obj"
	-@erase "$(INTDIR)\UpgradeFrom50.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Wizards.obj"
	-@erase "$(OUTDIR)\avpgui.exp"
	-@erase "$(OUTDIR)\avpgui.pdb"
	-@erase "$(OUTDIR)\avpgui.ppl"
	-@erase "..\..\out\Release\avpgui.tlb"
	-@erase ".\avpgui_i.c"
	-@erase ".\comgui.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_PRAGUE_TRACE_" /D "XTREE_MEM_PRAGUE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avpgui.res" /i "..\..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avpgui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=basegui.lib /nologo /base:"0x62000000" /dll /incremental:no /pdb:"$(OUTDIR)\avpgui.pdb" /debug /machine:I386 /def:".\avpgui.def" /out:"$(OUTDIR)\avpgui.ppl" /implib:"$(OUTDIR)\avpgui.lib" /libpath:"..\..\CommonFiles\msvcrt42" /libpath:"..\..\CommonFiles\ReleaseDll" /libpath:"../../out/Release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\ComAvpGui.obj" \
	"$(INTDIR)\AlertDialog.obj" \
	"$(INTDIR)\avpgui.obj" \
	"$(INTDIR)\FWRules.obj" \
	"$(INTDIR)\ListsView.obj" \
	"$(INTDIR)\MainWindow.obj" \
	"$(INTDIR)\plugin_gui.obj" \
	"$(INTDIR)\ProductInfoCab.obj" \
	"$(INTDIR)\ProfilesView.obj" \
	"$(INTDIR)\ReportWindow.obj" \
	"$(INTDIR)\RootSink.obj" \
	"$(INTDIR)\ScanObjects.obj" \
	"$(INTDIR)\ServiceSettings.obj" \
	"$(INTDIR)\ServicesView.obj" \
	"$(INTDIR)\SettingsDlgs.obj" \
	"$(INTDIR)\SettingsWindow.obj" \
	"$(INTDIR)\ss.obj" \
	"$(INTDIR)\StatusWindow.obj" \
	"$(INTDIR)\TaskProfile.obj" \
	"$(INTDIR)\TaskProfileProduct.obj" \
	"$(INTDIR)\TaskView.obj" \
	"$(INTDIR)\Test.obj" \
	"$(INTDIR)\ThreatsView.obj" \
	"$(INTDIR)\tmclient.obj" \
	"$(INTDIR)\TrayIcon.obj" \
	"$(INTDIR)\UpgradeFrom46.obj" \
	"$(INTDIR)\UpgradeFrom50.obj" \
	"$(INTDIR)\Wizards.obj" \
	"$(INTDIR)\avpgui.res" \
	"$(INTDIR)\NewsReader.obj"

"$(OUTDIR)\avpgui.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\Projects\KAVP7\out\Release\avpgui.ppl
SOURCE="$(InputPath)"
PostBuild_Desc=Postbuild...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "params - Win32 Release" "basegui - Win32 Release" "$(OUTDIR)\avpgui.ppl" "$(OUTDIR)\avpgui.tlb" ".\comgui.h" ".\avpgui_i.c" "..\..\temp\Release\ppp\avpgui\avpgui.pch"
   tsigner "\Projects\KAVP7\out\Release\avpgui.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/avpgui
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "\Projects\KAVP7\out\Debug\skin\skin.ini" "$(OUTDIR)\avpgui.ppl" "$(OUTDIR)\avpgui.tlb" ".\comgui.h" ".\avpgui_i.c" "..\..\temp\Debug\ppp\avpgui\avpgui.pch" "$(OUTDIR)\avpgui.bsc" ".\../../out/Debug\regsvr32.trg"

!ELSE 

ALL : "params - Win32 Debug" "basegui - Win32 Debug" "\Projects\KAVP7\out\Debug\skin\skin.ini" "$(OUTDIR)\avpgui.ppl" "$(OUTDIR)\avpgui.tlb" ".\comgui.h" ".\avpgui_i.c" "..\..\temp\Debug\ppp\avpgui\avpgui.pch" "$(OUTDIR)\avpgui.bsc" ".\../../out/Debug\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"basegui - Win32 DebugCLEAN" "params - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\AlertDialog.obj"
	-@erase "$(INTDIR)\AlertDialog.sbr"
	-@erase "$(INTDIR)\avpgui.obj"
	-@erase "$(INTDIR)\avpgui.pch"
	-@erase "$(INTDIR)\avpgui.res"
	-@erase "$(INTDIR)\avpgui.sbr"
	-@erase "$(INTDIR)\ComAvpGui.obj"
	-@erase "$(INTDIR)\ComAvpGui.sbr"
	-@erase "$(INTDIR)\FWRules.obj"
	-@erase "$(INTDIR)\FWRules.sbr"
	-@erase "$(INTDIR)\ListsView.obj"
	-@erase "$(INTDIR)\ListsView.sbr"
	-@erase "$(INTDIR)\MainWindow.obj"
	-@erase "$(INTDIR)\MainWindow.sbr"
	-@erase "$(INTDIR)\NewsReader.obj"
	-@erase "$(INTDIR)\NewsReader.sbr"
	-@erase "$(INTDIR)\plugin_gui.obj"
	-@erase "$(INTDIR)\plugin_gui.sbr"
	-@erase "$(INTDIR)\ProductInfoCab.obj"
	-@erase "$(INTDIR)\ProductInfoCab.sbr"
	-@erase "$(INTDIR)\ProfilesView.obj"
	-@erase "$(INTDIR)\ProfilesView.sbr"
	-@erase "$(INTDIR)\ReportWindow.obj"
	-@erase "$(INTDIR)\ReportWindow.sbr"
	-@erase "$(INTDIR)\RootSink.obj"
	-@erase "$(INTDIR)\RootSink.sbr"
	-@erase "$(INTDIR)\ScanObjects.obj"
	-@erase "$(INTDIR)\ScanObjects.sbr"
	-@erase "$(INTDIR)\ServiceSettings.obj"
	-@erase "$(INTDIR)\ServiceSettings.sbr"
	-@erase "$(INTDIR)\ServicesView.obj"
	-@erase "$(INTDIR)\ServicesView.sbr"
	-@erase "$(INTDIR)\SettingsDlgs.obj"
	-@erase "$(INTDIR)\SettingsDlgs.sbr"
	-@erase "$(INTDIR)\SettingsWindow.obj"
	-@erase "$(INTDIR)\SettingsWindow.sbr"
	-@erase "$(INTDIR)\ss.obj"
	-@erase "$(INTDIR)\ss.sbr"
	-@erase "$(INTDIR)\StatusWindow.obj"
	-@erase "$(INTDIR)\StatusWindow.sbr"
	-@erase "$(INTDIR)\TaskProfile.obj"
	-@erase "$(INTDIR)\TaskProfile.sbr"
	-@erase "$(INTDIR)\TaskProfileProduct.obj"
	-@erase "$(INTDIR)\TaskProfileProduct.sbr"
	-@erase "$(INTDIR)\TaskView.obj"
	-@erase "$(INTDIR)\TaskView.sbr"
	-@erase "$(INTDIR)\Test.obj"
	-@erase "$(INTDIR)\Test.sbr"
	-@erase "$(INTDIR)\ThreatsView.obj"
	-@erase "$(INTDIR)\ThreatsView.sbr"
	-@erase "$(INTDIR)\tmclient.obj"
	-@erase "$(INTDIR)\tmclient.sbr"
	-@erase "$(INTDIR)\TrayIcon.obj"
	-@erase "$(INTDIR)\TrayIcon.sbr"
	-@erase "$(INTDIR)\UpgradeFrom46.obj"
	-@erase "$(INTDIR)\UpgradeFrom46.sbr"
	-@erase "$(INTDIR)\UpgradeFrom50.obj"
	-@erase "$(INTDIR)\UpgradeFrom50.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\Wizards.obj"
	-@erase "$(INTDIR)\Wizards.sbr"
	-@erase "$(OUTDIR)\avpgui.bsc"
	-@erase "$(OUTDIR)\avpgui.exp"
	-@erase "$(OUTDIR)\avpgui.ilk"
	-@erase "$(OUTDIR)\avpgui.pdb"
	-@erase "$(OUTDIR)\avpgui.ppl"
	-@erase "..\..\out\Debug\avpgui.tlb"
	-@erase ".\avpgui_i.c"
	-@erase ".\comgui.h"
	-@erase ".\../../out/Debug\regsvr32.trg"
	-@erase "\Projects\KAVP7\out\Debug\skin\skin.ini"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XTREE_MEM_PRAGUE" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\avpgui.res" /i "..\..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avpgui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\ComAvpGui.sbr" \
	"$(INTDIR)\AlertDialog.sbr" \
	"$(INTDIR)\avpgui.sbr" \
	"$(INTDIR)\FWRules.sbr" \
	"$(INTDIR)\ListsView.sbr" \
	"$(INTDIR)\MainWindow.sbr" \
	"$(INTDIR)\plugin_gui.sbr" \
	"$(INTDIR)\ProductInfoCab.sbr" \
	"$(INTDIR)\ProfilesView.sbr" \
	"$(INTDIR)\ReportWindow.sbr" \
	"$(INTDIR)\RootSink.sbr" \
	"$(INTDIR)\ScanObjects.sbr" \
	"$(INTDIR)\ServiceSettings.sbr" \
	"$(INTDIR)\ServicesView.sbr" \
	"$(INTDIR)\SettingsDlgs.sbr" \
	"$(INTDIR)\SettingsWindow.sbr" \
	"$(INTDIR)\ss.sbr" \
	"$(INTDIR)\StatusWindow.sbr" \
	"$(INTDIR)\TaskProfile.sbr" \
	"$(INTDIR)\TaskProfileProduct.sbr" \
	"$(INTDIR)\TaskView.sbr" \
	"$(INTDIR)\Test.sbr" \
	"$(INTDIR)\ThreatsView.sbr" \
	"$(INTDIR)\tmclient.sbr" \
	"$(INTDIR)\TrayIcon.sbr" \
	"$(INTDIR)\UpgradeFrom46.sbr" \
	"$(INTDIR)\UpgradeFrom50.sbr" \
	"$(INTDIR)\Wizards.sbr" \
	"$(INTDIR)\NewsReader.sbr"

"$(OUTDIR)\avpgui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=basegui.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\avpgui.pdb" /debug /machine:I386 /def:".\avpgui.def" /out:"$(OUTDIR)\avpgui.ppl" /implib:"$(OUTDIR)\avpgui.lib" /libpath:"..\..\CommonFiles\DebugDll" /libpath:"../../out/Debug" 
DEF_FILE= \
	".\avpgui.def"
LINK32_OBJS= \
	"$(INTDIR)\ComAvpGui.obj" \
	"$(INTDIR)\AlertDialog.obj" \
	"$(INTDIR)\avpgui.obj" \
	"$(INTDIR)\FWRules.obj" \
	"$(INTDIR)\ListsView.obj" \
	"$(INTDIR)\MainWindow.obj" \
	"$(INTDIR)\plugin_gui.obj" \
	"$(INTDIR)\ProductInfoCab.obj" \
	"$(INTDIR)\ProfilesView.obj" \
	"$(INTDIR)\ReportWindow.obj" \
	"$(INTDIR)\RootSink.obj" \
	"$(INTDIR)\ScanObjects.obj" \
	"$(INTDIR)\ServiceSettings.obj" \
	"$(INTDIR)\ServicesView.obj" \
	"$(INTDIR)\SettingsDlgs.obj" \
	"$(INTDIR)\SettingsWindow.obj" \
	"$(INTDIR)\ss.obj" \
	"$(INTDIR)\StatusWindow.obj" \
	"$(INTDIR)\TaskProfile.obj" \
	"$(INTDIR)\TaskProfileProduct.obj" \
	"$(INTDIR)\TaskView.obj" \
	"$(INTDIR)\Test.obj" \
	"$(INTDIR)\ThreatsView.obj" \
	"$(INTDIR)\tmclient.obj" \
	"$(INTDIR)\TrayIcon.obj" \
	"$(INTDIR)\UpgradeFrom46.obj" \
	"$(INTDIR)\UpgradeFrom50.obj" \
	"$(INTDIR)\Wizards.obj" \
	"$(INTDIR)\avpgui.res" \
	"$(INTDIR)\NewsReader.obj"

"$(OUTDIR)\avpgui.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\../../out/Debug
TargetPath=\Projects\KAVP7\out\Debug\avpgui.ppl
InputPath=\Projects\KAVP7\out\Debug\avpgui.ppl
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	
ProjDir=.
TargetDir=\Projects\KAVP7\out\Debug
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "params - Win32 Debug" "basegui - Win32 Debug" "\Projects\KAVP7\out\Debug\skin\skin.ini" "$(OUTDIR)\avpgui.ppl" "$(OUTDIR)\avpgui.tlb" ".\comgui.h" ".\avpgui_i.c" "..\..\temp\Debug\ppp\avpgui\avpgui.pch" "$(OUTDIR)\avpgui.bsc" ".\../../out/Debug\regsvr32.trg"
   xcopy ".\skin" "\Projects\KAVP7\out\Debug\skin" /Y /R /S /I
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
!IF EXISTS("avpgui.dep")
!INCLUDE "avpgui.dep"
!ELSE 
!MESSAGE Warning: cannot find "avpgui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avpgui - Win32 Release" || "$(CFG)" == "avpgui - Win32 Debug"
SOURCE=.\avpgui.idl

!IF  "$(CFG)" == "avpgui - Win32 Release"

MTL_SWITCHES=/nologo /D "NDEBUG" /tlb "$(OUTDIR)\avpgui.tlb" /h "comgui.h" /iid "avpgui_i.c" /Oicf /win32 

"..\..\out\Release\avpgui.tlb"	".\comgui.h"	".\avpgui_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

MTL_SWITCHES=/nologo /D "_DEBUG" /tlb "$(OUTDIR)\avpgui.tlb" /h "comgui.h" /iid "avpgui_i.c" /win32 

"..\..\out\Debug\avpgui.tlb"	".\comgui.h"	".\avpgui_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\basegui.idl
SOURCE=.\ComAvpGui.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ComAvpGui.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ComAvpGui.obj"	"$(INTDIR)\ComAvpGui.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ComBaseGui.cpp
SOURCE=.\AlertDialog.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\AlertDialog.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\AlertDialog.obj"	"$(INTDIR)\AlertDialog.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\avpgui.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /I "../include/licensing" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_PRAGUE_TRACE_" /D "XTREE_MEM_PRAGUE" /Fp"$(INTDIR)\avpgui.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\avpgui.obj"	"$(INTDIR)\avpgui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XTREE_MEM_PRAGUE" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\avpgui.obj"	"$(INTDIR)\avpgui.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\FWRules.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\FWRules.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\FWRules.obj"	"$(INTDIR)\FWRules.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ListsView.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ListsView.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ListsView.obj"	"$(INTDIR)\ListsView.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\MainWindow.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\MainWindow.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\MainWindow.obj"	"$(INTDIR)\MainWindow.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\NewsReader.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\NewsReader.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\NewsReader.obj"	"$(INTDIR)\NewsReader.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\plugin_gui.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /I "../include/licensing" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_PRAGUE_TRACE_" /D "XTREE_MEM_PRAGUE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\plugin_gui.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../CommonFiles" /I "../include" /I "../../prague/include" /I "../../prague" /I "../../Updater60/include" /I "..\..\CommonFiles" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "XTREE_MEM_PRAGUE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\avpgui.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\plugin_gui.obj"	"$(INTDIR)\plugin_gui.sbr"	"$(INTDIR)\avpgui.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ProductInfoCab.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ProductInfoCab.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ProductInfoCab.obj"	"$(INTDIR)\ProductInfoCab.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ProfilesView.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ProfilesView.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ProfilesView.obj"	"$(INTDIR)\ProfilesView.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ReportWindow.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ReportWindow.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ReportWindow.obj"	"$(INTDIR)\ReportWindow.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\RootSink.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\RootSink.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\RootSink.obj"	"$(INTDIR)\RootSink.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ScanObjects.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ScanObjects.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ScanObjects.obj"	"$(INTDIR)\ScanObjects.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ServiceSettings.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ServiceSettings.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ServiceSettings.obj"	"$(INTDIR)\ServiceSettings.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ServicesView.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ServicesView.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ServicesView.obj"	"$(INTDIR)\ServicesView.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SettingsDlgs.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\SettingsDlgs.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\SettingsDlgs.obj"	"$(INTDIR)\SettingsDlgs.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\SettingsWindow.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\SettingsWindow.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\SettingsWindow.obj"	"$(INTDIR)\SettingsWindow.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\Upgrade\ss.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ss.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ss.obj"	"$(INTDIR)\ss.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\StatusWindow.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\StatusWindow.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\StatusWindow.obj"	"$(INTDIR)\StatusWindow.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TaskProfile.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\TaskProfile.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\TaskProfile.obj"	"$(INTDIR)\TaskProfile.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TaskProfileProduct.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\TaskProfileProduct.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\TaskProfileProduct.obj"	"$(INTDIR)\TaskProfileProduct.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TaskView.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\TaskView.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\TaskView.obj"	"$(INTDIR)\TaskView.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Test.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\Test.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\Test.obj"	"$(INTDIR)\Test.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ThreatsView.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\ThreatsView.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\ThreatsView.obj"	"$(INTDIR)\ThreatsView.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\tmclient.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\tmclient.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\tmclient.obj"	"$(INTDIR)\tmclient.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\TrayIcon.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\TrayIcon.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\TrayIcon.obj"	"$(INTDIR)\TrayIcon.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\UpgradeFrom46.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\UpgradeFrom46.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\UpgradeFrom46.obj"	"$(INTDIR)\UpgradeFrom46.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\UpgradeFrom50.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\UpgradeFrom50.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\UpgradeFrom50.obj"	"$(INTDIR)\UpgradeFrom50.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Wizards.cpp

!IF  "$(CFG)" == "avpgui - Win32 Release"


"$(INTDIR)\Wizards.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"


"$(INTDIR)\Wizards.obj"	"$(INTDIR)\Wizards.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\avpgui.rc

"$(INTDIR)\avpgui.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\skin\skin.ini

!IF  "$(CFG)" == "avpgui - Win32 Release"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

ProjDir=.
TargetDir=\Projects\KAVP7\out\Debug
InputPath=.\skin\skin.ini

"..\..\out\Debug\skin\skin.ini" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	xcopy "$(ProjDir)\skin" "$(TargetDir)\skin" /Y /R /S /I
<< 
	

!ENDIF 

!IF  "$(CFG)" == "avpgui - Win32 Release"

"basegui - Win32 Release" : 
   cd "\Projects\KAVP7\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Release" 
   cd "..\gui60"

"basegui - Win32 ReleaseCLEAN" : 
   cd "\Projects\KAVP7\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Release" RECURSE=1 CLEAN 
   cd "..\gui60"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

"basegui - Win32 Debug" : 
   cd "\Projects\KAVP7\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Debug" 
   cd "..\gui60"

"basegui - Win32 DebugCLEAN" : 
   cd "\Projects\KAVP7\PPP\basegui"
   $(MAKE) /$(MAKEFLAGS) /F .\basegui.mak CFG="basegui - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\gui60"

!ENDIF 

!IF  "$(CFG)" == "avpgui - Win32 Release"

"params - Win32 Release" : 
   cd "\Projects\KAVP7\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Release" 
   cd "..\gui60"

"params - Win32 ReleaseCLEAN" : 
   cd "\Projects\KAVP7\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Release" RECURSE=1 CLEAN 
   cd "..\gui60"

!ELSEIF  "$(CFG)" == "avpgui - Win32 Debug"

"params - Win32 Debug" : 
   cd "\Projects\KAVP7\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Debug" 
   cd "..\gui60"

"params - Win32 DebugCLEAN" : 
   cd "\Projects\KAVP7\PPP\params"
   $(MAKE) /$(MAKEFLAGS) /F .\params.mak CFG="params - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\gui60"

!ENDIF 


!ENDIF 

