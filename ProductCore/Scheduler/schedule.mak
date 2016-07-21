# Microsoft Developer Studio Generated NMAKE File, Based on schedule.dsp
!IF "$(CFG)" == ""
CFG=Scheduler - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Scheduler - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Scheduler - Win32 Release" && "$(CFG)" != "Scheduler - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "schedule.mak" CFG="Scheduler - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Scheduler - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Scheduler - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Scheduler - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/scheduler
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\schedule.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_scheduler.obj"
	-@erase "$(INTDIR)\schedule_res.res"
	-@erase "$(INTDIR)\scheduler.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\schedule.exp"
	-@erase "$(OUTDIR)\schedule.pdb"
	-@erase "$(OUTDIR)\schedule.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "../include" /I "../../prague/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\schedule_res.res" /i "..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\schedule.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x67100000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\schedule.pdb" /debug /machine:I386 /out:"$(OUTDIR)\schedule.ppl" /implib:"$(OUTDIR)\schedule.lib" /pdbtype:sept /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_scheduler.obj" \
	"$(INTDIR)\scheduler.obj" \
	"$(INTDIR)\schedule_res.res"

"$(OUTDIR)\schedule.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\schedule.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\schedule.ppl"
   call prconvert "\out\Release\schedule.ppl"
	tsigner "\out\Release\schedule.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Scheduler - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/scheduler
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\schedule.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_scheduler.obj"
	-@erase "$(INTDIR)\schedule_res.res"
	-@erase "$(INTDIR)\scheduler.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\schedule.exp"
	-@erase "$(OUTDIR)\schedule.ilk"
	-@erase "$(OUTDIR)\schedule.pdb"
	-@erase "$(OUTDIR)\schedule.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../prague/include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\schedule_res.res" /i "..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\schedule.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\schedule.pdb" /debug /machine:I386 /out:"$(OUTDIR)\schedule.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_scheduler.obj" \
	"$(INTDIR)\scheduler.obj" \
	"$(INTDIR)\schedule_res.res"

"$(OUTDIR)\schedule.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("schedule.dep")
!INCLUDE "schedule.dep"
!ELSE 
!MESSAGE Warning: cannot find "schedule.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Scheduler - Win32 Release" || "$(CFG)" == "Scheduler - Win32 Debug"
SOURCE=.\plugin_scheduler.cpp

"$(INTDIR)\plugin_scheduler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\scheduler.cpp

"$(INTDIR)\scheduler.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\schedule_res.rc"

"$(INTDIR)\schedule_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

