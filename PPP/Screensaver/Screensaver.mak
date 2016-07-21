# Microsoft Developer Studio Generated NMAKE File, Based on Screensaver.dsp
!IF "$(CFG)" == ""
CFG=Screensaver - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Screensaver - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Screensaver - Win32 Release" && "$(CFG)" != "Screensaver - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Screensaver.mak" CFG="Screensaver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Screensaver - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Screensaver - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Screensaver - Win32 Release"

OUTDIR=.\..\..\out\release
INTDIR=.\..\..\temp\release/ppp/screeensaver
# Begin Custom Macros
OutDir=.\..\..\out\release
# End Custom Macros

ALL : "$(OUTDIR)\Kaspersky.scr"


CLEAN :
	-@erase "$(INTDIR)\ConfigDlg.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\SaverWnd.obj"
	-@erase "$(INTDIR)\Screensaver.obj"
	-@erase "$(INTDIR)\Screensaver.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Kaspersky.pdb"
	-@erase "$(OUTDIR)\Kaspersky.scr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Screensaver.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Screensaver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Kaspersky.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Kaspersky.scr" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\ConfigDlg.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\SaverWnd.obj" \
	"$(INTDIR)\Screensaver.obj" \
	"$(INTDIR)\Screensaver.res"

"$(OUTDIR)\Kaspersky.scr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Screensaver - Win32 Debug"

OUTDIR=.\..\..\out\Debug
INTDIR=.\..\..\temp\Debug/ppp/screensaver
# Begin Custom Macros
OutDir=.\..\..\out\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Kaspersky.scr"


CLEAN :
	-@erase "$(INTDIR)\ConfigDlg.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\SaverWnd.obj"
	-@erase "$(INTDIR)\Screensaver.obj"
	-@erase "$(INTDIR)\Screensaver.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Kaspersky.ilk"
	-@erase "$(OUTDIR)\Kaspersky.pdb"
	-@erase "$(OUTDIR)\Kaspersky.scr"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x419 /fo"$(INTDIR)\Screensaver.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Screensaver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Kaspersky.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Kaspersky.scr" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ConfigDlg.obj" \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\SaverWnd.obj" \
	"$(INTDIR)\Screensaver.obj" \
	"$(INTDIR)\Screensaver.res"

"$(OUTDIR)\Kaspersky.scr" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Screensaver.dep")
!INCLUDE "Screensaver.dep"
!ELSE 
!MESSAGE Warning: cannot find "Screensaver.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Screensaver - Win32 Release" || "$(CFG)" == "Screensaver - Win32 Debug"
SOURCE=.\ConfigDlg.cpp

"$(INTDIR)\ConfigDlg.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\SaverWnd.cpp

"$(INTDIR)\SaverWnd.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Screensaver.cpp

"$(INTDIR)\Screensaver.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Screensaver.rc

"$(INTDIR)\Screensaver.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

