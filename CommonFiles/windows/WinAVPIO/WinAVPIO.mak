# Microsoft Developer Studio Generated NMAKE File, Based on WinAVPIO.dsp
!IF "$(CFG)" == ""
CFG=WinAVPIO - Win32 UnicodeOnMbcs DebugDll
!MESSAGE No configuration specified. Defaulting to WinAVPIO - Win32 UnicodeOnMbcs DebugDll.
!ENDIF 

!IF "$(CFG)" != "WinAVPIO - Win32 Release" && "$(CFG)" != "WinAVPIO - Win32 Debug" && "$(CFG)" != "WinAVPIO - Win32 DebugDll" && "$(CFG)" != "WinAVPIO - Win32 ReleaseDll" && "$(CFG)" != "WinAVPIO - Win32 Unicode DebugDll" && "$(CFG)" != "WinAVPIO - Win32 Unicode Release" && "$(CFG)" != "WinAVPIO - Win32 Unicode Debug" && "$(CFG)" != "WinAVPIO - Win32 Unicode ReleaseDll" && "$(CFG)" != "WinAVPIO - Win32 UnicodeOnMbcs Debug" && "$(CFG)" != "WinAVPIO - Win32 UnicodeOnMbcs Release" && "$(CFG)" != "WinAVPIO - Win32 UnicodeOnMbcs DebugDll" && "$(CFG)" != "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinAVPIO.mak" CFG="WinAVPIO - Win32 UnicodeOnMbcs DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinAVPIO - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 Unicode ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "WinAVPIO - Win32 Release"

OUTDIR=.\../../../Out/ReleaseS
INTDIR=.\../../../Temp/ReleaseS/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\Release\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\Release\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\Release\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\Release
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Debug"

OUTDIR=.\../../../Out/DebugS
INTDIR=.\../../../Temp/DebugS/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/DebugS
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\Debug\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\Debug\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\Debug\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 DebugDll"

OUTDIR=.\../../../Out/Debug
INTDIR=.\../../../Temp/Debug/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\DebugDll\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\DebugDll\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Debug\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugDll\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 ReleaseDll"

OUTDIR=.\../../../Out/Release
INTDIR=.\../../../Temp/Release/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/Release
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\ReleaseDll\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\ReleaseDll\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Release\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseDll\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseDll
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode DebugDll"

OUTDIR=.\../../../Out/DebugU
INTDIR=.\../../../Temp/DebugU/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/DebugU
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\DebugUDll\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\DebugUDll\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugU\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugUDll\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugUDll
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode Release"

OUTDIR=.\../../../Out/ReleaseUS
INTDIR=.\../../../Temp/ReleaseUS/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/ReleaseUS
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\ReleaseU\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\ReleaseU\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseUS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseU\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseU
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode Debug"

OUTDIR=.\../../../Out/DebugUS
INTDIR=.\../../../Temp/DebugUS/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/DebugUS
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\DebugU\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\DebugU\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugUS\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugU\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugU
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 Unicode ReleaseDll"

OUTDIR=.\../../../Out/ReleaseU
INTDIR=.\../../../Temp/ReleaseU/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/ReleaseU
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\ReleaseUDll\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\ReleaseUDll\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "UNICODE" /D "_UNICODE" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseU\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseUDll\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseUDll
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs Debug"

OUTDIR=.\../../../Out/DebugUOM
INTDIR=.\../../../Temp/DebugUOM/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/DebugUOM
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\DebugUOM\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\DebugUOM\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugUOM\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugUOM\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugUOM
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs Release"

OUTDIR=.\../../../Out/ReleaseUOM
INTDIR=.\../../../Temp/ReleaseUOM/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/ReleaseUOM
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\ReleaseUOM\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\ReleaseUOM\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /O2 /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseUOM\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseUOM\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseUOM
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs DebugDll"

OUTDIR=.\../../../Out/DebugUOMDll
INTDIR=.\../../../Temp/DebugUOMDll/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/DebugUOMDll
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\DebugUOMDll\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\DebugUOMDll\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugUOMDll\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\DebugUOMDll\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugUOMDll
<< 
	

!ELSEIF  "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll"

OUTDIR=.\../../../Out/ReleaseUOMDll
INTDIR=.\../../../Temp/ReleaseUOMDll/CommonFiles/windows/WinAVPIO
# Begin Custom Macros
OutDir=.\../../../Out/ReleaseUOMDll
# End Custom Macros

ALL : "$(OUTDIR)\WinAVPIO.lib" "..\..\ReleaseUOMDll\WinAVPIO.lib"


CLEAN :
	-@erase "$(INTDIR)\IORedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAVPIO.lib"
	-@erase "..\..\ReleaseUOMDll\WinAVPIO.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MD /W3 /GX /O2 /I "..\..\\" /D "_UNICODEONMBCS" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\WinAVPIO.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAVPIO.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAVPIO.lib" 
LIB32_OBJS= \
	"$(INTDIR)\IORedirect.obj"

"$(OUTDIR)\WinAVPIO.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseUOMDll\WinAVPIO.lib
InputName=WinAVPIO
SOURCE="$(InputPath)"

"..\..\ReleaseUOMDll\WinAVPIO.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseUOMDll
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
!IF EXISTS("WinAVPIO.dep")
!INCLUDE "WinAVPIO.dep"
!ELSE 
!MESSAGE Warning: cannot find "WinAVPIO.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinAVPIO - Win32 Release" || "$(CFG)" == "WinAVPIO - Win32 Debug" || "$(CFG)" == "WinAVPIO - Win32 DebugDll" || "$(CFG)" == "WinAVPIO - Win32 ReleaseDll" || "$(CFG)" == "WinAVPIO - Win32 Unicode DebugDll" || "$(CFG)" == "WinAVPIO - Win32 Unicode Release" || "$(CFG)" == "WinAVPIO - Win32 Unicode Debug" || "$(CFG)" == "WinAVPIO - Win32 Unicode ReleaseDll" || "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs Debug" || "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs Release" || "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs DebugDll" || "$(CFG)" == "WinAVPIO - Win32 UnicodeOnMbcs ReleaseDll"
SOURCE=.\IORedirect.cpp

"$(INTDIR)\IORedirect.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

