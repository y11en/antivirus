# Microsoft Developer Studio Generated NMAKE File, Based on hashutil.dsp
!IF "$(CFG)" == ""
CFG=hashutil - Win32 DebugDll
!MESSAGE No configuration specified. Defaulting to hashutil - Win32 DebugDll.
!ENDIF 

!IF "$(CFG)" != "hashutil - Win32 Release" && "$(CFG)" != "hashutil - Win32 Debug" && "$(CFG)" != "hashutil - Win32 DebugDll" && "$(CFG)" != "hashutil - Win32 ReleaseDll"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hashutil.mak" CFG="hashutil - Win32 DebugDll"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hashutil - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hashutil - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "hashutil - Win32 DebugDll" (based on "Win32 (x86) Static Library")
!MESSAGE "hashutil - Win32 ReleaseDll" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "hashutil - Win32 Release"

OUTDIR=.\..\..\CommonFiles\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\CommonFiles\Release
# End Custom Macros

ALL : "$(OUTDIR)\hashutil.lib"


CLEAN :
	-@erase "$(INTDIR)\crc16.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32str.obj"
	-@erase "$(INTDIR)\src16str.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hashutil.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hashutil.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\hashutil.lib" 
LIB32_OBJS= \
	"$(INTDIR)\crc16.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crc32str.obj" \
	"$(INTDIR)\src16str.obj"

"$(OUTDIR)\hashutil.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hashutil - Win32 Debug"

OUTDIR=.\..\..\CommonFiles\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\CommonFiles\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hashutil.lib" "$(OUTDIR)\hashutil.bsc"


CLEAN :
	-@erase "$(INTDIR)\crc16.obj"
	-@erase "$(INTDIR)\crc16.sbr"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32.sbr"
	-@erase "$(INTDIR)\crc32str.obj"
	-@erase "$(INTDIR)\crc32str.sbr"
	-@erase "$(INTDIR)\src16str.obj"
	-@erase "$(INTDIR)\src16str.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hashutil.bsc"
	-@erase "$(OUTDIR)\hashutil.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hashutil.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\crc16.sbr" \
	"$(INTDIR)\crc32.sbr" \
	"$(INTDIR)\crc32str.sbr" \
	"$(INTDIR)\src16str.sbr"

"$(OUTDIR)\hashutil.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\hashutil.lib" 
LIB32_OBJS= \
	"$(INTDIR)\crc16.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crc32str.obj" \
	"$(INTDIR)\src16str.obj"

"$(OUTDIR)\hashutil.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hashutil - Win32 DebugDll"

OUTDIR=.\..\..\CommonFiles\DebugDll
INTDIR=.\DebugDll
# Begin Custom Macros
OutDir=.\..\..\CommonFiles\DebugDll
# End Custom Macros

ALL : "$(OUTDIR)\hashutil.lib" "$(OUTDIR)\hashutil.bsc"


CLEAN :
	-@erase "$(INTDIR)\crc16.obj"
	-@erase "$(INTDIR)\crc16.sbr"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32.sbr"
	-@erase "$(INTDIR)\crc32str.obj"
	-@erase "$(INTDIR)\crc32str.sbr"
	-@erase "$(INTDIR)\src16str.obj"
	-@erase "$(INTDIR)\src16str.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\hashutil.bsc"
	-@erase "$(OUTDIR)\hashutil.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hashutil.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\crc16.sbr" \
	"$(INTDIR)\crc32.sbr" \
	"$(INTDIR)\crc32str.sbr" \
	"$(INTDIR)\src16str.sbr"

"$(OUTDIR)\hashutil.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\hashutil.lib" 
LIB32_OBJS= \
	"$(INTDIR)\crc16.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crc32str.obj" \
	"$(INTDIR)\src16str.obj"

"$(OUTDIR)\hashutil.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hashutil - Win32 ReleaseDll"

OUTDIR=.\..\..\CommonFiles\ReleaseDll
INTDIR=.\ReleaseDll
# Begin Custom Macros
OutDir=.\..\..\CommonFiles\ReleaseDll
# End Custom Macros

ALL : "$(OUTDIR)\hashutil.lib"


CLEAN :
	-@erase "$(INTDIR)\crc16.obj"
	-@erase "$(INTDIR)\crc32.obj"
	-@erase "$(INTDIR)\crc32str.obj"
	-@erase "$(INTDIR)\src16str.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\hashutil.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hashutil.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\hashutil.lib" 
LIB32_OBJS= \
	"$(INTDIR)\crc16.obj" \
	"$(INTDIR)\crc32.obj" \
	"$(INTDIR)\crc32str.obj" \
	"$(INTDIR)\src16str.obj"

"$(OUTDIR)\hashutil.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("hashutil.dep")
!INCLUDE "hashutil.dep"
!ELSE 
!MESSAGE Warning: cannot find "hashutil.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hashutil - Win32 Release" || "$(CFG)" == "hashutil - Win32 Debug" || "$(CFG)" == "hashutil - Win32 DebugDll" || "$(CFG)" == "hashutil - Win32 ReleaseDll"
SOURCE=.\crc16.c

!IF  "$(CFG)" == "hashutil - Win32 Release"


"$(INTDIR)\crc16.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 Debug"


"$(INTDIR)\crc16.obj"	"$(INTDIR)\crc16.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 DebugDll"


"$(INTDIR)\crc16.obj"	"$(INTDIR)\crc16.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 ReleaseDll"


"$(INTDIR)\crc16.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\crc32.c

!IF  "$(CFG)" == "hashutil - Win32 Release"


"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 Debug"


"$(INTDIR)\crc32.obj"	"$(INTDIR)\crc32.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 DebugDll"


"$(INTDIR)\crc32.obj"	"$(INTDIR)\crc32.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 ReleaseDll"


"$(INTDIR)\crc32.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\crc32str.c

!IF  "$(CFG)" == "hashutil - Win32 Release"


"$(INTDIR)\crc32str.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 Debug"


"$(INTDIR)\crc32str.obj"	"$(INTDIR)\crc32str.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 DebugDll"


"$(INTDIR)\crc32str.obj"	"$(INTDIR)\crc32str.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 ReleaseDll"


"$(INTDIR)\crc32str.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\src16str.c

!IF  "$(CFG)" == "hashutil - Win32 Release"


"$(INTDIR)\src16str.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 Debug"


"$(INTDIR)\src16str.obj"	"$(INTDIR)\src16str.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 DebugDll"


"$(INTDIR)\src16str.obj"	"$(INTDIR)\src16str.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "hashutil - Win32 ReleaseDll"


"$(INTDIR)\src16str.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

