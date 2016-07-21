# Microsoft Developer Studio Generated NMAKE File, Based on Avp32.dsp
!IF "$(CFG)" == ""
CFG=Avp32 - Win32 Release
!MESSAGE No configuration specified. Defaulting to Avp32 - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Avp32 - Win32 Release" && "$(CFG)" != "Avp32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Avp32.mak" CFG="Avp32 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Avp32 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Avp32 - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Avp32 - Win32 Release"

OUTDIR=.\..\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\..\out\release\Avp32.exe" ".\Release\Avp32.pch"

!ELSE 

ALL : "AvpBase - Win32 Release" "Dos AVP_STUB - Win32 Release" "..\..\out\release\Avp32.exe" ".\Release\Avp32.pch"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Dos AVP_STUB - Win32 ReleaseCLEAN" "AvpBase - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\aboutviewctrl.obj"
	-@erase "$(INTDIR)\Agentint.obj"
	-@erase "$(INTDIR)\Avp32.obj"
	-@erase "$(INTDIR)\Avp32.pch"
	-@erase "$(INTDIR)\Avp32.res"
	-@erase "$(INTDIR)\Avp32Dlg.obj"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CheckSelfExp.obj"
	-@erase "$(INTDIR)\CheckUse.obj"
	-@erase "$(INTDIR)\cust.obj"
	-@erase "$(INTDIR)\DisDlg.obj"
	-@erase "$(INTDIR)\DlgProxy.obj"
	-@erase "$(INTDIR)\FindDlg.obj"
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\LoadCCClientDLL.obj"
	-@erase "$(INTDIR)\LoadKeys.obj"
	-@erase "$(INTDIR)\LogCtrl.obj"
	-@erase "$(INTDIR)\NewToolbar.obj"
	-@erase "$(INTDIR)\Policy.obj"
	-@erase "$(INTDIR)\reportviewctrl.obj"
	-@erase "$(INTDIR)\Scandir.obj"
	-@erase "$(INTDIR)\Sq_u.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TabPages.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp32.pdb"
	-@erase "..\..\out\release\Avp32.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Avp32.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Avp32.res" /i "..\..\commonfiles" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp32.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=winmm.lib mpr.lib imagehlp.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Avp32.pdb" /debug /machine:I386 /out:"../../out/release/Avp32.exe" /libpath:"..\release" /stub:$(IntDir)\avp_stub.exe /MAPINFO:LINES /fixed:no 
LINK32_OBJS= \
	"$(INTDIR)\aboutviewctrl.obj" \
	"$(INTDIR)\Agentint.obj" \
	"$(INTDIR)\Avp32.obj" \
	"$(INTDIR)\Avp32Dlg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CheckSelfExp.obj" \
	"$(INTDIR)\CheckUse.obj" \
	"$(INTDIR)\cust.obj" \
	"$(INTDIR)\DisDlg.obj" \
	"$(INTDIR)\DlgProxy.obj" \
	"$(INTDIR)\FindDlg.obj" \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCCClientDLL.obj" \
	"$(INTDIR)\LoadKeys.obj" \
	"$(INTDIR)\LogCtrl.obj" \
	"$(INTDIR)\NewToolbar.obj" \
	"$(INTDIR)\Policy.obj" \
	"$(INTDIR)\reportviewctrl.obj" \
	"$(INTDIR)\Scandir.obj" \
	"$(INTDIR)\Sq_u.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TabPages.obj" \
	"$(INTDIR)\Avp32.res" \
	"..\..\CommonFiles\ReleaseDll\Sign.lib" \
	"..\..\CommonFiles\ReleaseDll\SWM.lib" \
	"..\..\CommonFiles\ReleaseDll\Property.lib" \
	"..\..\CommonFiles\ReleaseDll\KLDTSer.lib" \
	"..\..\out\Release\AvpBase.lib"

"..\..\out\release\Avp32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\release\Avp32.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "AvpBase - Win32 Release" "Dos AVP_STUB - Win32 Release" "..\..\out\release\Avp32.exe" ".\Release\Avp32.pch"
   tsigner \out\release\Avp32.exe
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

