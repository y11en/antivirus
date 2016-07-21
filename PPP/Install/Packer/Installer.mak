# Microsoft Developer Studio Generated NMAKE File, Based on Installer.dsp
!IF "$(CFG)" == ""
CFG=Installer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Installer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Installer - Win32 Release" && "$(CFG)" != "Installer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Installer.mak" CFG="Installer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Installer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Installer - Win32 Debug" (based on "Win32 (x86) Application")
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

!IF  "$(CFG)" == "Installer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Installer.exe"


CLEAN :
	-@erase "$(INTDIR)\buff.obj"
	-@erase "$(INTDIR)\code.obj"
	-@erase "$(INTDIR)\exe.obj"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\pack.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Installer.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Installer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Installer.pdb" /machine:I386 /out:"$(OUTDIR)\Installer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\pack.obj" \
	"$(INTDIR)\buff.obj" \
	"$(INTDIR)\code.obj" \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\exe.obj" \
	"$(INTDIR)\ini.obj"

"$(OUTDIR)\Installer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Installer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Installer.exe"


CLEAN :
	-@erase "$(INTDIR)\buff.obj"
	-@erase "$(INTDIR)\code.obj"
	-@erase "$(INTDIR)\exe.obj"
	-@erase "$(INTDIR)\ini.obj"
	-@erase "$(INTDIR)\install.obj"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\pack.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Installer.exe"
	-@erase "$(OUTDIR)\Installer.ilk"
	-@erase "$(OUTDIR)\Installer.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Installer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Installer.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Installer.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\pack.obj" \
	"$(INTDIR)\buff.obj" \
	"$(INTDIR)\code.obj" \
	"$(INTDIR)\install.obj" \
	"$(INTDIR)\exe.obj" \
	"$(INTDIR)\ini.obj"

"$(OUTDIR)\Installer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("Installer.dep")
!INCLUDE "Installer.dep"
!ELSE 
!MESSAGE Warning: cannot find "Installer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Installer - Win32 Release" || "$(CFG)" == "Installer - Win32 Debug"
SOURCE=.\buff.cpp

"$(INTDIR)\buff.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\code.cpp

"$(INTDIR)\code.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\exe.cpp

"$(INTDIR)\exe.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ini.cpp

"$(INTDIR)\ini.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\install.cpp

"$(INTDIR)\install.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\pack.cpp

"$(INTDIR)\pack.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

