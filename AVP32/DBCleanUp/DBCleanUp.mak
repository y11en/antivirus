# Microsoft Developer Studio Generated NMAKE File, Based on DBCleanUp.dsp
!IF "$(CFG)" == ""
CFG=DBCleanUp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to DBCleanUp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DBCleanUp - Win32 Release" && "$(CFG)" != "DBCleanUp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DBCleanUp.mak" CFG="DBCleanUp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DBCleanUp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "DBCleanUp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DBCleanUp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL :  "."


CLEAN :
	-@erase 
	-@erase "."

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MT /W3 /GX /Zi /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\DBCleanUp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DBCleanUp.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\DBCleanUp.lib" 
LIB32_OBJS= \
	
InputPath=.\Release\DBCleanUp.lib
SOURCE="$(InputPath)"

"..\DBCleanUp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nmake -fDBCleanUpWinNT 
	nmake -fDBCleanUpWin32 
	nmake -fDBCleanUpWin9x 
<< 
	

!ELSEIF  "$(CFG)" == "DBCleanUp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL :  "."


CLEAN :
	-@erase 
	-@erase "."

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\DBCleanUp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DBCleanUp.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\DBCleanUp.lib" 
LIB32_OBJS= \
	
InputPath=.\Debug\DBCleanUp.lib
SOURCE="$(InputPath)"

"..\DBCleanUp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	nmake -fDBCleanUpWin32 CFG="DEBUG" 
	nmake -fDBCleanUpWinNT CFG="DEBUG" 
	nmake -fDBCleanUpWin9x CFG="DEBUG" 
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
!IF EXISTS("DBCleanUp.dep")
!INCLUDE "DBCleanUp.dep"
!ELSE 
!MESSAGE Warning: cannot find "DBCleanUp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DBCleanUp - Win32 Release" || "$(CFG)" == "DBCleanUp - Win32 Debug"

!ENDIF 