OUTDIR=.\..\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\..\out\debug\Avp32.exe" ".\Debug\Avp32.pch" "$(OUTDIR)\Avp32.bsc"

!ELSE 

ALL : "AvpBase - Win32 Debug" "Dos AVP_STUB - Win32 Debug" "..\..\out\debug\Avp32.exe" ".\Debug\Avp32.pch" "$(OUTDIR)\Avp32.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Dos AVP_STUB - Win32 DebugCLEAN" "AvpBase - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\aboutviewctrl.obj"
	-@erase "$(INTDIR)\aboutviewctrl.sbr"
	-@erase "$(INTDIR)\Agentint.obj"
	-@erase "$(INTDIR)\Agentint.sbr"
	-@erase "$(INTDIR)\Avp32.obj"
	-@erase "$(INTDIR)\Avp32.pch"
	-@erase "$(INTDIR)\Avp32.res"
	-@erase "$(INTDIR)\Avp32.sbr"
	-@erase "$(INTDIR)\Avp32Dlg.obj"
	-@erase "$(INTDIR)\Avp32Dlg.sbr"
	-@erase "$(INTDIR)\CalcSum.obj"
	-@erase "$(INTDIR)\CalcSum.sbr"
	-@erase "$(INTDIR)\CheckSelfExp.obj"
	-@erase "$(INTDIR)\CheckSelfExp.sbr"
	-@erase "$(INTDIR)\CheckUse.obj"
	-@erase "$(INTDIR)\CheckUse.sbr"
	-@erase "$(INTDIR)\cust.obj"
	-@erase "$(INTDIR)\cust.sbr"
	-@erase "$(INTDIR)\DisDlg.obj"
	-@erase "$(INTDIR)\DisDlg.sbr"
	-@erase "$(INTDIR)\DlgProxy.obj"
	-@erase "$(INTDIR)\DlgProxy.sbr"
	-@erase "$(INTDIR)\FindDlg.obj"
	-@erase "$(INTDIR)\FindDlg.sbr"
	-@erase "$(INTDIR)\Globals.obj"
	-@erase "$(INTDIR)\Globals.sbr"
	-@erase "$(INTDIR)\Layout.obj"
	-@erase "$(INTDIR)\Layout.sbr"
	-@erase "$(INTDIR)\LoadCCClientDLL.obj"
	-@erase "$(INTDIR)\LoadCCClientDLL.sbr"
	-@erase "$(INTDIR)\LoadKeys.obj"
	-@erase "$(INTDIR)\LoadKeys.sbr"
	-@erase "$(INTDIR)\LogCtrl.obj"
	-@erase "$(INTDIR)\LogCtrl.sbr"
	-@erase "$(INTDIR)\NewToolbar.obj"
	-@erase "$(INTDIR)\NewToolbar.sbr"
	-@erase "$(INTDIR)\Policy.obj"
	-@erase "$(INTDIR)\Policy.sbr"
	-@erase "$(INTDIR)\reportviewctrl.obj"
	-@erase "$(INTDIR)\reportviewctrl.sbr"
	-@erase "$(INTDIR)\Scandir.obj"
	-@erase "$(INTDIR)\Scandir.sbr"
	-@erase "$(INTDIR)\Sq_u.obj"
	-@erase "$(INTDIR)\Sq_u.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TabPages.obj"
	-@erase "$(INTDIR)\TabPages.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Avp32.bsc"
	-@erase "$(OUTDIR)\Avp32.pdb"
	-@erase "..\..\out\debug\Avp32.exe"
	-@erase "..\..\out\debug\Avp32.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Avp32.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Avp32.res" /i "..\..\commonfiles" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Avp32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\aboutviewctrl.sbr" \
	"$(INTDIR)\Agentint.sbr" \
	"$(INTDIR)\Avp32.sbr" \
	"$(INTDIR)\Avp32Dlg.sbr" \
	"$(INTDIR)\CalcSum.sbr" \
	"$(INTDIR)\CheckSelfExp.sbr" \
	"$(INTDIR)\CheckUse.sbr" \
	"$(INTDIR)\cust.sbr" \
	"$(INTDIR)\DisDlg.sbr" \
	"$(INTDIR)\DlgProxy.sbr" \
	"$(INTDIR)\FindDlg.sbr" \
	"$(INTDIR)\Globals.sbr" \
	"$(INTDIR)\Layout.sbr" \
	"$(INTDIR)\LoadCCClientDLL.sbr" \
	"$(INTDIR)\LoadKeys.sbr" \
	"$(INTDIR)\LogCtrl.sbr" \
	"$(INTDIR)\NewToolbar.sbr" \
	"$(INTDIR)\Policy.sbr" \
	"$(INTDIR)\reportviewctrl.sbr" \
	"$(INTDIR)\Scandir.sbr" \
	"$(INTDIR)\Sq_u.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TabPages.sbr"

