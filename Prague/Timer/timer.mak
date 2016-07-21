# Microsoft Developer Studio Generated NMAKE File, Based on timer.dsp
!IF "$(CFG)" == ""
CFG=Timer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Timer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Timer - Win32 Release" && "$(CFG)" != "Timer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "timer.mak" CFG="Timer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Timer - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Timer - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Timer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\../../temp/Release/prague/timer

ALL : "..\..\out\Release\timer.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_timer.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\timer_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\timer.exp"
	-@erase "..\..\out\Release\timer.pdb"
	-@erase "..\..\out\Release\timer.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O1 /I "..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\timer_res.res" /i "..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\timer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib /nologo /base:"0x68100000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"..\..\out/Release/timer.pdb" /debug /machine:I386 /out:"..\..\out/Release/timer.ppl" /implib:"$(OUTDIR)\timer.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_timer.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\timer_res.res"

"..\..\out\Release\timer.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\timer.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\out\Release\timer.ppl"
   call prconvert "\out\Release\timer.ppl"
	tsigner "\out\Release\timer.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Timer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\../../temp/Debug/prague/timer

ALL : "..\..\out\Debug\timer.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_timer.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\timer_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\out\Debug\timer.exp"
	-@erase "..\..\out\Debug\timer.ilk"
	-@erase "..\..\out\Debug\timer.pdb"
	-@erase "..\..\out\Debug\timer.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\timer_res.res" /i "..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\timer.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"..\..\out/Debug/timer.pdb" /debug /machine:I386 /out:"..\..\out/Debug/timer.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_timer.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\timer_res.res"

"..\..\out\Debug\timer.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("timer.dep")
!INCLUDE "timer.dep"
!ELSE 
!MESSAGE Warning: cannot find "timer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Timer - Win32 Release" || "$(CFG)" == "Timer - Win32 Debug"
SOURCE=.\plugin_timer.cpp

"$(INTDIR)\plugin_timer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\timer.cpp

"$(INTDIR)\timer.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\timer_res.rc"

"$(INTDIR)\timer_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

