# Microsoft Developer Studio Generated NMAKE File, Based on basegui.dsp
!IF "$(CFG)" == ""
CFG=basegui - Win32 Debug
!MESSAGE No configuration specified. Defaulting to basegui - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "basegui - Win32 Release" && "$(CFG)" != "basegui - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "basegui.mak" CFG="basegui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "basegui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "basegui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "basegui - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/basegui
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\basegui.dll"

!ELSE 

ALL : "Inflate - Win32 Release" "pnglib - Win32 Release" "wingui - Win32 Release" "$(OUTDIR)\basegui.dll"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"wingui - Win32 ReleaseCLEAN" "pnglib - Win32 ReleaseCLEAN" "Inflate - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\basegui.obj"
	-@erase "$(INTDIR)\binding.obj"
	-@erase "$(INTDIR)\Formats.obj"
	-@erase "$(INTDIR)\IniRead.obj"
	-@erase "$(INTDIR)\ItemBase.obj"
	-@erase "$(INTDIR)\ItemProps.obj"
	-@erase "$(INTDIR)\ItemRoot.obj"
	-@erase "$(INTDIR)\pagememorymanager.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wingui.res"
	-@erase "$(OUTDIR)\basegui.dll"
	-@erase "$(OUTDIR)\basegui.exp"
	-@erase "$(OUTDIR)\basegui.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "../include" /I "..\..\.\CommonFiles" /I "../../prague/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\wingui.res" /i "..\..\include" /i "..\..\.\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\basegui.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=wingui.lib shell32.lib comdlg32.lib user32.lib gdi32.lib comctl32.lib htmlhelp.lib version.lib /nologo /base:"0x62600000" /dll /incremental:no /pdb:"$(OUTDIR)\basegui.pdb" /debug /machine:I386 /out:"$(OUTDIR)\basegui.dll" /implib:"$(OUTDIR)\basegui.lib" /pdbtype:sept /libpath:"..\..\out\Release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\basegui.obj" \
	"$(INTDIR)\binding.obj" \
	"$(INTDIR)\Formats.obj" \
	"$(INTDIR)\ItemBase.obj" \
	"$(INTDIR)\ItemProps.obj" \
	"$(INTDIR)\ItemRoot.obj" \
	"$(INTDIR)\wingui.res" \
	"$(INTDIR)\IniRead.obj" \
	"$(INTDIR)\pagememorymanager.obj" \
	"$(OUTDIR)\wingui.lib" \
	"$(OUTDIR)\pnglib.lib"

"$(OUTDIR)\basegui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/basegui
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\basegui.dll" "$(OUTDIR)\basegui.bsc" ".\../../out/Debug\regsvr32.trg"

!ELSE 

ALL : "Inflate - Win32 Debug" "pnglib - Win32 Debug" "wingui - Win32 Debug" "$(OUTDIR)\basegui.dll" "$(OUTDIR)\basegui.bsc" ".\../../out/Debug\regsvr32.trg"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"wingui - Win32 DebugCLEAN" "pnglib - Win32 DebugCLEAN" "Inflate - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\basegui.obj"
	-@erase "$(INTDIR)\basegui.sbr"
	-@erase "$(INTDIR)\binding.obj"
	-@erase "$(INTDIR)\binding.sbr"
	-@erase "$(INTDIR)\Formats.obj"
	-@erase "$(INTDIR)\Formats.sbr"
	-@erase "$(INTDIR)\IniRead.obj"
	-@erase "$(INTDIR)\IniRead.sbr"
	-@erase "$(INTDIR)\ItemBase.obj"
	-@erase "$(INTDIR)\ItemBase.sbr"
	-@erase "$(INTDIR)\ItemProps.obj"
	-@erase "$(INTDIR)\ItemProps.sbr"
	-@erase "$(INTDIR)\ItemRoot.obj"
	-@erase "$(INTDIR)\ItemRoot.sbr"
	-@erase "$(INTDIR)\pagememorymanager.obj"
	-@erase "$(INTDIR)\pagememorymanager.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wingui.res"
	-@erase "$(OUTDIR)\basegui.bsc"
	-@erase "$(OUTDIR)\basegui.dll"
	-@erase "$(OUTDIR)\basegui.exp"
	-@erase "$(OUTDIR)\basegui.ilk"
	-@erase "$(OUTDIR)\basegui.pdb"
	-@erase ".\../../out/Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../prague/include" /I "../include" /D "_DEBUG" /D "_PRAGUE_TRACE_" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\wingui.res" /i "..\..\include" /i "..\..\.\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\basegui.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\basegui.sbr" \
	"$(INTDIR)\binding.sbr" \
	"$(INTDIR)\Formats.sbr" \
	"$(INTDIR)\ItemBase.sbr" \
	"$(INTDIR)\ItemProps.sbr" \
	"$(INTDIR)\ItemRoot.sbr" \
	"$(INTDIR)\IniRead.sbr" \
	"$(INTDIR)\pagememorymanager.sbr"