"$(OUTDIR)\Avp32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=winmm.lib mpr.lib imagehlp.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Avp32.pdb" /debug /machine:I386 /out:"../../out/debug/Avp32.exe" /libpath:"..\debug" /stub:$(IntDir)\avp_stub.exe /IGNORE:4098 
LINK32_OBJS= \
	"$(INTDIR)\aboutviewctrl.obj" \
	"$(INTDIR)\Agentint.obj" \
	"$(INTDIR)\Avp32.obj" \
	"$(INTDIR)\Avp32Dlg.obj" \
	"$(INTDIR)\CalcSum.obj" \
	"$(INTDIR)\CheckSelfExp.obj" \
	"$(INTDIR)\CheckUse.obj" \
	"$(INTDIR)\cust.obj" \
	"$(INTDIR)\DisDlg.obj" \
	"$(INTDIR)\DlgProxy.obj" \
	"$(INTDIR)\FindDlg.obj" \
	"$(INTDIR)\Globals.obj" \
	"$(INTDIR)\Layout.obj" \
	"$(INTDIR)\LoadCCClientDLL.obj" \
	"$(INTDIR)\LoadKeys.obj" \
	"$(INTDIR)\LogCtrl.obj" \
	"$(INTDIR)\NewToolbar.obj" \
	"$(INTDIR)\Policy.obj" \
	"$(INTDIR)\reportviewctrl.obj" \
	"$(INTDIR)\Scandir.obj" \
	"$(INTDIR)\Sq_u.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TabPages.obj" \
	"$(INTDIR)\Avp32.res" \
	"..\..\CommonFiles\ReleaseDll\Sign.lib" \
	"..\..\CommonFiles\ReleaseDll\SWM.lib" \
	"..\..\CommonFiles\ReleaseDll\Property.lib" \
	"..\..\CommonFiles\ReleaseDll\KLDTSer.lib" \
	"..\..\out\Debug\AvpBase.lib"

"..\..\out\debug\Avp32.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\debug\Avp32.exe
SOURCE="$(InputPath)"
PostBuild_Desc=Finalizing...
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\..\Debug
# End Custom Macros

$(DS_POSTBUILD_DEP) : "AvpBase - Win32 Debug" "Dos AVP_STUB - Win32 Debug" "..\..\out\debug\Avp32.exe" ".\Debug\Avp32.pch" "$(OUTDIR)\Avp32.bsc"
   tsigner \out\debug\Avp32.exe
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
!IF EXISTS("Avp32.dep")
!INCLUDE "Avp32.dep"
!ELSE 
!MESSAGE Warning: cannot find "Avp32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Avp32 - Win32 Release" || "$(CFG)" == "Avp32 - Win32 Debug"
SOURCE=.\aboutviewctrl.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\aboutviewctrl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\aboutviewctrl.obj"	"$(INTDIR)\aboutviewctrl.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Agentint.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Agentint.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Agentint.obj"	"$(INTDIR)\Agentint.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Avp32.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Avp32.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Avp32.obj"	"$(INTDIR)\Avp32.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Avp32Dlg.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Avp32Dlg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Avp32Dlg.obj"	"$(INTDIR)\Avp32Dlg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\CommonFiles\Stuff\CalcSum.c

