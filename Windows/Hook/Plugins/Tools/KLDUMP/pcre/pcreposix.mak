# Microsoft Developer Studio Generated NMAKE File, Based on pcreposix.dsp
!IF "$(CFG)" == ""
CFG=pcreposix - Win32 Debug
!MESSAGE No configuration specified. Defaulting to pcreposix - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "pcreposix - Win32 Release" && "$(CFG)" != "pcreposix - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pcreposix.mak" CFG="pcreposix - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pcreposix - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pcreposix - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "pcreposix - Win32 Release"

OUTDIR=.\LibR
INTDIR=.\LibR
# Begin Custom Macros
OutDir=.\LibR
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\pcreposix.lib"

!ELSE 

ALL : "pcre - Win32 Release" "$(OUTDIR)\pcreposix.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"pcre - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\pcreposix.obj"
	-@erase "$(INTDIR)\pcreposix_src.idb"
	-@erase "$(INTDIR)\pcreposix_src.pdb"
	-@erase "$(OUTDIR)\pcreposix.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /Zi /O2 /D "_WIN32" /D "NDEBUG" /D "_WINDOWS" /D "STATIC" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\pcreposix_src" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pcreposix.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\pcreposix.lib" 
LIB32_OBJS= \
	"$(INTDIR)\pcreposix.obj" \
	"$(OUTDIR)\pcre.lib"

"$(OUTDIR)\pcreposix.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "pcreposix - Win32 Debug"

OUTDIR=.\LibD
INTDIR=.\LibD
# Begin Custom Macros
OutDir=.\LibD
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\pcre.h" ".\config.h" "$(OUTDIR)\pcreposix.lib"

!ELSE 

ALL : "pcre - Win32 Debug" ".\pcre.h" ".\config.h" "$(OUTDIR)\pcreposix.lib"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"pcre - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\pcreposix.obj"
	-@erase "$(INTDIR)\pcreposix_src.idb"
	-@erase "$(INTDIR)\pcreposix_src.pdb"
	-@erase "$(OUTDIR)\pcreposix.lib"
	-@erase ".\config.h"
	-@erase ".\pcre.h"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "_WIN32" /D "_DEBUG" /D "_WINDOWS" /D "STATIC" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\pcreposix_src" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\pcreposix.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\pcreposix.lib" 
LIB32_OBJS= \
	"$(INTDIR)\pcreposix.obj" \
	"$(OUTDIR)\pcre.lib"

"$(OUTDIR)\pcreposix.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("pcreposix.dep")
!INCLUDE "pcreposix.dep"
!ELSE 
!MESSAGE Warning: cannot find "pcreposix.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "pcreposix - Win32 Release" || "$(CFG)" == "pcreposix - Win32 Debug"
SOURCE=.\pcreposix.c

"$(INTDIR)\pcreposix.obj" : $(SOURCE) "$(INTDIR)" ".\config.h" ".\pcre.h"


SOURCE=.\config.hw

!IF  "$(CFG)" == "pcreposix - Win32 Release"

InputPath=.\config.hw

".\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	type .\config.hw > .\config.h
<< 
	

!ELSEIF  "$(CFG)" == "pcreposix - Win32 Debug"

InputPath=.\config.hw

".\config.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	type .\config.hw > .\config.h
<< 
	

!ENDIF 

SOURCE=.\pcre.hw

!IF  "$(CFG)" == "pcreposix - Win32 Release"

InputPath=.\pcre.hw

".\pcre.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	type .\pcre.hw > .\pcre.h
<< 
	

!ELSEIF  "$(CFG)" == "pcreposix - Win32 Debug"

InputPath=.\pcre.hw

".\pcre.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	type .\pcre.hw > .\pcre.h
<< 
	

!ENDIF 

!IF  "$(CFG)" == "pcreposix - Win32 Release"

"pcre - Win32 Release" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\pcre.mak" CFG="pcre - Win32 Release" 
   cd "."

"pcre - Win32 ReleaseCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\pcre.mak" CFG="pcre - Win32 Release" RECURSE=1 CLEAN 
   cd "."

!ELSEIF  "$(CFG)" == "pcreposix - Win32 Debug"

"pcre - Win32 Debug" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\pcre.mak" CFG="pcre - Win32 Debug" 
   cd "."

"pcre - Win32 DebugCLEAN" : 
   cd "."
   $(MAKE) /$(MAKEFLAGS) /F ".\pcre.mak" CFG="pcre - Win32 Debug" RECURSE=1 CLEAN 
   cd "."

!ENDIF 


!ENDIF 

