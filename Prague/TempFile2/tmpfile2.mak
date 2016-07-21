# Microsoft Developer Studio Generated NMAKE File, Based on tmpfile2.dsp
!IF "$(CFG)" == ""
CFG=TempFile2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to TempFile2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "TempFile2 - Win32 ReleaseWithTrace" && "$(CFG)" != "TempFile2 - Win32 Release" && "$(CFG)" != "TempFile2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tmpfile2.mak" CFG="TempFile2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "TempFile2 - Win32 ReleaseWithTrace" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TempFile2 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "TempFile2 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "TempFile2 - Win32 ReleaseWithTrace"

OUTDIR=.\../../out/ReleaseWithTrace
INTDIR=.\../../temp/ReleaseWithTrace/prague/TempFile2
# Begin Custom Macros
OutDir=.\../../out/ReleaseWithTrace
# End Custom Macros

ALL : "$(OUTDIR)\tmpfile2.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_temporaryio2.obj"
	-@erase "$(INTDIR)\posio.obj"
	-@erase "$(INTDIR)\tmpfile2_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\tmpfile2.exp"
	-@erase "$(OUTDIR)\tmpfile2.pdb"
	-@erase "$(OUTDIR)\tmpfile2.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fp"$(INTDIR)\tmpfile2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\tmpfile2_res.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tmpfile2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /entry:"DllMain" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\tmpfile2.pdb" /debug /machine:I386 /nodefaultlib /out:"$(OUTDIR)\tmpfile2.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_temporaryio2.obj" \
	"$(INTDIR)\posio.obj" \
	"$(INTDIR)\tmpfile2_res.res"

"$(OUTDIR)\tmpfile2.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetPath=\out\ReleaseWithTrace\tmpfile2.ppl
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/ReleaseWithTrace
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\tmpfile2.ppl"
   call prconvert "\out\ReleaseWithTrace\tmpfile2.ppl"
	tsigner "\out\ReleaseWithTrace\tmpfile2.ppl"
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "TempFile2 - Win32 Release"

OUTDIR=.\../../out/Release
INTDIR=.\../../temp/Release/prague/TempFile2
# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

ALL : "$(OUTDIR)\tmpfile2.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_temporaryio2.obj"
	-@erase "$(INTDIR)\posio.obj"
	-@erase "$(INTDIR)\tmpfile2_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\tmpfile2.exp"
	-@erase "$(OUTDIR)\tmpfile2.pdb"
	-@erase "$(OUTDIR)\tmpfile2.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MD /W3 /GX /Zi /O2 /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)\tmpfile2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\tmpfile2_res.res" /i "..\..\CommonFiles" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tmpfile2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /base:"0x5DC00000" /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\tmpfile2.pdb" /debug /machine:I386 /out:"$(OUTDIR)\tmpfile2.ppl" /implib:"$(OUTDIR)\tmpfile2.lib" /pdbtype:sept /ALIGN:4096 /IGNORE:4108 
LINK32_OBJS= \
	"$(INTDIR)\plugin_temporaryio2.obj" \
	"$(INTDIR)\posio.obj" \
	"$(INTDIR)\tmpfile2_res.res"

"$(OUTDIR)\tmpfile2.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

TargetDir=\out\Release
TargetPath=\out\Release\tmpfile2.ppl
TargetName=tmpfile2
SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

# Begin Custom Macros
OutDir=.\../../out/Release
# End Custom Macros

$(DS_POSTBUILD_DEP) : "$(OUTDIR)\tmpfile2.ppl"
   tsigner "\out\Release\tmpfile2.ppl"
	addsym \out\Release\tmpfile2.pdb tmpfile2
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "TempFile2 - Win32 Debug"

OUTDIR=.\../../out/Debug
INTDIR=.\../../temp/Debug/prague/TempFile2
# Begin Custom Macros
OutDir=.\../../out/Debug
# End Custom Macros

ALL : "$(OUTDIR)\tmpfile2.ppl"


CLEAN :
	-@erase "$(INTDIR)\plugin_temporaryio2.obj"
	-@erase "$(INTDIR)\posio.obj"
	-@erase "$(INTDIR)\tmpfile2_res.res"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\tmpfile2.exp"
	-@erase "$(OUTDIR)\tmpfile2.ilk"
	-@erase "$(OUTDIR)\tmpfile2.pdb"
	-@erase "$(OUTDIR)\tmpfile2.ppl"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /G5 /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\CommonFiles" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_PRAGUE_TRACE_" /Fp"$(INTDIR)\tmpfile2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x417 /fo"$(INTDIR)\tmpfile2_res.res" /i "..\..\CommonFiles" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\tmpfile2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\tmpfile2.pdb" /debug /machine:I386 /out:"$(OUTDIR)\tmpfile2.ppl" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\plugin_temporaryio2.obj" \
	"$(INTDIR)\posio.obj" \
	"$(INTDIR)\tmpfile2_res.res"

"$(OUTDIR)\tmpfile2.ppl" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("tmpfile2.dep")
!INCLUDE "tmpfile2.dep"
!ELSE 
!MESSAGE Warning: cannot find "tmpfile2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "TempFile2 - Win32 ReleaseWithTrace" || "$(CFG)" == "TempFile2 - Win32 Release" || "$(CFG)" == "TempFile2 - Win32 Debug"
SOURCE=.\cache.c
SOURCE=.\plugin_temporaryio2.c

"$(INTDIR)\plugin_temporaryio2.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\posio.c

"$(INTDIR)\posio.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=".\tmpfile2_res.rc"

"$(INTDIR)\tmpfile2_res.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