!IF  "$(CFG)" == "Avp32 - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CalcSum.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\CalcSum.obj"	"$(INTDIR)\CalcSum.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\CommonFiles\LoadKeys\CheckSelfExp.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\CheckSelfExp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\CheckSelfExp.obj"	"$(INTDIR)\CheckSelfExp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\CommonFiles\LoadKeys\CheckUse.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\CheckUse.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\CheckUse.obj"	"$(INTDIR)\CheckUse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\cust.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\cust.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\cust.obj"	"$(INTDIR)\cust.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DisDlg.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\DisDlg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\DisDlg.obj"	"$(INTDIR)\DisDlg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\DlgProxy.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\DlgProxy.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\DlgProxy.obj"	"$(INTDIR)\DlgProxy.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\FindDlg.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\FindDlg.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\FindDlg.obj"	"$(INTDIR)\FindDlg.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\CommonFiles\StuffMFC\Globals.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Globals.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Globals.obj"	"$(INTDIR)\Globals.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\CommonFiles\Stuff\Layout.c

!IF  "$(CFG)" == "Avp32 - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Avp32.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Layout.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Avp32.pch" /YX"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\Layout.obj"	"$(INTDIR)\Layout.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\CommonFiles\CCClient\LoadCCClientDLL.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\LoadCCClientDLL.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\LoadCCClientDLL.obj"	"$(INTDIR)\LoadCCClientDLL.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\CommonFiles\LoadKeys\LoadKeys.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\LoadKeys.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\LoadKeys.obj"	"$(INTDIR)\LoadKeys.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\LogCtrl.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\LogCtrl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\LogCtrl.obj"	"$(INTDIR)\LogCtrl.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\CommonFiles\StuffMFC\NewToolbar.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\NewToolbar.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\NewToolbar.obj"	"$(INTDIR)\NewToolbar.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\CommonFiles\CCClient\Policy.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Policy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Policy.obj"	"$(INTDIR)\Policy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\reportviewctrl.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\reportviewctrl.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\reportviewctrl.obj"	"$(INTDIR)\reportviewctrl.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Scandir.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Scandir.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Scandir.obj"	"$(INTDIR)\Scandir.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\CommonFiles\Stuff\Sq_u.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\Sq_u.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\Sq_u.obj"	"$(INTDIR)\Sq_u.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Zi /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\Avp32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Avp32.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

CPP_SWITCHES=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\commonfiles" /I "..\\" /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Avp32.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Avp32.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TabPages.cpp

!IF  "$(CFG)" == "Avp32 - Win32 Release"


"$(INTDIR)\TabPages.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"


"$(INTDIR)\TabPages.obj"	"$(INTDIR)\TabPages.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Avp32.rc

"$(INTDIR)\Avp32.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Avp32 - Win32 Release"

"Dos AVP_STUB - Win32 Release" : 
   cd "."
   nmake /f"avp_stub.mak" CFG=..\..\out\release
   cd "."

"Dos AVP_STUB - Win32 ReleaseCLEAN" : 
   cd "."
   cd "."

!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

"Dos AVP_STUB - Win32 Debug" : 
   cd "."
   nmake /f"avp_stub.mak" CFG=..\..\out\debug
   cd "."

"Dos AVP_STUB - Win32 DebugCLEAN" : 
   cd "."
   cd "."

!ENDIF 

!IF  "$(CFG)" == "Avp32 - Win32 Release"

"AvpBase - Win32 Release" : 
   cd "\avp32\AvpBase"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpBase.mak CFG="AvpBase - Win32 Release" 
   cd "..\AVP_32"

"AvpBase - Win32 ReleaseCLEAN" : 
   cd "\avp32\AvpBase"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpBase.mak CFG="AvpBase - Win32 Release" RECURSE=1 CLEAN 
   cd "..\AVP_32"

!ELSEIF  "$(CFG)" == "Avp32 - Win32 Debug"

"AvpBase - Win32 Debug" : 
   cd "\avp32\AvpBase"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpBase.mak CFG="AvpBase - Win32 Debug" 
   cd "..\AVP_32"

"AvpBase - Win32 DebugCLEAN" : 
   cd "\avp32\AvpBase"
   $(MAKE) /$(MAKEFLAGS) /F .\AvpBase.mak CFG="AvpBase - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\AVP_32"

!ENDIF 


!ENDIF 

