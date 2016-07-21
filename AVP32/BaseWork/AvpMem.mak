# Microsoft Developer Studio Generated NMAKE File, Based on AvpMem.dsp
!IF "$(CFG)" == ""
CFG=AvpMem - Win32 Debug
!MESSAGE No configuration specified. Defaulting to AvpMem - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "AvpMem - Win32 Release" && "$(CFG)" != "AvpMem - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AvpMem.mak" CFG="AvpMem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AvpMem - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AvpMem - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "AvpMem - Win32 Release"

OUTDIR=.\..\..\CommonFiles\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\..\..\CommonFiles\Release
# End Custom Macros

ALL : "$(OUTDIR)\AvpMem.lib"


CLEAN :
	-@erase "$(INTDIR)\Avpmem.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\AvpMem.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /GB /MD /W3 /GX /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AvpMem.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\AvpMem.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Avpmem.obj"

"$(OUTDIR)\AvpMem.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "AvpMem - Win32 Debug"

OUTDIR=.\..\..\CommonFiles\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\..\..\CommonFiles\Debug
# End Custom Macros

ALL : "$(OUTDIR)\AvpMem.lib"


CLEAN :
	-@erase "$(INTDIR)\Avpmem.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\AvpMem.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G3 /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\AvpMem.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\AvpMem.lib" 
LIB32_OBJS= \
	"$(INTDIR)\Avpmem.obj"

"$(OUTDIR)\AvpMem.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("AvpMem.dep")
!INCLUDE "AvpMem.dep"
!ELSE 
!MESSAGE Warning: cannot find "AvpMem.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "AvpMem - Win32 Release" || "$(CFG)" == "AvpMem - Win32 Debug"
SOURCE=.\Avpmem.cpp

"$(INTDIR)\Avpmem.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

