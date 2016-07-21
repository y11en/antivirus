# Microsoft Developer Studio Generated NMAKE File, Based on report.dsp
!IF "$(CFG)" == ""
CFG=Report - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Report - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Report - Win32 ReleaseWithTrace" && "$(CFG)" != "Report - Win32 Release" && "$(CFG)" != "Report - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "report.mak" CFG="Report - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Report - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Report - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Report - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Report - Win32 ReleaseWithTrace"

OUTDIR=.\ReleaseWithTrace
INTDIR=.\../../temp/ReleaseWithTrace

ALL : "..\..\Prague\ReleaseWithTrace\report.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_report.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\report_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\report.exp"
	-@erase "..\..\Prague\ReleaseWithTrace\report.pdb"
	-@erase "..\..\Prague\ReleaseWithTrace\report.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /GX /O2 /I "../include" /I "../../Prague/Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\report_res.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\report.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"../../Prague/ReleaseWithTrace/report.pdb" /debug /machine:I386 /nodefaultlib /out:"../../Prague/ReleaseWithTrace/report.ppl" /implib:"$(OUTDIR)\report.lib" /pdbtype:sept /OPT:REF 
LINK32_OBJS= \
	"$(INTDIR)\plugin_report.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\report_res.res"

"..\..\Prague\ReleaseWithTrace\report.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\Prague\ReleaseWithTrace\report.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\..\Prague\ReleaseWithTrace\report.ppl"
   call prconvert "\Prague\ReleaseWithTrace\report.ppl"
	tsigner "\Prague\ReleaseWithTrace\report.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Report - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/report
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\report.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_report.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\report_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\report.exp"
	-@erase "$(OUTDIR)\report.pdb"
	-@erase "$(OUTDIR)\report.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "../include" /I "../../Prague/Include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\report_res.res" /i "..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\report.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=libcmt.lib /nologo /base:"0x66e00000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\report.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\report.ppl" /implib:"$(OUTDIR)\report.lib" /pdbtype:sept /OPT:REF 
LINK32_OBJS= \
	"$(INTDIR)\plugin_report.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\report_res.res"

"$(OUTDIR)\report.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\report.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\report.ppl"
   call prconvert "\out\Release\report.ppl"
	tsigner "\out\Release\report.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Report - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/report
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\report.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_report.obj"
	-@erase "$(INTDIR)\report.obj"
	-@erase "$(INTDIR)\report_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\report.exp"
	-@erase "$(OUTDIR)\report.ilk"
	-@erase "$(OUTDIR)\report.pdb"
	-@erase "$(OUTDIR)\report.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "../include" /I "../../Prague/Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\report_res.res" /i "..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\report.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\report.pdb" /debug /machine:I386 /out:"$(OUTDIR)\report.ppl" /implib:"$(OUTDIR)\report.lib" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_report.obj" \
	"$(INTDIR)\report.obj" \
	"$(INTDIR)\report_res.res"

"$(OUTDIR)\report.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("report.dep")
!INCLUDE "report.dep"
!ELSE 
!MESSAGE Warning: cannot find "report.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Report - Win32 ReleaseWithTrace" || "$(CFG)" == "Report - Win32 Release" || "$(CFG)" == "Report - Win32 Debug"
SOURCE=.\plugin_report.cpp

"$(INTDIR)\plugin_report.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\report.cpp

"$(INTDIR)\report.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\report_res.rc"

"$(INTDIR)\report_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

