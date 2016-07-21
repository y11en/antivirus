# Microsoft Developer Studio Generated NMAKE File, Based on FSDrvLib.dsp
!IF "$(CFG)" == ""
CFG=FSDrvLib - Win32 DebugS
!MESSAGE No configuration specified. Defaulting to FSDrvLib - Win32 DebugS.
!ENDIF 

!IF "$(CFG)" != "FSDrvLib - Win32 Release" && "$(CFG)" != "FSDrvLib - Win32 Debug" && "$(CFG)" != "FSDrvLib - Win32 DebugS" && "$(CFG)" != "FSDrvLib - Win32 ReleaseS"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FSDrvLib.mak" CFG="FSDrvLib - Win32 DebugS"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FSDrvLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "FSDrvLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "FSDrvLib - Win32 DebugS" (based on "Win32 (x86) Static Library")
!MESSAGE "FSDrvLib - Win32 ReleaseS" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\..\..\..\temp\Release\fsdrv

ALL : "..\..\..\out\Release\FSDrvLib.lib"


CLEAN :
	-@erase "$(INTDIR)\FSDrv.obj"
	-@erase "$(INTDIR)\IDriver.obj"
	-@erase "$(INTDIR)\IDriverEx.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\out\Release\FSDrvLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSDrvLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\out\Release\FSDrvLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FSDrv.obj" \
	"$(INTDIR)\IDriver.obj" \
	"$(INTDIR)\IDriverEx.obj" \
	"$(INTDIR)\sa.obj"

"..\..\..\out\Release\FSDrvLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
   PreBuild Release
	 $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\..\..\..\temp\Debug\fsdrv
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\out\Debug\FSDrvLib.lib" "$(OUTDIR)\FSDrvLib.bsc"


CLEAN :
	-@erase "$(INTDIR)\FSDrv.obj"
	-@erase "$(INTDIR)\FSDrv.sbr"
	-@erase "$(INTDIR)\IDriver.obj"
	-@erase "$(INTDIR)\IDriver.sbr"
	-@erase "$(INTDIR)\IDriverEx.obj"
	-@erase "$(INTDIR)\IDriverEx.sbr"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\sa.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FSDrvLib.bsc"
	-@erase "..\..\..\out\Debug\FSDrvLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSDrvLib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\FSDrv.sbr" \
	"$(INTDIR)\IDriver.sbr" \
	"$(INTDIR)\IDriverEx.sbr" \
	"$(INTDIR)\sa.sbr"

"$(OUTDIR)\FSDrvLib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\out\Debug\FSDrvLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FSDrv.obj" \
	"$(INTDIR)\IDriver.obj" \
	"$(INTDIR)\IDriverEx.obj" \
	"$(INTDIR)\sa.obj"

"..\..\..\out\Debug\FSDrvLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"

OUTDIR=.\DebugS
INTDIR=.\..\..\..\temp\DebugS\fsdrv

ALL : "..\..\..\out\DebugS\FSDrvLib.lib"


CLEAN :
	-@erase "$(INTDIR)\FSDrv.obj"
	-@erase "$(INTDIR)\IDriver.obj"
	-@erase "$(INTDIR)\IDriverEx.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\out\DebugS\FSDrvLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSDrvLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\out\DebugS\FSDrvLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FSDrv.obj" \
	"$(INTDIR)\IDriver.obj" \
	"$(INTDIR)\IDriverEx.obj" \
	"$(INTDIR)\sa.obj"

"..\..\..\out\DebugS\FSDrvLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"

OUTDIR=.\ReleaseS
INTDIR=.\..\..\..\temp\ReleaseS\fsdrv

ALL : "..\..\..\out\ReleaseS\FSDrvLib.lib"


CLEAN :
	-@erase "$(INTDIR)\FSDrv.obj"
	-@erase "$(INTDIR)\IDriver.obj"
	-@erase "$(INTDIR)\IDriverEx.obj"
	-@erase "$(INTDIR)\sa.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\out\ReleaseS\FSDrvLib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP_PROJ=/nologo /G5 /MT /W3 /GX /O2 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FSDrvLib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\out\ReleaseS\FSDrvLib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\FSDrv.obj" \
	"$(INTDIR)\IDriver.obj" \
	"$(INTDIR)\IDriverEx.obj" \
	"$(INTDIR)\sa.obj"

"..\..\..\out\ReleaseS\FSDrvLib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("FSDrvLib.dep")
!INCLUDE "FSDrvLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "FSDrvLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FSDrvLib - Win32 Release" || "$(CFG)" == "FSDrvLib - Win32 Debug" || "$(CFG)" == "FSDrvLib - Win32 DebugS" || "$(CFG)" == "FSDrvLib - Win32 ReleaseS"
SOURCE=.\FSDrv.cpp

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"

CPP_SWITCHES=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\FSDrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"

CPP_SWITCHES=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\FSDrv.obj"	"$(INTDIR)\FSDrv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"

CPP_SWITCHES=/nologo /G5 /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\FSDrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"

CPP_SWITCHES=/nologo /G5 /MT /W3 /GX /O2 /I "..\..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\FSDrv.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\IDriver.cpp

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"


"$(INTDIR)\IDriver.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"


"$(INTDIR)\IDriver.obj"	"$(INTDIR)\IDriver.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"


"$(INTDIR)\IDriver.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"


"$(INTDIR)\IDriver.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=.\IDriverEx.cpp

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"


"$(INTDIR)\IDriverEx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"


"$(INTDIR)\IDriverEx.obj"	"$(INTDIR)\IDriverEx.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"


"$(INTDIR)\IDriverEx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"


"$(INTDIR)\IDriverEx.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\..\..\CommonFiles\Service\sa.cpp

!IF  "$(CFG)" == "FSDrvLib - Win32 Release"


"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 Debug"


"$(INTDIR)\sa.obj"	"$(INTDIR)\sa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 DebugS"


"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "FSDrvLib - Win32 ReleaseS"


"$(INTDIR)\sa.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

