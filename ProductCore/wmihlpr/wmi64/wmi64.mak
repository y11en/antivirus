# Microsoft Developer Studio Generated NMAKE File, Based on wmi64.dsp
!IF "$(CFG)" == ""
CFG=wmi64 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to wmi64 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "wmi64 - Win32 Release" && "$(CFG)" != "wmi64 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wmi64.mak" CFG="wmi64 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wmi64 - Win32 Release" (based on "Win32 (x64) Console Application")
!MESSAGE "wmi64 - Win32 Debug" (based on "Win32 (x64) Console Application")
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

!IF  "$(CFG)" == "wmi64 - Win32 Release"

OUTDIR=.\..\..\..\out\Release
INTDIR=.\..\..\..\temp\Release/ppp/wmi64
# Begin Custom Macros
OutDir=.\..\..\..\out\Release
# End Custom Macros

ALL : "$(OUTDIR)\wmi64.exe"


CLEAN :
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wmi64.obj"
	-@erase "$(INTDIR)\wmi64.pch"
	-@erase "$(OUTDIR)\wmi64.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /EHsc /W3 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmi64.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib bufferoverflowu.lib WbemUuid.Lib /subsystem:console /pdb:"$(OUTDIR)\wmi64.pdb" /machine:AMD64 /out:"$(OUTDIR)\wmi64.exe"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\wmi64.obj"

"$(OUTDIR)\wmi64.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "wmi64 - Win32 Debug"

OUTDIR=.\..\..\..\out\Debug
INTDIR=.\..\..\..\temp\Debug/ppp/wmi64
# Begin Custom Macros
OutDir=.\..\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\wmi64.exe"


CLEAN :
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wmi64.obj"
	-@erase "$(INTDIR)\wmi64.pch"
	-@erase "$(OUTDIR)\wmi64.exe"
	-@erase "$(OUTDIR)\wmi64.ilk"
	-@erase "$(OUTDIR)\wmi64.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /Fp"$(INTDIR)\wmi64.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\wmi64.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /subsystem:console /pdb:"$(OUTDIR)\wmi64.pdb" /debug /machine:AMD64 /out:"$(OUTDIR)\wmi64.exe"
LINK32_OBJS= \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\wmi64.obj"

"$(OUTDIR)\wmi64.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("wmi64.dep")
!INCLUDE "wmi64.dep"
!ELSE 
!MESSAGE Warning: cannot find "wmi64.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "wmi64 - Win32 Release" || "$(CFG)" == "wmi64 - Win32 Debug"
SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "wmi64 - Win32 Release"

CPP_SWITCHES=/nologo /W3 /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /Fp"$(INTDIR)\wmi64.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\wmi64.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "wmi64 - Win32 Debug"

CPP_SWITCHES=/nologo /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_UNICODE" /D "UNICODE" /Fp"$(INTDIR)\wmi64.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ  /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\wmi64.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\wmi64.cpp \

"$(INTDIR)\wmi64.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\wmi64.pch"



!ENDIF 

