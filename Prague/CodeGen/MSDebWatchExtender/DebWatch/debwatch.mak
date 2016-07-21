# Microsoft Developer Studio Generated NMAKE File, Based on debwatch.dsp
!IF "$(CFG)" == ""
CFG=debwatch - Win32 Debug
!MESSAGE No configuration specified. Defaulting to debwatch - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "debwatch - Win32 Release" && "$(CFG)" != "debwatch - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "debwatch.mak" CFG="debwatch - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "debwatch - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "debwatch - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "debwatch - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

!IF "$(RECURSE)" == "0" 

ALL : "..\release\debwatch.dll"

!ELSE 

ALL : "winstyle - Win32 Release" "winmsg - Win32 Release" "PrString - Win32 Release" "PrIDs - Win32 Release" "PrHandle - Win32 Release" "hresult - Win32 Release" "..\release\debwatch.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"hresult - Win32 ReleaseCLEAN" "PrHandle - Win32 ReleaseCLEAN" "PrIDs - Win32 ReleaseCLEAN" "PrString - Win32 ReleaseCLEAN" "winmsg - Win32 ReleaseCLEAN" "winstyle - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\rec_format.obj"
	-@erase "$(INTDIR)\vassist.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\debwatch.exp"
	-@erase "..\release\debwatch.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /I "../../../../include" /I "../.." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEBWATCH_EXPORTS" /Fp"$(INTDIR)\debwatch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\debwatch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib Shlwapi.lib advapi32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\debwatch.pdb" /machine:I386 /def:".\debwatch.def" /out:"../release/debwatch.dll" /implib:"$(OUTDIR)\debwatch.lib" 
DEF_FILE= \
	".\debwatch.def"
LINK32_OBJS= \
	"$(INTDIR)\rec_format.obj" \
	"$(INTDIR)\vassist.obj" \
	"..\AddIns\hresult\Release\hresult.lib" \
	"..\AddIns\PrHandle\Release\PrHandle.lib" \
	"..\AddIns\PrIDs\Release\PrIDs.lib" \
	"..\AddIns\PrString\Release\PrString.lib" \
	"..\AddIns\winmsg\Release\winmsg.lib" \
	"..\AddIns\winstyle\Release\winstyle.lib"

"..\release\debwatch.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : "e:\ds\Common\MSDev98\Bin\debwatch.dll"

!ELSE 

ALL : "winstyle - Win32 Debug" "winmsg - Win32 Debug" "PrString - Win32 Debug" "PrIDs - Win32 Debug" "PrHandle - Win32 Debug" "hresult - Win32 Debug" "e:\ds\Common\MSDev98\Bin\debwatch.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"hresult - Win32 DebugCLEAN" "PrHandle - Win32 DebugCLEAN" "PrIDs - Win32 DebugCLEAN" "PrString - Win32 DebugCLEAN" "winmsg - Win32 DebugCLEAN" "winstyle - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\rec_format.obj"
	-@erase "$(INTDIR)\vassist.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\debwatch.exp"
	-@erase "$(OUTDIR)\debwatch.pdb"
	-@erase "e:\ds\Common\MSDev98\Bin\debwatch.dll"
	-@erase "e:\ds\Common\MSDev98\Bin\debwatch.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../../../../include" /I "../.." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEBWATCH_EXPORTS" /Fp"$(INTDIR)\debwatch.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\debwatch.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=user32.lib Shlwapi.lib advapi32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\debwatch.pdb" /debug /machine:I386 /def:".\debwatch.def" /out:"e:\ds\Common\MSDev98\Bin\debwatch.dll" /implib:"$(OUTDIR)\debwatch.lib" /pdbtype:sept 
DEF_FILE= \
	".\debwatch.def"
LINK32_OBJS= \
	"$(INTDIR)\rec_format.obj" \
	"$(INTDIR)\vassist.obj" \
	"..\AddIns\PrIDs\Debug\PrIDs.lib"

"e:\ds\Common\MSDev98\Bin\debwatch.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("debwatch.dep")
!INCLUDE "debwatch.dep"
!ELSE 
!MESSAGE Warning: cannot find "debwatch.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "debwatch - Win32 Release" || "$(CFG)" == "debwatch - Win32 Debug"
SOURCE=.\rec_format.c

"$(INTDIR)\rec_format.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vassist.c

"$(INTDIR)\vassist.obj" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "debwatch - Win32 Release"

"hresult - Win32 Release" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\hresult"
   $(MAKE) /$(MAKEFLAGS) /F .\hresult.mak CFG="hresult - Win32 Release" 
   cd "..\..\DebWatch"

"hresult - Win32 ReleaseCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\hresult"
   $(MAKE) /$(MAKEFLAGS) /F .\hresult.mak CFG="hresult - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