"$(OUTDIR)\basegui.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=wingui.lib shell32.lib comdlg32.lib user32.lib gdi32.lib comctl32.lib htmlhelp.lib version.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\basegui.pdb" /debug /machine:I386 /out:"$(OUTDIR)\basegui.dll" /implib:"$(OUTDIR)\basegui.lib" /libpath:"../../out/Debug" 
LINK32_OBJS= \
	"$(INTDIR)\basegui.obj" \
	"$(INTDIR)\binding.obj" \
	"$(INTDIR)\Formats.obj" \
	"$(INTDIR)\ItemBase.obj" \
	"$(INTDIR)\ItemProps.obj" \
	"$(INTDIR)\ItemRoot.obj" \
	"$(INTDIR)\wingui.res" \
	"$(INTDIR)\IniRead.obj" \
	"$(INTDIR)\pagememorymanager.obj" \
	"$(OUTDIR)\wingui.lib" \
	"$(OUTDIR)\pnglib.lib"

"$(OUTDIR)\basegui.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\../../out/Debug
TargetPath=\out\Debug\basegui.dll
InputPath=\out\Debug\basegui.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
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
!IF EXISTS("basegui.dep")
!INCLUDE "basegui.dep"
!ELSE 
!MESSAGE Warning: cannot find "basegui.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "basegui - Win32 Release" || "$(CFG)" == "basegui - Win32 Debug"
SOURCE=.\basegui.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\basegui.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\basegui.obj"	"$(INTDIR)\basegui.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\binding.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\binding.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\binding.obj"	"$(INTDIR)\binding.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\Formats.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\Formats.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\Formats.obj"	"$(INTDIR)\Formats.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\prague\IniLib\IniRead.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\IniRead.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\IniRead.obj"	"$(INTDIR)\IniRead.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\ItemBase.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\ItemBase.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\ItemBase.obj"	"$(INTDIR)\ItemBase.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ItemProps.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\ItemProps.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\ItemProps.obj"	"$(INTDIR)\ItemProps.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\ItemRoot.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\ItemRoot.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\ItemRoot.obj"	"$(INTDIR)\ItemRoot.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\AVP32\pagememorymanager\pagememorymanager.cpp

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\pagememorymanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\pagememorymanager.obj"	"$(INTDIR)\pagememorymanager.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\WinGUI\wingui.rc

!IF  "$(CFG)" == "basegui - Win32 Release"


"$(INTDIR)\wingui.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\wingui.res" /i "..\..\include" /i "..\..\.\CommonFiles" /i "wingui" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"


"$(INTDIR)\wingui.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x419 /fo"$(INTDIR)\wingui.res" /i "..\..\include" /i "..\..\.\CommonFiles" /i "wingui" /d "_DEBUG" $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "basegui - Win32 Release"

"wingui - Win32 Release" : 
   cd ".\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Release" 
   cd ".."

"wingui - Win32 ReleaseCLEAN" : 
   cd ".\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"

"wingui - Win32 Debug" : 
   cd ".\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Debug" 
   cd ".."

"wingui - Win32 DebugCLEAN" : 
   cd ".\wingui"
   $(MAKE) /$(MAKEFLAGS) /F .\wingui.mak CFG="wingui - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "basegui - Win32 Release"

"pnglib - Win32 Release" : 
   cd ".\png"
   $(MAKE) /$(MAKEFLAGS) /F .\pnglib.mak CFG="pnglib - Win32 Release" 
   cd ".."

"pnglib - Win32 ReleaseCLEAN" : 
   cd ".\png"
   $(MAKE) /$(MAKEFLAGS) /F .\pnglib.mak CFG="pnglib - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"

"pnglib - Win32 Debug" : 
   cd ".\png"
   $(MAKE) /$(MAKEFLAGS) /F .\pnglib.mak CFG="pnglib - Win32 Debug" 
   cd ".."

"pnglib - Win32 DebugCLEAN" : 
   cd ".\png"
   $(MAKE) /$(MAKEFLAGS) /F .\pnglib.mak CFG="pnglib - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "basegui - Win32 Release"

"Inflate - Win32 Release" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Release" 
   cd "..\..\..\PPP\basegui"

"Inflate - Win32 ReleaseCLEAN" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\basegui"

!ELSEIF  "$(CFG)" == "basegui - Win32 Debug"

"Inflate - Win32 Debug" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Debug" 
   cd "..\..\..\PPP\basegui"

"Inflate - Win32 DebugCLEAN" : 
   cd "\prague\Extract\inflate"
   $(MAKE) /$(MAKEFLAGS) /F .\Inflate.mak CFG="Inflate - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\..\PPP\basegui"

!ENDIF 


!ENDIF 

