# Microsoft Developer Studio Generated NMAKE File, Based on avz.dsp
!IF "$(CFG)" == ""
CFG=avz - Win32 Debug
!MESSAGE No configuration specified. Defaulting to avz - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "avz - Win32 Release" && "$(CFG)" != "avz - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "avz.mak" CFG="avz - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "avz - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "avz - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "avz - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\avz.sys"


CLEAN :
	-@erase "$(INTDIR)\avz.obj"
	-@erase "$(INTDIR)\avz.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\avz.exp"
	-@erase "$(OUTDIR)\avz.lib"
	-@erase "$(OUTDIR)\avz.sys"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MT /W3 /GX /O2 /I "$(BASEDIRNT4)\inc" /I "$(BASEDIR2K)\src\filesys\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "avz_EXPORTS" /Fp"$(INTDIR)\avz.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avz.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=$(BASEDIR)\Lib\i386\Checked\ntoskrnl.lib $(BASEDIR)\Lib\i386\Checked\int64.lib $(BASEDIR)\Lib\i386\Checked\Hal.lib $(BASEDIR)\Lib\i386\Checked\Ntdll.lib /nologo /entry:"DriverEntry" /dll /incremental:no /pdb:"$(OUTDIR)\avz.pdb" /machine:I386 /nodefaultlib /out:"$(OUTDIR)\avz.sys" /implib:"$(OUTDIR)\avz.lib" /subsystem:native,4.00 /driver 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\avz.obj"

"$(OUTDIR)\avz.sys" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "avz - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\avz.sys"


CLEAN :
	-@erase "$(INTDIR)\avz.obj"
	-@erase "$(INTDIR)\avz.pch"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\avz.exp"
	-@erase "$(OUTDIR)\avz.lib"
	-@erase "$(OUTDIR)\avz.pdb"
	-@erase "$(OUTDIR)\avz.sys"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /Gz /MTd /W3 /Gm /GX /Zi /Od /I "$(BASEDIRNT4)\inc" /I "$(BASEDIR2K)\src\filesys\inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "avz_EXPORTS" /Fp"$(INTDIR)\avz.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\avz.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=$(BASEDIR)\Lib\i386\Checked\ntoskrnl.lib $(BASEDIR)\Lib\i386\Checked\int64.lib $(BASEDIR)\Lib\i386\Checked\Hal.lib $(BASEDIR)\Lib\i386\Checked\Ntdll.lib /nologo /entry:"DriverEntry" /dll /incremental:no /pdb:"$(OUTDIR)\avz.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\avz.sys" /implib:"$(OUTDIR)\avz.lib" /pdbtype:sept /subsystem:native,4.00 /driver 
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\avz.obj"

"$(OUTDIR)\avz.sys" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("avz.dep")
!INCLUDE "avz.dep"
!ELSE 
!MESSAGE Warning: cannot find "avz.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "avz - Win32 Release" || "$(CFG)" == "avz - Win32 Debug"
SOURCE=.\avz.cpp

"$(INTDIR)\avz.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\avz.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "avz - Win32 Release"

CPP_SWITCHES=/nologo /Gz /MT /W3 /GX /O2 /I "$(BASEDIRNT4)\inc" /I "$(BASEDIR2K)\src\filesys\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "avz_EXPORTS" /Fp"$(INTDIR)\avz.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\avz.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "avz - Win32 Debug"

CPP_SWITCHES=/nologo /Gz /MTd /W3 /Gm /GX /Zi /Od /I "$(BASEDIRNT4)\inc" /I "$(BASEDIR2K)\src\filesys\inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "avz_EXPORTS" /Fp"$(INTDIR)\avz.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\avz.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

