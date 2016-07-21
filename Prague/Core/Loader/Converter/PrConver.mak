# Microsoft Developer Studio Generated NMAKE File, Based on PrConver.dsp
!IF "$(CFG)" == ""
CFG=PrConver - Win32 Debug
!MESSAGE No configuration specified. Defaulting to PrConver - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "PrConver - Win32 Release" && "$(CFG)" != "PrConver - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PrConver.mak" CFG="PrConver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PrConver - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "PrConver - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "PrConver - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\Release\PrConverter.exe"


CLEAN :
	-@erase "$(INTDIR)\get_opt.obj"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\PrConver.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\Release\PrConverter.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\Include" /I "..\..\..\CommonFiles\stuff" /D "NDEBUG" /D "_MBCS" /D "_CONSOLE" /Fp"$(INTDIR)\PrConver.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PrConver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\PrConverter.pdb" /machine:I386 /out:"../../Release/PrConverter.exe" 
LINK32_OBJS= \
	"$(INTDIR)\get_opt.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\PrConver.obj"

"..\..\Release\PrConverter.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "PrConver - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\Debug\PrConverter.exe" "$(OUTDIR)\PrConver.bsc"


CLEAN :
	-@erase "$(INTDIR)\get_opt.obj"
	-@erase "$(INTDIR)\get_opt.sbr"
	-@erase "$(INTDIR)\output.obj"
	-@erase "$(INTDIR)\output.sbr"
	-@erase "$(INTDIR)\PrConver.obj"
	-@erase "$(INTDIR)\PrConver.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\PrConver.bsc"
	-@erase "$(OUTDIR)\PrConverter.pdb"
	-@erase "..\..\Debug\PrConverter.exe"
	-@erase "..\..\Debug\PrConverter.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\Include" /I "..\..\..\CommonFiles\stuff" /D "_DEBUG" /D "_MBCS" /D "_CONSOLE" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\PrConver.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\PrConver.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\get_opt.sbr" \
	"$(INTDIR)\output.sbr" \
	"$(INTDIR)\PrConver.sbr"

"$(OUTDIR)\PrConver.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib shell32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\PrConverter.pdb" /debug /machine:I386 /out:"../../Debug/PrConverter.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\get_opt.obj" \
	"$(INTDIR)\output.obj" \
	"$(INTDIR)\PrConver.obj"

"..\..\Debug\PrConverter.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("PrConver.dep")
!INCLUDE "PrConver.dep"
!ELSE 
!MESSAGE Warning: cannot find "PrConver.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "PrConver - Win32 Release" || "$(CFG)" == "PrConver - Win32 Debug"
SOURCE=..\..\..\CommonFiles\Stuff\get_opt.c

!IF  "$(CFG)" == "PrConver - Win32 Release"


"$(INTDIR)\get_opt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PrConver - Win32 Debug"


"$(INTDIR)\get_opt.obj"	"$(INTDIR)\get_opt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\Kernel\output.c

!IF  "$(CFG)" == "PrConver - Win32 Release"


"$(INTDIR)\output.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "PrConver - Win32 Debug"


"$(INTDIR)\output.obj"	"$(INTDIR)\output.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\PrConver.c

!IF  "$(CFG)" == "PrConver - Win32 Release"


"$(INTDIR)\PrConver.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "PrConver - Win32 Debug"


"$(INTDIR)\PrConver.obj"	"$(INTDIR)\PrConver.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

