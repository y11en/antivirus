# Microsoft Developer Studio Generated NMAKE File, Based on appldiff.dsp
!IF "$(CFG)" == ""
CFG=appldiff - Win32 Debug
!MESSAGE No configuration specified. Defaulting to appldiff - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "appldiff - Win32 Release" && "$(CFG)" != "appldiff - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "appldiff.mak" CFG="appldiff - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "appldiff - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "appldiff - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "appldiff - Win32 Release"

OUTDIR=.\..\..\..\..\out\release
INTDIR=.\..\..\..\..\temp\release\applxmld
# Begin Custom Macros
OutDir=.\..\..\..\..\out\release
# End Custom Macros

ALL : "$(OUTDIR)\applxmld.exe"


CLEAN :
	-@erase "$(INTDIR)\appldiff.obj"
	-@erase "$(INTDIR)\file_al.obj"
	-@erase "$(INTDIR)\heap_al.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xml_diff.obj"
	-@erase "$(OUTDIR)\applxmld.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\\" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "USE_FILE_TYPE_STDIO" /D "USE_FILE_TYPE_MEMIO" /D "USE_RTL_HEAP" /Fp"$(INTDIR)\appldiff.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\appldiff.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\applxmld.pdb" /machine:I386 /out:"$(OUTDIR)\applxmld.exe" 
LINK32_OBJS= \
	"$(INTDIR)\appldiff.obj" \
	"$(INTDIR)\file_al.obj" \
	"$(INTDIR)\heap_al.obj" \
	"$(INTDIR)\xml_diff.obj"

"$(OUTDIR)\applxmld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "appldiff - Win32 Debug"

OUTDIR=.\..\..\..\..\out\debug
INTDIR=.\..\..\..\..\temp\debug\applxmld
# Begin Custom Macros
OutDir=.\..\..\..\..\out\debug
# End Custom Macros

ALL : "$(OUTDIR)\applxmld.exe"


CLEAN :
	-@erase "$(INTDIR)\appldiff.obj"
	-@erase "$(INTDIR)\file_al.obj"
	-@erase "$(INTDIR)\heap_al.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xml_diff.obj"
	-@erase "$(OUTDIR)\applxmld.exe"
	-@erase "$(OUTDIR)\applxmld.ilk"
	-@erase "$(OUTDIR)\applxmld.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "USE_FILE_TYPE_STDIO" /D "USE_FILE_TYPE_MEMIO" /D "USE_RTL_HEAP" /Fp"$(INTDIR)\appldiff.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\appldiff.bsc" 
BSC32_SBRS= \
	
LINK32=xilink6.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\applxmld.pdb" /debug /machine:I386 /out:"$(OUTDIR)\applxmld.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\appldiff.obj" \
	"$(INTDIR)\file_al.obj" \
	"$(INTDIR)\heap_al.obj" \
	"$(INTDIR)\xml_diff.obj"

"$(OUTDIR)\applxmld.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("appldiff.dep")
!INCLUDE "appldiff.dep"
!ELSE 
!MESSAGE Warning: cannot find "appldiff.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "appldiff - Win32 Release" || "$(CFG)" == "appldiff - Win32 Debug"
SOURCE=.\appldiff.c

"$(INTDIR)\appldiff.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=..\file_al.c

"$(INTDIR)\file_al.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\heap_al.c

"$(INTDIR)\heap_al.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\xml_diff.c

"$(INTDIR)\xml_diff.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

