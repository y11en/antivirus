# Microsoft Developer Studio Generated NMAKE File, Based on WinAvpFF.dsp
!IF "$(CFG)" == ""
CFG=WinAvpFF - Win32 DebugDll
!MESSAGE No configuration specified. Defaulting to WinAvpFF - Win32 DebugDll.
!ENDIF 

!IF "$(CFG)" != "WinAvpFF - Win32 Release" && "$(CFG)" != "WinAvpFF - Win32 Debug" && "$(CFG)" != "WinAvpFF - Win32 DebugDll" && "$(CFG)" != "WinAvpFF - Win32 ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "WinAvpFF.mak" CFG="WinAvpFF - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinAvpFF - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAvpFF - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAvpFF - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "WinAvpFF - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "WinAvpFF - Win32 Release"

OUTDIR=.\../../../Out/ReleaseS
INTDIR=.\../../../Temp/ReleaseS/CommonFiles/windows/WinAVPFF
# Begin Custom Macros
OutDir=.\../../../Out/ReleaseS
# End Custom Macros

ALL : "$(OUTDIR)\WinAvpFF.lib" "..\..\Release\WinAvpFF.lib"


CLEAN :
	-@erase "$(INTDIR)\FFRedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAvpFF.lib"
	-@erase "..\..\Release\WinAvpFF.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\WinAvpFF.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAvpFF.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAvpFF.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FFRedirect.obj"

"$(OUTDIR)\WinAvpFF.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\ReleaseS\WinAvpFF.lib
InputName=WinAvpFF
SOURCE="$(InputPath)"

"..\..\Release\WinAvpFF.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\Release
<< 
	

!ELSEIF  "$(CFG)" == "WinAvpFF - Win32 Debug"

OUTDIR=.\../../../Out/DebugS
INTDIR=.\../../../Temp/DebugS/CommonFiles/windows/WinAVPFF
# Begin Custom Macros
OutDir=.\../../../Out/DebugS
# End Custom Macros

ALL : "$(OUTDIR)\WinAvpFF.lib" "..\..\Debug\WinAvpFF.lib"


CLEAN :
	-@erase "$(INTDIR)\FFRedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAvpFF.lib"
	-@erase "..\..\Debug\WinAvpFF.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\WinAvpFF.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAvpFF.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAvpFF.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FFRedirect.obj"

"$(OUTDIR)\WinAvpFF.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\DebugS\WinAvpFF.lib
InputName=WinAvpFF
SOURCE="$(InputPath)"

"..\..\Debug\WinAvpFF.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\Debug
<< 
	

!ELSEIF  "$(CFG)" == "WinAvpFF - Win32 DebugDll"

OUTDIR=.\../../../Out/Debug
INTDIR=.\../../../Temp/Debug/CommonFiles/windows/WinAVPFF
# Begin Custom Macros
OutDir=.\../../../Out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\WinAvpFF.lib" "..\..\DebugDll\WinAvpFF.lib"


CLEAN :
	-@erase "$(INTDIR)\FFRedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\WinAvpFF.lib"
	-@erase "..\..\DebugDll\WinAvpFF.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\WinAvpFF.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAvpFF.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAvpFF.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FFRedirect.obj"

"$(OUTDIR)\WinAvpFF.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Debug\WinAvpFF.lib
InputName=WinAvpFF
SOURCE="$(InputPath)"

"..\..\DebugDll\WinAvpFF.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\DebugDll
<< 
	

!ELSEIF  "$(CFG)" == "WinAvpFF - Win32 ReleaseDll"

OUTDIR=.\../../../Out/Release
INTDIR=.\../../../Temp/Release/CommonFiles/windows/WinAVPFF
# Begin Custom Macros
OutDir=.\../../../Out/Release
# End Custom Macros

ALL : "$(OUTDIR)\WinAvpFF.lib" "..\..\ReleaseDll\WinAvpFF.lib"


CLEAN :
	-@erase "$(INTDIR)\FFRedirect.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\WinAvpFF.lib"
	-@erase "..\..\ReleaseDll\WinAvpFF.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\WinAvpFF.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\WinAvpFF.bsc" 
BSC32_SBRS= \
	
LIB32=xilink6.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\WinAvpFF.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FFRedirect.obj"

"$(OUTDIR)\WinAvpFF.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

InputPath=\Out\Release\WinAvpFF.lib
InputName=WinAvpFF
SOURCE="$(InputPath)"

"..\..\ReleaseDll\WinAvpFF.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	copy "$(InputPath)" ..\..\ReleaseDll
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
!IF EXISTS("WinAvpFF.dep")
!INCLUDE "WinAvpFF.dep"
!ELSE 
!MESSAGE Warning: cannot find "WinAvpFF.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinAvpFF - Win32 Release" || "$(CFG)" == "WinAvpFF - Win32 Debug" || "$(CFG)" == "WinAvpFF - Win32 DebugDll" || "$(CFG)" == "WinAvpFF - Win32 ReleaseDll"
SOURCE=.\FFRedirect.cpp

"$(INTDIR)\FFRedirect.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