"hresult - Win32 Debug" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\hresult"
   $(MAKE) /$(MAKEFLAGS) /F .\hresult.mak CFG="hresult - Win32 Debug" 
   cd "..\..\DebWatch"

"hresult - Win32 DebugCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\hresult"
   $(MAKE) /$(MAKEFLAGS) /F .\hresult.mak CFG="hresult - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ENDIF 

!IF  "$(CFG)" == "debwatch - Win32 Release"

"PrHandle - Win32 Release" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrHandle"
   $(MAKE) /$(MAKEFLAGS) /F .\PrHandle.mak CFG="PrHandle - Win32 Release" 
   cd "..\..\DebWatch"

"PrHandle - Win32 ReleaseCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrHandle"
   $(MAKE) /$(MAKEFLAGS) /F .\PrHandle.mak CFG="PrHandle - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

"PrHandle - Win32 Debug" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrHandle"
   $(MAKE) /$(MAKEFLAGS) /F .\PrHandle.mak CFG="PrHandle - Win32 Debug" 
   cd "..\..\DebWatch"

"PrHandle - Win32 DebugCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrHandle"
   $(MAKE) /$(MAKEFLAGS) /F .\PrHandle.mak CFG="PrHandle - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ENDIF 

!IF  "$(CFG)" == "debwatch - Win32 Release"

"PrIDs - Win32 Release" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrIDs"
   $(MAKE) /$(MAKEFLAGS) /F .\PrIDs.mak CFG="PrIDs - Win32 Release" 
   cd "..\..\DebWatch"

"PrIDs - Win32 ReleaseCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrIDs"
   $(MAKE) /$(MAKEFLAGS) /F .\PrIDs.mak CFG="PrIDs - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

"PrIDs - Win32 Debug" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrIDs"
   $(MAKE) /$(MAKEFLAGS) /F .\PrIDs.mak CFG="PrIDs - Win32 Debug" 
   cd "..\..\DebWatch"

"PrIDs - Win32 DebugCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrIDs"
   $(MAKE) /$(MAKEFLAGS) /F .\PrIDs.mak CFG="PrIDs - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ENDIF 

!IF  "$(CFG)" == "debwatch - Win32 Release"

"PrString - Win32 Release" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrString"
   $(MAKE) /$(MAKEFLAGS) /F .\PrString.mak CFG="PrString - Win32 Release" 
   cd "..\..\DebWatch"

"PrString - Win32 ReleaseCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrString"
   $(MAKE) /$(MAKEFLAGS) /F .\PrString.mak CFG="PrString - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

"PrString - Win32 Debug" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrString"
   $(MAKE) /$(MAKEFLAGS) /F .\PrString.mak CFG="PrString - Win32 Debug" 
   cd "..\..\DebWatch"

"PrString - Win32 DebugCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\PrString"
   $(MAKE) /$(MAKEFLAGS) /F .\PrString.mak CFG="PrString - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ENDIF 

!IF  "$(CFG)" == "debwatch - Win32 Release"

"winmsg - Win32 Release" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winmsg"
   $(MAKE) /$(MAKEFLAGS) /F .\winmsg.mak CFG="winmsg - Win32 Release" 
   cd "..\..\DebWatch"

"winmsg - Win32 ReleaseCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winmsg"
   $(MAKE) /$(MAKEFLAGS) /F .\winmsg.mak CFG="winmsg - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

"winmsg - Win32 Debug" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winmsg"
   $(MAKE) /$(MAKEFLAGS) /F .\winmsg.mak CFG="winmsg - Win32 Debug" 
   cd "..\..\DebWatch"

"winmsg - Win32 DebugCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winmsg"
   $(MAKE) /$(MAKEFLAGS) /F .\winmsg.mak CFG="winmsg - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ENDIF 

!IF  "$(CFG)" == "debwatch - Win32 Release"

"winstyle - Win32 Release" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winstyle"
   $(MAKE) /$(MAKEFLAGS) /F .\winstyle.mak CFG="winstyle - Win32 Release" 
   cd "..\..\DebWatch"

"winstyle - Win32 ReleaseCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winstyle"
   $(MAKE) /$(MAKEFLAGS) /F .\winstyle.mak CFG="winstyle - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ELSEIF  "$(CFG)" == "debwatch - Win32 Debug"

"winstyle - Win32 Debug" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winstyle"
   $(MAKE) /$(MAKEFLAGS) /F .\winstyle.mak CFG="winstyle - Win32 Debug" 
   cd "..\..\DebWatch"

"winstyle - Win32 DebugCLEAN" : 
   cd "\prague\CodeGen\MSDebWatchExtender\AddIns\winstyle"
   $(MAKE) /$(MAKEFLAGS) /F .\winstyle.mak CFG="winstyle - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\DebWatch"

!ENDIF 


!ENDIF 

