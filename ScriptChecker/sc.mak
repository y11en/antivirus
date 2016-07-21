# Microsoft Developer Studio Generated NMAKE File, Based on sc.dsp
!IF "$(CFG)" == ""
CFG=Script Checker - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Script Checker - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Script Checker - Win32 Release" && "$(CFG)" != "Script Checker - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "sc.mak" CFG="Script Checker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Script Checker - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Script Checker - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "Script Checker - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/ppp/sc
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\sc.ppl"

!ELSE 

ALL : "SC AntiVirus Plugin - Win32 Release" "Script Checker Interceptor - Win32 Release" "$(OUTDIR)\sc.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Script Checker Interceptor - Win32 ReleaseCLEAN" "SC AntiVirus Plugin - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\plugin_sc.obj"
	-@erase "$(INTDIR)\sc_res.res"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sc.exp"
	-@erase "$(OUTDIR)\sc.pdb"
	-@erase "$(OUTDIR)\sc.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /G5 /MD /W3 /Zi /O1 /I "..\include" /I "..\..\prague\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\sc_res.res" /i "..\include" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=pr_remote.lib kernel32.lib /nologo /base:"0x67400000" /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\sc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\sc.ppl" /implib:"$(OUTDIR)\sc.lib" /pdbtype:sept /libpath:"..\..\out\release" /opt:ref 
LINK32_OBJS= \
	"$(INTDIR)\plugin_sc.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\sc_res.res"

"$(OUTDIR)\sc.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\Release\sc.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "SC AntiVirus Plugin - Win32 Release" "Script Checker Interceptor - Win32 Release" "$(OUTDIR)\sc.ppl"
   call prconvert "\out\Release\sc.ppl"
	tsigner "\out\Release\sc.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "Script Checker - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/ppp/sc
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\sc.ppl"

!ELSE 

ALL : "SC AntiVirus Plugin - Win32 Debug" "Script Checker Interceptor - Win32 Debug" "$(OUTDIR)\sc.ppl"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"Script Checker Interceptor - Win32 DebugCLEAN" "SC AntiVirus Plugin - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\plugin_sc.obj"
	-@erase "$(INTDIR)\sc_res.res"
	-@erase "$(INTDIR)\task.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\sc.exp"
	-@erase "$(OUTDIR)\sc.ilk"
	-@erase "$(OUTDIR)\sc.pdb"
	-@erase "$(OUTDIR)\sc.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

LIB32=link.exe -lib
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\prague\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\sc_res.res" /i "..\include" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\sc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=pr_remote.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\sc.pdb" /debug /machine:I386 /out:"$(OUTDIR)\sc.ppl" /pdbtype:sept /libpath:"..\..\out\debug" 
LINK32_OBJS= \
	"$(INTDIR)\plugin_sc.obj" \
	"$(INTDIR)\task.obj" \
	"$(INTDIR)\sc_res.res"

"$(OUTDIR)\sc.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("sc.dep")
!INCLUDE "sc.dep"
!ELSE 
!MESSAGE Warning: cannot find "sc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Script Checker - Win32 Release" || "$(CFG)" == "Script Checker - Win32 Debug"
SOURCE=.\plugin_sc.cpp

"$(INTDIR)\plugin_sc.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\task.cpp

"$(INTDIR)\task.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\sc_res.rc"

"$(INTDIR)\sc_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


!IF  "$(CFG)" == "Script Checker - Win32 Release"

"Script Checker Interceptor - Win32 Release" : 
   cd ".\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Release" 
   cd ".."

"Script Checker Interceptor - Win32 ReleaseCLEAN" : 
   cd ".\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "Script Checker - Win32 Debug"

"Script Checker Interceptor - Win32 Debug" : 
   cd ".\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Debug" 
   cd ".."

"Script Checker Interceptor - Win32 DebugCLEAN" : 
   cd ".\SC_Interceptor2"
   $(MAKE) /$(MAKEFLAGS) /F .\scr_ch_pg.mak CFG="Script Checker Interceptor - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 

!IF  "$(CFG)" == "Script Checker - Win32 Release"

"SC AntiVirus Plugin - Win32 Release" : 
   cd ".\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Release" 
   cd ".."

"SC AntiVirus Plugin - Win32 ReleaseCLEAN" : 
   cd ".\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Release" RECURSE=1 CLEAN 
   cd ".."

!ELSEIF  "$(CFG)" == "Script Checker - Win32 Debug"

"SC AntiVirus Plugin - Win32 Debug" : 
   cd ".\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Debug" 
   cd ".."

"SC AntiVirus Plugin - Win32 DebugCLEAN" : 
   cd ".\klscav"
   $(MAKE) /$(MAKEFLAGS) /F .\klscav.mak CFG="SC AntiVirus Plugin - Win32 Debug" RECURSE=1 CLEAN 
   cd ".."

!ENDIF 


!ENDIF 

